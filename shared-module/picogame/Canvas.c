// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-module/picogame/Canvas.h"
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/fontio/BuiltinFont.h"
#include "shared-bindings/displayio/Bitmap.h"

// Thin wrappers over the shared int32 accumulator (dx1,dy1,dx2,dy2 are contiguous int32 at the
// struct tail). See picogame_dirty_* in __init__.c.
void picogame_canvas_dirty_reset(picogame_canvas_obj_t *cv) {
    picogame_dirty_reset(&cv->dx1);
}

void picogame_canvas_dirty_union(picogame_canvas_obj_t *cv, int x1, int y1, int x2, int y2) {
    picogame_dirty_union(&cv->dx1, x1, y1, x2, y2);
}

bool picogame_canvas_take_dirty(picogame_canvas_obj_t *cv, int *x1, int *y1, int *x2, int *y2) {
    return picogame_dirty_take(&cv->dx1, x1, y1, x2, y2);
}

// Union a canvas-local rect (clamped to the surface) into the dirty rect (scene coords).
static void mark(picogame_canvas_obj_t *cv, int lx1, int ly1, int lx2, int ly2) {
    if (lx1 < 0) {
        lx1 = 0;
    }
    if (ly1 < 0) {
        ly1 = 0;
    }
    if (lx2 > cv->w) {
        lx2 = cv->w;
    }
    if (ly2 > cv->h) {
        ly2 = cv->h;
    }
    if (lx1 >= lx2 || ly1 >= ly2) {
        return;
    }
    picogame_dirty_union(&cv->dx1, cv->x + lx1, cv->y + ly1, cv->x + lx2, cv->y + ly2);
}

// NOT inlined on purpose: the shape primitives call put() many times (circle =
// 8 calls/iteration). Inlining bloated them (circle was ~1.4 KB); a real call keeps
// them small. Shapes aren't the hot path (the sprite/tilemap blits don't use put).
static __attribute__((noinline)) void put(picogame_canvas_obj_t *cv, int x, int y, uint16_t c) {
    if (x >= 0 && y >= 0 && x < cv->w && y < cv->h) {
        cv->data[y * cv->w + x] = c;
    }
}

// Fill `n` RGB565 pixels at `p` with `color`, word-filling two pixels per store (half the writes of
// a 16-bit loop); memset for the common 0 case. Handles a leading odd (2-byte-but-not-4-byte) address
// so it stays safe on Cortex-M0+ (RP2040), which faults on an unaligned 32-bit access - a StripDraw
// view's rows into the render strip can start on an odd pixel. This is the per-frame path for
// view.clear / Sky / HUD-bar / Fade fills, so the word-fill is worth it.
static void fill565(uint16_t *p, int n, uint16_t color) {
    if (n <= 0) {
        return;
    }
    if (color == 0) {
        memset(p, 0, (size_t)n * 2);
        return;
    }
    if ((uintptr_t)p & 3) {                    // align to 4 bytes: one leading pixel
        *p++ = color;
        n--;
    }
    uint32_t w = (uint32_t)color | ((uint32_t)color << 16);
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    uint32_t *w32 = (uint32_t *)p;             // now 4-byte aligned
    #pragma GCC diagnostic pop
    int nw = n >> 1;
    for (int i = 0; i < nw; i++) {
        w32[i] = w;
    }
    if (n & 1) {                               // trailing odd pixel
        p[n - 1] = color;
    }
}

// Defined with the filled shapes below; forward-declared for picogame_canvas_road.
static void span565(picogame_canvas_obj_t *cv, int y, int xs, int xe, uint16_t color);

void picogame_canvas_clear(picogame_canvas_obj_t *cv, uint16_t color) {
    fill565(cv->data, cv->w * cv->h, color);
    mark(cv, 0, 0, cv->w, cv->h);
}

void picogame_canvas_pixel(picogame_canvas_obj_t *cv, int x, int y, uint16_t color) {
    put(cv, x, y, color);
    mark(cv, x, y, x + 1, y + 1);
}

