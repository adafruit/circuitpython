// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/displayio/Bitmap.h"
#include "shared-module/displayio/Palette.h"
#include "shared-module/displayio/lvgl/lvgl_protocol.h"

#include "lvgl.h"

// LVGL-based version of displayio TileGrid.
//
// A TileGrid owns one composed buffer holding exactly one tile grid of pixels, in tile-matrix
// order: cell (i, j) of tiles[] is at (i * tile_width, j * tile_height). The tiles are drawn
// into it through LVGL's draw pipeline, so the palette and the tile indirection are resolved
// once, when a cell changes, instead of on every frame.
//
// The display shows that buffer through up to four lv_images ("views"), because displayio
// scrolls by moving a wrapped window over the tile matrix. A scroll re-points the views and
// invalidates; no pixels move. See lvgl/TileGrid.c for the whole design.
//
// Dirty tracking: the buffer is composed at the start of an LVGL refresh (see
// displayio_lvgl_tilegrid_refresh_all()), so the write path only has to set bits.

// A wrapped window over the composed buffer is at most two row runs by two column runs.
#define DISPLAYIO_LVGL_TILEGRID_VIEWS 4

typedef struct displayio_tilegrid {
    mp_obj_base_t base;
    mp_obj_t bitmap;
    mp_obj_t pixel_shader;
    int16_t x;
    int16_t y;
    uint16_t pixel_width;
    uint16_t pixel_height;
    uint16_t bitmap_width_in_tiles;
    uint16_t tiles_in_bitmap;
    uint16_t width_in_tiles;
    uint16_t height_in_tiles;
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t top_left_x;
    uint16_t top_left_y;
    void *tiles;  // Can be either uint8_t* or uint16_t* depending on tiles_in_bitmap
    bool in_group : 1;
    bool flip_x : 1;
    bool flip_y : 1;
    bool transpose_xy  : 1;
    bool hidden : 1;
    bool hidden_by_parent : 1;
    uint16_t padding : 10;
    // LVGL mirror state.
    lv_obj_t *lv_obj;        // Container of the views, or the single image on the direct path.
    lv_obj_t *lv_views[DISPLAYIO_LVGL_TILEGRID_VIEWS];
    lv_image_dsc_t lv_view_dsc[DISPLAYIO_LVGL_TILEGRID_VIEWS];
    uint8_t lv_view_count;
    lv_draw_buf_t *lv_buf;   // Composed pixels; owned by lv_canvas once it exists.
    lv_obj_t *lv_canvas;     // Hidden canvas that hosts the compose layer.
    // The source atlas: the whole bitmap as one LVGL image, drawn one cell at a time.
    const lv_image_dsc_t *lv_src;
    lv_image_dsc_t lv_src_dsc;  // Our own descriptor for a Bitmap source.
    // A single-cell grid of a direct color source shows those pixels as they are instead of
    // copying them into a composed buffer.
    bool lv_direct;
    bool lv_src_odb_full;       // lv_src is the whole-image OnDiskBitmap decode.
    bool lv_argb;               // Composed buffer carries alpha (the source has transparent pixels).
    // Cells of the composed buffer that need composing, one bit per cell.
    uint32_t *lv_cell_dirty;
    uint32_t lv_cell_words;
    uint32_t lv_dirty_count;
    bool lv_full_dirty;
    bool lv_work;               // Has something to do at the next refresh.
    bool lv_views_dirty;        // The scroll window moved; re-point the views.
    // Pixels of the source changed. Writers mark the area before they change it, so the
    // next compose pass rereads the bitmap and recomposes the cells the area reaches.
    int16_t lv_src_x1;
    int16_t lv_src_y1;
    int16_t lv_src_x2;
    int16_t lv_src_y2;
    bool lv_src_dirty;
    struct displayio_tilegrid *lv_next;  // The compositor's list of attached TileGrids.
    struct displayio_tilegrid *lv_bitmap_next;  // The Bitmap's list of grids drawing it.
    uint16_t lv_scale;                   // Scale inherited from the parent group.
    int32_t lv_origin_x;                 // The parent group's origin in screen pixels.
    int32_t lv_origin_y;
} displayio_tilegrid_t;

void displayio_tilegrid_set_hidden_by_parent(displayio_tilegrid_t *self, bool hidden);
bool displayio_tilegrid_get_rendered_hidden(displayio_tilegrid_t *self);
void displayio_tilegrid_mark_tile_dirty(displayio_tilegrid_t *self, uint16_t x, uint16_t y);

// TileGrid mirror lifecycle, implemented in TileGrid.c and called by Group.c.
void displayio_lvgl_tilegrid_attach(displayio_tilegrid_t *self, lv_obj_t *parent);
void displayio_lvgl_tilegrid_detach(displayio_tilegrid_t *self);
void displayio_lvgl_tilegrid_update_scale(displayio_tilegrid_t *self);

// Compose whatever changed since the last refresh. Called at the start of every LVGL refresh,
// before anything is drawn, with the LVGL lock already held by the renderer.
void displayio_lvgl_tilegrid_refresh_all(void);
// The pixels of bitmap changed (or went away). Called from the Bitmap write path.
void displayio_lvgl_tilegrid_bitmap_changed(displayio_bitmap_t *bitmap,
    int16_t x1, int16_t y1, int16_t x2, int16_t y2);
// The Palette this TileGrid draws with changed.
void displayio_lvgl_tilegrid_shader_changed(displayio_tilegrid_t *self);
// Root the attached TileGrids during GC. They live in a port-memory list.
void displayio_lvgl_tilegrid_gc_collect(void);

// How Group attaches, positions and hides a TileGrid. See lvgl_protocol.h.
extern const lvgl_protocol_t displayio_lvgl_tilegrid_protocol;
