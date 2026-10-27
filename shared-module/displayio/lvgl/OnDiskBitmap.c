// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2025 SamantazFox
//
// SPDX-License-Identifier: MIT

// LVGL-based displayio OnDiskBitmap implementation.
//
// The BMP is parsed here, but the pixels stay in the file. A custom LVGL image decoder
// (registered by displayio_lvgl_init()) serves them to LVGL:
//
// - When the file lives in mapped flash (CIRCUITPY_STORAGE_MAP_FILE), each request is a
//   one-row draw buffer that points straight at the flash bytes. No copy, no seek.
// - Otherwise, the whole image is decoded into an lv_draw_buf when a TileGrid attaches
//   (on the CircuitPython thread, so the render thread never touches VFS). That is the
//   memory cost OnDiskBitmap normally avoids, but it is the only thread-safe option for
//   files on SD cards.
// - A TileGrid that inherits a scale needs a whole image too: LVGL can't transform a
//   partially decoded image.
//
// The file owns one lv_image_dsc_t per decode mode (streaming rows and whole image), whose
// `data` points at this object and whose `reserved` carries the decoder tag. TileGrids share
// them, so a Palette change reaches every image drawn from the file without going through a
// TileGrid. For indexed files the Palette is converted to lv_color32_t entries here, and the
// decoder expands rows through them.

#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-module/displayio/ColorConverter.h"
#include "shared-module/displayio/Palette.h"
#include "shared-module/displayio/lvgl/lvgl_internal.h"
// Private image-decoder descriptor (lives in different directories in LVGL trees).
#include "lvgl_private.h"

#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#if CIRCUITPY_STORAGE_MAP_FILE
#include "shared-bindings/storage/__init__.h"
#endif


#define DISPLAYIO_ODBMP_DEBUG(...) (void)0
// #define DISPLAYIO_ODBMP_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)

// Tags an lv_image_dsc_t as describing a displayio OnDiskBitmap. Shared with TileGrid.c.
const char displayio_lvgl_ondiskbitmap_tag = 0;

typedef struct odb_decoder {
    displayio_ondiskbitmap_t *odb;
    // One-row scratch used when a mapped row is split across flash runs or needs conversion.
    uint8_t *row_data;
    uint32_t row_stride;
    lv_draw_buf_t row;
    lv_draw_buf_t wrap;
} odb_decoder_t;

// The decoder opens once per draw of every image, so its scratch lives on the OnDiskBitmap
// instead of being allocated and freed each time. Opens never nest: LVGL draws one task at a
// time on its own thread, and `open`/`close` are the only places that touch it.
static void odb_decoder_free(displayio_ondiskbitmap_t *odb) {
    odb_decoder_t *s = (odb_decoder_t *)odb->lv_decoder;
    if (s == NULL) {
        return;
    }
    if (s->row_data != NULL) {
        lv_free(s->row_data);
    }
    lv_free(s);
    odb->lv_decoder = NULL;
}

static uint32_t read_word(uint16_t *bmp_header, uint16_t index) {
    return bmp_header[index] | bmp_header[index + 1] << 16;
}

// Bytes of pixel data in one row, without the BMP row padding.
static uint32_t row_bytes(displayio_ondiskbitmap_t *self) {
    return ((uint32_t)self->width * self->bits_per_pixel + 7) / 8;
}

// True when file rows have to be converted instead of passed through to LVGL (indexed
// pixels expand through the Palette, 555/bitfield 16bpp becomes XRGB8888).
static bool row_converts(displayio_ondiskbitmap_t *self) {
    return self->bits_per_pixel <= 8 ||
           (self->bits_per_pixel == 16 && self->g_bitmask != 0x07e0);
}

lv_color_format_t displayio_ondiskbitmap_lv_color_format(displayio_ondiskbitmap_t *self) {
    switch (self->bits_per_pixel) {
        case 1:
        case 2:
        case 4:
        case 8:
            // The software renderer only blends I1 natively, so indexed rows are
            // expanded through the Palette to ARGB8888 (transparent entries keep alpha 0).
            return LV_COLOR_FORMAT_ARGB8888;
        case 16:
            // 565 is LVGL's native little-endian RGB565; 555 and bitfields are converted.
            return row_converts(self) ? LV_COLOR_FORMAT_XRGB8888 : LV_COLOR_FORMAT_RGB565;
        case 24:
            // BMP stores B, G, R, which is LVGL's RGB888 byte order.
            return LV_COLOR_FORMAT_RGB888;
        case 32:
            // BMP stores B, G, R, X; LVGL's XRGB8888 ignores the top byte like the legacy
            // compositor does.
            return LV_COLOR_FORMAT_XRGB8888;
        default:
            return LV_COLOR_FORMAT_UNKNOWN;
    }
}

