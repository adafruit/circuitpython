// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "shared-module/picogame3d/__init__.h"
#include "shared-module/picogame/__init__.h"

// Mark rows y0..y1 of the canvas dirty (the full width), in scene coordinates.
static void mark_rows(picogame_canvas_obj_t *cv, int y0, int y1) {
    if (y0 < y1 && cv->w > 0) {
        picogame_canvas_dirty_union(cv, cv->x, cv->y + y0, cv->x + cv->w, cv->y + y1);
    }
}

// Number of low zero bits (log2 for a power of 2; 0 for non-pow2, caught by caller).
static int log2_pow2(unsigned v) {
    int n = 0;
    while ((v & 1) == 0 && v > 1) {
        v >>= 1;
        n++;
    }
    return n;
}

// Fill pixels xs..xe (inclusive) of one canvas row, clipped to the row width.
static inline void row_span(uint16_t *row, int w, int xs, int xe, uint16_t color) {
    if (xs < 0) {
        xs = 0;
    }
    if (xe >= w) {
        xe = w - 1;
    }
    if (xs <= xe) {
        picogame_fill565(row + xs, xe - xs + 1, color);
    }
}

// One racing-road strip, all rows in one call (the OutRun-genre "draw_road" scanline loop - profiled
// at ~20-25 ms of Python on picobike: ~4 fill_rect boundary crossings + an int(float) phase per row).
// ri0 = the road-table row of THIS surface's row 0 (vy - horizon_base); negative rows are sky.
// tab = int16[ntab][5]: {edge_w, dash_hw, wb05_q8, wb07_q8, flags(bit0 = dashes allowed)} - static per
// game. rl/rr = per-frame integer road edges (road_edges output). d05/d07 = the frame's scrolling
// stripe/dash phases in Q8; the row's band parity is ((d05+wb05)>>8)&1, matching the Python
// int(d05f + wb05f) & 1 (both non-negative). colors = uint16[6]: {sky, road_a, road_b, rumble_a,
// rumble_b, dash}. Grass underneath and the finish-line chequer stay the caller's job (one fill_rect
// per strip / a few rows near the lap line - no reason to carry them in C).
void picogame3d_road(picogame_canvas_obj_t *cv, int ri0,
    const int16_t *tab, int ntab, const int16_t *rl, const int16_t *rr,
    int32_t d05_q8, int32_t d07_q8, const uint16_t *colors) {
    int w = cv->w;
    for (int ly = 0; ly < cv->h; ly++) {
        int ri = ri0 + ly;
        uint16_t *row = &cv->data[ly * w];
        if (ri < 0) {                                // above the horizon: sky
            picogame_fill565(row, w, colors[0]);
            continue;
        }
        if (ri >= ntab) {
            ri = ntab - 1;
        }
        const int16_t *t = tab + ri * 5;
        int band = (int)(((d05_q8 + t[2]) >> 8) & 1);
        uint16_t road = band ? colors[1] : colors[2];
        uint16_t rumble = band ? colors[3] : colors[4];
        int l = rl[ri], r = rr[ri];
        if (r <= l) {
            continue;
        }
        row_span(row, w, l, r - 1, road);           // fill_rect(l, w=r-l) covers l..r-1
        int ew = t[0];
        row_span(row, w, l, l + ew - 1, rumble);
        row_span(row, w, r - ew, r - 1, rumble);
        if ((t[4] & 1) && (((d07_q8 + t[3]) >> 8) & 1)) {
            int mid = (l + r) >> 1, dw = t[1];
            row_span(row, w, mid - dw, mid + dw - 1, colors[5]);
        }
    }
    mark_rows(cv, 0, cv->h);
}

// Context + row walker for the mode7 loop (each row derives rowdist/steps from its own sy).
typedef struct {
    picogame_canvas_obj_t *cv;
    const uint8_t *data;
    const uint16_t *pal;
    int fmt, stride, shx, shy, mx, my, horizon, y_off;
    int32_t z, rx0, ry0, rsx, rsy, cam_x, cam_y;
    bool transp;
    int32_t key;
    bool up;                                    // ceiling: rows above the horizon
} mode7_ctx_t;

