// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-bindings/wifi/Radio.h"

typedef struct {
    mp_obj_base_t base;
    wifi_radio_obj_t *radio;
} socketpool_socketpool_obj_t;

bool socketpool_gethostbyname_ipv4(socketpool_socketpool_obj_t *self, const char *host, uint8_t ipv4[IPV4_LENGTH]);
