// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Legacy scanline compositor implementation of videocore.VideoBuffer. The shared
// stream/target state lives in shared-module/videocore/VideoBuffer.c; this file owns the
// draw protocol and the dirty-area bookkeeping the scanline compositor needs.

#include <string.h>

#include "shared-module/videocore/VideoBuffer.h"

#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/videocore/VideoBuffer.h"

void videocore_videobuffer_backend_construct(videocore_videobuffer_obj_t *self) {
    displayio_area_t empty = {};
    self->backend.frame_ready = false;
    self->backend.frame_changed_area.area = empty;
    self->backend.frame_changed_area.next = NULL;
    self->backend.frame_replaceable = false;
    self->backend.frame_dirty_area.area = empty;
    self->backend.frame_dirty_area.next = NULL;
    self->backend.ephemeral_dirty_area.area = empty;
    self->backend.ephemeral_dirty_area.next = NULL;
}

bool videocore_videobuffer_backend_shown(videocore_videobuffer_obj_t *self) {
    return self->absolute_transform.scale != 0 && !self->hidden;
}

void videocore_videobuffer_backend_stream_changed(videocore_videobuffer_obj_t *self) {
    (void)self;
}

void videocore_videobuffer_backend_source_changed(videocore_videobuffer_obj_t *self) {
    (void)self;
}

void videocore_videobuffer_backend_check_buffer_format(videocore_videobuffer_obj_t *self,
    mp_int_t bits_per_pixel) {
    // The scanline compositor checks the display's depth when it copies the frame.
    (void)self;
    (void)bits_per_pixel;
}

// Called from interrupt context. A frame that landed where the video used to be was
// already added to the shared vacated_area by the common on_frame() dispatcher.
void videocore_videobuffer_backend_on_frame(videocore_videobuffer_obj_t *self,
    const void *frame, const displayio_area_t *changed) {
    if (frame != self->stream_base) {
        return;
    }
    const displayio_area_t whole_frame = {
        .x2 = self->source_width,
        .y2 = self->source_height,
    };
    if (changed == NULL) {
        self->backend.frame_changed_area.area = whole_frame;
        self->backend.frame_replaceable = true;
    } else {
        // Keep a bad source rect from dirtying other layers.
        displayio_area_t clipped;
        if (!displayio_area_compute_overlap(changed, &whole_frame, &clipped)) {
            return;
        }
        displayio_area_union(&self->backend.frame_changed_area.area, &clipped,
            &self->backend.frame_changed_area.area);
        self->backend.frame_replaceable = false;
    }
    self->backend.frame_ready = true;
}

void videocore_videobuffer_backend_forget_frame(videocore_videobuffer_obj_t *self) {
    displayio_area_t empty = {};
    common_hal_mcu_disable_interrupts();
    self->backend.frame_ready = false;
    self->backend.frame_changed_area.area = empty;
    common_hal_mcu_enable_interrupts();
}

// Add the currently rendered rect to the dirty area so the next refresh repaints what a
// move leaves behind. Accumulates across moves. Does nothing while the video isn't shown.
void videocore_videobuffer_backend_dirty_current_area(videocore_videobuffer_obj_t *self) {
    if (self->absolute_transform.scale == 0 || self->hidden) {
        return;
    }
    displayio_area_union(&self->backend.ephemeral_dirty_area.area, &self->current_area.area,
        &self->backend.ephemeral_dirty_area.area);
}

void videocore_videobuffer_backend_validate_member(mp_obj_t member, mp_obj_t display,
    mp_int_t rotation) {
    const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, member);
    if (draw_protocol == NULL) {
        return;
    }
    mp_obj_t layer = draw_protocol->draw_get_protocol_self(member);
    if (mp_obj_get_type(layer) != &videocore_videobuffer_type) {
        return;
    }
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(layer);
    if (self->framebuffer_protocol == NULL) {
        return; // Plain buffer targets are drawn, and rotated, by displayio.
    }
    videocore_videobuffer_validate_framebuffer_target(self, display, rotation);
}

// The source pixel that a screen pixel shows along one axis. d and origin come
// from the transform. A mirrored axis (d < 0) runs the other way, so the source
// pixel ends at the origin edge instead of starting there.
static mp_int_t videocore_videobuffer_source_axis(int8_t d, mp_int_t screen,
    mp_int_t origin, mp_int_t offset) {
    if (d > 0) {
        return screen - origin - offset;
    }
    return origin - screen - 1 - offset;
}

