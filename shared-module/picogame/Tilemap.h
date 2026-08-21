// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame Tilemap: a grid of tile indices into a tileset Bitmap (each tile = one
// frame of the bitmap). Rendered as a Scene layer; maintains an accumulated dirty
// rectangle (screen coords) so only changed tiles/areas are repainted.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "py/obj.h"
#include "shared-module/picogame/Bitmap.h"

typedef struct {
    mp_obj_base_t base;
    picogame_bitmap_obj_t *tileset; // each frame is a tile (tileset->width x height)
    mp_obj_t tileset_obj;           // keep alive
    uint8_t *map;                   // map_w*map_h tile indices
    mp_obj_t map_obj;               // keep alive
    uint8_t *orient;                // map_w*map_h orientation bits (bit0 flipX, bit1 flipY,
                                    // bit2 transpose); NULL until a tile sets an orientation
    mp_obj_t orient_obj;            // keep alive (lazily allocated)
    uint16_t map_w, map_h;
    int32_t x, y;                   // pixel position of tile (0,0) (int32: big maps scroll past +-32767)
    int32_t dx1, dy1, dx2, dy2;     // accumulated dirty rect (scene coords; int32, see x/y); x1>=x2 => empty
} picogame_tilemap_obj_t;

void picogame_tilemap_dirty_reset(picogame_tilemap_obj_t *tm);
void picogame_tilemap_dirty_union(picogame_tilemap_obj_t *tm, int x1, int y1, int x2, int y2);
// Returns true and fills the dirty rect if non-empty, then resets it.
bool picogame_tilemap_take_dirty(picogame_tilemap_obj_t *tm, int *x1, int *y1, int *x2, int *y2);

// On-screen bounding box of the whole map.
void picogame_tilemap_extent(picogame_tilemap_obj_t *tm, int *x1, int *y1, int *x2, int *y2);

// Blit the tiles intersecting the strip region into buf. (ox, oy) is the view
// offset added to the tilemap position (scene space -> screen space).
void picogame_blit_tilemap(
    uint16_t *buf, int region_w, int strip_top, int strip_h, int x0,
    picogame_tilemap_obj_t *tm, int ox, int oy);
