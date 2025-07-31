// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "audio_dma.h"

#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/audiocore/WaveFile.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "bindings/rp2pio/StateMachine.h"
#include "supervisor/background_callback.h"

#include "py/mpstate.h"
#include "py/runtime.h"

#include "hardware/irq.h"
#include "hardware/regs/intctrl.h" // For isr_ macro.


#if CIRCUITPY_AUDIOCORE

void audio_dma_reset(void) {
    for (size_t channel = 0; channel < NUM_DMA_CHANNELS; channel++) {
        if (MP_STATE_PORT(playing_audio)[channel] != NULL) {
            audio_dma_stop_output(MP_STATE_PORT(playing_audio)[channel]);
        } else if (MP_STATE_PORT(recording_audio)[channel] != NULL) {
            audio_dma_stop_input(MP_STATE_PORT(recording_audio)[channel]);
        }
    }
}


static size_t audio_dma_convert_samples(audio_dma_t *dma, uint8_t *input, uint32_t input_length, uint8_t *output, uint32_t output_length) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"

    uint32_t output_length_used = input_length / dma->sample_spacing;

    if (output_length_used > output_length) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Internal audio buffer too small"));
    }

    uint32_t out_i = 0;
    if (dma->sample_resolution <= 8 && dma->output_resolution > 8) {
        // reading bytes, writing 16-bit words, so output buffer will be bigger.

        output_length_used *= 2;
        if (output_length_used > output_length) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Internal audio buffer too small"));
        }

        // Correct "rail-to-rail" scaling of arbitrary-depth input to output
        // requires more operations than this, but at least the vital 8- to
        // 16-bit cases are correctly scaled now. Prior code was only
        // considering 8-to-16 anyway, but had a slight DC offset in the
        // result, so this is no worse off WRT supported resolutions.
        uint16_t mul = ((1 << dma->output_resolution) - 1) / ((1 << dma->sample_resolution) - 1);
        uint16_t offset = (1 << dma->output_resolution) / 2;

        for (uint32_t i = 0; i < input_length; i += dma->sample_spacing) {
            if (dma->signed_to_unsigned) {
                ((uint16_t *)output)[out_i] = (uint16_t)((((int8_t *)input)[i] + 0x80) * mul);
            } else if (dma->unsigned_to_signed) {
                ((int16_t *)output)[out_i] = (int16_t)(((uint8_t *)input)[i] * mul - offset);
            } else {
                ((uint16_t *)output)[out_i] = (uint16_t)(((uint8_t *)input)[i] * mul);
            }
            out_i += 1;
        }
    } else if (dma->sample_resolution <= 8 && dma->output_resolution <= 8) {
        for (uint32_t i = 0; i < input_length; i += dma->sample_spacing) {
            if (dma->signed_to_unsigned) {
                ((uint8_t *)output)[out_i] = ((int8_t *)input)[i] + 0x80;
            } else if (dma->unsigned_to_signed) {
                ((int8_t *)output)[out_i] = ((uint8_t *)input)[i] - 0x80;
            } else {
                ((uint8_t *)output)[out_i] = ((uint8_t *)input)[i];
            }
            out_i += 1;
        }
    } else if (dma->sample_resolution > 8 && dma->output_resolution > 8) {
        size_t shift = 16 - dma->output_resolution;
        for (uint32_t i = 0; i < input_length / 2; i += dma->sample_spacing) {
            if (dma->signed_to_unsigned) {
                ((uint16_t *)output)[out_i] = ((int16_t *)input)[i] + 0x8000;
            } else if (dma->unsigned_to_signed) {
                ((int16_t *)output)[out_i] = ((uint16_t *)input)[i] - 0x8000;
            } else {
                ((uint16_t *)output)[out_i] = ((uint16_t *)input)[i];
            }
            if (dma->output_resolution < 16) {
                if (dma->output_signed) {
                    ((int16_t *)output)[out_i] = ((int16_t *)output)[out_i] >> shift;
                } else {
                    ((uint16_t *)output)[out_i] = ((uint16_t *)output)[out_i] >> shift;
                }
            }
            out_i += 1;
        }
    } else {
        // (dma->sample_resolution > 8 && dma->output_resolution <= 8)
        // Not currently used, but might be in the future.
        mp_raise_RuntimeError(MP_ERROR_TEXT("Audio conversion not implemented"));
    }
    if (dma->swap_channel) {
        // Loop for swapping left and right channels
        for (uint32_t i = 0; i < out_i; i += 2) {
            uint16_t temp = ((uint16_t *)output)[i];
            ((uint16_t *)output)[i] = ((uint16_t *)output)[i + 1];
            ((uint16_t *)output)[i + 1] = temp;
        }
    }
    #pragma GCC diagnostic pop
    return output_length_used;
}

