// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

#include "hardware/spi.h"

#if CIRCUITPY_WIZNET_W6300

typedef struct {
    mp_obj_base_t base;
    bool has_lock;
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *quad_io0;
    const mcu_pin_obj_t *quad_io1;
    const mcu_pin_obj_t *quad_io2;
    const mcu_pin_obj_t *quad_io3;
} wiznet_pio_spi_obj_t;

#else // W55RP20

typedef struct {
    mp_obj_base_t base;
    bool has_lock;
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *MOSI;
    const mcu_pin_obj_t *MISO;
} wiznet_pio_spi_obj_t;
#endif

void reset_spi(void);