void picogame_canvas_fill_rect(picogame_canvas_obj_t *cv, int x, int y, int w, int h, uint16_t color) {
    int x2 = x + w, y2 = y + h;
    int cx1 = x < 0 ? 0 : x, cy1 = y < 0 ? 0 : y;
    int cx2 = x2 > cv->w ? cv->w : x2, cy2 = y2 > cv->h ? cv->h : y2;
    for (int yy = cy1; yy < cy2; yy++) {
        fill565(cv->data + yy * cv->w + cx1, cx2 - cx1, color);
    }
    mark(cv, x, y, x2, y2);
}

void picogame_canvas_blit(picogame_canvas_obj_t *cv, picogame_bitmap_obj_t *bm,
    int x, int y, int frame, bool flip_x, bool flip_y) {
    // Composite a bitmap FRAME into the canvas buffer (honours the bitmap's transparent key).
    // Reuses the sprite blit path, targeting the canvas's own RGB565 surface instead of a strip.
    picogame_blit_bitmap(cv->data, cv->w, cv->h, 0, 0, bm, x, y, frame, flip_x, flip_y, false, NULL);
    mark(cv, x, y, x + bm->width, y + bm->height);
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

// One racing-road strip, all rows in one call (the OutRun-genre "draw_road" scanline loop - profiled
// at ~20-25 ms of Python on picobike: ~4 fill_rect boundary crossings + an int(float) phase per row).
// ri0 = the road-table row of THIS surface's row 0 (vy - horizon_base); negative rows are sky.
// tab = int16[ntab][5]: {edge_w, dash_hw, wb05_q8, wb07_q8, flags(bit0 = dashes allowed)} - static per
// game. rl/rr = per-frame integer road edges (road_edges output). d05/d07 = the frame's scrolling
// stripe/dash phases in Q8; the row's band parity is ((d05+wb05)>>8)&1, matching the Python
// int(d05f + wb05f) & 1 (both non-negative). colors = uint16[6]: {sky, road_a, road_b, rumble_a,
// rumble_b, dash}. Grass underneath and the finish-line chequer stay the caller's job (one fill_rect
// per strip / a few rows near the lap line - no reason to carry them in C).
void picogame_canvas_road(picogame_canvas_obj_t *cv, int ri0,
    const int16_t *tab, int ntab, const int16_t *rl, const int16_t *rr,
    int32_t d05_q8, int32_t d07_q8, const uint16_t *colors) {
    int w = cv->w;
    for (int ly = 0; ly < cv->h; ly++) {
        int ri = ri0 + ly;
        if (ri < 0) {                                // above the horizon: sky
            fill565(&cv->data[ly * w], w, colors[0]);
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
        span565(cv, ly, l, r - 1, road);             // fill_rect(l, w=r-l) covers l..r-1
        int ew = t[0];
        span565(cv, ly, l, l + ew - 1, rumble);
        span565(cv, ly, r - ew, r - 1, rumble);
        if ((t[4] & 1) && (((d07_q8 + t[3]) >> 8) & 1)) {
            int mid = (l + r) >> 1, dw = t[1];
            span565(cv, ly, mid - dw, mid + dw - 1, colors[5]);
        }
    }
    mark(cv, 0, 0, w, cv->h);
}

// Context + row walker for the mode7 loop (each row derives rowdist/steps from its own sy).
typedef struct {
    picogame_canvas_obj_t *cv;
    const uint8_t *data;
    const uint16_t *pal;
    int fmt, stride, shx, shy, mx, my, horizon, y_off;
    int32_t z, rx0, ry0, rsx, rsy, cam_x, cam_y;
    bool transp;
    uint16_t key;
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
        int denom = (sy + c->y_off) - c->horizon;
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
            picogame_mode7_row_interp(drow, w, c->data, c->pal,
                (uint32_t)fx, (uint32_t)fy, stepx, stepy, c->shx, c->shy, ltw, lth);
            continue;
        }
        #endif
        for (int sx = 0; sx < w; sx++) {
            int tx = (fx >> c->shx) & c->mx, ty = (fy >> c->shy) & c->my;
            uint16_t val;
            if (src_pixel_s(c->fmt, c->data, c->pal, c->transp, c->key, ty * c->stride + tx, &val)) {
                drow[sx] = val;
            }
            fx += stepx;
            fy += stepy;
        }
    }
}

