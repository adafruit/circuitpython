
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/synthio/Biquad.h"
#include "shared-module/synthio/block.h"

#define BIQUAD_SHIFT (15)

typedef struct synthio_biquad {
    mp_obj_base_t base;
    synthio_filter_mode mode;
    synthio_block_slot_t f0, Q, A;
    mp_float_t cached_W0, cached_Q, cached_A;
    int32_t a1, a2, b0, b1, b2;
} synthio_biquad_t;

typedef struct {
    int32_t x[2], y[2];
} biquad_filter_state;

void common_hal_synthio_biquad_tick(mp_obj_t self_in);
void synthio_biquad_filter_reset(biquad_filter_state *st);
void synthio_biquad_filter_samples(mp_obj_t self_in, biquad_filter_state *st, int32_t *buffer, size_t n_samples);
int32_t synthio_biquad_filter_sample(mp_obj_t self_in, biquad_filter_state *st, int32_t input);
