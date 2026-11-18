// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/framebufferio/__init__.h"

typedef struct usb_video_uvcframebuffer_obj {
    mp_obj_base_t base;
    framebuffer_frame_callback_t frame_cb;
    void *frame_cb_data;
} usb_video_uvcframebuffer_obj_t;