// buffer_idx is 0 or 1.
static void audio_dma_load_next_block(audio_dma_t *dma, size_t buffer_idx) {
    if (!dma->output_register_address) {
        return;
    }

    assert(dma->channel[buffer_idx] < NUM_DMA_CHANNELS);
    size_t dma_channel = dma->output_channel[buffer_idx];

    audioio_get_buffer_result_t get_buffer_result;
    uint8_t *sample_buffer;
    uint32_t sample_buffer_length;
    get_buffer_result = audiosample_get_buffer(dma->sample,
        dma->single_channel_output, dma->audio_channel, &sample_buffer, &sample_buffer_length);

    if (get_buffer_result == GET_BUFFER_ERROR) {
        audio_dma_stop_output(dma);
        dma->dma_result = AUDIO_DMA_SOURCE_ERROR;
        return;
    }

    // Convert the sample format resolution and signedness, as necessary.
    // The input sample buffer is what was read from a file, Mixer, or a raw sample buffer.
    // The output buffer is one of the DMA buffers (passed in).

    size_t output_length_used = audio_dma_convert_samples(
        dma, sample_buffer, sample_buffer_length,
        dma->output_buffer[buffer_idx], dma->output_buffer_length[buffer_idx]);

    dma_channel_set_read_addr(dma_channel, dma->output_buffer[buffer_idx], false /* trigger */);
    dma_channel_set_trans_count(dma_channel, output_length_used / dma->output_size, false /* trigger */);

    if (get_buffer_result == GET_BUFFER_DONE) {
        if (dma->loop) {
            audiosample_reset_buffer(dma->sample, dma->single_channel_output, dma->audio_channel);
        } else {
            // Set channel trigger to ourselves so we don't keep going.
            dma_channel_hw_t *c = &dma_hw->ch[dma_channel];
            c->al1_ctrl =
                (c->al1_ctrl & ~DMA_CH0_CTRL_TRIG_CHAIN_TO_BITS) |
                (dma_channel << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB);

            if (output_length_used == 0 &&
                !dma_channel_is_busy(dma->output_channel[0]) &&
                !dma_channel_is_busy(dma->output_channel[1])) {
                // No data has been read, and both DMA channels have now finished, so it's safe to stop.
                audio_dma_stop_output(dma);
                dma->dma_result = AUDIO_DMA_OK;
                return;
            }
        }
    }
    // Enable the channel so that it can be played.
    if (!dma->paused) {
        dma_hw->ch[dma_channel].al1_ctrl |= DMA_CH1_CTRL_TRIG_EN_BITS;
    }
    dma->dma_result = AUDIO_DMA_OK;
}

