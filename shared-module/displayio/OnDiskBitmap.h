// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// The BMP parser and pixel access have separate legacy and LVGL implementations. The LVGL
// version also keeps decoder state for streaming pixels from the file.
#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/displayio/lvgl/OnDiskBitmap.h"
#else
#include "shared-module/displayio/legacy/OnDiskBitmap.h"
#endif
