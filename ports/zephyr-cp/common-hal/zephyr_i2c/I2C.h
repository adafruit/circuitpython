// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include <zephyr/kernel.h>

typedef struct {
    mp_obj_base_t base;
    const struct device *i2c_device;
    struct k_mutex mutex;
    bool has_lock;
} zephyr_i2c_i2c_obj_t;

mp_obj_t zephyr_i2c_i2c_zephyr_init(zephyr_i2c_i2c_obj_t *self, const struct device *i2c_device);
