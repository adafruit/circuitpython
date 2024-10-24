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
#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "bindings/rp2clock/AuxSrc.h"
#include "hardware/clocks.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    rp2clock_auxsrc_t src;
    mp_float_t divisor;
    bool enabled;
} rp2clock_outputpin_obj_t;

void common_hal_rp2clock_outputpin_validate_src_pin(const mcu_pin_obj_t *pin);
bool common_hal_rp2clock_outputpin_deinited(rp2clock_outputpin_obj_t *self);
void common_hal_rp2clock_outputpin_deinit(rp2clock_outputpin_obj_t *self);
mp_float_t common_hal_rp2clock_outputpin_validate_divisor(mp_float_t div);

// Configure clock out
void common_hal_rp2clock_outputpin_enable(rp2clock_outputpin_obj_t *self);
void common_hal_rp2clock_outputpin_disable(rp2clock_outputpin_obj_t *self);
