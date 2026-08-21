// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "shared-module/picogame/Tilemap.h"
#include "shared-module/picogame/__init__.h"

// Floor division for b > 0.
static inline int floordiv(int a, int b) {
    return (a >= 0) ? (a / b) : -(((-a) + b - 1) / b);
}

// log2 of a power-of-2 (small tile dims), without a __ctzsi2 lib call on the M0+ (no CLZ/RBIT).
static inline int pow2_shift(unsigned v) {
    int n = 0;
    while (v > 1u) {
        v >>= 1;
        n++;
    }
    return n;
}

// Thin wrappers over the shared int32 accumulator (dx1,dy1,dx2,dy2 are contiguous int32 at the
// struct tail). See picogame_dirty_* in __init__.c.
void picogame_tilemap_dirty_reset(picogame_tilemap_obj_t *tm) {
    picogame_dirty_reset(&tm->dx1);
}

void picogame_tilemap_dirty_union(picogame_tilemap_obj_t *tm, int x1, int y1, int x2, int y2) {
    picogame_dirty_union(&tm->dx1, x1, y1, x2, y2);
}

bool picogame_tilemap_take_dirty(picogame_tilemap_obj_t *tm, int *x1, int *y1, int *x2, int *y2) {
    return picogame_dirty_take(&tm->dx1, x1, y1, x2, y2);
}

void picogame_tilemap_extent(picogame_tilemap_obj_t *tm, int *x1, int *y1, int *x2, int *y2) {
    int tw = tm->tileset ? tm->tileset->width : 0;
    int th = tm->tileset ? tm->tileset->height : 0;
    *x1 = tm->x;
    *y1 = tm->y;
    *x2 = tm->x + (int)tm->map_w * tw;
    *y2 = tm->y + (int)tm->map_h * th;
}

void picogame_blit_tilemap(
    uint16_t *buf, int region_w, int strip_top, int strip_h, int x0,
    picogame_tilemap_obj_t *tm, int ox, int oy) {
    picogame_bitmap_obj_t *ts = tm->tileset;
    if (ts == NULL) {
        return;
    }
    int tw = ts->width;
    int th = ts->height;
    int nframes = ts->frames;

    // Tilemap origin in screen coords (scene position + view offset).
    int tmx = tm->x + ox;
    int tmy = tm->y + oy;

    // Region in screen coords.
    int rx1 = x0, ry1 = strip_top;
    int rx2 = x0 + region_w, ry2 = strip_top + strip_h;

    // Tile index range overlapping the region. Tile dims are almost always powers of two (8, 16),
    // where floor division is an arithmetic shift (signed >> floors toward -inf, exactly what
    // floordiv does) - which skips 4 idiv per strip on the tilemap background. Non-pow2 tiles fall
    // back to floordiv. (shx>=0 signals the pow2 fast path; the ctz runs twice per call, not per tile.)
    int shx = (tw & (tw - 1)) ? -1 : pow2_shift((unsigned)tw);
    int shy = (th & (th - 1)) ? -1 : pow2_shift((unsigned)th);
    int tx_lo = (shx >= 0) ? ((rx1 - tmx) >> shx) : floordiv(rx1 - tmx, tw);
    int tx_hi = (shx >= 0) ? ((rx2 - 1 - tmx) >> shx) : floordiv(rx2 - 1 - tmx, tw);
    int ty_lo = (shy >= 0) ? ((ry1 - tmy) >> shy) : floordiv(ry1 - tmy, th);
    int ty_hi = (shy >= 0) ? ((ry2 - 1 - tmy) >> shy) : floordiv(ry2 - 1 - tmy, th);
    tx_lo = picogame_imax(tx_lo, 0);
    ty_lo = picogame_imax(ty_lo, 0);
    tx_hi = picogame_imin(tx_hi, (int)tm->map_w - 1);
    ty_hi = picogame_imin(ty_hi, (int)tm->map_h - 1);

    const uint8_t *map = tm->map;            // hoist per-tile decode invariants out of the inner loop
    const uint8_t *orient = tm->orient;
    int map_w = (int)tm->map_w;
    for (int ty = ty_lo; ty <= ty_hi; ty++) {
        int dy = tmy + ty * th;
        size_t row = (size_t)ty * map_w;
        for (int tx = tx_lo; tx <= tx_hi; tx++) {
            size_t cell = row + tx;
            uint8_t idx = map[cell];
            if (idx >= nframes) {
                continue;
            }
            uint8_t o = orient ? orient[cell] : 0;
            int dx = tmx + tx * tw;
            picogame_blit_bitmap(buf, region_w, strip_h, x0, strip_top,
                ts, dx, dy, idx, (o & 1) != 0, (o & 2) != 0, (o & 4) != 0, NULL);
        }
    }
}
