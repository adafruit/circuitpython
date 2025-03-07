// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

#include "i2c_regs.h"
#include "mxc_sys.h"
#include "i2c.h"
#include "peripherals/pins.h"

#define NUM_I2C 2

int pinsToI2c(const mcu_pin_obj_t *sda, const mcu_pin_obj_t *scl);
