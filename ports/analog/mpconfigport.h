// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2015 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

// 24KiB stack
#define CIRCUITPY_DEFAULT_STACK_SIZE            0x6000

#define CIRCUITPY_USB_DEVICE_HIGH_SPEED (1)

// Also includes mpconfigboard.h
#include "py/circuitpy_mpconfig.h"

// Board flags:
#ifndef BOARD_OVERWRITE_SWD
#define BOARD_OVERWRITE_SWD (0)
#endif
#ifndef BOARD_VTOR_DEFER
#define BOARD_VTOR_DEFER (0)
#endif
#ifndef BOARD_NO_VBUS_SENSE
#define BOARD_NO_VBUS_SENSE (0)
#endif
#ifndef BOARD_NO_USB_OTG_ID_SENSE
#define BOARD_NO_USB_OTG_ID_SENSE (0)
#endif