// Bytes in one row of a source frame. 1-bit pixels are packed and each row is
// padded to a whole byte. (The shared code has the same helper; it is static there.)
static size_t videocore_videobuffer_source_row_bytes(videocore_videobuffer_obj_t *self) {
    return ((size_t)self->source_width * self->source_bits_per_pixel + 7) / 8;
}

// Draw protocol implementation. displayio copies plain-buffer frames into the
// display. Framebuffer-target frames are already in place; fill_area only
// copies them into displayio's scratch buffer when other layers overlap.

static mp_obj_t videocore_videobuffer_draw_protocol_self(mp_obj_t protocol_container) {
    return protocol_container;
}

// Whether a layer above already claimed a pixel near the overlap. Checks whole words, which may
// include pixels between overlap rows or just outside the overlap. That can only make the caller
// take its slower path; it never misses a claimed pixel.
static bool videocore_videobuffer_overlap_claimed(const uint32_t *mask, const displayio_area_t *area,
    const displayio_area_t *overlap) {
    const size_t span_px = displayio_area_width(area);
    const size_t first_px = (size_t)(overlap->y1 - area->y1) * span_px + (overlap->x1 - area->x1);
    const size_t end_px = (size_t)(overlap->y2 - 1 - area->y1) * span_px + (overlap->x2 - area->x1);
    for (size_t word = first_px / 32; word <= (end_px - 1) / 32; word++) {
        if (mask[word] != 0) {
            return true;
        }
    }
    return false;
}

// Copy a 1 bit per pixel plain buffer into a 1 bit display's buffer. Both are MSB first with
// 1 = white, the same as a monochrome EPaperDisplay's first RAM pass before any inversion.
// On a rotated display each screen pixel is walked back to its source pixel, as the 16 bit copy
// does. Whole bytes are copied when the source runs forwards along screen x, the rects line up
// and nothing above claimed these pixels.
static bool videocore_videobuffer_fill_area_1bit(videocore_videobuffer_obj_t *self,
    const _displayio_colorspace_t *colorspace, const displayio_area_t *area, const displayio_area_t *overlap,
    bool full_coverage, uint32_t *mask, uint32_t *buffer) {
    // Every grayscale bit of a 1 bit pixel is that pixel, so this also serves each grayscale pass.
    if (!colorspace->pixels_in_byte_share_row || !colorspace->reverse_pixels_in_byte ||
        !colorspace->grayscale) {
        return false;
    }
    const size_t span_px = displayio_area_width(area);
    const size_t width = displayio_area_width(overlap);
    const size_t row_bytes = videocore_videobuffer_source_row_bytes(self);
    const size_t x_in_area = overlap->x1 - area->x1;
    const uint8_t *frame = self->stream_base;
    uint8_t *out = (uint8_t *)buffer;

    // While the video isn't shown there is no transform and the orientations match.
    const displayio_buffer_transform_t *t = &self->absolute_transform;
    const bool shown = t->scale != 0;
    const int8_t dx = shown ? t->dx : 1;
    const int8_t dy = shown ? t->dy : 1;
    const bool transpose = shown && t->transpose_xy;
    const mp_int_t origin_x = shown ? t->x : 0;
    const mp_int_t origin_y = shown ? t->y : 0;
    const bool contiguous = !transpose && dx > 0;
    // The first source column of every row when the source runs along screen x.
    const mp_int_t first_x = videocore_videobuffer_source_axis(dx, overlap->x1, origin_x, self->x);

    const bool whole_bytes = contiguous && span_px % 8 == 0 && x_in_area % 8 == 0 && first_x % 8 == 0 &&
        width % 8 == 0 && !videocore_videobuffer_overlap_claimed(mask, area, overlap);
    for (int16_t y = overlap->y1; y < overlap->y2; y++) {
        const size_t px = (size_t)(y - area->y1) * span_px + x_in_area;
        if (!contiguous) {
            // Mirrored or transposed: walk each screen pixel back to its source pixel. Screen x
            // steps along a source row, or down a source column on a transposed display.
            mp_int_t source_x;
            mp_int_t source_y;
            if (transpose) {
                source_x = videocore_videobuffer_source_axis(dy, y, origin_y, self->x);
                source_y = videocore_videobuffer_source_axis(dx, overlap->x1, origin_x, self->y);
            } else {
                source_x = videocore_videobuffer_source_axis(dx, overlap->x1, origin_x, self->x);
                source_y = videocore_videobuffer_source_axis(dy, y, origin_y, self->y);
            }
            for (size_t i = 0; i < width; i++) {
                const size_t p = px + i;
                if (mask[p / 32] & (1u << (p % 32))) {
                    continue;
                }
                const mp_int_t step = (mp_int_t)i * dx;
                const mp_int_t fx = transpose ? source_x : source_x + step;
                const mp_int_t fy = transpose ? source_y + step : source_y;
                if (frame[(size_t)fy * row_bytes + (size_t)fx / 8] & (0x80 >> (fx % 8))) {
                    out[p / 8] |= 0x80 >> (p % 8);
                }
                mask[p / 32] |= 1u << (p % 32);
            }
            continue;
        }
        const size_t x_in_frame = first_x;
        const mp_int_t source_y = videocore_videobuffer_source_axis(dy, y, origin_y, self->y);
        const uint8_t *row = frame + (size_t)source_y * row_bytes;
        if (whole_bytes) {
            memcpy(out + px / 8, row + x_in_frame / 8, width / 8);
            // Full coverage ends the layer walk, so filling the mask once below is enough.
            if (!full_coverage) {
                for (size_t p = px; p < px + width; p++) {
                    mask[p / 32] |= 1u << (p % 32);
                }
            }
            continue;
        }
        for (size_t i = 0; i < width; i++) {
            const size_t p = px + i;
            if (mask[p / 32] & (1u << (p % 32))) {
                continue;
            }
            const size_t f = x_in_frame + i;
            if (row[f / 8] & (0x80 >> (f % 8))) {
                out[p / 8] |= 0x80 >> (p % 8);
            }
            mask[p / 32] |= 1u << (p % 32);
        }
    }
    if (whole_bytes && full_coverage) {
        // A FramebufferDisplay writes nothing back when the mask is empty.
        memset(mask, 0xff, (displayio_area_size(area) + 31) / 32 * sizeof(uint32_t));
    }
    return full_coverage;
}

