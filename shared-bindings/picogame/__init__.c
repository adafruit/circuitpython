// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame: 2D game engine bindings for the PicoPad and similar boards.

#include "py/runtime.h"
#include "shared-module/picogame/pg_compat.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-bindings/picogame/__init__.h"
#include "shared-bindings/picogame/Bitmap.h"
#include "shared-bindings/picogame/Sprite.h"
#if CIRCUITPY_PICOGAME_FAST_DISPLAY
#include "shared-bindings/picogame/Display.h"   // fast DMA backend; absent on portable ports
#include "common-hal/picogame/Display.h"        // its struct (pg_get_display unwraps the wrapper)
#endif
#include "shared-bindings/picogame/Scene.h"
#include "shared-bindings/picogame/Tilemap.h"
#include "shared-bindings/picogame/Particles.h"
#include "shared-bindings/picogame/Canvas.h"
#include "shared-bindings/picogame/Framebuffer.h"
#include "shared-bindings/picogame/StripDraw.h"
#include "shared-bindings/picogame/Triangles.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/Sprite.h"

// ---------------------------------------------------------------------------
// Module-level functions
// ---------------------------------------------------------------------------

// Cast a (possibly subclassed) BusDisplay arg to its native object, raising if it isn't one.
// Also accepts the pg.Display fast-DMA wrapper (unwrapped to its underlying busdisplay - the
// portable send path): any handle that identifies the panel works wherever a display is
// expected, so the same object a Scene renders through also works for render()/invert().
// Without this, code holding the wrapper (custom setup, rgb444) worked on ports WITHOUT the
// fast backend and TypeError'd on ports WITH it.
static busdisplay_busdisplay_obj_t *pg_get_display(mp_obj_t obj) {
    #if CIRCUITPY_PICOGAME_FAST_DISPLAY
    if (mp_obj_is_type(obj, &picogame_display_type)) {
        return ((picogame_display_obj_t *)MP_OBJ_TO_PTR(obj))->display;
    }
    #endif
    mp_obj_t native = mp_obj_cast_to_native_base(obj, &busdisplay_busdisplay_type);
    if (!mp_obj_is_type(native, &busdisplay_busdisplay_type)) {
        mp_arg_validate_type(native, &busdisplay_busdisplay_type, MP_QSTR_display);
    }
    return MP_OBJ_TO_PTR(native);
}

//| """2D game engine for the PicoPad and similar boards.
//|
//| Draws arbitrary-size sprites (unlike ``_stage``'s fixed 16x16 tiles) to a
//| ``busdisplay`` through a reusable strip buffer, with a dirty-rect scene,
//| tilemaps, particles, a drawing canvas and camera/effects.
//|
//| **Relationship to displayio.** picogame does not replace or extend
//| :py:mod:`displayio`; it is a second, game-shaped way to drive the same
//| hardware, and the two do not share a display at the same time. displayio
//| retains a widget tree the supervisor refreshes for you, holds a full
//| :py:class:`~displayio.Bitmap` per image and is the right tool for UI. picogame
//| composites on demand, one horizontal strip at a time, into a buffer the game
//| owns: nothing is retained per pixel, so a scrolling game fits in the RAM a
//| microcontroller actually has, and the game decides when a frame happens
//| (``scene.refresh()``).
//|
//| displayio and picogame share the Display object. A ``picogame.Scene`` takes the same
//| :py:class:`~busdisplay.BusDisplay` displayio uses - it just talks to it
//| directly instead of through the displayio refresh loop, so set
//| ``display.auto_refresh = False`` (``picogame_game.setup()`` does this) and let
//| the game drive. On boards that scan out of RAM, ``picogame.Framebuffer`` takes
//| that buffer instead. Bitmaps are separate types: displayio's is a mutable
//| indexed surface, picogame's is read-only pixel data (PAL8 or wire RGB565) that
//| may live in flash, so it costs no RAM at all."""
//|
//| RGB565: int
//| """16-bit color bitmap format (wire byte order)."""
//| PAL8: int
//| """8-bit paletted bitmap format."""
//|
//|
//| def rgb565(r: int, g: int, b: int) -> int:
//|     """Build a display wire-order RGB565 color from 8-bit components."""
//|     ...
//|
//|
static mp_obj_t picogame_rgb565(mp_obj_t r_in, mp_obj_t g_in, mp_obj_t b_in) {
    int r = mp_obj_get_int(r_in) & 0xff;
    int g = mp_obj_get_int(g_in) & 0xff;
    int b = mp_obj_get_int(b_in) & 0xff;
    uint16_t c = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
    uint16_t wire = (uint16_t)((c >> 8) | (c << 8));
    return MP_OBJ_NEW_SMALL_INT(wire);
}
static MP_DEFINE_CONST_FUN_OBJ_3(picogame_rgb565_obj, picogame_rgb565);

