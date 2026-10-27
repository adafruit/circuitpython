// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// The Zephyr display object layout and API are the same for both compositors; only the
// implementation differs.
#if CIRCUITPY_DISPLAYIO_LVGL
#include "common-hal/zephyr_display/lvgl/Display.h"
#else
#include "common-hal/zephyr_display/legacy/Display.h"
#endif
