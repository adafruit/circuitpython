// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t device_address;
    uint8_t configuration_index; // not bConfigurationValue
    uint8_t *configuration_descriptor; // Contains the length of the all descriptors.
    uint8_t open_endpoints[8];
    // bInterfaceNumber of each open_endpoints[] entry, so deinit can leave alone the endpoints
    // of an interface the USB keyboard workflow owns again.
    uint8_t open_endpoint_interfaces[8];
    uint16_t first_langid;
} usb_core_device_obj_t;
