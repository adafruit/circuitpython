// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame Canvas: a RAM RGB565 surface (any size) you draw primitives into,
// composited as a Scene layer. The general home for shapes (fill_rect, line,
// circle, pixel) - accumulates a dirty rect (scene coords) so only redrawn
// areas repaint. Colors are wire-order (picogame.rgb565).

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "py/obj.h"
#include "shared-module/picogame/Bitmap.h"

typedef struct {
    mp_obj_base_t base;
    uint16_t *data;      // w*h wire-order RGB565
    mp_obj_t data_obj;   // backing buffer kept alive (MP_OBJ_NULL if m_new'd)
    uint16_t w, h;
    int32_t x, y;        // scene position (int32: a canvas can sit past +-32767 px in a big world)
    uint16_t transparent;
    bool has_transparent;
    int32_t dx1, dy1, dx2, dy2;  // accumulated dirty rect (scene coords; int32, see x/y)
} picogame_canvas_obj_t;

void picogame_canvas_dirty_reset(picogame_canvas_obj_t *cv);
// Grow the dirty rect to also cover a scene-coord rect (no surface clamping).
void picogame_canvas_dirty_union(picogame_canvas_obj_t *cv, int x1, int y1, int x2, int y2);
bool picogame_canvas_take_dirty(picogame_canvas_obj_t *cv, int *x1, int *y1, int *x2, int *y2);

void picogame_canvas_clear(picogame_canvas_obj_t *cv, uint16_t color);
void picogame_canvas_pixel(picogame_canvas_obj_t *cv, int x, int y, uint16_t color);
void picogame_canvas_fill_rect(picogame_canvas_obj_t *cv, int x, int y, int w, int h, uint16_t color);
void picogame_canvas_blit(picogame_canvas_obj_t *cv, picogame_bitmap_obj_t *bm, int x, int y, int frame, bool flip_x, bool flip_y);
void picogame_canvas_rect(picogame_canvas_obj_t *cv, int x, int y, int w, int h, uint16_t color);
void picogame_canvas_line(picogame_canvas_obj_t *cv, int x0, int y0, int x1, int y1, uint16_t color);
void picogame_canvas_fill_circle(picogame_canvas_obj_t *cv, int cx, int cy, int r, uint16_t color);
void picogame_canvas_circle(picogame_canvas_obj_t *cv, int cx, int cy, int r, uint16_t color);
void picogame_canvas_ring(picogame_canvas_obj_t *cv, int cx, int cy, int r, int thick, uint16_t color);
void picogame_canvas_triangle(picogame_canvas_obj_t *cv, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);
void picogame_canvas_fill_triangle(picogame_canvas_obj_t *cv, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);
void picogame_canvas_ellipse(picogame_canvas_obj_t *cv, int cx, int cy, int rx, int ry, uint16_t color);
void picogame_canvas_fill_ellipse(picogame_canvas_obj_t *cv, int cx, int cy, int rx, int ry, uint16_t color);
void picogame_canvas_fill_round_rect(picogame_canvas_obj_t *cv, int x, int y, int w, int h, int r, uint16_t color);
void picogame_canvas_frame3d(picogame_canvas_obj_t *cv, int x, int y, int w, int h, uint16_t light, uint16_t dark);
void picogame_canvas_text(picogame_canvas_obj_t *cv, int x, int y, const char *text,
    uint16_t fg, uint16_t bg, bool has_bg, const void *font);
// Fill n RGB565 pixels at p (word stores; any 2-byte alignment).
void picogame_fill565(uint16_t *p, int n, uint16_t color);

void picogame_blit_canvas(
    uint16_t *buf, int region_w, int strip_top, int strip_h, int x0,
    picogame_canvas_obj_t *cv, int ox, int oy);

void picogame_fill_triangle_batch(picogame_canvas_obj_t *cv, const int16_t *v,
    const uint16_t *col, int n, int xo, int yo);