void picogame_canvas_mode7(picogame_canvas_obj_t *cv, picogame_bitmap_obj_t *tex,
    int horizon, int y_off, int32_t z, int32_t rx0, int32_t ry0, int32_t rsx, int32_t rsy,
    int32_t cam_x, int32_t cam_y) {
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
    bool transp = tex->has_transparent;
    uint16_t key = tex->transparent;
    // sy is a row WITHIN this surface (a StripDraw view is a Canvas onto one strip);
    // the absolute screen row is sy + y_off, so the horizon test uses that. y_off = 0
    // for a full-screen Canvas, = the strip's screen y for a StripDraw view (0-RAM floor).
    int y0 = horizon - y_off + 1;
    if (y0 < 0) {
        y0 = 0;
    }
    mode7_ctx_t ctx = {
        cv, data, pal, fmt, stride, shx, shy, mx, my, horizon, y_off,
        z, rx0, ry0, rsx, rsy, cam_x, cam_y, transp, key
    };
    mode7_rows(&ctx, y0, cv->h);
    mark(cv, 0, y0, cv->w, cv->h);
}

void picogame_canvas_rect(picogame_canvas_obj_t *cv, int x, int y, int w, int h, uint16_t color) {
    picogame_canvas_fill_rect(cv, x, y, w, 1, color);
    picogame_canvas_fill_rect(cv, x, y + h - 1, w, 1, color);
    picogame_canvas_fill_rect(cv, x, y, 1, h, color);
    picogame_canvas_fill_rect(cv, x + w - 1, y, 1, h, color);
}

void picogame_canvas_line(picogame_canvas_obj_t *cv, int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = x1 - x0, dy = y1 - y0;
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;
    int sx = dx < 0 ? -1 : 1;
    int sy = dy < 0 ? -1 : 1;
    int err = adx - ady;
    int x = x0, y = y0;
    while (true) {
        put(cv, x, y, color);
        if (x == x1 && y == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -ady) {
            err -= ady;
            x += sx;
        }
        if (e2 < adx) {
            err += adx;
            y += sy;
        }
    }
    int lx1 = x0 < x1 ? x0 : x1, ly1 = y0 < y1 ? y0 : y1;
    int lx2 = (x0 > x1 ? x0 : x1) + 1, ly2 = (y0 > y1 ? y0 : y1) + 1;
    mark(cv, lx1, ly1, lx2, ly2);
}

// Clamp a row span to the surface and word-fill it (the span-pass idiom shared by the filled
// shapes; the per-pixel put() loops it replaced clipped and indexed every pixel).
static inline int64_t edge_slope(int32_t dx, int32_t dy) {
    if (dx >= -32768 && dx <= 32767) {
        return (int32_t)(dx << 16) / dy;
    }
    return ((int64_t)dx << 16) / dy;
}

static void span565(picogame_canvas_obj_t *cv, int y, int xs, int xe, uint16_t color) {
    if (y < 0 || y >= cv->h) {
        return;
    }
    if (xs < 0) {
        xs = 0;
    }
    if (xe >= cv->w) {
        xe = cv->w - 1;
    }
    if (xs <= xe) {
        fill565(&cv->data[y * cv->w + xs], xe - xs + 1, color);
    }
}

