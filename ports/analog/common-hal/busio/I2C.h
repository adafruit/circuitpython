// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

// HAL-specific
#include "i2c.h"
#include "gpio.h"

// Define a struct for what BUSIO.I2C should carry
typedef struct {
    mp_obj_base_t base;

    int i2c_id;
    mxc_i2c_regs_t *i2c_regs;
    const mcu_pin_obj_t *scl;
    const mcu_pin_obj_t *sda;
    const int frequency;

    uint32_t timeout;
    bool has_lock;
} busio_i2c_obj_t;
