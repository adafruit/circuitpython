// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "common-hal/busio/SPI.h"
#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-module/displayio/Group.h"

typedef struct {
    mp_obj_base_t base;
    busio_spi_obj_t *bus;
    busio_spi_obj_t inline_bus;
    mp_obj_t command;
    mp_obj_t chip_select;
    mp_obj_t reset;
    uint32_t frequency;
    uint8_t polarity;
    uint8_t phase;
    bool own_command;
    bool own_chip_select;
    bool own_reset;
    #if CIRCUITPY_BUSIO_SPI_ASYNC
    circuitpy_async_flag_t sent;   // set once the last send_async has been sent
    #endif
} fourwire_fourwire_obj_t;
