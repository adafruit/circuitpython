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
#include "bindings/rp2clock/Index.h"
#include "hardware/clocks.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    rp2clock_index_t index;
    uint32_t src_freq;
    uint32_t target_freq;
    bool enabled;
} rp2clock_inputpin_obj_t;

void common_hal_rp2clock_inputpin_validate_index_pin(const mcu_pin_obj_t *pin);
bool common_hal_rp2clock_inputpin_deinited(rp2clock_inputpin_obj_t *self);
void common_hal_rp2clock_inputpin_deinit(rp2clock_inputpin_obj_t *self);
void common_hal_rp2clock_inputpin_validate_freqs(uint32_t src, uint32_t target);

// Configure clock in/out
void common_hal_rp2clock_inputpin_enable(rp2clock_inputpin_obj_t *self);
void common_hal_rp2clock_inputpin_disable(rp2clock_inputpin_obj_t *self);
