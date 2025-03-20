// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"

#include "shared-bindings/os/__init__.h"

#include <zephyr/random/random.h>

bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length) {
    #if !DT_HAS_CHOSEN(zephyr_entropy)
    return false;
    #else
    return sys_csrand_get(buffer, length) == 0;
    #endif
}
