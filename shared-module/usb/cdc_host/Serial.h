// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: 2025 rianadon
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    mp_float_t timeout;        // Read timeout (s). <0 means block forever
    mp_float_t write_timeout;  // Write timeout (s). <0 means block forever
    uint8_t idx;               // TinyUSB CDC interface index
} usb_cdc_host_serial_obj_t;
