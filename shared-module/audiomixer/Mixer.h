// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objtuple.h"

#include "shared-module/audiocore/__init__.h"

typedef struct {
    audiosample_base_t base;
    uint32_t *first_buffer;
    uint32_t *second_buffer;
    uint32_t len; // in words
    bool use_first_buffer;

    uint32_t read_count;
    uint32_t left_read_count;
    uint32_t right_read_count;

    uint8_t voice_count;
    mp_obj_tuple_t *voice_tuple;
    mp_obj_t voice[];
} audiomixer_mixer_obj_t;


// These are not available from Python because it may be called in an interrupt.
void audiomixer_mixer_reset_buffer(audiomixer_mixer_obj_t *self,
    bool single_channel_output,
    uint8_t channel);
audioio_get_buffer_result_t audiomixer_mixer_get_buffer(audiomixer_mixer_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);                                                      // length in bytes
