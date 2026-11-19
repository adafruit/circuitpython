// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-module/videocore/VideoBuffer.h"

#include "py/runtime.h"

#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/videocore/VideoBuffer.h"
#include "shared-bindings/videocore/__init__.h"
#include "shared-module/displayio/Group.h"
#include "shared-module/videocore/__init__.h"

#if CIRCUITPY_FRAMEBUFFERIO
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#endif

// Add the screen area of a frame that landed at frame to vacated_area. Uses
// only the cached framebuffer geometry since this runs in interrupt context.
// A shifted frame can start mid-row and wrap its rows; then cover the full
// width.
static void videocore_videobuffer_add_vacated_frame(videocore_videobuffer_obj_t *self, const void *frame) {
    const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
    const uint8_t *pixels = self->fb_pixels;
    if (pixels == NULL || bytes_per_pixel == 0 || self->fb_row_stride == 0 ||
        (const uint8_t *)frame < pixels) {
        return; // Plain buffer frames are never shown where they land.
    }
    const size_t offset = (const uint8_t *)frame - pixels;
    if (offset >= self->fb_pixels_len) {
        return;
    }
    displayio_area_t area = {
        .x1 = (offset % self->fb_row_stride) / bytes_per_pixel,
        .y1 = offset / self->fb_row_stride,
    };
    area.x2 = area.x1 + self->source_width;
    area.y2 = area.y1 + self->source_height;
    if (area.x2 > self->fb_width) {
        area.x1 = 0;
        area.x2 = self->fb_width;
        area.y2 += 1;
    }
    displayio_area_union(&self->vacated_area.area, &area, &self->vacated_area.area);
}

// Called from interrupt context or a background task when the source lands a
// frame, so keep it tiny.
static void videocore_videobuffer_on_frame(void *ctx, const void *frame, const displayio_area_t *changed) {
    videocore_videobuffer_obj_t *self = ctx;
    if (frame != self->stream_base) {
        // It landed where the video used to be (the frame in flight when it
        // moved or paused). Repaint there, since that area may already have
        // been repainted.
        videocore_videobuffer_add_vacated_frame(self, frame);
    }
    videocore_videobuffer_backend_on_frame(self, frame, changed);
}

// Called from interrupt context or a background task when the source reached
// the end of its stream. Only flag it: end_stream stops the source later, the
// way audio outputs stop a finished sample.
static void videocore_videobuffer_on_stream_end(void *ctx) {
    videocore_videobuffer_obj_t *self = ctx;
    self->source_ended = true;
}

// Drop any landed frame the refresh hasn't taken yet.
void videocore_videobuffer_forget_frame(videocore_videobuffer_obj_t *self) {
    videocore_videobuffer_backend_forget_frame(self);
}

// Cache the framebuffer geometry for on_frame (see fb_pixels).
static void videocore_videobuffer_cache_fb_geometry(videocore_videobuffer_obj_t *self,
    const mp_buffer_info_t *bufinfo, mp_int_t fb_width, mp_int_t fb_height,
    mp_int_t row_stride, mp_int_t first_pixel_offset) {
    self->fb_pixels = (const uint8_t *)bufinfo->buf + first_pixel_offset;
    self->fb_pixels_len = bufinfo->len - first_pixel_offset;
    self->fb_row_stride = row_stride;
    self->fb_width = fb_width;
    self->fb_height = fb_height;
}

// Compute the rect (in screen coordinates, which are the framebuffer's
// physical coordinates) that a frame written at the logical position x/y
// covers. A framebuffer target's frames are never rotated, and a rotated
// display is rejected when its transform is applied, so the transform only
// translates: the rect is source_width wide and source_height tall. A video
// that isn't shown has no transform, which translates by nothing, so the
// areas reported while detached stay sensible.
static void videocore_videobuffer_frame_rect(videocore_videobuffer_obj_t *self,
    mp_int_t x, mp_int_t y, videocore_area_t *area) {
    const displayio_buffer_transform_t *t = &self->absolute_transform;
    area->area.x1 = t->x + x;
    area->area.y1 = t->y + y;
    area->area.x2 = area->area.x1 + self->source_width;
    area->area.y2 = area->area.y1 + self->source_height;
    area->next = NULL;
}


