// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
// FIXME: Silabs includes "peripherals/periph.h. Figure out what's in this file. "
#include "py/obj.h"

// HAL-specific
#include "i2c.h"
#include "gpio.h"

// Define a struct for what BUSIO.I2C should carry
typedef struct {
    mp_obj_base_t base;
    mxc_i2c_regs_t *i2c_regs;
    bool has_lock;
    const mcu_pin_obj_t *scl;
    const mcu_pin_obj_t *sda;
} busio_i2c_obj_t;
