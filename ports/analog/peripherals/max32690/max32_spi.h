// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

#include "spi_regs.h"
#include "mxc_sys.h"
#include "spi.h"
#include "peripherals/pins.h"

#define NUM_SPI 5

int pinsToSpi(const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso,
    const mcu_pin_obj_t *sck);
