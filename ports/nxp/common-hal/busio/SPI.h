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

#ifndef MICROPY_INCLUDED_NXP_COMMON_HAL_BUSIO_SPI_H
#define MICROPY_INCLUDED_NXP_COMMON_HAL_BUSIO_SPI_H

#include "cmsis5/CMSIS/Driver/Include/Driver_SPI.h"
#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"


typedef struct {
    uint8_t clock;
    uint8_t mosi;
    uint8_t miso;
} spi_pin_set_t;

typedef struct {
    const size_t id;
    bool is_used;
    ARM_DRIVER_SPI *driver;
    const spi_pin_set_t *pin_map;
    const size_t pin_map_len;
} spi_inst_t;

typedef struct {
    mp_obj_base_t base;
    spi_inst_t *spi_instance;
    bool has_lock;
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *mosi;
    const mcu_pin_obj_t *miso;
    uint32_t target_baudrate;
    uint32_t real_baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
} busio_spi_obj_t;

void reset_spi(void);


#endif // MICROPY_INCLUDED_NXP_COMMON_HAL_BUSIO_SPI_H
