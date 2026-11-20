// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objstr.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

extern mp_obj_str_t display_init_byte_obj, i2c_init_byte_obj;
extern framebufferio_framebufferdisplay_obj_t board_display;
