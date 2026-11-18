// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

// The pixel storage format (uint32_t word rows, MSB-first sub-byte values) and the
// higher-level dirty API are shared by both compositing backends; the dirty-tracking
// fields and scanline refresh API differ. See legacy/Bitmap.h and lvgl/Bitmap.h.

#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/displayio/lvgl/Bitmap.h"
#else
#include "shared-module/displayio/legacy/Bitmap.h"
#endif