static void ensure_full_image(displayio_ondiskbitmap_t *self);

// Rebuild the decoder's LVGL copy of the Palette after it changed.
static void refresh_palette(displayio_ondiskbitmap_t *self) {
    if (self->lv_palette == NULL || self->palette == NULL) {
        return;
    }
    displayio_lvgl_palette_fill(self->palette, self->lv_palette,
        1u << self->bits_per_pixel);
    self->lv_palette_generation++;
    // The decoded image has the old palette baked in; redo it on the CP thread.
    if (self->lv_decoded != NULL && self->lv_decoded_generation != self->lv_palette_generation) {
        lv_draw_buf_destroy(self->lv_decoded);
        self->lv_decoded = NULL;
        ensure_full_image(self);
    }
}

#if CIRCUITPY_STORAGE_MAP_FILE
static void add_run(void *self_in, const uint8_t *addr, size_t len) {
    displayio_ondiskbitmap_t *self = self_in;
    if (self->map_count < MP_ARRAY_SIZE(self->map)) {
        self->map[self->map_count].addr = addr;
        self->map[self->map_count].len = len;
        self->map_count++;
    } else {
        self->map_count = MP_ARRAY_SIZE(self->map) + 1;  // too fragmented: decode into RAM instead
    }
}

// Flash address of len bytes at offset if they lie in one run, else NULL.
static const uint8_t *map_at(displayio_ondiskbitmap_t *self, uint32_t offset, uint32_t len) {
    for (uint8_t i = 0; i < self->map_count && i < MP_ARRAY_SIZE(self->map); i++) {
        if (offset < self->map[i].len) {
            return offset + len <= self->map[i].len ? self->map[i].addr + offset : NULL;
        }
        offset -= self->map[i].len;
    }
    return NULL;
}

// Copy len bytes at offset out of the mapped runs. Works across run boundaries by going
// byte by byte; returns false if the file isn't mapped at that offset.
static bool copy_mapped(displayio_ondiskbitmap_t *self, uint32_t offset, uint32_t len, uint8_t *dst) {
    const uint8_t *p = map_at(self, offset, len);
    if (p != NULL) {
        memcpy(dst, p, len);
        return true;
    }
    for (uint32_t i = 0; i < len; i++) {
        p = map_at(self, offset + i, 1);
        if (p == NULL) {
            return false;
        }
        dst[i] = *p;
    }
    return true;
}

// Row y of an 8-bit file in flash, or NULL if it spans two runs or the file isn't mapped.
const uint8_t *displayio_ondiskbitmap_row(displayio_ondiskbitmap_t *self, int16_t y) {
    uint32_t file_y = self->top_down ? y : self->height - y - 1;
    return map_at(self, self->data_offset + file_y * self->stride, self->width);
}
#endif

static bool is_mapped(displayio_ondiskbitmap_t *self) {
    #if CIRCUITPY_STORAGE_MAP_FILE
    return self->map_count > 0 && self->map_count <= MP_ARRAY_SIZE(self->map);
    #else
    (void)self;
    return false;
    #endif
}

// Read one whole row of pixel data into dst. Runs on the CircuitPython thread.
static bool read_row(displayio_ondiskbitmap_t *self, uint32_t file_y, uint8_t *dst, uint32_t len) {
    uint32_t offset = self->data_offset + file_y * self->stride;
    #if CIRCUITPY_STORAGE_MAP_FILE
    if (is_mapped(self)) {
        return copy_mapped(self, offset, len, dst);
    }
    #endif
    int errcode = 0;
    mp_stream_seek(self->file, offset, MP_SEEK_SET, &errcode);
    if (errcode != 0) {
        return false;
    }
    mp_uint_t n = mp_stream_rw(self->file, dst, len, &errcode, MP_STREAM_RW_ONCE);
    return errcode == 0 && n == len;
}