// Playback and recording should be shutdown before calling this.
audio_dma_result audio_dma_setup(
    audio_dma_t *dma,
    mp_obj_t sample,
    bool loop,
    bool single_channel_output,
    uint8_t audio_channel,
    bool output_signed,
    uint8_t output_resolution,
    uint32_t output_register_address,
    uint8_t output_dma_trigger_source,
    uint32_t input_register_address,
    uint8_t input_dma_trigger_source,
    bool swap_channel) {

    int output_dma_channel_0_maybe = -1;
    int output_dma_channel_1_maybe = -1;

    if (output_register_address) {
        // Use two DMA channels to play because the DMA can't wrap to itself without the
        // buffer being power of two aligned.
        output_dma_channel_0_maybe = dma_claim_unused_channel(false);
        if (output_dma_channel_0_maybe < 0) {
            return AUDIO_DMA_DMA_BUSY;
        }

        output_dma_channel_1_maybe = dma_claim_unused_channel(false);
        if (output_dma_channel_1_maybe < 0) {
            dma_channel_unclaim((uint)output_dma_channel_0_maybe);
            return AUDIO_DMA_DMA_BUSY;
        }

        dma->output_channel[0] = (uint8_t)output_dma_channel_0_maybe;
        dma->output_channel[1] = (uint8_t)output_dma_channel_1_maybe;
    }

    if (input_register_address) {
        // Use two DMA channels to record because the DMA can't wrap to itself without the
        // buffer being power of two aligned.
        int input_dma_channel_0_maybe = dma_claim_unused_channel(false);
        if (input_dma_channel_0_maybe < 0) {
            if (output_register_address) {
                dma_channel_unclaim((uint)output_dma_channel_0_maybe);
                dma_channel_unclaim((uint)output_dma_channel_1_maybe);
            }
            return AUDIO_DMA_DMA_BUSY;
        }

        int input_dma_channel_1_maybe = dma_claim_unused_channel(false);
        if (input_dma_channel_1_maybe < 0) {
            if (output_register_address) {
                dma_channel_unclaim((uint)output_dma_channel_0_maybe);
                dma_channel_unclaim((uint)output_dma_channel_1_maybe);
            }
            dma_channel_unclaim((uint)input_dma_channel_0_maybe);
            return AUDIO_DMA_DMA_BUSY;
        }

        dma->input_channel[0] = (uint8_t)input_dma_channel_0_maybe;
        dma->input_channel[1] = (uint8_t)input_dma_channel_1_maybe;
    }

    dma->sample = sample;
    dma->loop = loop;
    dma->single_channel_output = single_channel_output;
    dma->audio_channel = audio_channel;
    dma->signed_to_unsigned = false;
    dma->unsigned_to_signed = false;
    dma->output_signed = output_signed;
    dma->sample_spacing = 1;
    dma->output_resolution = output_resolution;
    dma->sample_resolution = audiosample_get_bits_per_sample(sample);
    dma->output_register_address = output_register_address;
    dma->input_register_address = input_register_address;
    dma->swap_channel = swap_channel;

    if (output_register_address) {
        audiosample_reset_buffer(sample, single_channel_output, audio_channel);
    }


    bool single_buffer;  // True if data fits in one single buffer.

    bool samples_signed;
    uint32_t max_buffer_length;
    audiosample_get_buffer_structure(sample, single_channel_output, &single_buffer, &samples_signed,
        &max_buffer_length, &dma->sample_spacing);

    // Check to see if we have to scale the resolution up.
    if (dma->sample_resolution <= 8 && dma->output_resolution > 8) {
        max_buffer_length *= 2;
    }
    if (output_signed != samples_signed ||
        dma->sample_spacing > 1 ||
        (dma->sample_resolution != dma->output_resolution)) {
        max_buffer_length /= dma->sample_spacing;
    }

    if (output_register_address) {
        #ifdef PICO_RP2350
        dma->output_buffer[0] = (uint8_t *)port_realloc(dma->output_buffer[0], max_buffer_length, true);
        #else
        dma->output_buffer[0] = (uint8_t *)m_realloc(dma->output_buffer[0],
            #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
            dma->output_buffer_length[0], // Old size
            #endif
            max_buffer_length);
        #endif
        dma->output_buffer_length[0] = max_buffer_length;

        if (dma->output_buffer[0] == NULL) {
            return AUDIO_DMA_MEMORY_ERROR;
        }

        if (!single_buffer) {
            #ifdef PICO_RP2350
            dma->output_buffer[1] = (uint8_t *)port_realloc(dma->output_buffer[1], max_buffer_length, true);
            #else
            dma->output_buffer[1] = (uint8_t *)m_realloc(dma->output_buffer[1],
                #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
                dma->output_buffer_length[1], // Old size
                #endif
                max_buffer_length);
            #endif
            dma->output_buffer_length[1] = max_buffer_length;

            if (dma->output_buffer[1] == NULL) {
                return AUDIO_DMA_MEMORY_ERROR;
            }
        }
    }

    if (input_register_address) {
        #ifdef PICO_RP2350
        dma->input_buffer[0] = (uint8_t *)port_realloc(dma->input_buffer[0], max_buffer_length, true);
        #else
        dma->input_buffer[0] = (uint8_t *)m_realloc(dma->input_buffer[0],
            #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
            dma->input_buffer_length[0], // Old size
            #endif
            max_buffer_length);                                       
        #endif
        dma->input_buffer_length[0] = max_buffer_length;
        
        if (dma->input_buffer[0] == NULL) {
            return AUDIO_DMA_MEMORY_ERROR;
        }

        if (!single_buffer) {
            #ifdef PICO_RP2350
            dma->input_buffer[1] = (uint8_t *)port_realloc(dma->input_buffer[1], max_buffer_length, true);
            #else
            dma->input_buffer[1] = (uint8_t *)m_realloc(dma->input_buffer[1],
                #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
                dma->input_buffer_length[1], // Old size
                #endif
                max_buffer_length);                                       
            #endif
            dma->input_buffer_length[1] = max_buffer_length;
            
            if (dma->input_buffer[1] == NULL) {
                return AUDIO_DMA_MEMORY_ERROR;
            }
        }
    }

    dma->signed_to_unsigned = !output_signed && samples_signed;
    dma->unsigned_to_signed = output_signed && !samples_signed;

    if (output_resolution > 8) {
        dma->output_size = 2;
    } else {
        dma->output_size = 1;
    }
    // Transfer both channels at once.
    if (!single_channel_output && audiosample_get_channel_count(sample) == 2) {
        dma->output_size *= 2;
    }
    enum dma_channel_transfer_size dma_size = DMA_SIZE_8;
    if (dma->output_size == 2) {
        dma_size = DMA_SIZE_16;
    } else if (dma->output_size == 4) {
        dma_size = DMA_SIZE_32;
    }

    for (size_t i = 0; i < 2; i++) {
        dma_channel_config c;
        if (output_register_address) {
            c = dma_channel_get_default_config(dma->output_channel[i]);
            channel_config_set_transfer_data_size(&c, dma_size);
            channel_config_set_dreq(&c, output_dma_trigger_source);
            channel_config_set_read_increment(&c, true);
            channel_config_set_write_increment(&c, false);

            // Chain to the other channel by default.
            channel_config_set_chain_to(&c, dma->output_channel[(i + 1) % 2]);
            dma_channel_set_config(dma->output_channel[i], &c, false /* trigger */);

            dma_channel_set_write_addr(dma->output_channel[i], (void *)output_register_address, false /* trigger */);
        }
        if (input_register_address) {
            c = dma_channel_get_default_config(dma->input_channel[i]);
            channel_config_set_transfer_data_size(&c, dma_size);
            channel_config_set_dreq(&c, input_dma_trigger_source);
            channel_config_set_read_increment(&c, false);
            channel_config_set_write_increment(&c, true);

            // Chain to the other channel by default.
            channel_config_set_chain_to(&c, dma->input_channel[(i + 1) % 2]);
            dma_channel_set_config(dma->input_channel[i], &c, false /* trigger */);

            dma_channel_set_read_addr(dma->input_channel[i], (void *)input_register_address, false /* trigger */);
            dma_channel_set_write_addr(dma->input_channel[i], dma->input_buffer[i], false /* trigger */);
            dma_channel_set_trans_count(dma->input_channel[i], dma->input_buffer_length[i] / dma->output_size, false /* trigger */);
        }

    }

    // Start input before output to allow the first two blocks to load.
    if (input_register_address) {
        // We keep the audio_dma_t for internal use and the sample as a root pointer because it
        // contains the audiodma structure.
        MP_STATE_PORT(recording_audio)[dma->input_channel[0]] = dma;
        MP_STATE_PORT(recording_audio)[dma->input_channel[1]] = dma;
      
        // Special case the DMA for a single buffer.
        if (single_buffer) {
            dma_channel_config c = dma_channel_get_default_config(dma->input_channel[1]);
            channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
            channel_config_set_dreq(&c, 0x3f); // dma as fast as possible
            channel_config_set_read_increment(&c, false);
            channel_config_set_write_increment(&c, false);
            channel_config_set_chain_to(&c, dma->input_channel[1]); // Chain to ourselves so we stop.
            dma_channel_configure(dma->input_channel[1], &c,
                &dma->input_buffer[0], // write address
                &dma_hw->ch[dma->input_channel[0]].al2_write_addr_trig, // read address
                1, // transaction count
                false); // trigger
        } else {
            // Clear any latent interrupts so that we don't immediately disable channels.
            dma_hw->ints0 |= (1 << dma->input_channel[0]) | (1 << dma->input_channel[1]);
            // Enable our DMA channels on DMA_IRQ_0 to the CPU.
            dma_hw->inte0 |= (1 << dma->input_channel[0]) | (1 << dma->input_channel[1]);
            irq_set_mask_enabled(1 << DMA_IRQ_0, true);
        }

        dma->input_index = -1;
        dma->recording_in_progress = true;
        dma_channel_start(dma->input_channel[0]);
    }

    if (output_register_address) {
        // We keep the audio_dma_t for internal use and the sample as a root pointer because it
        // contains the audiodma structure.
        MP_STATE_PORT(playing_audio)[dma->output_channel[0]] = dma;
        MP_STATE_PORT(playing_audio)[dma->output_channel[1]] = dma;
      
        dma->paused = false;
        
        // Load the first two blocks up front.
        audio_dma_load_next_block(dma, 0);
        if (dma->dma_result != AUDIO_DMA_OK) {
            return dma->dma_result;
        }
        if (!single_buffer) {
            audio_dma_load_next_block(dma, 1);
            if (dma->dma_result != AUDIO_DMA_OK) {
                return dma->dma_result;
            }
        }

        // Special case the DMA for a single buffer. It's commonly used for a single wave length of sound
        // and may be short. Therefore, we use DMA chaining to loop quickly without involving interrupts.
        // On the RP2040 we chain by having a second DMA writing to the config registers of the first.
        // Read and write addresses change with DMA so we need to reset the read address back to the
        // start of the sample.
        if (single_buffer) {
            dma_channel_config c = dma_channel_get_default_config(dma->output_channel[1]);
            channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
            channel_config_set_dreq(&c, 0x3f); // dma as fast as possible
            channel_config_set_read_increment(&c, false);
            channel_config_set_write_increment(&c, false);
            channel_config_set_chain_to(&c, dma->output_channel[1]); // Chain to ourselves so we stop.
            dma_channel_configure(dma->output_channel[1], &c,
                &dma_hw->ch[dma->output_channel[0]].al3_read_addr_trig, // write address
                &dma->output_buffer[0], // read address
                1, // transaction count
                false); // trigger
        } else {
            // Clear any latent interrupts so that we don't immediately disable channels.
            dma_hw->ints0 |= (1 << dma->output_channel[0]) | (1 << dma->output_channel[1]);
            // Enable our DMA channels on DMA_IRQ_0 to the CPU. This will wake us up when
            // we're WFI.
            dma_hw->inte0 |= (1 << dma->output_channel[0]) | (1 << dma->output_channel[1]);
            irq_set_mask_enabled(1 << DMA_IRQ_0, true);
        }

        dma->playing_in_progress = true;
        dma_channel_start(dma->output_channel[0]);
    }

    return AUDIO_DMA_OK;
}

