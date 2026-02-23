// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2015 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// 24kiB stack
#define CIRCUITPY_DEFAULT_STACK_SIZE            (24 * 1024)

#define MICROPY_PY_SYS_PLATFORM "Zephyr"

#define CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL (1)
#define DIGITALINOUT_INVALID_DRIVE_MODE (1)

#define CIRCUITPY_DEBUG_TINYUSB 0

// Disable native _Float16 handling for host builds.
#define MICROPY_FLOAT_USE_NATIVE_FLT16 (0)

////////////////////////////////////////////////////////////////////////////////////////////////////

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"
