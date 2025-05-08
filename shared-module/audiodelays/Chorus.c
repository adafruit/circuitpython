// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Mark Komus
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiodelays/Chorus.h"

#include <stdint.h>
#include <math.h>
#include "py/runtime.h"

void common_hal_audiodelays_chorus_construct(audiodelays_chorus_obj_t *self, uint32_t max_delay_ms,
    mp_obj_t delay_ms, mp_obj_t voices, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample,
    bool samples_signed, uint8_t channel_count, uint32_t sample_rate) {

    // Basic settings every effect and audio sample has
    // These are the effects values, not the source sample(s)
    self->base.bits_per_sample = bits_per_sample; // Most common is 16, but 8 is also supported in many places
    self->base.samples_signed = samples_signed; // Are the samples we provide signed (common is true)
    self->base.channel_count = channel_count; // Channels can be 1 for mono or 2 for stereo
    self->base.sample_rate = sample_rate; // Sample rate for the effect, this generally needs to match all audio objects
    self->base.single_buffer = false;
    self->base.max_buffer_length = buffer_size;

    // To smooth things out as CircuitPython is doing other tasks most audio objects have a buffer
    // A double buffer is set up here so the audio output can use DMA on buffer 1 while we
    // write to and create buffer 2.
    // This buffer is what is passed to the audio component that plays the effect.
    // Samples are set sequentially. For stereo audio they are passed L/R/L/R/...
    self->buffer_len = buffer_size; // in bytes

    self->buffer[0] = m_malloc_without_collect(self->buffer_len);
    if (self->buffer[0] == NULL) {
        common_hal_audiodelays_chorus_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc_without_collect(self->buffer_len);
    if (self->buffer[1] == NULL) {
        common_hal_audiodelays_chorus_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[1], 0, self->buffer_len);

    self->last_buf_idx = 1; // Which buffer to use first, toggle between 0 and 1

    // Initialize other values most effects will need.
    self->sample = NULL; // The current playing sample
    self->sample_remaining_buffer = NULL; // Pointer to the start of the sample buffer we have not played
    self->sample_buffer_length = 0; // How many samples do we have left to play (these may be 16 bit!)
    self->loop = false; // When the sample is done do we loop to the start again or stop (e.g. in a wav file)
    self->more_data = false; // Is there still more data to read from the sample or did we finish

    // The below section sets up the chorus effect's starting values. For a different effect this section will change

    // If we did not receive a BlockInput we need to create a default float value
    if (voices == MP_OBJ_NULL) {
        voices = mp_obj_new_float(MICROPY_FLOAT_CONST(1.0));
    }
    synthio_block_assign_slot(voices, &self->voices, MP_QSTR_voices);

    if (delay_ms == MP_OBJ_NULL) {
        delay_ms = mp_obj_new_float(MICROPY_FLOAT_CONST(50.0));
    }
    synthio_block_assign_slot(delay_ms, &self->delay_ms, MP_QSTR_delay_ms);

    if (mix == MP_OBJ_NULL) {
        mix = mp_obj_new_float(MICROPY_FLOAT_CONST(0.5));
    }
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);

    // Many effects may need buffers of what was played this shows how it was done for the chorus
    // A maximum length buffer was created and then the current chorus length can be dynamically changes
    // without having to reallocate a large chunk of memory.

    // Allocate the chorus buffer for the max possible delay, chorus is always 16-bit
    self->max_delay_ms = max_delay_ms;
    self->max_chorus_buffer_len = (uint32_t)(self->base.sample_rate / MICROPY_FLOAT_CONST(1000.0) * max_delay_ms * (self->base.channel_count * sizeof(uint16_t))); // bytes
    self->chorus_buffer = m_malloc_without_collect(self->max_chorus_buffer_len);
    if (self->chorus_buffer == NULL) {
        common_hal_audiodelays_chorus_deinit(self);
        m_malloc_fail(self->max_chorus_buffer_len);
    }
    memset(self->chorus_buffer, 0, self->max_chorus_buffer_len);

    // calculate the length of a single sample in milliseconds
    self->sample_ms = MICROPY_FLOAT_CONST(1000.0) / self->base.sample_rate;

    // calculate everything needed for the current delay
    mp_float_t f_delay_ms = synthio_block_slot_get(&self->delay_ms);
    chorus_recalculate_delay(self, f_delay_ms);

    // where we are storing the next chorus sample
    self->chorus_buffer_pos = 0;
}

bool common_hal_audiodelays_chorus_deinited(audiodelays_chorus_obj_t *self) {
    if (self->chorus_buffer == NULL) {
        return true;
    }
    return false;
}

