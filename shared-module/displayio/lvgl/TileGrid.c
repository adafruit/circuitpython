// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// LVGL-based displayio TileGrid implementation.
//
// A TileGrid owns one composed buffer holding exactly one tile grid of pixels, in tile-matrix
// order: the pixels of tiles[] cell (i, j) live at (i * tile_width, j * tile_height), with the
// tile indirection and the palette already resolved. Cells are composed through LVGL's own draw
// pipeline (a hidden lv_canvas plus lv_draw_image() per changed cell), so the shim has no pixel
// loop: palette expansion, alpha, format conversion and clipping are LVGL's work.
//
// The display shows that buffer through up to four lv_images ("views"): the 2x2 split of the
// wrapped window that displayio's scrolling describes, where display column i shows matrix column
// (i + top_left_x) % width_in_tiles. A scroll therefore re-points the views and invalidates; no
// pixels move and no cell is re-sourced. The common cases (no scroll, or scrolling by whole rows
// with top_left_x == 0) are one and two views.
//
// A single-cell grid of a direct color source skips the buffer entirely: one view draws the source
// pixels as they are, which is what plain image viewers and the boot logo want.
//
// Composition resolves the palette, so a Bitmap never owns an LVGL image, a descriptor or a
// palette copy. It only tells the TileGrids drawing it which pixels changed
// (displayio_bitmap_set_dirty()), and they re-read that area before the next refresh.
//
// Everything here runs synchronously on the CircuitPython thread under the port's LVGL lock,
// except the compose pass, which runs at the start of an LVGL refresh with the lock already held
// by the renderer (see displayio_lvgl_register_display()).

#include "shared-bindings/displayio/TileGrid.h"

#include <string.h>

#include "py/gc.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/__init__.h"
#include "shared-module/displayio/lvgl/lvgl_internal.h"

#include "misc/cache/instance/lv_image_cache.h"

#if CIRCUITPY_DISPLAYIO_LVGL

// Tile-index debug logging goes through ESP_LOG, not mp_printf: mp_printf ends up in
// serial_write_substring(), which writes the display terminal, which calls back into
// common_hal_displayio_tilegrid_set_tile() -- recursion, then a crash. The IDF log console is a
// separate sink, so it can be used from the write path and from the compose pass.
//
// Off by default: this prints several lines per grid per refresh, which buries the heap and
// out-of-memory logging in lvgl_port.c and port.c. Flip to 1 to watch the compose pass; the
// memory logging is independent and stays on either way.
#define DISPLAYIO_TILEGRID_DEBUG_ENABLED 0

#if DISPLAYIO_TILEGRID_DEBUG_ENABLED && CIRCUITPY_ESPIDF
#include "esp_log.h"
#define DISPLAYIO_TILEGRID_DEBUG(...) ESP_LOGI("TileGrid", __VA_ARGS__)
#else
#define DISPLAYIO_TILEGRID_DEBUG(...) ((void)0)
#endif

// Tile index 0 is the blank tile: the terminal fills itself with spaces, and every tiles[] array
// starts zeroed. Logging those hides the glyph changes we care about.
#define DISPLAYIO_TILEGRID_BLANK_TILE 0

static void _cell_display_area(displayio_tilegrid_t *self, uint32_t x, uint32_t y, lv_area_t *area);

static void _raise_unsupported(void) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("LVGL displayio backend does not support flips/transpose yet"));
}

// --- Attached mirrors -------------------------------------------------------

// Every attached TileGrid. The compose pass walks it, the Bitmap write path looks for the grids
// that draw a bitmap in it, and displayio_lvgl_tilegrid_gc_collect() roots it: the list lives in
// port memory, which the GC can't see.
static displayio_tilegrid_t *mirror_head = NULL;

static bool _is_linked(displayio_tilegrid_t *self) {
    for (displayio_tilegrid_t *other = mirror_head; other != NULL; other = other->lv_next) {
        if (other == self) {
            return true;
        }
    }
    return false;
}

// A Bitmap keeps its own list of the attached grids drawing it. A pixel write only walks the
// grids that can actually be affected, and an empty list means the write can skip LVGL entirely.
// Only the CircuitPython thread mutates these lists, under the LVGL lock.
static void _watch_bitmap(displayio_tilegrid_t *self) {
    if (!mp_obj_is_type(self->bitmap, &displayio_bitmap_type)) {
        return;
    }
    displayio_bitmap_t *bitmap = MP_OBJ_TO_PTR(self->bitmap);
    self->lv_bitmap_next = bitmap->lv_watch_head;
    bitmap->lv_watch_head = self;
}

static void _unwatch_bitmap(displayio_tilegrid_t *self) {
    if (!mp_obj_is_type(self->bitmap, &displayio_bitmap_type)) {
        return;
    }
    displayio_bitmap_t *bitmap = MP_OBJ_TO_PTR(self->bitmap);
    displayio_tilegrid_t **link = &bitmap->lv_watch_head;
    while (*link != NULL) {
        if (*link == self) {
            *link = self->lv_bitmap_next;
            break;
        }
        link = &(*link)->lv_bitmap_next;
    }
    self->lv_bitmap_next = NULL;
}

static void _link_mirror(displayio_tilegrid_t *self) {
    if (_is_linked(self)) {
        return;
    }
    self->lv_next = mirror_head;
    mirror_head = self;
    _watch_bitmap(self);
}

static void _unlink_mirror(displayio_tilegrid_t *self) {
    bool linked = false;
    displayio_tilegrid_t **link = &mirror_head;
    while (*link != NULL) {
        if (*link == self) {
            *link = self->lv_next;
            linked = true;
            break;
        }
        link = &(*link)->lv_next;
    }
    self->lv_next = NULL;
    if (linked) {
        _unwatch_bitmap(self);
    }
}

void displayio_lvgl_tilegrid_gc_collect(void) {
    for (displayio_tilegrid_t *self = mirror_head; self != NULL; self = self->lv_next) {
        gc_collect_ptr(self);
    }
}

// --- Cells -----------------------------------------------------------------

static uint32_t _cell_count(displayio_tilegrid_t *self) {
    return (uint32_t)self->width_in_tiles * self->height_in_tiles;
}

// Tile pixel origin in the source bitmap for a tile index.
static void _tile_origin(displayio_tilegrid_t *self, uint16_t tile_index, uint16_t *x0, uint16_t *y0) {
    *x0 = (tile_index % self->bitmap_width_in_tiles) * self->tile_width;
    *y0 = (tile_index / self->bitmap_width_in_tiles) * self->tile_height;
}

// Read one index from an indexed source row. LVGL packs indices MSB first within each byte.
static uint8_t _source_index(const uint8_t *row, uint32_t x, uint8_t bpp) {
    uint32_t bit = x * bpp;
    return (row[bit >> 3] >> (8 - bpp - (bit & 7))) & ((1u << bpp) - 1);
}

// Whether a tile holds only palette entry 0. A compose buffer is cleared to entry 0 (black or
// transparent), so only such a cell can be left out of the compose draw entirely.
static bool _tile_is_blank(displayio_tilegrid_t *self, uint16_t tile_index) {
    const lv_image_dsc_t *src = self->lv_src;
    if (src == NULL || src->data == NULL) {
        return false;
    }
    uint8_t bpp = lv_color_format_get_bpp(src->header.cf);
    if (bpp != 1 && bpp != 2 && bpp != 4 && bpp != 8) {
        return false;
    }
    uint16_t x0, y0;
    _tile_origin(self, tile_index, &x0, &y0);
    for (uint16_t y = 0; y < self->tile_height; y++) {
        const uint8_t *row = src->data + (uint32_t)(y0 + y) * src->header.stride;
        for (uint16_t x = 0; x < self->tile_width; x++) {
            if (_source_index(row, x0 + x, bpp) != 0) {
                return false;
            }
        }
    }
    return true;
}

// Read the tile index stored for a cell of the tile matrix, in the same order as the composed
// buffer: cell (i, j) shows tiles[j * width_in_tiles + i].
static uint16_t _tile_index(displayio_tilegrid_t *self, uint32_t cell_index) {
    if (self->tiles == NULL) {
        return 0;
    }
    if (self->tiles_in_bitmap > 255) {
        return ((uint16_t *)self->tiles)[cell_index];
    }
    return ((uint8_t *)self->tiles)[cell_index];
}

