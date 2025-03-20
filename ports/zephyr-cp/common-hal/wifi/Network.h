// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include <zephyr/net/wifi_mgmt.h>

typedef struct {
    mp_obj_base_t base;
    struct wifi_scan_result scan_result;
} wifi_network_obj_t;