void common_hal_audiodelays_chorus_deinit(audiodelays_chorus_obj_t *self) {
    if (common_hal_audiodelays_chorus_deinited(self)) {
        return;
    }
    self->chorus_buffer = NULL;
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
}

mp_obj_t common_hal_audiodelays_chorus_get_delay_ms(audiodelays_chorus_obj_t *self) {
    return self->delay_ms.obj;
}

void common_hal_audiodelays_chorus_set_delay_ms(audiodelays_chorus_obj_t *self, mp_obj_t delay_ms) {
    synthio_block_assign_slot(delay_ms, &self->delay_ms, MP_QSTR_delay_ms);

    mp_float_t f_delay_ms = synthio_block_slot_get(&self->delay_ms);

    chorus_recalculate_delay(self, f_delay_ms);
}

void chorus_recalculate_delay(audiodelays_chorus_obj_t *self, mp_float_t f_delay_ms) {
    // Require that delay is at least 1 sample long
    f_delay_ms = MAX(f_delay_ms, self->sample_ms);

    // Calculate the current chorus buffer length in bytes
    uint32_t new_chorus_buffer_len = (uint32_t)(self->base.sample_rate / MICROPY_FLOAT_CONST(1000.0) * f_delay_ms) * (self->base.channel_count * sizeof(uint16_t));

    self->chorus_buffer_len = new_chorus_buffer_len;

    self->current_delay_ms = f_delay_ms;
}

mp_obj_t common_hal_audiodelays_chorus_get_voices(audiodelays_chorus_obj_t *self) {
    return self->voices.obj;
}

void common_hal_audiodelays_chorus_set_voices(audiodelays_chorus_obj_t *self, mp_obj_t voices) {
    synthio_block_assign_slot(voices, &self->voices, MP_QSTR_voices);
}

void audiodelays_chorus_reset_buffer(audiodelays_chorus_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
    memset(self->chorus_buffer, 0, self->chorus_buffer_len);
}

mp_obj_t common_hal_audiodelays_chorus_get_mix(audiodelays_chorus_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiodelays_chorus_set_mix(audiodelays_chorus_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->mix, MP_QSTR_mix);
}

bool common_hal_audiodelays_chorus_get_playing(audiodelays_chorus_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiodelays_chorus_play(audiodelays_chorus_obj_t *self, mp_obj_t sample, bool loop) {
    audiosample_must_match(&self->base, sample);

    self->sample = sample;
    self->loop = loop;

    audiosample_reset_buffer(self->sample, false, 0);
    audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);

    // Track remaining sample length in terms of bytes per sample
    self->sample_buffer_length /= (self->base.bits_per_sample / 8);
    // Store if we have more data in the sample to retrieve
    self->more_data = result == GET_BUFFER_MORE_DATA;

    return;
}

void common_hal_audiodelays_chorus_stop(audiodelays_chorus_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    // For chorus we clear the sample but the chorus continues until the object reading our effect stops
    self->sample = NULL;
    return;
}

