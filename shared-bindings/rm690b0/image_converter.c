// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include "image_converter.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef CIRCUITPY
#include "py/misc.h"
#include "py/runtime.h"
#endif

#if defined(ESP_PLATFORM)
#include "esp_heap_caps.h"
#include "esp_err.h"
#if __has_include("esp_jpeg/esp_jpeg.h")
#include "esp_jpeg/esp_jpeg.h"
#define USE_ESP_JPEG 1
#else
#define USE_ESP_JPEG 0
#endif
#else
#define USE_ESP_JPEG 0
#endif

// =============================================================================
// Platform-Specific Memory Allocation
// =============================================================================
//
// IMAGE_MALLOC/IMAGE_FREE provide platform-appropriate memory allocation for
// temporary buffers used during image decoding. Behavior varies by platform:
//
// ESP_PLATFORM (ESP32, ESP32-S3, etc.):
//   - Priority: SPIRAM (PSRAM) first, then internal RAM
//   - Rationale: Image buffers can be large; SPIRAM preserves precious internal RAM
//   - Failure: Returns NULL if both allocations fail
//
// CIRCUITPY (CircuitPython runtime):
//   - Uses m_malloc_maybe() which is GC-aware and returns NULL on failure
//   - Memory is tracked by the garbage collector
//   - Failure: Returns NULL (does not raise exception)
//
// Other platforms (desktop, testing):
//   - Uses standard malloc()/free()
//   - Failure: Returns NULL per C standard
//
// All platforms: Caller MUST check for NULL return and handle gracefully.
// =============================================================================

#if !USE_ESP_JPEG
#if defined(ESP_PLATFORM)
static void *image_malloc(size_t size) {
    // Try SPIRAM first (larger, slower), fall back to internal RAM (faster, limited)
    void *ptr = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!ptr) {
        ptr = heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    }
    return ptr;
}
static void image_free(void *ptr) {
    if (ptr) {
        heap_caps_free(ptr);
    }
}
#define IMAGE_MALLOC(size) image_malloc(size)
#define IMAGE_FREE(ptr) image_free(ptr)
#elif defined(CIRCUITPY)
#define IMAGE_MALLOC(size) m_malloc_maybe(size)
#define IMAGE_FREE(ptr) m_free(ptr)
#else
#define IMAGE_MALLOC(size) malloc(size)
#define IMAGE_FREE(ptr) free(ptr)
#endif
#endif


#if !USE_ESP_JPEG
#include "tjpgd.h"
#endif

// =============================================================================
// Utility Functions
// =============================================================================

const char *img_error_string(img_error_t error) {
    switch (error) {
        case IMG_OK:
            return "Success";
        case IMG_ERR_NULL_POINTER:
            return "NULL pointer";
        case IMG_ERR_INVALID_SIZE:
            return "Invalid size";
        case IMG_ERR_BUFFER_TOO_SMALL:
            return "Buffer too small";
        case IMG_ERR_INVALID_FORMAT:
            return "Malformed data";
        case IMG_ERR_CORRUPTED_DATA:
            return "Corrupted data";
        case IMG_ERR_UNSUPPORTED:
            return "Unsupported format";
        case IMG_ERR_OUT_OF_MEMORY:
            return "Out of memory";
        case IMG_ERR_IO_ERROR:
            return "I/O error";
        default:
            return "Unknown error";
    }
}

