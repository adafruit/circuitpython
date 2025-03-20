// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"
#include "shared-module/synthio/__init__.h"
#include "shared-module/synthio/block.h"

#define PITCH_READ_SHIFT (8)

extern const mp_obj_type_t audiodelays_pitch_shift_type;

typedef struct {
    audiosample_base_t base;
    synthio_block_slot_t semitones;
    mp_float_t current_semitones;
    synthio_block_slot_t mix;
    uint32_t window_len;
    uint32_t overlap_len;

    int8_t *buffer[2];
    uint8_t last_buf_idx;
    uint32_t buffer_len; // max buffer in bytes

    uint8_t *sample_remaining_buffer;
    uint32_t sample_buffer_length;

    bool loop;
    bool more_data;

    int8_t *window_buffer;
    uint32_t window_index; // words

    int8_t *overlap_buffer;
    uint32_t overlap_index; // words

    uint32_t read_index; // words << PITCH_READ_SHIFT
    uint32_t read_rate; // words << PITCH_READ_SHIFT

    mp_obj_t sample;
} audiodelays_pitch_shift_obj_t;

void recalculate_rate(audiodelays_pitch_shift_obj_t *self, mp_float_t semitones);

void audiodelays_pitch_shift_reset_buffer(audiodelays_pitch_shift_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t audiodelays_pitch_shift_get_buffer(audiodelays_pitch_shift_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);  // length in bytes
