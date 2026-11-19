// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

#include "shared-module/displayio/area.h"

struct _videocore_videobuffer_obj_t;
#include "shared-module/displayio/lvgl/lvgl_protocol.h"

#include "lvgl.h"

// LVGL compositor state for a VideoBuffer.
//
// The video is one lv_image that samples the source's current destination directly: the
// user's buffer for a plain-buffer target, or the framebuffer band a framebuffer target
// streams into. Frames are rewritten in place, so a landed frame only invalidates the
// image; there is no copy. The shared stream code reports frame arrivals and stream
// changes through the backend hooks below.
typedef struct {
    // The image view of the current destination.
    lv_obj_t *lv_obj;
    lv_image_dsc_t lv_dsc;
    // Descriptor contents last pushed into LVGL, so a move can drop the image cache and
    // repoint the view without doing either on every frame.
    const uint8_t *lv_data;
    lv_color_format_t lv_cf;
    uint16_t lv_width;
    uint16_t lv_height;
    uint16_t lv_stride;
    // The compositor's list of attached mirrors, rooted during GC.
    struct _videocore_videobuffer_obj_t *lv_next;
    bool lv_registered;
    // Set from interrupt context when a frame (or a vacated frame) needs an invalidation.
    // Lets the background tick skip the LVGL lock when idle.
    volatile bool lv_pending;
} videocore_videobuffer_backend_t;

// How the LVGL compositor attaches and positions a VideoBuffer. See
// displayio/lvgl/lvgl_protocol.h.
extern const lvgl_protocol_t videocore_videobuffer_lvgl_protocol;

// Backend hooks called by the shared stream code. See shared-module/videocore/VideoBuffer.c.
void videocore_videobuffer_backend_construct(struct _videocore_videobuffer_obj_t *self);
void videocore_videobuffer_backend_on_frame(struct _videocore_videobuffer_obj_t *self,
    const void *frame, const displayio_area_t *changed);
void videocore_videobuffer_backend_forget_frame(struct _videocore_videobuffer_obj_t *self);
bool videocore_videobuffer_backend_shown(struct _videocore_videobuffer_obj_t *self);
void videocore_videobuffer_backend_stream_changed(struct _videocore_videobuffer_obj_t *self);
void videocore_videobuffer_backend_source_changed(struct _videocore_videobuffer_obj_t *self);
void videocore_videobuffer_backend_dirty_current_area(struct _videocore_videobuffer_obj_t *self);
void videocore_videobuffer_backend_check_buffer_format(struct _videocore_videobuffer_obj_t *self,
    mp_int_t bits_per_pixel);
void videocore_videobuffer_backend_validate_member(mp_obj_t member, mp_obj_t display,
    mp_int_t rotation);

// Invalidate the screen rects frames landed in since the last call. Runs from the
// supervisor tick and at the start of a refresh, on the CircuitPython thread.
void displayio_lvgl_video_background(void);
// Root the attached mirrors during GC. They live in a port-memory list.
void displayio_lvgl_video_gc_collect(void);
// False when a framebuffer target is attached to display and can't stream into it at the
// given rotation.
bool displayio_lvgl_video_can_rotate(lv_display_t *display, lv_display_rotation_t rotation);
