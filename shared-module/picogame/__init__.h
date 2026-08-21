// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "py/obj.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/Sprite.h"

// Hardware-FPU boards run the pseudo-3D/math primitives (picogame.project) on plain float32 -
// faster than the soft-float-equivalent fixed path and free of 16.16 range limits; no-FPU
// targets (Cortex-M0+) use integer 16.16. Only ONE path is compiled per board. The default
// follows the architecture; a board can override with CIRCUITPY_PICOGAME_FPU=0/1 in its
// mpconfigboard.mk. Python reads `picogame.FPU` to pack camera/point buffers to match.
#ifndef CIRCUITPY_PICOGAME_FPU
#if (defined(__ARM_FP) && (__ARM_FP != 0)) || (defined(__riscv_flen) && (__riscv_flen > 0))
#define CIRCUITPY_PICOGAME_FPU (1)
#else
#define CIRCUITPY_PICOGAME_FPU (0)
#endif
#endif

// Sample one texel as wire RGB565; false = transparent (skip). Shared by the sprite/canvas
// blit paths so they inline one copy (see the blit contract: PAL8 indices must be < palette len).
static inline bool src_pixel_s(int format, const uint8_t *data, const uint16_t *pal,
    bool transp, uint16_t key, int idx, uint16_t *out) {
    if (format == PICOGAME_FMT_PAL8) {
        uint8_t i = data[idx];
        if (transp && i == (uint8_t)key) {
            return false;
        }
        *out = pal[i];                           // indices must be < palette length (see blit contract)
        return true;
    }
    // GC buffer is >=4-byte aligned; silence xtensa -Wcast-align (see picogame_blit_bitmap).
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    uint16_t v = ((const uint16_t *)data)[idx];
    #pragma GCC diagnostic pop
    if (transp && v == key) {
        return false;
    }
    *out = v;
    return true;
}


// Scene layer kinds (tags stored alongside items so blit/dirty can dispatch
// without cross-referencing shared-bindings type objects).
enum {
    PICOGAME_KIND_SPRITE = 0,
    PICOGAME_KIND_TILEMAP = 1,
    PICOGAME_KIND_PARTICLES = 2,
    PICOGAME_KIND_CANVAS = 3,
    PICOGAME_KIND_STRIPDRAW = 4,
    PICOGAME_KIND_TRIANGLES = 5,
    // High bit on a kind = "fixed": the item ignores the scene view offset
    // (camera), so HUD / score / dialog stay put while the world scrolls.
    PICOGAME_KIND_FIXED = 0x80,
    PICOGAME_KIND_MASK = 0x7f,
};

// StripDraw: immediate-mode layer. Holds NO pixel buffer - instead its `callback`
// is invoked once per render strip with a Canvas "view" repointed at the live strip
// buffer, so the user draws primitives straight into the strip (zero RAM vs a Canvas,
// which costs w*h*2 bytes). Its rect is repainted every frame (it's for animated /
// scanline content: pseudo-3D, gradients, procedural backgrounds). The view's local
// (0,0) maps to screen (vx, vy) handed to the callback. `faulted` latches after the
// callback raises once, so a buggy callback prints one traceback, not one per strip.
typedef struct {
    mp_obj_base_t base;
    mp_obj_t callback;       // draw(view, vx, vy, vw, vh): vx/vy = screen origin of view (0,0)
    mp_obj_t view;           // a reused picogame_canvas_obj_t (data repointed each strip)
    int32_t x, y, w, h;      // scene rect (int32: scene coords, big-world safe)
    int32_t dx1, dy1, dx2, dy2;  // accumulated dirty rect (scene coords) when !always_dirty - the same
    // picogame_dirty_* accumulator Canvas/Tilemap use, so invalidate() can mark
    // a sub-rect and the Scene repaints only that region (not the whole layer).
    bool faulted;
    bool always_dirty;       // True: repaint every frame (animated). False: only the dirty rect (on-change UI).
} picogame_stripdraw_obj_t;

// Triangles: a retained SCREEN-SPACE triangle batch the compositor rasterises entirely
// in C (per strip, band-rejected) - no Python callback per strip, so unlike StripDraw it
// stays composable without re-entering Python mid-frame. verts (int16 x0,y0,x1,y1,x2,y2 per tri) and
// colors (uint16 wire RGB565 per tri) are CALLER-OWNED arrays (refs held for GC; fill
// them in place). Setting `count` selects how many draw and marks the layer dirty
// full-screen (a 3D frame repaints everything anyway).
typedef struct {
    mp_obj_base_t base;
    mp_obj_t verts_obj, colors_obj;   // GC anchors for the caller's arrays
    const int16_t *verts;
    const uint16_t *colors;
    uint16_t count, cap;              // cap = what the buffers can hold
    int32_t dx1, dy1, dx2, dy2;       // dirty accumulator (count-set -> full screen)
} picogame_triangles_obj_t;

