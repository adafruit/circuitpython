// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "shared-module/picogame/Scene.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/Tilemap.h"
#include "shared-module/picogame/Particles.h"
#include "shared-module/picogame/Canvas.h"

// Raw change rects collected before merging. Caps how many disjoint changes we
// track in one frame; on overflow we safely fall back to a full-screen repaint.
#define PICOGAME_RAW_RECTS 64

static inline bool rects_overlap(const picogame_rect_t *a, const picogame_rect_t *b) {
    return a->x1 < b->x2 && b->x1 < a->x2 && a->y1 < b->y2 && b->y1 < a->y2;
}

static inline void rect_merge(picogame_rect_t *a, const picogame_rect_t *b) {
    if (b->x1 < a->x1) {
        a->x1 = b->x1;
    }
    if (b->y1 < a->y1) {
        a->y1 = b->y1;
    }
    if (b->x2 > a->x2) {
        a->x2 = b->x2;
    }
    if (b->y2 > a->y2) {
        a->y2 = b->y2;
    }
}

static inline long rect_area(const picogame_rect_t *r) {
    return (long)(r->x2 - r->x1) * (long)(r->y2 - r->y1);
}

// Compute up to `max_rects` mostly-disjoint dirty rectangles (screen coords) and
// update the per-sprite snapshots. Returns the rect count (0 = nothing changed).
// Collecting per-change rects (instead of one union) means scattered movers no
// longer inflate the repaint to the whole screen.
// Out-of-line rect append for compute_dirty_rects below: the old macro expanded ~30 B
// four times; a real (noinline) call keeps each site at argument setup only.
static __attribute__((noinline)) void add_rect(picogame_rect_t *raw, int *nr, bool *overflow,
    int x1, int y1, int x2, int y2) {
    if (*nr < PICOGAME_RAW_RECTS) {
        raw[*nr].x1 = x1;
        raw[*nr].y1 = y1;
        raw[*nr].x2 = x2;
        raw[*nr].y2 = y2;
        (*nr)++;
    } else {
        *overflow = true;
    }
}

