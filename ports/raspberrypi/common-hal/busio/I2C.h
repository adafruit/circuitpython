// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/bitbangio/I2C.h"

#include "py/obj.h"

#include "hardware/i2c.h"

typedef struct {
    mp_obj_base_t base;
    i2c_inst_t *peripheral;
    bitbangio_i2c_obj_t bitbangio_i2c;
    bool has_lock;
    uint baudrate;
    uint8_t scl_pin;
    uint8_t sda_pin;
} busio_i2c_obj_t;

typedef struct i2c_transfer_state i2c_transfer_state;

i2c_transfer_state *common_hal_busio_i2c_start_read(busio_i2c_obj_t *i2c, uint8_t address, uint8_t *data, size_t len, bool nostop);
i2c_transfer_state *common_hal_busio_i2c_start_write(busio_i2c_obj_t *i2c, uint8_t address, const uint8_t *data, size_t len, bool nostop);
bool common_hal_busio_i2c_read_isbusy(i2c_transfer_state *state);
bool common_hal_busio_i2c_write_isbusy(i2c_transfer_state *state);
