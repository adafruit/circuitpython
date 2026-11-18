// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/usb_video/__init__.h"
#include "shared-module/usb_video/USBFramebuffer.h"
#include "shared-bindings/usb_video/USBFramebuffer.h"
#include "shared-bindings/usb_video/__init__.h"

usb_video_uvcframebuffer_obj_t usb_video_uvcframebuffer_singleton_obj = {
    .base = { &usb_video_USBFramebuffer_type },
};

void shared_module_usb_video_uvcframebuffer_get_bufinfo(usb_video_uvcframebuffer_obj_t *self, mp_buffer_info_t *bufinfo) {
    bufinfo->buf = usb_video_framebuffer_rgb565;
    bufinfo->len = 2 * usb_video_frame_width * usb_video_frame_height;
}

void shared_module_usb_video_uvcframebuffer_refresh(usb_video_uvcframebuffer_obj_t *self) {
    shared_module_usb_video_swapbuffers();
}

void shared_module_usb_video_uvcframebuffer_swapbuffers(usb_video_uvcframebuffer_obj_t *self, const uint8_t *dirty_row_bitmap) {
    shared_module_usb_video_swapbuffers_dirty(dirty_row_bitmap);
}

void shared_module_usb_video_uvcframebuffer_set_frame_callback(usb_video_uvcframebuffer_obj_t *self, framebuffer_frame_callback_t cb, void *data) {
    self->frame_cb = cb;
    self->frame_cb_data = data;
}

void shared_module_usb_video_uvcframebuffer_frame_boundary(usb_video_uvcframebuffer_obj_t *self) {
    if (self->frame_cb != NULL) {
        self->frame_cb(self->frame_cb_data);
    }
}

int shared_module_usb_video_uvcframebuffer_get_width(usb_video_uvcframebuffer_obj_t *self) {
    return usb_video_frame_width;
}

int shared_module_usb_video_uvcframebuffer_get_height(usb_video_uvcframebuffer_obj_t *self) {
    return usb_video_frame_height;
}