// raycast(map, mw, mh, posx, posy, lrx, lry, srx, sry, sh, stride, ncols, wcolors, top, bot, col, dist)
// C DDA wall raycaster for picogame_ray.Raycaster - INTEGER ONLY (16.16 fixed-point, no FPU; the paint,
// temporal invalidate, pose-cache and billboard math stay in Python; Python does the once-per-frame
// trig and passes Q16 ray params). map: read-only bytes, mw*mh wall types (0 = empty). pos*, l*x/l*y
// (leftRay, column 0), s*x/s*y (rayStep per column) are all 16.16. wcolors: uint16[(maxtype+1)*2] -
// [t*2] near, [t*2+1] side colour. top/bot/col: uint16 write buffers (len>=ncols); dist: int32 write
// buffer (perpendicular distance, 16.16). The int64 divides/muls are ONLY the per-column setup
// (O(ncols)); the DDA step loop is pure 32-bit. Mirrors the Python float fallback closely.
// Optional arg 17 (runs - ONE uint16 write buffer, len>=5*ncols, laid out as five ncols-long
// planes [x0s | x1s | tops | bots | colors]): also emit the RLE-MERGED wall runs (adjacent equal
// columns fused; x in PIXELS = column*stride) and return the run count. The planes feed
// Canvas.vspans directly as memoryview slices. This hoists picogame_ray's per-frame Python merge
// loop into the same C pass (measured 2-6.5 ms/frame of interpreted merge at stride=1 on RP2040).
// Callers clamp the LAST run's x1 to the screen width (stride rounding can overshoot by <stride).
// Without it: returns None.
static mp_obj_t picogame_raycast(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t mi, wi, ti, bi, ci, di;
    mp_get_buffer_raise(args[0], &mi, MP_BUFFER_READ);
    int mw = mp_obj_get_int(args[1]);
    int mh = mp_obj_get_int(args[2]);
    int32_t posx = mp_obj_get_int(args[3]);        // camera x, 16.16
    int32_t posy = mp_obj_get_int(args[4]);
    int32_t rdx = mp_obj_get_int(args[5]);         // leftRay x (column 0), 16.16 - accumulates per column
    int32_t rdy = mp_obj_get_int(args[6]);
    int32_t srx = mp_obj_get_int(args[7]);         // rayStep x per column, 16.16
    int32_t sry = mp_obj_get_int(args[8]);
    int sh = mp_obj_get_int(args[9]);
    int stride = mp_obj_get_int(args[10]);
    int ncols = mp_obj_get_int(args[11]);
    mp_get_buffer_raise(args[12], &wi, MP_BUFFER_READ);
    mp_get_buffer_raise(args[13], &ti, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[14], &bi, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[15], &ci, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[16], &di, MP_BUFFER_WRITE);
    uint16_t *r0 = NULL, *r1 = NULL, *rt = NULL, *rb = NULL, *rcol = NULL;
    if (n_args >= 18) {                            // run outputs requested
        mp_buffer_info_t q;
        mp_get_buffer_raise(args[17], &q, MP_BUFFER_WRITE);
        int cap = (int)(q.len / 10);               // five uint16 planes
        if (ncols > cap) {
            ncols = cap;                           // never write past the run planes
        }
        r0 = q.buf;
        r1 = r0 + cap;
        rt = r1 + cap;
        rb = rt + cap;
        rcol = rb + cap;
    }
    const uint8_t *map = mi.buf;
    const uint16_t *wc = wi.buf;
    int wc_types = (int)(wi.len >> 2);
    uint16_t *top = ti.buf;
    uint16_t *bot = bi.buf;
    uint16_t *col = ci.buf;
    int32_t *dist_out = di.buf;                    // perpendicular distance, 16.16
    int half = sh >> 1;
    int imapx0 = posx >> 16;
    int imapy0 = posy >> 16;
    int32_t fracx = posx & 0xFFFF;                 // fractional part of pos, 16.16
    int32_t fracy = posy & 0xFFFF;
    const int32_t DD_CAP = (int32_t)1 << 24;       // cap deltaDist so a 64-step accumulation stays in int32
    if (ncols > (int)(ti.len >> 1)) {
        ncols = (int)(ti.len >> 1);
    }
    for (int c = 0; c < ncols; c++) {
        int mapx = imapx0;
        int mapy = imapy0;
        int32_t ax = rdx < 0 ? -rdx : rdx;
        int32_t ay = rdy < 0 ? -rdy : rdy;
        // deltaDist = |1/rayDir| in 16.16 = (1<<32)/|rayDir_q16| (int64; per-column setup, not per-step)
        int64_t ddx64 = ax ? (((int64_t)1 << 32) / ax) : (int64_t)DD_CAP;
        int64_t ddy64 = ay ? (((int64_t)1 << 32) / ay) : (int64_t)DD_CAP;
        int32_t ddx = ddx64 > DD_CAP ? DD_CAP : (int32_t)ddx64;
        int32_t ddy = ddy64 > DD_CAP ? DD_CAP : (int32_t)ddy64;
        int stepx, stepy;
        int32_t sidex, sidey;
        // sideDist to the first grid line = (fractional distance) * deltaDist, 16.16 (int64 mul, setup only)
        if (rdx < 0) {
            stepx = -1;
            sidex = (int32_t)(((int64_t)fracx * ddx) >> 16);
        } else {
            stepx = 1;
            sidex = (int32_t)(((int64_t)(65536 - fracx) * ddx) >> 16);
        }
        if (rdy < 0) {
            stepy = -1;
            sidey = (int32_t)(((int64_t)fracy * ddy) >> 16);
        } else {
            stepy = 1;
            sidey = (int32_t)(((int64_t)(65536 - fracy) * ddy) >> 16);
        }
        int side = 0;
        int cell = 1;
        for (int i = 0; i < 64; i++) {             // DDA - pure 32-bit
            if (sidex < sidey) {
                sidex += ddx;
                mapx += stepx;
                side = 0;
            } else {
                sidey += ddy;
                mapy += stepy;
                side = 1;
            }
            cell = (mapx >= 0 && mapx < mw && mapy >= 0 && mapy < mh) ? map[mapy * mw + mapx] : 1;
            if (cell) {
                break;
            }
        }
        int32_t perp = (side == 0) ? (sidex - ddx) : (sidey - ddy);   // perpWallDist, 16.16
        if (perp < 655) {
            perp = 655;                            // ~0.01 in 16.16
        }
        int lh = (int)(((int32_t)sh << 16) / perp);   // sh / perpWallDist (px); 32-bit (sh<<16 <= ~15.7M)
        int t = half - (lh >> 1);
        int b = t + lh;
        if (t < 0) {
            t = 0;
        }
        if (b > sh) {
            b = sh;
        }
        top[c] = (uint16_t)t;
        bot[c] = (uint16_t)b;
        int ct = (cell < wc_types) ? cell : 1;     // unknown type -> type 1 (matches Python default)
        col[c] = wc[ct * 2 + side];
        dist_out[c] = perp;
        rdx += srx;                                // accumulate ray direction for the next column (no overflow)
        rdy += sry;
    }
    if (r0 && ncols > 0) {
        // post-pass RLE over the just-written (cache-hot) column arrays: one flush point
        int nr = 0;
        int rstart = 0;
        for (int c = 1; c <= ncols; c++) {
            if (c == ncols || top[c] != top[rstart] || bot[c] != bot[rstart] || col[c] != col[rstart]) {
                r0[nr] = (uint16_t)(rstart * stride);
                r1[nr] = (uint16_t)(c * stride);
                rt[nr] = top[rstart];
                rb[nr] = bot[rstart];
                rcol[nr] = col[rstart];
                nr++;
                rstart = c;
            }
        }
        return MP_OBJ_NEW_SMALL_INT(nr);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame_raycast_obj, 17, 18, picogame_raycast);

// road_edges(rl, rr, hw, n, cx0, dist, cfg) - one racing-road frame's curve accumulator + integer
// edges in one call (the OutRun-genre compute_road loop; core + cfg layout documented in
// shared-module). rl/rr = int16 out, hw = int32 Q16 half-widths, cx0 = Q16 screen centre
// (incl. lateral), dist = integer world distance, cfg = int32[7].
static mp_obj_t picogame_road_edges_fn(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t rli, rri, hwi, cfgi;
    mp_get_buffer_raise(args[0], &rli, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[1], &rri, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[2], &hwi, MP_BUFFER_READ);
    mp_get_buffer_raise(args[6], &cfgi, MP_BUFFER_READ);
    int n = mp_obj_get_int(args[3]);
    int cap = (int)(rli.len < rri.len ? rli.len : rri.len) / 2;
    if (n > cap) {
        n = cap;
    }
    if (n > (int)(hwi.len / 4)) {
        n = (int)(hwi.len / 4);
    }
    if (n <= 0 || cfgi.len < 7 * 4) {
        return mp_const_none;
    }
    picogame_road_edges((int16_t *)rli.buf, (int16_t *)rri.buf, (const int32_t *)hwi.buf, n,
        mp_obj_get_int(args[4]), mp_obj_get_int(args[5]), (const int32_t *)cfgi.buf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame_road_edges_obj, 7, 7, picogame_road_edges_fn);

// project(cam, pts, n, out_sx, out_sy) - batch perspective projection of `n` 3D points to screen.
//   cam  = 15 camera params: ex,ey,ez, rx,rz, ux,uy,uz, fx,fy,fz, focal, cx0, cy0, near
//   pts  = n*3 world coords (x,y,z per point)
//   out_sx/out_sy = int16 screen coords; a point behind the near plane gets sentinel -32768
// On an FPU board (CIRCUITPY_PICOGAME_FPU) cam/pts are float32; else they are 16.16 fixed int32.
// This is the shared hot path for blocky pseudo-3D (project the 8 corners of each box, then fill).
static mp_obj_t picogame_project(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t ci, pi, xi, yi;
    mp_get_buffer_raise(args[0], &ci, MP_BUFFER_READ);
    mp_get_buffer_raise(args[1], &pi, MP_BUFFER_READ);
    int n = mp_obj_get_int(args[2]);
    mp_get_buffer_raise(args[3], &xi, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[4], &yi, MP_BUFFER_WRITE);
    int16_t *osx = xi.buf;
    int16_t *osy = yi.buf;
    if (n > (int)(xi.len >> 1)) {
        n = (int)(xi.len >> 1);
    }
    #if CIRCUITPY_PICOGAME_FPU
    const float *cam = ci.buf;
    const float *pts = pi.buf;
    float ex = cam[0], ey = cam[1], ez = cam[2];
    float rx = cam[3], rz = cam[4];
    float ux = cam[5], uy = cam[6], uz = cam[7];
    float fx = cam[8], fy = cam[9], fz = cam[10];
    float focal = cam[11], cx0 = cam[12], cy0 = cam[13], near = cam[14];
    for (int i = 0; i < n; i++) {
        float X = pts[i * 3] - ex, Y = pts[i * 3 + 1] - ey, Z = pts[i * 3 + 2] - ez;
        float cz = X * fx + Y * fy + Z * fz;
        if (cz < near) {
            osx[i] = -32768;
            osy[i] = -32768;
            continue;
        }
        float k = focal / cz;                       // hardware divide on an FPU part
        osx[i] = (int16_t)(cx0 + (X * rx + Z * rz) * k);
        osy[i] = (int16_t)(cy0 - (X * ux + Y * uy + Z * uz) * k);
    }
    #else
    const int32_t *cam = ci.buf;                    // all values 16.16
    const int32_t *pts = pi.buf;
    int32_t ex = cam[0], ey = cam[1], ez = cam[2];
    int32_t rx = cam[3], rz = cam[4];
    int32_t ux = cam[5], uy = cam[6], uz = cam[7];
    int32_t fx = cam[8], fy = cam[9], fz = cam[10];
    int32_t focal = cam[11], cx0 = cam[12], cy0 = cam[13], near = cam[14];
    // Full-precision Q16 dot products (int64 mul per term). A Q8-prescaled-basis/MULS variant was
    // ~30% faster, but its error grows with |coord| (~0.2%/axis) and k = focal/cz AMPLIFIES it near
    // the near plane - host-measured 23-34 px warps on close fly-bys at a file-browser world scale
    // (walls visibly broke). Correctness first: Q16 keeps the worst error a few px at any cz >= near,
    // for coords up to +-32k units; still ~4-5x faster than the same math in Python on the M0+.
    #define FMUL(a, b) ((int32_t)(((int64_t)(a) * (b)) >> 16))
    for (int i = 0; i < n; i++) {
        int32_t X = pts[i * 3] - ex, Y = pts[i * 3 + 1] - ey, Z = pts[i * 3 + 2] - ez;
        int32_t cz = FMUL(X, fx) + FMUL(Y, fy) + FMUL(Z, fz);
        if (cz < near) {
            osx[i] = -32768;
            osy[i] = -32768;
            continue;
        }
        // focal/cz in 16.16. A 32-bit divide (focal<<8 = Q24, cz>>8 = Q8 -> Q16) is ~4x cheaper than
        // an int64 divide on the M0+ (no HW divide) and the lost cz precision costs <0.02 px (host-
        // measured). Needs FOCAL < ~250 (focal<<8 in uint32) and near >= 1/256 (cz>>8 nonzero).
        int32_t k = (int32_t)(((uint32_t)focal << 8) / (uint32_t)(cz >> 8));
        int32_t rr = FMUL(X, rx) + FMUL(Z, rz);
        int32_t uu = FMUL(X, ux) + FMUL(Y, uy) + FMUL(Z, uz);
        osx[i] = (int16_t)((cx0 + FMUL(rr, k)) >> 16);
        osy[i] = (int16_t)((cy0 - FMUL(uu, k)) >> 16);
    }
#undef FMUL
    #endif
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame_project_obj, 5, 5, picogame_project);

//| def invert(display: busdisplay.BusDisplay, on: bool) -> None:
//|     """Toggle the panel's hardware colour inversion (INVON/INVOFF). Instant and sends NO
//|     pixel data, so a brief invert is a FREE full-screen flash (a 1-bit negative 'hit' look)
//|     - cheaper than a Fade overlay. ST7789/ST7735 support it."""
//|     ...
//|
//|
static mp_obj_t picogame_invert(mp_obj_t display_in, mp_obj_t on_in) {
    #if CIRCUITPY_PICOGAME_FRAMEBUFFER
    // A Framebuffer target (RP2350 DVI, the WASM playground) has no hardware INVON/INVOFF -
    // emulate the flash by XORing the composite (mirrors the Scene/render Framebuffer handling).
    if (mp_obj_is_type(display_in, &picogame_framebuffer_type)) {
        picogame_fb_set_invert(mp_obj_is_true(on_in));
        return mp_const_none;
    }
    #endif
    picogame_set_invert(pg_get_display(display_in), mp_obj_is_true(on_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(picogame_invert_obj, picogame_invert);

//| def render(
//|     display: busdisplay.BusDisplay,
//|     sprites: List[Sprite],
//|     buffer: WriteableBuffer,
//|     x0: int,
//|     y0: int,
//|     x1: int,
//|     y1: int,
//|     *,
//|     background: int = 0,
//| ) -> None:
//|     """Render ``sprites`` into the screen region [x0,x1) x [y0,y1) and push it
//|     to ``display``. ``buffer`` is a reusable strip buffer (>= region_width*2 bytes)."""
//|     ...
//|
//|

// Map a layer object to its PICOGAME_KIND_*, or raise the one shared TypeError. Both
// Scene.add() and pg.render() classify through here (one type chain, one message).
uint8_t picogame_kind_of(mp_obj_t o) {
    if (mp_obj_is_type(o, &picogame_sprite_type)) {
        return PICOGAME_KIND_SPRITE;
    }
    if (mp_obj_is_type(o, &picogame_stripdraw_type)) {
        return PICOGAME_KIND_STRIPDRAW;
    }
    if (mp_obj_is_type(o, &picogame_tilemap_type)) {
        return PICOGAME_KIND_TILEMAP;
    }
    if (mp_obj_is_type(o, &picogame_particles_type)) {
        return PICOGAME_KIND_PARTICLES;
    }
    if (mp_obj_is_type(o, &picogame_canvas_type)) {
        return PICOGAME_KIND_CANVAS;
    }
    if (mp_obj_is_type(o, &picogame_triangles_type)) {
        return PICOGAME_KIND_TRIANGLES;
    }
    mp_arg_error_invalid(MP_QSTR_item);        // the accepted layer types are listed in the docs
}

static mp_obj_t picogame_render_fun(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_display, ARG_sprites, ARG_buffer, ARG_x0, ARG_y0, ARG_x1, ARG_y1, ARG_background };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sprites, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x0, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y0, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_background, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Accept a picogame.Framebuffer (RAM scanout buffer) as the target too, when built
    // in: immediate render composites straight into it (no strip buffer, no bus), so
    // pg.render(board.DISPLAY, ...) works when board.DISPLAY is a Framebuffer - the HUD /
    // HudBar / immediate-mode path on scanout-buffer platforms. Mirrors the Scene change.
    #if CIRCUITPY_PICOGAME_FRAMEBUFFER
    picogame_framebuffer_obj_t *fbt =
        mp_obj_is_type(args[ARG_display].u_obj, &picogame_framebuffer_type)
        ? MP_OBJ_TO_PTR(args[ARG_display].u_obj) : NULL;
    busdisplay_busdisplay_obj_t *display = fbt ? NULL : pg_get_display(args[ARG_display].u_obj);
    #else
    busdisplay_busdisplay_obj_t *display = pg_get_display(args[ARG_display].u_obj);
    #endif

    size_t n = 0;
    mp_obj_t *items;
    mp_obj_get_array(args[ARG_sprites].u_obj, &n, &items);

    // Classify items into layer kinds. All-Sprite lists stay on the NULL-kinds fast path (no alloc -
    // the common case). Any non-Sprite layer (StripDraw/Canvas/Tilemap/Particles) builds a small kinds
    // array so immediate render uses the SAME multi-layer blitter the Scene does - e.g. a StripDraw
    // composited straight into the strip with `view.text()` = 0-RAM immediate HUD / text screen.
    uint8_t kbuf[16];
    uint8_t *kinds = NULL;
    for (size_t i = 0; i < n; i++) {
        if (!mp_obj_is_type(items[i], &picogame_sprite_type)) {
            kinds = (n <= MP_ARRAY_SIZE(kbuf)) ? kbuf : m_new(uint8_t, n);
            break;
        }
    }
    if (kinds != NULL) {
        for (size_t i = 0; i < n; i++) {
            // (an unknown type raises from kind_of; the GC reclaims a heap `kinds`)
            kinds[i] = picogame_kind_of(items[i]);
        }
    }

    #if CIRCUITPY_PICOGAME_FRAMEBUFFER
    if (fbt != NULL) {
        // Framebuffer target: composite the region straight into it (no strip buffer, no
        // bus). Same compositor as the SPI path; re-raise a latched StripDraw exception.
        mp_obj_t exc = picogame_render_framebuffer(fbt->fb, fbt->width, fbt->height, fbt->fmt,
            fbt->scratch, fbt->scratch_rows,
            items, kinds, n,
            args[ARG_x0].u_int, args[ARG_y0].u_int, args[ARG_x1].u_int, args[ARG_y1].u_int,
            args[ARG_background].u_int, 0, 0);
        if (kinds != NULL && n > MP_ARRAY_SIZE(kbuf)) {
            m_del(uint8_t, kinds, n);
        }
        if (exc != MP_OBJ_NULL) {
            nlr_raise(MP_OBJ_TO_PTR(exc));
        }
        return mp_const_none;
    }
    #endif

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);

    if (kinds == NULL) {
        picogame_render(display, items, n,
            (uint16_t *)bufinfo.buf, bufinfo.len / 2,
            args[ARG_x0].u_int, args[ARG_y0].u_int, args[ARG_x1].u_int, args[ARG_y1].u_int,
            args[ARG_background].u_int);
    } else {
        picogame_render_region(display, items, kinds, n,
            (uint16_t *)bufinfo.buf, bufinfo.len / 2,
            args[ARG_x0].u_int, args[ARG_y0].u_int, args[ARG_x1].u_int, args[ARG_y1].u_int,
            args[ARG_background].u_int, 0, 0);
        if (n > MP_ARRAY_SIZE(kbuf)) {
            m_del(uint8_t, kinds, n);
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_render_obj, 7, picogame_render_fun);

//| def collide(
//|     x1: int, y1: int, x2: int, y2: int, ax1: int, ay1: int, ax2: int = ..., ay2: int = ...
//| ) -> bool:
//|     """AABB overlap test with INCLUSIVE bounds - both corners are part of the box, so two
//|     boxes collide the moment they TOUCH (no visible overlap, no gap). Pass sprite hitboxes
//|     as (x, y, x+w, y+h): collision fires on contact, the usual game feel. With 8 args: box
//|     (x1,y1,x2,y2) vs box (ax1,ay1,ax2,ay2). With 6 args: box vs point (ax1, ay1).
//|     NOTE: this is intentionally inclusive, unlike render's half-open [x0,x1) pixel ranges -
//|     render is about pixels, collide is about game hitboxes (touch = hit)."""
//|     ...
//|
//|
static mp_obj_t picogame_collide(size_t n_args, const mp_obj_t *args) {
    int x1 = mp_obj_get_int(args[0]);
    int y1 = mp_obj_get_int(args[1]);
    int x2 = mp_obj_get_int(args[2]);
    int y2 = mp_obj_get_int(args[3]);
    bool hit;
    if (n_args == 8) {
        int bx1 = mp_obj_get_int(args[4]);
        int by1 = mp_obj_get_int(args[5]);
        int bx2 = mp_obj_get_int(args[6]);
        int by2 = mp_obj_get_int(args[7]);
        hit = (x1 <= bx2) && (x2 >= bx1) && (y1 <= by2) && (y2 >= by1);
    } else if (n_args == 6) {
        int px = mp_obj_get_int(args[4]);
        int py = mp_obj_get_int(args[5]);
        hit = (px >= x1) && (px <= x2) && (py >= y1) && (py <= y2);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("argument num/types mismatch"));
    }
    return mp_obj_new_bool(hit);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame_collide_obj, 6, 8, picogame_collide);

// ---- procedural value-noise in C (the desktop simulator sim/picogame.py mirrors it) ----
// The CANONICAL implementation is FIXED-POINT (Q16.16 coords, Q0.16 values), exposed
// under the plain names value2d/value1d/fbm2d/fbm1d (see further down). It benchmarked
// ~1.8x faster than float on-device (0.649 s vs 1.186 s / 5000 fbm2d), so the float
// version was retired (2026-06-18) to free flash for future engine features.
// The float reference is preserved but DISABLED in the `#if 0` below (cf. PicoLibSDK's
// own Noise2D, which is likewise float) - revive by flipping it to `#if 1` and pointing
// the module table at the *_obj names instead of the *_fx_obj ones.
#if 0   // float reference implementation - superseded by the fixed-point path below
static inline float pg_nhash(int32_t x, int32_t y, int32_t seed) {
    uint32_t h = (uint32_t)x * 374761393u + (uint32_t)y * 668265263u + (uint32_t)seed * 362437u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h = h ^ (h >> 16);
    return (float)(h & 0xFFFFu) / 65535.0f;
}
static inline float pg_nsmooth(float t) {
    return t * t * (3.0f - 2.0f * t);
}
static inline int32_t pg_ifloor(float x) {
    int32_t i = (int32_t)x;
    return (x < (float)i) ? i - 1 : i;
}
static float pg_value2d(float x, float y, int32_t seed) {
    int32_t xi = pg_ifloor(x), yi = pg_ifloor(y);
    float xf = x - (float)xi, yf = y - (float)yi;
    float a = pg_nhash(xi, yi, seed), b = pg_nhash(xi + 1, yi, seed);
    float c = pg_nhash(xi, yi + 1, seed), d = pg_nhash(xi + 1, yi + 1, seed);
    float u = pg_nsmooth(xf), v = pg_nsmooth(yf);
    return (a * (1.0f - u) + b * u) * (1.0f - v) + (c * (1.0f - u) + d * u) * v;
}
static float pg_value1d(float x, int32_t seed) {
    int32_t xi = pg_ifloor(x);
    float xf = x - (float)xi;
    float a = pg_nhash(xi, 0, seed), b = pg_nhash(xi + 1, 0, seed);
    return a + (b - a) * pg_nsmooth(xf);
}

//| def value2d(x: float, y: float, *, seed: int = 0) -> float:
//|     """Smooth 2-D value noise in 0..1 (fast C)."""
//|     ...
//|
//|
static mp_obj_t picogame_value2d(size_t n_args, const mp_obj_t *pos, mp_map_t *kw) {
    static const mp_arg_t spec[] = { {MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
                                     {MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}}, {MP_QSTR_seed, MP_ARG_INT, {.u_int = 0}} };
    mp_arg_val_t a[3];
    mp_arg_parse_all(n_args, pos, kw, 3, spec, a);
    return mp_obj_new_float(pg_value2d(mp_obj_get_float(a[0].u_obj), mp_obj_get_float(a[1].u_obj), a[2].u_int));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_value2d_obj, 2, picogame_value2d);

//| def value1d(x: float, *, seed: int = 0) -> float: ...
static mp_obj_t picogame_value1d(size_t n_args, const mp_obj_t *pos, mp_map_t *kw) {
    static const mp_arg_t spec[] = { {MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
                                     {MP_QSTR_seed, MP_ARG_INT, {.u_int = 0}} };
    mp_arg_val_t a[2];
    mp_arg_parse_all(n_args, pos, kw, 2, spec, a);
    return mp_obj_new_float(pg_value1d(mp_obj_get_float(a[0].u_obj), a[1].u_int));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_value1d_obj, 1, picogame_value1d);
#endif  // float value2d / value1d

// Shared arg spec for both fbm2d (disabled float) and fbm2d_fx (active fixed-point).
static const mp_arg_t pg_fbm2d_args[] = {
    { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_octaves, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 4} },
    { MP_QSTR_seed, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    { MP_QSTR_lacunarity, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_gain, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
};

#if 0   // float reference fbm - superseded by the fixed-point path below
//| def fbm2d(x: float, y: float, *, octaves: int = 4, seed: int = 0, lacunarity: float = 2.0, gain: float = 0.5) -> float: ...
static mp_obj_t picogame_fbm2d(size_t n_args, const mp_obj_t *pos, mp_map_t *kw) {
    mp_arg_val_t a[6];
    mp_arg_parse_all(n_args, pos, kw, 6, pg_fbm2d_args, a);
    float x = mp_obj_get_float(a[0].u_obj), y = mp_obj_get_float(a[1].u_obj);
    int octaves = a[2].u_int;
    int32_t seed = a[3].u_int;
    float lac = (a[4].u_obj == MP_OBJ_NULL) ? 2.0f : mp_obj_get_float(a[4].u_obj);
    float gain = (a[5].u_obj == MP_OBJ_NULL) ? 0.5f : mp_obj_get_float(a[5].u_obj);
    float total = 0.0f, amp = 1.0f, freq = 1.0f, norm = 0.0f;
    for (int i = 0; i < octaves; i++) {
        total += amp * pg_value2d(x * freq, y * freq, seed);
        norm += amp;
        amp *= gain;
        freq *= lac;
    }
    return mp_obj_new_float(norm > 0.0f ? total / norm : 0.0f);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_fbm2d_obj, 2, picogame_fbm2d);

//| def fbm1d(x: float, *, octaves: int = 4, seed: int = 0, lacunarity: float = 2.0, gain: float = 0.5) -> float: ...
//|
//|
static mp_obj_t picogame_fbm1d(size_t n_args, const mp_obj_t *pos, mp_map_t *kw) {
    static const mp_arg_t spec[] = { {MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
                                     {MP_QSTR_octaves, MP_ARG_INT, {.u_int = 4}}, {MP_QSTR_seed, MP_ARG_INT, {.u_int = 0}},
                                     {MP_QSTR_lacunarity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}}, {MP_QSTR_gain, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}} };
    mp_arg_val_t a[5];
    mp_arg_parse_all(n_args, pos, kw, 5, spec, a);
    float x = mp_obj_get_float(a[0].u_obj);
    int octaves = a[1].u_int;
    int32_t seed = a[2].u_int;
    float lac = (a[3].u_obj == MP_OBJ_NULL) ? 2.0f : mp_obj_get_float(a[3].u_obj);
    float gain = (a[4].u_obj == MP_OBJ_NULL) ? 0.5f : mp_obj_get_float(a[4].u_obj);
    float total = 0.0f, amp = 1.0f, freq = 1.0f, norm = 0.0f;
    for (int i = 0; i < octaves; i++) {
        total += amp * pg_value1d(x * freq, seed);
        norm += amp;
        amp *= gain;
        freq *= lac;
    }
    return mp_obj_new_float(norm > 0.0f ? total / norm : 0.0f);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_fbm1d_obj, 1, picogame_fbm1d);
#endif  // float fbm2d / fbm1d

// ---- fixed-point (Q16.16 coords, Q0.16 values) noise: the CANONICAL value-noise impl,
// exposed under the plain names value2d/value1d/fbm2d/fbm1d. The inner math is integer
// (float only at the Python boundary); ~1.8x faster than the retired float path. ----
static inline uint32_t pg_nhash_raw(int32_t x, int32_t y, int32_t seed) {
    uint32_t h = (uint32_t)x * 374761393u + (uint32_t)y * 668265263u + (uint32_t)seed * 362437u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return (h ^ (h >> 16)) & 0xFFFFu;                      // Q0.16 in [0,1)
}
static inline uint32_t pg_smooth16(uint32_t t) {           // t,result Q0.16: t*t*(3-2t)
    uint32_t t2 = (t * t) >> 16;
    uint32_t e = (3u << 16) - 2u * t;
    return (uint32_t)(((uint64_t)t2 * e) >> 16);
}
static inline uint32_t pg_lerp16(uint32_t a, uint32_t b, uint32_t u) {
    return (uint32_t)((int32_t)a + (int32_t)(((int64_t)((int32_t)b - (int32_t)a) * (int32_t)u) >> 16));
}
static uint32_t pg_value2d_fx(int32_t X, int32_t Y, int32_t seed) {     // X,Y Q16.16 -> Q0.16
    int32_t xi = X >> 16, yi = Y >> 16;
    uint32_t xf = (uint32_t)(X - (xi << 16)), yf = (uint32_t)(Y - (yi << 16));
    uint32_t a = pg_nhash_raw(xi, yi, seed), b = pg_nhash_raw(xi + 1, yi, seed);
    uint32_t c = pg_nhash_raw(xi, yi + 1, seed), d = pg_nhash_raw(xi + 1, yi + 1, seed);
    uint32_t u = pg_smooth16(xf), v = pg_smooth16(yf);
    return pg_lerp16(pg_lerp16(a, b, u), pg_lerp16(c, d, u), v);
}
// (1-D value noise == the 2-D sampler at Y=0, bit for bit: v = smooth16(0) = 0 makes the
// outer lerp return its first argument, which is exactly lerp(hash(xi,0), hash(xi+1,0), u).
// So the 1-D entry points below just call pg_value2d_fx(X, 0, seed) - no separate kernel.)
#define PG_Q16(f) ((int32_t)((f) * 65536.0f))

static mp_obj_t picogame_value2d_fx(size_t n_args, const mp_obj_t *pos, mp_map_t *kw) {
    static const mp_arg_t spec[] = { {MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
                                     {MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}}, {MP_QSTR_seed, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0}} };
    mp_arg_val_t a[3];
    mp_arg_parse_all(n_args, pos, kw, 3, spec, a);
    int32_t v = pg_value2d_fx(PG_Q16(mp_obj_get_float(a[0].u_obj)), PG_Q16(mp_obj_get_float(a[1].u_obj)), a[2].u_int);
    return mp_obj_new_float((float)v * (1.0f / 65536.0f));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_value2d_fx_obj, 2, picogame_value2d_fx);

static mp_obj_t picogame_value1d_fx(size_t n_args, const mp_obj_t *pos, mp_map_t *kw) {
    static const mp_arg_t spec[] = { {MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
                                     {MP_QSTR_seed, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0}} };
    mp_arg_val_t a[2];
    mp_arg_parse_all(n_args, pos, kw, 2, spec, a);
    int32_t v = pg_value2d_fx(PG_Q16(mp_obj_get_float(a[0].u_obj)), 0, a[1].u_int);
    return mp_obj_new_float((float)v * (1.0f / 65536.0f));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_value1d_fx_obj, 1, picogame_value1d_fx);

// Shared fBm octave accumulator (the 1-D entry passes Y=0; sy is then 0 every octave,
// which the value sampler maps to the exact 1-D lattice - see the note above).
static mp_obj_t pg_fbm_eval(int32_t X, int32_t Y, int octaves, int32_t seed,
    const mp_arg_val_t *lac, const mp_arg_val_t *gain) {
    int32_t lacq = (lac->u_obj == MP_OBJ_NULL) ? (2 << 16) : PG_Q16(mp_obj_get_float(lac->u_obj));
    int32_t gainq = (gain->u_obj == MP_OBJ_NULL) ? (1 << 15) : PG_Q16(mp_obj_get_float(gain->u_obj));
    int32_t amp = 1 << 16, freq = 1 << 16;
    int64_t total = 0, norm = 0;
    for (int i = 0; i < octaves; i++) {
        int32_t sx = (int32_t)(((int64_t)X * freq) >> 16), sy = (int32_t)(((int64_t)Y * freq) >> 16);
        total += ((int64_t)amp * pg_value2d_fx(sx, sy, seed)) >> 16;
        norm += amp;
        amp = (int32_t)(((int64_t)amp * gainq) >> 16);
        freq = (int32_t)(((int64_t)freq * lacq) >> 16);
    }
    return mp_obj_new_float(norm ? (float)total / (float)norm : 0.0f);
}

static mp_obj_t picogame_fbm2d_fx(size_t n_args, const mp_obj_t *pos, mp_map_t *kw) {
    mp_arg_val_t a[6];
    mp_arg_parse_all(n_args, pos, kw, 6, pg_fbm2d_args, a);
    return pg_fbm_eval(PG_Q16(mp_obj_get_float(a[0].u_obj)), PG_Q16(mp_obj_get_float(a[1].u_obj)),
        a[2].u_int, a[3].u_int, &a[4], &a[5]);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_fbm2d_fx_obj, 2, picogame_fbm2d_fx);

static mp_obj_t picogame_fbm1d_fx(size_t n_args, const mp_obj_t *pos, mp_map_t *kw) {
    static const mp_arg_t spec[] = { {MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
                                     {MP_QSTR_octaves, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 4}}, {MP_QSTR_seed, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0}},
                                     {MP_QSTR_lacunarity, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL}}, {MP_QSTR_gain, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL}} };
    mp_arg_val_t a[5];
    mp_arg_parse_all(n_args, pos, kw, 5, spec, a);
    return pg_fbm_eval(PG_Q16(mp_obj_get_float(a[0].u_obj)), 0,
        a[1].u_int, a[2].u_int, &a[3], &a[4]);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_fbm1d_fx_obj, 1, picogame_fbm1d_fx);


static const mp_rom_map_elem_t picogame_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picogame) },
    { MP_ROM_QSTR(MP_QSTR_Bitmap), MP_ROM_PTR(&picogame_bitmap_type) },
    { MP_ROM_QSTR(MP_QSTR_Sprite), MP_ROM_PTR(&picogame_sprite_type) },
    #if CIRCUITPY_PICOGAME_FAST_DISPLAY
    { MP_ROM_QSTR(MP_QSTR_Display), MP_ROM_PTR(&picogame_display_type) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_Scene), MP_ROM_PTR(&picogame_scene_type) },
    { MP_ROM_QSTR(MP_QSTR_Tilemap), MP_ROM_PTR(&picogame_tilemap_type) },
    { MP_ROM_QSTR(MP_QSTR_Particles), MP_ROM_PTR(&picogame_particles_type) },
    { MP_ROM_QSTR(MP_QSTR_Canvas), MP_ROM_PTR(&picogame_canvas_type) },
    { MP_ROM_QSTR(MP_QSTR_StripDraw), MP_ROM_PTR(&picogame_stripdraw_type) },
    { MP_ROM_QSTR(MP_QSTR_Triangles), MP_ROM_PTR(&picogame_triangles_type) },
    #if CIRCUITPY_PICOGAME_FRAMEBUFFER
    { MP_ROM_QSTR(MP_QSTR_Framebuffer), MP_ROM_PTR(&picogame_framebuffer_type) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_render), MP_ROM_PTR(&picogame_render_obj) },
    { MP_ROM_QSTR(MP_QSTR_raycast), MP_ROM_PTR(&picogame_raycast_obj) },
    { MP_ROM_QSTR(MP_QSTR_road_edges), MP_ROM_PTR(&picogame_road_edges_obj) },
    { MP_ROM_QSTR(MP_QSTR_project), MP_ROM_PTR(&picogame_project_obj) },
    // True when the pseudo-3D/math primitives use the hardware-float path (FPU board). Python packs
    // camera/point buffers as float32 when this is set, else as 16.16 fixed int32.
    { MP_ROM_QSTR(MP_QSTR_FPU), MP_ROM_INT(CIRCUITPY_PICOGAME_FPU) },
    { MP_ROM_QSTR(MP_QSTR_invert), MP_ROM_PTR(&picogame_invert_obj) },
    { MP_ROM_QSTR(MP_QSTR_collide), MP_ROM_PTR(&picogame_collide_obj) },
    // Canonical noise = the fixed-point implementation (float retired; see `#if 0` above).
    { MP_ROM_QSTR(MP_QSTR_value2d), MP_ROM_PTR(&picogame_value2d_fx_obj) },
    { MP_ROM_QSTR(MP_QSTR_value1d), MP_ROM_PTR(&picogame_value1d_fx_obj) },
    { MP_ROM_QSTR(MP_QSTR_fbm2d), MP_ROM_PTR(&picogame_fbm2d_fx_obj) },
    { MP_ROM_QSTR(MP_QSTR_fbm1d), MP_ROM_PTR(&picogame_fbm1d_fx_obj) },
    { MP_ROM_QSTR(MP_QSTR_rgb565), MP_ROM_PTR(&picogame_rgb565_obj) },
    { MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_INT(PICOGAME_FMT_RGB565) },
    { MP_ROM_QSTR(MP_QSTR_PAL8), MP_ROM_INT(PICOGAME_FMT_PAL8) },
    // Engine API level: bump by 1 whenever the PYTHON-VISIBLE surface grows (new method/property/
    // module function/constant), so picogame-libs can diagnose a too-old firmware up front
    // ("needs API_LEVEL >= N") instead of failing later with a random missing attribute.
    // Level 1 = the 2026-07 surface (post API-freeze + Canvas.text/Framebuffer/StripDraw
    // always_dirty). Older firmwares have no attribute at all -> getattr(pg, "API_LEVEL", 0).
    { MP_ROM_QSTR(MP_QSTR_API_LEVEL), MP_ROM_INT(1) },
    // Build-time capability flag: does THIS board's panel controller support 12-bit RGB444
    // (COLMOD)? The board declares it (it knows its controller); a game reads it to enable
    // Display(rgb444=...) only where it works - one codebase runs on ST7789 AND ILI9341.
    #if CIRCUITPY_PICOGAME_RGB444
    { MP_ROM_QSTR(MP_QSTR_RGB444_SUPPORTED), MP_ROM_TRUE },
    #else
    { MP_ROM_QSTR(MP_QSTR_RGB444_SUPPORTED), MP_ROM_FALSE },
    #endif
    // Build-time default render-strip height (rows). picogame_game.setup() uses it when strip_h is
    // None; games can override per call; a board can override the default in mpconfigboard.h.
    // MEASURED (RP2040): with async DMA double-buffering, SMALL strips overlap render+transfer best ->
    // 8 is both fastest and least RAM (the two w*strip_h*2 buffers shrink). WITHOUT the DMA backend
    // there's no overlap, so a blocking send per strip makes LARGER strips win -> 24.
    #ifndef PICOGAME_STRIP_H
    #if CIRCUITPY_PICOGAME_FAST_DISPLAY
    #define PICOGAME_STRIP_H 8
    #else
    #define PICOGAME_STRIP_H 24
    #endif
    #endif
    { MP_ROM_QSTR(MP_QSTR_STRIP_H), MP_ROM_INT(PICOGAME_STRIP_H) },
};
static MP_DEFINE_CONST_DICT(picogame_module_globals, picogame_module_globals_table);

const mp_obj_module_t picogame_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&picogame_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_picogame, picogame_module);
