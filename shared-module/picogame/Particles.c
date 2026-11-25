// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "shared-module/picogame/Particles.h"
#include "shared-module/picogame/__init__.h"

static uint32_t s_prng = 0x1234abcdu;

static int32_t prng_range(int32_t lo, int32_t hi) {
    s_prng ^= s_prng << 13;
    s_prng ^= s_prng >> 17;
    s_prng ^= s_prng << 5;
    if (hi <= lo) {
        return lo;
    }
    return lo + (int32_t)(s_prng % (uint32_t)(hi - lo + 1));
}

static void swap_remove(picogame_particles_obj_t *ps, int i) {
    int last = ps->count - 1;
    ps->px[i] = ps->px[last];
    ps->py[i] = ps->py[last];
    ps->vx[i] = ps->vx[last];
    ps->vy[i] = ps->vy[last];
    ps->life[i] = ps->life[last];
    ps->life0[i] = ps->life0[last];
    ps->color[i] = ps->color[last];
    ps->count--;
}

// Blend wire-order RGB565 `wire` toward `to`: num/den of `wire`, the rest of `to` (per channel).
// With to = 0 this is the classic dim toward black.
static inline uint16_t lerp_wire565(uint16_t wire, uint16_t to, int num, int den) {
    uint16_t c = (uint16_t)((wire >> 8) | (wire << 8));   // wire -> native
    uint16_t t = (uint16_t)((to >> 8) | (to << 8));
    // One Q8 reciprocal for all three channels (0 <= q <= 256). At most 1 LSB darker.
    int q = (num << 8) / den;
    int p = 256 - q;
    int r = (((c >> 11) & 0x1F) * q + ((t >> 11) & 0x1F) * p) >> 8;
    int g = (((c >> 5) & 0x3F) * q + ((t >> 5) & 0x3F) * p) >> 8;
    int b = ((c & 0x1F) * q + (t & 0x1F) * p) >> 8;
    uint16_t out = (uint16_t)((r << 11) | (g << 5) | b);
    return (uint16_t)((out >> 8) | (out << 8));           // native -> wire
}

void picogame_particles_emit(picogame_particles_obj_t *ps, int x, int y,
    int count, int speed, int life, uint16_t color) {
    int sp = speed * 256;   // px/tick -> 8.8
    for (int k = 0; k < count && ps->count < ps->cap; k++) {
        int i = ps->count++;
        ps->px[i] = x << 8;
        ps->py[i] = y << 8;
        ps->vx[i] = (int16_t)prng_range(-sp, sp);
        ps->vy[i] = (int16_t)prng_range(-sp, sp);
        ps->life[i] = life;
        ps->life0[i] = (uint16_t)(life > 0 ? life : 1);
        ps->color[i] = color;
    }
}

void picogame_particles_update(picogame_particles_obj_t *ps) {
    int16_t g = ps->gravity;
    int x1 = 0x7fffffff, y1 = 0x7fffffff, x2 = -0x7fffffff - 1, y2 = -0x7fffffff - 1;  // INT32: big-world
    int sz = ps->size;
    int i = 0;
    while (i < ps->count) {
        ps->px[i] += ps->vx[i];
        ps->py[i] += ps->vy[i];
        ps->vy[i] += g;
        if (ps->life[i] == 0) {
            swap_remove(ps, i);
            continue;   // re-process the swapped-in particle
        }
        ps->life[i]--;
        int sx = ps->px[i] >> 8;
        int sy = ps->py[i] >> 8;
        if (ps->stretch) {                    // the streak's tail lies one velocity back
            int tx = (ps->px[i] - ps->vx[i]) >> 8;
            int ty = (ps->py[i] - ps->vy[i]) >> 8;
            if (tx < x1) {
                x1 = tx;
            }
            if (ty < y1) {
                y1 = ty;
            }
            if (tx + sz > x2) {
                x2 = tx + sz;
            }
            if (ty + sz > y2) {
                y2 = ty + sz;
            }
        }
        if (sx < x1) {
            x1 = sx;
        }
        if (sy < y1) {
            y1 = sy;
        }
        if (sx + sz > x2) {
            x2 = sx + sz;
        }
        if (sy + sz > y2) {
            y2 = sy + sz;
        }
        i++;
    }
    // current-frame bbox becomes "previous" on the next take_dirty
    ps->px1 = ps->cx1;
    ps->py1 = ps->cy1;
    ps->px2 = ps->cx2;
    ps->py2 = ps->cy2;
    if (ps->count > 0) {
        ps->cx1 = x1;
        ps->cy1 = y1;
        ps->cx2 = x2;
        ps->cy2 = y2;
    } else {
        picogame_dirty_reset(&ps->cx1);   // empty -> INT32 sentinels (cx1,cy1,cx2,cy2 are contiguous)
    }
}