// Pull one BGR channel out of a 16bpp pixel with arbitrary bitfields.
static uint8_t extract_channel(uint32_t pixel, uint32_t mask) {
    if (mask == 0) {
        return 0;
    }
    uint32_t shift = 0;
    while (((mask >> shift) & 1u) == 0) {
        shift++;
    }
    uint32_t max = mask >> shift;
    return (uint8_t)((((pixel & mask) >> shift) * 255u + max / 2) / max);
}

// Convert a 16bpp row (BGR in little-endian) to XRGB8888.
static void convert_row_16(displayio_ondiskbitmap_t *self, const uint8_t *src, uint8_t *dst,
    uint32_t width) {
    for (uint32_t x = 0; x < width; x++) {
        uint16_t pixel = src[0] | (src[1] << 8);
        dst[0] = extract_channel(pixel, self->b_bitmask);
        dst[1] = extract_channel(pixel, self->g_bitmask);
        dst[2] = extract_channel(pixel, self->r_bitmask);
        dst[3] = 0xff;
        src += 2;
        dst += 4;
    }
}

// Expand one indexed row (MSB first, like BMP) through the Palette to ARGB8888.
static void convert_row_indexed(displayio_ondiskbitmap_t *self, const uint8_t *src, uint8_t *dst,
    uint32_t width) {
    uint32_t mask = (1u << self->bits_per_pixel) - 1u;
    for (uint32_t x = 0; x < width; x++) {
        uint32_t bit = x * self->bits_per_pixel;
        uint8_t shift = 8 - self->bits_per_pixel - (bit & 7);
        uint32_t index = (src[bit >> 3] >> shift) & mask;
        lv_color32_t color = self->lv_palette[index];
        dst[0] = color.blue;
        dst[1] = color.green;
        dst[2] = color.red;
        dst[3] = color.alpha;
        dst += 4;
    }
}

// Decode the whole image into RAM. Called on the CircuitPython thread while a TileGrid
// attaches (or starts scaling), never from the render thread.
static lv_draw_buf_t *decode_all(displayio_ondiskbitmap_t *self) {
    lv_color_format_t cf = displayio_ondiskbitmap_lv_color_format(self);
    if (cf == LV_COLOR_FORMAT_UNKNOWN) {
        return NULL;
    }
    lv_draw_buf_t *buf = lv_draw_buf_create(self->width, self->height, cf, LV_STRIDE_AUTO);
    if (buf == NULL) {
        return NULL;
    }
    uint32_t src_len = row_bytes(self);
    uint8_t *src = lv_malloc(src_len);
    if (src == NULL) {
        lv_draw_buf_destroy(buf);
        return NULL;
    }
    bool converts = row_converts(self);
    uint32_t dst_stride = buf->header.stride;
    for (uint32_t y = 0; y < self->height; y++) {
        uint32_t file_y = self->top_down ? y : self->height - 1 - y;
        if (!read_row(self, file_y, src, src_len)) {
            lv_free(src);
            lv_draw_buf_destroy(buf);
            return NULL;
        }
        uint8_t *dst = (uint8_t *)buf->data + y * dst_stride;
        if (self->bits_per_pixel <= 8) {
            convert_row_indexed(self, src, dst, self->width);
        } else if (converts) {
            convert_row_16(self, src, dst, self->width);
        } else {
            memcpy(dst, src, src_len);
        }
    }
    lv_free(src);
    return buf;
}

// Does any attached TileGrid need the whole image in RAM / as one draw buffer?
static bool uses_full_image(displayio_ondiskbitmap_t *self) {
    return self->lv_full_users > 0 || !is_mapped(self);
}

static void ensure_full_image(displayio_ondiskbitmap_t *self) {
    if (self->lv_decoded != NULL) {
        return;
    }
    // A top-down, contiguous mapped file of a direct color format can be drawn from
    // flash directly (see decoder open); indexed rows need the palette expansion.
    if (!row_converts(self) && self->lv_mapped_all != NULL && self->top_down &&
        self->height > 0) {
        return;
    }
    self->lv_decoded = decode_all(self);
    self->lv_decoded_generation = self->lv_palette_generation;
}

