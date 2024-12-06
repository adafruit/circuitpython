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
#include "common-hal/rp2clock/OutputPin.h"
#include "bindings/rp2clock/AuxSrc.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "py/runtime.h"

// GPOUT must be [21,23,24,25]
void common_hal_rp2clock_outputpin_validate_src_pin(const mcu_pin_obj_t *pin) {
    if ((pin->number != 21) && (pin->number != 23) &&
        (pin->number != 24) && (pin->number != 25)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Pin %d invalid, valid pins are: 21,23,24,25"), pin->number);
    }
}

mp_float_t common_hal_rp2clock_outputpin_validate_divisor(mp_float_t div) {
    if (mp_obj_float_binary_op(MP_BINARY_OP_EQUAL, div, mp_obj_new_float(1.0f)) ||
        (div >= 2.0f && div <= 16777215.0f)) {
        return div;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Invalid divisor: [1, 2-16777215]"));
    return 1.0f;
}

bool common_hal_rp2clock_outputpin_deinited(rp2clock_outputpin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_rp2clock_outputpin_deinit(rp2clock_outputpin_obj_t *self) {
    if (common_hal_rp2clock_outputpin_deinited(self)) {
        return;
    }
    if (self->enabled) {
        common_hal_rp2clock_outputpin_disable(self);
    }
    self->pin = NULL;
}

static void common_hal_rp2clock_outputpin_claim_pin(rp2clock_outputpin_obj_t *self) {
    // Check pin is available
    assert_pin_free(self->pin);
    // Claim pin
    common_hal_mcu_pin_claim(self->pin);
}

void common_hal_rp2clock_outputpin_enable(rp2clock_outputpin_obj_t *self) {
    common_hal_rp2clock_outputpin_claim_pin(self);
    clock_gpio_init(self->pin->number, self->src, self->divisor);
    self->enabled = true;
}

void common_hal_rp2clock_outputpin_disable(rp2clock_outputpin_obj_t *self) {
    if (!self->enabled) {
        return;
    }
    common_hal_reset_pin(self->pin);
    self->enabled = 0;
}