void picogame_canvas_fill_circle(picogame_canvas_obj_t *cv, int cx, int cy, int r, uint16_t color) {
    // A filled circle IS fill_ellipse(r, r) - the ellipse row condition s^2*ry2 <= rr - dy^2*rx2
    // collapses to s^2 <= r^2 - dy^2 (host-proven byte-exact over 36k cases). Same delegation the
    // OUTLINE circle already does; only r == 0 needs care (the ellipse rejects rx <= 0, a zero-radius
    // circle is one pixel). Flash: this replaced a ~320 B twin of the ellipse body.
    if (r < 0) {
        return;
    }
    if (r == 0) {
        picogame_canvas_pixel(cv, cx, cy, color);
        return;
    }
    picogame_canvas_fill_ellipse(cv, cx, cy, r, r, color);
}

void picogame_canvas_circle(picogame_canvas_obj_t *cv, int cx, int cy, int r, uint16_t color) {
    picogame_canvas_ellipse(cv, cx, cy, r, r, color);   // a circle is an ellipse with rx == ry
}

void picogame_canvas_ring(picogame_canvas_obj_t *cv, int cx, int cy, int r, int thick, uint16_t color) {
    if (r < 0) {
        return;
    }
    int inner = r - thick;
    if (inner < 0) {
        inner = 0;
    }
    // Mirrored rows + decremental outer/inner widths, two word-filled spans per row - see fill_circle.
    int out = r, ins = inner;
    for (int dy = 0; dy <= r; dy++) {
        long rr = (long)r * r - (long)dy * dy;     // long (like ellipse): int r*r overflows at big radii
        while ((long)out * out > rr) {
            out--;
        }
        int two_seg = dy <= inner;
        if (two_seg) {
            int ri = inner * inner - dy * dy;
            while (ins * ins > ri) {
                ins--;
            }
        }
        for (int half = 0; half < (dy ? 2 : 1); half++) {
            int y = half ? cy - dy : cy + dy;
            if (two_seg) {
                span565(cv, y, cx - out, cx - ins - 1, color);
                span565(cv, y, cx + ins + 1, cx + out, color);
            } else {
                span565(cv, y, cx - out, cx + out, color);
            }
        }
    }
    mark(cv, cx - r, cy - r, cx + r + 1, cy + r + 1);
}

void picogame_canvas_triangle(picogame_canvas_obj_t *cv,
    int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    picogame_canvas_line(cv, x0, y0, x1, y1, color);
    picogame_canvas_line(cv, x1, y1, x2, y2, color);
    picogame_canvas_line(cv, x2, y2, x0, y0, color);
}