// Build the descriptor for one decode mode. LVGL's decoder recognizes an OnDiskBitmap by
// these two fields: data points at the file object and reserved carries the tag.
static void init_dsc(displayio_ondiskbitmap_t *self, lv_image_dsc_t *dsc) {
    lv_image_dsc_t *d = dsc;
    lv_memzero(d, sizeof(*d));
    lv_color_format_t cf = displayio_ondiskbitmap_lv_color_format(self);
    d->header.magic = LV_IMAGE_HEADER_MAGIC;
    d->header.cf = cf;
    d->header.w = self->width;
    d->header.h = self->height;
    d->header.stride = lv_draw_buf_width_to_stride(self->width, cf);
    d->data = (const uint8_t *)self;
    d->reserved = &displayio_lvgl_ondiskbitmap_tag;
}

const lv_image_dsc_t *displayio_ondiskbitmap_lv_acquire(displayio_ondiskbitmap_t *self, bool full,
    lv_obj_t *root) {
    uint8_t mode = full ? 1 : 0;
    self->lv_users++;
    if (full) {
        self->lv_full_users++;
    }
    if (uses_full_image(self)) {
        ensure_full_image(self);
    }
    init_dsc(self, &self->lv_dsc[mode]);
    displayio_lvgl_image_root_add((displayio_lvgl_image_root_t **)&self->lv_roots[mode], root);
    if (self->lv_users == 1 && self->palette != NULL) {
        // Indexed files draw through their own Palette, so it has to reach the decoder.
        displayio_lvgl_shader_register_user(MP_OBJ_FROM_PTR(self->palette), self,
            DISPLAYIO_LVGL_USER_ONDISKBITMAP);
    }
    return &self->lv_dsc[mode];
}

void displayio_ondiskbitmap_lv_release(displayio_ondiskbitmap_t *self, bool full, lv_obj_t *root) {
    uint8_t mode = full ? 1 : 0;
    displayio_lvgl_image_root_remove((displayio_lvgl_image_root_t **)&self->lv_roots[mode], root);
    lv_image_cache_drop(&self->lv_dsc[mode]);
    if (self->lv_users > 0) {
        self->lv_users--;
    }
    if (full && self->lv_full_users > 0) {
        self->lv_full_users--;
    }
    if (self->lv_users == 0) {
        self->lv_full_users = 0;
        lv_memzero(&self->lv_dsc[0], sizeof(self->lv_dsc[0]));
        lv_memzero(&self->lv_dsc[1], sizeof(self->lv_dsc[1]));
        if (self->palette != NULL) {
            displayio_lvgl_shader_unregister_user(MP_OBJ_FROM_PTR(self->palette), self);
        }
        if (self->lv_decoded != NULL) {
            lv_draw_buf_destroy(self->lv_decoded);
            self->lv_decoded = NULL;
        }
        odb_decoder_free(self);
    } else if (!uses_full_image(self) && self->lv_decoded != NULL) {
        // Back to streaming rows: give the RAM copy back.
        lv_draw_buf_destroy(self->lv_decoded);
        self->lv_decoded = NULL;
    }
}

void displayio_ondiskbitmap_lv_palette_changed(displayio_ondiskbitmap_t *self) {
    refresh_palette(self);
    for (uint8_t mode = 0; mode < 2; mode++) {
        lv_image_cache_drop(&self->lv_dsc[mode]);
        displayio_lvgl_image_invalidate_all(
            (displayio_lvgl_image_root_t *)self->lv_roots[mode], &self->lv_dsc[mode]);
    }
}

// --- LVGL image decoder ------------------------------------------------------

static lv_result_t odb_decoder_info(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc,
    lv_image_header_t *header) {
    (void)decoder;
    if (dsc->src_type != LV_IMAGE_SRC_VARIABLE) {
        return LV_RESULT_INVALID;
    }
    const lv_image_dsc_t *src = dsc->src;
    if (src->reserved != &displayio_lvgl_ondiskbitmap_tag || src->data == NULL) {
        return LV_RESULT_INVALID;
    }
    displayio_ondiskbitmap_t *odb = (displayio_ondiskbitmap_t *)(void *)src->data;
    lv_color_format_t cf = displayio_ondiskbitmap_lv_color_format(odb);
    if (cf == LV_COLOR_FORMAT_UNKNOWN) {
        return LV_RESULT_INVALID;
    }
    header->cf = cf;
    header->w = odb->width;
    header->h = odb->height;
    header->stride = lv_draw_buf_width_to_stride(odb->width, cf);
    return LV_RESULT_OK;
}