// A composed grid is brought up to date by the compose pass, which runs at the start of a refresh
// (LV_EVENT_REFR_START, before LVGL reads its invalid-area list), so a change has to invalidate
// something or LVGL never runs a refresh and the grid stays stale forever. Invalidate the area the
// changed cell occupies on screen: that is the same rectangle the compose pass would invalidate,
// and it keeps `display.refresh()`/auto_refresh-independent redraws honest even before the compose
// runs. Only the first change of a burst does this -- invalidating every changed cell would
// overflow LVGL's invalidation buffer (LV_INV_BUF_SIZE, 32 rects) and repaint the whole screen --
// and the compose pass invalidates the union of everything it recomposed in the same pass.
// `cell` < 0 means "not one cell", i.e. invalidate the whole grid.
static void _wake_refresh(displayio_tilegrid_t *self, int32_t cell) {
    if (self->lv_obj == NULL || lv_obj_get_display(self->lv_obj) == NULL) {
        // Not attached yet: attaching marks everything dirty, so a later write wakes it.
        return;
    }
    if (cell < 0 || self->lv_full_dirty || self->width_in_tiles == 0 ||
        self->height_in_tiles == 0) {
        DISPLAYIO_TILEGRID_DEBUG("wake %p whole obj", self);
        lv_obj_invalidate(self->lv_obj);
        return;
    }
    lv_area_t area;
    _cell_display_area(self, (uint32_t)cell % self->width_in_tiles,
        (uint32_t)cell / self->width_in_tiles, &area);
    if (area.x2 < area.x1) {
        DISPLAYIO_TILEGRID_DEBUG("wake %p whole obj (no cell area)", self);
        lv_obj_invalidate(self->lv_obj);
        return;
    }
    // The area comes back in container coordinates; LVGL invalidates in screen coordinates.
    lv_area_t obj_coords;
    lv_obj_get_coords(self->lv_obj, &obj_coords);
    lv_area_move(&area, obj_coords.x1, obj_coords.y1);
    DISPLAYIO_TILEGRID_DEBUG("wake %p cell %ld at (%ld,%ld)-(%ld,%ld)", self, (long)cell,
        (long)area.x1, (long)area.y1, (long)area.x2, (long)area.y2);
    lv_obj_invalidate_area(self->lv_obj, &area);
}

static void _mark_work(displayio_tilegrid_t *self) {
    if (self->lv_work) {
        return;
    }
    self->lv_work = true;
    _wake_refresh(self, -1);
}

static void _mark_all(displayio_tilegrid_t *self) {
    self->lv_full_dirty = true;
    _mark_work(self);
}

static void _mark_views(displayio_tilegrid_t *self) {
    self->lv_views_dirty = true;
    _mark_work(self);
}

static void _mark_cell(displayio_tilegrid_t *self, uint32_t cell) {
    if (self->lv_cell_dirty == NULL || cell >= _cell_count(self)) {
        _mark_all(self);
        return;
    }
    uint32_t *word = &self->lv_cell_dirty[cell / 32];
    uint32_t bit = 1u << (cell % 32);
    bool was_working = self->lv_work;
    if ((*word & bit) == 0) {
        *word |= bit;
        self->lv_dirty_count++;
    }
    self->lv_work = true;
    if (!was_working) {
        _wake_refresh(self, (int32_t)cell);
    }
}

static bool _cell_is_marked(displayio_tilegrid_t *self, uint32_t cell) {
    return (self->lv_cell_dirty[cell / 32] & (1u << (cell % 32))) != 0;
}

static void _clear_cell(displayio_tilegrid_t *self, uint32_t cell) {
    if (self->lv_cell_dirty != NULL) {
        self->lv_cell_dirty[cell / 32] &= ~(1u << (cell % 32));
    }
}

// The supervisor pokes grid dimensions straight into the static terminal grids, so the bitset is
// checked against the current dimensions instead of assuming it still fits.
static void _ensure_cell_bits(displayio_tilegrid_t *self) {
    uint32_t words = (_cell_count(self) + 31) / 32;
    if (words == 0) {
        words = 1;
    }
    if (self->lv_cell_dirty != NULL && self->lv_cell_words >= words) {
        return;
    }
    if (self->lv_cell_dirty != NULL) {
        lv_free(self->lv_cell_dirty);
    }
    self->lv_cell_dirty = lv_malloc_zeroed(words * sizeof(uint32_t));
    self->lv_cell_words = words;
    self->lv_dirty_count = 0;
    self->lv_full_dirty = true;
}

// --- The source atlas -------------------------------------------------------

// The LVGL color format that draws bitmap with pixel_shader. Raises NotImplementedError for the
// combinations that haven't been ported to LVGL.
static lv_color_format_t _source_cf(mp_obj_t bitmap, mp_obj_t pixel_shader) {
    if (mp_obj_is_type(bitmap, &displayio_ondiskbitmap_type)) {
        displayio_ondiskbitmap_t *odb = MP_OBJ_TO_PTR(bitmap);
        lv_color_format_t cf = displayio_ondiskbitmap_lv_color_format(odb);
        if (cf == LV_COLOR_FORMAT_UNKNOWN) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("Format not supported"));
        }
        // The file owns its pixel shader (Palette for indexed files, pass-through
        // ColorConverter otherwise), so a different one can't be applied.
        mp_obj_t odb_shader = common_hal_displayio_ondiskbitmap_get_pixel_shader(odb);
        if (MP_OBJ_TO_PTR(pixel_shader) != MP_OBJ_TO_PTR(odb_shader)) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("OnDiskBitmap requires its own pixel_shader"));
        }
        return cf;
    }
    if (mp_obj_is_type(bitmap, &displayio_bitmap_type)) {
        uint8_t bpp = ((displayio_bitmap_t *)MP_OBJ_TO_PTR(bitmap))->bits_per_value;
        if (mp_obj_is_type(pixel_shader, &displayio_palette_type)) {
            switch (bpp) {
                case 1:
                    return LV_COLOR_FORMAT_I1;
                case 2:
                    return LV_COLOR_FORMAT_I2;
                case 4:
                    return LV_COLOR_FORMAT_I4;
                case 8:
                    return LV_COLOR_FORMAT_I8;
            }
        } else if (mp_obj_is_type(pixel_shader, &displayio_colorconverter_type)) {
            displayio_colorconverter_t *converter = MP_OBJ_TO_PTR(pixel_shader);
            if (converter->transparent_color == NO_TRANSPARENT_COLOR) {
                if (bpp == 16 && converter->input_colorspace == DISPLAYIO_COLORSPACE_RGB565) {
                    return LV_COLOR_FORMAT_RGB565;
                }
                if (bpp == 16 && converter->input_colorspace == DISPLAYIO_COLORSPACE_RGB565_SWAPPED) {
                    return LV_COLOR_FORMAT_RGB565_SWAPPED;
                }
                if (bpp == 32 && converter->input_colorspace == DISPLAYIO_COLORSPACE_RGB888) {
                    return LV_COLOR_FORMAT_XRGB8888;
                }
                if (bpp == 8 && converter->input_colorspace == DISPLAYIO_COLORSPACE_L8) {
                    return LV_COLOR_FORMAT_L8;
                }
            }
        }
    }
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Format not supported"));
}

// displayio's only alpha is the palette's per-index "transparent" flag (plus indices past the end
// of the palette). A composed buffer has to carry that through to the display, so those grids
// compose to ARGB8888 with real alpha, and everything else composes to RGB565, which the display
// path takes directly.
static bool _palette_has_transparency(displayio_palette_t *palette, uint32_t slots) {
    for (uint32_t i = 0; i < palette->color_count; i++) {
        if (palette->colors[i].transparent) {
            return true;
        }
    }
    return palette->color_count < slots;
}

static bool _source_has_alpha(displayio_tilegrid_t *self, lv_color_format_t cf) {
    if (mp_obj_is_type(self->bitmap, &displayio_ondiskbitmap_type)) {
        displayio_ondiskbitmap_t *odb = MP_OBJ_TO_PTR(self->bitmap);
        if (odb->bits_per_pixel > 8 || odb->palette == NULL) {
            return false;
        }
        return _palette_has_transparency(odb->palette, 1u << odb->bits_per_pixel);
    }
    if (!LV_COLOR_FORMAT_IS_INDEXED(cf) || !mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
        return false;
    }
    return _palette_has_transparency(MP_OBJ_TO_PTR(self->pixel_shader),
        LV_COLOR_INDEXED_PALETTE_SIZE(cf));
}

static bool _source_is_indexed(displayio_tilegrid_t *self) {
    if (mp_obj_is_type(self->bitmap, &displayio_ondiskbitmap_type)) {
        // OnDiskBitmap always expands indexed files to a direct color format.
        return false;
    }
    return LV_COLOR_FORMAT_IS_INDEXED(_source_cf(self->bitmap, self->pixel_shader));
}

// A single cell showing a direct color source is exactly the source pixels, so there is nothing
// to compose and nothing to copy.
static bool _wants_direct(displayio_tilegrid_t *self) {
    return self->width_in_tiles == 1 && self->height_in_tiles == 1 && !_source_is_indexed(self);
}

