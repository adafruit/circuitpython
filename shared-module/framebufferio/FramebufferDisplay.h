// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/framebufferio/lvgl/FramebufferDisplay.h"
#else
#include "shared-module/framebufferio/legacy/FramebufferDisplay.h"
#endif
