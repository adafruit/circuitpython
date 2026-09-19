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

// I2C register compatibility macros for MAX32650
#define I2C_CTRL_REG(i2c)               ((i2c)->ctrl0)
#define I2C_STATUS_REG(i2c)             ((i2c)->stat)
#define I2C_INTFL0_REG(i2c)             ((i2c)->int_fl0)
#define I2C_FIFO_REG(i2c)               ((i2c)->fifo)

#define I2C_F_CTRL_MST_MODE             MXC_F_I2C_CTRL0_MST
#define I2C_F_STATUS_BUSY               MXC_F_I2C_STAT_BUSY
#define I2C_F_INTFL0_ADDR_ACK           MXC_F_I2C_INT_FL0_ADRACKI
#define I2C_F_INTFL0_ADDR_NACK_ERR      MXC_F_I2C_INT_FL0_ADRERI

int pinsToI2c(const mcu_pin_obj_t *sda, const mcu_pin_obj_t *scl);
