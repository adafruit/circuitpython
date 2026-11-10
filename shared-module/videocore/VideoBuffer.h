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

#if CIRCUITPY_VIDEOCORE_ACCEL
#include "supervisor/background_callback.h"

// Staging buffers for rotated output: the source fills one, the accelerator
// reads another, and the third is where the source goes next.
#define VIDEOCORE_STAGING_COUNT (3)
// Staging buffer address and size alignment. Covers the ESP32-P4's L1 and L2
// cache lines, which DMA into and out of PSRAM needs.
#define VIDEOCORE_STAGING_ALIGN (128)
#endif

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
    // Destination position of the video within its Group. A framebuffer target
    // streams rows straight into the framebuffer, so on a rotated (transposed)
    // display these logical coordinates are transposed when the destination is
    // computed.
    mp_int_t x;
    mp_int_t y;
    mp_int_t source_width;
    mp_int_t source_height;
    mp_int_t source_bits_per_pixel;
    size_t source_frame_bytes;
    // Base of the destination the source last accepted, or NULL when there
    // is none.
    const void *stream_base;
    // Set from interrupt context when the source lands a frame in the
    // destination; cleared when the refresh takes the changed area.
    volatile bool frame_ready;
    // What changed in the frames landed since the last refresh, in source
    // coordinates. Grown with frame_ready and cleared with it.
    displayio_area_t frame_changed_area;
    // The last frame landed passed NULL, so the next one repaints everything.
    volatile bool frame_replaceable;
    // frame_changed_area in screen coordinates, as handed to the refresh.
    displayio_area_t frame_dirty_area;
    // Screen area of frames that landed where the video used to be: the frame
    // in flight when the destination moves or the source pauses still lands
    // at the old destination, possibly after that area was repainted. Grown
    // by on_frame in interrupt context; the refresh moves it into
    // ephemeral_dirty_area with interrupts disabled.
    displayio_area_t vacated_area;
    // Framebuffer geometry cached for the destination and for on_frame, which
    // can't call the framebuffer protocol from interrupt context. fb_pixels is
    // NULL for plain buffer targets. fb_width and fb_height are the
    // framebuffer's physical geometry, which is transposed on a rotated
    // display.
    const uint8_t *fb_pixels;
    size_t fb_pixels_len;
    size_t fb_row_stride;
    mp_int_t fb_width;
    mp_int_t fb_height;
    // displayio draw protocol state: the rect currently rendered (in screen
    // coordinates, which are the framebuffer's physical coordinates) and the
    // rect that changed since the last refresh.
    displayio_area_t current_area;
    displayio_area_t ephemeral_dirty_area;
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
    #if CIRCUITPY_VIDEOCORE_ACCEL
    // Rotated output. When the display rotates or mirrors and the port's
    // accelerator can do the copy, the source streams into staging buffers
    // in its own orientation, and a background callback rotates each landed
    // frame into the framebuffer at accel_dst_x/y, upright in display
    // coordinates. current_area is then the rotated rect. Otherwise frames
    // land directly in the framebuffer, unrotated.
    bool accel;
    // Clockwise rotation, then left-right flip, from source to screen.
    uint16_t accel_rotation;
    bool accel_mirror_x;
    // Top left of the rotated frame, in framebuffer pixels.
    mp_int_t accel_dst_x;
    mp_int_t accel_dst_y;
    // Destination handed to the source; only its base changes between
    // staging buffers.
    videocore_destination_t accel_dest;
    // Source-orientation frames, aligned within the port_malloc allocations
    // in staging_alloc (outside the VM heap, so never moved or collected).
    // Allocated on first use and freed once the source has stopped.
    uint8_t *staging_alloc[VIDEOCORE_STAGING_COUNT];
    uint8_t *staging[VIDEOCORE_STAGING_COUNT];
    // The latest frame that landed in a staging buffer and isn't copied yet,
    // or NULL. Set from interrupt context.
    const void *volatile accel_landed;
    // The staging buffer the source was filling when accel_landed landed, or
    // NULL when unknown. Set from interrupt context.
    const void *volatile accel_writing;
    // The staging buffer the source last accepted as its destination, or
    // NULL while that is being changed.
    const void *volatile accel_next;
    background_callback_t accel_callback;
    #endif
} videocore_videobuffer_obj_t;

void common_hal_videocore_videobuffer_construct(videocore_videobuffer_obj_t *self, mp_obj_t framebuffer);
void common_hal_videocore_videobuffer_play(videocore_videobuffer_obj_t *self, mp_obj_t source);
void common_hal_videocore_videobuffer_set_position(videocore_videobuffer_obj_t *self, mp_int_t x, mp_int_t y);
void common_hal_videocore_videobuffer_stop(videocore_videobuffer_obj_t *self);
// Stop without raising (from the finaliser). A failed stop is best-effort;
// the source's own finaliser stops it as well.
void videocore_videobuffer_stop_quiet(videocore_videobuffer_obj_t *self);
bool common_hal_videocore_videobuffer_get_playing(videocore_videobuffer_obj_t *self);
mp_obj_t common_hal_videocore_videobuffer_get_source(videocore_videobuffer_obj_t *self);

// Draw protocol that lets a VideoBuffer be added to a displayio Group.
// displayio copies plain-buffer frames into the display; framebuffer-target
// frames are already in place, so displayio only marks their rows.
extern const vectorio_draw_protocol_t videocore_videobuffer_draw_protocol;