static lv_result_t odb_decoder_open(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc) {
    (void)decoder;
    const lv_image_dsc_t *src = dsc->src;
    displayio_ondiskbitmap_t *odb = (displayio_ondiskbitmap_t *)(void *)src->data;
    lv_color_format_t cf = dsc->header.cf;
    if (cf == LV_COLOR_FORMAT_UNKNOWN) {
        return LV_RESULT_INVALID;
    }

    odb_decoder_t *s = (odb_decoder_t *)odb->lv_decoder;
    if (s == NULL) {
        s = lv_malloc_zeroed(sizeof(odb_decoder_t));
        if (s == NULL) {
            return LV_RESULT_INVALID;
        }
        s->odb = odb;
        odb->lv_decoder = s;
    }
    dsc->user_data = s;

    if (uses_full_image(odb)) {
        if (odb->lv_decoded != NULL) {
            dsc->decoded = odb->lv_decoded;
            return LV_RESULT_OK;
        }
        if (!row_converts(odb) && odb->lv_mapped_all != NULL && odb->top_down &&
            odb->height > 0) {
            uint32_t size = (uint32_t)odb->stride * odb->height;
            if (lv_draw_buf_init(&s->wrap, odb->width, odb->height, cf, odb->stride,
                (void *)odb->lv_mapped_all, size) == LV_RESULT_OK) {
                dsc->decoded = &s->wrap;
                return LV_RESULT_OK;
            }
        }
        // Fall through to row streaming when the file is mapped.
    }

    if (s->row_data == NULL) {
        s->row_stride = lv_draw_buf_width_to_stride(odb->width, cf);
        if (s->row_stride < odb->stride) {
            s->row_stride = odb->stride;
        }
        s->row_data = lv_malloc(s->row_stride);
        if (s->row_data == NULL) {
            return LV_RESULT_INVALID;
        }
    }
    return LV_RESULT_OK;
}

static lv_result_t odb_decoder_get_area(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc,
    const lv_area_t *full_area, lv_area_t *decoded_area) {
    (void)decoder;
    odb_decoder_t *s = dsc->user_data;
    displayio_ondiskbitmap_t *odb = s->odb;

    if (decoded_area->y1 == LV_COORD_MIN) {
        *decoded_area = *full_area;
        decoded_area->y2 = decoded_area->y1;
    } else {
        decoded_area->y1++;
        decoded_area->y2++;
        if (decoded_area->y1 > full_area->y2) {
            return LV_RESULT_INVALID;
        }
    }

    // Serve whole rows in image coordinates. LVGL intersects the decoded area with the
    // clip area, so this also covers a TileGrid cell showing a horizontal window.
    decoded_area->x1 = 0;
    decoded_area->x2 = odb->width - 1;

    if (decoded_area->y1 < 0 || decoded_area->y1 >= odb->height) {
        return LV_RESULT_INVALID;
    }
    uint32_t y = decoded_area->y1;
    lv_color_format_t cf = dsc->header.cf;
    const uint8_t *row = NULL;

    #if CIRCUITPY_STORAGE_MAP_FILE
    uint32_t file_y = odb->top_down ? y : odb->height - 1 - y;
    uint32_t offset = odb->data_offset + file_y * odb->stride;
    uint32_t len = row_bytes(odb);
    if (is_mapped(odb)) {
        row = map_at(odb, offset, len);
        if (row == NULL) {
            // The row crosses a run boundary; copy it together in RAM.
            if (!copy_mapped(odb, offset, len, s->row_data)) {
                return LV_RESULT_INVALID;
            }
            row = s->row_data;
        }
    }
    #else
    (void)y;
    #endif
    if (row == NULL) {
        // Not mapped. Attach decodes the whole image, so streaming here would mean the
        // decode failed; don't touch the file from the render thread.
        return LV_RESULT_INVALID;
    }

    if (row_converts(odb)) {
        if (odb->bits_per_pixel <= 8) {
            convert_row_indexed(odb, row, s->row_data, odb->width);
        } else {
            convert_row_16(odb, row, s->row_data, odb->width);
        }
        if (lv_draw_buf_init(&s->row, odb->width, 1, cf, s->row_stride, s->row_data,
            s->row_stride) != LV_RESULT_OK) {
            return LV_RESULT_INVALID;
        }
    } else if (lv_draw_buf_init(&s->row, odb->width, 1, cf, odb->stride, (void *)row,
        odb->stride) != LV_RESULT_OK) {
        return LV_RESULT_INVALID;
    }
    dsc->decoded = &s->row;
    return LV_RESULT_OK;
}

