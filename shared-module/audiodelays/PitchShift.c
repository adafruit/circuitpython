// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiodelays/PitchShift.h"
#include "shared-bindings/audiocore/__init__.h"

#include <stdint.h>
#include "py/runtime.h"
#include <math.h>

void common_hal_audiodelays_pitch_shift_construct(audiodelays_pitch_shift_obj_t *self,
    mp_obj_t semitones, mp_obj_t mix, uint32_t window, uint32_t overlap,
    uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate) {

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
        common_hal_audiodelays_pitch_shift_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc_without_collect(self->buffer_len);
    if (self->buffer[1] == NULL) {
        common_hal_audiodelays_pitch_shift_deinit(self);
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

    // The below section sets up the effect's starting values.

    synthio_block_assign_slot(semitones, &self->semitones, MP_QSTR_semitones);
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);

    // Allocate the window buffer
    self->window_len = window; // bytes
    self->window_buffer = m_malloc_without_collect(self->window_len);
    if (self->window_buffer == NULL) {
        common_hal_audiodelays_pitch_shift_deinit(self);
        m_malloc_fail(self->window_len);
    }
    memset(self->window_buffer, 0, self->window_len);

    // Allocate the overlap buffer
    self->overlap_len = overlap; // bytes
    if (self->overlap_len) {
        self->overlap_buffer = m_malloc_without_collect(self->overlap_len);
        if (self->overlap_buffer == NULL) {
            common_hal_audiodelays_pitch_shift_deinit(self);
            m_malloc_fail(self->overlap_len);
        }
        memset(self->overlap_buffer, 0, self->overlap_len);
    } else {
        self->overlap_buffer = NULL;
    }

    // The current position that the end of the overlap buffer will be written to the window buffer
    self->window_index = 0;

    // The position that the current sample will be written to the overlap buffer
    self->overlap_index = 0;

    // The position that the window buffer will be read from and written to the output
    self->read_index = 0;

    // Calculate the rate to increment the read index
    mp_float_t f_semitones = synthio_block_slot_get(&self->semitones);
    recalculate_rate(self, f_semitones);
}

void common_hal_audiodelays_pitch_shift_deinit(audiodelays_pitch_shift_obj_t *self) {
    audiosample_mark_deinit(&self->base);
    self->window_buffer = NULL;
    self->overlap_buffer = NULL;
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
}

mp_obj_t common_hal_audiodelays_pitch_shift_get_semitones(audiodelays_pitch_shift_obj_t *self) {
    return self->semitones.obj;
}

void common_hal_audiodelays_pitch_shift_set_semitones(audiodelays_pitch_shift_obj_t *self, mp_obj_t delay_ms) {
    synthio_block_assign_slot(delay_ms, &self->semitones, MP_QSTR_semitones);
    mp_float_t semitones = synthio_block_slot_get(&self->semitones);
    recalculate_rate(self, semitones);
}

void recalculate_rate(audiodelays_pitch_shift_obj_t *self, mp_float_t semitones) {
    self->read_rate = (uint32_t)(MICROPY_FLOAT_C_FUN(pow)(2.0, semitones / MICROPY_FLOAT_CONST(12.0)) * (1 << PITCH_READ_SHIFT));
    self->current_semitones = semitones;
}

mp_obj_t common_hal_audiodelays_pitch_shift_get_mix(audiodelays_pitch_shift_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiodelays_pitch_shift_set_mix(audiodelays_pitch_shift_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->mix, MP_QSTR_mix);
}

void audiodelays_pitch_shift_reset_buffer(audiodelays_pitch_shift_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
    memset(self->window_buffer, 0, self->window_len);
    if (self->overlap_len) {
        memset(self->overlap_buffer, 0, self->overlap_len);
    }
}