static bool videocore_videobuffer_draw_fill_area(mp_obj_t self_in, const _displayio_colorspace_t *colorspace, const displayio_area_t *area, uint32_t *mask, uint32_t *buffer) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // buffer isn't necessarily the framebuffer. It may be a scratch buffer.
    if (self->stream_base == NULL || self->hidden) {
        return false;
    }

    displayio_area_t overlap;
    if (!displayio_area_compute_overlap(area, &self->current_area.area, &overlap)) {
        return false;
    }
    const bool full_coverage = displayio_area_equal(area, &overlap);
    const uint16_t span_px = displayio_area_width(area);
    const size_t overlap_width = displayio_area_width(&overlap);

    if (self->framebuffer_protocol != NULL) {
        // The source already wrote these pixels into the framebuffer. If
        // nothing else covers this area, claim no pixels so the display skips
        // its write-back. Otherwise copy the video's unclaimed pixels into
        // the scratch buffer and claim them, since the scratch buffer is
        // written back over the framebuffer.
        const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
        const mp_int_t fb_width = FB_GETTER(get_width, 0);
        const mp_int_t row_stride = FB_GETTER(get_row_stride, fb_width * self->source_bits_per_pixel / 8);
        const uint8_t *frame = (const uint8_t *)self->stream_base;
        uint8_t *out = (uint8_t *)buffer;

        if (full_coverage && !videocore_videobuffer_overlap_claimed(mask, area, &overlap)) {
            // Sole coverer: nothing was composed, so the driver skips the copy.
            return true;
        }

        for (int16_t y = overlap.y1; y < overlap.y2; y++) {
            const size_t px = (size_t)(y - area->y1) * span_px + (overlap.x1 - area->x1);
            const int16_t fy = y - self->current_area.area.y1;
            const uint8_t *row = frame + (size_t)fy * row_stride +
                (size_t)(overlap.x1 - self->current_area.area.x1) * bytes_per_pixel;
            size_t i = 0;
            while (i < overlap_width) {
                // Skip pixels a higher layer already claimed or drew.
                while (i < overlap_width && (mask[(px + i) / 32] & (1u << ((px + i) % 32)))) {
                    i++;
                }
                const size_t start = i;
                while (i < overlap_width && !(mask[(px + i) / 32] & (1u << ((px + i) % 32)))) {
                    mask[(px + i) / 32] |= 1u << ((px + i) % 32);
                    i++;
                }
                if (i > start) {
                    memcpy(
                        out + (px + start) * bytes_per_pixel,
                        row + start * bytes_per_pixel,
                        (i - start) * bytes_per_pixel);
                }
            }
        }
        return full_coverage;
    }

    if (colorspace->depth == 1 && self->source_bits_per_pixel == 1) {
        return videocore_videobuffer_fill_area_1bit(self, colorspace, area, &overlap, full_coverage, mask, buffer);
    }

    // Raw buffer target: copy the pixels, with no color conversion: the
    // display depth must match the video's pixel format byte for byte.
    if (colorspace->depth != self->source_bits_per_pixel || colorspace->depth != 16) {
        return false;
    }

    const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
    const uint8_t *frame = (const uint8_t *)self->stream_base;
    // displayio renders in screen coordinates (the framebuffer's physical
    // ones) while the frame is stored in the source's own orientation, so
    // walk from each screen pixel back to its source pixel. A plain buffer
    // target is rotated with the display; while the video isn't shown there
    // is no transform and the orientations match.
    const displayio_buffer_transform_t *t = &self->absolute_transform;
    const bool shown = t->scale != 0;
    const int8_t dx = shown ? t->dx : 1;
    const int8_t dy = shown ? t->dy : 1;
    const bool transpose = shown && t->transpose_xy;
    const mp_int_t origin_x = shown ? t->x : 0;
    const mp_int_t origin_y = shown ? t->y : 0;
    // Source pixels run along screen x only when the frame isn't transposed,
    // and forwards only when screen x isn't mirrored.
    const bool contiguous = !transpose && dx > 0;

    for (int16_t y = overlap.y1; y < overlap.y2; y++) {
        const size_t px = (size_t)(y - area->y1) * span_px + (overlap.x1 - area->x1);
        uint16_t *out = (uint16_t *)buffer + px;
        // The first source pixel this row shows. The frame's column and row
        // follow the transform, which swaps them on a transposed display.
        mp_int_t source_x;
        mp_int_t source_y;
        if (transpose) {
            source_x = videocore_videobuffer_source_axis(dy, y, origin_y, self->x);
            source_y = videocore_videobuffer_source_axis(dx, overlap.x1, origin_x, self->y);
        } else {
            source_x = videocore_videobuffer_source_axis(dx, overlap.x1, origin_x, self->x);
            source_y = videocore_videobuffer_source_axis(dy, y, origin_y, self->y);
        }
        // Where the next screen x leads: the next source column, or on a
        // transposed display the next source row.
        const ptrdiff_t step = transpose
            ? (ptrdiff_t)dx * (ptrdiff_t)self->source_width * (ptrdiff_t)bytes_per_pixel
            : (ptrdiff_t)dx * (ptrdiff_t)bytes_per_pixel;
        const uint8_t *in = frame +
            ((size_t)source_y * self->source_width + (size_t)source_x) * bytes_per_pixel;
        if (contiguous) {
            size_t i = 0;
            while (i < overlap_width) {
                // Skip pixels a higher layer already claimed or drew.
                while (i < overlap_width && (mask[(px + i) / 32] & (1u << ((px + i) % 32)))) {
                    i++;
                }
                const size_t start = i;
                while (i < overlap_width && !(mask[(px + i) / 32] & (1u << ((px + i) % 32)))) {
                    mask[(px + i) / 32] |= 1u << ((px + i) % 32);
                    i++;
                }
                if (i > start) {
                    memcpy(
                        out + start,
                        in + start * bytes_per_pixel,
                        (i - start) * bytes_per_pixel);
                    if (colorspace->reverse_bytes_in_word) {
                        for (size_t j = start; j < i; j++) {
                            out[j] = __builtin_bswap16(out[j]);
                        }
                    }
                }
            }
        } else {
            // Mirrored or transposed: the source pixels between screen pixels
            // aren't next to each other, so copy them one by one.
            for (size_t i = 0; i < overlap_width; i++) {
                if (mask[(px + i) / 32] & (1u << ((px + i) % 32))) {
                    continue;
                }
                mask[(px + i) / 32] |= 1u << ((px + i) % 32);
                uint16_t value;
                memcpy(&value, in + (ptrdiff_t)i * step, bytes_per_pixel);
                if (colorspace->reverse_bytes_in_word) {
                    value = __builtin_bswap16(value);
                }
                out[i] = value;
            }
        }
    }
    return full_coverage;
}

