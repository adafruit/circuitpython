// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// LVGL implementation of videocore.VideoBuffer. See lvgl/VideoBuffer.h and
// shared-module/displayio/lvgl/lvgl_protocol.h.
//
// A VideoBuffer is one lv_image that samples the source's current destination
// directly: the user's buffer for a plain-buffer target, or the framebuffer
// band a framebuffer target streams into. Frames are written in place, so a
// landed frame only has to invalidate the image; there is no copy and no
// second render path. Invalidation is deferred out of the source's interrupt
// context to displayio_lvgl_video_background(), which runs on the
// CircuitPython thread at every supervisor tick.

#include "shared-module/videocore/VideoBuffer.h"

#include <string.h>

#include "py/gc.h"
#include "py/objtype.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/videocore/VideoBuffer.h"
#include "shared-module/displayio/lvgl/lvgl_internal.h"
#include "shared-module/videocore/__init__.h"

#include "misc/cache/instance/lv_image_cache.h"

#if CIRCUITPY_VIDEOCORE && CIRCUITPY_DISPLAYIO_LVGL

// Attached mirrors. The list lives in port memory, so displayio_gc_collect()
// roots it explicitly.
static videocore_videobuffer_obj_t *lvgl_videos = NULL;

static void _register(videocore_videobuffer_obj_t *self) {
    if (self->backend.lv_registered) {
        return;
    }
    self->backend.lv_next = lvgl_videos;
    lvgl_videos = self;
    self->backend.lv_registered = true;
}

static void _unregister(videocore_videobuffer_obj_t *self) {
    if (!self->backend.lv_registered) {
        return;
    }
    for (videocore_videobuffer_obj_t **link = &lvgl_videos; *link != NULL; link = &(*link)->backend.lv_next) {
        if (*link == self) {
            *link = self->backend.lv_next;
            break;
        }
    }
    self->backend.lv_next = NULL;
    self->backend.lv_registered = false;
}

// The LVGL format of the pixels the source writes.
static lv_color_format_t _video_color_format(videocore_videobuffer_obj_t *self) {
    switch (self->source_bits_per_pixel) {
        case 16:
            if (self->framebuffer_protocol != NULL &&
                self->framebuffer_protocol->get_reverse_pixels_in_word != NULL &&
                self->framebuffer_protocol->get_reverse_pixels_in_word(self->framebuffer)) {
                return LV_COLOR_FORMAT_RGB565_SWAPPED;
            }
            return LV_COLOR_FORMAT_RGB565;
        case 24:
            return LV_COLOR_FORMAT_RGB888;
        case 32:
            return LV_COLOR_FORMAT_XRGB8888;
        default:
            return LV_COLOR_FORMAT_UNKNOWN;
    }
}

// Bytes between rows of the destination the mirror samples.
static uint32_t _video_stride(videocore_videobuffer_obj_t *self) {
    if (self->framebuffer_protocol != NULL) {
        return self->fb_row_stride;
    }
    return (uint32_t)self->source_width * self->source_bits_per_pixel / 8;
}

// lv_obj_set_pos() only records a style that a later layout pass applies, and
// lv_obj_move_to() leaves that style behind, so an unrelated size change would
// snap the object back. Set both: the coordinates are right immediately and
// nothing can undo them.
static void _place(lv_obj_t *obj, int32_t x, int32_t y) {
    lv_obj_set_style_x(obj, x, 0);
    lv_obj_set_style_y(obj, y, 0);
    lv_obj_move_to(obj, x, y);
}

// Drop any cached decode of the current source. A descriptor that was never
// handed to LVGL has no cache entry.
static void _drop_cached_source(videocore_videobuffer_obj_t *self) {
    if (self->backend.lv_dsc.header.magic == LV_IMAGE_HEADER_MAGIC) {
        lv_image_cache_drop(&self->backend.lv_dsc);
    }
}

