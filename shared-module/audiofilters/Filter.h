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
    audiosample_base_t base;
    mp_obj_t filter;
    synthio_block_slot_t mix;

    mp_obj_t *filter_objs;
    size_t filter_states_len;
    biquad_filter_state *filter_states;

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

void audiofilters_filter_reset_buffer(audiofilters_filter_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t audiofilters_filter_get_buffer(audiofilters_filter_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);  // length in bytes