// Playback should be shutdown before calling this.
audio_dma_result audio_dma_setup_playback(
    audio_dma_t *dma,
    mp_obj_t sample,
    bool loop,
    bool single_channel_output,
    uint8_t audio_channel,
    bool output_signed,
    uint8_t output_resolution,
    uint32_t output_register_address,
    uint8_t dma_trigger_source,
    bool swap_channel) {
    return audio_dma_setup(dma, sample,
        loop, single_channel_output, audio_channel,
        output_signed, output_resolution,
        output_register_address, dma_trigger_source,
        0, 0,
        swap_channel
        );
}

// Recording should be shutdown before calling this.
audio_dma_result audio_dma_setup_record(
    audio_dma_t *dma,
    mp_obj_t sample,
    bool loop,
    bool single_channel_output,
    uint8_t audio_channel,
    bool output_signed,
    uint8_t output_resolution,
    uint32_t input_register_address,
    uint8_t dma_trigger_source,
    bool swap_channel) {
    return audio_dma_setup(dma, sample,
        loop, single_channel_output, audio_channel,
        output_signed, output_resolution,
        0, 0,
        input_register_address, dma_trigger_source,
        swap_channel
        );
}

void audio_dma_stop_output(audio_dma_t *dma) {
    dma->paused = true;

    // Disable our interrupts.
    uint32_t channel_mask = 0;
    if (dma->output_channel[0] < NUM_DMA_CHANNELS) {
        channel_mask |= 1 << dma->output_channel[0];
    }
    if (dma->output_channel[1] < NUM_DMA_CHANNELS) {
        channel_mask |= 1 << dma->output_channel[1];
    }
    dma_hw->inte0 &= ~channel_mask;
    if (!dma_hw->inte0) {
        irq_set_mask_enabled(1 << DMA_IRQ_0, false);
    }

    // Run any remaining audio tasks because we remove ourselves from
    // playing_audio.
    RUN_BACKGROUND_TASKS;

    for (size_t i = 0; i < 2; i++) {
        size_t channel = dma->output_channel[i];
        dma->output_channel[i] = NUM_DMA_CHANNELS;
        if (channel == NUM_DMA_CHANNELS) {
            // Channel not in use.
            continue;
        }

        dma_channel_config c = dma_channel_get_default_config(channel);
        channel_config_set_enable(&c, false);
        dma_channel_set_config(channel, &c, false /* trigger */);

        if (dma_channel_is_busy(channel)) {
            dma_channel_abort(channel);
        }

        dma_channel_set_read_addr(channel, NULL, false /* trigger */);
        dma_channel_set_write_addr(channel, NULL, false /* trigger */);
        dma_channel_set_trans_count(channel, 0, false /* trigger */);
        dma_channel_unclaim(channel);
        MP_STATE_PORT(playing_audio)[channel] = NULL;
    }
    dma->playing_in_progress = false;

    // Hold onto our buffers.
}

