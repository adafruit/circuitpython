// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/framebufferio/__init__.h"
#include "py/obj.h"
#include "py/proto.h"

#include "shared-bindings/displayio/Group.h"

#include "lvgl.h"

#include "shared-module/displayio/display_core.h"

typedef struct {
    mp_obj_base_t base;
    displayio_display_core_t core;
    mp_obj_t framebuffer;
    const struct _framebuffer_p_t *framebuffer_protocol;
    mp_buffer_info_t bufinfo;
    uint64_t last_refresh_call;
    uint16_t native_frames_per_second;
    uint16_t native_ms_per_frame;
    uint16_t first_pixel_offset;
    uint16_t row_stride;
    bool auto_refresh;
    bool first_manual_refresh;
    // The LVGL display renders either directly into the provided framebuffer or into
    // an aligned partial buffer that the flush callback copies into it.
    lv_display_t *lv_display;
    lv_color_format_t lv_color_format;
    uint8_t *lv_buf;
    size_t lv_buf_size;
    uint8_t *lv_dirty_row_bitmask;
    size_t lv_dirty_row_bitmask_size;
    bool lv_direct;
    // The framebuffer reports frame boundaries, so this display renders on those instead of the
    // LVGL autorefresh tick.
    bool lv_paced;
} framebufferio_framebufferdisplay_obj_t;

void framebufferio_framebufferdisplay_background(framebufferio_framebufferdisplay_obj_t *self);
void release_framebufferdisplay(framebufferio_framebufferdisplay_obj_t *self);
void framebufferio_framebufferdisplay_reset(framebufferio_framebufferdisplay_obj_t *self);

void framebufferio_framebufferdisplay_collect_ptrs(framebufferio_framebufferdisplay_obj_t *self);

mp_obj_t common_hal_framebufferio_framebufferdisplay_get_framebuffer(framebufferio_framebufferdisplay_obj_t *self);

typedef bool (*framebuffer_get_reverse_pixels_in_byte_fun)(mp_obj_t);
typedef bool (*framebuffer_get_reverse_pixels_in_word_fun)(mp_obj_t);
typedef bool (*framebuffer_set_brightness_fun)(mp_obj_t, mp_float_t);
typedef int (*framebuffer_get_bytes_per_cell_fun)(mp_obj_t);
typedef int (*framebuffer_get_color_depth_fun)(mp_obj_t);
typedef int (*framebuffer_get_first_pixel_offset_fun)(mp_obj_t);
typedef bool (*framebuffer_get_grayscale_fun)(mp_obj_t);
typedef int (*framebuffer_get_height_fun)(mp_obj_t);
typedef int (*framebuffer_get_native_frames_per_second_fun)(mp_obj_t);
typedef bool (*framebuffer_get_pixels_in_byte_share_row_fun)(mp_obj_t);
typedef int (*framebuffer_get_row_stride_fun)(mp_obj_t);
typedef int (*framebuffer_get_width_fun)(mp_obj_t);
typedef mp_float_t (*framebuffer_get_brightness_fun)(mp_obj_t);
typedef void (*framebuffer_deinit_fun)(mp_obj_t);
typedef void (*framebuffer_get_bufinfo_fun)(mp_obj_t, mp_buffer_info_t *bufinfo);
typedef void (*framebuffer_swapbuffers_fun)(mp_obj_t, uint8_t *dirty_row_bitmask);
typedef void (*framebuffer_set_frame_callback_fun)(mp_obj_t, framebuffer_frame_callback_t, void *);

typedef struct _framebuffer_p_t {
    MP_PROTOCOL_HEAD // MP_QSTR_protocol_framebuffer

    // Mandatory
    framebuffer_get_bufinfo_fun get_bufinfo;
    framebuffer_swapbuffers_fun swapbuffers;
    framebuffer_deinit_fun deinit;
    framebuffer_get_width_fun get_width;
    framebuffer_get_height_fun get_height;

    // Optional getters
    framebuffer_get_bytes_per_cell_fun get_bytes_per_cell; // default: 2
    framebuffer_get_color_depth_fun get_color_depth; // default: 16
    framebuffer_get_first_pixel_offset_fun get_first_pixel_offset; // default: 0
    framebuffer_get_grayscale_fun get_grayscale; // default: grayscale if depth < 8
    framebuffer_get_native_frames_per_second_fun get_native_frames_per_second; // default: 60
    framebuffer_get_pixels_in_byte_share_row_fun get_pixels_in_byte_share_row; // default: false
    framebuffer_get_reverse_pixels_in_byte_fun get_reverse_pixels_in_byte; // default: false
    framebuffer_get_reverse_pixels_in_word_fun get_reverse_pixels_in_word; // default: false
    framebuffer_get_row_stride_fun get_row_stride; // default: 0 (no extra row padding)

    // Optional -- default is no brightness control
    framebuffer_get_brightness_fun get_brightness;
    framebuffer_set_brightness_fun set_brightness;

    // Optional -- called at each frame boundary. Pass a NULL callback to clear it.
    framebuffer_set_frame_callback_fun set_frame_callback;

} framebuffer_p_t;
