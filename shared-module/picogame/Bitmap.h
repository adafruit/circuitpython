// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame: 2D game engine for the PicoPad and similar boards.
// Bitmap = an image atlas of one or more equal-size frames, arbitrary width/height.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "py/obj.h"

enum {
    PICOGAME_FMT_RGB565 = 0, // 2 bytes/pixel, values in display wire order
    PICOGAME_FMT_PAL8 = 1,   // 1 byte/pixel index into palette (wire-order RGB565)
};

typedef struct {
    mp_obj_base_t base;
    mp_obj_t data_obj;        // keep the source buffer alive
    mp_obj_t palette_obj;     // keep the palette buffer alive (MP_OBJ_NULL for RGB565)
    const uint8_t *data;      // pixel data
    const uint16_t *palette;  // wire-order RGB565 entries (PAL8), else NULL
    uint16_t width, height;   // size of a single frame
    uint16_t stride;          // atlas width in pixels (>= width * frames for a horizontal atlas)
    uint16_t transparent;     // transparent key: palette index (PAL8) or wire color (RGB565)
    uint16_t pal_entries;     // palette length in entries (PAL8; 0 for RGB565). Informational: the
                              // blitter does NOT clamp - indices must be < this (see PAL8 blit contract).
    uint8_t format;           // PICOGAME_FMT_*
    uint8_t frames;
    bool has_transparent;
} picogame_bitmap_obj_t;