// Read 16-bit little-endian value
static inline uint16_t read_le16(const uint8_t *data) {
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

// Read 32-bit little-endian value
static inline uint32_t read_le32(const uint8_t *data) {
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

// Read 32-bit signed little-endian value
static inline int32_t read_le32_signed(const uint8_t *data) {
    return (int32_t)read_le32(data);
}



// =============================================================================
// BMP Format Converter
// =============================================================================

img_error_t img_bmp_parse_header(
    const uint8_t *bmp_data,
    size_t bmp_size,
    img_info_t *info
    ) {
    if (bmp_data == NULL || info == NULL) {
        return IMG_ERR_NULL_POINTER;
    }

    // Check minimum size for headers (14 byte file header + 40 byte info header)
    if (bmp_size < 54) {
        return IMG_ERR_INVALID_FORMAT;
    }

    // Check BMP signature ('BM')
    if (bmp_data[0] != 'B' || bmp_data[1] != 'M') {
        return IMG_ERR_INVALID_FORMAT;
    }

    // Parse info header size
    uint32_t header_size = read_le32(bmp_data + 14);
    if (header_size < 40) {
        return IMG_ERR_INVALID_FORMAT;
    }

    // Parse dimensions
    int32_t width = read_le32_signed(bmp_data + 18);
    int32_t height = read_le32_signed(bmp_data + 22);

    // Validate dimensions
    if (width <= 0 || height == 0) {
        return IMG_ERR_INVALID_SIZE;
    }

    // Parse format information
    uint16_t planes = read_le16(bmp_data + 26);
    uint16_t bit_count = read_le16(bmp_data + 28);
    uint32_t compression = read_le32(bmp_data + 30);

    // Validate format
    if (planes != 1) {
        return IMG_ERR_INVALID_FORMAT;
    }

    // Check for supported bit depths
    if (bit_count != 16 && bit_count != 24 && bit_count != 32) {
        return IMG_ERR_UNSUPPORTED;
    }

    // Check compression (0 = BI_RGB uncompressed, 3 = BI_BITFIELDS)
    if (compression != 0 && compression != 3) {
        return IMG_ERR_UNSUPPORTED;
    }

    // Fill info structure
    info->width = (uint32_t)width;
    info->height = (uint32_t)(height > 0 ? height : -height);
    info->data_size = img_rgb565_buffer_size(info->width, info->height);
    if (info->data_size == SIZE_MAX) {
        return IMG_ERR_INVALID_SIZE;  // Overflow in buffer size calculation
    }
    info->bit_depth = bit_count;
    info->channels = (bit_count == 32) ? 4 : 3;
    info->has_alpha = (bit_count == 32);

    return IMG_OK;
}

img_error_t img_bmp_to_rgb565(
    const uint8_t *bmp_data,
    size_t bmp_size,
    uint8_t *rgb565_buffer,
    size_t buffer_size,
    img_info_t *info
    ) {
    if (bmp_data == NULL || rgb565_buffer == NULL) {
        return IMG_ERR_NULL_POINTER;
    }

    // Parse header
    img_info_t local_info;
    img_error_t err = img_bmp_parse_header(bmp_data, bmp_size, &local_info);
    if (err != IMG_OK) {
        return err;
    }

    // Check buffer size
    if (buffer_size < local_info.data_size) {
        return IMG_ERR_BUFFER_TOO_SMALL;
    }

    // Get parameters
    uint32_t width = local_info.width;
    uint32_t height = local_info.height;
    uint32_t data_offset = read_le32(bmp_data + 10);
    int32_t height_signed = read_le32_signed(bmp_data + 22);
    uint16_t bit_count = read_le16(bmp_data + 28);

    // Check if we have enough data
    if (data_offset >= bmp_size) {
        return IMG_ERR_CORRUPTED_DATA;
    }

    // Determine if image is bottom-up (positive height) or top-down (negative height)
    bool bottom_up = (height_signed > 0);

    // Calculate bytes per pixel and row stride with overflow checking
    uint32_t bytes_per_pixel = bit_count / 8;

    // Check for overflow in width * bytes_per_pixel
    if (bytes_per_pixel != 0 && width > (UINT32_MAX - 3) / bytes_per_pixel) {
        return IMG_ERR_INVALID_SIZE;
    }

    uint32_t row_stride = ((width * bytes_per_pixel + 3) & ~3);  // Rows padded to 4-byte boundary

    // Check for overflow in row_stride * height
    if (height != 0 && row_stride > UINT32_MAX / height) {
        return IMG_ERR_INVALID_SIZE;
    }
    uint32_t total_data_size = row_stride * height;

    // Check for overflow in data_offset + total_data_size
    if (data_offset > UINT32_MAX - total_data_size) {
        return IMG_ERR_INVALID_SIZE;
    }

    // Verify we have enough data
    if (data_offset + total_data_size > bmp_size) {
        return IMG_ERR_CORRUPTED_DATA;
    }

    // Convert pixel data
    const uint8_t *src_base = bmp_data + data_offset;
    uint8_t *dst = rgb565_buffer;

    for (uint32_t y = 0; y < height; y++) {
        // Calculate source row based on orientation
        uint32_t src_row = bottom_up ? (height - 1 - y) : y;
        const uint8_t *src = src_base + src_row * row_stride;

        for (uint32_t x = 0; x < width; x++) {
            uint8_t r, g, b;

            if (bit_count == 24) {
                // 24-bit BGR format
                b = src[0];
                g = src[1];
                r = src[2];
                src += 3;
            } else if (bit_count == 32) {
                // 32-bit BGRA format
                b = src[0];
                g = src[1];
                r = src[2];
                // src[3] is alpha (ignored for now)
                src += 4;
            } else if (bit_count == 16) {
                // 16-bit format (assume RGB565)
                uint16_t pixel = read_le16(src);
                r = RGB565_TO_R(pixel);
                g = RGB565_TO_G(pixel);
                b = RGB565_TO_B(pixel);
                src += 2;
            } else {
                // Should not reach here due to earlier validation
                return IMG_ERR_UNSUPPORTED;
            }

            // Convert to RGB565 and store (byte-by-byte to avoid alignment issues)
            uint16_t rgb565 = RGB888_TO_RGB565(r, g, b);
            dst[0] = rgb565 & 0xFF;         // Low byte
            dst[1] = (rgb565 >> 8) & 0xFF;  // High byte
            dst += 2;
        }
    }

    // Copy info if requested
    if (info != NULL) {
        *info = local_info;
    }

    return IMG_OK;
}

// =============================================================================
// JPEG Format Converter
// =============================================================================

#if USE_ESP_JPEG

img_error_t img_jpg_parse_header(
    const uint8_t *jpg_data,
    size_t jpg_size,
    img_info_t *info
    ) {
    if (jpg_data == NULL || info == NULL) {
        return IMG_ERR_NULL_POINTER;
    }

    esp_jpeg_image_cfg_t cfg = {
        .indata = jpg_data,
        .indata_size = jpg_size,
        .outbuf = NULL,
        .outbuf_size = 0,
        .out_format = JPEG_IMAGE_FORMAT_RGB565,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = {
            .swap_color_bytes = false,
            .use_scaler = false,
        },
    };

    esp_jpeg_image_output_t out;
    esp_err_t err = esp_jpeg_get_image_info(&cfg, &out);
    if (err != ESP_OK) {
        return IMG_ERR_INVALID_FORMAT;
    }

    info->width = out.width;
    info->height = out.height;
    info->data_size = img_rgb565_buffer_size(out.width, out.height);
    if (info->data_size == SIZE_MAX) {
        return IMG_ERR_INVALID_SIZE;  // Overflow in buffer size calculation
    }
    info->bit_depth = 8;
    info->channels = 3;
    info->has_alpha = false;

    return IMG_OK;
}

img_error_t img_jpg_to_rgb565(
    const uint8_t *jpg_data,
    size_t jpg_size,
    uint8_t *rgb565_buffer,
    size_t buffer_size,
    img_info_t *info
    ) {
    if (jpg_data == NULL || rgb565_buffer == NULL) {
        return IMG_ERR_NULL_POINTER;
    }

    esp_jpeg_image_cfg_t cfg = {
        .indata = jpg_data,
        .indata_size = jpg_size,
        .outbuf = rgb565_buffer,
        .outbuf_size = buffer_size,
        .out_format = JPEG_IMAGE_FORMAT_RGB565,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = {
            .swap_color_bytes = false,
            .use_scaler = false,
        },
    };

    esp_jpeg_image_output_t out_info;
    esp_err_t err = esp_jpeg_decode(&cfg, &out_info);
    if (err == ESP_ERR_NO_MEM) {
        return IMG_ERR_BUFFER_TOO_SMALL;
    }
    if (err != ESP_OK) {
        return IMG_ERR_CORRUPTED_DATA;
    }

    if (info != NULL) {
        info->width = out_info.width;
        info->height = out_info.height;
        info->data_size = out_info.outbuf_size;
        info->bit_depth = 8;
        info->channels = 3;
        info->has_alpha = false;
    }

    return IMG_OK;
}

#else // USE_ESP_JPEG

#define TJPGD_WORK_BUFFER_SIZE 4096

typedef struct {
    uint8_t *rgb565_buffer;
    uint32_t width;
    const uint8_t *jpg_data;
    size_t jpg_size;
    size_t jpg_offset;
} tjpgd_context_t;

static size_t tjpgd_input_func(JDEC *jd, uint8_t *buff, size_t ndata) {
    tjpgd_context_t *ctx = (tjpgd_context_t *)jd->device;

    if (!buff) {
        size_t bytes_available = ctx->jpg_size - ctx->jpg_offset;
        size_t bytes_to_skip = ndata;
        if (bytes_to_skip > bytes_available) {
            bytes_to_skip = bytes_available;
        }
        ctx->jpg_offset += bytes_to_skip;
        return bytes_to_skip;
    }

    size_t bytes_to_read = ndata;
    size_t bytes_available = ctx->jpg_size - ctx->jpg_offset;
    if (bytes_to_read > bytes_available) {
        bytes_to_read = bytes_available;
    }

    if (bytes_to_read > 0) {
        memcpy(buff, ctx->jpg_data + ctx->jpg_offset, bytes_to_read);
        ctx->jpg_offset += bytes_to_read;
    }

    return bytes_to_read;
}

static int tjpgd_output_func(JDEC *jd, void *bitmap, JRECT *rect) {
    tjpgd_context_t *ctx = (tjpgd_context_t *)jd->device;
    uint8_t *src = (uint8_t *)bitmap;

    for (int y = rect->top; y <= rect->bottom; y++) {
        uint32_t row_base = (uint32_t)y * ctx->width;
        for (int x = rect->left; x <= rect->right; x++) {
            uint16_t rgb565 = ((uint16_t)src[0] << 8) | src[1];
            src += 2;
            uint32_t offset = (row_base + x) * 2;
            ctx->rgb565_buffer[offset] = rgb565 & 0xFF;
            ctx->rgb565_buffer[offset + 1] = (rgb565 >> 8) & 0xFF;
        }
    }

    return 1;
}

img_error_t img_jpg_to_rgb565(
    const uint8_t *jpg_data,
    size_t jpg_size,
    uint8_t *rgb565_buffer,
    size_t buffer_size,
    img_info_t *info
    ) {
    if (jpg_data == NULL || rgb565_buffer == NULL) {
        return IMG_ERR_NULL_POINTER;
    }

    void *work_buffer = IMAGE_MALLOC(TJPGD_WORK_BUFFER_SIZE);
    if (work_buffer == NULL) {
        return IMG_ERR_OUT_OF_MEMORY;
    }

    JDEC jdec;
    tjpgd_context_t ctx = {
        .rgb565_buffer = NULL,
        .width = 0,
        .jpg_data = jpg_data,
        .jpg_size = jpg_size,
        .jpg_offset = 0,
    };

    JRESULT res = jd_prepare(&jdec, tjpgd_input_func, work_buffer, TJPGD_WORK_BUFFER_SIZE, (void *)&ctx);
    if (res != JDR_OK) {
        IMAGE_FREE(work_buffer);
        return IMG_ERR_INVALID_FORMAT;
    }

    if (jdec.width == 0 || jdec.height == 0) {
        IMAGE_FREE(work_buffer);
        return IMG_ERR_INVALID_SIZE;
    }

    size_t required_size = img_rgb565_buffer_size(jdec.width, jdec.height);
    if (required_size == SIZE_MAX) {
        IMAGE_FREE(work_buffer);
        return IMG_ERR_INVALID_SIZE;  // Overflow in buffer size calculation
    }
    if (buffer_size < required_size) {
        IMAGE_FREE(work_buffer);
        return IMG_ERR_BUFFER_TOO_SMALL;
    }

    ctx.rgb565_buffer = rgb565_buffer;
    ctx.width = jdec.width;

    res = jd_decomp(&jdec, tjpgd_output_func, 0);

    IMAGE_FREE(work_buffer);

    if (res != JDR_OK) {
        return IMG_ERR_CORRUPTED_DATA;
    }

    if (info != NULL) {
        info->width = jdec.width;
        info->height = jdec.height;
        info->data_size = required_size;
        info->bit_depth = 8;
        info->channels = 3;
        info->has_alpha = false;
    }

    return IMG_OK;
}

img_error_t img_jpg_parse_header(
    const uint8_t *jpg_data,
    size_t jpg_size,
    img_info_t *info
    ) {
    if (jpg_data == NULL || info == NULL) {
        return IMG_ERR_NULL_POINTER;
    }

    void *work_buffer = IMAGE_MALLOC(TJPGD_WORK_BUFFER_SIZE);
    if (work_buffer == NULL) {
        return IMG_ERR_OUT_OF_MEMORY;
    }

    JDEC jdec;
    tjpgd_context_t ctx = {
        .rgb565_buffer = NULL,
        .width = 0,
        .jpg_data = jpg_data,
        .jpg_size = jpg_size,
        .jpg_offset = 0,
    };

    JRESULT res = jd_prepare(&jdec, tjpgd_input_func, work_buffer, TJPGD_WORK_BUFFER_SIZE, (void *)&ctx);
    if (res != JDR_OK) {
        IMAGE_FREE(work_buffer);
        return IMG_ERR_INVALID_FORMAT;
    }

    if (jdec.width == 0 || jdec.height == 0) {
        IMAGE_FREE(work_buffer);
        return IMG_ERR_INVALID_SIZE;
    }

    info->width = jdec.width;
    info->height = jdec.height;
    info->data_size = img_rgb565_buffer_size(jdec.width, jdec.height);
    if (info->data_size == SIZE_MAX) {
        IMAGE_FREE(work_buffer);
        return IMG_ERR_INVALID_SIZE;  // Overflow in buffer size calculation
    }
    info->bit_depth = 8;
    info->channels = 3;
    info->has_alpha = false;

    IMAGE_FREE(work_buffer);
    return IMG_OK;
}

#endif // USE_ESP_JPEG
