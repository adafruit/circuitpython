// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

#include "hardware/spi.h"

#define SPI_HEADER_LEN 3

typedef struct {
    mp_obj_base_t base;
    bool has_lock;
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *MOSI;
    const mcu_pin_obj_t *MISO;
} wiznet_pio_spi_obj_t;

void reset_spi(void);