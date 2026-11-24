// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

#include "shared-bindings/vectorio/__init__.h" // draw protocol
#include "shared-module/displayio/area.h"

struct _videocore_videobuffer_obj_t;

// Legacy scanline compositor state for a VideoBuffer.
//
// Frames land in the source's destination directly. The draw protocol reports where the
// video is now (current_area, shared with the stream code) and where it has been since the
// last refresh (ephemeral_dirty_area) so displayio repaints exactly what changed. A frame
// that lands at a destination the video no longer occupies is reported through the shared
// vacated_area.
typedef struct {
    // Set from interrupt context when the source lands a frame in the destination; cleared
    // when the refresh takes the changed area.
    volatile bool frame_ready;
    // What changed in the frames landed since the last refresh, in source coordinates.
    // Grown with frame_ready and cleared with it.
    videocore_area_t frame_changed_area;
    // The last frame landed passed NULL, so the next one repaints everything.
    volatile bool frame_replaceable;
    // frame_changed_area in screen coordinates, as handed to the refresh.
    videocore_area_t frame_dirty_area;
    // Rect the video has occupied since the last refresh, for repainting what it leaves
    // behind when it moves or is hidden.
    videocore_area_t ephemeral_dirty_area;
} videocore_videobuffer_backend_t;

// Draw protocol that lets a VideoBuffer be added to a displayio Group.
// displayio copies plain-buffer frames into the display; framebuffer-target
// frames are already in place, so displayio only marks their rows.
extern const vectorio_draw_protocol_t videocore_videobuffer_draw_protocol;

// Chain node's area in front of tail and return the new list head. The scanline compositor
// walks displayio_area_t.next, which LVGL's lv_area_t doesn't have, so the link lives in
// the wrapper.
static inline displayio_area_t *videocore_area_link(videocore_area_t *node, displayio_area_t *tail) {
    node->area.next = tail;
    return &node->area;
}

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
