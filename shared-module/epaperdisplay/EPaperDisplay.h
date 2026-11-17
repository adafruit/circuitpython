// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// The EPaperDisplay implementation has two backends selected at build time with
// CIRCUITPY_DISPLAYIO_LVGL:
//
//   legacy/ - renders with the scanline compositor through display_core.
//   lvgl/   - renders with LVGL, then converts the composed RGB pixels into the
//             packed pixel planes the panel understands. The converted planes are
//             kept in a framebuffer of their own so partial updates and the fast
//             lag drive do not have to read pixels back from the panel.
#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/epaperdisplay/lvgl/EPaperDisplay.h"
#else
#include "shared-module/epaperdisplay/legacy/EPaperDisplay.h"
#endif