void audio_dma_stop_input(audio_dma_t *dma) {
    // Disable our interrupts.
    uint32_t channel_mask = 0;
    if (dma->input_channel[0] < NUM_DMA_CHANNELS) {
        channel_mask |= 1 << dma->input_channel[0];
    }
    if (dma->input_channel[1] < NUM_DMA_CHANNELS) {
        channel_mask |= 1 << dma->input_channel[1];
    }
    dma_hw->inte0 &= ~channel_mask;
    if (!dma_hw->inte0) {
        irq_set_mask_enabled(1 << DMA_IRQ_0, false);
    }

    // Run any remaining audio tasks because we remove ourselves from
    // playing_audio.
    RUN_BACKGROUND_TASKS;

    for (size_t i = 0; i < 2; i++) {
        size_t channel = dma->input_channel[i];
        dma->input_channel[i] = NUM_DMA_CHANNELS;
        if (channel == NUM_DMA_CHANNELS) {
            // Channel not in use.
            continue;
        }

        dma_channel_config c = dma_channel_get_default_config(channel);
        channel_config_set_enable(&c, false);
        dma_channel_set_config(channel, &c, false /* trigger */);

        if (dma_channel_is_busy(channel)) {
            dma_channel_abort(channel);
        }

        dma_channel_set_read_addr(channel, NULL, false /* trigger */);
        dma_channel_set_write_addr(channel, NULL, false /* trigger */);
        dma_channel_set_trans_count(channel, 0, false /* trigger */);
        dma_channel_unclaim(channel);
        MP_STATE_PORT(playing_audio)[channel] = NULL;
    }
    dma->recording_in_progress = false;

    // Hold onto our buffers.
}

