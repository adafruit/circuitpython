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
#include "bindings/clkio/ClkIndex.h"
#include "hardware/clocks.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    clkio_clkindex_t clkindex;
    uint32_t src_freq;
    uint32_t target_freq;
    bool enabled;
} clkio_clkinput_obj_t;

void common_hal_clkio_clkinput_validate_clkindex_pin(const mcu_pin_obj_t *pin);
bool common_hal_clkio_clkinput_deinited(clkio_clkinput_obj_t *self);
void common_hal_clkio_clkinput_deinit(clkio_clkinput_obj_t *self);
void common_hal_clkio_clkinput_validate_freqs(uint32_t src, uint32_t target);

// Configure clock in/out
void common_hal_clkio_clkinput_enable(clkio_clkinput_obj_t *self);
void common_hal_clkio_clkinput_disable(clkio_clkinput_obj_t *self);