// Point the source descriptor at the whole bitmap. Caller holds the lock and has no source bound.
static bool _bind_source(displayio_tilegrid_t *self) {
    lv_color_format_t cf;
    if (mp_obj_is_type(self->bitmap, &displayio_ondiskbitmap_type)) {
        displayio_ondiskbitmap_t *odb = MP_OBJ_TO_PTR(self->bitmap);
        cf = displayio_ondiskbitmap_lv_color_format(odb);
        if (cf == LV_COLOR_FORMAT_UNKNOWN) {
            DISPLAYIO_TILEGRID_DEBUG("bind %p ondisk bitmap has no LVGL format", self);
            return false;
        }
        // LVGL can't transform a partially decoded image, but a composed grid draws its cells
        // unscaled and scales the views of its own buffer instead.
        self->lv_src_odb_full = self->lv_direct && self->lv_scale != 1;
        self->lv_src = displayio_ondiskbitmap_lv_acquire(odb, self->lv_src_odb_full, self->lv_obj);
        self->lv_argb = _source_has_alpha(self, cf);
        return self->lv_src != NULL;
    }

    displayio_bitmap_t *bitmap = MP_OBJ_TO_PTR(self->bitmap);
    cf = _source_cf(self->bitmap, self->pixel_shader);
    uint32_t stride = (uint32_t)bitmap->stride * sizeof(uint32_t);
    lv_image_dsc_t *dsc = &self->lv_src_dsc;
    lv_memzero(dsc, sizeof(*dsc));
    dsc->header.magic = LV_IMAGE_HEADER_MAGIC;
    dsc->header.cf = cf;
    dsc->header.w = bitmap->width;
    dsc->header.h = bitmap->height;
    dsc->header.stride = stride;
    if (LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        // The indices are handed to LVGL in place and the palette is a separate, shared LVGL
        // view of the displayio Palette (see displayio_lvgl_shader_palette()), so neither the
        // bitmap nor the palette is copied into the TileGrid.
        if (bitmap->data == NULL) {
            DISPLAYIO_TILEGRID_DEBUG("bind %p bitmap has no data", self);
            return false;
        }
        dsc->header.flags |= LV_IMAGE_FLAGS_PALETTE_EXTERNAL;
        dsc->data = (const uint8_t *)bitmap->data;
        dsc->data_size = (uint32_t)stride * bitmap->height;
        displayio_lvgl_shader_register_user(self->pixel_shader, self,
            DISPLAYIO_LVGL_USER_TILEGRID);
        dsc->palette = displayio_lvgl_shader_palette(self->pixel_shader);
        if (dsc->palette == NULL) {
            DISPLAYIO_TILEGRID_DEBUG("bind %p palette has no LVGL view", self);
            displayio_lvgl_shader_unregister_user(self->pixel_shader, self);
            return false;
        }
    } else if (bitmap->data != NULL) {
        dsc->data = (const uint8_t *)bitmap->data;
        dsc->data_size = (uint32_t)stride * bitmap->height;
    }
    self->lv_src = dsc;
    self->lv_argb = _source_has_alpha(self, cf);
    DISPLAYIO_TILEGRID_DEBUG("bind %p cf %d %dx%d stride %lu data %p palette %p",
        self, (int)cf, bitmap->width, bitmap->height, (unsigned long)stride, dsc->data,
        dsc->palette);
    return true;
}

// Undo _bind_source(). Caller holds the lock.
static void _unbind_source(displayio_tilegrid_t *self) {
    if (self->lv_src == &self->lv_src_dsc) {
        // A Bitmap source is a descriptor over the Bitmap's own pixels. Nothing is owned, but
        // the Palette registry and the image cache still point at it.
        lv_image_cache_drop(&self->lv_src_dsc);
        displayio_lvgl_shader_unregister_user(self->pixel_shader, self);
        lv_memzero(&self->lv_src_dsc, sizeof(self->lv_src_dsc));
    } else if (self->lv_src != NULL && mp_obj_is_type(self->bitmap, &displayio_ondiskbitmap_type)) {
        displayio_ondiskbitmap_lv_release(MP_OBJ_TO_PTR(self->bitmap), self->lv_src_odb_full,
            self->lv_obj);
    }
    self->lv_src = NULL;
    self->lv_src_odb_full = false;
}

// --- Objects ---------------------------------------------------------------

// lv_obj_set_pos() only records a style that a later layout pass applies, and lv_obj_move_to()
// leaves that style behind, so an unrelated size change would then snap the object back to where
// the style says. Set both: the coordinates are right immediately and nothing can undo them.
static void _place(lv_obj_t *obj, int32_t x, int32_t y) {
    lv_obj_set_style_x(obj, x, 0);
    lv_obj_set_style_y(obj, y, 0);
    lv_obj_move_to(obj, x, y);
}

// Place the mirror in screen coordinates.
static void _set_lv_pos(displayio_tilegrid_t *self) {
    if (self->lv_obj == NULL) {
        return;
    }
    _place(self->lv_obj, self->lv_origin_x + self->x * self->lv_scale,
        self->lv_origin_y + self->y * self->lv_scale);
}

// Scale a view from its top left corner. Moving a pivot makes LVGL lay out the whole screen, and
// the pivot only matters once the view is scaled.
static void _set_view_scale(lv_obj_t *img, uint16_t scale) {
    if (scale != 1) {
        lv_image_set_pivot(img, 0, 0);
    }
    lv_image_set_scale(img, (int32_t)scale * LV_SCALE_NONE);
}

// Create one view: an lv_image that shows one window of the composed buffer, scaled about its top
// left corner like the legacy compositor's whole-pixel scaling.
static lv_obj_t *_create_view(displayio_tilegrid_t *self, lv_obj_t *parent) {
    lv_obj_t *img = lv_image_create(parent);
    if (img == NULL) {
        return NULL;
    }
    lv_obj_clear_flag(img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TOP_LEFT);
    lv_image_set_antialias(img, false);
    _set_view_scale(img, self->lv_scale);
    return img;
}

static bool _ensure_canvas(displayio_tilegrid_t *self) {
    if (self->lv_canvas != NULL) {
        return true;
    }
    lv_obj_t *canvas = lv_canvas_create(self->lv_obj);
    if (canvas == NULL) {
        return false;
    }
    // The canvas is only here to provide a layer and a display for the compose pass; it is hidden
    // and never draws itself.
    lv_obj_remove_style_all(canvas);
    lv_obj_add_flag(canvas, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_SCROLLABLE);
    self->lv_canvas = canvas;
    return true;
}

