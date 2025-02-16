
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/synthio/BlockBiquad.h"
#include "shared-module/synthio/Biquad.h"
#include "shared-module/synthio/block.h"

typedef struct synthio_block_biquad {
    mp_obj_base_t base;
    synthio_filter_mode mode;
    synthio_block_slot_t f0, Q, A;
    mp_float_t cached_W0, cached_Q, cached_A;
} synthio_block_biquad_t;

void common_hal_synthio_block_biquad_tick(mp_obj_t self_in, biquad_filter_state *filter_state);
