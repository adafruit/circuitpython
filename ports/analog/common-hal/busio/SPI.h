// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_MAX32_COMMON_HAL_BUSIO_SPI_H
#define MICROPY_INCLUDED_MAX32_COMMON_HAL_BUSIO_SPI_H

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

// HAL-specific
#include "spi.h"

// Define a struct for what BUSIO.SPI should carry
typedef struct {
    mp_obj_base_t base;

    // Spi regs & pins
    int spi_id;
    mxc_spi_regs_t *spi_regs;
    const mcu_pin_obj_t *sck;
    const mcu_pin_obj_t *mosi;
    const mcu_pin_obj_t *miso;
    const mcu_pin_obj_t *nss;

    // Configuration
    uint32_t baudrate;
    uint16_t prescaler;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;

    // Synch data
    bool has_lock;
} busio_spi_obj_t;

void spi_reset(void);

#endif  // MICROPY_INCLUDED_MAX32_COMMON_HAL_BUSIO_SPI_H
