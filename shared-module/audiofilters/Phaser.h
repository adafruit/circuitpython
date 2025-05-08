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

extern const mp_obj_type_t audiofilters_phaser_type;

typedef struct {
    audiosample_base_t base;
    synthio_block_slot_t frequency;
    synthio_block_slot_t feedback;
    synthio_block_slot_t mix;
    uint8_t stages;

    mp_float_t nyquist;

    int8_t *buffer[2];
    uint8_t last_buf_idx;
    uint32_t buffer_len; // max buffer in bytes

    uint8_t *sample_remaining_buffer;
    uint32_t sample_buffer_length;

    bool loop;
    bool more_data;

    int16_t *allpass_buffer;
    int16_t *word_buffer;

    mp_obj_t sample;
} audiofilters_phaser_obj_t;

void audiofilters_phaser_reset_buffer(audiofilters_phaser_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t audiofilters_phaser_get_buffer(audiofilters_phaser_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);  // length in bytes