void audio_dma_stop(audio_dma_t *dma) {
    if (dma->output_register_address) {
        audio_dma_stop_output(dma);
    }
    if (dma->input_register_address) {
        audio_dma_stop_input(dma);
    }
}

// To pause we simply stop the DMA. It is the responsibility of the output peripheral
// to hold the previous value.
void audio_dma_pause(audio_dma_t *dma) {
    dma_hw->ch[dma->output_channel[0]].al1_ctrl &= ~DMA_CH0_CTRL_TRIG_EN_BITS;
    dma_hw->ch[dma->output_channel[1]].al1_ctrl &= ~DMA_CH1_CTRL_TRIG_EN_BITS;
    dma_hw->ch[dma->input_channel[0]].al1_ctrl &= ~DMA_CH0_CTRL_TRIG_EN_BITS;
    dma_hw->ch[dma->input_channel[1]].al1_ctrl &= ~DMA_CH1_CTRL_TRIG_EN_BITS;
    dma->paused = true;
}

void audio_dma_resume(audio_dma_t *dma) {
    // Always re-enable the non-busy channel first so it's ready to continue when the busy channel
    // finishes and chains to it. (An interrupt could make the time between enables long.)
    if (dma_channel_is_busy(dma->output_channel[0])) {
        dma_hw->ch[dma->output_channel[1]].al1_ctrl |= DMA_CH1_CTRL_TRIG_EN_BITS;
        dma_hw->ch[dma->output_channel[0]].al1_ctrl |= DMA_CH0_CTRL_TRIG_EN_BITS;
    } else {
        dma_hw->ch[dma->output_channel[0]].al1_ctrl |= DMA_CH0_CTRL_TRIG_EN_BITS;
        dma_hw->ch[dma->output_channel[1]].al1_ctrl |= DMA_CH1_CTRL_TRIG_EN_BITS;
    }
    if (dma_channel_is_busy(dma->input_channel[0])) {
        dma_hw->ch[dma->input_channel[1]].al1_ctrl |= DMA_CH1_CTRL_TRIG_EN_BITS;
        dma_hw->ch[dma->input_channel[0]].al1_ctrl |= DMA_CH0_CTRL_TRIG_EN_BITS;
    } else {
        dma_hw->ch[dma->input_channel[0]].al1_ctrl |= DMA_CH0_CTRL_TRIG_EN_BITS;
        dma_hw->ch[dma->input_channel[1]].al1_ctrl |= DMA_CH1_CTRL_TRIG_EN_BITS;
    }
    dma->paused = false;
}