void picogame_canvas_fill_triangle(picogame_canvas_obj_t *cv,
    int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    int X[3] = { x0, x1, x2 }, Y[3] = { y0, y1, y2 };
    for (int i = 0; i < 2; i++) {
        for (int j = i + 1; j < 3; j++) {
            if (Y[j] < Y[i]) {
                int t = Y[i];
                Y[i] = Y[j];
                Y[j] = t;
                t = X[i];
                X[i] = X[j];
                X[j] = t;
            }
        }
    }
    // Scanline fill via 16.16 edge DDA + word-filled spans. One 64-bit divide per EDGE replaces two
    // 32-bit divides per ROW (M0+ has no HW divide, ~75 cyc each; a 20-row wall paid ~40 divides),
    // and each row goes through fill565 (word stores) instead of a per-pixel clipped put(). Rows and
    // spans clamp to the canvas up front, so a mostly off-screen triangle costs only its visible rows
    // (the old loop walked EVERY row of huge triangles and clipped per pixel - quadratic blowup).
    // Edge x differs from the old divide by at most 1 px (trunc vs floor; host-verified over 100k
    // triangles), and convex quads - the box faces the 3D demos draw - stay seam-hole-free.
    int w = cv->w, h = cv->h;
    if (Y[0] < h && Y[2] >= 0) {
        int64_t sAC = (Y[2] != Y[0]) ? edge_slope(X[2] - X[0], Y[2] - Y[0]) : 0;
        int64_t sAB = (Y[1] != Y[0]) ? edge_slope(X[1] - X[0], Y[1] - Y[0]) : 0;
        int64_t sBC = (Y[2] != Y[1]) ? edge_slope(X[2] - X[1], Y[2] - Y[1]) : 0;
        uint16_t *data = cv->data;
        // top half: rows [Y0, Y1) walk edges A->C and A->B
        int ys = Y[0] < 0 ? 0 : Y[0];
        int ye = (Y[1] - 1) < (h - 1) ? (Y[1] - 1) : (h - 1);
        int64_t accAC = ((int64_t)X[0] << 16) + sAC * (ys - Y[0]);
        int64_t acc2 = ((int64_t)X[0] << 16) + sAB * (ys - Y[0]);
        for (int y = ys; y <= ye; y++) {
            int xac = (int)(accAC >> 16);
            int xsh = (int)(acc2 >> 16);
            int xs = xac < xsh ? xac : xsh, xe = xac < xsh ? xsh : xac;
            if (xs < 0) {
                xs = 0;
            }
            if (xe >= w) {
                xe = w - 1;
            }
            if (xs <= xe) {
                fill565(&data[y * w + xs], xe - xs + 1, color);
            }
            accAC += sAC;
            acc2 += sAB;
        }
        // bottom half: rows [Y1, Y2] walk edges A->C and B->C (a flat bottom degenerates to sBC=0)
        ys = Y[1] < 0 ? 0 : Y[1];
        ye = Y[2] < (h - 1) ? Y[2] : (h - 1);
        accAC = ((int64_t)X[0] << 16) + sAC * (ys - Y[0]);
        acc2 = ((int64_t)X[1] << 16) + sBC * (ys - Y[1]);
        for (int y = ys; y <= ye; y++) {
            int xac = (int)(accAC >> 16);
            int xsh = (int)(acc2 >> 16);
            int xs = xac < xsh ? xac : xsh, xe = xac < xsh ? xsh : xac;
            if (xs < 0) {
                xs = 0;
            }
            if (xe >= w) {
                xe = w - 1;
            }
            if (xs <= xe) {
                fill565(&data[y * w + xs], xe - xs + 1, color);
            }
            accAC += sAC;
            acc2 += sBC;
        }
    }
    int mnx = X[0] < X[1] ? X[0] : X[1];
    mnx = mnx < X[2] ? mnx : X[2];
    int mxx = X[0] > X[1] ? X[0] : X[1];
    mxx = mxx > X[2] ? mxx : X[2];
    mark(cv, mnx, Y[0], mxx + 1, Y[2] + 1);
}

void picogame_canvas_ellipse(picogame_canvas_obj_t *cv, int cx, int cy, int rx, int ry, uint16_t color) {
    if (rx <= 0 || ry <= 0) {
        return;
    }
    // 32-bit `long`: rx2*ry2 stays in range while rx*ry <= 46340 (both radii <~210 px). That covers any
    // canvas that fits in RAM on this target. A larger ellipse (only reachable on a big-RAM board with an
    // oversized canvas) renders a wrong shape - never a fault, since put() clips every pixel to the canvas.
    long rx2 = (long)rx * rx, ry2 = (long)ry * ry, rr = rx2 * ry2;
    for (int dy = -ry; dy <= ry; dy++) {
        int s = 0;
        while ((long)(s + 1) * (s + 1) * ry2 + (long)dy * dy * rx2 <= rr) {
            s++;
        }
        put(cv, cx - s, cy + dy, color);
        put(cv, cx + s, cy + dy, color);
    }
    for (int dx = -rx; dx <= rx; dx++) {
        int s = 0;
        while ((long)(s + 1) * (s + 1) * rx2 + (long)dx * dx * ry2 <= rr) {
            s++;
        }
        put(cv, cx + dx, cy - s, color);
        put(cv, cx + dx, cy + s, color);
    }
    mark(cv, cx - rx, cy - ry, cx + rx + 1, cy + ry + 1);
}

