/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Elliot Buller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "shared-bindings/microcontroller/Pin.h"
#include "common-hal/rp2clock/InputPin.h"
#include "bindings/rp2clock/Index.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "py/runtime.h"

// GPIN must be [20,22]
void common_hal_rp2clock_inputpin_validate_index_pin(const mcu_pin_obj_t *pin) {
    if ((pin->number != 20) && (pin->number != 22)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Pin %d invalid, valid pins are: 20,22"), pin->number);
    }
}

void common_hal_rp2clock_inputpin_validate_freqs(uint32_t src, uint32_t target) {
    if (src == 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid freq: %u"), src);
    }
    if (target == 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid freq: %u"), target);
    }
    if (target > src) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid freqs: %u > %u"), target, src);
    }
}

bool common_hal_rp2clock_inputpin_deinited(rp2clock_inputpin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_rp2clock_inputpin_deinit(rp2clock_inputpin_obj_t *self) {
    if (common_hal_rp2clock_inputpin_deinited(self)) {
        return;
    }
    if (self->enabled) {
        common_hal_rp2clock_inputpin_disable(self);
    }
    self->pin = NULL;
}

static void common_hal_rp2clock_inputpin_claim_pin(rp2clock_inputpin_obj_t *self) {
    // Check pin is available
    assert_pin_free(self->pin);
    // Claim pin
    common_hal_mcu_pin_claim(self->pin);
}

void common_hal_rp2clock_inputpin_enable(rp2clock_inputpin_obj_t *self) {
    common_hal_rp2clock_inputpin_claim_pin(self);
    // Check return value
    if (!clock_configure_gpin(self->index, self->pin->number, self->src_freq, self->target_freq)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("clock_configure_gpin failed!"));
    }
    self->enabled = true;
}

void common_hal_rp2clock_inputpin_disable(rp2clock_inputpin_obj_t *self) {
    if (!self->enabled) {
        return;
    }
    common_hal_reset_pin(self->pin);
    self->enabled = false;
}