int picogame_scene_compute_dirty_rects(
    mp_obj_t *items, uint8_t *kinds, picogame_snapshot_t *snap, size_t n,
    int screen_w, int screen_h, int ox, int oy,
    picogame_rect_t *out, int max_rects) {

    picogame_rect_t raw[PICOGAME_RAW_RECTS];
    int nr = 0;
    bool overflow = false;

    // Rects are stored in SCREEN coords: non-fixed items get the view offset added
    // here (per item), fixed (HUD) items don't - so no uniform offset at the end.
    #define ADD_RECT(ax, ay, bx, by) \
    add_rect(raw, &nr, &overflow, (ax) + iox, (ay) + ioy, (bx) + iox, (by) + ioy)

    for (size_t i = 0; i < n; i++) {
        uint8_t rawk = kinds[i];
        uint8_t kind = rawk & PICOGAME_KIND_MASK;
        int iox = (rawk & PICOGAME_KIND_FIXED) ? 0 : ox;
        int ioy = (rawk & PICOGAME_KIND_FIXED) ? 0 : oy;
        if (kind != PICOGAME_KIND_SPRITE) {
            int tx1, ty1, tx2, ty2;
            bool d = false;
            if (kind == PICOGAME_KIND_TILEMAP) {
                d = picogame_tilemap_take_dirty(MP_OBJ_TO_PTR(items[i]), &tx1, &ty1, &tx2, &ty2);
            } else if (kind == PICOGAME_KIND_PARTICLES) {
                d = picogame_particles_take_dirty(MP_OBJ_TO_PTR(items[i]), &tx1, &ty1, &tx2, &ty2);
            } else if (kind == PICOGAME_KIND_CANVAS) {
                d = picogame_canvas_take_dirty(MP_OBJ_TO_PTR(items[i]), &tx1, &ty1, &tx2, &ty2);
            } else if (kind == PICOGAME_KIND_STRIPDRAW) {
                // No retained pixels to diff. always_dirty -> repaint the whole rect every frame
                // (animated content). Otherwise repaint only the accumulated invalidate() rect - the
                // same take_dirty contract as Canvas/Tilemap, so on-change UI repaints just its region
                // (and still re-runs when another layer's dirty rect overlaps it).
                picogame_stripdraw_obj_t *sd = MP_OBJ_TO_PTR(items[i]);
                if (sd->always_dirty) {
                    tx1 = sd->x;
                    ty1 = sd->y;
                    tx2 = sd->x + sd->w;
                    ty2 = sd->y + sd->h;
                    d = true;
                } else {
                    d = picogame_dirty_take(&sd->dx1, &tx1, &ty1, &tx2, &ty2);
                }
            } else if (kind == PICOGAME_KIND_TRIANGLES) {
                // Screen-space batch: count-set marked a full-screen dirty (clipped later).
                picogame_triangles_obj_t *t = MP_OBJ_TO_PTR(items[i]);
                d = picogame_dirty_take(&t->dx1, &tx1, &ty1, &tx2, &ty2);
            }
            if (d) {
                ADD_RECT(tx1, ty1, tx2, ty2);
            }
            continue;
        }

        picogame_sprite_obj_t *s = MP_OBJ_TO_PTR(items[i]);
        picogame_snapshot_t *sn = &snap[i];

        // Snapshot tracks the drawn screen AABB (already includes scale + rotation),
        // so position/anchor/size/scale/angle/bitmap changes are all detected.
        int ax1, ay1, ax2, ay2;
        picogame_sprite_aabb(s, &ax1, &ay1, &ax2, &ay2);
        picogame_bitmap_obj_t *bm = s->bitmap;
        bool changed = (ax1 != sn->x) || (ay1 != sn->y) ||
            ((ax2 - ax1) != sn->w) || ((ay2 - ay1) != sn->h) ||
            (s->frame != sn->frame) || (s->flags != sn->flags) ||
            (s->scale != sn->scale) || (s->angle != sn->angle) ||
            (s->seq != sn->seq) ||                 // touch(): in-place bitmap content change
            (s->dither != sn->dither) ||           // translucency level animation
            (s->flash_color != sn->flash_color) || // flash/tint colour-only change (flag stays set)
            ((void *)bm != sn->bitmap);
        if (!changed) {
            continue;
        }

        // Old rect = previous AABB (snapshot); new rect = current AABB.
        if (sn->flags & PICOGAME_SPR_VISIBLE) {
            ADD_RECT(sn->x, sn->y, sn->x + sn->w, sn->y + sn->h);
        }
        if (s->flags & PICOGAME_SPR_VISIBLE) {
            ADD_RECT(ax1, ay1, ax2, ay2);
        }

        sn->x = ax1;
        sn->y = ay1;
        sn->w = ax2 - ax1;
        sn->h = ay2 - ay1;
        sn->bitmap = (void *)bm;
        sn->frame = s->frame;
        sn->flags = s->flags;
        sn->scale = s->scale;
        sn->angle = s->angle;
        sn->seq = s->seq;
        sn->dither = s->dither;
        sn->flash_color = s->flash_color;
    }
#undef ADD_RECT

    if (nr == 0) {
        return 0;
    }
    // Too many changes to track individually -> one full-screen repaint is both
    // correct and likely cheaper than dozens of windows.
    if (overflow) {
        out[0].x1 = 0;
        out[0].y1 = 0;
        out[0].x2 = screen_w;
        out[0].y2 = screen_h;
        return 1;
    }

    // Merge overlapping rects (avoids painting the same pixels twice) until stable.
    bool again = true;
    while (again) {
        again = false;
        for (int i = 0; i < nr; i++) {
            for (int j = i + 1; j < nr; j++) {
                if (rects_overlap(&raw[i], &raw[j])) {
                    rect_merge(&raw[i], &raw[j]);
                    raw[j] = raw[nr - 1];
                    nr--;
                    again = true;
                    j--;
                }
            }
        }
    }

    // Cap the count by repeatedly merging the pair that wastes the fewest pixels.
    while (nr > max_rects) {
        int bi = 0, bj = 1;
        long best = -1;
        for (int i = 0; i < nr; i++) {
            long area_i = rect_area(&raw[i]);            // loop-invariant in j -> hoist out
            for (int j = i + 1; j < nr; j++) {
                picogame_rect_t u = raw[i];
                rect_merge(&u, &raw[j]);
                long waste = rect_area(&u) - area_i - rect_area(&raw[j]);
                if (best < 0 || waste < best) {
                    best = waste;
                    bi = i;
                    bj = j;
                }
            }
        }
        rect_merge(&raw[bi], &raw[bj]);
        raw[bj] = raw[nr - 1];
        nr--;
    }

    // Rects are already in screen coords (offset applied per item). Clip + drop empties.
    int outn = 0;
    for (int i = 0; i < nr; i++) {
        int x1 = picogame_imax(raw[i].x1, 0);
        int y1 = picogame_imax(raw[i].y1, 0);
        int x2 = picogame_imin(raw[i].x2, screen_w);
        int y2 = picogame_imin(raw[i].y2, screen_h);
        if (x1 >= x2 || y1 >= y2) {
            continue;
        }
        out[outn].x1 = x1;
        out[outn].y1 = y1;
        out[outn].x2 = x2;
        out[outn].y2 = y2;
        outn++;
    }
    return outn;
}