static void mode7_rows(void *arg, int lo, int hi) {
    mode7_ctx_t *c = arg;
    picogame_canvas_obj_t *cv = c->cv;
    int w = cv->w;
    #if defined(PICOGAME_HAS_INTERP)
    int ltw = 16 - c->shx;
    int lth = 16 - c->shy;
    bool use_interp = (c->fmt == PICOGAME_FMT_PAL8) && !c->transp && c->pal != NULL
        && c->stride == c->mx + 1
        && ltw >= 1 && lth >= 1 && ltw + lth <= 16;   // lane1 shift = shy-ltw must be >= 0
    #endif
    for (int sy = lo; sy < hi; sy++) {
        // A ceiling mirrors the floor about the horizon, so one sign flip covers both.
        int denom = (sy + c->y_off) - c->horizon;
        if (c->up) {
            denom = -denom;
        }
        if (denom <= 0) {
            continue;
        }
        // 32-bit throughout (no 64-bit mul helper on the M0+): rowdist*coeff stays
        // within int32 for sane camera params - the Python helper keeps z and the
        // ray deltas small; extreme values degrade to wrong pixels, never a crash.
        int32_t rowdist = c->z / denom;
        int32_t stepx = (rowdist * c->rsx) >> 16;
        int32_t stepy = (rowdist * c->rsy) >> 16;
        int32_t fx = c->cam_x + ((rowdist * c->rx0) >> 16);
        int32_t fy = c->cam_y + ((rowdist * c->ry0) >> 16);
        uint16_t *drow = cv->data + sy * w;
        #if defined(PICOGAME_HAS_INTERP)
        if (use_interp) {
            picogame3d_mode7_row_interp(drow, w, c->data, c->pal,
                (uint32_t)fx, (uint32_t)fy, stepx, stepy, c->shx, c->shy, ltw, lth);
            continue;
        }
        #endif
        for (int sx = 0; sx < w; sx++) {
            int tx = (fx >> c->shx) & c->mx, ty = (fy >> c->shy) & c->my;
            uint16_t val;
            if (src_pixel_s(c->fmt, c->data, c->pal, c->key, ty * c->stride + tx, &val)) {
                drow[sx] = val;
            }
            fx += stepx;
            fy += stepy;
        }
    }
}

void picogame3d_mode7(picogame_canvas_obj_t *cv, picogame_bitmap_obj_t *tex,
    int horizon, int y_off, int32_t z, int32_t rx0, int32_t ry0, int32_t rsx, int32_t rsy,
    int32_t cam_x, int32_t cam_y, bool up) {
    // Perspective ground plane (Mode-7 / floorcaster). For each screen row below
    // `horizon`, distance = z / (row - horizon) (16.16); the texture-space coord
    // of the left edge is cam + distance*ray0, stepping by distance*rayDelta per
    // pixel; sample `tex` (power-of-2, so wrap = a mask, and world 1.0 = one tile
    // via a shift, no multiply). Integer throughout - no FPU needed (RP2040).
    if (tex == NULL) {
        return;
    }
    int tw = tex->width, th = tex->height;
    if ((tw & (tw - 1)) || (th & (th - 1))) {   // require power-of-2 dims
        return;
    }
    int shx = 16 - log2_pow2((unsigned)tw);     // world(1.0) -> one full tile
    int shy = 16 - log2_pow2((unsigned)th);
    int mx = tw - 1, my = th - 1, stride = tex->stride;
    int fmt = tex->format;
    const uint8_t *data = tex->data;
    const uint16_t *pal = tex->palette;
    bool transp = tex->has_transparent;           // still gates the interp fast path above
    int32_t key = picogame_key_of(tex);
    // sy is a row WITHIN this surface (a StripDraw view is a Canvas onto one strip);
    // the absolute screen row is sy + y_off, so the horizon test uses that. y_off = 0
    // for a full-screen Canvas, = the strip's screen y for a StripDraw view (0-RAM floor).
    // A ceiling takes the rows above the horizon instead.
    int y0 = picogame_imax(horizon - y_off + 1, 0), y1 = cv->h;
    if (up) {
        y0 = 0;
        y1 = picogame_imin(horizon - y_off, cv->h);
    }
    mode7_ctx_t ctx = {
        cv, data, pal, fmt, stride, shx, shy, mx, my, horizon, y_off,
        z, rx0, ry0, rsx, rsy, cam_x, cam_y, transp, key, up
    };
    mode7_rows(&ctx, y0, y1);
    mark_rows(cv, y0, y1);
}

