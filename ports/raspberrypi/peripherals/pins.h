/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

// DO NOT include this file directly. Use shared-bindings/microcontroller/Pin.h instead to ensure
// that all necessary includes are already included.

#ifndef MICROPY_INCLUDED_RASPBERRYPI_PERIPHERALS_PINS_H
#define MICROPY_INCLUDED_RASPBERRYPI_PERIPHERALS_PINS_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t number;
} mcu_pin_obj_t;

extern const mcu_pin_obj_t pin_GPIO0;
extern const mcu_pin_obj_t pin_GPIO1;
extern const mcu_pin_obj_t pin_GPIO2;
extern const mcu_pin_obj_t pin_GPIO3;
extern const mcu_pin_obj_t pin_GPIO4;
extern const mcu_pin_obj_t pin_GPIO5;
extern const mcu_pin_obj_t pin_GPIO6;
extern const mcu_pin_obj_t pin_GPIO7;
extern const mcu_pin_obj_t pin_GPIO8;
extern const mcu_pin_obj_t pin_GPIO9;
extern const mcu_pin_obj_t pin_GPIO10;
extern const mcu_pin_obj_t pin_GPIO11;
extern const mcu_pin_obj_t pin_GPIO12;
extern const mcu_pin_obj_t pin_GPIO13;
extern const mcu_pin_obj_t pin_GPIO14;
extern const mcu_pin_obj_t pin_GPIO15;
extern const mcu_pin_obj_t pin_GPIO16;
extern const mcu_pin_obj_t pin_GPIO17;
extern const mcu_pin_obj_t pin_GPIO18;
extern const mcu_pin_obj_t pin_GPIO19;
extern const mcu_pin_obj_t pin_GPIO20;
extern const mcu_pin_obj_t pin_GPIO21;
extern const mcu_pin_obj_t pin_GPIO22;
extern const mcu_pin_obj_t pin_GPIO23;
extern const mcu_pin_obj_t pin_GPIO24;
#if !defined(IGNORE_GPIO25)
extern const mcu_pin_obj_t pin_GPIO25;
#endif
extern const mcu_pin_obj_t pin_GPIO26;
extern const mcu_pin_obj_t pin_GPIO27;
extern const mcu_pin_obj_t pin_GPIO28;
extern const mcu_pin_obj_t pin_GPIO29;

#if CIRCUITPY_CYW43
extern const mcu_pin_obj_t pin_CYW0;
extern const mcu_pin_obj_t pin_CYW1;
extern const mcu_pin_obj_t pin_CYW2;
#endif
#if CIRCUITPY_TCA9555R
extern const mcu_pin_obj_t pin_TCA0_0;
extern const mcu_pin_obj_t pin_TCA0_1;
extern const mcu_pin_obj_t pin_TCA0_2;
extern const mcu_pin_obj_t pin_TCA0_3;
extern const mcu_pin_obj_t pin_TCA0_4;
extern const mcu_pin_obj_t pin_TCA0_5;
extern const mcu_pin_obj_t pin_TCA0_6;
extern const mcu_pin_obj_t pin_TCA0_7;
extern const mcu_pin_obj_t pin_TCA0_8;
extern const mcu_pin_obj_t pin_TCA0_9;
extern const mcu_pin_obj_t pin_TCA0_10;
extern const mcu_pin_obj_t pin_TCA0_11;
extern const mcu_pin_obj_t pin_TCA0_12;
extern const mcu_pin_obj_t pin_TCA0_13;
extern const mcu_pin_obj_t pin_TCA0_14;
extern const mcu_pin_obj_t pin_TCA0_15;

extern const mcu_pin_obj_t pin_TCA1_0;
extern const mcu_pin_obj_t pin_TCA1_1;
extern const mcu_pin_obj_t pin_TCA1_2;
extern const mcu_pin_obj_t pin_TCA1_3;
extern const mcu_pin_obj_t pin_TCA1_4;
extern const mcu_pin_obj_t pin_TCA1_5;
extern const mcu_pin_obj_t pin_TCA1_6;
extern const mcu_pin_obj_t pin_TCA1_7;
extern const mcu_pin_obj_t pin_TCA1_8;
extern const mcu_pin_obj_t pin_TCA1_9;
extern const mcu_pin_obj_t pin_TCA1_10;
extern const mcu_pin_obj_t pin_TCA1_11;
extern const mcu_pin_obj_t pin_TCA1_12;
extern const mcu_pin_obj_t pin_TCA1_13;
extern const mcu_pin_obj_t pin_TCA1_14;
extern const mcu_pin_obj_t pin_TCA1_15;
#endif

#endif  // MICROPY_INCLUDED_RASPBERRYPI_PERIPHERALS_PINS_H
