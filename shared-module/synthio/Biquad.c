
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include "shared-bindings/synthio/Biquad.h"
#include "shared-module/synthio/Biquad.h"
#include "shared-module/synthio/block.h"
#include "shared-bindings/synthio/__init__.h"

typedef struct {
    mp_float_t s, c;
} sincos_result_t;

#include <stdint.h>  // uint32_t

// The famous Quake approximate square root function
static mp_float_t Q_rsqrt(mp_float_t number_in) {
    float number = (float)number_in;
    union {
        float f;
        uint32_t i;
    } conv = { .f = (float)number };
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);
    return (mp_float_t)conv.f;
}

static mp_float_t fast_sqrt(mp_float_t number) {
    return number * Q_rsqrt(number);
}

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

mp_obj_t common_hal_synthio_biquad_new(synthio_filter_mode mode) {
    synthio_biquad_t *self = mp_obj_malloc(synthio_biquad_t, &synthio_biquad_type_obj);
    self->mode = mode;
    return MP_OBJ_FROM_PTR(self);
}

synthio_filter_mode common_hal_synthio_biquad_get_mode(synthio_biquad_t *self) {
    return self->mode;
}

mp_obj_t common_hal_synthio_biquad_get_Q(synthio_biquad_t *self) {
    return self->Q.obj;
}

void common_hal_synthio_biquad_set_Q(synthio_biquad_t *self, mp_obj_t Q) {
    synthio_block_assign_slot(Q, &self->Q, MP_QSTR_Q);
}

mp_obj_t common_hal_synthio_biquad_get_A(synthio_biquad_t *self) {
    return self->A.obj;
}

void common_hal_synthio_biquad_set_A(synthio_biquad_t *self, mp_obj_t A) {
    synthio_block_assign_slot(A, &self->A, MP_QSTR_A);
}

mp_obj_t common_hal_synthio_biquad_get_frequency(synthio_biquad_t *self) {
    return self->f0.obj;
}

void common_hal_synthio_biquad_set_frequency(synthio_biquad_t *self, mp_obj_t frequency) {
    synthio_block_assign_slot(frequency, &self->f0, MP_QSTR_frequency);
}

static int32_t biquad_scale_arg_float(mp_float_t arg) {
    return (int32_t)MICROPY_FLOAT_C_FUN(round)(MICROPY_FLOAT_C_FUN(ldexp)(arg, BIQUAD_SHIFT));
}

static int float_equal_or_update(
    mp_float_t *cached,
    mp_float_t new) {
    // uses memcmp to avoid error about equality float comparison
    if (memcmp(cached, &new, sizeof(mp_float_t))) {
        *cached = new;
        return false;
    }
    return true;
}

void common_hal_synthio_biquad_tick(mp_obj_t self_in) {
    synthio_biquad_t *self = MP_OBJ_TO_PTR(self_in);

    mp_float_t W0 = synthio_block_slot_get(&self->f0) * synthio_global_W_scale;
    mp_float_t Q = synthio_block_slot_get(&self->Q);
    mp_float_t A =
        (self->mode >= SYNTHIO_PEAKING_EQ) ? synthio_block_slot_get(&self->A) : 0;

    // n.b., assumes that the `mode` field is read-only
    // n.b., use of `&` is deliberate, avoids short-circuiting behavior
    if (float_equal_or_update(&self->cached_W0, W0)
        & float_equal_or_update(&self->cached_Q, Q)
        & float_equal_or_update(&self->cached_A, A)) {
        return;
    }

    sincos_result_t sc;
    fast_sincos(W0, &sc);

    mp_float_t alpha = sc.s / (2 * Q);

    mp_float_t a0, a1, a2, b0, b1, b2;

    switch (self->mode) {
        default:
            a0 = 1 + alpha;
            a1 = -2 * sc.c;
            a2 = 1 - alpha;

            switch (self->mode) {
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
                    break;

                case SYNTHIO_NOTCH:
                    b0 = 1;
                    b1 = -2 * sc.c;
                    b2 = 1;
            }

            break;

        case SYNTHIO_PEAKING_EQ:
            b0 = 1 + alpha * A;
            b1 = -2 * sc.c;
            b2 = 1 + alpha * A;
            a0 = 1 + alpha / A;
            a1 = -2 * sc.c;
            a2 = 1 - alpha / A;
            break;

        case SYNTHIO_LOW_SHELF: {
            mp_float_t sqrt_A = fast_sqrt(A);
            b0 = A * ((A + 1) - (A - 1) * sc.c + 2 * sqrt_A * alpha);
            b1 = 2 * A * ((A - 1) - (A + 1) * sc.c);
            b2 = A * ((A + 1) - (A - 1) * sc.c - 2 * sqrt_A * alpha);
            a0 = (A + 1) + (A - 1) * sc.c + 2 * sqrt_A * alpha;
            a1 = -2 * ((A - 1) + (A + 1) * sc.c);
            a2 = (A + 1) + (A - 1) * sc.c - 2 * sqrt_A * alpha;
        }
        break;

        case SYNTHIO_HIGH_SHELF: {
            mp_float_t sqrt_A = fast_sqrt(A);
            b0 = A * ((A + 1) + (A - 1) * sc.c + 2 * sqrt_A * alpha);
            b1 = -2 * A * ((A - 1) + (A + 1) * sc.c);
            b2 = A * ((A + 1) + (A - 1) * sc.c - 2 * sqrt_A * alpha);
            a0 = (A + 1) - (A - 1) * sc.c + 2 * sqrt_A * alpha;
            a1 = 2 * ((A - 1) - (A + 1) * sc.c);
            a2 = (A + 1) - (A - 1) * sc.c - 2 * sqrt_A * alpha;
        }
        break;
    }
    mp_float_t recip_a0 = 1 / a0;

    self->a1 = biquad_scale_arg_float(a1 * recip_a0);
    self->a2 = biquad_scale_arg_float(a2 * recip_a0);
    self->b0 = biquad_scale_arg_float(b0 * recip_a0);
    self->b1 = biquad_scale_arg_float(b1 * recip_a0);
    self->b2 = biquad_scale_arg_float(b2 * recip_a0);
}

void synthio_biquad_filter_reset(biquad_filter_state *st) {
    memset(&st->x, 0, 4 * sizeof(int16_t));
}

void synthio_biquad_filter_samples(mp_obj_t self_in, biquad_filter_state *st, int32_t *buffer, size_t n_samples) {
    synthio_biquad_t *self = MP_OBJ_TO_PTR(self_in);

    int32_t a1 = self->a1;
    int32_t a2 = self->a2;
    int32_t b0 = self->b0;
    int32_t b1 = self->b1;
    int32_t b2 = self->b2;

    int32_t x0 = st->x[0];
    int32_t x1 = st->x[1];
    int32_t y0 = st->y[0];
    int32_t y1 = st->y[1];

    for (size_t n = n_samples; n; --n, ++buffer) {
        int32_t input = *buffer;
        int32_t output = synthio_sat16((b0 * input + b1 * x0 + b2 * x1 - a1 * y0 - a2 * y1 + (1 << (BIQUAD_SHIFT - 1))), BIQUAD_SHIFT);

        x1 = x0;
        x0 = input;
        y1 = y0;
        y0 = output;
        *buffer = output;
    }
    st->x[0] = x0;
    st->x[1] = x1;
    st->y[0] = y0;
    st->y[1] = y1;
}