// Build the container the views live in. One object per TileGrid keeps the group's z-order right,
// and the container carries the grid's position and scale so moving a grid never touches its
// views.
static bool _create_objects(displayio_tilegrid_t *self, lv_obj_t *parent) {
    lv_obj_t *container = lv_obj_create(parent);
    if (container == NULL) {
        return false;
    }
    // Plain, transparent, no layout: the views are placed explicitly, so the LAYOUT_UPDATE that
    // lv_obj_set_size() carries resolves to a pass over a handful of children.
    lv_obj_remove_style_all(container);
    lv_obj_add_flag(container, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(container, self->pixel_width * self->lv_scale,
        self->pixel_height * self->lv_scale);
    self->lv_obj = container;
    return _ensure_canvas(self);
}

// Tear the objects down. The composed buffer is ours (a canvas never frees its draw buffer), so it
// goes after the tree that might still reference it. Caller holds the lock.
static void _destroy_objects(displayio_tilegrid_t *self) {
    lv_draw_buf_t *buf = self->lv_buf;
    self->lv_buf = NULL;
    if (self->lv_obj != NULL) {
        lv_obj_delete(self->lv_obj);
    }
    self->lv_obj = NULL;
    self->lv_canvas = NULL;
    self->lv_view_count = 0;
    for (uint8_t i = 0; i < DISPLAYIO_LVGL_TILEGRID_VIEWS; i++) {
        self->lv_views[i] = NULL;
    }
    if (buf != NULL) {
        lv_draw_buf_destroy(buf);
    }
}

// --- The composed buffer ---------------------------------------------------

static bool _ensure_buffer(displayio_tilegrid_t *self) {
    uint32_t w = self->pixel_width;
    uint32_t h = self->pixel_height;
    if (w == 0 || h == 0 || self->lv_obj == NULL) {
        return false;
    }
    lv_color_format_t cf = self->lv_argb ? LV_COLOR_FORMAT_ARGB8888 : LV_COLOR_FORMAT_RGB565;
    if (self->lv_buf != NULL && self->lv_buf->header.w == w && self->lv_buf->header.h == h &&
        self->lv_buf->header.cf == cf) {
        return true;
    }
    if (!_ensure_canvas(self)) {
        return false;
    }
    lv_draw_buf_t *old = self->lv_buf;
    // ESP_LOGI(TAG, "creating draw buf %d x %d", w, h);
    lv_draw_buf_t *buf = lv_draw_buf_create(w, h, cf, 0);
    if (buf == NULL) {
        return false;
    }
    lv_canvas_set_draw_buf(self->lv_canvas, buf);
    self->lv_buf = buf;
    if (old != NULL) {
        lv_draw_buf_destroy(old);
    }
    _ensure_cell_bits(self);
    _mark_all(self);
    _mark_views(self);
    return true;
}

// One run of the scroll window: pixels [src, src + len) of the buffer go to display offset dst.
typedef struct {
    uint32_t src;
    uint32_t len;
    uint32_t dst;
} _run_t;

static void _runs(uint32_t offset, uint32_t extent, _run_t runs[2], uint8_t *count) {
    if (offset == 0) {
        runs[0].src = 0;
        runs[0].len = extent;
        runs[0].dst = 0;
        *count = 1;
        return;
    }
    runs[0].src = offset;
    runs[0].len = extent - offset;
    runs[0].dst = 0;
    runs[1].src = 0;
    runs[1].len = offset;
    runs[1].dst = extent - offset;
    *count = 2;
}

// Create views up to `needed`. Returns false when one can't be allocated.
static bool _ensure_views(displayio_tilegrid_t *self, uint8_t needed) {
    for (uint8_t i = 0; i < needed; i++) {
        if (self->lv_views[i] == NULL) {
            self->lv_views[i] = _create_view(self, self->lv_obj);
            if (self->lv_views[i] == NULL) {
                // Out of memory: try again on the next refresh.
                self->lv_views_dirty = true;
                return false;
            }
        }
    }
    return true;
}

// Point the views at the current window of the buffer, or at the source itself on the direct path.
// Caller holds the lock.
static void _layout_views(displayio_tilegrid_t *self) {
    self->lv_views_dirty = false;
    if (self->lv_obj == NULL) {
        return;
    }
    // Views are created on demand, so an unscrolled grid has one and the direct path has one too.
    if (self->lv_direct) {
        if (!_ensure_views(self, 1)) {
            return;
        }
        // The view is the source: the tile's origin in the atlas becomes a negative offset, in
        // scaled pixels, so that the tile's top left lands on the grid's top left.
        uint16_t x0, y0;
        _tile_origin(self, _tile_index(self, 0), &x0, &y0);
        lv_image_set_src(self->lv_views[0],
            self->lv_src != NULL && self->lv_src->data != NULL ? self->lv_src : NULL);
        lv_image_set_offset_x(self->lv_views[0], -(int32_t)x0 * self->lv_scale);
        lv_image_set_offset_y(self->lv_views[0], -(int32_t)y0 * self->lv_scale);
        lv_obj_set_size(self->lv_views[0], self->tile_width * self->lv_scale,
            self->tile_height * self->lv_scale);
        lv_obj_clear_flag(self->lv_views[0], LV_OBJ_FLAG_HIDDEN);
        _place(self->lv_views[0], 0, 0);
        for (uint8_t i = 1; i < DISPLAYIO_LVGL_TILEGRID_VIEWS; i++) {
            if (self->lv_views[i] != NULL) {
                lv_obj_add_flag(self->lv_views[i], LV_OBJ_FLAG_HIDDEN);
            }
        }
        self->lv_view_count = 1;
        return;
    }
    if (self->lv_buf == NULL || self->width_in_tiles == 0 || self->height_in_tiles == 0 ||
        self->pixel_width == 0 || self->pixel_height == 0) {
        return;
    }
    uint32_t grid_w = self->pixel_width;
    uint32_t grid_h = self->pixel_height;
    uint32_t tx = (self->top_left_x % self->width_in_tiles) * self->tile_width;
    uint32_t ty = (self->top_left_y % self->height_in_tiles) * self->tile_height;
    _run_t rows[2];
    _run_t cols[2];
    uint8_t row_count;
    uint8_t col_count;
    _runs(ty, grid_h, rows, &row_count);
    _runs(tx, grid_w, cols, &col_count);
    if (!_ensure_views(self, row_count * col_count)) {
        return;
    }

    uint32_t stride = self->lv_buf->header.stride;
    lv_color_format_t cf = self->lv_buf->header.cf;
    uint32_t bytes_per_pixel = lv_color_format_get_size(cf);
    uint8_t view = 0;
    for (uint8_t r = 0; r < row_count; r++) {
        for (uint8_t c = 0; c < col_count; c++) {
            lv_image_dsc_t *dsc = &self->lv_view_dsc[view];
            lv_memzero(dsc, sizeof(*dsc));
            dsc->header.magic = LV_IMAGE_HEADER_MAGIC;
            dsc->header.cf = cf;
            dsc->header.w = cols[c].len;
            dsc->header.h = rows[r].len;
            dsc->header.stride = stride;
            dsc->data = self->lv_buf->data + rows[r].src * stride +
                cols[c].src * bytes_per_pixel;
            dsc->data_size = stride * rows[r].len;
            lv_obj_t *img = self->lv_views[view];
            lv_image_set_src(img, dsc);
            lv_image_set_offset_x(img, 0);
            lv_image_set_offset_y(img, 0);
            lv_obj_set_size(img, cols[c].len * self->lv_scale, rows[r].len * self->lv_scale);
            lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
            _place(img, cols[c].dst * self->lv_scale, rows[r].dst * self->lv_scale);
            DISPLAYIO_TILEGRID_DEBUG(
                "views %p view %d buf %p cf %d %dx%d stride %lu data %p at (%d,%d)",
                self, view, self->lv_buf->data, (int)cf, cols[c].len, rows[r].len,
                (unsigned long)stride, (void *)dsc->data, (int)(cols[c].dst * self->lv_scale),
                (int)(rows[r].dst * self->lv_scale));
            view++;
        }
    }
    self->lv_view_count = view;
    DISPLAYIO_TILEGRID_DEBUG(
        "views %p count %d grid %lux%lu scroll (%lu,%lu) -> (%lu,%lu) scale %u",
        self, view, (unsigned long)grid_w, (unsigned long)grid_h,
        (unsigned long)(self->top_left_x % self->width_in_tiles),
        (unsigned long)(self->top_left_y % self->height_in_tiles), (unsigned long)tx,
        (unsigned long)ty, (unsigned)self->lv_scale);
    for (uint8_t i = view; i < DISPLAYIO_LVGL_TILEGRID_VIEWS; i++) {
        if (self->lv_views[i] != NULL) {
            lv_obj_add_flag(self->lv_views[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

// --- Compose ---------------------------------------------------------------

// Grow `area` to cover `other` too.
static void _union_area(lv_area_t *area, const lv_area_t *other) {
    if (other->x1 < area->x1) {
        area->x1 = other->x1;
    }
    if (other->y1 < area->y1) {
        area->y1 = other->y1;
    }
    if (other->x2 > area->x2) {
        area->x2 = other->x2;
    }
    if (other->y2 > area->y2) {
        area->y2 = other->y2;
    }
}

// Cells whose tile overlaps the changed source rect need composing. This is the inverse of the
// tile map; the terminal's 560 byte compares are free next to a frame.
static void _mark_cells_for_source(displayio_tilegrid_t *self, int32_t x1, int32_t y1,
    int32_t x2, int32_t y2) {
    if (self->lv_full_dirty) {
        return;
    }
    uint32_t cells = _cell_count(self);
    for (uint32_t cell = 0; cell < cells; cell++) {
        uint16_t x0, y0;
        _tile_origin(self, _tile_index(self, cell), &x0, &y0);
        if ((int32_t)x0 < x2 && (int32_t)x0 + self->tile_width > x1 &&
            (int32_t)y0 < y2 && (int32_t)y0 + self->tile_height > y1) {
            _mark_cell(self, cell);
        }
    }
}

// The bitmap's pixels changed, so drop any decoded view and mark the cells the area reaches.
// Indexed sources are drawn from the Bitmap's own memory, so there is nothing to copy here.
static void _sync_source(displayio_tilegrid_t *self) {
    if (!self->lv_src_dirty) {
        return;
    }
    self->lv_src_dirty = false;
    if (self->lv_src != NULL) {
        lv_image_cache_drop(self->lv_src);
    }
    _mark_cells_for_source(self, self->lv_src_x1, self->lv_src_y1, self->lv_src_x2,
        self->lv_src_y2);
}

// Where cell (x, y) of the tile matrix lands on the display, in container coordinates.
static void _cell_display_area(displayio_tilegrid_t *self, uint32_t x, uint32_t y, lv_area_t *area) {
    int32_t grid_w = self->pixel_width;
    int32_t grid_h = self->pixel_height;
    if (grid_w <= 0 || grid_h <= 0 || self->width_in_tiles == 0 || self->height_in_tiles == 0) {
        *area = (lv_area_t) {0, 0, -1, -1};
        return;
    }
    int32_t tx = (self->top_left_x % self->width_in_tiles) * self->tile_width;
    int32_t ty = (self->top_left_y % self->height_in_tiles) * self->tile_height;
    int32_t px = ((int32_t)x * self->tile_width - tx) % grid_w;
    int32_t py = ((int32_t)y * self->tile_height - ty) % grid_h;
    if (px < 0) {
        px += grid_w;
    }
    if (py < 0) {
        py += grid_h;
    }
    area->x1 = px * self->lv_scale;
    area->y1 = py * self->lv_scale;
    area->x2 = area->x1 + self->tile_width * self->lv_scale - 1;
    area->y2 = area->y1 + self->tile_height * self->lv_scale - 1;
}

// The most cells a compose pass queues before drawing them. LVGL walks the whole task list to add
// or dispatch each task, so drawing a long queue at once takes time with the square of its length.
#define DISPLAYIO_LVGL_COMPOSE_BATCH 64

// Fast compose for the common case: an indexed source (I1/I2/I4/I8) with a palette. Each
// cell is a straight palette lookup into the buffer, which is what lv_draw_image() of the cell
// onto a cleared buffer produces, without a draw task per cell. LVGL's per-task cost is tens of
// microseconds, which a 1500-cell text grid turns into 100+ ms per refresh.
typedef struct {
    bool ok;
    bool argb;
    uint8_t bpp;
    uint32_t lut32[256];
    uint16_t lut16[256];
} _fast_compose_t;

static void _fast_compose_init(displayio_tilegrid_t *self, _fast_compose_t *fc) {
    fc->ok = false;
    const lv_image_dsc_t *src = self->lv_src;
    if (src != &self->lv_src_dsc || src == NULL || src->data == NULL || src->palette == NULL ||
        !LV_COLOR_FORMAT_IS_INDEXED(src->header.cf)) {
        return;
    }
    lv_color_format_t dcf = self->lv_buf->header.cf;
    if (dcf != LV_COLOR_FORMAT_ARGB8888 && dcf != LV_COLOR_FORMAT_RGB565) {
        return;
    }
    fc->bpp = lv_color_format_get_bpp(src->header.cf);
    if (fc->bpp != 1 && fc->bpp != 2 && fc->bpp != 4 && fc->bpp != 8) {
        return;
    }
    fc->argb = dcf == LV_COLOR_FORMAT_ARGB8888;
    uint32_t n = 1u << fc->bpp;
    for (uint32_t i = 0; i < n; i++) {
        lv_color32_t c = src->palette[i];
        if (fc->argb) {
            memcpy(&fc->lut32[i], &c, sizeof(uint32_t));
        } else {
            fc->lut16[i] = lv_color_to_u16(lv_color_make(c.red, c.green, c.blue));
        }
    }
    fc->ok = true;
}

// lv_draw_buf_clear() without its cache write-back: the ESP handler syncs whole buffer rows per
// call, and _compose() writes back the union of the cells once at the end anyway.
static void _clear_cell_area(lv_draw_buf_t *buf, const lv_area_t *a) {
    uint32_t bytes = (lv_area_get_width(a) * lv_color_format_get_bpp(buf->header.cf) + 7) >> 3;
    for (int32_t y = a->y1; y <= a->y2; y++) {
        lv_memzero(lv_draw_buf_goto_xy(buf, a->x1, y), bytes);
    }
}

// Compose every cell that changed into the buffer. Returns false when nothing was drawn so the
// caller can skip the invalidation. `dirty` comes back as the union of the composed cells, in
// container coordinates.
static bool _compose(displayio_tilegrid_t *self, lv_area_t *dirty) {
    if (!self->lv_full_dirty && self->lv_dirty_count == 0) {
        return false;
    }
    if (self->lv_buf == NULL || self->lv_canvas == NULL) {
        DISPLAYIO_TILEGRID_DEBUG("compose %p no buffer: buf %p canvas %p",
            self, self->lv_buf, self->lv_canvas);
        return false;
    }
    bool full = self->lv_full_dirty;
    bool any = false;
    lv_area_t buf_dirty = {0, 0, -1, -1};
    lv_area_t drawn_cell_area = {0, 0, -1, -1};

    // A cell whose tile is all palette entry 0 can be left untouched: the cleared buffer already
    // holds entry 0's color (black in RGB565, transparent in ARGB8888). Tile index 0 isn't
    // necessarily blank (it is for the terminal's space glyph, but it's an ordinary tile in any
    // other atlas), so check the tile's pixels instead of assuming it.
    bool skip_blank = self->lv_src == &self->lv_src_dsc && self->lv_src->palette != NULL &&
        LV_COLOR_FORMAT_IS_INDEXED(self->lv_src->header.cf) &&
        (self->lv_argb ? self->lv_src->palette[0].alpha == 0 :
            (self->lv_src->palette[0].red | self->lv_src->palette[0].green |
                self->lv_src->palette[0].blue) == 0) &&
        _tile_is_blank(self, 0);

    // A cell is a cropped window of the atlas, not an image allowed to grow beyond it, so the
    // layer's clip area is narrowed to the cell while its draw task is created; lv_draw_add_task()
    // saves the clip area into the task.
    lv_layer_t layer;
    lv_canvas_init_layer(self->lv_canvas, &layer);
    lv_area_t layer_clip = layer._clip_area;
    uint32_t queued = 0;

    uint32_t cells = _cell_count(self);
    uint32_t tiles_w = self->width_in_tiles;
    _fast_compose_t fc;
    _fast_compose_init(self, &fc);
    if (fc.ok) {
        // Write the fast cells directly, one pixel line across every changed cell of a tile
        // row at a time. The loop below then only does the bookkeeping for these cells.
        const lv_image_dsc_t *src = self->lv_src;
        for (uint32_t cy = 0; cy < self->height_in_tiles; cy++) {
            for (uint16_t y = 0; y < self->tile_height; y++) {
                uint8_t *line = lv_draw_buf_goto_xy(self->lv_buf, 0, cy * self->tile_height + y);
                for (uint32_t cx = 0; cx < tiles_w; cx++) {
                    uint32_t cell = cy * tiles_w + cx;
                    if (!full && !_cell_is_marked(self, cell)) {
                        continue;
                    }
                    uint16_t tile_index = _tile_index(self, cell);
                    if (tile_index == DISPLAYIO_TILEGRID_BLANK_TILE) {
                        continue;
                    }
                    uint16_t x0, y0;
                    _tile_origin(self, tile_index, &x0, &y0);
                    const uint8_t *row = src->data + (uint32_t)(y0 + y) * src->header.stride;
                    if (fc.argb) {
                        uint32_t *d = (uint32_t *)(void *)line + cx * self->tile_width;
                        for (uint16_t x = 0; x < self->tile_width; x++) {
                            d[x] = fc.lut32[_source_index(row, x0 + x, fc.bpp)];
                        }
                    } else {
                        uint16_t *d = (uint16_t *)(void *)line + cx * self->tile_width;
                        for (uint16_t x = 0; x < self->tile_width; x++) {
                            d[x] = fc.lut16[_source_index(row, x0 + x, fc.bpp)];
                        }
                    }
                }
            }
        }
    }
    for (uint32_t cell = 0; cell < cells; cell++) {
        if (!full && !_cell_is_marked(self, cell)) {
            continue;
        }
        uint32_t cx = cell % tiles_w;
        uint32_t cy = cell / tiles_w;
        lv_area_t cell_area = {
            .x1 = cx * self->tile_width,
            .y1 = cy * self->tile_height,
        };
        cell_area.x2 = cell_area.x1 + self->tile_width - 1;
        cell_area.y2 = cell_area.y1 + self->tile_height - 1;

        // Start from transparent (ARGB8888) or black (RGB565): lv_draw_image() writes nothing
        // where the source is transparent, which is exactly where the hole has to stay.
        // ESP_LOGI(TAG, "clearing cell area %d,%d-%d,%d", cell_area.x1, cell_area.y1, cell_area.x2, cell_area.y2);
        if (fc.ok && self->lv_src->data != NULL &&
            _tile_index(self, cell) != DISPLAYIO_TILEGRID_BLANK_TILE) {
            // pixels already written by the raster pass above
            drawn_cell_area = cell_area;
            goto cell_done;
        }
        _clear_cell_area(self->lv_buf, &cell_area);

        if (self->lv_src != NULL && self->lv_src->data != NULL) {
            uint16_t x0, y0;
            uint16_t tile_index = _tile_index(self, cell);
            bool blank = tile_index == DISPLAYIO_TILEGRID_BLANK_TILE;
            if (!(blank && skip_blank)) {
                _tile_origin(self, tile_index, &x0, &y0);
                if (!blank) {
                    DISPLAYIO_TILEGRID_DEBUG(
                        "compose %p cell %d (%d,%d) tile %d origin (%d,%d) full %d",
                        self, cell, cx, cy, tile_index, x0, y0, full);
                }
                drawn_cell_area = cell_area;
                lv_draw_image_dsc_t dsc;
                lv_draw_image_dsc_init(&dsc);
                dsc.src = self->lv_src;
                dsc.antialias = 0;
                layer._clip_area = cell_area;
                // The whole atlas is placed so that the tile's origin lands on the cell's corner;
                // the clip above limits the draw to the cell.
                int32_t dest_x = cell_area.x1 - (int32_t)x0;
                int32_t dest_y = cell_area.y1 - (int32_t)y0;
                lv_area_t coords = {
                    .x1 = dest_x,
                    .y1 = dest_y,
                    .x2 = dest_x + (int32_t)self->lv_src->header.w - 1,
                    .y2 = dest_y + (int32_t)self->lv_src->header.h - 1,
                };
                lv_draw_image(&layer, &dsc, &coords);
                layer._clip_area = layer_clip;
                queued++;
                if (queued == DISPLAYIO_LVGL_COMPOSE_BATCH) {
                    lv_canvas_finish_layer(self->lv_canvas, &layer);
                    // A big compose (a re-rendered text grid) can outlast the audio buffers.
                    if (displayio_lvgl_refresh_yield) {
                        RUN_BACKGROUND_TASKS;
                    }
                    lv_canvas_init_layer(self->lv_canvas, &layer);
                    queued = 0;
                }
            }
        }
    cell_done:
        _clear_cell(self, cell);
        lv_area_t cell_display;
        _cell_display_area(self, cx, cy, &cell_display);
        if (!any) {
            buf_dirty = cell_area;
            *dirty = cell_display;
            any = true;
        } else {
            _union_area(&buf_dirty, &cell_area);
            _union_area(dirty, &cell_display);
        }
    }
    self->lv_dirty_count = 0;
    self->lv_full_dirty = false;

    if (!any) {
        DISPLAYIO_TILEGRID_DEBUG("compose %p drew nothing: src %p srcdata %p buf %d x %d",
            self, self->lv_src, self->lv_src == NULL ? NULL : self->lv_src->data,
            (int)self->lv_buf->header.w, (int)self->lv_buf->header.h);
    }

    lv_canvas_finish_layer(self->lv_canvas, &layer);
    if (any) {
        // The compose pass may have run in software, so write those lines out before a DMA
        // consumer (the display flush, or the PPA on the display pass) reads the buffer.
        lv_draw_buf_flush_cache(self->lv_buf, &buf_dirty);
        if (drawn_cell_area.x2 < drawn_cell_area.x1) {
            DISPLAYIO_TILEGRID_DEBUG("compose %p done, nothing drawn", self);
            return any;
        }
        #if DISPLAYIO_TILEGRID_DEBUG_ENABLED
        // Did the draw actually put pixels in the buffer? Zero here means the compose draw wrote
        // nothing, so the problem is upstream of the views; non-zero means the buffer has content
        // and the display side is at fault. Scans the composed cell, so it only runs when the
        // logging that uses it is compiled in.
        uint32_t stride = self->lv_buf->header.stride;
        uint32_t bpp = lv_color_format_get_size(self->lv_buf->header.cf);
        const uint8_t *sample = self->lv_buf->data + drawn_cell_area.y1 * stride +
            drawn_cell_area.x1 * bpp;
        uint32_t nonzero = 0;
        for (int32_t y = 0; y <= drawn_cell_area.y2 - drawn_cell_area.y1; y++) {
            for (int32_t x = 0; x < (drawn_cell_area.x2 - drawn_cell_area.x1 + 1) * (int32_t)bpp; x++) {
                nonzero += sample[y * stride + x] != 0;
            }
        }
        DISPLAYIO_TILEGRID_DEBUG("compose %p done buf %p cell (%d,%d)-(%d,%d) nonzero %lu",
            self, self->lv_buf->data, (int)drawn_cell_area.x1, (int)drawn_cell_area.y1,
            (int)drawn_cell_area.x2, (int)drawn_cell_area.y2, (unsigned long)nonzero);
        #endif
    }
    return any;
}

// --- Refresh ---------------------------------------------------------------

static void _refresh(displayio_tilegrid_t *self) {
    if (!self->lv_work) {
        return;
    }
    if (self->lv_obj == NULL) {
        self->lv_work = false;
        DISPLAYIO_TILEGRID_DEBUG("refresh %p unattached, dropping work", self);
        return;
    }
    lv_display_t *display = lv_obj_get_display(self->lv_obj);
    if (display == NULL) {
        DISPLAYIO_TILEGRID_DEBUG("refresh %p no display, work stays pending", self);
        return;
    }
    DISPLAYIO_TILEGRID_DEBUG("refresh %p direct %d buf %p canvas %p full %d views %d dirty %lu",
        self, self->lv_direct, self->lv_buf, self->lv_canvas, self->lv_full_dirty,
        self->lv_views_dirty, (unsigned long)self->lv_dirty_count);
    DISPLAYIO_TILEGRID_DEBUG("refresh %p obj %dx%d at (%d,%d) scale %u hidden %d",
        self, (int)lv_obj_get_width(self->lv_obj), (int)lv_obj_get_height(self->lv_obj),
        (int)lv_obj_get_x(self->lv_obj), (int)lv_obj_get_y(self->lv_obj), (unsigned)self->lv_scale,
        (int)lv_obj_has_flag(self->lv_obj, LV_OBJ_FLAG_HIDDEN));
    bool full = self->lv_full_dirty || self->lv_views_dirty;
    bool changed = false;
    lv_area_t dirty = {0, 0, -1, -1};

    // Batch: the compose pass rewrites many objects and only the final invalidation matters.
    lv_display_enable_invalidation(display, false);
    if (self->lv_direct) {
        // The view is the source pixels, so a content change is a plain invalidation.
        changed = self->lv_src_dirty;
        // Direct sources can still need a cached conversion (RGB565_SWAPPED, for
        // example). Pixel writes must invalidate that decode just like composed sources.
        if (changed && self->lv_src != NULL) {
            lv_image_cache_drop(self->lv_src);
        }
        self->lv_src_dirty = false;
        if (self->lv_views_dirty) {
            _layout_views(self);
        }
    } else {
        if (!_ensure_buffer(self)) {
            // Out of memory (or a zero-sized grid): there is nothing to compose into. Keep the
            // pending work (lv_full_dirty/lv_dirty_count) so the next invalidation retries, but
            // don't invalidate here. Invalidating is what makes LVGL run a refresh, and the
            // refresh is what calls this function, so invalidating on failure spins the renderer
            // at 100% retrying the allocation (and flooding the log) forever.
            lv_display_enable_invalidation(display, true);
            self->lv_work = false;
            DISPLAYIO_TILEGRID_DEBUG("refresh %p no buffer, backing off", self);
            return;
        }
        _sync_source(self);
        if (self->lv_views_dirty) {
            _layout_views(self);
        }
        changed = _compose(self, &dirty);
    }
    lv_display_enable_invalidation(display, true);

    if (full || changed) {
        lv_area_t area;
        lv_obj_get_coords(self->lv_obj, &area);
        if (!full && dirty.x2 >= dirty.x1) {
            lv_area_move(&dirty, area.x1, area.y1);
            DISPLAYIO_TILEGRID_DEBUG("refresh %p invalidate area (%d,%d)-(%d,%d)",
                self, (int)dirty.x1, (int)dirty.y1, (int)dirty.x2, (int)dirty.y2);
            lv_obj_invalidate_area(self->lv_obj, &dirty);
        } else {
            DISPLAYIO_TILEGRID_DEBUG("refresh %p invalidate whole obj at (%d,%d)-(%d,%d)",
                self, (int)area.x1, (int)area.y1, (int)area.x2, (int)area.y2);
            lv_obj_invalidate(self->lv_obj);
        }
    } else {
        DISPLAYIO_TILEGRID_DEBUG("refresh %p nothing to invalidate", self);
    }
    self->lv_work = false;
}

void displayio_lvgl_tilegrid_refresh_all(void) {
    for (displayio_tilegrid_t *self = mirror_head; self != NULL; self = self->lv_next) {
        _refresh(self);
    }
}

// --- Bitmap and Palette plumbing ------------------------------------------

void displayio_lvgl_tilegrid_bitmap_changed(displayio_bitmap_t *bitmap, int16_t x1, int16_t y1,
    int16_t x2, int16_t y2) {
    if (bitmap->lv_watch_head == NULL) {
        return;
    }
    displayio_lvgl_lock();
    bool gone = bitmap->data == NULL;
    for (displayio_tilegrid_t *self = bitmap->lv_watch_head; self != NULL;
         self = self->lv_bitmap_next) {
        if (self->lv_obj == NULL) {
            continue;
        }
        if (gone) {
            // A cached decoder can borrow the Bitmap's pixels too. Drop it while the
            // storage still exists, then stop the source descriptor from exposing it.
            if (self->lv_src == &self->lv_src_dsc) {
                lv_image_cache_drop(self->lv_src);
                self->lv_src_dsc.data = NULL;
            }
            _mark_all(self);
            _mark_views(self);
            continue;
        }
        if (self->lv_src_dirty) {
            self->lv_src_x1 = LV_MIN(self->lv_src_x1, x1);
            self->lv_src_y1 = LV_MIN(self->lv_src_y1, y1);
            self->lv_src_x2 = LV_MAX(self->lv_src_x2, x2);
            self->lv_src_y2 = LV_MAX(self->lv_src_y2, y2);
        } else {
            self->lv_src_x1 = x1;
            self->lv_src_y1 = y1;
            self->lv_src_x2 = x2;
            self->lv_src_y2 = y2;
            self->lv_src_dirty = true;
        }
        _mark_work(self);
    }
    displayio_lvgl_unlock();
}

void displayio_lvgl_tilegrid_shader_changed(displayio_tilegrid_t *self) {
    if (self->lv_obj == NULL) {
        return;
    }
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    if (self->lv_src == &self->lv_src_dsc) {
        // The shared palette array was updated in place, so only the decoded view is stale.
        lv_image_cache_drop(self->lv_src);
    }
    if (!self->lv_direct) {
        // Gaining or losing transparency changes the composed buffer's format.
        lv_color_format_t cf = _source_cf(self->bitmap, self->pixel_shader);
        bool argb = _source_has_alpha(self, cf);
        if (argb != self->lv_argb) {
            self->lv_argb = argb;
            _mark_views(self);
        }
    }
    _mark_all(self);
    displayio_lvgl_unlock_with_cleanup();
}

// --- Lifecycle -------------------------------------------------------------

// Bind the source for the current bitmap and pixel shader and schedule a full compose. Caller
// holds the lock and has no source bound.
static bool _rebind(displayio_tilegrid_t *self) {
    self->lv_direct = _wants_direct(self);
    if (!_bind_source(self)) {
        return false;
    }
    // The supervisor resizes and positions its static terminal grids directly before
    // rebinding their bitmap. Keep the mirror geometry in sync too: resizing the canvas
    // or its views does not resize or move their container, and LVGL clips invalidation
    // to that container even when OVERFLOW_VISIBLE is set.
    lv_obj_set_size(self->lv_obj, self->pixel_width * self->lv_scale,
        self->pixel_height * self->lv_scale);
    _set_lv_pos(self);
    _mark_all(self);
    _mark_views(self);
    return true;
}

void displayio_lvgl_tilegrid_attach(displayio_tilegrid_t *self, lv_obj_t *parent) {
    if (!self->in_group || parent == NULL) {
        return;
    }
    // ESP_LOGI(TAG, "attaching tilegrid %p to parent %p", self, parent);
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    if (self->lv_obj == NULL) {
        if (_create_objects(self, parent)) {
            _rebind(self);
            _set_lv_pos(self);
            _link_mirror(self);
        } else {
            // Do not root a failed attachment forever. Discard a partially created
            // container too; a later group sync can retry attaching it.
            _destroy_objects(self);
        }
    } else if (lv_obj_get_parent(self->lv_obj) != parent) {
        lv_obj_set_parent(self->lv_obj, parent);
    }
    displayio_lvgl_unlock_with_cleanup();
}

void displayio_lvgl_tilegrid_detach(displayio_tilegrid_t *self) {
    displayio_lvgl_lock();
    _unbind_source(self);
    _destroy_objects(self);
    _unlink_mirror(self);
    // This allocation is outside the Python heap, so collecting the TileGrid cannot
    // free it. Drop it even when attachment failed before creating an LVGL object.
    lv_free(self->lv_cell_dirty);
    self->lv_cell_dirty = NULL;
    self->lv_cell_words = 0;
    self->lv_dirty_count = 0;
    self->lv_full_dirty = true;
    self->lv_views_dirty = true;
    self->lv_src_dirty = false;
    self->lv_work = false;
    displayio_lvgl_unlock();
}

// Apply the inherited scale chain to the grid. TileGrids have no scale of their own; they render
// at the scale of the parent group chain.
void displayio_lvgl_tilegrid_update_scale(displayio_tilegrid_t *self) {
    if (self->lv_obj == NULL) {
        return;
    }
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    if (mp_obj_is_type(self->bitmap, &displayio_ondiskbitmap_type) && self->lv_direct &&
        (self->lv_scale != 1) != self->lv_src_odb_full) {
        // Re-source: whether the file's rows can be streamed changed with the scale.
        _unbind_source(self);
        _bind_source(self);
    }
    if (!self->lv_direct) {
        lv_obj_set_size(self->lv_obj, self->pixel_width * self->lv_scale,
            self->pixel_height * self->lv_scale);
    }
    for (uint8_t i = 0; i < DISPLAYIO_LVGL_TILEGRID_VIEWS; i++) {
        if (self->lv_views[i] != NULL) {
            _set_view_scale(self->lv_views[i], self->lv_scale);
        }
    }
    _set_lv_pos(self);
    _mark_views(self);
    displayio_lvgl_unlock_with_cleanup();
}

// protocol_lvgl implementation. Group owns the tree; these only translate its calls into the
// TileGrid mirror operations above.
//
// A Python subclass of TileGrid (a Label, a RoundRect, ...) is an mp_obj_instance_t that holds
// the native TileGrid in subobj[0], so every function below has to see that native object
// instead of the instance. Resolving it here keeps the type slot on the native type.
static mp_obj_t _tilegrid_protocol_self(mp_obj_t layer) {
    mp_obj_t native = mp_obj_cast_to_native_base(layer, MP_OBJ_FROM_PTR(&displayio_tilegrid_type));
    if (native == MP_OBJ_NULL) {
        return layer;
    }
    mp_obj_assert_native_inited(native);
    return native;
}

static lv_obj_t *_tilegrid_protocol_root_obj(mp_obj_t layer) {
    return ((displayio_tilegrid_t *)MP_OBJ_TO_PTR(layer))->lv_obj;
}

static void _tilegrid_protocol_attach(mp_obj_t layer, lv_obj_t *parent) {
    displayio_lvgl_tilegrid_attach(MP_OBJ_TO_PTR(layer), parent);
}

static void _tilegrid_protocol_detach(mp_obj_t layer) {
    displayio_lvgl_tilegrid_detach(MP_OBJ_TO_PTR(layer));
}

static bool _tilegrid_protocol_set_transform(mp_obj_t layer, int32_t abs_x, int32_t abs_y,
    uint16_t scale) {
    displayio_tilegrid_t *self = MP_OBJ_TO_PTR(layer);
    // A TileGrid has no position or scale of its own: it places itself from the parent
    // origin plus its own x/y, and renders at the scale of the parent chain.
    self->lv_scale = scale;
    self->lv_origin_x = abs_x;
    self->lv_origin_y = abs_y;
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _set_lv_pos(self);
        displayio_lvgl_unlock();
        displayio_lvgl_tilegrid_update_scale(self);
    }
    return true;
}

static void _tilegrid_protocol_set_hidden_by_parent(mp_obj_t layer, bool hidden) {
    displayio_tilegrid_set_hidden_by_parent(MP_OBJ_TO_PTR(layer), hidden);
}

static bool _tilegrid_protocol_set_in_group(mp_obj_t layer, bool in_group) {
    displayio_tilegrid_t *self = MP_OBJ_TO_PTR(layer);
    bool was_in_group = self->in_group;
    self->in_group = in_group;
    return was_in_group;
}

const lvgl_protocol_t displayio_lvgl_tilegrid_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_lvgl)
    .protocol_self = _tilegrid_protocol_self,
    .root_obj = _tilegrid_protocol_root_obj,
    .attach = _tilegrid_protocol_attach,
    .detach = _tilegrid_protocol_detach,
    .set_transform = _tilegrid_protocol_set_transform,
    .set_hidden_by_parent = _tilegrid_protocol_set_hidden_by_parent,
    .set_in_group = _tilegrid_protocol_set_in_group,
};

// --- displayio API ---------------------------------------------------------

void common_hal_displayio_tilegrid_construct(displayio_tilegrid_t *self, mp_obj_t bitmap,
    uint16_t bitmap_width_in_tiles, uint16_t bitmap_height_in_tiles,
    mp_obj_t pixel_shader, uint16_t width, uint16_t height,
    uint16_t tile_width, uint16_t tile_height, uint16_t x, uint16_t y, uint16_t default_tile) {
    // Raise before anything is allocated if LVGL can't draw this combination.
    _source_cf(bitmap, pixel_shader);

    uint32_t total_tiles = width * height;
    self->bitmap_width_in_tiles = bitmap_width_in_tiles;
    self->tiles_in_bitmap = bitmap_width_in_tiles * bitmap_height_in_tiles;

    // Determine if we need uint16_t or uint8_t for tile indices
    bool use_uint16 = self->tiles_in_bitmap > 255;

    // ESP_LOGI(TAG, "allocating tile grid %d x %d", width, height);
    if (use_uint16) {
        uint16_t *tiles16 = (uint16_t *)m_malloc_without_collect(total_tiles * sizeof(uint16_t));
        for (uint32_t i = 0; i < total_tiles; i++) {
            tiles16[i] = default_tile;
        }
        self->tiles = tiles16;
    } else {
        uint8_t *tiles8 = (uint8_t *)m_malloc_without_collect(total_tiles);
        for (uint32_t i = 0; i < total_tiles; i++) {
            tiles8[i] = (uint8_t)default_tile;
        }
        self->tiles = tiles8;
    }

    self->width_in_tiles = width;
    self->height_in_tiles = height;
    self->x = x;
    self->y = y;
    self->pixel_width = width * tile_width;
    self->pixel_height = height * tile_height;
    self->tile_width = tile_width;
    self->tile_height = tile_height;
    self->bitmap = bitmap;
    self->pixel_shader = pixel_shader;
    self->in_group = false;
    self->hidden = false;
    self->hidden_by_parent = false;
    self->flip_x = false;
    self->flip_y = false;
    self->transpose_xy = false;
    self->top_left_x = 0;
    self->top_left_y = 0;
    self->lv_obj = NULL;
    self->lv_canvas = NULL;
    self->lv_buf = NULL;
    self->lv_src = NULL;
    self->lv_cell_dirty = NULL;
    self->lv_cell_words = 0;
    self->lv_dirty_count = 0;
    self->lv_full_dirty = true;
    self->lv_work = false;
    self->lv_views_dirty = true;
    self->lv_src_dirty = false;
    self->lv_direct = false;
    self->lv_src_odb_full = false;
    self->lv_argb = false;
    self->lv_view_count = 0;
    self->lv_next = NULL;
    self->lv_bitmap_next = NULL;
    self->lv_scale = 1;
    self->lv_origin_x = 0;
    self->lv_origin_y = 0;
    for (uint8_t i = 0; i < DISPLAYIO_LVGL_TILEGRID_VIEWS; i++) {
        self->lv_views[i] = NULL;
    }
    lv_memzero(&self->lv_src_dsc, sizeof(self->lv_src_dsc));
    lv_memzero(self->lv_view_dsc, sizeof(self->lv_view_dsc));
}

bool common_hal_displayio_tilegrid_get_hidden(displayio_tilegrid_t *self) {
    return self->hidden;
}

bool displayio_tilegrid_get_rendered_hidden(displayio_tilegrid_t *self) {
    (void)self;
    return false;
}

void displayio_tilegrid_set_hidden_by_parent(displayio_tilegrid_t *self, bool hidden) {
    self->hidden_by_parent = hidden;
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        if (self->hidden || self->hidden_by_parent) {
            lv_obj_add_flag(self->lv_obj, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(self->lv_obj, LV_OBJ_FLAG_HIDDEN);
        }
        displayio_lvgl_unlock();
    }
}

void common_hal_displayio_tilegrid_set_hidden(displayio_tilegrid_t *self, bool hidden) {
    self->hidden = hidden;
    displayio_tilegrid_set_hidden_by_parent(self, self->hidden_by_parent);
}

// Mark the cell of the tile matrix that shows tile (x, y) of the grid for recomposing.
void displayio_tilegrid_mark_tile_dirty(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    if (self->lv_obj == NULL || self->width_in_tiles == 0 || self->height_in_tiles == 0) {
        // Not mirrored yet; binding composes everything.
        return;
    }
    displayio_lvgl_lock();
    uint32_t cell = (uint32_t)(y % self->height_in_tiles) * self->width_in_tiles +
        (x % self->width_in_tiles);
    _mark_cell(self, cell);
    displayio_lvgl_unlock();
}

mp_int_t common_hal_displayio_tilegrid_get_x(displayio_tilegrid_t *self) {
    return self->x;
}

void common_hal_displayio_tilegrid_set_x(displayio_tilegrid_t *self, mp_int_t x) {
    if (self->x == x) {
        return;
    }
    self->x = x;
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _set_lv_pos(self);
        displayio_lvgl_unlock();
    }
}

mp_int_t common_hal_displayio_tilegrid_get_y(displayio_tilegrid_t *self) {
    return self->y;
}

void common_hal_displayio_tilegrid_set_y(displayio_tilegrid_t *self, mp_int_t y) {
    if (self->y == y) {
        return;
    }
    self->y = y;
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _set_lv_pos(self);
        displayio_lvgl_unlock();
    }
}

mp_obj_t common_hal_displayio_tilegrid_get_pixel_shader(displayio_tilegrid_t *self) {
    return self->pixel_shader;
}

void common_hal_displayio_tilegrid_set_pixel_shader(displayio_tilegrid_t *self, mp_obj_t pixel_shader) {
    _source_cf(self->bitmap, pixel_shader);
    if (self->lv_obj == NULL) {
        self->pixel_shader = pixel_shader;
        return;
    }
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    // Give up the old shader's source while the descriptor still belongs to it.
    _unbind_source(self);
    self->pixel_shader = pixel_shader;
    _rebind(self);
    displayio_lvgl_unlock_with_cleanup();
}

mp_obj_t common_hal_displayio_tilegrid_get_bitmap(displayio_tilegrid_t *self) {
    return self->bitmap;
}

void common_hal_displayio_tilegrid_set_bitmap(displayio_tilegrid_t *self, mp_obj_t bitmap) {
    _source_cf(bitmap, self->pixel_shader);
    if (self->lv_obj == NULL) {
        self->bitmap = bitmap;
        return;
    }
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    bool linked = _is_linked(self);
    // Release the old bitmap's source before the reference to it is replaced.
    _unbind_source(self);
    if (linked) {
        _unwatch_bitmap(self);
    }
    self->bitmap = bitmap;
    _rebind(self);
    if (linked) {
        _watch_bitmap(self);
    }
    displayio_lvgl_unlock_with_cleanup();
}

uint16_t common_hal_displayio_tilegrid_get_width(displayio_tilegrid_t *self) {
    return self->width_in_tiles;
}

uint16_t common_hal_displayio_tilegrid_get_height(displayio_tilegrid_t *self) {
    return self->height_in_tiles;
}

uint16_t common_hal_displayio_tilegrid_get_tile_width(displayio_tilegrid_t *self) {
    return self->tile_width;
}

uint16_t common_hal_displayio_tilegrid_get_tile_height(displayio_tilegrid_t *self) {
    return self->tile_height;
}

uint16_t common_hal_displayio_tilegrid_get_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    if (self->tiles == NULL) {
        return 0;
    }

    uint32_t index = y * self->width_in_tiles + x;
    if (self->tiles_in_bitmap > 255) {
        return ((uint16_t *)self->tiles)[index];
    }
    return ((uint8_t *)self->tiles)[index];
}

