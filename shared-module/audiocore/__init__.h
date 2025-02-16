// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"

typedef enum {
    GET_BUFFER_DONE,            // No more data to read
    GET_BUFFER_MORE_DATA,       // More data to read.
    GET_BUFFER_ERROR,           // Error while reading data.
} audioio_get_buffer_result_t;

typedef struct audiosample_base {
    mp_obj_base_t self;
    uint32_t sample_rate;
    uint32_t max_buffer_length;
    uint8_t bits_per_sample;
    uint8_t channel_count;
    uint8_t samples_signed;
    bool single_buffer;
} audiosample_base_t;

typedef void (*audiosample_reset_buffer_fun)(mp_obj_t,
    bool single_channel_output, uint8_t audio_channel);
typedef audioio_get_buffer_result_t (*audiosample_get_buffer_fun)(mp_obj_t,
    bool single_channel_output, uint8_t channel, uint8_t **buffer,
    uint32_t *buffer_length);

typedef struct _audiosample_p_t {
    MP_PROTOCOL_HEAD // MP_QSTR_protocol_audiosample
    audiosample_reset_buffer_fun reset_buffer;
    audiosample_get_buffer_fun get_buffer;
} audiosample_p_t;

static inline uint32_t audiosample_get_bits_per_sample(audiosample_base_t *self) {
    return self->bits_per_sample;
}

static inline uint32_t audiosample_get_sample_rate(audiosample_base_t *self) {
    return self->sample_rate;
}

static inline void audiosample_set_sample_rate(audiosample_base_t *self, uint32_t sample_rate) {
    self->sample_rate = sample_rate;
}

static inline uint8_t audiosample_get_channel_count(audiosample_base_t *self) {
    return self->channel_count;
}

void audiosample_reset_buffer(mp_obj_t sample_obj, bool single_channel_output, uint8_t audio_channel);
audioio_get_buffer_result_t audiosample_get_buffer(mp_obj_t sample_obj,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length);

static inline void audiosample_get_buffer_structure(audiosample_base_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing) {

    *single_buffer = self->single_buffer;
    *samples_signed = self->samples_signed;
    *max_buffer_length = self->max_buffer_length;

    if (single_channel_output) {
        *spacing = self->channel_count;
    } else {
        *spacing = 1;
    }
}

static inline audiosample_base_t *audiosample_check(mp_obj_t self_in) {
    // called for side effect
    (void)mp_proto_get_or_throw(MP_QSTR_protocol_audiosample, self_in);
    return MP_OBJ_TO_PTR(self_in);
}

static inline void audiosample_get_buffer_structure_checked(mp_obj_t self_in, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing) {
    audiosample_get_buffer_structure(audiosample_check(self_in), single_channel_output, single_buffer, samples_signed, max_buffer_length, spacing);
}

void audiosample_must_match(audiosample_base_t *self, mp_obj_t other);

void audiosample_convert_u8m_s16s(int16_t *buffer_out, const uint8_t *buffer_in, size_t nframes);
void audiosample_convert_u8s_s16s(int16_t *buffer_out, const uint8_t *buffer_in, size_t nframes);
void audiosample_convert_s8m_s16s(int16_t *buffer_out, const int8_t *buffer_in, size_t nframes);
void audiosample_convert_s8s_s16s(int16_t *buffer_out, const int8_t *buffer_in, size_t nframes);
void audiosample_convert_u16m_s16s(int16_t *buffer_out, const uint16_t *buffer_in, size_t nframes);
void audiosample_convert_u16s_s16s(int16_t *buffer_out, const uint16_t *buffer_in, size_t nframes);
void audiosample_convert_s16m_s16s(int16_t *buffer_out, const int16_t *buffer_in, size_t nframes);

void audiosample_convert_u8s_u8m(uint8_t *buffer_out, const uint8_t *buffer_in, size_t nframes);
void audiosample_convert_s8m_u8m(uint8_t *buffer_out, const int8_t *buffer_in, size_t nframes);
void audiosample_convert_s8s_u8m(uint8_t *buffer_out, const int8_t *buffer_in, size_t nframes);
void audiosample_convert_u16m_u8m(uint8_t *buffer_out, const uint16_t *buffer_in, size_t nframes);
void audiosample_convert_u16s_u8m(uint8_t *buffer_out, const uint16_t *buffer_in, size_t nframes);
void audiosample_convert_s16m_u8m(uint8_t *buffer_out, const int16_t *buffer_in, size_t nframes);
void audiosample_convert_s16s_u8m(uint8_t *buffer_out, const int16_t *buffer_in, size_t nframes);

void audiosample_convert_u8m_u8s(uint8_t *buffer_out, const uint8_t *buffer_in, size_t nframes);
void audiosample_convert_s8m_u8s(uint8_t *buffer_out, const int8_t *buffer_in, size_t nframes);
void audiosample_convert_s8s_u8s(uint8_t *buffer_out, const int8_t *buffer_in, size_t nframes);
void audiosample_convert_u16m_u8s(uint8_t *buffer_out, const uint16_t *buffer_in, size_t nframes);
void audiosample_convert_u16s_u8s(uint8_t *buffer_out, const uint16_t *buffer_in, size_t nframes);
void audiosample_convert_s16m_u8s(uint8_t *buffer_out, const int16_t *buffer_in, size_t nframes);
void audiosample_convert_s16s_u8s(uint8_t *buffer_out, const int16_t *buffer_in, size_t nframes);