bool audio_dma_get_paused(audio_dma_t *dma) {
    uint32_t channel = NUM_DMA_CHANNELS;
    if (dma->output_channel[0] < NUM_DMA_CHANNELS) {
        channel = dma->output_channel[0];
    } else if (dma->input_channel[0] < NUM_DMA_CHANNELS) {
        channel = dma->input_channel[0];
    } else {
        return false;
    }
    return (dma->playing_in_progress || dma->recording_in_progress) && dma->paused;
}

uint32_t audio_dma_pause_all(void) {
    uint32_t result = 0;
    for (size_t channel = 0; channel < NUM_DMA_CHANNELS; channel++) {
        audio_dma_t *dma = MP_STATE_PORT(playing_audio)[channel];
        if (dma == NULL) {
            dma = MP_STATE_PORT(recording_audio)[channel];
        }
        if (dma != NULL && !audio_dma_get_paused(dma)) {
            audio_dma_pause(dma);
            result |= (1 << channel);
        }
    }
    return result;
}

void audio_dma_unpause_mask(uint32_t channel_mask) {
    for (size_t channel = 0; channel < NUM_DMA_CHANNELS; channel++) {
        audio_dma_t *dma = MP_STATE_PORT(playing_audio)[channel];
        if (dma == NULL) {
            dma = MP_STATE_PORT(recording_audio)[channel];
        }
        if (dma != NULL && (channel_mask & (1 << channel))) {
            audio_dma_resume(dma);
        }
    }
}

void audio_dma_init(audio_dma_t *dma) {
    dma->output_buffer[0] = NULL;
    dma->output_buffer[1] = NULL;
  
    dma->output_buffer_length[0] = 0;
    dma->output_buffer_length[1] = 0;
    
    dma->output_channel[0] = NUM_DMA_CHANNELS;
    dma->output_channel[1] = NUM_DMA_CHANNELS;

    dma->input_buffer[0] = NULL;
    dma->input_buffer[1] = NULL;
  
    dma->input_buffer_length[0] = 0;
    dma->input_buffer_length[1] = 0;

    dma->input_channel[0] = NUM_DMA_CHANNELS;
    dma->input_channel[1] = NUM_DMA_CHANNELS;
  
    dma->playing_in_progress = false;
    dma->recording_in_progress = false;
    dma->paused = false;
}

void audio_dma_deinit(audio_dma_t *dma) {
    #ifdef PICO_RP2350
    port_free(dma->output_buffer[0]);
    #else
    #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    m_free(dma->output_buffer[0], dma->output_buffer_length[0]);
    #else
    m_free(dma->output_buffer[0]);
    #endif
    #endif
    dma->output_buffer[0] = NULL;
    dma->output_buffer_length[0] = 0;

    #ifdef PICO_RP2350
    port_free(dma->output_buffer[1]);
    #else
    #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    m_free(dma->output_buffer[1], dma->output_buffer_length[1]);
    #else
    m_free(dma->output_buffer[1]);
    #endif
    #endif
    dma->output_buffer[1] = NULL;
    dma->output_buffer_length[1] = 0;
  
    #ifdef PICO_RP2350
    port_free(dma->input_buffer[0]);
    #else
    #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    m_free(dma->input_buffer[0], dma->input_buffer_length[0]);
    #else
    m_free(dma->input_buffer[0]);
    #endif
    #endif
    dma->input_buffer[0] = NULL;
    dma->input_buffer_length[0] = 0;

    #ifdef PICO_RP2350
    port_free(dma->input_buffer[1]);
    #else
    #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    m_free(dma->input_buffer[1], dma->input_buffer_length[1]);
    #else
    m_free(dma->input_buffer[1]);
    #endif
    #endif
    dma->input_buffer[1] = NULL;
    dma->input_buffer_length[1] = 0;
}

bool audio_dma_get_playing(audio_dma_t *dma) {
    if (dma->output_channel[0] == NUM_DMA_CHANNELS) {
        return false;
    }
    return dma->playing_in_progress;
}

bool audio_dma_get_recording(audio_dma_t *dma) {
    if (dma->input_channel[0] == NUM_DMA_CHANNELS) {
        return false;
    }
    return dma->recording_in_progress;
}