// Map a point in the source's coordinates (0, 0 to source_width/
// source_height) to screen coordinates, which are the framebuffer's physical
// coordinates. A plain buffer target is rotated with the display as displayio
// copies its pixels, so both axes can be mirrored and swapped. The transform
// is only ever a translation plus mirroring and/or transposing (scale 1),
// which this relies on. While the video isn't shown there is no transform, so
// the point keeps its plain x/y position.
static void videocore_videobuffer_source_point(videocore_videobuffer_obj_t *self,
    mp_int_t source_x, mp_int_t source_y, mp_int_t *screen_x, mp_int_t *screen_y) {
    const displayio_buffer_transform_t *t = &self->absolute_transform;
    const mp_int_t x = self->x + source_x;
    const mp_int_t y = self->y + source_y;
    if (t->scale == 0) {
        // Not on a display, so there is no transform to apply. Keep the plain
        // position so the areas reported while detached stay sensible.
        *screen_x = x;
        *screen_y = y;
    } else if (t->transpose_xy) {
        // The local y axis becomes screen x and the local x axis screen y.
        *screen_x = t->x + t->dx * y;
        *screen_y = t->y + t->dy * x;
    } else {
        *screen_x = t->x + t->dx * x;
        *screen_y = t->y + t->dy * y;
    }
}

// Map a rect in source coordinates to the screen rect it covers once it is
// rotated. Mirroring and transposing a rect keeps it axis aligned, only
// swapping its width and height on a transposed (90 or 270 degree) display.
void videocore_videobuffer_source_rect(videocore_videobuffer_obj_t *self,
    const displayio_area_t *source, videocore_area_t *screen) {
    mp_int_t x1;
    mp_int_t y1;
    mp_int_t x2;
    mp_int_t y2;
    videocore_videobuffer_source_point(self, source->x1, source->y1, &x1, &y1);
    videocore_videobuffer_source_point(self, source->x2, source->y2, &x2, &y2);
    screen->area.x1 = MIN(x1, x2);
    screen->area.y1 = MIN(y1, y2);
    screen->area.x2 = MAX(x1, x2);
    screen->area.y2 = MAX(y1, y2);
    screen->next = NULL;
}

// Recompute the rendered rect of the video from the x/y position and the
// absolute transform. A framebuffer target only follows the transform's
// position because its frames are never rotated. A plain buffer target is
// rotated with the display, so its whole frame is transformed.
void videocore_videobuffer_update_area(videocore_videobuffer_obj_t *self) {
    if (self->framebuffer_protocol != NULL) {
        videocore_videobuffer_frame_rect(self, self->x, self->y, &self->current_area);
        return;
    }
    displayio_area_t whole = {
        .x2 = self->source_width,
        .y2 = self->source_height,
    };
    videocore_videobuffer_source_rect(self, &whole, &self->current_area);
}

// Bytes in one row of a source frame. 1-bit pixels are packed and each row is
// padded to a whole byte. Framebuffer targets always use whole bytes (see
// read_source_format).
static size_t videocore_videobuffer_row_bytes(mp_int_t width, mp_int_t bits_per_pixel) {
    return ((size_t)width * bits_per_pixel + 7) / 8;
}

static size_t videocore_videobuffer_source_row_bytes(videocore_videobuffer_obj_t *self) {
    return videocore_videobuffer_row_bytes(self->source_width, self->source_bits_per_pixel);
}

// Bytes a frame covers in a framebuffer with this row stride, from its first
// pixel. A contiguous DMA (the camera) writes frame_bytes in a row; a source
// that honors the stride (a GIF) ends its last row (height - 1) strides in.
// The frame must fit either way.
static mp_int_t videocore_videobuffer_frame_span(videocore_videobuffer_obj_t *self, mp_int_t row_stride) {
    const mp_int_t strided = (self->source_height - 1) * row_stride +
        videocore_videobuffer_source_row_bytes(self);
    return MAX((mp_int_t)self->source_frame_bytes, strided);
}

