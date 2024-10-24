// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Mark Komus, Cooper Dalrymple
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiodelays/Echo.h"

#include <stdint.h>
#include "py/runtime.h"

void common_hal_audiodelays_echo_construct(audiodelays_echo_obj_t *self, uint32_t max_delay_ms,
    mp_obj_t delay_ms, mp_obj_t decay, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample,
    bool samples_signed, uint8_t channel_count, uint32_t sample_rate, bool freq_shift) {

    // Set whether the echo shifts frequencies as the delay changes like a doppler effect
    self->freq_shift = freq_shift;

    // Basic settings every effect and audio sample has
    // These are the effects values, not the source sample(s)
    self->bits_per_sample = bits_per_sample; // Most common is 16, but 8 is also supported in many places
    self->samples_signed = samples_signed; // Are the samples we provide signed (common is true)
    self->channel_count = channel_count; // Channels can be 1 for mono or 2 for stereo
    self->sample_rate = sample_rate; // Sample rate for the effect, this generally needs to match all audio objects

    // To smooth things out as CircuitPython is doing other tasks most audio objects have a buffer
    // A double buffer is set up here so the audio output can use DMA on buffer 1 while we
    // write to and create buffer 2.
    // This buffer is what is passed to the audio component that plays the effect.
    // Samples are set sequentially. For stereo audio they are passed L/R/L/R/...
    self->buffer_len = buffer_size; // in bytes

    self->buffer[0] = m_malloc(self->buffer_len);
    if (self->buffer[0] == NULL) {
        common_hal_audiodelays_echo_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc(self->buffer_len);
    if (self->buffer[1] == NULL) {
        common_hal_audiodelays_echo_deinit(self);
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

    // The below section sets up the echo effect's starting values. For a different effect this section will change

    // If we did not receive a BlockInput we need to create a default float value
    if (decay == MP_OBJ_NULL) {
        decay = mp_obj_new_float(0.7);
    }
    synthio_block_assign_slot(decay, &self->decay, MP_QSTR_decay);

    if (delay_ms == MP_OBJ_NULL) {
        delay_ms = mp_obj_new_float(250.0);
    }
    synthio_block_assign_slot(delay_ms, &self->delay_ms, MP_QSTR_delay_ms);

    if (mix == MP_OBJ_NULL) {
        mix = mp_obj_new_float(0.5);
    }
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);

    // Many effects may need buffers of what was played this shows how it was done for the echo
    // A maximum length buffer was created and then the current echo length can be dynamically changes
    // without having to reallocate a large chunk of memory.

    // Allocate the echo buffer for the max possible delay, echo is always 16-bit
    self->max_delay_ms = max_delay_ms;
    self->max_echo_buffer_len = self->sample_rate / 1000.0f * max_delay_ms * (self->channel_count * sizeof(uint16_t)); // bytes
    self->echo_buffer = m_malloc(self->max_echo_buffer_len);
    if (self->echo_buffer == NULL) {
        common_hal_audiodelays_echo_deinit(self);
        m_malloc_fail(self->max_echo_buffer_len);
    }
    memset(self->echo_buffer, 0, self->max_echo_buffer_len);

    // calculate everything needed for the current delay
    mp_float_t f_delay_ms = synthio_block_slot_get(&self->delay_ms);
    recalculate_delay(self, f_delay_ms);

    // read is where we read previous echo from delay_ms ago to play back now
    // write is where the store the latest playing sample to echo back later
    self->echo_buffer_read_pos = self->buffer_len / sizeof(uint16_t);
    self->echo_buffer_write_pos = 0;

    // where we read the previous echo from delay_ms ago to play back now (for freq shift)
    self->echo_buffer_left_pos = self->echo_buffer_right_pos = 0;
}

bool common_hal_audiodelays_echo_deinited(audiodelays_echo_obj_t *self) {
    if (self->echo_buffer == NULL) {
        return true;
    }
    return false;
}

void common_hal_audiodelays_echo_deinit(audiodelays_echo_obj_t *self) {
    if (common_hal_audiodelays_echo_deinited(self)) {
        return;
    }
    self->echo_buffer = NULL;
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
}

mp_obj_t common_hal_audiodelays_echo_get_delay_ms(audiodelays_echo_obj_t *self) {
    return self->delay_ms.obj;
}

void common_hal_audiodelays_echo_set_delay_ms(audiodelays_echo_obj_t *self, mp_obj_t delay_ms) {
    synthio_block_assign_slot(delay_ms, &self->delay_ms, MP_QSTR_delay_ms);

    mp_float_t f_delay_ms = synthio_block_slot_get(&self->delay_ms);

    recalculate_delay(self, f_delay_ms);
}

void recalculate_delay(audiodelays_echo_obj_t *self, mp_float_t f_delay_ms) {
    if (self->freq_shift) {
        // Calculate the rate of iteration over the echo buffer with 8 sub-bits
        self->echo_buffer_rate = MAX(self->max_delay_ms / f_delay_ms * 256.0f, 1.0);
        self->echo_buffer_len = self->max_echo_buffer_len;
    } else {
        // Calculate the current echo buffer length in bytes
        uint32_t new_echo_buffer_len = self->sample_rate / 1000.0f * f_delay_ms * (self->channel_count * sizeof(uint16_t));

        // Check if our new echo is too long for our maximum buffer
        if (new_echo_buffer_len > self->max_echo_buffer_len) {
            return;
        } else if (new_echo_buffer_len < 0.0) { // or too short!
            return;
        }

        // If the echo buffer is larger then our audio buffer weird things happen
        if (new_echo_buffer_len < self->buffer_len) {
            return;
        }

        self->echo_buffer_len = new_echo_buffer_len;

        // Clear the now unused part of the buffer or some weird artifacts appear
        memset(self->echo_buffer + self->echo_buffer_len, 0, self->max_echo_buffer_len - self->echo_buffer_len);
    }

    self->current_delay_ms = f_delay_ms;
}

mp_obj_t common_hal_audiodelays_echo_get_decay(audiodelays_echo_obj_t *self) {
    return self->decay.obj;
}

void common_hal_audiodelays_echo_set_decay(audiodelays_echo_obj_t *self, mp_obj_t decay) {
    synthio_block_assign_slot(decay, &self->decay, MP_QSTR_decay);
}

mp_obj_t common_hal_audiodelays_echo_get_mix(audiodelays_echo_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiodelays_echo_set_mix(audiodelays_echo_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->mix, MP_QSTR_mix);
}

bool common_hal_audiodelays_echo_get_freq_shift(audiodelays_echo_obj_t *self) {
    return self->freq_shift;
}

void common_hal_audiodelays_echo_set_freq_shift(audiodelays_echo_obj_t *self, bool freq_shift) {
    self->freq_shift = freq_shift;
    uint32_t delay_ms = (uint32_t)synthio_block_slot_get(&self->delay_ms);
    recalculate_delay(self, delay_ms);
}

uint32_t common_hal_audiodelays_echo_get_sample_rate(audiodelays_echo_obj_t *self) {
    return self->sample_rate;
}

uint8_t common_hal_audiodelays_echo_get_channel_count(audiodelays_echo_obj_t *self) {
    return self->channel_count;
}

uint8_t common_hal_audiodelays_echo_get_bits_per_sample(audiodelays_echo_obj_t *self) {
    return self->bits_per_sample;
}

void audiodelays_echo_reset_buffer(audiodelays_echo_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
    memset(self->echo_buffer, 0, self->max_echo_buffer_len);
}

bool common_hal_audiodelays_echo_get_playing(audiodelays_echo_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiodelays_echo_play(audiodelays_echo_obj_t *self, mp_obj_t sample, bool loop) {
    // When a sample is to be played we must ensure the samples values matches what we expect
    // Then we reset the sample and get the first buffer to play
    // The get_buffer function will actually process that data

    if (audiosample_sample_rate(sample) != self->sample_rate) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_sample_rate);
    }
    if (audiosample_channel_count(sample) != self->channel_count) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_channel_count);
    }
    if (audiosample_bits_per_sample(sample) != self->bits_per_sample) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_bits_per_sample);
    }
    bool single_buffer;
    bool samples_signed;
    uint32_t max_buffer_length;
    uint8_t spacing;
    audiosample_get_buffer_structure(sample, false, &single_buffer, &samples_signed, &max_buffer_length, &spacing);
    if (samples_signed != self->samples_signed) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_signedness);
    }

    self->sample = sample;
    self->loop = loop;

    audiosample_reset_buffer(self->sample, false, 0);
    audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);

    // Track remaining sample length in terms of bytes per sample
    self->sample_buffer_length /= (self->bits_per_sample / 8);
    // Store if we have more data in the sample to retrieve
    self->more_data = result == GET_BUFFER_MORE_DATA;

    return;
}

