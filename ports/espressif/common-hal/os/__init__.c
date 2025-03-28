// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Sean Cross
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/qstr.h"

#include "shared-bindings/os/__init__.h"

#include "esp_system.h"
#include "esp_random.h"

bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length) {
    uint32_t i = 0;
    while (i < length) {
        uint32_t new_random = esp_random();
        for (int j = 0; j < 4 && i < length; j++) {
            buffer[i] = new_random & 0xff;
            i++;
            new_random >>= 8;
        }
    }

    return true;
}