static inline int picogame_imin(int a, int b) {
    return a < b ? a : b;
}
static inline int picogame_imax(int a, int b) {
    return a > b ? a : b;
}

// Drawn top-left in scene pixels: the logical position minus the anchor offset
// (anchor is a 1/256 fraction of the bitmap size). Used by BOTH the blitter and
// the dirty-rect tracker so they always agree on where the sprite lands.
static inline void picogame_sprite_topleft(const picogame_sprite_obj_t *s, int *tx, int *ty) {
    int w = (s->bitmap != NULL) ? s->bitmap->width : 0;
    int h = (s->bitmap != NULL) ? s->bitmap->height : 0;
    int sw = (w * s->scale) >> 8;     // anchor is a fraction of the SCALED size
    int sh = (h * s->scale) >> 8;
    // The blitter only honours transpose on the fast path (scale==256); the scaled blitter ignores it.
    // Swap the footprint ONLY when scale==256, or aabb/topleft disagree with what's drawn (trailing).
    if ((s->flags & PICOGAME_SPR_TRANSPOSE) && s->scale == 256) {   // 90deg transpose swaps footprint
        int t = sw;                            // picogame_sprite_aabb, or the blit top-left and the
        sw = sh;                               // tracked dirty rect disagree (sprite trails)
        sh = t;
    }
    *tx = (s->x >> 8) - ((int)s->anchor_x * sw >> 8);
    *ty = (s->y >> 8) - ((int)s->anchor_y * sh >> 8);
}

// Drawn screen-space bounding box of a sprite (accounts for scale + rotation).
// Used by the dirty-rect tracker so it always covers the transformed sprite.
void picogame_sprite_aabb(const picogame_sprite_obj_t *s, int *x1, int *y1, int *x2, int *y2);

// Per-pixel blit effect, shared by all three blit paths. One mode at a time; a NULL
// pointer means "no effect" (the fast path). SHADOW darkens the destination, FLASH
// replaces opaque pixels with `color`, DITHER skips pixels via a Bayer pattern (0..16
// transparency) for fake translucency without alpha.
enum { PICOGAME_FX_NONE = 0, PICOGAME_FX_SHADOW, PICOGAME_FX_FLASH, PICOGAME_FX_DITHER, PICOGAME_FX_TINT };
typedef struct {
    uint8_t mode;
    uint16_t color;       // FLASH: solid colour to write; TINT: colour to multiply by (wire RGB565)
    uint8_t level;        // DITHER: 0..16 transparency (higher = more pixels skipped)
} picogame_fx_t;

// Shared dirty-rect accumulator over a contiguous int32 [x1,y1,x2,y2] (Canvas + Tilemap both end in
// dx1,dy1,dx2,dy2). INT32 sentinels so big-world (>32767 px) scene coords still accumulate.
void picogame_dirty_reset(int32_t *r);
void picogame_dirty_union(int32_t *r, int x1, int y1, int x2, int y2);
bool picogame_dirty_take(int32_t *r, int *x1, int *y1, int *x2, int *y2);

// Blit one frame of a bitmap at screen (dx0, dy0) into the strip buffer that
// covers [ox, ox+bw) x [oy, oy+bh). Shared by sprites and tilemap tiles.
// fxm: per-pixel effect (NULL = plain colour copy).
void picogame_blit_bitmap(
    uint16_t *buf, int bw, int bh, int ox, int oy,
    picogame_bitmap_obj_t *bm, int dx0, int dy0, int frame, bool flip_x, bool flip_y,
    bool transpose, const picogame_fx_t *fxm);

// Nearest-neighbour scaled blit (axis-aligned); scale is 8.8 fixed-point.
void picogame_blit_bitmap_scaled(
    uint16_t *buf, int bw, int bh, int ox, int oy,
    picogame_bitmap_obj_t *bm, int dx0, int dy0, int frame, bool flip_x, bool flip_y,
    uint16_t scale, const picogame_fx_t *fxm);

