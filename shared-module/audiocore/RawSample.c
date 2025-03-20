// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Tim Chinowsky
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/audiocore/__init__.h"

#include <stdint.h>

#include "shared-module/audiocore/RawSample.h"

void common_hal_audioio_rawsample_construct(audioio_rawsample_obj_t *self,
    uint8_t *buffer,
    uint32_t len,
    uint8_t bytes_per_sample,
    bool samples_signed,
    uint8_t channel_count,
    uint32_t sample_rate,
    bool single_buffer) {

    self->buffer = buffer;
    self->base.bits_per_sample = bytes_per_sample * 8;
    self->base.samples_signed = samples_signed;
    self->base.max_buffer_length = len;
    self->base.channel_count = channel_count;
    self->base.sample_rate = sample_rate;
    self->base.single_buffer = single_buffer;
    self->buffer_index = 0;
}

void common_hal_audioio_rawsample_deinit(audioio_rawsample_obj_t *self) {
    self->buffer = NULL;
    audiosample_mark_deinit(&self->base);
}

void audioio_rawsample_reset_buffer(audioio_rawsample_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {
}

audioio_get_buffer_result_t audioio_rawsample_get_buffer(audioio_rawsample_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length) {

    if (self->base.single_buffer) {
        *buffer_length = self->base.max_buffer_length;
        if (single_channel_output) {
            *buffer = self->buffer + (channel % self->base.channel_count) * (self->base.bits_per_sample / 8);
        } else {
            *buffer = self->buffer;
        }
        return GET_BUFFER_DONE;
    } else {
        *buffer_length = self->base.max_buffer_length / 2;
        if (single_channel_output) {
            *buffer = self->buffer + (channel % self->base.channel_count) * (self->base.bits_per_sample / 8) + \
                self->base.max_buffer_length / 2 * self->buffer_index;
        } else {
            *buffer = self->buffer + self->base.max_buffer_length / 2 * self->buffer_index;
        }
        self->buffer_index = 1 - self->buffer_index;
        return GET_BUFFER_DONE;
    }
}