void common_hal_audiodelays_echo_stop(audiodelays_echo_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    // For echo we clear the sample but the echo continues until the object reading our effect stops
    self->sample = NULL;
    return;
}

#define RANGE_LOW_16 (-28000)
#define RANGE_HIGH_16 (28000)
#define RANGE_SHIFT_16 (16)
#define RANGE_SCALE_16 (0xfffffff / (32768 * 2 - RANGE_HIGH_16)) // 2 for echo+sample

// dynamic range compression via a downward compressor with hard knee
//
// When the output value is within the range +-28000 (about 85% of full scale),
// it is unchanged. Otherwise, it undergoes a gain reduction so that the
// largest possible values, (+32768,-32767) * 2 (2 for echo and sample),
// still fit within the output range
//
// This produces a much louder overall volume with multiple voices, without
// much additional processing.
//
// https://en.wikipedia.org/wiki/Dynamic_range_compression
static
int16_t mix_down_sample(int32_t sample) {
    if (sample < RANGE_LOW_16) {
        sample = (((sample - RANGE_LOW_16) * RANGE_SCALE_16) >> RANGE_SHIFT_16) + RANGE_LOW_16;
    } else if (sample > RANGE_HIGH_16) {
        sample = (((sample - RANGE_HIGH_16) * RANGE_SCALE_16) >> RANGE_SHIFT_16) + RANGE_HIGH_16;
    }
    return sample;
}