// Repaint the area an object covered. Used before moving or hiding it, when
// invalidating the object itself no longer covers where it was.
static void _invalidate_screen_area(lv_obj_t *obj, const lv_area_t *area) {
    lv_obj_t *screen = lv_obj_get_screen(obj);
    if (screen != NULL) {
        lv_obj_invalidate_area(screen, area);
    }
}

// Point the mirror at the current destination and place it where the video is.
// Called under the LVGL lock from every state change.
static void _sync_mirror(videocore_videobuffer_obj_t *self) {
    if (self->backend.lv_obj == NULL) {
        return;
    }

    const uint8_t *data = (const uint8_t *)self->stream_base;
    lv_color_format_t cf = _video_color_format(self);
    const bool visible = data != NULL && cf != LV_COLOR_FORMAT_UNKNOWN &&
        self->source_width > 0 && self->source_height > 0 && !self->hidden;

    lv_area_t old_coords;
    lv_obj_get_coords(self->backend.lv_obj, &old_coords);
    const bool was_hidden = lv_obj_has_flag(self->backend.lv_obj, LV_OBJ_FLAG_HIDDEN);

    if (!visible) {
        if (!was_hidden) {
            // Repaint the last frame with whatever is underneath it before it
            // goes away.
            lv_obj_invalidate(self->backend.lv_obj);
            lv_obj_add_flag(self->backend.lv_obj, LV_OBJ_FLAG_HIDDEN);
        } else if (old_coords.x1 != 0 || old_coords.y1 != 0) {
            // A stopped or moved video may have left pixels on the screen
            // while its mirror was already hidden.
            _invalidate_screen_area(self->backend.lv_obj, &old_coords);
        }
        return;
    }

    const uint32_t stride = _video_stride(self);
    const uint32_t row_bytes = (uint32_t)self->source_width * self->source_bits_per_pixel / 8;
    const uint32_t data_size = stride * (self->source_height - 1) + row_bytes;

    if (data != self->backend.lv_data || cf != self->backend.lv_cf ||
        self->source_width != self->backend.lv_width || self->source_height != self->backend.lv_height ||
        stride != self->backend.lv_stride) {
        // The storage or the way it is read changed. Drop cached decodes of the
        // old view; per-frame pixel writes do not need this.
        _drop_cached_source(self);
        lv_memzero(&self->backend.lv_dsc, sizeof(self->backend.lv_dsc));
        self->backend.lv_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
        self->backend.lv_dsc.header.cf = cf;
        self->backend.lv_dsc.header.w = self->source_width;
        self->backend.lv_dsc.header.h = self->source_height;
        self->backend.lv_dsc.header.stride = stride;
        self->backend.lv_dsc.data = data;
        self->backend.lv_dsc.data_size = data_size;
        self->backend.lv_data = data;
        self->backend.lv_cf = cf;
        self->backend.lv_width = self->source_width;
        self->backend.lv_height = self->source_height;
        self->backend.lv_stride = stride;
        lv_image_set_src(self->backend.lv_obj, &self->backend.lv_dsc);
    }

    // Group scaling is refused before it can change anything (see
    // _video_protocol_set_transform), so the position is the parent's origin
    // plus the video's own position.
    const int32_t x = self->absolute_transform.x + self->x;
    const int32_t y = self->absolute_transform.y + self->y;
    if (lv_obj_get_x(self->backend.lv_obj) != x || lv_obj_get_y(self->backend.lv_obj) != y) {
        if (!was_hidden) {
            lv_obj_invalidate(self->backend.lv_obj);
        }
        _place(self->backend.lv_obj, x, y);
    }
    if (lv_obj_get_width(self->backend.lv_obj) != self->source_width ||
        lv_obj_get_height(self->backend.lv_obj) != self->source_height) {
        if (!was_hidden) {
            lv_obj_invalidate(self->backend.lv_obj);
        }
        lv_obj_set_size(self->backend.lv_obj, self->source_width, self->source_height);
    }
    if (was_hidden) {
        lv_obj_clear_flag(self->backend.lv_obj, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_invalidate(self->backend.lv_obj);
}

void videocore_videobuffer_backend_construct(videocore_videobuffer_obj_t *self) {
    self->backend.lv_obj = NULL;
    self->backend.lv_data = NULL;
    self->backend.lv_cf = LV_COLOR_FORMAT_UNKNOWN;
    self->backend.lv_width = 0;
    self->backend.lv_height = 0;
    self->backend.lv_stride = 0;
    self->backend.lv_next = NULL;
    self->backend.lv_registered = false;
    self->backend.lv_pending = false;
    lv_memzero(&self->backend.lv_dsc, sizeof(self->backend.lv_dsc));
}

bool videocore_videobuffer_backend_shown(videocore_videobuffer_obj_t *self) {
    return self->absolute_transform.scale != 0 && !self->hidden &&
           self->backend.lv_obj != NULL;
}

void videocore_videobuffer_backend_check_buffer_format(videocore_videobuffer_obj_t *self,
    mp_int_t bits_per_pixel) {
    // LVGL samples the buffer itself, so it has to know the pixel format.
    (void)self;
    if (bits_per_pixel != 16 && bits_per_pixel != 24 && bits_per_pixel != 32) {
        mp_raise_ValueError(MP_ERROR_TEXT("video format must be 16, 24 or 32 bits per pixel"));
    }
}

void videocore_videobuffer_backend_dirty_current_area(videocore_videobuffer_obj_t *self) {
    // LVGL invalidates the mirror itself when it moves.
    (void)self;
}

// Called from interrupt context. The common dispatcher already added a frame that landed at
// an old destination to vacated_area.
void videocore_videobuffer_backend_on_frame(videocore_videobuffer_obj_t *self,
    const void *frame, const displayio_area_t *changed) {
    (void)frame;
    (void)changed;
    self->backend.lv_pending = true;
}

void videocore_videobuffer_backend_forget_frame(videocore_videobuffer_obj_t *self) {
    common_hal_mcu_disable_interrupts();
    // Keep a pending repaint of a vacated area: an in-flight frame still has to be erased
    // even though the frame itself no longer needs drawing.
    if (displayio_area_empty(&self->vacated_area.area)) {
        self->backend.lv_pending = false;
    }
    common_hal_mcu_enable_interrupts();
}

void videocore_videobuffer_backend_validate_member(mp_obj_t member, mp_obj_t display,
    mp_int_t rotation) {
    const lvgl_protocol_t *protocol = mp_proto_get(MP_QSTR_protocol_lvgl, member);
    if (protocol == NULL) {
        return;
    }
    mp_obj_t layer = protocol->protocol_self != NULL ? protocol->protocol_self(member) : member;
    if (mp_obj_get_type(layer) != &videocore_videobuffer_type) {
        return;
    }
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(layer);
    if (self->framebuffer_protocol == NULL) {
        return; // Plain buffer targets are sampled by LVGL and rotate with the display.
    }
    videocore_videobuffer_validate_framebuffer_target(self, display, rotation);
}

void videocore_videobuffer_backend_source_changed(videocore_videobuffer_obj_t *self) {
    if (self->backend.lv_obj == NULL) {
        // Not attached: there is no mirror to update. attach() syncs from the
        // current state, including a frame that already landed.
        return;
    }
    displayio_lvgl_lock();
    _sync_mirror(self);
    displayio_lvgl_unlock();
}

void videocore_videobuffer_backend_stream_changed(videocore_videobuffer_obj_t *self) {
    if (self->backend.lv_obj == NULL) {
        return;
    }
    displayio_lvgl_lock();
    _sync_mirror(self);
    displayio_lvgl_unlock();
}

// protocol_lvgl implementation.

static lv_obj_t *_video_protocol_root_obj(mp_obj_t layer) {
    return ((videocore_videobuffer_obj_t *)MP_OBJ_TO_PTR(layer))->backend.lv_obj;
}

static void _video_protocol_attach(mp_obj_t layer, lv_obj_t *parent) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(layer);
    if (parent == NULL) {
        return;
    }
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    // A framebuffer target's pixels land in the framebuffer's physical memory,
    // so it can only be shown on an unrotated display. LVGL rotates the whole
    // scene at flush time, which would make the image and its pixels disagree.
    if (self->framebuffer_protocol != NULL) {
        lv_display_t *display = lv_obj_get_display(parent);
        if (display != NULL && lv_display_get_rotation(display) != LV_DISPLAY_ROTATION_0) {
            displayio_lvgl_unlock_with_cleanup();
            mp_raise_ValueError(MP_ERROR_TEXT("VideoBuffer can't stream into a rotated display"));
        }
    }
    if (self->backend.lv_obj == NULL) {
        self->backend.lv_obj = lv_image_create(parent);
        if (self->backend.lv_obj == NULL) {
            displayio_lvgl_unlock_with_cleanup();
            return;
        }
        lv_obj_clear_flag(self->backend.lv_obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(self->backend.lv_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_image_set_inner_align(self->backend.lv_obj, LV_IMAGE_ALIGN_TOP_LEFT);
        lv_image_set_pivot(self->backend.lv_obj, 0, 0);
        lv_image_set_antialias(self->backend.lv_obj, false);
        lv_image_set_scale(self->backend.lv_obj, LV_SCALE_NONE);
        _register(self);
    } else if (lv_obj_get_parent(self->backend.lv_obj) != parent) {
        lv_obj_set_parent(self->backend.lv_obj, parent);
    }
    // The transform may already be known from when the member joined the
    // group; apply it now that there is a mirror and a display to drive.
    videocore_videobuffer_update_stream(self);
    _sync_mirror(self);
    displayio_lvgl_unlock_with_cleanup();
}

static void _video_protocol_detach(mp_obj_t layer) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(layer);
    displayio_lvgl_lock();
    _unregister(self);
    if (self->backend.lv_obj != NULL) {
        lv_obj_delete(self->backend.lv_obj);
        self->backend.lv_obj = NULL;
    }
    self->backend.lv_data = NULL;
    self->backend.lv_cf = LV_COLOR_FORMAT_UNKNOWN;
    self->backend.lv_width = 0;
    self->backend.lv_height = 0;
    self->backend.lv_stride = 0;
    _drop_cached_source(self);
    lv_memzero(&self->backend.lv_dsc, sizeof(self->backend.lv_dsc));
    // No mirror means the video isn't shown, which pauses a framebuffer target
    // until it is attached again.
    videocore_videobuffer_update_stream(self);
    displayio_lvgl_unlock();
}

static bool _video_protocol_set_transform(mp_obj_t layer, int32_t abs_x, int32_t abs_y,
    uint16_t scale) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(layer);
    // displayio's scale is whole pixels; LVGL's image scaling would change
    // that and take the video off the draw unit's fast path.
    if (scale != 1) {
        return false;
    }
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    displayio_buffer_transform_t transform = {
        .x = abs_x,
        .y = abs_y,
        .dx = 1,
        .dy = 1,
        .scale = scale,
    };
    self->absolute_transform = transform;
    videocore_videobuffer_update_area(self);
    // May resume a paused framebuffer target, so it may raise.
    videocore_videobuffer_update_stream(self);
    // A playing framebuffer target follows the group's translation by moving
    // its DMA destination with it.
    if (self->framebuffer_protocol != NULL) {
        videocore_videobuffer_move_dest(self);
    }
    _sync_mirror(self);
    displayio_lvgl_unlock_with_cleanup();
    return true;
}

static void _video_protocol_set_hidden_by_parent(mp_obj_t layer, bool hidden) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(layer);
    if (self->hidden == hidden) {
        return;
    }
    self->hidden = hidden;
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    videocore_videobuffer_update_stream(self);
    _sync_mirror(self);
    displayio_lvgl_unlock_with_cleanup();
}

