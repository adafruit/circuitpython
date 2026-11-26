// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/Canvas.h"

// Hardware-FPU boards run project() on plain float32 - faster than the soft-float-equivalent
// fixed path and free of 16.16 range limits; no-FPU targets (Cortex-M0+) use integer 16.16.
// Only ONE path is compiled per board. The default follows the architecture; a build can
// override it with -DCIRCUITPY_PICOGAME3D_FLOAT=0/1. Xtensa has an FPU but uses the fixed path:
// its float divide is a library call that is slower than the 16.16 path.
// Python reads `picogame3d.FLOAT` to pack camera/point buffers to match.
#ifndef CIRCUITPY_PICOGAME3D_FLOAT
#if (defined(__ARM_FP) && (__ARM_FP != 0)) || (defined(__riscv_flen) && (__riscv_flen > 0))
#define CIRCUITPY_PICOGAME3D_FLOAT (1)
#else
#define CIRCUITPY_PICOGAME3D_FLOAT (0)
#endif
#endif

// Mode-7 perspective ground plane: fill rows below `horizon` with a receding view
// of `tex` (power-of-2 dims). Args are 16.16 fixed-point (a Python helper computes
// them from camera angle/pos/fov). See picogame3d_mode7 for the exact math.
void picogame3d_mode7(picogame_canvas_obj_t *cv, picogame_bitmap_obj_t *tex,
    int horizon, int y_off, int32_t z, int32_t rx0, int32_t ry0, int32_t rsx, int32_t rsy,
    int32_t cam_x, int32_t cam_y, bool up);

// One racing-road strip: per-row spans (road/rumbles/dashes/sky) from precomputed tables.
void picogame3d_road(picogame_canvas_obj_t *cv, int ri0,
    const int16_t *tab, int ntab, const int16_t *rl, const int16_t *rr,
    int32_t d05_q8, int32_t d07_q8, const uint16_t *colors);

// Textured wall columns for one raycast frame, from its dist/wallx/col planes and a PAL8 atlas
// stored transposed (row = texture column). The plain-int arguments are also an array, so the
// binding can fill them in a loop.
typedef union {
    struct {
        int sh, stride, tex_cols, nlights, light_shift, x_off, y_off, mode, tex_id;
    };
    int v[9];
} picogame3d_tcolumns_args_t;
void picogame3d_tcolumns(picogame_canvas_obj_t *cv, picogame_bitmap_obj_t *tex,
    const int32_t *dists, const uint16_t *wallx, const uint16_t *kinds, int n,
    const uint8_t *cmap, const picogame3d_tcolumns_args_t *p);

// Racing-road curve pass (see the implementation comment in __init__.c).
void picogame3d_road_edges(int16_t *rl, int16_t *rr, const int32_t *hw_q16, int n,
    int32_t cx_q16, int32_t dist, const int32_t *cfg);

#if defined(PICOGAME_HAS_INTERP)
// rp2-port SIO-interpolator mode7 row walker. Fast path only:
// PAL8, opaque, stride == tw, log2(tw)+log2(th) <= 16; the caller guards and falls back.
void picogame3d_mode7_row_interp(uint16_t *dst, int n,
    const uint8_t *tex, const uint16_t *pal,
    uint32_t fx, uint32_t fy, int32_t stepx, int32_t stepy,
    int shx, int shy, int ltw, int lth);
#endif