void picogame_canvas_fill_ellipse(picogame_canvas_obj_t *cv, int cx, int cy, int rx, int ry, uint16_t color) {
    if (rx <= 0 || ry <= 0) {
        return;
    }
    // 32-bit `long`: rx2*ry2 stays in range while rx*ry <= 46340 (both radii <~210 px). That covers any
    // canvas that fits in RAM on this target. A larger ellipse (only reachable on a big-RAM board with an
    // oversized canvas) renders a wrong shape - never a fault, since put() clips every pixel to the canvas.
    long rx2 = (long)rx * rx, ry2 = (long)ry * ry, rr = rx2 * ry2;
    // Mirrored rows + decremental width, spans word-filled - see fill_circle.
    int s = rx;
    for (int dy = 0; dy <= ry; dy++) {
        long lim = rr - (long)dy * dy * rx2;       // s*s*ry2 <= lim <=> the old (s+1)-increment bound
        while ((long)s * s * ry2 > lim) {
            s--;
        }
        span565(cv, cy + dy, cx - s, cx + s, color);
        if (dy) {
            span565(cv, cy - dy, cx - s, cx + s, color);
        }
    }
    mark(cv, cx - rx, cy - ry, cx + rx + 1, cy + ry + 1);
}

void picogame_canvas_fill_round_rect(picogame_canvas_obj_t *cv, int x, int y, int w, int h, int r, uint16_t color) {
    if (r > w / 2) {
        r = w / 2;
    }
    if (r > h / 2) {
        r = h / 2;
    }
    if (r < 0) {
        r = 0;
    }
    picogame_canvas_fill_rect(cv, x + r, y, w - 2 * r, h, color);
    picogame_canvas_fill_rect(cv, x, y + r, r, h - 2 * r, color);
    picogame_canvas_fill_rect(cv, x + w - r, y + r, r, h - 2 * r, color);
    picogame_canvas_fill_circle(cv, x + r, y + r, r, color);
    picogame_canvas_fill_circle(cv, x + w - r - 1, y + r, r, color);
    picogame_canvas_fill_circle(cv, x + r, y + h - r - 1, r, color);
    picogame_canvas_fill_circle(cv, x + w - r - 1, y + h - r - 1, r, color);
}

void picogame_canvas_frame3d(picogame_canvas_obj_t *cv, int x, int y, int w, int h, uint16_t light, uint16_t dark) {
    picogame_canvas_fill_rect(cv, x, y, w, 1, light);
    picogame_canvas_fill_rect(cv, x, y, 1, h, light);
    picogame_canvas_fill_rect(cv, x, y + h - 1, w, 1, dark);
    picogame_canvas_fill_rect(cv, x + w - 1, y, 1, h, dark);
}

void picogame_blit_canvas(
    uint16_t *buf, int region_w, int strip_top, int strip_h, int x0,
    picogame_canvas_obj_t *cv, int ox, int oy) {
    // Reuse the bitmap blitter by viewing the canvas as a 1-frame RGB565 bitmap.
    picogame_bitmap_obj_t bm;
    bm.data = (const uint8_t *)cv->data;
    bm.palette = NULL;
    bm.width = cv->w;
    bm.height = cv->h;
    bm.stride = cv->w;
    bm.transparent = cv->transparent;
    bm.format = PICOGAME_FMT_RGB565;
    bm.frames = 1;
    bm.has_transparent = cv->has_transparent;
    picogame_blit_bitmap(buf, region_w, strip_h, x0, strip_top, &bm,
        cv->x + ox, cv->y + oy, 0, false, false, false, NULL);
}