// Compute the destination for the current position, including a containing
// group's translation. Raises unless the whole frame fits in the target,
// whether the source writes it contiguously or at the target's stride.
// move_dest() is the non-raising variant used from displayio.
static void videocore_videobuffer_compute_destination(
    videocore_videobuffer_obj_t *self, videocore_destination_t *dest) {
    const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
    const size_t row_bytes = videocore_videobuffer_source_row_bytes(self);

    if (self->framebuffer_protocol == NULL) {
        // Raw buffer mode: frames are written compactly from the start.
        dest->base = self->bufinfo.buf;
        dest->stride = row_bytes;
        dest->row_bytes = row_bytes;
        dest->height = self->source_height;
        // draw_fill_area swaps the bytes for displays that need it.
        dest->reverse_bytes_in_word = false;
        dest->on_stream_end = videocore_videobuffer_on_stream_end;
        if (self->bufinfo.len < self->source_frame_bytes) {
            mp_raise_ValueError(MP_ERROR_TEXT("Buffer too small"));
        }
        return;
    }

    mp_buffer_info_t bufinfo;
    self->framebuffer_protocol->get_bufinfo(self->framebuffer, &bufinfo);
    if (bufinfo.buf == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("framebuffer went away"));
    }

    const mp_int_t fb_width = FB_GETTER(get_width, 0);
    const mp_int_t fb_height = FB_GETTER(get_height, 0);
    const mp_int_t row_stride = FB_GETTER(get_row_stride, fb_width * self->source_bits_per_pixel / 8);
    const mp_int_t first_pixel_offset = FB_GETTER(get_first_pixel_offset, 0);

    // Frames keep the framebuffer's orientation: the containing group's
    // transform only translates them. The whole frame must stay within the
    // framebuffer and the buffer.
    videocore_area_t rect;
    videocore_videobuffer_frame_rect(self, self->x, self->y, &rect);
    if (self->x < 0 || self->y < 0 ||
        rect.area.x1 < 0 || rect.area.y1 < 0 ||
        rect.area.x2 > fb_width || rect.area.y2 > fb_height ||
        first_pixel_offset + rect.area.y1 * row_stride + rect.area.x1 * bytes_per_pixel +
        videocore_videobuffer_frame_span(self, row_stride) >
        bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("video doesn't fit in framebuffer"));
    }
    videocore_videobuffer_cache_fb_geometry(
        self, &bufinfo, fb_width, fb_height, row_stride, first_pixel_offset);

    dest->base = (uint8_t *)bufinfo.buf + first_pixel_offset +
        rect.area.y1 * row_stride + rect.area.x1 * bytes_per_pixel;
    dest->stride = row_stride;
    dest->row_bytes = row_bytes;
    dest->height = self->source_height;
    dest->reverse_bytes_in_word = FB_GETTER(get_reverse_pixels_in_word, false);
    dest->on_stream_end = videocore_videobuffer_on_stream_end;
}

