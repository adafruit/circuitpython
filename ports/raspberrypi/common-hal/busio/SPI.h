// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

#include "hardware/spi.h"

typedef struct {
    mp_obj_base_t base;
    spi_inst_t *peripheral;
    bool has_lock;
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *MOSI;
    const mcu_pin_obj_t *MISO;
    uint32_t target_frequency;
    int32_t real_frequency;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
} busio_spi_obj_t;

typedef struct spi_transfer_state spi_transfer_state;

spi_transfer_state *common_hal_busio_spi_start_transfer(busio_spi_obj_t *spi, const uint8_t *out_data, uint8_t *in_data, size_t len);
bool common_hal_busio_spi_transfer_isbusy(spi_transfer_state *state);
