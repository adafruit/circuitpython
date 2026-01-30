// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT
//
// Minimal esp_jpeg implementation backed by TJpgDec.

#include "esp_jpeg/esp_jpeg.h"

#include <string.h>
#include <stdlib.h>

#include "tjpgd.h"

#if defined(ESP_PLATFORM)
#include "esp_heap_caps.h"
#endif

#define ESP_JPEG_TJPGD_WORK_BUFFER_SIZE 4096
#define ESP_JPEG_MAX_BLOCK_PIXELS      256

typedef struct {
    uint8_t *rgb565_buffer;
    uint32_t width;
    const uint8_t *jpg_data;
    size_t jpg_size;
    size_t jpg_offset;
    bool swap_bytes;
    intptr_t user_data;
    esp_err_t (*on_block)(intptr_t,
        uint32_t, uint32_t,
        uint32_t, uint32_t,
        const uint16_t *);
    esp_err_t last_error;
} esp_jpeg_tjpgd_ctx_t;

static void *esp_jpeg_alloc(size_t size) {
#if defined(ESP_PLATFORM)
    void *ptr = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!ptr) {
        ptr = heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    }
    return ptr;
#else
    return malloc(size);
#endif
}

static void esp_jpeg_free(void *ptr) {
#if defined(ESP_PLATFORM)
    heap_caps_free(ptr);
#else
    free(ptr);
#endif
}

static size_t esp_jpeg_input(JDEC *jd, uint8_t *buff, size_t ndata) {
    esp_jpeg_tjpgd_ctx_t *ctx = (esp_jpeg_tjpgd_ctx_t *)jd->device;

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

static int esp_jpeg_output(JDEC *jd, void *bitmap, JRECT *rect) {
    esp_jpeg_tjpgd_ctx_t *ctx = (esp_jpeg_tjpgd_ctx_t *)jd->device;

    if (ctx->rgb565_buffer != NULL) {
        uint8_t *src = (uint8_t *)bitmap;
        for (int y = rect->top; y <= rect->bottom; y++) {
            uint32_t row_base = (uint32_t)y * ctx->width;
            for (int x = rect->left; x <= rect->right; x++) {
                uint16_t rgb565 = ((uint16_t)src[0] << 8) | src[1];
                src += 2;
                uint32_t offset = (row_base + x) * 2;
                if (ctx->swap_bytes) {
                    ctx->rgb565_buffer[offset] = (rgb565 >> 8) & 0xFF;
                    ctx->rgb565_buffer[offset + 1] = rgb565 & 0xFF;
                } else {
                    ctx->rgb565_buffer[offset] = rgb565 & 0xFF;
                    ctx->rgb565_buffer[offset + 1] = (rgb565 >> 8) & 0xFF;
                }
            }
        }
    } else if (ctx->on_block != NULL) {
        uint32_t width = (uint32_t)rect->right - (uint32_t)rect->left + 1;
        uint32_t height = (uint32_t)rect->bottom - (uint32_t)rect->top + 1;
        uint32_t total = width * height;
        if (total == 0 || total > ESP_JPEG_MAX_BLOCK_PIXELS) {
            ctx->last_error = ESP_ERR_INVALID_SIZE;
            return 0;
        }

        uint16_t block_pixels[ESP_JPEG_MAX_BLOCK_PIXELS];
        const uint8_t *block_src = (const uint8_t *)bitmap;
        for (uint32_t i = 0; i < total; i++) {
            uint16_t rgb565 = ((uint16_t)block_src[0] << 8) | block_src[1];
            block_src += 2;
            if (ctx->swap_bytes) {
                rgb565 = (uint16_t)((rgb565 << 8) | (rgb565 >> 8));
            }
            block_pixels[i] = rgb565;
        }

        esp_err_t cb = ctx->on_block(ctx->user_data,
            (uint32_t)rect->top,
            (uint32_t)rect->left,
            (uint32_t)rect->bottom,
            (uint32_t)rect->right,
            block_pixels);
        if (cb != ESP_OK) {
            ctx->last_error = cb;
            return 0;
        }
    } else {
        return 0;
    }

    return 1;
}

static esp_err_t esp_jpeg_prepare_decoder(const esp_jpeg_image_cfg_t *cfg, JDEC *out_dec, esp_jpeg_tjpgd_ctx_t *ctx, void **work_buffer) {
    if (cfg == NULL || cfg->indata == NULL || cfg->indata_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (cfg->out_scale != JPEG_IMAGE_SCALE_0) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    void *work = esp_jpeg_alloc(ESP_JPEG_TJPGD_WORK_BUFFER_SIZE);
    if (!work) {
        return ESP_ERR_NO_MEM;
    }

    ctx->rgb565_buffer = NULL;
    ctx->width = 0;
    ctx->jpg_data = cfg->indata;
    ctx->jpg_size = cfg->indata_size;
    ctx->jpg_offset = 0;
    ctx->swap_bytes = cfg->flags.swap_color_bytes;
    ctx->user_data = cfg->user_data;
    ctx->on_block = cfg->on_block;
    ctx->last_error = ESP_OK;

    JRESULT res = jd_prepare(out_dec, esp_jpeg_input, work, ESP_JPEG_TJPGD_WORK_BUFFER_SIZE, ctx);
    if (res != JDR_OK) {
        esp_jpeg_free(work);
        return ESP_ERR_INVALID_ARG;
    }

    if (out_dec->width == 0 || out_dec->height == 0) {
        esp_jpeg_free(work);
        return ESP_ERR_INVALID_ARG;
    }

    *work_buffer = work;
    return ESP_OK;
}

esp_err_t esp_jpeg_get_image_info(const esp_jpeg_image_cfg_t *cfg, esp_jpeg_image_output_t *out) {
    if (out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    JDEC jdec;
    esp_jpeg_tjpgd_ctx_t ctx;
    void *work = NULL;

    esp_err_t err = esp_jpeg_prepare_decoder(cfg, &jdec, &ctx, &work);
    if (err != ESP_OK) {
        return err;
    }

    out->width = jdec.width;
    out->height = jdec.height;
    out->outbuf = NULL;
    out->outbuf_size = (size_t)jdec.width * jdec.height * 2;

    esp_jpeg_free(work);
    return ESP_OK;
}

esp_err_t esp_jpeg_decode(const esp_jpeg_image_cfg_t *cfg, esp_jpeg_image_output_t *out) {
    if (cfg == NULL || (cfg->outbuf == NULL && cfg->on_block == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cfg->out_format != JPEG_IMAGE_FORMAT_RGB565) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    JDEC jdec;
    esp_jpeg_tjpgd_ctx_t ctx;
    void *work = NULL;

    esp_err_t err = esp_jpeg_prepare_decoder(cfg, &jdec, &ctx, &work);
    if (err != ESP_OK) {
        return err;
    }

    size_t required_size = (size_t)jdec.width * jdec.height * 2;
    if (cfg->outbuf != NULL && cfg->outbuf_size < required_size) {
        esp_jpeg_free(work);
        return ESP_ERR_NO_MEM;
    }

    ctx.rgb565_buffer = cfg->outbuf;
    ctx.width = jdec.width;

    JRESULT res = jd_decomp(&jdec, esp_jpeg_output, 0);
    esp_jpeg_free(work);

    if (res != JDR_OK) {
        return (ctx.last_error != ESP_OK) ? ctx.last_error : ESP_FAIL;
    }

    if (out) {
        out->width = jdec.width;
        out->height = jdec.height;
        out->outbuf = cfg->outbuf;
        out->outbuf_size = (cfg->outbuf != NULL) ? required_size : 0;
    }

    return ESP_OK;
}