uint8_t *audio_dma_get_buffer(audio_dma_t *dma) {
    if (!dma->input_register_address || dma->input_index >= 2) {
        return NULL;
    }
    return dma->input_buffer[dma->input_index];
}

// WARN(tannewt): DO NOT print from here, or anything it calls. Printing calls
// background tasks such as this and causes a stack overflow.
// NOTE(dhalbert): I successfully printed from here while debugging.
// So it's possible, but be careful.
static void dma_callback_fun(void *arg) {
    // Any audio interrupts that happen below will requeue the background task
    // after updating channels_to_load_mask.
    audio_dma_t *dma = arg;
    if (dma == NULL) {
        return;
    }

    common_hal_mcu_disable_interrupts();
    uint32_t channels_to_load_mask = dma->channels_to_load_mask;
    // This can be 0 if the background task was queued between the call to
    // dma_callback_fun and the above read of channels_to_load_mask.
    dma->channels_to_load_mask = 0;
    common_hal_mcu_enable_interrupts();

    uint8_t first_filled_channel = NUM_DMA_CHANNELS;
    size_t filled_count = 0;
    if (dma->output_channel[0] != NUM_DMA_CHANNELS && (channels_to_load_mask & (1 << dma->output_channel[0]))) {
        audio_dma_load_next_block(dma, 0);
        first_filled_channel = dma->output_channel[0];
        filled_count++;
    }
    if (dma->output_channel[1] != NUM_DMA_CHANNELS && (channels_to_load_mask & (1 << dma->output_channel[1]))) {
        audio_dma_load_next_block(dma, 1);
        first_filled_channel = dma->output_channel[1];
        filled_count++;
    }

    // Restart if the other channel has been queued while we were filling the first or we filled two
    // now. (Two get filled if the second buffer completes while the first is waiting in the
    // background task queue.)
    if (first_filled_channel != NUM_DMA_CHANNELS && (dma->channels_to_load_mask != 0 || filled_count == 2)) {
        dma_channel_start(first_filled_channel);
    }
}

void __not_in_flash_func(isr_dma_0)(void) {
    for (size_t i = 0; i < NUM_DMA_CHANNELS; i++) {
        uint32_t mask = 1 << i;
        if ((dma_hw->ints0 & mask) == 0) {
            continue;
        }
        // acknowledge interrupt early. Doing so late means that you could lose an
        // interrupt if the buffer is very small and the DMA operation
        // completed by the time callback_add() / dma_complete() returned. This
        // affected PIO continuous write more than audio.
        dma_hw->ints0 = mask;
        if (MP_STATE_PORT(playing_audio)[i] != NULL) {
            audio_dma_t *dma = MP_STATE_PORT(playing_audio)[i];
            // Record all channels whose DMA has completed; they need loading.
            dma->channels_to_load_mask |= mask;
            // Disable the channel so that we don't play it without filling it.
            dma_hw->ch[i].al1_ctrl &= ~DMA_CH0_CTRL_TRIG_EN_BITS;
            // This is a noop if the callback is already queued.
            background_callback_add(&dma->callback, dma_callback_fun, (void *)dma);
        }
        if (MP_STATE_PORT(recording_audio)[i] != NULL) {
            audio_dma_t *dma = MP_STATE_PORT(recording_audio)[i];
            // Update last recorded buffer.
            dma->input_index = (uint8_t)(i != dma->input_channel[0]);
            // Reset destination buffer for the dma channel.
            dma_channel_set_write_addr(i, dma->input_buffer[dma->input_index], false /* trigger */);
            dma_channel_set_trans_count(i, dma->input_buffer_length[dma->input_index] / dma->output_size, false /* trigger */);
        }
        if (MP_STATE_PORT(background_pio_read)[i] != NULL) {
            rp2pio_statemachine_obj_t *pio = MP_STATE_PORT(background_pio_read)[i];
            rp2pio_statemachine_dma_complete_read(pio, i);
        }
        if (MP_STATE_PORT(background_pio_write)[i] != NULL) {
            rp2pio_statemachine_obj_t *pio = MP_STATE_PORT(background_pio_write)[i];
            rp2pio_statemachine_dma_complete_write(pio, i);
        }
    }
}

MP_REGISTER_ROOT_POINTER(mp_obj_t playing_audio[enum_NUM_DMA_CHANNELS]);
MP_REGISTER_ROOT_POINTER(mp_obj_t recording_audio[enum_NUM_DMA_CHANNELS]);
#endif