static bool _video_protocol_set_in_group(mp_obj_t layer, bool in_group) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(layer);
    bool was_in_group = self->in_group;
    self->in_group = in_group;
    return was_in_group;
}

// A Python subclass of VideoBuffer is an mp_obj_instance_t that holds the native VideoBuffer in
// subobj[0]. Resolve it here so the methods above never see the instance.
static mp_obj_t _video_protocol_self(mp_obj_t layer) {
    mp_obj_t native = mp_obj_cast_to_native_base(layer,
        MP_OBJ_FROM_PTR(&videocore_videobuffer_type));
    if (native == MP_OBJ_NULL) {
        return layer;
    }
    mp_obj_assert_native_inited(native);
    return native;
}

const lvgl_protocol_t videocore_videobuffer_lvgl_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_lvgl)
    .protocol_self = _video_protocol_self,
    .root_obj = _video_protocol_root_obj,
    .attach = _video_protocol_attach,
    .detach = _video_protocol_detach,
    .set_transform = _video_protocol_set_transform,
    .set_hidden_by_parent = _video_protocol_set_hidden_by_parent,
    .set_in_group = _video_protocol_set_in_group,
};

void displayio_lvgl_video_background(void) {
    bool pending = false;
    for (videocore_videobuffer_obj_t *self = lvgl_videos; self != NULL; self = self->backend.lv_next) {
        if (self->backend.lv_pending) {
            pending = true;
            break;
        }
    }
    if (!pending) {
        return;
    }
    displayio_lvgl_lock();
    for (videocore_videobuffer_obj_t *self = lvgl_videos; self != NULL; self = self->backend.lv_next) {
        if (self->backend.lv_obj == NULL) {
            continue;
        }
        displayio_area_t empty = {};
        displayio_area_t vacated;
        // Take what landed so far in one step: a frame landing from here on
        // waits for the next background pass.
        common_hal_mcu_disable_interrupts();
        if (!self->backend.lv_pending) {
            common_hal_mcu_enable_interrupts();
            continue;
        }
        vacated = self->vacated_area.area;
        self->vacated_area.area = empty;
        self->vacated_area.next = NULL;
        self->backend.lv_pending = false;
        common_hal_mcu_enable_interrupts();

        if (!displayio_area_empty(&vacated)) {
            // A frame in flight landed where the video used to be. Repaint that
            // area so the stale frame doesn't stay on top of the new layout.
            // This has to happen even while hidden: the frame can land after
            // the last repaint of the area.
            _invalidate_screen_area(self->backend.lv_obj, &vacated);
        }
        if (self->hidden) {
            continue;
        }
        // The image already points at the destination the frame landed in, so
        // re-rendering the image samples the new pixels.
        lv_obj_invalidate(self->backend.lv_obj);
    }
    displayio_lvgl_unlock();
}

void displayio_lvgl_video_gc_collect(void) {
    gc_collect_root((void **)&lvgl_videos, 1);
}

bool displayio_lvgl_video_can_rotate(lv_display_t *display, lv_display_rotation_t rotation) {
    if (rotation == LV_DISPLAY_ROTATION_0) {
        return true;
    }
    for (videocore_videobuffer_obj_t *self = lvgl_videos; self != NULL; self = self->backend.lv_next) {
        if (self->framebuffer_protocol != NULL && self->backend.lv_obj != NULL &&
            lv_obj_get_display(self->backend.lv_obj) == display) {
            return false;
        }
    }
    return true;
}

#endif  // CIRCUITPY_VIDEOCORE && CIRCUITPY_DISPLAYIO_LVGL
