// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/digitalio/DigitalInOut.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t scl;
    mp_obj_t sda;
    uint32_t us_delay;
    uint32_t us_timeout;
    volatile bool locked;
    bool own_scl;
    bool own_sda;
} bitbangio_i2c_obj_t;
