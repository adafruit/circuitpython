// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT
//
// Minimal esp_jpeg interface used by the RM690B0 driver.
// Provides a drop-in replacement for the ESP-IDF component by wrapping TJpgDec.

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(ESP_PLATFORM)
#include "esp_err.h"
#else
typedef int32_t esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_NO_MEM 0x103
#define ESP_ERR_NOT_SUPPORTED 0x105
#define ESP_ERR_INVALID_SIZE 0x10B
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    JPEG_IMAGE_FORMAT_RGB565 = 0,
    JPEG_IMAGE_FORMAT_RGB888 = 1,
    JPEG_IMAGE_FORMAT_GRAYSCALE = 2,
} esp_jpeg_image_format_t;

typedef enum {
    JPEG_IMAGE_SCALE_0 = 0,
} esp_jpeg_image_scale_t;

typedef struct {
    bool swap_color_bytes : 1;
    bool use_scaler : 1;
} esp_jpeg_flags_t;

typedef struct {
    const uint8_t *indata;
    size_t indata_size;
    uint8_t *outbuf;
    size_t outbuf_size;
    esp_jpeg_image_format_t out_format;
    esp_jpeg_image_scale_t out_scale;
    esp_jpeg_flags_t flags;
    intptr_t user_data;
    esp_err_t (*on_block)(intptr_t ctx,
        uint32_t top, uint32_t left,
        uint32_t bottom, uint32_t right,
        const uint16_t *pixels);
} esp_jpeg_image_cfg_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t *outbuf;
    size_t outbuf_size;
} esp_jpeg_image_output_t;

esp_err_t esp_jpeg_get_image_info(const esp_jpeg_image_cfg_t *cfg, esp_jpeg_image_output_t *out);
esp_err_t esp_jpeg_decode(const esp_jpeg_image_cfg_t *cfg, esp_jpeg_image_output_t *out);

#ifdef __cplusplus
}
#endif
