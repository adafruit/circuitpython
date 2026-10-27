// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "lvgl.h"

typedef struct {
    mp_obj_base_t base;
    uint16_t width;
    uint16_t height;
    uint32_t data_offset;
    uint16_t stride;
    uint32_t r_bitmask;
    uint32_t g_bitmask;
    uint32_t b_bitmask;
    mp_obj_t file;
    union {
        mp_obj_base_t *pixel_shader_base;
        struct displayio_palette *palette;
        struct displayio_colorconverter *colorconverter;
    };
    bool bitfield_compressed;
    uint8_t bits_per_pixel;
    // The last bytes read from the file. Pixels are read along rows, so most reads hit it.
    uint8_t cache_len;
    #if CIRCUITPY_STORAGE_MAP_FILE
    // Flash runs the pixels are read from instead of the cache; 0 = read through the cache.
    uint8_t map_count;
    // 8-bit file with every row inside one run, so TileGrid can read whole rows.
    bool rows_mapped;
    #endif
    uint32_t cache_start;
    union {
        uint8_t cache[64];
        #if CIRCUITPY_STORAGE_MAP_FILE
        struct {
            const uint8_t *addr;
            uint32_t len;
        } map[8];
        #endif
    };
    // A negative BMP height means the rows are stored top to bottom.
    bool top_down;
    // LVGL backend state. The pixels are exposed through a custom image decoder (see
    // shared-module/displayio/lvgl/OnDiskBitmap.c) so the file can be drawn without
    // loading it into RAM when it lives in mapped flash.
    const uint8_t *lv_mapped_all;
    lv_draw_buf_t *lv_decoded;
    lv_color32_t *lv_palette;
    uint32_t lv_palette_generation;
    uint32_t lv_decoded_generation;
    // One image per decode mode, shared by every TileGrid drawing the file that way: [0]
    // streams rows, [1] holds the whole decode (unmapped files and scaled TileGrids).
    lv_image_dsc_t lv_dsc[2];
    struct displayio_lvgl_image_root *lv_roots[2];
    // The decoder's scratch (row buffer and draw buffer wrappers), owned here so that opening
    // an image per draw doesn't allocate. See odb_decoder_open().
    struct odb_decoder *lv_decoder;
    uint16_t lv_users;
    uint16_t lv_full_users;
} displayio_ondiskbitmap_t;

#if CIRCUITPY_STORAGE_MAP_FILE
// Row y of an 8-bit file in flash, or NULL if it spans two runs or the file isn't mapped.
const uint8_t *displayio_ondiskbitmap_row(displayio_ondiskbitmap_t *self, int16_t y);
#endif

// Image decoder used by OnDiskBitmap. The descriptors it hands out carry this address in
// reserved and point data at the OnDiskBitmap.
extern const char displayio_lvgl_ondiskbitmap_tag;
void displayio_lvgl_ondiskbitmap_decoder_init(void);
lv_color_format_t displayio_ondiskbitmap_lv_color_format(displayio_ondiskbitmap_t *self);
// Attach/detach a TileGrid's root object as a user of the file's LVGL image. full requests a
// whole image (the file isn't in mapped flash, or the TileGrid inherits a scale). The
// returned descriptor stays valid until the matching release. Caller holds the LVGL lock.
const lv_image_dsc_t *displayio_ondiskbitmap_lv_acquire(displayio_ondiskbitmap_t *self, bool full,
    lv_obj_t *root);
void displayio_ondiskbitmap_lv_release(displayio_ondiskbitmap_t *self, bool full, lv_obj_t *root);
// The file's Palette changed: rebuild the decoder's LVGL palette and the whole decode, and
// invalidate every image drawn from the file.
void displayio_ondiskbitmap_lv_palette_changed(displayio_ondiskbitmap_t *self);
