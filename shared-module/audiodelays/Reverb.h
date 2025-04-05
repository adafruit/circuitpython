// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Mark Komus
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"
#include "shared-module/synthio/__init__.h"
#include "shared-module/synthio/block.h"

extern const mp_obj_type_t audiodelays_reverb_type;

typedef struct {
    audiosample_base_t base;
    synthio_block_slot_t roomsize;
    synthio_block_slot_t damp;
    synthio_block_slot_t mix;

    int8_t *buffer[2];
    uint8_t last_buf_idx;
    uint32_t buffer_len; // max buffer in bytes

    uint8_t *sample_remaining_buffer;
    uint32_t sample_buffer_length;

    bool loop;
    bool more_data;

    int16_t combbuffersizes[16];
    int16_t *combbuffers[16];
    int16_t combbufferindex[16];
    int16_t combfitlers[16];

    int16_t allpassbuffersizes[8];
    int16_t *allpassbuffers[8];
    int16_t allpassbufferindex[8];

    mp_obj_t sample;
} audiodelays_reverb_obj_t;

void audiodelays_reverb_reset_buffer(audiodelays_reverb_obj_t *self,
    bool single_channel_output,
    uint8_t channel);

audioio_get_buffer_result_t audiodelays_reverb_get_buffer(audiodelays_reverb_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);  // length in bytes

int16_t audiodelays_reverb_get_roomsize_fixedpoint(mp_float_t n);
void audiodelays_reverb_get_damp_fixedpoint(mp_float_t n, int16_t *damp1, int16_t *damp2);