static bool videocore_videobuffer_draw_get_dirty_area(mp_obj_t self_in, displayio_area_t *out_area) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    out_area->x1 = out_area->x2;
    displayio_area_union(&self->backend.ephemeral_dirty_area.area, &self->current_area.area, out_area);
    return true;
}

static void videocore_videobuffer_draw_update_transform(mp_obj_t self_in, displayio_buffer_transform_t *group_transform) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Remember where the video was so a move can repaint the pixels it
    // vacates. The area is only valid while the video is shown; a layer being
    // attached or detached is handled by the display's full refresh.
    displayio_area_t previous_area = self->current_area.area;
    const bool was_shown = self->absolute_transform.scale != 0 && !self->hidden;
    // A group that isn't attached to a display has no transform, and its
    // update_transform clears the absolute transform we receive. A real
    // (shown) transform always has scale >= 1.
    if (group_transform == NULL || group_transform->scale == 0) {
        displayio_buffer_transform_t no_transform = {};
        self->absolute_transform = no_transform;
    } else {
        // Validate the transform. A framebuffer target streams rows straight
        // into the framebuffer's memory, so it can't turn with a rotated
        // display. This is also checked when a group becomes the display's
        // root group, but rotation can change afterwards.
        if (self->framebuffer_protocol != NULL &&
            (group_transform->mirror_x || group_transform->mirror_y ||
             group_transform->transpose_xy)) {
            mp_raise_ValueError(MP_ERROR_TEXT("VideoBuffer can't stream into a rotated display"));
        }
        // A plain buffer target is copied into the display, so a rotated
        // display works: its transform mirrors and/or transposes the logical
        // position, which the copy applies to the frame. Neither can scale.
        if (group_transform->scale != 1 ||
            group_transform->dx == 0 || group_transform->dy == 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("VideoBuffer does not support transforms"));
        }
        self->absolute_transform = *group_transform;
    }

    // A framebuffer target starts or stops with whether it is shown (the
    // absolute transform is known now). This runs from Python contexts, so a
    // (re)start may raise.
    videocore_videobuffer_update_stream(self);

    videocore_videobuffer_update_area(self);

    // Moved by a containing Group (or a transform change): dirty the rect it
    // used to occupy so the refresh repaints those pixels instead of leaving
    // the previous frame there.
    if (was_shown && self->absolute_transform.scale != 0 &&
        !displayio_area_equal(&previous_area, &self->current_area.area)) {
        displayio_area_union(
            &self->backend.ephemeral_dirty_area.area, &previous_area, &self->backend.ephemeral_dirty_area.area);
        if (self->framebuffer_protocol == NULL) {
            // Draw the last frame at its new position.
            videocore_videobuffer_backend_dirty_current_area(self);
        } else if (!self->playing) {
            // A repaint overwrites an ended stream's last frame.
            self->stream_base = NULL;
        }
    }

    // Framebuffer targets stream straight into the buffer: move the
    // destination along with the shift. Best effort, since this can't raise:
    // the shift may land the start mid-row, which won't look right but is
    // obvious; the frame is capped to stay completely within the buffer.
    if (self->framebuffer_protocol != NULL) {
        videocore_videobuffer_move_dest(self);
    }
}

