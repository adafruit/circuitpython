// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiofilters/Phaser.h"
#include "shared-bindings/audiocore/__init__.h"

#include <stdint.h>
#include "py/runtime.h"

void common_hal_audiofilters_phaser_construct(audiofilters_phaser_obj_t *self,
    mp_obj_t frequency, mp_obj_t feedback, mp_obj_t mix, uint8_t stages,
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
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc_without_collect(self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);

    self->last_buf_idx = 1; // Which buffer to use first, toggle between 0 and 1

    // Initialize other values most effects will need.
    self->sample = NULL; // The current playing sample
    self->sample_remaining_buffer = NULL; // Pointer to the start of the sample buffer we have not played
    self->sample_buffer_length = 0; // How many samples do we have left to play (these may be 16 bit!)
    self->loop = false; // When the sample is done do we loop to the start again or stop (e.g. in a wav file)
    self->more_data = false; // Is there still more data to read from the sample or did we finish

    // The below section sets up the effect's starting values.

    // Create buffer to hold the last processed word
    self->word_buffer = m_malloc_without_collect(self->base.channel_count * sizeof(int16_t));
    memset(self->word_buffer, 0, self->base.channel_count * sizeof(int16_t));

    self->nyquist = (mp_float_t)self->base.sample_rate / 2;

    if (feedback == mp_const_none) {
        feedback = mp_obj_new_float(MICROPY_FLOAT_CONST(0.7));
    }

    synthio_block_assign_slot(frequency, &self->frequency, MP_QSTR_frequency);
    synthio_block_assign_slot(feedback, &self->feedback, MP_QSTR_feedback);
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);

    common_hal_audiofilters_phaser_set_stages(self, stages);
}

void common_hal_audiofilters_phaser_deinit(audiofilters_phaser_obj_t *self) {
    audiosample_mark_deinit(&self->base);
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
    self->word_buffer = NULL;
    self->allpass_buffer = NULL;
}

mp_obj_t common_hal_audiofilters_phaser_get_frequency(audiofilters_phaser_obj_t *self) {
    return self->frequency.obj;
}

void common_hal_audiofilters_phaser_set_frequency(audiofilters_phaser_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->frequency, MP_QSTR_frequency);
}

mp_obj_t common_hal_audiofilters_phaser_get_feedback(audiofilters_phaser_obj_t *self) {
    return self->feedback.obj;
}

void common_hal_audiofilters_phaser_set_feedback(audiofilters_phaser_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->feedback, MP_QSTR_feedback);
}

mp_obj_t common_hal_audiofilters_phaser_get_mix(audiofilters_phaser_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiofilters_phaser_set_mix(audiofilters_phaser_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->mix, MP_QSTR_mix);
}

uint8_t common_hal_audiofilters_phaser_get_stages(audiofilters_phaser_obj_t *self) {
    return self->stages;
}

void common_hal_audiofilters_phaser_set_stages(audiofilters_phaser_obj_t *self, uint8_t arg) {
    if (!arg) {
        arg = 1;
    }

    self->allpass_buffer = (int16_t *)m_realloc(self->allpass_buffer,
        #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
        self->base.channel_count * self->stages * sizeof(int16_t), // Old size
        #endif
        self->base.channel_count * arg * sizeof(int16_t));
    self->stages = arg;

    memset(self->allpass_buffer, 0, self->base.channel_count * self->stages * sizeof(int16_t));
}

void audiofilters_phaser_reset_buffer(audiofilters_phaser_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
    memset(self->word_buffer, 0, self->base.channel_count * sizeof(int16_t));
    memset(self->allpass_buffer, 0, self->base.channel_count * self->stages * sizeof(int16_t));
}

