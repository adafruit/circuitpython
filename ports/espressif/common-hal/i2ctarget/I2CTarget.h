// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "driver/i2c_slave.h"
#include "common-hal/microcontroller/Pin.h"

#define I2CTARGET_RECV_BUF_SIZE 128

typedef struct {
    mp_obj_base_t base;
    i2c_slave_dev_handle_t handle;
    uint8_t *addresses;
    uint8_t num_addresses;
    const mcu_pin_obj_t *scl_pin;
    const mcu_pin_obj_t *sda_pin;
    uint8_t recv_buf[I2CTARGET_RECV_BUF_SIZE];
    volatile uint16_t recv_head;
    volatile uint16_t recv_tail;
} i2ctarget_i2c_target_obj_t;