// Move the framebuffer destination for a new absolute position, capping the
// shift so the frame stays completely within the framebuffer. Best effort:
// never raises, because this runs from displayio's refresh.
void videocore_videobuffer_move_dest(videocore_videobuffer_obj_t *self) {
    if (!self->playing || self->paused) {
        return;
    }
    mp_buffer_info_t bufinfo;
    self->framebuffer_protocol->get_bufinfo(self->framebuffer, &bufinfo);
    if (bufinfo.buf == NULL) {
        return;
    }
    const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
    const size_t row_bytes = videocore_videobuffer_source_row_bytes(self);
    const mp_int_t fb_width = FB_GETTER(get_width, 0);
    const mp_int_t fb_height = FB_GETTER(get_height, 0);
    const mp_int_t row_stride = FB_GETTER(get_row_stride, fb_width * self->source_bits_per_pixel / 8);
    const mp_int_t first_pixel_offset = FB_GETTER(get_first_pixel_offset, 0);
    if (row_stride <= 0 || bytes_per_pixel == 0) {
        return;
    }

    videocore_area_t rect;
    videocore_videobuffer_frame_rect(self, self->x, self->y, &rect);
    mp_int_t shift_x = MAX(rect.area.x1, 0);
    mp_int_t shift_y = MAX(rect.area.y1, 0);
    // Cap the start and the end. Cap the rows first: enough rows must fit
    // above the frame for it to fit at all.
    const mp_int_t max_end = (mp_int_t)bufinfo.len - first_pixel_offset -
        videocore_videobuffer_frame_span(self, row_stride);
    const mp_int_t buf_max_y = max_end / row_stride;
    const mp_int_t max_y = MIN(fb_height - self->source_height, buf_max_y);
    if (shift_y > max_y) {
        shift_y = MAX(max_y, 0);
    }
    const mp_int_t buf_max_x =
        (max_end - shift_y * row_stride) / (mp_int_t)bytes_per_pixel;
    const mp_int_t max_x = MIN(fb_width - self->source_width, buf_max_x);
    if (shift_x > max_x) {
        shift_x = MAX(max_x, 0);
    }
    videocore_videobuffer_cache_fb_geometry(
        self, &bufinfo, fb_width, fb_height, row_stride, first_pixel_offset);

    videocore_destination_t dest = {
        .base = bufinfo.buf + first_pixel_offset + shift_y * row_stride + shift_x * bytes_per_pixel,
        .stride = row_stride,
        .row_bytes = row_bytes,
        .height = self->source_height,
        .reverse_bytes_in_word = FB_GETTER(get_reverse_pixels_in_word, false),
        .on_stream_end = videocore_videobuffer_on_stream_end,
    };
    // A rejected destination leaves the stream where it was.
    if (videosource_set_destination(self->source, &dest, videocore_videobuffer_on_frame, self)) {
        self->stream_base = dest.base;
        videocore_videobuffer_backend_source_changed(self);
    }
}

void common_hal_videocore_videobuffer_construct(videocore_videobuffer_obj_t *self, mp_obj_t framebuffer,
    mp_int_t bits_per_pixel) {
    // Initialise what the finaliser uses before anything can raise.
    self->source = mp_const_none;
    self->stream_base = NULL;
    self->playing = false;
    // The finaliser can run after any of the raises below, so the backend state
    // has to be valid before anything else can fail.
    videocore_videobuffer_backend_construct(self);
    displayio_area_t empty = {};
    self->current_area.area = empty;
    self->current_area.next = NULL;
    self->vacated_area.area = empty;
    self->vacated_area.next = NULL;
    self->framebuffer = framebuffer;
    self->framebuffer_protocol = mp_proto_get(MP_QSTR_protocol_framebuffer, framebuffer);
    if (self->framebuffer_protocol != NULL) {
        if (self->framebuffer_protocol->get_bufinfo == NULL ||
            self->framebuffer_protocol->get_width == NULL ||
            self->framebuffer_protocol->get_height == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("framebuffer missing required functions"));
        }
        self->bits_per_pixel = FB_GETTER(get_color_depth, 16);
    } else if (!mp_get_buffer(framebuffer, &self->bufinfo, MP_BUFFER_WRITE)) {
        // Not a framebuffer and not a writable buffer such as a bytearray or
        // memoryview.
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"),
            MP_QSTR_framebuffer, MP_QSTR_buffer, MP_QSTR_framebuffer,
            mp_obj_get_type(framebuffer)->name);
    } else {
        self->bits_per_pixel = bits_per_pixel;
    }

    self->x = 0;
    self->y = 0;
    self->fb_pixels = NULL;
    self->fb_pixels_len = 0;
    self->fb_row_stride = 0;
    self->fb_width = 0;
    self->fb_height = 0;
    self->in_group = false;
    self->hidden = false;
    self->stale_size = false;
    self->source_ended = false;
    displayio_buffer_transform_t empty_transform = {};
    self->absolute_transform = empty_transform;
    // Nothing is shown until a Group attached to a display updates our
    // transform.
    self->paused = true;
}