// Composite a string's glyphs straight into the surface in C: rasterize each glyph from the
// font's 1-bit atlas on the fly (no Python glyph cache, no per-call Bitmap/Sprite). Because the
// StripDraw `view` is a Canvas pointing at the live strip buffer, view.text() draws immediate-mode
// text into the frame with zero retained RAM - the same primitive serves retained Canvas screens.
void picogame_canvas_text(picogame_canvas_obj_t *cv, int x, int y, const char *text,
    uint16_t fg, uint16_t bg, bool has_bg, const void *font) {
    const fontio_builtinfont_t *f = font;
    displayio_bitmap_t *sheet = (displayio_bitmap_t *)f->bitmap;
    int fw = f->width, fh = f->height;
    int tpr = sheet->width / fw;            // glyph tiles per atlas row
    int x0 = x;
    // Hoist the canvas target + read the 1-bpp glyph atlas DIRECTLY (no per-pixel get_pixel/put calls).
    // Clip each glyph rect to the canvas ONCE, then the inner loop is atlas-bit -> direct store. This is
    // the per-frame path for StripDraw HUD text (repainted every frame), so it's worth the directness.
    uint16_t *cdata = cv->data;
    int cw = cv->w, ch = cv->h;
    bool onebit = (sheet->bits_per_value == 1);   // terminalio.FONT is 1-bpp; other fonts take the fallback
    const uint8_t *sdata = (const uint8_t *)sheet->data;
    int sstride_b = sheet->stride * 4;            // atlas row stride in BYTES (stride counts uint32)
    for (const uint8_t *p = (const uint8_t *)text; *p; p++) {
        uint8_t gi = fontio_builtinfont_get_glyph_index(f, *p);
        if (gi != 0xff) {                   // 0xff = no glyph -> blank advance
            int tx = (gi % tpr) * fw, ty = (gi / tpr) * fh;
            int gx0 = (x < 0) ? -x : 0;                     // clip the glyph rect to the canvas once
            int gx1 = (x + fw > cw) ? cw - x : fw;
            int gy0 = (y < 0) ? -y : 0;
            int gy1 = (y + fh > ch) ? ch - y : fh;
            for (int gy = gy0; gy < gy1; gy++) {
                uint16_t *drow = cdata + (y + gy) * cw + x;   // dst; index by gx (x+gx is in-bounds)
                int sy = ty + gy;
                if (onebit) {
                    const uint8_t *srow = sdata + (size_t)sy * sstride_b;
                    for (int gx = gx0; gx < gx1; gx++) {
                        int sx = tx + gx;
                        if ((srow[sx >> sheet->x_shift] >> (sheet->x_mask - (sx & sheet->x_mask))) & sheet->bitmask) {
                            drow[gx] = fg;
                        } else if (has_bg) {
                            drow[gx] = bg;
                        }
                    }
                } else {
                    for (int gx = gx0; gx < gx1; gx++) {
                        if (common_hal_displayio_bitmap_get_pixel(sheet, tx + gx, sy)) {
                            drow[gx] = fg;
                        } else if (has_bg) {
                            drow[gx] = bg;
                        }
                    }
                }
            }
        }
        x += fw;
    }
    mark(cv, x0, y, x, y + fh);
}

// Fill a screen-space triangle batch with per-triangle band reject - shared by the
// Canvas.fill_triangles binding and the compositor's Triangles layer (one loop, one place).
void picogame_fill_triangle_batch(picogame_canvas_obj_t *cv, const int16_t *v,
    const uint16_t *col, int n, int xo, int yo) {
    int cw = cv->w, ch = cv->h;
    for (int i = 0; i < n; i++) {
        const int16_t *p = v + i * 6;
        int y0 = p[1] + yo, y1 = p[3] + yo, y2 = p[5] + yo;
        if ((y0 < 0 && y1 < 0 && y2 < 0) || (y0 >= ch && y1 >= ch && y2 >= ch)) {
            continue;
        }
        int x0 = p[0] + xo, x1 = p[2] + xo, x2 = p[4] + xo;
        if ((x0 < 0 && x1 < 0 && x2 < 0) || (x0 >= cw && x1 >= cw && x2 >= cw)) {
            continue;
        }
        picogame_canvas_fill_triangle(cv, x0, y0, x1, y1, x2, y2, col[i]);
    }
}
