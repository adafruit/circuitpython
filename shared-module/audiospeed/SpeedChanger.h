// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/audiocore/__init__.h"

// Fixed-point 16.16 format
#define SPEED_SHIFT 16

typedef struct {
    audiosample_base_t base;
    mp_obj_t source;
    uint8_t *output_buffer;
    uint32_t output_buffer_length; // in bytes, allocated size
    // Source buffer cache
    uint8_t *src_buffer;
    uint32_t src_buffer_length; // in bytes
    uint32_t src_sample_count;  // in frames
    // Phase accumulator and rate in 16.16 fixed-point (units: source frames)
    uint32_t phase;
    uint32_t rate_fp; // 16.16 fixed-point rate
    bool source_done;  // source returned DONE on last get_buffer
    bool source_exhausted; // source DONE and we consumed all of it
} audiospeed_speedchanger_obj_t;

void audiospeed_speedchanger_reset_buffer(audiospeed_speedchanger_obj_t *self,
    bool single_channel_output, uint8_t channel);
audioio_get_buffer_result_t audiospeed_speedchanger_get_buffer(audiospeed_speedchanger_obj_t *self,
    bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length);