static void videocore_videobuffer_draw_finish_refresh(mp_obj_t self_in) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    displayio_area_t empty = {};
    self->backend.ephemeral_dirty_area.area = empty;
    self->backend.ephemeral_dirty_area.next = NULL;
    // A frame that landed during the refresh waits for the next one, unless its
    // successor repaints everything anyway.
    if (self->backend.frame_replaceable) {
        videocore_videobuffer_forget_frame(self);
    }
    videocore_videobuffer_end_stream(self);
}

static displayio_area_t *videocore_videobuffer_draw_get_refresh_areas(mp_obj_t self_in, displayio_area_t *tail) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Take what landed so far in one step: a frame landing from here on waits
    // for the next refresh.
    displayio_area_t empty = {};
    common_hal_mcu_disable_interrupts();
    displayio_area_t vacated = self->vacated_area.area;
    const bool frame_ready = self->backend.frame_ready;
    self->backend.frame_dirty_area.area = self->backend.frame_changed_area.area;
    self->vacated_area.area = empty;
    self->vacated_area.next = NULL;
    self->backend.frame_ready = false;
    self->backend.frame_changed_area.area = empty;
    common_hal_mcu_enable_interrupts();
    // Frames that landed where the video used to be, possibly over the repaint
    // of that area: repaint there again.
    displayio_area_union(&self->backend.ephemeral_dirty_area.area, &vacated, &self->backend.ephemeral_dirty_area.area);
    // Always repaint the area the video vacated, even once it is hidden or
    // stopped, so its last frame doesn't stay on the target.
    if (!displayio_area_empty(&self->backend.ephemeral_dirty_area.area)) {
        // The video moved or was hidden: repaint with the lower layers.
        tail = videocore_area_link(&self->backend.ephemeral_dirty_area, tail);
    }
    if (self->stream_base == NULL || self->paused || self->hidden || self->stale_size) {
        return tail;
    }
    if (frame_ready) {
        // New frame: copy the part that changed. A plain buffer target is
        // rotated as it is copied, so map the changed source pixels to the
        // screen; a framebuffer target's frame lands unrotated at the current
        // area, so shift them there.
        if (self->framebuffer_protocol == NULL) {
            displayio_area_t changed = self->backend.frame_dirty_area.area;
            videocore_videobuffer_source_rect(self, &changed, &self->backend.frame_dirty_area);
        } else {
            displayio_area_shift(
                &self->backend.frame_dirty_area.area, self->current_area.area.x1, self->current_area.area.y1);
        }
        tail = videocore_area_link(&self->backend.frame_dirty_area, tail);
    }
    return tail;
}