void common_hal_displayio_tilegrid_set_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y, uint16_t tile_index) {
    if (tile_index >= self->tiles_in_bitmap) {
        mp_raise_ValueError(MP_ERROR_TEXT("Tile index out of bounds"));
    }

    if (self->tiles == NULL) {
        return;
    }

    uint32_t index = y * self->width_in_tiles + x;
    uint16_t old_tile_index;
    if (self->tiles_in_bitmap > 255) {
        old_tile_index = ((uint16_t *)self->tiles)[index];
        ((uint16_t *)self->tiles)[index] = tile_index;
    } else {
        old_tile_index = ((uint8_t *)self->tiles)[index];
        ((uint8_t *)self->tiles)[index] = (uint8_t)tile_index;
    }
    if (old_tile_index != tile_index && tile_index != DISPLAYIO_TILEGRID_BLANK_TILE) {
        DISPLAYIO_TILEGRID_DEBUG(
            "set_tile %p (%d,%d) tile %d -> %d cell %d mirrored %d",
            self, x, y, old_tile_index, tile_index, index,
            self->lv_obj != NULL && self->width_in_tiles != 0 && self->height_in_tiles != 0);
    }
    displayio_tilegrid_mark_tile_dirty(self, x, y);
}

void common_hal_displayio_tilegrid_set_all_tiles(displayio_tilegrid_t *self, uint16_t tile_index) {
    if (tile_index >= self->tiles_in_bitmap) {
        mp_raise_ValueError(MP_ERROR_TEXT("Tile index out of bounds"));
    }

    if (self->tiles == NULL) {
        return;
    }

    uint32_t changed = 0;
    if (self->tiles_in_bitmap > 255) {
        uint16_t *tiles16 = (uint16_t *)self->tiles;
        for (uint16_t y = 0; y < self->height_in_tiles; y++) {
            for (uint16_t x = 0; x < self->width_in_tiles; x++) {
                uint32_t i = y * self->width_in_tiles + x;
                if (tiles16[i] != tile_index) {
                    changed++;
                }
                tiles16[i] = tile_index;
            }
        }
    } else {
        uint8_t *tiles8 = (uint8_t *)self->tiles;
        for (uint16_t y = 0; y < self->height_in_tiles; y++) {
            for (uint16_t x = 0; x < self->width_in_tiles; x++) {
                uint32_t i = y * self->width_in_tiles + x;
                if (tiles8[i] != (uint8_t)tile_index) {
                    changed++;
                }
                tiles8[i] = (uint8_t)tile_index;
            }
        }
    }
    if (changed > 0) {
        DISPLAYIO_TILEGRID_DEBUG(
            "set_all_tiles %p tile %d changed %d of %d cells mirrored %d",
            self, tile_index, changed, self->width_in_tiles * self->height_in_tiles,
            self->lv_obj != NULL);
    }

    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _mark_all(self);
        displayio_lvgl_unlock();
    }
}

