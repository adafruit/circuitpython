// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

// Mode-7 inner row via the SIO INTERPOLATOR (RP2040 + RP2350). The interpolator does the
// whole per-pixel texture walk in hardware: lane0 = (fx >> shx) & (tw-1), lane1 =
// ((fy >> shy) & (th-1)) << log2(tw), POP_FULL = texture_base + lane0 + lane1 = the texel
// ADDRESS, and (ADD_RAW) both accumulators advance by their full-precision steps on the
// same pop. That replaces ~8 shift/mask/mul/add ops per pixel with one SIO read.
//
// Per-CORE hardware: each core has its own interp0, so this stays safe even if rows are
// ever split across cores (each configures its own). The engine claims no SDK
// lane locks: CircuitPython core does not use the interpolators, and the config is
// rewritten per row call anyway.
//
// Fast path constraints (the caller guards): PAL8 texture, no transparency, stride == tw,
// log2(tw)+log2(th) <= 16 (mode7 textures are 64..256 pow2 - always true there).

#include "py/mpconfig.h"

#if CIRCUITPY_PICOGAME

#include <stdint.h>
#include "hardware/interp.h"
#include "shared-module/picogame/__init__.h"   // the prototype (PICOGAME_HAS_INTERP)

void picogame_mode7_row_interp(uint16_t *dst, int n,
    const uint8_t *tex, const uint16_t *pal,
    uint32_t fx, uint32_t fy, int32_t stepx, int32_t stepy,
    int shx, int shy, int ltw, int lth) {
    interp_config c0 = interp_default_config();
    interp_config_set_shift(&c0, (uint)shx);
    interp_config_set_mask(&c0, 0, (uint)(ltw - 1));
    interp_config_set_add_raw(&c0, true);       // accumulator advances by the RAW step
    interp_set_config(interp0, 0, &c0);
    interp_config c1 = interp_default_config();
    interp_config_set_shift(&c1, (uint)(shy - ltw));
    interp_config_set_mask(&c1, (uint)ltw, (uint)(ltw + lth - 1));
    interp_config_set_add_raw(&c1, true);
    interp_set_config(interp0, 1, &c1);
    interp0->base[0] = (uint32_t)stepx;
    interp0->base[1] = (uint32_t)stepy;
    interp0->base[2] = (uint32_t)(uintptr_t)tex;
    interp0->accum[0] = fx;
    interp0->accum[1] = fy;
    for (int i = 0; i < n; i++) {
        const uint8_t *p = (const uint8_t *)interp0->pop[2];
        dst[i] = pal[*p];
    }
}

#endif
