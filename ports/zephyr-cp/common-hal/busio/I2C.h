// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include <zephyr/kernel.h>

typedef struct {
    mp_obj_base_t base;
    const struct device *i2c_device;
    struct k_mutex mutex;
    bool has_lock;
    // True when the underlying Zephyr device was dynamically routed to the
    // pins below at construction time. Such objects deinitialize the device
    // and release their pins.
    bool dynamic;
    const mcu_pin_obj_t *sda;
    const mcu_pin_obj_t *scl;
} busio_i2c_obj_t;

// Helper function to construct from Zephyr device tree device
mp_obj_t common_hal_busio_i2c_construct_from_device(busio_i2c_obj_t *self, const struct device *i2c_device);