bool common_hal_displayio_tilegrid_get_flip_x(displayio_tilegrid_t *self) {
    return self->flip_x;
}

void common_hal_displayio_tilegrid_set_flip_x(displayio_tilegrid_t *self, bool flip_x) {
    if (self->flip_x == flip_x) {
        return;
    }
    if (flip_x) {
        _raise_unsupported();
    }
    self->flip_x = flip_x;
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _mark_all(self);
        displayio_lvgl_unlock();
    }
}

bool common_hal_displayio_tilegrid_get_flip_y(displayio_tilegrid_t *self) {
    return self->flip_y;
}

void common_hal_displayio_tilegrid_set_flip_y(displayio_tilegrid_t *self, bool flip_y) {
    if (self->flip_y == flip_y) {
        return;
    }
    if (flip_y) {
        _raise_unsupported();
    }
    self->flip_y = flip_y;
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _mark_all(self);
        displayio_lvgl_unlock();
    }
}

bool common_hal_displayio_tilegrid_get_transpose_xy(displayio_tilegrid_t *self) {
    return self->transpose_xy;
}

void common_hal_displayio_tilegrid_set_transpose_xy(displayio_tilegrid_t *self, bool transpose_xy) {
    if (self->transpose_xy == transpose_xy) {
        return;
    }
    if (transpose_xy) {
        _raise_unsupported();
    }
    self->transpose_xy = transpose_xy;
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _mark_all(self);
        displayio_lvgl_unlock();
    }
}

