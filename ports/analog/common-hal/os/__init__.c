// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2025 Peggy Zhu, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"

#include "py/mperrno.h"
#include "py/runtime.h"

// #include "peripherals/periph.h"

// true random number generator, TRNG
#include "trng.h"

bool common_hal_os_urandom(uint8_t *buffer, uint32_t length) {
    #if (HAS_TRNG)
    // get a random number of "length" number of bytes
    MXC_TRNG_Random(buffer, length);
    return true;
    #else
    #endif
    return false;
}