static void odb_decoder_close(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc) {
    (void)decoder;
    // The scratch stays on the OnDiskBitmap for the next draw.
    dsc->user_data = NULL;
}

void displayio_lvgl_ondiskbitmap_decoder_init(void) {
    lv_image_decoder_t *dec = lv_image_decoder_create();
    if (dec == NULL) {
        return;
    }
    lv_image_decoder_set_info_cb(dec, odb_decoder_info);
    lv_image_decoder_set_open_cb(dec, odb_decoder_open);
    lv_image_decoder_set_get_area_cb(dec, odb_decoder_get_area);
    lv_image_decoder_set_close_cb(dec, odb_decoder_close);
    dec->name = "CP OnDiskBitmap";
}

// --- displayio API -----------------------------------------------------------

void common_hal_displayio_ondiskbitmap_construct(displayio_ondiskbitmap_t *self, mp_obj_t file) {
    self->file = file;
    self->cache_len = 0;
    self->cache_start = 0;
    self->lv_mapped_all = NULL;
    self->lv_decoded = NULL;
    self->lv_palette = NULL;
    self->lv_palette_generation = 0;
    self->lv_decoded_generation = 0;
    lv_memzero(self->lv_dsc, sizeof(self->lv_dsc));
    self->lv_decoder = NULL;
    self->lv_roots[0] = NULL;
    self->lv_roots[1] = NULL;
    self->lv_users = 0;
    self->lv_full_users = 0;
    uint16_t bmp_header[69];
    int errcode = 0;

    mp_stream_seek(file, 0, MP_SEEK_SET, &errcode);
    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }

    // Read the minimum amount of bytes required to parse a BITMAPCOREHEADER.
    // If needed, we will read more bytes down below.
    mp_uint_t bytes_read = mp_stream_rw(file, bmp_header, 26, &errcode, MP_STREAM_RW_ONCE);
    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }
    if (bytes_read != 26 || memcmp(bmp_header, "BM", 2) != 0) {
        mp_arg_error_invalid(MP_QSTR_file);
    }

    // Read header size to determine if more header bytes needs to be read.
    uint32_t header_size = read_word(bmp_header, 7);
    DISPLAYIO_ODBMP_DEBUG("header_size: %d\n", header_size);

    if (header_size == 40 || header_size == 108 || header_size == 124) {
        // Read the remaining header bytes
        bytes_read = mp_stream_rw(file, bmp_header + 13, header_size - 12, &errcode, MP_STREAM_RW_ONCE);
        if (errcode != 0) {
            mp_raise_OSError(errcode);
        }
        DISPLAYIO_ODBMP_DEBUG("bytes_read: %d\n", bytes_read);
        if (bytes_read != (header_size - 12)) {
            mp_arg_error_invalid(MP_QSTR_file);
        }
    } else if (header_size != 12) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Only Windows format, uncompressed BMP supported: given header size is %d"), header_size);
    }


    uint32_t compression = read_word(bmp_header, 15);
    DISPLAYIO_ODBMP_DEBUG("compression: %d\n", compression);

    // 0 is uncompressed; 3 is bitfield compressed. 1 and 2 are RLE compression.
    if (compression != 0 && compression != 3) {
        mp_raise_ValueError(MP_ERROR_TEXT("RLE-compressed BMP not supported"));
    }

    // We can't cast because we're not aligned.
    self->data_offset = read_word(bmp_header, 5);

    self->bitfield_compressed = (compression == 3);
    self->bits_per_pixel = bmp_header[14];
    self->width = read_word(bmp_header, 9);
    // A negative height means the rows are stored top to bottom.
    int32_t bmp_height = (int16_t)read_word(bmp_header, 11);
    self->top_down = bmp_height < 0;
    if (bmp_height < 0) {
        bmp_height = -bmp_height;
    }
    self->height = (uint16_t)bmp_height;

    DISPLAYIO_ODBMP_DEBUG("data offset: %d\n", self->data_offset);
    DISPLAYIO_ODBMP_DEBUG("width: %d\n", self->width);
    DISPLAYIO_ODBMP_DEBUG("height: %d\n", self->height);
    DISPLAYIO_ODBMP_DEBUG("bpp: %d\n", self->bits_per_pixel);


    displayio_colorconverter_t *colorconverter =
        mp_obj_malloc(displayio_colorconverter_t, &displayio_colorconverter_type);
    common_hal_displayio_colorconverter_construct(colorconverter, false, DISPLAYIO_COLORSPACE_RGB888);
    self->colorconverter = colorconverter;

    if (self->bits_per_pixel == 16) {
        if (((header_size >= 56)) || (self->bitfield_compressed)) {
            self->r_bitmask = read_word(bmp_header, 27);
            self->g_bitmask = read_word(bmp_header, 29);
            self->b_bitmask = read_word(bmp_header, 31);

        } else { // no compression or short header means 5:5:5
            self->r_bitmask = 0x7c00;
            self->g_bitmask = 0x3e0;
            self->b_bitmask = 0x1f;
        }
    } else if (self->bits_per_pixel <= 8) { // indexed
        uint32_t number_of_colors = 0;
        if (header_size >= 40) {
            number_of_colors = read_word(bmp_header, 23);
        }
        if (number_of_colors == 0) {
            number_of_colors = 1 << self->bits_per_pixel;
        }

        displayio_palette_t *palette = mp_obj_malloc(displayio_palette_t, &displayio_palette_type);
        common_hal_displayio_palette_construct(palette, number_of_colors, false);

        if (number_of_colors > 1) {
            uint16_t palette_size = number_of_colors * sizeof(uint32_t);
            uint16_t palette_offset = 0xe + header_size;

            uint32_t *palette_data = m_malloc_without_collect(palette_size);

            mp_stream_seek(file, palette_offset, MP_SEEK_SET, &errcode);
            if (errcode != 0) {
                mp_raise_OSError(errcode);
            }

            mp_uint_t palette_bytes_read = mp_stream_rw(file, palette_data, palette_size, &errcode, MP_STREAM_RW_ONCE);
            if (errcode != 0) {
                mp_raise_OSError(errcode);
            }
            if (palette_bytes_read != palette_size) {
                mp_raise_ValueError(MP_ERROR_TEXT("Unable to read color palette data"));
            }
            for (uint16_t i = 0; i < number_of_colors; i++) {
                common_hal_displayio_palette_set_color(palette, i, palette_data[i]);
            }

            #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
            m_free(palette_data, palette_size);
            #else
            m_free(palette_data);
            #endif
        } else {
            common_hal_displayio_palette_set_color(palette, 0, 0x0);
            common_hal_displayio_palette_set_color(palette, 1, 0xffffff);
        }
        self->palette = palette;

        // The decoder expands indexed rows through this LVGL copy of the palette.
        uint32_t lv_palette_count = 1u << self->bits_per_pixel;
        self->lv_palette = m_malloc_without_collect(lv_palette_count * sizeof(lv_color32_t));
        refresh_palette(self);
    }

    uint8_t bytes_per_pixel = (self->bits_per_pixel / 8)  ? (self->bits_per_pixel / 8) : 1;
    uint8_t pixels_per_byte = 8 / self->bits_per_pixel;
    if (pixels_per_byte == 0) {
        self->stride = (self->width * bytes_per_pixel);
        // Rows are word aligned.
        if (self->stride % 4 != 0) {
            self->stride += 4 - self->stride % 4;
        }
    } else {
        uint32_t bit_stride = self->width * self->bits_per_pixel;
        if (bit_stride % 32 != 0) {
            bit_stride += 32 - bit_stride % 32;
        }
        self->stride = (bit_stride / 8);
    }

    #if CIRCUITPY_STORAGE_MAP_FILE
    self->map_count = 0;
    self->rows_mapped = false;
    if (storage_map_runs(file, add_run, self) != 0 || self->map_count > MP_ARRAY_SIZE(self->map)) {
        self->map_count = 0;
    }
    if (self->map_count > 0 && self->height > 0) {
        // Whole pixel block in one run? Then a transform can draw straight from flash.
        uint32_t size = (uint32_t)(self->height - 1) * self->stride + row_bytes(self);
        self->lv_mapped_all = map_at(self, self->data_offset, size);
    }
    #endif
}


