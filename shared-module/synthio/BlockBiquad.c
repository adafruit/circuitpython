
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include "shared-bindings/synthio/BlockBiquad.h"
#include "shared-module/synthio/BlockBiquad.h"
#include "shared-module/synthio/block.h"

typedef struct {
    mp_float_t s, c;
} sincos_result_t;

#define FOUR_OVER_PI (4 / M_PI)
static void fast_sincos(mp_float_t theta, sincos_result_t *result) {
    mp_float_t x = (theta * FOUR_OVER_PI) - 1;
    mp_float_t x2 = x * x, x3 = x2 * x, x4 = x2 * x2, x5 = x2 * x3;
    mp_float_t c0 = 0.70708592,
               c1x = -0.55535724 * x,
               c2x2 = -0.21798592 * x2,
               c3x3 = 0.05707685 * x3,
               c4x4 = 0.0109 * x4,
               c5x5 = -0.00171961 * x5;

    mp_float_t evens = c4x4 + c2x2 + c0, odds = c5x5 + c3x3 + c1x;
    result->c = evens + odds;
    result->s = evens - odds;
}

mp_obj_t common_hal_synthio_block_biquad_new(synthio_filter_e kind, mp_obj_t f0, mp_obj_t Q) {
    synthio_block_biquad_t *self = mp_obj_malloc(synthio_block_biquad_t, &synthio_block_biquad_type_obj);
    self->kind = kind;
    synthio_block_assign_slot(f0, &self->f0, MP_QSTR_frequency);
    synthio_block_assign_slot(Q, &self->Q, MP_QSTR_q_factor);
    return MP_OBJ_FROM_PTR(self);
}

synthio_filter_e common_hal_synthio_block_biquad_get_kind(synthio_block_biquad_t *self) {
    return self->kind;
}

mp_obj_t common_hal_synthio_block_biquad_get_q_factor(synthio_block_biquad_t *self) {
    return self->Q.obj;
}

void common_hal_synthio_block_biquad_set_q_factor(synthio_block_biquad_t *self, mp_obj_t q_factor) {
    synthio_block_assign_slot(q_factor, &self->Q, MP_QSTR_q_factor);
}

mp_obj_t common_hal_synthio_block_biquad_get_frequency(synthio_block_biquad_t *self) {
    return self->f0.obj;
}

void common_hal_synthio_block_biquad_set_frequency(synthio_block_biquad_t *self, mp_obj_t frequency) {
    synthio_block_assign_slot(frequency, &self->f0, MP_QSTR_frequency);
}

static int32_t biquad_scale_arg_float(mp_float_t arg) {
    return (int32_t)MICROPY_FLOAT_C_FUN(round)(MICROPY_FLOAT_C_FUN(ldexp)(arg, BIQUAD_SHIFT));
}

void common_hal_synthio_block_biquad_tick(mp_obj_t self_in, biquad_filter_state *filter_state) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);

    mp_float_t W0 = synthio_block_slot_get(&self->f0) * synthio_global_W_scale;
    mp_float_t Q = synthio_block_slot_get(&self->Q);

    sincos_result_t sc;
    fast_sincos(W0, &sc);

    mp_float_t alpha = sc.s / (2 * Q);

    mp_float_t a0, a1, a2, b0, b1, b2;

    a0 = 1 + alpha;
    a1 = -2 * sc.c;
    a2 = 1 - alpha;

    switch (self->kind) {
        default:
        case SYNTHIO_LOW_PASS:
            b2 = b0 = (1 - sc.c) * .5;
            b1 = 1 - sc.c;
            break;

        case SYNTHIO_HIGH_PASS:
            b2 = b0 = (1 + sc.c) * .5;
            b1 = -(1 + sc.c);
            break;

        case SYNTHIO_BAND_PASS:
            b0 = alpha;
            b1 = 0;
            b2 = -b0;
    }

    mp_float_t recip_a0 = 1 / a0;

    filter_state->a1 = biquad_scale_arg_float(a1 * recip_a0);
    filter_state->a2 = biquad_scale_arg_float(a2 * recip_a0);
    filter_state->b0 = biquad_scale_arg_float(b0 * recip_a0);
    filter_state->b1 = biquad_scale_arg_float(b1 * recip_a0);
    filter_state->b2 = biquad_scale_arg_float(b2 * recip_a0);
}
