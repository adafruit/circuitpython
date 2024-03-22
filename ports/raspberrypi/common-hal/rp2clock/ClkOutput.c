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
#include "common-hal/rp2clock/ClkOutput.h"
#include "bindings/rp2clock/ClkAuxSrc.h"
#include "bindings/rp2clock/ClkIndex.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "py/runtime.h"

// GPOUT must be [21,23,24,25]
void common_hal_clkio_clkoutput_validate_clksrc_pin(const mcu_pin_obj_t *pin) {
    if ((pin->number != 21) && (pin->number != 23) &&
        (pin->number != 24) && (pin->number != 25)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Pin %d invalid, valid pins are: 21,23,24,25"), pin->number);
    }
}

mp_float_t common_hal_clkio_clkoutput_validate_divisor(mp_float_t div) {
    if (mp_obj_float_binary_op(MP_BINARY_OP_EQUAL, div, mp_obj_new_float(1.0f)) ||
        (div >= 2.0f && div <= 16777215.0f)) {
        return div;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Invalid divisor: [1, 2-16777215]"));
    return 1.0f;
}

bool common_hal_clkio_clkoutput_deinited(clkio_clkoutput_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_clkio_clkoutput_deinit(clkio_clkoutput_obj_t *self) {
    if (common_hal_clkio_clkoutput_deinited(self)) {
        return;
    }
    if (self->enabled) {
        common_hal_clkio_clkoutput_disable(self);
    }
    self->pin = NULL;
}

static void common_hal_clkio_clkoutput_claim_pin(clkio_clkoutput_obj_t *self) {
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

void common_hal_clkio_clkoutput_enable(clkio_clkoutput_obj_t *self) {
    if (self->clksrc == CLKAUXSRC_NONE) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q not set"), MP_QSTR_clksrc);
    }
    common_hal_clkio_clkoutput_claim_pin(self);
    clock_gpio_init(self->pin->number, self->clksrc, self->divisor);
}

void common_hal_clkio_clkoutput_disable(clkio_clkoutput_obj_t *self) {
    if (!self->enabled) {
        return;
    }
    common_hal_reset_pin(self->pin);
    self->enabled = 0;
}