static void videocore_videobuffer_draw_set_dirty(mp_obj_t self_in) {
    (void)self_in;
}

static void videocore_videobuffer_draw_set_hidden(mp_obj_t self_in, bool hidden) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->hidden == hidden) {
        return;
    }
    videocore_videobuffer_end_stream(self);
    self->hidden = hidden;
    // Repaint what the video showed or vacated.
    self->backend.ephemeral_dirty_area.area = self->current_area.area;
    if (!hidden && self->playing) {
        // The source normally can't change while the sensor stays streaming,
        // but validate its sizing again in case it did: if it no longer
        // matches, don't raise; simply stop showing the stream until stop and
        // play again.
        const videosource_p_t *protocol = mp_proto_get(MP_QSTR_protocol_videosource, self->source);
        if (protocol != NULL &&
            (protocol->get_width(self->source) != self->source_width ||
             protocol->get_height(self->source) != self->source_height ||
             protocol->get_bits_per_pixel(self->source) != self->source_bits_per_pixel)) {
            self->stale_size = true;
        }
    }
    // Hiding or unhiding a framebuffer target pauses or resumes its source.
    videocore_videobuffer_update_stream(self);
}

static bool videocore_videobuffer_draw_set_in_group(mp_obj_t self_in, bool in_group) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool was_in_group = self->in_group;
    self->in_group = in_group;
    return was_in_group;
}

static vectorio_draw_protocol_impl_t videocore_videobuffer_draw_protocol_impl = {
    .draw_fill_area = videocore_videobuffer_draw_fill_area,
    .draw_get_dirty_area = videocore_videobuffer_draw_get_dirty_area,
    .draw_update_transform = videocore_videobuffer_draw_update_transform,
    .draw_finish_refresh = videocore_videobuffer_draw_finish_refresh,
    .draw_get_refresh_areas = videocore_videobuffer_draw_get_refresh_areas,
    .draw_set_dirty = videocore_videobuffer_draw_set_dirty,
    .draw_set_hidden = videocore_videobuffer_draw_set_hidden,
    .draw_set_in_group = videocore_videobuffer_draw_set_in_group,
};

const vectorio_draw_protocol_t videocore_videobuffer_draw_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_draw)
    .draw_get_protocol_self = videocore_videobuffer_draw_protocol_self,
    .draw_protocol_impl = &videocore_videobuffer_draw_protocol_impl,
};
