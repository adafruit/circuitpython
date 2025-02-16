// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // twim_peripheral_t *twim_peripheral;
    bool has_lock;
    uint8_t scl_pin_number;
    uint8_t sda_pin_number;
} busio_i2c_obj_t;
