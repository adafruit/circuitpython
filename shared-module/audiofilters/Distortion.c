// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "py/obj.h"
#include "py/runtime.h"
#include <math.h>
#include "shared-bindings/audiofilters/Distortion.h"
#include "shared-module/audiofilters/Distortion.h"
#include "shared-bindings/audiocore/__init__.h"

/**
 * Based on Godot's AudioEffectDistortion
 * - https://docs.godotengine.org/en/stable/classes/class_audioeffectdistortion.html
 * - https://github.com/godotengine/godot/blob/master/servers/audio/effects/audio_effect_distortion.cpp
 */

void common_hal_audiofilters_distortion_construct(audiofilters_distortion_obj_t *self,
    mp_obj_t drive, mp_obj_t pre_gain, mp_obj_t post_gain,
    audiofilters_distortion_mode mode, bool soft_clip, mp_obj_t mix,
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
        common_hal_audiofilters_distortion_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc_without_collect(self->buffer_len);
    if (self->buffer[1] == NULL) {
        common_hal_audiofilters_distortion_deinit(self);
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

    synthio_block_assign_slot(drive, &self->drive, MP_QSTR_drive);
    synthio_block_assign_slot(pre_gain, &self->pre_gain, MP_QSTR_pre_gain);
    synthio_block_assign_slot(post_gain, &self->post_gain, MP_QSTR_post_gain);
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);

    self->mode = mode;
    self->soft_clip = soft_clip;
}

void common_hal_audiofilters_distortion_deinit(audiofilters_distortion_obj_t *self) {
    audiosample_mark_deinit(&self->base);
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
}

mp_obj_t common_hal_audiofilters_distortion_get_drive(audiofilters_distortion_obj_t *self) {
    return self->drive.obj;
}

void common_hal_audiofilters_distortion_set_drive(audiofilters_distortion_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->drive, MP_QSTR_drive);
}

mp_obj_t common_hal_audiofilters_distortion_get_pre_gain(audiofilters_distortion_obj_t *self) {
    return self->pre_gain.obj;
}

void common_hal_audiofilters_distortion_set_pre_gain(audiofilters_distortion_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->pre_gain, MP_QSTR_pre_gain);
}

mp_obj_t common_hal_audiofilters_distortion_get_post_gain(audiofilters_distortion_obj_t *self) {
    return self->post_gain.obj;
}

void common_hal_audiofilters_distortion_set_post_gain(audiofilters_distortion_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->post_gain, MP_QSTR_post_gain);
}

audiofilters_distortion_mode common_hal_audiofilters_distortion_get_mode(audiofilters_distortion_obj_t *self) {
    return self->mode;
}

void common_hal_audiofilters_distortion_set_mode(audiofilters_distortion_obj_t *self, audiofilters_distortion_mode arg) {
    self->mode = arg;
}

bool common_hal_audiofilters_distortion_get_soft_clip(audiofilters_distortion_obj_t *self) {
    return self->soft_clip;
}

void common_hal_audiofilters_distortion_set_soft_clip(audiofilters_distortion_obj_t *self, bool soft_clip) {
    self->soft_clip = soft_clip;
}

mp_obj_t common_hal_audiofilters_distortion_get_mix(audiofilters_distortion_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiofilters_distortion_set_mix(audiofilters_distortion_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->mix, MP_QSTR_mix);
}

void audiofilters_distortion_reset_buffer(audiofilters_distortion_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
}

