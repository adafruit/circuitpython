// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

// videocore_accel_rotate_copy on the ESP32-P4 Pixel-Processing Accelerator
// (PPA). Its scale-rotate-mirror (SRM) engine copies a block of one picture
// into a block of another, rotated and/or mirrored, at scale 1.

#include "shared-module/videocore/__init__.h"

#include "driver/ppa.h"
#include "esp_cache.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_private/esp_cache_private.h"

// Registered on first use and kept: it's a small IDF heap allocation, and
// nothing else uses the SRM engine.
static ppa_client_handle_t videocore_ppa_client;

// Cache line alignment the PPA needs for an output buffer at p.
static size_t videocore_ppa_out_alignment(const void *p) {
    size_t alignment = 0;
    const uint32_t caps = esp_ptr_external_ram(p)
        ? MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA
        : MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA;
    if (esp_cache_get_alignment(caps, &alignment) != ESP_OK || alignment == 0) {
        alignment = 1;
    }
    return alignment;
}

// Fill everything in oper except the source buffer. Returns false when the PPA
// can't do op.
static bool videocore_ppa_config(const videocore_accel_rotate_copy_t *op, ppa_srm_oper_config_t *oper) {
    // RGB565 only: PPA RGB888 and ARGB8888 byte orders aren't checked against
    // the framebuffer's.
    if (op->bits_per_pixel != 16) {
        return false;
    }
    const size_t bytes_per_pixel = 2;

    // ppa_srm_rotation_angle_t is counter-clockwise; op->rotation is
    // clockwise. Clockwise 90 is counter-clockwise 270.
    ppa_srm_rotation_angle_t angle;
    switch (op->rotation) {
        case 0:
            angle = PPA_SRM_ROTATION_ANGLE_0;
            break;
        case 90:
            angle = PPA_SRM_ROTATION_ANGLE_270;
            break;
        case 180:
            angle = PPA_SRM_ROTATION_ANGLE_180;
            break;
        case 270:
            angle = PPA_SRM_ROTATION_ANGLE_90;
            break;
        default:
            return false;
    }
    const bool transposed = op->rotation == 90 || op->rotation == 270;
    // op flips after rotating. The driver doesn't document whether the PPA
    // mirrors before or after rotating, which only matters for 90 and 270, so
    // leave those to the caller's fallback.
    if (op->mirror_x && transposed) {
        return false;
    }

    if (op->src_width <= 0 || op->src_height <= 0 || op->dst_x < 0 || op->dst_y < 0 ||
        op->src_stride % bytes_per_pixel != 0 || op->dst_stride % bytes_per_pixel != 0 ||
        op->src_stride < op->src_width * bytes_per_pixel) {
        return false;
    }

    // The output buffer's address and size must be cache line multiples:
    // trim the picture to whole cache lines.
    const size_t alignment = videocore_ppa_out_alignment(op->dst_buf);
    const size_t out_buffer_size = op->dst_len / alignment * alignment;
    if ((uintptr_t)op->dst_buf % alignment != 0 || op->dst_stride == 0) {
        return false;
    }
    const mp_int_t out_pic_w = op->dst_stride / bytes_per_pixel;
    const mp_int_t out_pic_h = out_buffer_size / op->dst_stride;
    const mp_int_t out_w = transposed ? op->src_height : op->src_width;
    const mp_int_t out_h = transposed ? op->src_width : op->src_height;
    if (op->dst_x + out_w > out_pic_w || op->dst_y + out_h > out_pic_h) {
        return false;
    }

    const ppa_srm_oper_config_t config = {
        .in = {
            .pic_w = op->src_stride / bytes_per_pixel,
            .pic_h = op->src_height,
            .block_w = op->src_width,
            .block_h = op->src_height,
            .srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        },
        .out = {
            .buffer = op->dst_buf,
            .buffer_size = out_buffer_size,
            .pic_w = out_pic_w,
            .pic_h = out_pic_h,
            .block_offset_x = op->dst_x,
            .block_offset_y = op->dst_y,
            .srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        },
        .rotation_angle = angle,
        .scale_x = 1.0f,
        .scale_y = 1.0f,
        .mirror_x = op->mirror_x,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };
    *oper = config;
    return true;
}

bool videocore_accel_rotate_copy_supported(const videocore_accel_rotate_copy_t *op) {
    ppa_srm_oper_config_t oper;
    return videocore_ppa_config(op, &oper);
}

bool videocore_accel_rotate_copy(const videocore_accel_rotate_copy_t *op) {
    ppa_srm_oper_config_t oper;
    if (op->src == NULL || !videocore_ppa_config(op, &oper)) {
        return false;
    }
    oper.in.buffer = op->src;

    if (videocore_ppa_client == NULL) {
        const ppa_client_config_t client_config = {
            .oper_type = PPA_OPERATION_SRM,
        };
        if (ppa_register_client(&client_config, &videocore_ppa_client) != ESP_OK) {
            videocore_ppa_client = NULL;
            return false;
        }
    }

    // The driver invalidates the cache over every output row the block
    // touches, full width. Write those rows back first so pixels the CPU drew
    // beside the block (other displayio layers) aren't dropped.
    const size_t out_h = (op->rotation == 90 || op->rotation == 270) ? op->src_width : op->src_height;
    esp_cache_msync(op->dst_buf + op->dst_y * op->dst_stride, out_h * op->dst_stride,
        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);

    return ppa_do_scale_rotate_mirror(videocore_ppa_client, &oper) == ESP_OK;
}