bool common_hal_audiofilters_phaser_get_playing(audiofilters_phaser_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiofilters_phaser_play(audiofilters_phaser_obj_t *self, mp_obj_t sample, bool loop) {
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

void common_hal_audiofilters_phaser_stop(audiofilters_phaser_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    self->sample = NULL;
    return;
}

audioio_get_buffer_result_t audiofilters_phaser_get_buffer(audiofilters_phaser_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    (void)channel;

    if (!single_channel_output) {
        channel = 0;
    }

    // Switch our buffers to the other buffer
    self->last_buf_idx = !self->last_buf_idx;

    // If we are using 16 bit samples we need a 16 bit pointer, 8 bit needs an 8 bit pointer
    int16_t *word_buffer = (int16_t *)self->buffer[self->last_buf_idx];
    int8_t *hword_buffer = self->buffer[self->last_buf_idx];
    uint32_t length = self->buffer_len / (self->base.bits_per_sample / 8);

    // Loop over the entire length of our buffer to fill it, this may require several calls to get data from the sample
    while (length != 0) {
        // Check if there is no more sample to play, we will either load more data, reset the sample if loop is on or clear the sample
        if (self->sample_buffer_length == 0) {
            if (!self->more_data) { // The sample has indicated it has no more data to play
                if (self->loop && self->sample) { // If we are supposed to loop reset the sample to the start
                    audiosample_reset_buffer(self->sample, false, 0);
                } else { // If we were not supposed to loop the sample, stop playing it
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

        if (self->sample == NULL) {
            // tick all block inputs
            shared_bindings_synthio_lfo_tick(self->base.sample_rate, length / self->base.channel_count);
            (void)synthio_block_slot_get(&self->frequency);
            (void)synthio_block_slot_get(&self->feedback);
            (void)synthio_block_slot_get(&self->mix);

            if (self->base.samples_signed) {
                memset(word_buffer, 0, length * (self->base.bits_per_sample / 8));
            } else {
                // For unsigned samples set to the middle which is "quiet"
                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    uint16_t *uword_buffer = (uint16_t *)word_buffer;
                    while (length--) {
                        *uword_buffer++ = 32768;
                    }
                } else {
                    memset(hword_buffer, 128, length * (self->base.bits_per_sample / 8));
                }
            }

            length = 0;
        } else {
            // we have a sample to play and filter
            // Determine how many bytes we can process to our buffer, the less of the sample we have left and our buffer remaining
            uint32_t n = MIN(MIN(self->sample_buffer_length, length), SYNTHIO_MAX_DUR * self->base.channel_count);

            int16_t *sample_src = (int16_t *)self->sample_remaining_buffer; // for 16-bit samples
            int8_t *sample_hsrc = (int8_t *)self->sample_remaining_buffer; // for 8-bit samples

            // get the effect values we need from the BlockInput. These may change at run time so you need to do bounds checking if required
            shared_bindings_synthio_lfo_tick(self->base.sample_rate, n / self->base.channel_count);
            mp_float_t frequency = synthio_block_slot_get_limited(&self->frequency, MICROPY_FLOAT_CONST(0.0), self->nyquist);
            int16_t feedback = (int16_t)(synthio_block_slot_get_limited(&self->feedback, MICROPY_FLOAT_CONST(0.1), MICROPY_FLOAT_CONST(0.9)) * 32767);
            int16_t mix = (int16_t)(synthio_block_slot_get_limited(&self->mix, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0)) * 32767);

            if (mix <= 328) { // if mix is zero (0.01 in fixed point), pure sample only
                for (uint32_t i = 0; i < n; i++) {
                    if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                        word_buffer[i] = sample_src[i];
                    } else {
                        hword_buffer[i] = sample_hsrc[i];
                    }
                }
            } else {
                // Update all-pass filter coefficient
                frequency /= self->nyquist; // scale relative to frequency range
                int16_t allpasscoef = (int16_t)((MICROPY_FLOAT_CONST(1.0) - frequency) / (MICROPY_FLOAT_CONST(1.0) + frequency) * 32767);

                for (uint32_t i = 0; i < n; i++) {
                    bool right_channel = (single_channel_output && channel == 1) || (!single_channel_output && (i % self->base.channel_count) == 1);
                    uint32_t allpass_buffer_offset = self->stages * right_channel;

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

                    int32_t word = synthio_sat16(sample_word + synthio_sat16((int32_t)self->word_buffer[right_channel] * feedback, 15), 0);
                    int32_t allpass_word = 0;

                    // Update all-pass filters
                    for (uint32_t j = 0; j < self->stages; j++) {
                        allpass_word = synthio_sat16(synthio_sat16(word * -allpasscoef, 15) + self->allpass_buffer[j + allpass_buffer_offset], 0);
                        self->allpass_buffer[j + allpass_buffer_offset] = synthio_sat16(synthio_sat16(allpass_word * allpasscoef, 15) + word, 0);
                        word = allpass_word;
                    }
                    self->word_buffer[(bool)allpass_buffer_offset] = (int16_t)word;

                    // Add original sample + effect
                    word = sample_word + (int32_t)(synthio_sat16(word * mix, 15));
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
                }
            }

            // Update the remaining length and the buffer positions based on how much we wrote into our buffer
            length -= n;
            word_buffer += n;
            hword_buffer += n;
            self->sample_remaining_buffer += (n * (self->base.bits_per_sample / 8));
            self->sample_buffer_length -= n;
        }
    }

    // Finally pass our buffer and length to the calling audio function
    *buffer = (uint8_t *)self->buffer[self->last_buf_idx];
    *buffer_length = self->buffer_len;

    // Phaser always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}