bool common_hal_audiofilters_distortion_get_playing(audiofilters_distortion_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiofilters_distortion_play(audiofilters_distortion_obj_t *self, mp_obj_t sample, bool loop) {
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

void common_hal_audiofilters_distortion_stop(audiofilters_distortion_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    self->sample = NULL;
    return;
}

static mp_float_t db_to_linear(mp_float_t value) {
    return MICROPY_FLOAT_C_FUN(exp)(value * MICROPY_FLOAT_CONST(0.11512925464970228420089957273422));
}

audioio_get_buffer_result_t audiofilters_distortion_get_buffer(audiofilters_distortion_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {

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
            (void)synthio_block_slot_get(&self->drive);
            (void)synthio_block_slot_get(&self->pre_gain);
            (void)synthio_block_slot_get(&self->post_gain);
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
            mp_float_t drive = synthio_block_slot_get_limited(&self->drive, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0));
            mp_float_t pre_gain = db_to_linear(synthio_block_slot_get_limited(&self->pre_gain, MICROPY_FLOAT_CONST(-60.0), MICROPY_FLOAT_CONST(60.0)));
            mp_float_t post_gain = db_to_linear(synthio_block_slot_get_limited(&self->post_gain, MICROPY_FLOAT_CONST(-80.0), MICROPY_FLOAT_CONST(24.0)));
            mp_float_t mix = synthio_block_slot_get_limited(&self->mix, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0));

            // Modify drive value depending on mode
            uint32_t word_mask = 0;
            if (self->mode == DISTORTION_MODE_CLIP) {
                drive = MICROPY_FLOAT_CONST(1.0001) - drive;
            } else if (self->mode == DISTORTION_MODE_WAVESHAPE) {
                drive = MICROPY_FLOAT_CONST(2.0) * drive / (MICROPY_FLOAT_CONST(1.0001) - drive);
            } else if (self->mode == DISTORTION_MODE_LOFI) {
                word_mask = 0xFFFFFFFF ^ ((1 << (uint32_t)MICROPY_FLOAT_C_FUN(round)(drive * MICROPY_FLOAT_CONST(14.0))) - 1);
            }

            if (mix <= MICROPY_FLOAT_CONST(0.01)) { // if mix is zero pure sample only
                for (uint32_t i = 0; i < n; i++) {
                    if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                        word_buffer[i] = sample_src[i];
                    } else {
                        hword_buffer[i] = sample_hsrc[i];
                    }
                }
            } else {
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

                    // Apply pre-gain
                    int32_t word = (int32_t)(sample_word * pre_gain);

                    // Apply bit mask before converting to float
                    if (self->mode == DISTORTION_MODE_LOFI) {
                        word = word & word_mask;
                    }

                    if (self->mode != DISTORTION_MODE_LOFI || self->soft_clip) {
                        // Convert sample to float
                        mp_float_t wordf = word / MICROPY_FLOAT_CONST(32768.0);

                        switch (self->mode) {
                            case DISTORTION_MODE_CLIP: {
                                wordf = MICROPY_FLOAT_C_FUN(pow)(MICROPY_FLOAT_C_FUN(fabs)(wordf), drive);
                                if (word < 0) {
                                    wordf *= MICROPY_FLOAT_CONST(-1.0);
                                }
                            } break;
                            case DISTORTION_MODE_LOFI:
                                break;
                            case DISTORTION_MODE_OVERDRIVE: {
                                wordf *= MICROPY_FLOAT_CONST(0.686306);
                                mp_float_t z = MICROPY_FLOAT_CONST(1.0) + MICROPY_FLOAT_C_FUN(exp)(MICROPY_FLOAT_C_FUN(sqrt)(MICROPY_FLOAT_C_FUN(fabs)(wordf)) * MICROPY_FLOAT_CONST(-0.75));
                                mp_float_t word_exp = MICROPY_FLOAT_C_FUN(exp)(wordf);
                                wordf *= MICROPY_FLOAT_CONST(-1.0);
                                wordf = (word_exp - MICROPY_FLOAT_C_FUN(exp)(wordf * z)) / (word_exp + MICROPY_FLOAT_C_FUN(exp)(wordf));
                            } break;
                            case DISTORTION_MODE_WAVESHAPE: {
                                wordf = (MICROPY_FLOAT_CONST(1.0) + drive) * wordf / (MICROPY_FLOAT_CONST(1.0) + drive * MICROPY_FLOAT_C_FUN(fabs)(wordf));
                            } break;
                        }

                        // Apply post-gain
                        wordf = wordf * post_gain;

                        // Soft clip
                        if (self->soft_clip) {
                            if (wordf > 0) {
                                wordf = MICROPY_FLOAT_CONST(1.0) - MICROPY_FLOAT_C_FUN(exp)(-wordf);
                            } else {
                                wordf = MICROPY_FLOAT_CONST(-1.0) + MICROPY_FLOAT_C_FUN(exp)(wordf);
                            }
                        }

                        // Convert sample back to signed integer
                        word = (int32_t)(wordf * MICROPY_FLOAT_CONST(32767.0));
                    } else {
                        // Apply post-gain
                        word = (int32_t)(word * post_gain);
                    }

                    // Hard clip
                    if (!self->soft_clip) {
                        word = MIN(MAX(word, -32767), 32768);
                    }

                    if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                        word_buffer[i] = (int16_t)((sample_word * (MICROPY_FLOAT_CONST(1.0) - mix)) + (word * mix));
                        if (!self->base.samples_signed) {
                            word_buffer[i] ^= 0x8000;
                        }
                    } else {
                        int8_t mixed = (int8_t)((sample_word * (MICROPY_FLOAT_CONST(1.0) - mix)) + (word * mix));
                        if (self->base.samples_signed) {
                            hword_buffer[i] = mixed;
                        } else {
                            hword_buffer[i] = (uint8_t)mixed ^ 0x80;
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

    // Distortion always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}
