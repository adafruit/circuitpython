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

// Screen areas handed between VideoBuffer's shared state and its backend. The legacy
// compositor walks linked lists of them, so keep the link in a wrapper: displayio_area_t
// itself is just lv_area_t in LVGL builds.
typedef struct _videocore_area_t {
    displayio_area_t area;
    const struct _videocore_area_t *next;
} videocore_area_t;

#include "shared-module/framebufferio/FramebufferDisplay.h" // framebuffer protocol
#include "shared-module/videocore/__init__.h"

typedef struct _videocore_videobuffer_obj_t videocore_videobuffer_obj_t;

// The compositor-specific half of a VideoBuffer lives in one of the two backend directories.
// See legacy/VideoBuffer.h and lvgl/VideoBuffer.h.
#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/videocore/lvgl/VideoBuffer.h"
// The protocol slot a VideoBuffer exposes to its Group. Legacy VideoBuffers are part of the
// draw protocol; LVGL ones provide protocol_lvgl.
#define VIDEOCORE_VIDEOBUFFER_PROTOCOL &videocore_videobuffer_lvgl_protocol
#else
#include "shared-module/videocore/legacy/VideoBuffer.h"
#define VIDEOCORE_VIDEOBUFFER_PROTOCOL &videocore_videobuffer_draw_protocol
#endif

struct _videocore_videobuffer_obj_t {
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
    // Destination position of the video within its Group.
    mp_int_t x;
    mp_int_t y;
    mp_int_t source_width;
    mp_int_t source_height;
    mp_int_t source_bits_per_pixel;
    size_t source_frame_bytes;
    // The pixel format play() asks the source for: the plain buffer's, or
    // the framebuffer's color depth.
    mp_int_t bits_per_pixel;
    // Base of the destination the source last accepted, or NULL when there
    // is none.
    const void *stream_base;
    // Screen area of frames that landed where the video used to be: the frame
    // in flight when the destination moves or the source pauses still lands
    // at the old destination, possibly after that area was repainted. Grown
    // by on_frame in interrupt context; the refresh moves it into its own
    // dirty bookkeeping with interrupts disabled.
    videocore_area_t vacated_area;
    // Framebuffer geometry cached for the destination and for on_frame, which
    // can't call the framebuffer protocol from interrupt context. fb_pixels is
    // NULL for plain buffer targets. fb_width and fb_height are the
    // framebuffer's physical geometry.
    const uint8_t *fb_pixels;
    size_t fb_pixels_len;
    size_t fb_row_stride;
    mp_int_t fb_width;
    mp_int_t fb_height;
    // The rect currently rendered, in screen coordinates, which are the
    // framebuffer's physical coordinates.
    videocore_area_t current_area;
    // Maps the video's Group-local coordinates to screen coordinates, which are
    // the framebuffer's physical coordinates.
    displayio_buffer_transform_t absolute_transform;
    bool in_group;
    // Set by the group when hidden; while hidden the video doesn't show and
    // a framebuffer target's source is paused (it is revalidated on unhide).
    bool hidden;
    // True when the source's sizing no longer matches what play() captured:
    // the video won't show until stop and play again.
    bool stale_size;
    bool playing;
    // Set from interrupt context or a background task when the source reached
    // the end of its stream; the VideoBuffer then stops it lazily.
    volatile bool source_ended;
    // True while a framebuffer target isn't shown, so its source is paused.
    // Tracked while stopped too, so play() can start the source paused.
    // Plain buffer targets are never paused.
    bool paused;
    // Compositor-specific mirror or draw state.
    videocore_videobuffer_backend_t backend;
};

// Read an optional framebuffer protocol getter, or default_value when it is NULL.
#define FB_GETTER(method, default_value) \
    (self->framebuffer_protocol->method \
        ? self->framebuffer_protocol->method(self->framebuffer) \
        : (default_value))

void common_hal_videocore_videobuffer_construct(videocore_videobuffer_obj_t *self, mp_obj_t framebuffer,
    mp_int_t bits_per_pixel);
void common_hal_videocore_videobuffer_play(videocore_videobuffer_obj_t *self, mp_obj_t source);
void common_hal_videocore_videobuffer_set_position(videocore_videobuffer_obj_t *self, mp_int_t x, mp_int_t y);
void common_hal_videocore_videobuffer_stop(videocore_videobuffer_obj_t *self);
// Stop without raising (from the finaliser). A failed stop is best-effort;
// the source's own finaliser stops it as well.
void videocore_videobuffer_stop_quiet(videocore_videobuffer_obj_t *self);
bool common_hal_videocore_videobuffer_get_playing(videocore_videobuffer_obj_t *self);
mp_obj_t common_hal_videocore_videobuffer_get_source(videocore_videobuffer_obj_t *self);

// Update the stream to match whether the VideoBuffer is shown (on a display,
// attached, and not hidden). May raise when resuming a framebuffer target.
void videocore_videobuffer_update_stream(videocore_videobuffer_obj_t *self);
// Recompute the rect the video covers from its position and transform.
void videocore_videobuffer_update_area(videocore_videobuffer_obj_t *self);
// Map a rect in source coordinates to the screen rect it covers once rotated.
void videocore_videobuffer_source_rect(videocore_videobuffer_obj_t *self,
    const displayio_area_t *source, videocore_area_t *screen);
// Follow a move of a playing framebuffer target. Best effort, never raises.
void videocore_videobuffer_move_dest(videocore_videobuffer_obj_t *self);
// Drop any landed frame the next refresh hasn't taken yet.
void videocore_videobuffer_forget_frame(videocore_videobuffer_obj_t *self);
// Stop a source that reached the end of its stream. Never raises.
void videocore_videobuffer_end_stream(videocore_videobuffer_obj_t *self);
// Raise when a framebuffer target can't be shown on this display.
void videocore_videobuffer_validate_framebuffer_target(videocore_videobuffer_obj_t *self,
    mp_obj_t display, mp_int_t rotation);
