// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/rm690b0/RM690B0.h"
#include "esp-idf/components/esp_lcd/include/esp_lcd_types.h"

void common_hal_rm690b0_rm690b0_pixel(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t color);
void common_hal_rm690b0_rm690b0_fill_rect(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, mp_int_t color);
void common_hal_rm690b0_rm690b0_hline(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t color);
void common_hal_rm690b0_rm690b0_vline(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t height, mp_int_t color);
void common_hal_rm690b0_rm690b0_line(rm690b0_rm690b0_obj_t *self, mp_int_t x0, mp_int_t y0, mp_int_t x1, mp_int_t y1, mp_int_t color);
void common_hal_rm690b0_rm690b0_rect(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, mp_int_t color);
void common_hal_rm690b0_rm690b0_circle(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t radius, mp_int_t color);
void common_hal_rm690b0_rm690b0_fill_circle(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t radius, mp_int_t color);
void common_hal_rm690b0_rm690b0_set_rotation(rm690b0_rm690b0_obj_t *self, mp_int_t degrees);
void common_hal_rm690b0_rm690b0_set_brightness(rm690b0_rm690b0_obj_t *self, mp_float_t value);
void common_hal_rm690b0_rm690b0_blit_buffer(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, mp_obj_t bitmap_data, bool dest_is_swapped);
void common_hal_rm690b0_rm690b0_blit_bmp(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_obj_t bmp_data);
void common_hal_rm690b0_rm690b0_blit_jpeg(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_obj_t jpeg_data);
void common_hal_rm690b0_rm690b0_deinit_all(void);

// Helper function to expose panel handle for LVGL integration
esp_lcd_panel_handle_t rm690b0_get_panel_handle(void);
