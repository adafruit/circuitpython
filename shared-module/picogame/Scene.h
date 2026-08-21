// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame retained-mode scene with dirty-rectangle tracking. The scene owns a
// list of sprites and a snapshot of their state from the previous frame; each
// refresh diffs against the snapshot to compute the changed regions and repaints
// only those: up to PICOGAME_MAX_DIRTY_RECTS (6) mostly-disjoint rectangles, each
// rendered into its own clamped SPI window, so several separated moving objects
// stay cheap; beyond that they merge toward a fuller redraw.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "py/obj.h"
#include "shared-module/picogame/Sprite.h"

typedef struct {
    int32_t x, y;
    int32_t w, h;     // drawn SCENE-space AABB (top-left + size) when last drawn - already accounts
                      // for scale + rotation, so it is the old-rect on any change. int32 (not int16):
                      // sprite coords are 24.8 in int32, so a big scrolling world exceeds +-32767 px
    void *bitmap;     // bitmap identity when last drawn (detect graphic swaps); never
                      // dereferenced - compared for inequality only, so it is safe
                      // even if the previous bitmap has since been freed
    uint16_t scale;   // draw scale (8.8) when last drawn - detect scale changes
    int16_t angle;    // rotation when last drawn - detect rotation changes
    uint8_t frame;
    uint8_t flags;
    uint8_t seq;      // sprite.seq when last drawn - detect touch() (in-place content change)
    uint8_t dither;   // dither level when last drawn - detect translucency animation
    uint16_t flash_color;  // flash/tint colour when last drawn - detect a colour-only change
                           // (the effect flag stays set, so without this red->blue wouldn't repaint)
} picogame_snapshot_t;

typedef struct {
    int x1, y1, x2, y2;     // screen-space dirty rectangle [x1,x2) x [y1,y2)
} picogame_rect_t;

typedef struct {
    mp_obj_base_t base;
    mp_obj_t display;     // picogame.Display (transport; also kept alive)
    mp_obj_t buf_a;       // strip buffer A (kept alive)
    mp_obj_t buf_b;       // strip buffer B (kept alive)
    mp_obj_t *items;      // sprite / tilemap objects (GC-scanned -> stay alive)
    uint8_t *kinds;       // PICOGAME_KIND_* per item
    picogame_snapshot_t *snap;       // previous-frame state; snap[i] is unused when
                                     // kinds[i] == TILEMAP (tilemaps track their own dirty rect)
    uint16_t count;
    uint16_t cap;
    uint16_t background;
    int32_t ox, oy;  // view offset: screen position of scene origin (camera/centering); int32 so a
                     // large-world camera can scroll past +-32767 px without truncating the offset
    int16_t top, bottom, left, right;  // reserved insets (px): the scene renders only the
    // play rect [left, w-right) x [top, h-bottom) and never touches the
    // border around it - the app owns it (HUD bars, side panels, frame).
    bool cleared;   // false until the first full-screen paint (covers stale pixels)
    bool fast;      // true: display is a fast picogame.Display (DMA); false: a plain
                    // busdisplay rendered via the portable bus.send fallback
    #if CIRCUITPY_PICOGAME_FRAMEBUFFER
    bool fb_target; // true: self->display is a picogame.Framebuffer (RAM scanout buffer);
                    // refresh() composites dirty rects straight into it, no bus. Mutually
                    // exclusive with `fast` and the busdisplay path.
    #endif
    mp_obj_t dirty_rect;  // reusable [x1,y1,x2,y2] list returned by refresh() (no
                          // per-frame tuple allocation / GC churn)
} picogame_scene_obj_t;

// Diff items against snapshots and produce up to `max_rects` mostly-disjoint
// dirty rectangles (screen coords), updating snapshots / draining layer dirties.
// Returns the rect count (0 = nothing changed). Scattered movers yield several
// small rects instead of one screen-spanning union.
int picogame_scene_compute_dirty_rects(
    mp_obj_t *items, uint8_t *kinds, picogame_snapshot_t *snap, size_t n,
    int screen_w, int screen_h, int ox, int oy,
    picogame_rect_t *out, int max_rects);
