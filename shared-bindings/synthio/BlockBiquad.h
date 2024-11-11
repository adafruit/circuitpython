// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

extern const mp_obj_type_t synthio_block_biquad_type_obj;
extern const mp_obj_type_t synthio_filter_mode_type;
typedef struct synthio_block_biquad synthio_block_biquad_t;

typedef enum {
    SYNTHIO_LOW_PASS, SYNTHIO_HIGH_PASS, SYNTHIO_BAND_PASS, SYNTHIO_NOTCH
} synthio_filter_mode;


mp_obj_t common_hal_synthio_block_biquad_get_Q(synthio_block_biquad_t *self);
void common_hal_synthio_block_biquad_set_Q(synthio_block_biquad_t *self, mp_obj_t Q);

mp_obj_t common_hal_synthio_block_biquad_get_frequency(synthio_block_biquad_t *self);
void common_hal_synthio_block_biquad_set_frequency(synthio_block_biquad_t *self, mp_obj_t frequency);

synthio_filter_mode common_hal_synthio_block_biquad_get_mode(synthio_block_biquad_t *self);

mp_obj_t common_hal_synthio_block_biquad_new(synthio_filter_mode mode);