// Full affine blit (scale + rotation about the anchor); (px,py)=screen anchor point,
// (pivx,pivy)=that anchor in SOURCE pixels. Nearest-neighbour inverse map. The transform is
// PRECOMPUTED by the caller (the sprite's xf_* cache): minx..maxy = the screen-space corner
// bbox, ic/is = the 16.16 inverse-map steps - so a per-strip call does no trig/divides.
void picogame_blit_bitmap_affine(
    uint16_t *buf, int bw, int bh, int ox, int oy,
    picogame_bitmap_obj_t *bm, int px, int py, int pivx, int pivy,
    int frame, bool flip_x, bool flip_y,
    int minx, int miny, int maxx, int maxy, int32_t ic, int32_t is,
    const picogame_fx_t *fxm);

// ===== OUTPUT TRANSPORT SEAM =====================================================
// picogame's compositor (picogame_blit_strip_layers) is OUTPUT-AGNOSTIC: it composites
// scene layers into a plain wire-order RGB565 strip buffer, knowing nothing about the
// destination. A physical display is reached only through the small transport contract
// below, so a non-CircuitPython port (e.g. a MicroPython framebuf/SPI backend) can reuse
// the whole compositor AND the generic picogame_render_region orchestrator and reimplement
// ONLY these few functions. `picogame_output_t` is the opaque display handle they take.
// (A RAM-framebuffer destination is a separate backend: picogame_render_framebuffer.)
//
// Strip-path contract a backend provides:
//   picogame_strip_begin      - open a window for [x0,y0,x1,y1); return strip geometry
//   picogame_out_strip_send   - push one composited strip (region_w*sh px, wire RGB565)
//   picogame_out_strip_end    - close the transaction
//   picogame_set_invert       - panel hardware colour inversion (a free full-screen flash)
//   picogame_set_pixel_format - panel COLMOD (RGB565/RGB444), when CIRCUITPY_PICOGAME_RGB444
//
// CircuitPython backend: picogame_output_t == busdisplay; the impl lives in __init__.c.
typedef busdisplay_busdisplay_obj_t picogame_output_t;

// Fill a strip with background, then composite items (sprites and tilemaps) in
// order (items[0] = bottom). kinds[i] selects the type; kinds == NULL means
// every item is a sprite. (ox, oy) is the view offset added to item positions
// (scene space -> screen space) for camera/centering.
// Returns a latched BaseException (Ctrl-C / ReloadException) raised by a StripDraw callback, or
// MP_OBJ_NULL. The caller must re-raise it AFTER closing the display transaction.
#if defined(PICOGAME_HAS_INTERP)
// rp2-port SIO-interpolator mode7 row walker. Fast path only:
// PAL8, opaque, stride == tw, log2(tw)+log2(th) <= 16; the caller guards and falls back.
void picogame_mode7_row_interp(uint16_t *dst, int n,
    const uint8_t *tex, const uint16_t *pal,
    uint32_t fx, uint32_t fy, int32_t stepx, int32_t stepy,
    int shx, int shy, int ltw, int lth);
#endif

// Racing-road curve pass (see the implementation comment in __init__.c).
void picogame_road_edges(int16_t *rl, int16_t *rr, const int32_t *hw_q16, int n,
    int32_t cx_q16, int32_t dist, const int32_t *cfg);

mp_obj_t picogame_blit_strip_layers(
    uint16_t *buf, int region_w, int strip_top, int strip_h, int x0,
    mp_obj_t *items, uint8_t *kinds, size_t n, uint16_t background, int ox, int oy);

// Compute strip geometry and open a render window on the display (set region,
// begin transaction, send RAMWR). Returns false if the region is empty; raises
// if the buffer is too small for the region width. Fills *region_w and *strip_h.
bool picogame_strip_begin(
    picogame_output_t *display,
    int *x0, int *y0, int *x1, int *y1, size_t buffer_pixels,
    int *region_w, int *strip_h);   // clamps *x0..*y1 to the panel in place (caller loops on them)

// Portable backend: strip-render a layered scene region to ANY busdisplay via
// its (blocking) bus.send - single buffer, no DMA. Same layer dispatch as the
// fast path (kinds + view offset), so it is the cross-port fallback for Scene on
// targets without the platform DMA Display. `kinds == NULL` => all sprites.
void picogame_render_region(
    picogame_output_t *display,
    mp_obj_t *items, uint8_t *kinds, size_t n,
    uint16_t *buffer, size_t buffer_pixels,
    int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    uint16_t background, int ox, int oy);

// Toggle the panel's hardware colour inversion (INVON/INVOFF) - a free full-screen flash.
void picogame_set_invert(picogame_output_t *display, bool on);