void common_hal_displayio_ondiskbitmap_deinit(displayio_ondiskbitmap_t *self) {
    if (common_hal_displayio_ondiskbitmap_deinited(self)) {
        return;
    }
    mp_stream_close(self->file);
    self->file = NULL;
    #if CIRCUITPY_STORAGE_MAP_FILE
    // A TileGrid may still draw this bitmap, so it must stop reading rows from mapped
    // flash: the file is closed and the mapping can go away. Nothing decodes after this,
    // so a redraw paints color 0 like the legacy compositor does.
    displayio_lvgl_lock();
    self->map_count = 0;
    self->lv_mapped_all = NULL;
    displayio_lvgl_unlock();
    #endif
}

bool common_hal_displayio_ondiskbitmap_deinited(displayio_ondiskbitmap_t *self) {
    return self->file == NULL;
}

uint32_t common_hal_displayio_ondiskbitmap_get_pixel(displayio_ondiskbitmap_t *self,
    int16_t x, int16_t y) {
    if (common_hal_displayio_ondiskbitmap_deinited(self) ||
        x < 0 || x >= self->width || y < 0 || y >= self->height) {
        return 0;
    }

    uint32_t location;
    uint8_t bytes_per_pixel = (self->bits_per_pixel / 8)  ? (self->bits_per_pixel / 8) : 1;
    uint8_t pixels_per_byte = 8 / self->bits_per_pixel;
    uint32_t file_y = self->top_down ? y : self->height - y - 1;
    if (pixels_per_byte == 0) {
        location = self->data_offset + file_y * self->stride + x * bytes_per_pixel;
    } else {
        location = self->data_offset + file_y * self->stride + x / pixels_per_byte;
    }
    int errcode = 0;
    if (location < self->cache_start || location + bytes_per_pixel > self->cache_start + self->cache_len) {
        mp_stream_seek(self->file, location, MP_SEEK_SET, &errcode);
        if (errcode != 0) {
            return 0;
        }
        self->cache_start = location;
        self->cache_len = mp_stream_rw(self->file, self->cache, sizeof(self->cache), &errcode, MP_STREAM_RW_ONCE);
        if (errcode != 0 || self->cache_len < bytes_per_pixel) {
            self->cache_len = 0;
            return 0;
        }
    }
    uint32_t pixel_data = 0;
    memcpy(&pixel_data, self->cache + (location - self->cache_start), bytes_per_pixel);
    uint32_t tmp = 0;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    if (bytes_per_pixel == 1) {
        uint8_t offset = (x % pixels_per_byte) * self->bits_per_pixel;
        uint8_t mask = (1 << self->bits_per_pixel) - 1;

        return (pixel_data >> ((8 - self->bits_per_pixel) - offset)) & mask;
    } else if (bytes_per_pixel == 2) {
        if (self->g_bitmask == 0x07e0) { // 565
            red = ((pixel_data & self->r_bitmask) >> 11);
            green = ((pixel_data & self->g_bitmask) >> 5);
            blue = ((pixel_data & self->b_bitmask) >> 0);
        } else { // 555
            red = ((pixel_data & self->r_bitmask) >> 10);
            green = ((pixel_data & self->g_bitmask) >> 4);
            blue = ((pixel_data & self->b_bitmask) >> 0);
        }
        tmp = (red << 19 | green << 10 | blue << 3);
        return tmp;
    } else if ((bytes_per_pixel == 4) && (self->bitfield_compressed)) {
        return pixel_data & 0x00FFFFFF;
    } else {
        return pixel_data;
    }
}

uint16_t common_hal_displayio_ondiskbitmap_get_height(displayio_ondiskbitmap_t *self) {
    return self->height;
}

uint16_t common_hal_displayio_ondiskbitmap_get_width(displayio_ondiskbitmap_t *self) {
    return self->width;
}

mp_obj_t common_hal_displayio_ondiskbitmap_get_pixel_shader(displayio_ondiskbitmap_t *self) {
    return MP_OBJ_FROM_PTR(self->pixel_shader_base);
}
