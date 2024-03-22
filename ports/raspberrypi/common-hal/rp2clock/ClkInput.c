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
#include "common-hal/rp2clock/ClkInput.h"
#include "bindings/rp2clock/ClkIndex.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "py/runtime.h"

// GPIN must be [20,22]
void common_hal_clkio_clkinput_validate_clkindex_pin(const mcu_pin_obj_t *pin) {
    if ((pin->number != 20) && (pin->number != 22)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Pin %d invalid, valid pins are: 20,22"), pin->number);
    }
}

void common_hal_clkio_clkinput_validate_freqs(uint32_t src, uint32_t target) {
    if (src == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("src freq == 0"));
    }
    if (target == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("target freq == 0"));
    }
    if (target > src) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid freqs: %u > %u"), target, src);
    }
}

bool common_hal_clkio_clkinput_deinited(clkio_clkinput_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_clkio_clkinput_deinit(clkio_clkinput_obj_t *self) {
    if (common_hal_clkio_clkinput_deinited(self)) {
        return;
    }
    if (self->enabled) {
        common_hal_clkio_clkinput_disable(self);
    }
    self->pin = NULL;
}

static void common_hal_clkio_clkinput_claim_pin(clkio_clkinput_obj_t *self) {
    // Avoid runtime error if enable already called
    if (self->enabled) {
        return;
    }
    // Check pin is available
    if (!common_hal_mcu_pin_is_free(self->pin)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Pin in use"));
    }
    // Claim pin
    common_hal_mcu_pin_claim(self->pin);
    // Store flag
    self->enabled = true;
}

void common_hal_clkio_clkinput_enable(clkio_clkinput_obj_t *self) {
    if (self->clkindex == CLKINDEX_NONE) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q not set"), MP_QSTR_clkindex);
    }
    common_hal_clkio_clkinput_claim_pin(self);
    // Check return value
    if (!clock_configure_gpin(self->clkindex, self->pin->number, self->src_freq, self->target_freq)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("clock_configure_gpin failed!"));
    }
}

void common_hal_clkio_clkinput_disable(clkio_clkinput_obj_t *self) {
    if (!self->enabled) {
        return;
    }
    common_hal_reset_pin(self->pin);
    self->enabled = 0;
}
