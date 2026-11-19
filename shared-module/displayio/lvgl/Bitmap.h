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

// LVGL-based version of displayio Bitmap. Same pixel storage as the legacy version (rows
// packed into uint32_t words, sub-byte values MSB-first).
//
// Deliberately no LVGL state: a TileGrid composes the tiles it shows into its own buffer, and
// reads this Bitmap's pixels (through its Palette) while doing it. So a Bitmap never owns an
// image or a palette copy; a write only has to tell the TileGrids showing it which pixels
// changed. Those grids are kept on lv_watch_head, so the write path never walks the global
// mirror list and skips LVGL entirely when nothing is showing this Bitmap.

// Forward declaration: the attached TileGrids that draw this bitmap form a list through
// their lv_bitmap_next pointer.
struct displayio_tilegrid;

typedef struct {
    mp_obj_base_t base;
    uint16_t width;
    uint16_t height;
    uint32_t *data;
    uint16_t stride; // uint32_t's
    uint8_t bits_per_value;
    uint8_t x_shift;
    size_t x_mask;
    uint16_t bitmask;
    bool read_only;
    bool data_alloc; // did bitmap allocate data or someone else
    // Attached TileGrids drawing this bitmap. Empty means a pixel write has nothing to
    // notify, so it can skip the LVGL lock and the mirror walk entirely.
    struct displayio_tilegrid *lv_watch_head;
} displayio_bitmap_t;

void displayio_bitmap_write_pixel(displayio_bitmap_t *self, int16_t x, int16_t y, uint32_t value);
// Higher-level dirty notification. The rectangular region is in bitmap pixel coordinates. The
// TileGrids drawing this bitmap note the region and re-read it before the next refresh, since
// writers mark the area *before* they change the pixels.
void displayio_bitmap_set_dirty(displayio_bitmap_t *self, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
// Compatibility for callers that still pass an area, such as bitmaptools.
void displayio_bitmap_set_dirty_area(displayio_bitmap_t *self, const displayio_area_t *area);
