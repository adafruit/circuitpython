// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include "genhdr/mpversion.h"
#include "py/objstr.h"
#include "py/objtuple.h"

bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length) {
    uint32_t i = 0;

    while (i < length) {
        uint32_t new_random = rand();
        for (int j = 0; j < 4 && i < length; j++) {
            buffer[i] = new_random & 0xff;
            i++;
            new_random >>= 8;
        }
    }

    return true;
}
