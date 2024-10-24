// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "components/hal/include/hal/i2c_types.h"
#include "FreeRTOS.h"
#include "freertos/semphr.h"
#include "py/obj.h"

#include "driver/i2c_master.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *scl_pin;
    const mcu_pin_obj_t *sda_pin;
    size_t timeout_ms;
    size_t frequency;
    i2c_master_bus_handle_t handle;
    SemaphoreHandle_t xSemaphore;
    bool has_lock;
} busio_i2c_obj_t;
