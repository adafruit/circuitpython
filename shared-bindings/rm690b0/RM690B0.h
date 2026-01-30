// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include "py/obj.h"

// Forward declaration for the rm690b0 object type
typedef struct {
    mp_obj_base_t base;
    bool initialized;
    mp_int_t width;
    mp_int_t height;
    mp_int_t rotation;
    uint8_t brightness_raw;
    mp_int_t font_id;  // Current font ID for text rendering
    // Port-specific implementation data will be added by common-hal
    void *impl;
} rm690b0_rm690b0_obj_t;

extern const mp_obj_type_t rm690b0_rm690b0_type;

// Common HAL function declarations
void common_hal_rm690b0_rm690b0_construct(rm690b0_rm690b0_obj_t *self);
void common_hal_rm690b0_rm690b0_deinit(rm690b0_rm690b0_obj_t *self);
void common_hal_rm690b0_rm690b0_init_display(rm690b0_rm690b0_obj_t *self);
void common_hal_rm690b0_rm690b0_fill_color(rm690b0_rm690b0_obj_t *self, uint16_t color);
void common_hal_rm690b0_rm690b0_set_brightness(rm690b0_rm690b0_obj_t *self, mp_float_t brightness);
mp_float_t common_hal_rm690b0_rm690b0_get_brightness(const rm690b0_rm690b0_obj_t *self);
void common_hal_rm690b0_rm690b0_pixel(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t color);
void common_hal_rm690b0_rm690b0_fill_rect(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, mp_int_t color);
void common_hal_rm690b0_rm690b0_hline(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t color);
void common_hal_rm690b0_rm690b0_vline(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t height, mp_int_t color);
void common_hal_rm690b0_rm690b0_line(rm690b0_rm690b0_obj_t *self, mp_int_t x0, mp_int_t y0, mp_int_t x1, mp_int_t y1, mp_int_t color);
void common_hal_rm690b0_rm690b0_rect(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, mp_int_t color);
void common_hal_rm690b0_rm690b0_circle(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t radius, mp_int_t color);
void common_hal_rm690b0_rm690b0_fill_circle(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t radius, mp_int_t color);
void common_hal_rm690b0_rm690b0_set_rotation(rm690b0_rm690b0_obj_t *self, mp_int_t degrees);
mp_int_t common_hal_rm690b0_rm690b0_get_rotation(const rm690b0_rm690b0_obj_t *self);
void common_hal_rm690b0_rm690b0_blit_buffer(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, mp_obj_t bitmap_data);
void common_hal_rm690b0_rm690b0_blit_bmp(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_obj_t bmp_data);
void common_hal_rm690b0_rm690b0_blit_jpeg(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_obj_t jpeg_data);
void common_hal_rm690b0_rm690b0_swap_buffers(rm690b0_rm690b0_obj_t *self, bool copy);
void common_hal_rm690b0_rm690b0_deinit_all(void);
void common_hal_rm690b0_rm690b0_set_font(rm690b0_rm690b0_obj_t *self, mp_int_t font_id);
void common_hal_rm690b0_rm690b0_text(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y,
    const char *text, size_t text_len, uint16_t fg, bool has_bg, uint16_t bg);
