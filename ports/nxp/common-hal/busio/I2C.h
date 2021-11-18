/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#ifndef MICROPY_INCLUDED_NXP_COMMON_HAL_BUSIO_I2C_H
#define MICROPY_INCLUDED_NXP_COMMON_HAL_BUSIO_I2C_H

#include "cmsis5/CMSIS/Driver/Include/Driver_I2C.h"
#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"


typedef struct {
    uint8_t scl;
    uint8_t sda;
} i2c_pin_set_t;

typedef struct {
    const size_t id;
    bool is_used;
    ARM_DRIVER_I2C *driver;
    const i2c_pin_set_t *pin_map;
    const size_t pin_map_len;
} i2c_inst_t;

typedef struct {
    mp_obj_base_t base;
    i2c_inst_t *i2c_instance;
    bool has_lock;
    const mcu_pin_obj_t *scl;
    const mcu_pin_obj_t *sda;
    uint32_t frequency;
} busio_i2c_obj_t;

void reset_i2c(void);


#endif // MICROPY_INCLUDED_NXP_COMMON_HAL_BUSIO_I2C_H