bool common_hal_displayio_tilegrid_contains(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    uint16_t right_edge = self->x + (self->width_in_tiles * self->tile_width);
    uint16_t bottom_edge = self->y + (self->height_in_tiles * self->tile_height);
    return x >= self->x && x < right_edge &&
           y >= self->y && y < bottom_edge;
}

// Scrolling moves the window over the composed buffer, not the pixels: no cell is re-sourced and
// nothing is copied, the views just point somewhere else.
void common_hal_displayio_tilegrid_set_top_left(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    // Only the wrapped position matters; the buffer doesn't know the window moved.
    uint16_t old_x = self->width_in_tiles == 0 ? 0 : self->top_left_x % self->width_in_tiles;
    uint16_t old_y = self->height_in_tiles == 0 ? 0 : self->top_left_y % self->height_in_tiles;
    uint16_t new_x = self->width_in_tiles == 0 ? 0 : x % self->width_in_tiles;
    uint16_t new_y = self->height_in_tiles == 0 ? 0 : y % self->height_in_tiles;
    self->top_left_x = x;
    self->top_left_y = y;
    if (old_x == new_x && old_y == new_y) {
        // Same window: nothing moves.
        return;
    }
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _mark_views(self);
        displayio_lvgl_unlock();
    }
}

#endif  // CIRCUITPY_DISPLAYIO_LVGL