bool common_hal_audiodelays_pitch_shift_get_playing(audiodelays_pitch_shift_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiodelays_pitch_shift_play(audiodelays_pitch_shift_obj_t *self, mp_obj_t sample, bool loop) {
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

void common_hal_audiodelays_pitch_shift_stop(audiodelays_pitch_shift_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    self->sample = NULL;
    return;
}

audioio_get_buffer_result_t audiodelays_pitch_shift_get_buffer(audiodelays_pitch_shift_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {

    if (!single_channel_output) {
        channel = 0;
    }

    // Switch our buffers to the other buffer
    self->last_buf_idx = !self->last_buf_idx;

    // If we are using 16 bit samples we need a 16 bit pointer, 8 bit needs an 8 bit pointer
    int16_t *word_buffer = (int16_t *)self->buffer[self->last_buf_idx];
    int8_t *hword_buffer = self->buffer[self->last_buf_idx];
    uint32_t length = self->buffer_len / (self->base.bits_per_sample / 8);

    // The window and overlap buffers are always stored as a 16-bit value internally
    int16_t *window_buffer = (int16_t *)self->window_buffer;
    uint32_t window_size = self->window_len / sizeof(uint16_t) / self->base.channel_count;

    int16_t *overlap_buffer = NULL;
    uint32_t overlap_size = 0;
    if (self->overlap_len) {
        overlap_buffer = (int16_t *)self->overlap_buffer;
        overlap_size = self->overlap_len / sizeof(uint16_t) / self->base.channel_count;
    }

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
            if (self->base.samples_signed) {
                memset(word_buffer, 0, length * (self->base.bits_per_sample / 8));
            } else {
                // For unsigned samples set to the middle which is "quiet"
                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    memset(word_buffer, 32768, length * (self->base.bits_per_sample / 8));
                } else {
                    memset(hword_buffer, 128, length * (self->base.bits_per_sample / 8));
                }
            }

            // tick all block inputs
            shared_bindings_synthio_lfo_tick(self->base.sample_rate, length / self->base.channel_count);
            (void)synthio_block_slot_get(&self->semitones);
            (void)synthio_block_slot_get(&self->mix);

            length = 0;
        } else {
            // we have a sample to play and apply effect
            // Determine how many bytes we can process to our buffer, the less of the sample we have left and our buffer remaining
            uint32_t n = MIN(MIN(self->sample_buffer_length, length), SYNTHIO_MAX_DUR * self->base.channel_count);

            int16_t *sample_src = (int16_t *)self->sample_remaining_buffer; // for 16-bit samples
            int8_t *sample_hsrc = (int8_t *)self->sample_remaining_buffer; // for 8-bit samples

            // get the effect values we need from the BlockInput. These may change at run time so you need to do bounds checking if required
            shared_bindings_synthio_lfo_tick(self->base.sample_rate, n / self->base.channel_count);
            mp_float_t semitones = synthio_block_slot_get(&self->semitones);
            mp_float_t mix = synthio_block_slot_get_limited(&self->mix, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0)) * MICROPY_FLOAT_CONST(2.0);

            // Only recalculate rate if semitones has changes
            if (memcmp(&semitones, &self->current_semitones, sizeof(mp_float_t))) {
                recalculate_rate(self, semitones);
            }

            for (uint32_t i = 0; i < n; i++) {
                bool buf_offset = (channel == 1 || i % self->base.channel_count == 1);

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

                if (overlap_size) {
                    // Copy last sample from overlap and store in buffer
                    window_buffer[self->window_index + window_size * buf_offset] = overlap_buffer[self->overlap_index + overlap_size * buf_offset];

                    // Save current sample in overlap
                    overlap_buffer[self->overlap_index + overlap_size * buf_offset] = (int16_t)sample_word;
                } else {
                    // Write sample to buffer
                    window_buffer[self->window_index + window_size * buf_offset] = (int16_t)sample_word;
                }

                // Determine how far we are into the overlap
                uint32_t read_index = self->read_index >> PITCH_READ_SHIFT;
                uint32_t read_overlap_offset = read_index + window_size * (read_index < self->window_index) - self->window_index;

                // Read sample from buffer
                int32_t word = (int32_t)window_buffer[read_index + window_size * buf_offset];

                // Check if we're within the overlap range and mix buffer sample with overlap sample
                if (overlap_size && read_overlap_offset > 0 && read_overlap_offset <= overlap_size) {
                    // Apply volume based on overlap position to buffer sample
                    word *= (int32_t)read_overlap_offset;

                    // Add overlap with volume based on overlap position
                    word += (int32_t)overlap_buffer[((self->overlap_index + read_overlap_offset) % overlap_size) + overlap_size * buf_offset] * (int32_t)(overlap_size - read_overlap_offset);

                    // Scale down
                    word /= (int32_t)overlap_size;
                }

                word = (int32_t)((sample_word * MIN(MICROPY_FLOAT_CONST(2.0) - mix, MICROPY_FLOAT_CONST(1.0))) + (word * MIN(mix, MICROPY_FLOAT_CONST(1.0))));
                word = synthio_mix_down_sample(word, SYNTHIO_MIX_DOWN_SCALE(2));

                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    word_buffer[i] = (int16_t)word;
                    if (!self->base.samples_signed) {
                        word_buffer[i] ^= 0x8000;
                    }
                } else {
                    int8_t mixed = (int8_t)word;
                    if (self->base.samples_signed) {
                        hword_buffer[i] = mixed;
                    } else {
                        hword_buffer[i] = (uint8_t)mixed ^ 0x80;
                    }
                }

                if (self->base.channel_count == 1 || buf_offset) {
                    // Increment window buffer write pointer
                    self->window_index++;
                    if (self->window_index >= window_size) {
                        self->window_index = 0;
                    }

                    // Increment overlap buffer pointer
                    if (overlap_size) {
                        self->overlap_index++;
                        if (self->overlap_index >= overlap_size) {
                            self->overlap_index = 0;
                        }
                    }

                    // Increment window buffer read pointer by rate
                    self->read_index += self->read_rate;
                    if (self->read_index >= window_size << PITCH_READ_SHIFT) {
                        self->read_index -= window_size << PITCH_READ_SHIFT;
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

    // PitchShift always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}