// Record the started source's frame format, which is fixed until stop. May
// raise.
static void videocore_videobuffer_read_source_format(videocore_videobuffer_obj_t *self) {
    mp_obj_t source = self->source;
    const videosource_p_t *protocol = videosource_check(source);

    mp_int_t bits_per_pixel = protocol->get_bits_per_pixel(source);
    // Whole bytes per pixel, or 1 bit packed into each byte.
    if (bits_per_pixel % 8 != 0 && bits_per_pixel != 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("video format must be byte aligned"));
    }
    mp_int_t width = protocol->get_width(source);
    mp_int_t height = protocol->get_height(source);
    size_t frame_bytes = (size_t)height * videocore_videobuffer_row_bytes(width, bits_per_pixel);

    if (self->framebuffer_protocol != NULL) {
        mp_int_t color_depth = FB_GETTER(get_color_depth, 16);
        if (color_depth % 8 != 0 || bits_per_pixel != color_depth) {
            mp_raise_ValueError(MP_ERROR_TEXT("video format doesn't match framebuffer"));
        }
    } else {
        videocore_videobuffer_backend_check_buffer_format(self, bits_per_pixel);
    }

    self->source_width = width;
    self->source_height = height;
    self->source_bits_per_pixel = bits_per_pixel;
    self->source_frame_bytes = frame_bytes;

    if (self->framebuffer_protocol == NULL && (mp_int_t)self->bufinfo.len < (mp_int_t)frame_bytes) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer too small"));
    }
}

// Point a started source at the current destination and let it stream. Runs
// from Python contexts (play and draw_update_transform) so it may raise.
static void videocore_videobuffer_start_stream(videocore_videobuffer_obj_t *self) {
    videocore_destination_t dest;
    videocore_videobuffer_compute_destination(self, &dest);

    // The source streams from now on; on_frame reports each frame.
    if (!videosource_set_destination(self->source, &dest, videocore_videobuffer_on_frame, self)) {
        mp_raise_ValueError(MP_ERROR_TEXT("video source rejected the destination"));
    }
    self->stream_base = dest.base;
    self->paused = false;
    videocore_videobuffer_backend_stream_changed(self);
}

// Un-show a framebuffer target by pausing its source. The play session and
// the source's hardware stay alive, but no frames are written to the target.
// Best effort: this also runs during display reset, so errors are ignored.
static void videocore_videobuffer_pause_stream(videocore_videobuffer_obj_t *self) {
    if (self->paused || self->source == mp_const_none) {
        return; // Already paused, or nothing has been started yet.
    }
    // Drop the destination before pausing the source: the source finishes its
    // pause at the next frame boundary, so the frame being written still
    // lands after this, and on_frame adds it to vacated_area to repaint over.
    self->stream_base = NULL;
    self->paused = true;
    videocore_videobuffer_backend_stream_changed(self);
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        videosource_pause(self->source);
        nlr_pop();
    }
    videocore_videobuffer_forget_frame(self);
}

// Re-show a framebuffer target by pointing its paused source at the current
// destination and resuming it. Position changes while hidden are applied here;
// the source's format was fixed at play(), so reject a changed source rather
// than resuming into a wrong-sized frame.
static void videocore_videobuffer_resume_stream(videocore_videobuffer_obj_t *self) {
    const videosource_p_t *protocol = videosource_check(self->source);
    if (protocol->get_width(self->source) != self->source_width ||
        protocol->get_height(self->source) != self->source_height ||
        protocol->get_bits_per_pixel(self->source) != self->source_bits_per_pixel) {
        self->stale_size = true;
        return;
    }

    videocore_destination_t dest;
    videocore_videobuffer_compute_destination(self, &dest);
    if (!videosource_set_destination(self->source, &dest, videocore_videobuffer_on_frame, self)) {
        mp_raise_ValueError(MP_ERROR_TEXT("video source rejected the destination"));
    }
    videosource_resume(self->source);
    self->stream_base = dest.base;
    self->paused = false;
    // Wait for the first frame at the new destination.
    videocore_videobuffer_forget_frame(self);
    videocore_videobuffer_backend_stream_changed(self);
}

// Stop a source that reached the end of its stream. stream_base stays, so the
// last frame stays shown until stop() or the next play(). Never raises.
void videocore_videobuffer_end_stream(videocore_videobuffer_obj_t *self) {
    if (!self->playing || !self->source_ended) {
        return;
    }
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        videosource_stop(self->source);
        nlr_pop();
    }
    self->playing = false;
    self->source = mp_const_none;
}

