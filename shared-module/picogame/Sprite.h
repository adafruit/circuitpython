// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame: a positioned, animatable instance of a Bitmap.

#pragma once

#include <stdint.h>
#include "py/obj.h"
#include "shared-module/picogame/Bitmap.h"

enum {
    PICOGAME_SPR_VISIBLE = 1 << 0,
    PICOGAME_SPR_FLIP_X = 1 << 1,
    PICOGAME_SPR_FLIP_Y = 1 << 2,
    PICOGAME_SPR_SHADOW = 1 << 3,   // draw opaque pixels as a darkened destination (shadow/dim)
    PICOGAME_SPR_FLASH = 1 << 4,    // draw opaque pixels as a solid colour (hit-flash)
    PICOGAME_SPR_DITHER = 1 << 5,   // skip pixels via a Bayer pattern -> fake transparency
    PICOGAME_SPR_TINT = 1 << 6,     // multiply opaque pixels by a colour (keeps shading)
    PICOGAME_SPR_TRANSPOSE = 1 << 7, // swap x/y -> cheap 90deg (with flips = all 8 orientations)
};

typedef struct {
    mp_obj_base_t base;
    picogame_bitmap_obj_t *bitmap;
    mp_obj_t data;        // arbitrary user payload (game state); GC-scanned
    int32_t x, y;         // position, 24.8 fixed-point (1/256 px), scene coords
    uint16_t anchor_x;    // pivot as a 1/256 fraction of width: 0=left, 128~=center, 256=right
    uint16_t anchor_y;    // pivot as a 1/256 fraction of height: 0=top, 128~=center, 256=bottom
    uint16_t scale;       // uniform draw scale, 8.8 fixed-point (256 = 1.0x); nearest-neighbour
    int16_t angle;        // rotation about the anchor, whole degrees (0 = axis-aligned fast path)
    uint16_t flash_color; // FLASH mode: wire-order RGB565 that replaces opaque pixels
    uint8_t frame;
    uint8_t flags;
    uint8_t seq;          // bumped by touch() to force a repaint after an in-place bitmap mutation
    uint8_t dither;       // DITHER mode: transparency level 0..16 (0=opaque, 16=invisible)
    // ---- affine transform cache (angle != 0 path). Filled lazily on first use, invalidated
    // by the scale/angle/bitmap/anchor setters (xf_valid = 0). POSITION-INDEPENDENT: the bbox
    // is relative to the sprite's integer position, ic/is are the 16.16 inverse-map steps.
    // Saves the trig LUT + 4-corner bbox + two software divides that otherwise re-run once per
    // STRIP the sprite touches (~6x/frame at strip_h=8) plus once for the dirty-rect AABB.
    uint8_t xf_valid;
    int16_t xf_minx, xf_miny, xf_maxx, xf_maxy;  // corners bbox relative to (x>>8, y>>8)
    int32_t xf_ic, xf_is;                        // inverse-map steps (16.16)
} picogame_sprite_obj_t;
