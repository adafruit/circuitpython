// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiofilters/Filter.h"
#include "shared-bindings/audiocore/__init__.h"

#include "shared-module/synthio/Biquad.h"
#include <stdint.h>
#include "py/runtime.h"

void common_hal_audiofilters_filter_construct(audiofilters_filter_obj_t *self,
    mp_obj_t filter, mp_obj_t mix,
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

    // This buffer will be used to process samples through the biquad filter
    self->filter_buffer = m_malloc_without_collect(SYNTHIO_MAX_DUR * sizeof(int32_t));
    memset(self->filter_buffer, 0, SYNTHIO_MAX_DUR * sizeof(int32_t));

    // Initialize other values most effects will need.
    self->sample = NULL; // The current playing sample
    self->sample_remaining_buffer = NULL; // Pointer to the start of the sample buffer we have not played
    self->sample_buffer_length = 0; // How many samples do we have left to play (these may be 16 bit!)
    self->loop = false; // When the sample is done do we loop to the start again or stop (e.g. in a wav file)
    self->more_data = false; // Is there still more data to read from the sample or did we finish

    // The below section sets up the effect's starting values.

    common_hal_audiofilters_filter_set_filter(self, filter);

    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);
}

void common_hal_audiofilters_filter_deinit(audiofilters_filter_obj_t *self) {
    audiosample_mark_deinit(&self->base);
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
    self->filter = mp_const_none;
    self->filter_buffer = NULL;
    self->filter_states = NULL;
}

void common_hal_audiofilters_filter_set_filter(audiofilters_filter_obj_t *self, mp_obj_t filter_in) {
    size_t n_items;
    mp_obj_t *items;
    mp_obj_t *filter_objs;

    if (filter_in == mp_const_none) {
        n_items = 0;
        filter_objs = NULL;
    } else if (MP_OBJ_TYPE_HAS_SLOT(mp_obj_get_type(filter_in), iter)) {
        // convert object to tuple if it wasn't before
        filter_in = MP_OBJ_TYPE_GET_SLOT(&mp_type_tuple, make_new)(
            &mp_type_tuple, 1, 0, &filter_in);
        mp_obj_tuple_get(filter_in, &n_items, &items);
        for (size_t i = 0; i < n_items; i++) {
            if (!mp_obj_is_type(items[i], &synthio_biquad_type_obj)) {
                mp_raise_TypeError_varg(
                    MP_ERROR_TEXT("%q in %q must be of type %q, not %q"),
                    MP_QSTR_object,
                    MP_QSTR_filter,
                    MP_QSTR_Biquad,
                    mp_obj_get_type(items[i])->name);
            }
        }
        filter_objs = items;
    } else {
        n_items = 1;
        if (!mp_obj_is_type(filter_in, &synthio_biquad_type_obj)) {
            mp_raise_TypeError_varg(
                MP_ERROR_TEXT("%q must be of type %q or %q, not %q"),
                MP_QSTR_filter, MP_QSTR_Biquad, MP_QSTR_iterable, mp_obj_get_type(filter_in)->name);
        }
        filter_objs = &self->filter;
    }

    // everything has been checked, so we can do the following without fear

    self->filter = filter_in;
    self->filter_objs = filter_objs;
    self->filter_states = m_renew(biquad_filter_state,
        self->filter_states,
        self->filter_states_len,
        n_items);
    self->filter_states_len = n_items;
}

mp_obj_t common_hal_audiofilters_filter_get_filter(audiofilters_filter_obj_t *self) {
    return self->filter;
}

mp_obj_t common_hal_audiofilters_filter_get_mix(audiofilters_filter_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiofilters_filter_set_mix(audiofilters_filter_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->mix, MP_QSTR_mix);
}

void audiofilters_filter_reset_buffer(audiofilters_filter_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
    memset(self->filter_buffer, 0, SYNTHIO_MAX_DUR * sizeof(int32_t));

    if (self->filter_states) {
        for (uint8_t i = 0; i < self->filter_states_len; i++) {
            synthio_biquad_filter_reset(&self->filter_states[i]);
        }
    }
}

bool common_hal_audiofilters_filter_get_playing(audiofilters_filter_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiofilters_filter_play(audiofilters_filter_obj_t *self, mp_obj_t sample, bool loop) {
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

void common_hal_audiofilters_filter_stop(audiofilters_filter_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    self->sample = NULL;
    return;
}

audioio_get_buffer_result_t audiofilters_filter_get_buffer(audiofilters_filter_obj_t *self, bool single_channel_output, uint8_t channel,
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
            (void)synthio_block_slot_get(&self->mix);

            // Tick biquad filters
            for (uint8_t j = 0; j < self->filter_states_len; j++) {
                common_hal_synthio_biquad_tick(self->filter_objs[j]);
            }
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
            mp_float_t mix = synthio_block_slot_get_limited(&self->mix, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0));

            if (mix <= MICROPY_FLOAT_CONST(0.01) || !self->filter_states) { // if mix is zero pure sample only or no biquad filter objects are provided
                for (uint32_t i = 0; i < n; i++) {
                    if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                        word_buffer[i] = sample_src[i];
                    } else {
                        hword_buffer[i] = sample_hsrc[i];
                    }
                }
            } else {
                uint32_t i = 0;
                while (i < n) {
                    uint32_t n_samples = MIN(SYNTHIO_MAX_DUR, n - i);

                    // Fill filter buffer with samples
                    for (uint32_t j = 0; j < n_samples; j++) {
                        if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                            self->filter_buffer[j] = sample_src[i + j];
                        } else {
                            if (self->base.samples_signed) {
                                self->filter_buffer[j] = sample_hsrc[i + j];
                            } else {
                                // Be careful here changing from an 8 bit unsigned to signed into a 32-bit signed
                                self->filter_buffer[j] = (int8_t)(((uint8_t)sample_hsrc[i + j]) ^ 0x80);
                            }
                        }
                    }

                    // Process biquad filters
                    for (uint8_t j = 0; j < self->filter_states_len; j++) {
                        mp_obj_t filter_obj = self->filter_objs[j];
                        common_hal_synthio_biquad_tick(filter_obj);
                        synthio_biquad_filter_samples(filter_obj, &self->filter_states[j], self->filter_buffer, n_samples);
                    }

                    // Mix processed signal with original sample and transfer to output buffer
                    for (uint32_t j = 0; j < n_samples; j++) {
                        if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                            word_buffer[i + j] = synthio_mix_down_sample((int32_t)((sample_src[i + j] * (MICROPY_FLOAT_CONST(1.0) - mix)) + (self->filter_buffer[j] * mix)), SYNTHIO_MIX_DOWN_SCALE(2));
                            if (!self->base.samples_signed) {
                                word_buffer[i + j] ^= 0x8000;
                            }
                        } else {
                            if (self->base.samples_signed) {
                                hword_buffer[i + j] = (int8_t)((sample_hsrc[i + j] * (MICROPY_FLOAT_CONST(1.0) - mix)) + (self->filter_buffer[j] * mix));
                            } else {
                                hword_buffer[i + j] = (uint8_t)(((int8_t)(((uint8_t)sample_hsrc[i + j]) ^ 0x80) * (MICROPY_FLOAT_CONST(1.0) - mix)) + (self->filter_buffer[j] * mix)) ^ 0x80;
                            }
                        }
                    }

                    i += n_samples;
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

    // Filter always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}