// Start, resume or pause a framebuffer target's source to match whether it is
// shown (on a display and not hidden). While hidden the source stays active
// but paused, so showing it again is quick.
void videocore_videobuffer_update_stream(videocore_videobuffer_obj_t *self) {
    if (self->framebuffer_protocol == NULL) {
        return; // Plain buffer targets always stream.
    }
    // An ended source must not resume.
    videocore_videobuffer_end_stream(self);
    const bool shown = videocore_videobuffer_backend_shown(self);
    if (!self->playing) {
        // Remember whether the target is shown so play() can start the source
        // in the right state.
        self->paused = !shown;
        if (!shown) {
            // A repaint overwrites an ended stream's last frame.
            self->stream_base = NULL;
        }
        videocore_videobuffer_backend_stream_changed(self);
        return;
    }
    if (shown && !self->stale_size) {
        if (self->paused) {
            videocore_videobuffer_resume_stream(self);
        }
    } else if (!shown && !self->paused) {
        videocore_videobuffer_pause_stream(self);
    }
    videocore_videobuffer_backend_stream_changed(self);
}

void common_hal_videocore_videobuffer_play(videocore_videobuffer_obj_t *self, mp_obj_t source) {
    videocore_videobuffer_end_stream(self);
    if (self->playing) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("already playing"));
    }
    if (!videosource_set_bits_per_pixel(source, self->bits_per_pixel)) {
        mp_arg_error_invalid(MP_QSTR_bits_per_pixel);
    }
    self->stale_size = false;
    self->source_ended = false;
    videocore_videobuffer_forget_frame(self);
    // An ended stream's last frame stays until repainted, and the format is about to change.
    self->stream_base = NULL;

    // A framebuffer target only streams while shown. When it isn't, start the
    // source paused rather than pausing it afterwards, so no frame slips out
    // in between. Plain buffer targets always stream.
    const bool start_paused = self->framebuffer_protocol != NULL && self->paused;
    // If this raises the source may be playing elsewhere, so leave it alone.
    videosource_play(source, start_paused);
    self->source = source;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        videocore_videobuffer_read_source_format(self);
        videocore_videobuffer_update_area(self);
        self->playing = true;
        // A paused source gets its destination when the VideoBuffer is shown.
        if (!start_paused) {
            videocore_videobuffer_start_stream(self);
        }
        videocore_videobuffer_backend_source_changed(self);
        nlr_pop();
    } else {
        videocore_videobuffer_stop_quiet(self);
        nlr_raise(nlr.ret_val);
    }
}

void common_hal_videocore_videobuffer_set_position(videocore_videobuffer_obj_t *self, mp_int_t x, mp_int_t y) {
    if (self->framebuffer_protocol != NULL && x != 0) {
        // A contiguous source (the camera) ignores the stride, so every frame
        // row must land on a framebuffer row: the video's x must be 0.
        mp_raise_ValueError(MP_ERROR_TEXT("video in a framebuffer must be at x == 0"));
    }
    videocore_videobuffer_end_stream(self);
    // Repaint where the video used to be. Union (rather than overwrite) so a
    // move before the previous one was refreshed isn't lost.
    videocore_videobuffer_backend_dirty_current_area(self);
    self->x = x;
    self->y = y;
    videocore_videobuffer_update_area(self);
    if (self->framebuffer_protocol == NULL) {
        // Draw the last frame at its new position.
        videocore_videobuffer_backend_dirty_current_area(self);
    }
    if (!self->playing && self->framebuffer_protocol != NULL) {
        // A repaint overwrites an ended stream's last frame.
        self->stream_base = NULL;
    }
    if (!self->playing || self->paused || self->framebuffer_protocol == NULL) {
        // Raw buffer targets are rendered by displayio; their capture
        // destination doesn't move with the position. A paused framebuffer
        // target has no destination to move yet.
        videocore_videobuffer_backend_source_changed(self);
        return;
    }
    // The source's frame size was fixed when play() was called. Reject
    // sources that changed it (e.g. a sensor mode change) rather than
    // blitting at stale geometry.
    const videosource_p_t *protocol = videosource_check(self->source);
    mp_int_t width = protocol->get_width(self->source);
    mp_int_t height = protocol->get_height(self->source);
    mp_int_t bits_per_pixel = protocol->get_bits_per_pixel(self->source);
    if (width != self->source_width || height != self->source_height ||
        bits_per_pixel != self->source_bits_per_pixel) {
        mp_raise_ValueError(MP_ERROR_TEXT("video size changed since play; stop and play again"));
    }
    // The source picks up the new destination on its next frame. A rejected
    // destination leaves the stream where it was.
    videocore_destination_t dest;
    videocore_videobuffer_compute_destination(self, &dest);
    if (videosource_set_destination(self->source, &dest, videocore_videobuffer_on_frame, self)) {
        self->stream_base = dest.base;
    }
    videocore_videobuffer_backend_source_changed(self);
}