// Textured wall columns (the Doom column renderer). For each column, walk the rows with a 16.16
// texture accumulator through a light-level colormap. The transposed atlas makes one texture
// column a contiguous run, and power-of-two sizes turn the wrap into a mask.
void picogame3d_tcolumns(picogame_canvas_obj_t *cv, picogame_bitmap_obj_t *tex,
    const int32_t *dists, const uint16_t *wallx, const uint16_t *kinds, int n,
    const uint8_t *cmap, const picogame3d_tcolumns_args_t *p) {
    const int sh = p->sh, stride = p->stride, tex_cols = p->tex_cols, nlights = p->nlights;
    const int light_shift = p->light_shift, x_off = p->x_off, y_off = p->y_off;
    int texels = tex->width;
    if (tex->format != PICOGAME_FMT_PAL8 || tex->palette == NULL
        || texels <= 0 || (texels & (texels - 1)) || tex_cols <= 0 || (tex_cols & (tex_cols - 1))
        || n <= 0 || stride <= 0 || nlights <= 0) {
        return;
    }
    const uint16_t *pal = tex->palette;
    const int cw = cv->w, ch = cv->h, mask = texels - 1, half = sh >> 1;
    for (int i = 0; i < n; i++) {
        int32_t perp = dists[i];
        int type = kinds[i] >> 1;
        if (perp <= 0 || type < 1) {
            continue;
        }
        int lh = (int)(((int32_t)sh << 16) / perp);   // full projected height, unclipped
        int t = half - (lh >> 1) + y_off;
        int b = t + lh;
        int x0 = picogame_imax(i * stride + x_off, 0);
        int x1 = picogame_imin(i * stride + x_off + stride, cw);
        int row = (type - 1) * tex_cols + ((wallx[i] * tex_cols) >> 16);
        if (lh <= 0 || b <= 0 || t >= ch || x1 <= x0 || (unsigned)row >= tex->height) {
            continue;                               // a type past the atlas draws nothing
        }
        const uint8_t *src = tex->data + (size_t)row * tex->stride;
        int light = picogame_imin((int)(perp >> light_shift) + (kinds[i] & 1), nlights - 1);
        const uint8_t *lm = cmap + (light << 8);
        int32_t vstep = (int32_t)(((uint32_t)texels << 16) / (uint32_t)lh);
        int ct = picogame_imax(t, 0), cb = picogame_imin(b, ch);
        int32_t v0 = (ct - t) * vstep;
        for (int x = x0; x < x1; x++) {
            uint16_t *dst = cv->data + ct * cw + x;
            int32_t v = v0;
            for (int y = ct; y < cb; y++) {
                *dst = pal[lm[src[(v >> 16) & mask]]];
                dst += cw;
                v += vstep;
            }
        }
    }
    int x0 = picogame_imax(x_off, 0), x1 = picogame_imin(x_off + n * stride, cw);
    if (x1 > x0) {
        picogame_canvas_dirty_union(cv, cv->x + x0, cv->y, cv->x + x1, cv->y + ch);
    }
}

// sin of a Q16 degree angle: lerp between whole-degree LUT entries. The racing-road curvature is
// DOUBLE-integrated over ~170 rows, which amplifies whole-degree quantization into visible pixels
// (host-measured 9 px); one lerp per curvature eval brings the road within 1 px of the float original.
static int32_t pg_sin_q15_lerp(int64_t deg_q16) {
    // Reduce once; d0 + 1 is then at most 360, which the table handles.
    int d0 = (int)(deg_q16 >> 16) % 360;
    if (d0 < 0) {
        d0 += 360;
    }
    int32_t frac = (int32_t)(deg_q16 & 0xFFFF);
    int32_t a = picogame_sin_q15_reduced(d0);
    // Adjacent entries differ by < 600 and frac < 2^16, so this fits in 32 bits.
    return a + (((picogame_sin_q15_reduced(d0 + 1) - a) * frac) >> 16);
}

// (s * amp) >> 15 with 32-bit multiplies. Exact for |s| <= 2^15 and any int32 amp.
static inline int32_t pg_mulshr15(int32_t s, int32_t amp) {
    uint32_t hi = (uint32_t)(amp >> 15) * (uint32_t)s;
    int32_t lo = ((amp & 0x7FFF) * s) >> 15;
    return (int32_t)(hi + (uint32_t)lo);
}

// One racing-road frame's curve pass: the bottom-up curvature accumulator + per-row integer edges
// (the OutRun-genre "compute_road" loop - profiled at ~8-10 ms of Python on picobike; this is the
// batch-boundary rule in action: one C call does the frame's whole row loop). Fixed-point throughout:
// cx/ddx accumulate in Q16, curvature = two LUT sines of the world distance. cfg (int32[7]):
// [f1_q20, f2_q20, amp1k_q16, amp2k_q16, world_step, curve_step, d_row_off] - frequencies in
// Q20 degrees/world-unit (Q16 phase-drifts over a long run), amplitudes premultiplied by the
// per-row gain k, curvature re-evaluated every curve_step rows (it varies slowly). Edge stores use
// trunc-toward-zero to match the Python original's int(). Host-proven <=1 px absolute AND row-delta
// smoothness vs the float reference over 8k+ frames (road_edges_test.c).
void picogame3d_road_edges(int16_t *rl, int16_t *rr, const int32_t *hw_q16, int n,
    int32_t cx_q16, int32_t dist, const int32_t *cfg) {
    int32_t f1 = cfg[0], f2 = cfg[1], a1k = cfg[2], a2k = cfg[3];
    int32_t wstep = cfg[4], cstep = cfg[5], drow = cfg[6];
    int32_t cx = cx_q16, ddx = 0, ck = 0;
    int cnt = 0;
    for (int i = n - 1; i >= 0; i--) {
        if (cnt == 0) {
            int32_t d = dist + (drow - i) * wstep;
            ck = pg_mulshr15(pg_sin_q15_lerp(((int64_t)d * f1) >> 4), a1k)
                + pg_mulshr15(pg_sin_q15_lerp(((int64_t)d * f2) >> 4), a2k);
            cnt = cstep;
        }
        cnt--;
        ddx += ck;
        cx += ddx;
        int32_t vl = cx - hw_q16[i], vr = cx + hw_q16[i];
        rl[i] = (int16_t)(vl >= 0 ? (vl >> 16) : -((-vl) >> 16));
        rr[i] = (int16_t)(vr >= 0 ? (vr >> 16) : -((-vr) >> 16));
    }
}