audioio_get_buffer_result_t audiodelays_echo_get_buffer(audiodelays_echo_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {

    if (!single_channel_output) {
        channel = 0;
    }

    // get the effect values we need from the BlockInput. These may change at run time so you need to do bounds checking if required
    mp_float_t mix = MIN(1.0, MAX(synthio_block_slot_get(&self->mix), 0.0));
    mp_float_t decay = MIN(1.0, MAX(synthio_block_slot_get(&self->decay), 0.0));

    uint32_t delay_ms = (uint32_t)synthio_block_slot_get(&self->delay_ms);
    if (self->current_delay_ms != delay_ms) {
        recalculate_delay(self, delay_ms);
    }

    // Switch our buffers to the other buffer
    self->last_buf_idx = !self->last_buf_idx;

    // If we are using 16 bit samples we need a 16 bit pointer, 8 bit needs an 8 bit pointer
    int16_t *word_buffer = (int16_t *)self->buffer[self->last_buf_idx];
    int8_t *hword_buffer = self->buffer[self->last_buf_idx];
    uint32_t length = self->buffer_len / (self->bits_per_sample / 8);

    // The echo buffer is always stored as a 16-bit value internally
    int16_t *echo_buffer = (int16_t *)self->echo_buffer;
    uint32_t echo_buf_len = self->echo_buffer_len / sizeof(uint16_t);

    // Set our echo buffer position accounting for stereo
    uint32_t echo_buffer_pos = 0;
    if (self->freq_shift) {
        echo_buffer_pos = self->echo_buffer_left_pos;
        if (channel == 1) {
            echo_buffer_pos = self->echo_buffer_right_pos;
        }
    }

    // Loop over the entire length of our buffer to fill it, this may require several calls to get data from the sample
    while (length != 0) {
        // Check if there is no more sample to play, we will either load more data, reset the sample if loop is on or clear the sample
        if (self->sample_buffer_length == 0) {
            if (!self->more_data) { // The sample has indicated it has no more data to play
                if (self->loop && self->sample) { // If we are supposed to loop reset the sample to the start
                    audiosample_reset_buffer(self->sample, false, 0);
                } else { // If we were not supposed to loop the sample, stop playing it but we still need to play the echo
                    self->sample = NULL;
                }
            }
            if (self->sample) {
                // Load another sample buffer to play
                audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);
                // Track length in terms of words.
                self->sample_buffer_length /= (self->bits_per_sample / 8);
                self->more_data = result == GET_BUFFER_MORE_DATA;
            }
        }

        // If we have no sample keep the echo echoing
        if (self->sample == NULL) {
            if (mix <= 0.01) {  // Mix of 0 is pure sample sound. We have no sample so no sound
                if (self->samples_signed) {
                    memset(word_buffer, 0, length * (self->bits_per_sample / 8));
                } else {
                    // For unsigned samples set to the middle which is "quiet"
                    if (MP_LIKELY(self->bits_per_sample == 16)) {
                        memset(word_buffer, 32768, length * (self->bits_per_sample / 8));
                    } else {
                        memset(hword_buffer, 128, length * (self->bits_per_sample / 8));
                    }
                }
            } else {
                // Since we have no sample we can just iterate over the our entire remaining buffer and finish
                for (uint32_t i = 0; i < length; i++) {
                    int16_t echo, word = 0;
                    uint32_t next_buffer_pos = 0;

                    if (self->freq_shift) {
                        echo = echo_buffer[echo_buffer_pos >> 8];
                        next_buffer_pos = echo_buffer_pos + self->echo_buffer_rate;

                        word = echo * decay;
                        for (uint32_t j = echo_buffer_pos >> 8; j < next_buffer_pos >> 8; j++) {
                            echo_buffer[j % echo_buf_len] = word;
                        }
                    } else {
                        echo = echo_buffer[self->echo_buffer_read_pos++];
                        word = echo * decay;
                        echo_buffer[self->echo_buffer_write_pos++] = word;
                    }

                    word = echo * mix;

                    if (MP_LIKELY(self->bits_per_sample == 16)) {
                        word_buffer[i] = word;
                        if (!self->samples_signed) {
                            word_buffer[i] ^= 0x8000;
                        }
                    } else {
                        hword_buffer[i] = (int8_t)word;
                        if (!self->samples_signed) {
                            hword_buffer[i] ^= 0x80;
                        }
                    }

                    if (self->freq_shift) {
                        echo_buffer_pos = next_buffer_pos % (echo_buf_len << 8);
                    } else {
                        if (self->echo_buffer_read_pos >= echo_buf_len) {
                            self->echo_buffer_read_pos = 0;
                        }
                        if (self->echo_buffer_write_pos >= echo_buf_len) {
                            self->echo_buffer_write_pos = 0;
                        }
                    }
                }
            }

            length = 0;
        } else {
            // we have a sample to play and echo
            // Determine how many bytes we can process to our buffer, the less of the sample we have left and our buffer remaining
            uint32_t n = MIN(self->sample_buffer_length, length);

            int16_t *sample_src = (int16_t *)self->sample_remaining_buffer; // for 16-bit samples
            int8_t *sample_hsrc = (int8_t *)self->sample_remaining_buffer; // for 8-bit samples

            if (mix <= 0.01) { // if mix is zero pure sample only
                for (uint32_t i = 0; i < n; i++) {
                    if (MP_LIKELY(self->bits_per_sample == 16)) {
                        word_buffer[i] = sample_src[i];
                    } else {
                        hword_buffer[i] = sample_hsrc[i];
                    }
                }
            } else {
                for (uint32_t i = 0; i < n; i++) {
                    int32_t sample_word = 0;
                    if (MP_LIKELY(self->bits_per_sample == 16)) {
                        sample_word = sample_src[i];
                    } else {
                        if (self->samples_signed) {
                            sample_word = sample_hsrc[i];
                        } else {
                            // Be careful here changing from an 8 bit unsigned to signed into a 32-bit signed
                            sample_word = (int8_t)(((uint8_t)sample_hsrc[i]) ^ 0x80);
                        }
                    }

                    int32_t echo, word = 0;
                    uint32_t next_buffer_pos = 0;
                    if (self->freq_shift) {
                        echo = echo_buffer[echo_buffer_pos >> 8];
                        next_buffer_pos = echo_buffer_pos + self->echo_buffer_rate;
                        word = echo * decay + sample_word;
                    } else {
                        echo = echo_buffer[self->echo_buffer_read_pos++];
                        word = echo * decay + sample_word;
                    }

                    if (MP_LIKELY(self->bits_per_sample == 16)) {
                        word = mix_down_sample(word);
                        if (self->freq_shift) {
                            for (uint32_t j = echo_buffer_pos >> 8; j < next_buffer_pos >> 8; j++) {
                                echo_buffer[j % echo_buf_len] = (int16_t)word;
                            }
                        } else {
                            echo_buffer[self->echo_buffer_write_pos++] = (int16_t)word;
                        }
                    } else {
                        // Do not have mix_down for 8 bit so just hard cap samples into 1 byte
                        if (word > 127) {
                            word = 127;
                        } else if (word < -128) {
                            word = -128;
                        }
                        if (self->freq_shift) {
                            for (uint32_t j = echo_buffer_pos >> 8; j < next_buffer_pos >> 8; j++) {
                                echo_buffer[j % echo_buf_len] = (int8_t)word;
                            }
                        } else {
                            echo_buffer[self->echo_buffer_write_pos++] = (int8_t)word;
                        }
                    }

                    word = echo + sample_word;

                    if (MP_LIKELY(self->bits_per_sample == 16)) {
                        word_buffer[i] = (sample_word * (1.0 - mix)) + (word * mix);
                        if (!self->samples_signed) {
                            word_buffer[i] ^= 0x8000;
                        }
                    } else {
                        int8_t mixed = (sample_word * (1.0 - mix)) + (word * mix);
                        if (self->samples_signed) {
                            hword_buffer[i] = mixed;
                        } else {
                            hword_buffer[i] = (uint8_t)mixed ^ 0x80;
                        }
                    }

                    if (self->freq_shift) {
                        echo_buffer_pos = next_buffer_pos % (echo_buf_len << 8);
                    } else {
                        if (self->echo_buffer_read_pos >= echo_buf_len) {
                            self->echo_buffer_read_pos = 0;
                        }
                        if (self->echo_buffer_write_pos >= echo_buf_len) {
                            self->echo_buffer_write_pos = 0;
                        }
                    }
                }
            }

            // Update the remaining length and the buffer positions based on how much we wrote into our buffer
            length -= n;
            word_buffer += n;
            hword_buffer += n;
            self->sample_remaining_buffer += (n * (self->bits_per_sample / 8));
            self->sample_buffer_length -= n;
        }
    }

    if (self->freq_shift) {
        if (channel == 0) {
            self->echo_buffer_left_pos = echo_buffer_pos;
        } else if (channel == 1) {
            self->echo_buffer_right_pos = echo_buffer_pos;
        }
    }

    // Finally pass our buffer and length to the calling audio function
    *buffer = (uint8_t *)self->buffer[self->last_buf_idx];
    *buffer_length = self->buffer_len;

    // Echo always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}

void audiodelays_echo_get_buffer_structure(audiodelays_echo_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {

    // Return information about the effect's buffer (not the sample's)
    // These are used by calling audio objects to determine how to handle the effect's buffer
    *single_buffer = false;
    *samples_signed = self->samples_signed;
    *max_buffer_length = self->buffer_len;
    if (single_channel_output) {
        *spacing = self->channel_count;
    } else {
        *spacing = 1;
    }
}
