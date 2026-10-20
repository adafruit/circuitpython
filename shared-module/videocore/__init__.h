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

// Called (in interrupt context) when a frame has landed. frame is the
// destination base it landed at: usually the latest accepted destination, but
// a frame already being written when the destination changed (or when the
// source was paused) still lands at the previous one.
typedef void (*videocore_on_frame_fun)(void *ctx, const void *frame);

// Protocol implemented by video streamers such as mipicsi.Camera. A source
// streams continuously while playing (unless paused); the target sets (and can
// change) where the pixels go by calling set_destination() at any time,
// including between frames.
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
    // May be NULL for sources that need no preparation. The source must not
    // write frames before set_destination(). When paused is true, the source's
    // hardware must come up active but not writing frames, as if pause() had
    // been called before it started; this avoids a play-then-pause race when
    // the target isn't shown yet.
    void (*play)(mp_obj_t self, bool paused);

    // Pause streaming without stopping the camera or releasing its hardware:
    // e.g. stop the CSI controller's DMA while the sensor keeps streaming.
    // No frames are written until resume(). Called when a framebuffer target
    // stops being shown. set_destination() while paused updates where resume()
    // will stream. Must not raise: it runs from displayio group operations.
    void (*pause)(mp_obj_t self);

    // Resume streaming after pause() into the most recent destination. Called
    // from Python contexts (when the framebuffer target is shown again), so it
    // may raise; the source must stay paused if it does.
    void (*resume)(mp_obj_t self);

    // Direct future frames into dest. The destination may be changed
    // between frames; there is no NULL/stop value: stopping is stop()'s
    // job. Never raises: returns whether the stream accepted the
    // destination (the caller may ignore the result). on_frame(ctx, frame)
    // is called from interrupt context whenever a frame has landed, with the
    // destination base it landed at (the source never relocates frames
    // elsewhere). A change takes effect from the next frame: the frame being
    // written still lands at the previous destination.
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
// whose protocol doesn't implement them. pause/resume are required: they
// keep a source's hardware alive while a framebuffer target is hidden.
void videosource_play(mp_obj_t source, bool paused);
void videosource_stop(mp_obj_t source);
void videosource_pause(mp_obj_t source);
void videosource_resume(mp_obj_t source);

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