audioio_get_buffer_result_t audiodelays_chorus_get_buffer(audiodelays_chorus_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {

    // Switch our buffers to the other buffer
    self->last_buf_idx = !self->last_buf_idx;

    // If we are using 16 bit samples we need a 16 bit pointer, 8 bit needs an 8 bit pointer
    int16_t *word_buffer = (int16_t *)self->buffer[self->last_buf_idx];
    int8_t *hword_buffer = self->buffer[self->last_buf_idx];
    uint32_t length = self->buffer_len / (self->base.bits_per_sample / 8);

    // The chorus buffer is always stored as a 16-bit value internally
    int16_t *chorus_buffer = (int16_t *)self->chorus_buffer;
    uint32_t chorus_buf_len = self->chorus_buffer_len / sizeof(uint16_t);
    uint32_t max_chorus_buf_len = self->max_chorus_buffer_len / sizeof(uint16_t);

    // Loop over the entire length of our buffer to fill it, this may require several calls to get data from the sample
    while (length != 0) {
        // Check if there is no more sample to play, we will either load more data, reset the sample if loop is on or clear the sample
        if (self->sample_buffer_length == 0) {
            if (!self->more_data) { // The sample has indicated it has no more data to play
                if (self->loop && self->sample) { // If we are supposed to loop reset the sample to the start
                    audiosample_reset_buffer(self->sample, false, 0);
                } else { // If we were not supposed to loop the sample, stop playing it but we still need to play the chorus
                    self->sample = NULL;
                }
            }
            if (self->sample) {
                // Load another sample buffer to play
                audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);
                // Track length in terms of words.
                self->sample_buffer_length /= (self->base.bits_per_sample / 8);
                self->more_data = result == GET_BUFFER_MORE_DATA;
            }
        }

        // Determine how many bytes we can process to our buffer, the less of the sample we have left and our buffer remaining
        uint32_t n;
        if (self->sample == NULL) {
            n = MIN(length, SYNTHIO_MAX_DUR * self->base.channel_count);
        } else {
            n = MIN(MIN(self->sample_buffer_length, length), SYNTHIO_MAX_DUR * self->base.channel_count);
        }

        // get the effect values we need from the BlockInput. These may change at run time so you need to do bounds checking if required
        shared_bindings_synthio_lfo_tick(self->base.sample_rate, n / self->base.channel_count);

        int32_t voices = (int32_t)MAX(synthio_block_slot_get(&self->voices), 1.0);
        int32_t mix_down_scale = SYNTHIO_MIX_DOWN_SCALE(voices);
        mp_float_t mix = synthio_block_slot_get_limited(&self->mix, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0));

        mp_float_t f_delay_ms = synthio_block_slot_get(&self->delay_ms);
        if (MICROPY_FLOAT_C_FUN(fabs)(self->current_delay_ms - f_delay_ms) >= self->sample_ms) {
            chorus_recalculate_delay(self, f_delay_ms);
        }

        if (self->sample == NULL) {
            if (self->base.samples_signed) {
                memset(word_buffer, 0, n * (self->base.bits_per_sample / 8));
            } else {
                // For unsigned samples set to the middle which is "quiet"
                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    uint16_t *uword_buffer = (uint16_t *)word_buffer;
                    for (uint32_t i = 0; i < n; i++) {
                        *uword_buffer++ = 32768;
                    }
                } else {
                    memset(hword_buffer, 128, n * (self->base.bits_per_sample / 8));
                }
            }
        } else {
            int16_t *sample_src = (int16_t *)self->sample_remaining_buffer; // for 16-bit samples
            int8_t *sample_hsrc = (int8_t *)self->sample_remaining_buffer; // for 8-bit samples

            for (uint32_t i = 0; i < n; i++) {
                int32_t sample_word = 0;
                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    sample_word = sample_src[i];
                } else {
                    if (self->base.samples_signed) {
                        sample_word = sample_hsrc[i];
                    } else {
                        // Be careful here changing from an 8 bit unsigned to signed into a 32-bit signed
                        sample_word = (int8_t)(((uint8_t)sample_hsrc[i]) ^ 0x80);
                    }
                }

                chorus_buffer[self->chorus_buffer_pos++] = (int16_t)sample_word;

                int32_t word = 0;
                if (voices == 1) {
                    word = sample_word;
                } else {
                    int32_t step = chorus_buf_len / (voices - 1) - 1;
                    int32_t c_pos = self->chorus_buffer_pos - 1;

                    for (int32_t v = 0; v < voices; v++) {
                        if (c_pos < 0) {
                            c_pos += max_chorus_buf_len;
                        }
                        word += chorus_buffer[c_pos];

                        c_pos -= step;
                    }

                    // Dividing would get an average but does not sound as good
                    // Leaving this here in case someone wants to try an average instead
                    // word = word / voices;

                    word = synthio_mix_down_sample(word, mix_down_scale);
                }

                // Add original sample + effect
                word = sample_word + (int32_t)(word * mix);
                word = synthio_mix_down_sample(word, 2);

                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    word_buffer[i] = word;
                    if (!self->base.samples_signed) {
                        word_buffer[i] ^= 0x8000;
                    }
                } else {
                    int8_t out = word;
                    if (self->base.samples_signed) {
                        hword_buffer[i] = out;
                    } else {
                        hword_buffer[i] = (uint8_t)out ^ 0x80;
                    }
                }

                if (self->chorus_buffer_pos >= max_chorus_buf_len) {
                    self->chorus_buffer_pos = 0;
                }
            }
            self->sample_remaining_buffer += (n * (self->base.bits_per_sample / 8));
            self->sample_buffer_length -= n;
        }
        // Update the remaining length and the buffer positions based on how much we wrote into our buffer
        length -= n;
        word_buffer += n;
        hword_buffer += n;
    }

    // Finally pass our buffer and length to the calling audio function
    *buffer = (uint8_t *)self->buffer[self->last_buf_idx];
    *buffer_length = self->buffer_len;

    // Chorus always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}