bool picogame_particles_take_dirty(picogame_particles_obj_t *ps,
    int *x1, int *y1, int *x2, int *y2) {
    int ax1 = ps->cx1 < ps->px1 ? ps->cx1 : ps->px1;
    int ay1 = ps->cy1 < ps->py1 ? ps->cy1 : ps->py1;
    int ax2 = ps->cx2 > ps->px2 ? ps->cx2 : ps->px2;
    int ay2 = ps->cy2 > ps->py2 ? ps->cy2 : ps->py2;
    // consume the previous box so a static system stops reporting dirty
    ps->px1 = ps->cx1;
    ps->py1 = ps->cy1;
    ps->px2 = ps->cx2;
    ps->py2 = ps->cy2;
    if (ax1 >= ax2 || ay1 >= ay2) {
        return false;
    }
    *x1 = ax1;
    *y1 = ay1;
    *x2 = ax2;
    *y2 = ay2;
    return true;
}

void picogame_particles_clear(picogame_particles_obj_t *ps) {
    // Move the currently-drawn region into "previous" and empty "current", so the next
    // take_dirty reports the old pixels ONCE (erasing the cleared particles) then goes quiet.
    ps->px1 = ps->cx1;
    ps->py1 = ps->cy1;
    ps->px2 = ps->cx2;
    ps->py2 = ps->cy2;
    picogame_dirty_reset(&ps->cx1);       // empty -> INT32 sentinels (cx1,cy1,cx2,cy2 are contiguous)
    ps->count = 0;
}

void picogame_blit_particles(
    uint16_t *buf, int region_w, int strip_top, int strip_h, int x0,
    picogame_particles_obj_t *ps, int ox, int oy) {
    int sz = ps->size;
    int rx2 = x0 + region_w;
    int ry2 = strip_top + strip_h;
    // Reject on Y first: a strip spans the full width, so X rarely rejects.
    const int32_t *pxs = ps->px, *pys = ps->py;
    int ylo = strip_top - sz - oy;             // py >> 8 <= ylo -> entirely above
    int yhi = ry2 - oy;                        // py >> 8 >= yhi -> entirely below
    int count = ps->count;
    int steps = ps->stretch;
    for (int i = 0; i < count; i++) {
        int sy8 = pys[i] >> 8;
        if (steps == 0 && (sy8 <= ylo || sy8 >= yhi)) {   // a streak's tail may still reach the strip
            continue;
        }
        uint16_t c = ps->color[i];
        if (ps->fade) {
            c = lerp_wire565(c, ps->fade_to, ps->life[i], ps->life0[i]);
        }
        // copy j of steps+1 sits j/steps of one velocity back: head at the particle, tail one tick behind
        for (int j = 0; j <= steps; j++) {
            int32_t bx = pxs[i], by = pys[i];
            if (j) {
                bx -= ps->vx[i] * j / steps;
                by -= ps->vy[i] * j / steps;
            }
            int sx = (bx >> 8) + ox;
            int sy = (by >> 8) + oy;
            int xs = picogame_imax(sx, x0);
            int ys = picogame_imax(sy, strip_top);
            int xe = picogame_imin(sx + sz, rx2);
            int ye = picogame_imin(sy + sz, ry2);
            if (xs >= xe || ys >= ye) {
                continue;
            }
            for (int y = ys; y < ye; y++) {
                uint16_t *dst = buf + (y - strip_top) * region_w + (xs - x0);
                for (int x = xs; x < xe; x++) {
                    *dst++ = c;
                }
            }
        }
    }
}
