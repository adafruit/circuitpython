// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/Bitmap.h"
bool shared_module_usb_video_enable(mp_int_t frame_width, mp_int_t frame_height, mp_int_t frame_rate);
bool shared_module_usb_video_disable(void);
void shared_module_usb_video_swapbuffers(void);
void shared_module_usb_video_swapbuffers_dirty(const uint8_t *dirty_row_bitmap);
