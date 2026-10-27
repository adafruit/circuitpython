// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "lvgl.h"

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-module/displayio/bus_core.h"
#if CIRCUITPY_PWMIO
#include "shared-bindings/pwmio/PWMOut.h"
#endif

// LVGL-backed BusDisplay. There is no display_core/bus_core; the display owns an
// lv_display_t and keeps the display bus as an mp_obj_t so the GC can see it.
typedef struct {
    mp_obj_base_t base;
    // Bus the display is attached to (fourwire, etc.).
    displayio_display_bus_t bus;
    // LVGL display handle - owns draw buffers and flush CB
    lv_display_t *lv_display;
    lv_color_format_t lv_color_format;
    void *lv_buf1;
    void *lv_buf2;
    size_t lv_buf_size;
    void *lv_rotation_buf;
    size_t lv_rotation_buf_size;
    displayio_group_t *root_group;
    // Display geometry / commands retained for flush
    uint16_t width;
    uint16_t height;
    uint16_t rotation;
    uint16_t color_depth;
    _displayio_colorspace_t colorspace;
    bool grayscale;
    bool pixels_in_byte_share_row;
    uint8_t bytes_per_cell;
    bool reverse_pixels_in_byte;
    bool reverse_bytes_in_word;
    uint8_t write_ram_command;
    busdisplay_backlight_t backlight;
    uint64_t last_refresh_call;
    mp_float_t current_brightness;
    uint16_t brightness_command;
    uint16_t native_frames_per_second;
    uint16_t native_ms_per_frame;
    bool auto_refresh;
    bool first_manual_refresh;
    bool backlight_on_high;
    bool deinited;
} busdisplay_busdisplay_obj_t;

void busdisplay_busdisplay_background(busdisplay_busdisplay_obj_t *self);
void release_busdisplay(busdisplay_busdisplay_obj_t *self);
void reset_busdisplay(busdisplay_busdisplay_obj_t *self);
void busdisplay_busdisplay_collect_ptrs(busdisplay_busdisplay_obj_t *self);
