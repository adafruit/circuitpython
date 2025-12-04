// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/zephyr_i2c/I2C.h"

extern const mp_obj_type_t zephyr_i2c_i2c_type;

// Check if the I2C object has been deinitialized
bool common_hal_zephyr_i2c_i2c_deinited(zephyr_i2c_i2c_obj_t *self);

// Deinitialize the I2C bus
void common_hal_zephyr_i2c_i2c_deinit(zephyr_i2c_i2c_obj_t *self);

// Locking functions
bool common_hal_zephyr_i2c_i2c_try_lock(zephyr_i2c_i2c_obj_t *self);
bool common_hal_zephyr_i2c_i2c_has_lock(zephyr_i2c_i2c_obj_t *self);
void common_hal_zephyr_i2c_i2c_unlock(zephyr_i2c_i2c_obj_t *self);

// Device communication functions
bool common_hal_zephyr_i2c_i2c_probe(zephyr_i2c_i2c_obj_t *self, uint8_t addr);
uint8_t common_hal_zephyr_i2c_i2c_write(zephyr_i2c_i2c_obj_t *self, uint16_t address,
    const uint8_t *data, size_t len);
uint8_t common_hal_zephyr_i2c_i2c_read(zephyr_i2c_i2c_obj_t *self, uint16_t address,
    uint8_t *data, size_t len);
uint8_t common_hal_zephyr_i2c_i2c_write_read(zephyr_i2c_i2c_obj_t *self, uint16_t address,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len);
