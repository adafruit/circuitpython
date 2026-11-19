// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/epaperdisplay/epaper_core.h"

#include "lvgl.h"

// Dirty rectangles kept since the last panel refresh. When more than this arrive they are
// merged into one bounding box, which is still correct because every one is inside it.
#define EPAPER_LVGL_MAX_DIRTY 8

struct epaperdisplay_epaperdisplay_obj_t {
    mp_obj_base_t base;
    epaperdisplay_epaper_core_t epaper;

    // LVGL display. The draw buffer is RGB565; the flush callback converts it into the
    // packed panel planes below.
    lv_display_t *lv_display;
    lv_color_format_t lv_color_format;
    uint8_t *lv_buf;
    size_t lv_buf_size;
    uint8_t *lv_rotation_buf;
    size_t lv_rotation_buf_size;

    // The converted framebuffer. Each byte holds (8 / canvas_depth) panel pixels, most
    // significant pixel first, exactly as the panel RAM does. black_buf always exists.
    // color_buf exists when the panel has a separate color RAM.
    uint8_t *black_buf;
    uint8_t *color_buf;
    uint16_t canvas_stride;
    uint16_t canvas_width; // Physical panel width, not the rotated display width.
    uint16_t canvas_height;
    uint8_t canvas_depth;

    // Areas the flush callback changed since the last panel refresh.
    displayio_area_t dirty[EPAPER_LVGL_MAX_DIRTY];
    uint8_t dirty_count;
    bool dirty_full;
    bool refresh_in_progress;
    bool full_refresh;
};
