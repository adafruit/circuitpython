// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"

// Destination for a video stream: where to put pixels, as a pointer to the
// first pixel and the destination row stride. row_bytes/height describe the
// source frame; the source assumes the destination holds the whole frame
// (the ESP CSI DMA is contiguous-only), so the rect is validated before
// streaming starts and when the position changes.
typedef struct {
    // First pixel of the destination.
    uint8_t *base;
    // Destination row stride, in bytes.
    size_t stride;
    // Source frame row size, in bytes.
    size_t row_bytes;
    // Source frame height, in rows.
    size_t height;
} videocore_destination_t;

// Called (in interrupt context) when a frame has landed in the buffer
// set_destination returned.
typedef void (*videocore_on_frame_fun)(void *ctx);

// Protocol implemented by video streamers such as mipicsi.Camera. The streams
// continuously; the target sets (and can change) where the pixels go by
// calling set_destination() at any time, including between frames.
//
// This is the video equivalent of the audiosample protocol in the audiocore
// module, inverted: instead of the target pulling frames, the source writes
// where it is told to.
typedef struct _videosource_p_t {
    MP_PROTOCOL_HEAD // MP_QSTR_protocol_videosource

    // Create whatever streaming hardware the source needs and start it (e.g.
    // start a sensor streaming). Called once before the first
    // set_destination of a stream, from Python (VideoBuffer.play), so it may
    // raise. Raises when the source is already streaming for someone else.
    // May be NULL for sources that need no preparation.
    void (*play)(mp_obj_t self);

    // Direct future frames into dest. The destination may be changed
    // between frames; there is no NULL/stop value: stopping is stop()'s
    // job. Never raises: returns whether the stream accepted the
    // destination (the caller may ignore the result). on_frame(ctx) is
    // called from interrupt context whenever a frame has landed in
    // dest.base (the source never relocates frames elsewhere).
    bool (*set_destination)(mp_obj_t self, const videocore_destination_t *dest,
        videocore_on_frame_fun on_frame, void *ctx);

    // Stop streaming and release the streaming hardware (e.g. stop the
    // sensor so its settings can be changed). Called once per stream, from
    // Python (VideoBuffer.stop), so it may raise; exceptions there are
    // clearer than implicit stops. Only called when play() succeeded.
    void (*stop)(mp_obj_t self);

    // Width of the frames the source streams, in pixels.
    mp_int_t (*get_width)(mp_obj_t self);
    // Height of the frames the source streams, in lines.
    mp_int_t (*get_height)(mp_obj_t self);
    // Bits per pixel of the frames the source streams.
    mp_int_t (*get_bits_per_pixel)(mp_obj_t self);
} videosource_p_t;

// Protocol function helpers. videosource_play/stop are no-ops for sources
// whose protocol doesn't implement them.
void videosource_play(mp_obj_t source);
void videosource_stop(mp_obj_t source);

// Return the videosource protocol of obj or raise TypeError.
const videosource_p_t *videosource_check(mp_obj_t obj);

// Dispatch helpers mirroring the audiosample_* functions. videosource_play
// is a no-op for sources whose protocol has no play.
bool videosource_set_destination(mp_obj_t source, const videocore_destination_t *dest,
    videocore_on_frame_fun on_frame, void *ctx);
mp_int_t videosource_get_width(mp_obj_t source);
mp_int_t videosource_get_height(mp_obj_t source);
mp_int_t videosource_get_bits_per_pixel(mp_obj_t source);

// Raise when the group tree contains a VideoBuffer that streams into a
// framebuffer other than the one this display renders. Called when a group
// is made a display's root group. Implemented in shared-module/videocore/
// VideoBuffer.c.
void videocore_validate_root_group(mp_obj_t root_group, mp_obj_t display);
