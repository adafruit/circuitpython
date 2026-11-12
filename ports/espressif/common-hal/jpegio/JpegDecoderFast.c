// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar
//
// SPDX-License-Identifier: MIT

// Faster software JPEG decode for jpegio, using Espressif's esp_new_jpeg
// library. On the ESP32-S3 it uses the PIE SIMD instructions for the IDCT and
// color conversion. Anything it does not handle returns false and jpegio
// decodes with TJpgDec instead.

#include "py/runtime.h"

#include "shared-bindings/bitmaptools/__init__.h"
#include "shared-module/displayio/Bitmap.h"
#include "shared-module/jpegio/JpegDecoder.h"

#include "esp_jpeg_dec.h"

// The S3 SIMD code needs a 16-byte aligned output buffer. Buffers from
// m_malloc start on a GC block, so they are aligned.
#define OUTPUT_ALIGNMENT (16)
_Static_assert(MICROPY_BYTES_PER_GC_BLOCK % OUTPUT_ALIGNMENT == 0, "GC block is not 16-byte aligned");

typedef enum {
    MODE_NONE,   // leave it to TJpgDec
    MODE_DIRECT, // decode straight into the Bitmap
    MODE_BANDS,  // decode a band of rows at a time and blit each into the Bitmap
} fast_mode_t;

static bool decode_direct(jpeg_dec_config_t *config, jpeg_dec_io_t *io, displayio_bitmap_t *bitmap) {
    jpeg_dec_handle_t decoder;
    if (jpeg_dec_open(config, &decoder) != JPEG_ERR_OK) {
        return false;
    }
    jpeg_dec_header_info_t info;
    int outbuf_len;
    bool ok = jpeg_dec_parse_header(decoder, io, &info) == JPEG_ERR_OK &&
        jpeg_dec_get_outbuf_len(decoder, &outbuf_len) == JPEG_ERR_OK &&
        (size_t)outbuf_len == bitmap->width * bitmap->height * sizeof(uint16_t);
    if (ok) {
        io->outbuf = (uint8_t *)bitmap->data;
        ok = jpeg_dec_process(decoder, io) == JPEG_ERR_OK;
    }
    jpeg_dec_close(decoder);
    if (ok) {
        displayio_area_t area = { 0, 0, bitmap->width, bitmap->height, NULL };
        displayio_bitmap_set_dirty_area(bitmap, &area);
    }
    return ok;
}

// Copies each band with the same clipping as TJpgDec's output.
static bool decode_bands(jpegio_jpegdecoder_obj_t *self, jpeg_dec_config_t *config, jpeg_dec_io_t *io) {
    int width = self->decoder.width;
    int height = self->decoder.height;
    jpeg_dec_handle_t decoder;
    config->block_enable = true;
    if (jpeg_dec_open(config, &decoder) != JPEG_ERR_OK) {
        return false;
    }
    jpeg_dec_header_info_t info;
    int outbuf_len;
    int band_count;
    uint32_t *band = NULL;
    bool ok = jpeg_dec_parse_header(decoder, io, &info) == JPEG_ERR_OK &&
        info.width == width && info.height == height &&
        jpeg_dec_get_outbuf_len(decoder, &outbuf_len) == JPEG_ERR_OK &&
        jpeg_dec_get_process_count(decoder, &band_count) == JPEG_ERR_OK &&
        outbuf_len > 0 && outbuf_len % (width * 2) == 0;
    if (ok) {
        band = m_malloc_maybe(outbuf_len);
        ok = band != NULL;
    }
    int top = 0;
    for (int i = 0; ok && i < band_count; i++) {
        io->outbuf = (uint8_t *)band;
        if (jpeg_dec_process(decoder, io) != JPEG_ERR_OK) {
            ok = false;
            break;
        }
        // The last band of a 4:2:0 image can be padded past the bottom of the image.
        int rows = MIN(io->out_size / (width * 2), height - top);
        if (rows <= 0) {
            break;
        }
        displayio_bitmap_t src = {
            .width = width,
            .height = rows,
            .data = band,
            .stride = width / 2, // in units of uint32_t
            .bits_per_value = 16,
            .x_shift = 1,
            .x_mask = 1,
            .bitmask = 0xffff,
        };
        if (!jpegio_jpegdecoder_blit_rect(self, &src, 0, top)) {
            break;
        }
        top += rows;
    }
    jpeg_dec_close(decoder);
    m_free(band);
    return ok;
}

static fast_mode_t choose_mode(jpegio_jpegdecoder_obj_t *self, int scale) {
    displayio_bitmap_t *bitmap = self->dest;
    // TJpgDec raises the right error for these.
    if (bitmap->bits_per_value != 16 || bitmap->read_only) {
        return MODE_NONE;
    }
    // Colors differ slightly from TJpgDec's, which would change which pixels
    // match a skip index.
    if (!self->skip_source_index_none || !self->skip_dest_index_none) {
        return MODE_NONE;
    }
    bool gray = self->decoder.ncomp == 1;
    bool yuv420 = self->decoder.msx == 2 && self->decoder.msy == 2;
    if (scale != 0 || !(gray || yuv420)) {
        return MODE_NONE;
    }

    int width = self->decoder.width;
    int height = self->decoder.height;
    bool whole = bitmap->width == width && bitmap->height == height &&
        self->x == 0 && self->y == 0 &&
        self->lim.x1 <= 0 && self->lim.y1 <= 0 &&
        self->lim.x2 >= width && self->lim.y2 >= height;
    // Bitmap rows are padded to 32 bits, so an odd width is not packed.
    bool packed = bitmap->stride * sizeof(uint32_t) == width * sizeof(uint16_t) &&
        ((uintptr_t)bitmap->data & (OUTPUT_ALIGNMENT - 1)) == 0;
    if (whole && packed) {
        return MODE_DIRECT;
    }
    // Band mode needs whole MCUs across and whole 8x8 blocks down.
    if (width % (self->decoder.msx * 8) == 0 && height % 8 == 0) {
        return MODE_BANDS;
    }
    return MODE_NONE;
}

bool jpegio_jpegdecoder_fast_supported(jpegio_jpegdecoder_obj_t *self, int scale) {
    return choose_mode(self, scale) != MODE_NONE;
}

bool jpegio_jpegdecoder_fast_decode(jpegio_jpegdecoder_obj_t *self, const mp_buffer_info_t *jpeg, int scale) {
    jpeg_dec_config_t config = DEFAULT_JPEG_DEC_CONFIG();
    config.output_type = JPEG_PIXEL_FORMAT_RGB565_BE; // RGB565_SWAPPED, as TJpgDec
    jpeg_dec_io_t io = {
        .inbuf = jpeg->buf,
        .inbuf_len = jpeg->len,
    };
    switch (choose_mode(self, scale)) {
        case MODE_DIRECT:
            return decode_direct(&config, &io, self->dest);
        case MODE_BANDS:
            return decode_bands(self, &config, &io);
        default:
            return false;
    }
}
