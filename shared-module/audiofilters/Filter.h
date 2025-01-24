// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/obj.h"

#include "shared-bindings/synthio/Biquad.h"
#include "shared-module/audiocore/__init__.h"
#include "shared-module/synthio/__init__.h"
#include "shared-module/synthio/block.h"
#include "shared-module/synthio/Biquad.h"

extern const mp_obj_type_t audiofilters_filter_type;

typedef struct {
    mp_obj_base_t base;
    mp_obj_t *filter;
    synthio_block_slot_t mix;

    size_t filter_states_len;
    biquad_filter_state *filter_states;

    uint8_t bits_per_sample;
    bool samples_signed;
    uint8_t channel_count;
    uint32_t sample_rate;

    int8_t *buffer[2];
    uint8_t last_buf_idx;
    uint32_t buffer_len; // max buffer in bytes

    uint8_t *sample_remaining_buffer;
    uint32_t sample_buffer_length;

    int32_t *filter_buffer;

    bool loop;
    bool more_data;

    mp_obj_t sample;
} audiofilters_filter_obj_t;

void reset_filter_states(audiofilters_filter_obj_t *self);

void audiofilters_filter_reset_buffer(audiofilters_filter_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t audiofilters_filter_get_buffer(audiofilters_filter_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);  // length in bytes

void audiofilters_filter_get_buffer_structure(audiofilters_filter_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing);