static void videocore_videobuffer_clear(videocore_videobuffer_obj_t *self) {
    self->playing = false;
    videocore_videobuffer_forget_frame(self);
    self->stream_base = NULL;
    self->source = mp_const_none;
    videocore_videobuffer_backend_stream_changed(self);
}

void common_hal_videocore_videobuffer_stop(videocore_videobuffer_obj_t *self) {
    // Stop the source; the camera also stops the sensor so its settings can
    // be changed. That may raise: surface it, after cleaning up, since the
    // stream did stop.
    if (self->source != mp_const_none) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            videosource_stop(self->source);
            nlr_pop();
        } else {
            videocore_videobuffer_clear(self);
            nlr_raise(nlr.ret_val);
        }
    }
    videocore_videobuffer_clear(self);
}

// Stop without raising: runs from background contexts (reload). A failed
// stop is best-effort; the source's own finaliser stops it as well.
void videocore_videobuffer_stop_quiet(videocore_videobuffer_obj_t *self) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        if (self->source != mp_const_none) {
            videosource_stop(self->source);
        }
        nlr_pop();
    }
    videocore_videobuffer_clear(self);
}

bool common_hal_videocore_videobuffer_get_playing(videocore_videobuffer_obj_t *self) {
    videocore_videobuffer_end_stream(self);
    return self->playing;
}

mp_obj_t common_hal_videocore_videobuffer_get_source(videocore_videobuffer_obj_t *self) {
    videocore_videobuffer_end_stream(self);
    return self->source;
}

// Root group validation: a VideoBuffer that streams straight into a
// framebuffer bypasses displayio rendering, so it may only be shown on a
// display that renders that same framebuffer, and only while that display is
// unrotated: its frames go straight into the framebuffer's memory, while LVGL
// rotates everything it renders. Plain buffer targets are drawn or sampled by
// the compositor, so they rotate with the display and are fine anywhere.
void videocore_videobuffer_validate_framebuffer_target(videocore_videobuffer_obj_t *self,
    mp_obj_t display, mp_int_t rotation) {
    if (rotation != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("VideoBuffer can't stream into a rotated display"));
    }
    if (display == self->framebuffer) {
        return;
    }
    #if CIRCUITPY_FRAMEBUFFERIO
    // A FramebufferDisplay renders the framebuffer it wraps. That is the only
    // kind of display a framebuffer-target VideoBuffer can be shown on.
    mp_obj_t native_display = mp_obj_cast_to_native_base(display, &framebufferio_framebufferdisplay_type);
    if (native_display != MP_OBJ_NULL) {
        framebufferio_framebufferdisplay_obj_t *fd = MP_OBJ_TO_PTR(native_display);
        if (fd->framebuffer == self->framebuffer) {
            return;
        }
    }
    #endif
    mp_raise_ValueError(MP_ERROR_TEXT("VideoBuffer is playing into a different framebuffer"));
}

// Walks the group tree and lets the backend check each VideoBuffer it finds.
static void videocore_validate_member(mp_obj_t member, mp_obj_t display, mp_int_t rotation) {
    displayio_group_t *group = mp_obj_cast_to_native_base(member, &displayio_group_type);
    if (group != MP_OBJ_NULL) {
        for (size_t i = 0; i < group->members->len; i++) {
            videocore_validate_member(group->members->items[i], display, rotation);
        }
        return;
    }
    videocore_videobuffer_backend_validate_member(member, display, rotation);
}

void videocore_validate_root_group(mp_obj_t root_group, mp_obj_t display, mp_int_t rotation) {
    videocore_validate_member(root_group, display, rotation);
}
