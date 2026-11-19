// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/framebufferio/__init__.h"

typedef struct usb_video_uvcframebuffer_obj usb_video_uvcframebuffer_obj_t;

extern const mp_obj_type_t usb_video_USBFramebuffer_type;
extern usb_video_uvcframebuffer_obj_t usb_video_uvcframebuffer_singleton_obj;

void shared_module_usb_video_uvcframebuffer_get_bufinfo(usb_video_uvcframebuffer_obj_t *self, mp_buffer_info_t *bufinfo);
void shared_module_usb_video_uvcframebuffer_refresh(usb_video_uvcframebuffer_obj_t *self);
void shared_module_usb_video_uvcframebuffer_swapbuffers(usb_video_uvcframebuffer_obj_t *self, const uint8_t *dirty_row_bitmap);
void shared_module_usb_video_uvcframebuffer_set_frame_callback(usb_video_uvcframebuffer_obj_t *self, framebuffer_frame_callback_t cb, void *data);
void shared_module_usb_video_uvcframebuffer_frame_boundary(usb_video_uvcframebuffer_obj_t *self);
int shared_module_usb_video_uvcframebuffer_get_width(usb_video_uvcframebuffer_obj_t *self);
int shared_module_usb_video_uvcframebuffer_get_height(usb_video_uvcframebuffer_obj_t *self);
