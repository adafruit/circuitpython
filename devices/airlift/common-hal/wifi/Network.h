// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t ssid;
    mp_obj_t bssid;
    mp_obj_t rssi;
    mp_obj_t channel;
    mp_obj_t country;
    mp_obj_t authmode;
} wifi_network_obj_t;

mp_obj_t esp_authmode_to_wifi_authmode_tuple(uint8_t esp_authmode);