#if CIRCUITPY_PICOGAME_FRAMEBUFFER
// Emulated invert for a picogame.Framebuffer target (no hardware INVON): set the flag (XORed into the
// wire->native conversion) and, via take_invert_dirty(), force one whole-frame recomposite on toggle.
void picogame_fb_set_invert(bool on);
bool picogame_fb_take_invert_dirty(void);
#endif

#if CIRCUITPY_PICOGAME_RGB444   // compiled in only on boards that opt into RGB444 (default off)
// Set panel pixel format (COLMOD): rgb444 -> 12-bit RGB444, else 16-bit RGB565.
void picogame_set_pixel_format(picogame_output_t *display, bool rgb444);

// Pack `npix` (even) wire-order RGB565 pixels in `buf` IN-PLACE to 12-bit RGB444; returns bytes.
size_t picogame_pack_rgb444(uint16_t *buf, size_t npix);
#endif

// Universal sprite-only convenience wrapper over picogame_render_region.
void picogame_render(
    picogame_output_t *display,
    mp_obj_t *items, size_t n,
    uint16_t *buffer, size_t buffer_pixels,
    int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    uint16_t background);

#if CIRCUITPY_PICOGAME_FRAMEBUFFER
// Rows per compose band for the tear-free NATIVE framebuffer path: composite+byte-swap into a
// private scratch strip, then memcpy the finished NATIVE band into the live scanout buffer - so a
// picodvi/HDMI beam scanning the framebuffer never samples a half-composed WIRE-order region (which
// would read as byte-swapped / pink). Small band = small scratch (width*this*2 bytes).
#define PICOGAME_FB_SCRATCH_H 16
// A render TARGET that is a caller-owned RAM framebuffer (wire-order RGB565), used in
// place of a BusDisplay for scanout-buffer platforms: the WASM playground (heap
// buffer read out to a canvas), the desktop sim, and FruitJam (the DVI/HSTX scanout
// buffer). Holds a WriteableBuffer alive + a typed view of it; allocation is the
// caller's (a bytearray in WASM, the DVI buffer memoryview on FruitJam), so the engine
// stays platform-neutral. Scene / render can target this instead of a display.
// Output pixel format of a picogame.Framebuffer target. The compositor always works in
// wire-order RGB565; the format only selects the publish conversion.
enum {
    PICOGAME_FB_WIRE565 = 0,    // no conversion (WASM playground / sim readout)
    PICOGAME_FB_NATIVE565 = 1,  // byte-swap to native RGB565 (picodvi 16-bit scanout)
    PICOGAME_FB_RGB332 = 2,     // quantize to RGB332 bytes (picodvi 8-bit scanout, e.g.
                                // Fruit Jam 640x480 - its max resolution is 8bpp-only)
};

typedef struct {
    mp_obj_base_t base;
    mp_obj_t buffer;     // the backing WriteableBuffer (kept alive)
    uint16_t *fb;        // typed view of buffer.buf: width*height px - RGB565 (2 B/px) for the
                         // 565 formats; cast to uint8_t* per-pixel bytes for PICOGAME_FB_RGB332
    int width;
    int height;
    uint8_t fmt;         // PICOGAME_FB_* output format (see enum above)
    mp_obj_t scratch_buf;  // GC-kept bytearray backing `scratch`; mp_const_none if none
    uint16_t *scratch;     // private compose strip: width*scratch_rows px, or NULL
    int scratch_rows;      // rows in `scratch` (PICOGAME_FB_SCRATCH_H), 0 if none
} picogame_framebuffer_obj_t;

// Full-frame RAM-framebuffer backend: same layered compositor as the SPI strip path
// but composited straight into a caller-owned wire-order RGB565 framebuffer (no bus).
// The shared render target for scanout-buffer platforms (RP2350 DVI/HSTX, sim, WASM).
// [x0,y0,x1,y1) is the region to (re)composite (clamped to the framebuffer). Returns a
// latched StripDraw BaseException for the caller to re-raise, or MP_OBJ_NULL.
mp_obj_t picogame_render_framebuffer(
    uint16_t *fb, int fb_stride, int fb_h, int fmt,
    uint16_t *scratch, int scratch_rows,
    mp_obj_t *items, uint8_t *kinds, size_t n,
    int x0, int y0, int x1, int y1,
    uint16_t background, int ox, int oy);
#endif // CIRCUITPY_PICOGAME_FRAMEBUFFER
