// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-bindings/vectorio/__init__.h" // draw protocol
#include "shared-module/displayio/area.h"
#include "shared-module/framebufferio/FramebufferDisplay.h" // framebuffer protocol
#include "shared-module/videocore/__init__.h"

typedef struct {
    mp_obj_base_t base;
    // Object frames are streamed into: an object implementing the framebuffer
    // protocol, such as mipidsi.Display, or a writable buffer such as a
    // bytearray or memoryview.
    mp_obj_t framebuffer;
    const framebuffer_p_t *framebuffer_protocol;
    // Target buffer info in raw buffer mode (framebuffer_protocol == NULL).
    mp_buffer_info_t bufinfo;
    // The video source being played, or None.
    mp_obj_t source;
    // Destination position of the video within the target.
    mp_int_t x;
    mp_int_t y;
    mp_int_t source_width;
    mp_int_t source_height;
    mp_int_t source_bits_per_pixel;
    size_t source_frame_bytes;
    // Where the source streams frames into: the requested destination, or a
    // later, hardware-aligned position inside it. "Later in the buffer" only
    // happens for plain-buffer targets; set by play() and the x/y
    // properties.
    const void *stream_base;
    // Set from interrupt context when the source lands a frame in the
    // destination; cleared by the refresh pass once the new frame has been
    // rendered into the display.
    volatile bool frame_ready;
    // displayio draw protocol state: the rect currently rendered (in screen
    // coordinates) and the rect that changed since the last refresh.
    displayio_area_t current_area;
    displayio_area_t ephemeral_dirty_area;
    displayio_buffer_transform_t absolute_transform;
    bool in_group;
    // Set by the group when hidden; while hidden the video doesn't show and
    // the source is free to reconfigure (it is revalidated on unhide).
    bool hidden;
    // True when the source's sizing no longer matches what play() captured:
    // the video won't show until stop and play again.
    bool stale_size;
    bool playing;
} videocore_videobuffer_obj_t;

void common_hal_videocore_videobuffer_construct(videocore_videobuffer_obj_t *self, mp_obj_t framebuffer_or_display);
void common_hal_videocore_videobuffer_play(videocore_videobuffer_obj_t *self, mp_obj_t source);
void common_hal_videocore_videobuffer_set_position(videocore_videobuffer_obj_t *self, mp_int_t x, mp_int_t y);
void common_hal_videocore_videobuffer_stop(videocore_videobuffer_obj_t *self);
// Stop without raising (from the finaliser). A failed stop is best-effort;
// the source's own finaliser stops it as well.
void videocore_videobuffer_stop_quiet(videocore_videobuffer_obj_t *self);
bool common_hal_videocore_videobuffer_get_playing(videocore_videobuffer_obj_t *self);
mp_obj_t common_hal_videocore_videobuffer_get_source(videocore_videobuffer_obj_t *self);

// The draw protocol implementation that lets a VideoBuffer be added to a
// displayio Group: plain-buffer targets get bulk copied into the framebuffer
// by displayio's rendering; framebuffer targets claim their area (a no-op
// fill: the source streams their pixels straight into the framebuffer) so
// displayio's refresh pushes the video's rows to the framebuffer.
extern const vectorio_draw_protocol_t videocore_videobuffer_draw_protocol;
