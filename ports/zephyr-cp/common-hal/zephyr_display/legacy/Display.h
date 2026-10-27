// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include "py/obj.h"
#include "shared-module/displayio/display_core.h"

typedef struct {
    mp_obj_base_t base;
    displayio_display_core_t core;
    const struct device *device;
    struct display_capabilities capabilities;
    enum display_pixel_format pixel_format;
    uint64_t last_refresh_call;
    uint16_t native_frames_per_second;
    uint16_t native_ms_per_frame;
    bool auto_refresh;
    bool first_manual_refresh;
} zephyr_display_display_obj_t;

void zephyr_display_display_background(zephyr_display_display_obj_t *self);
void zephyr_display_display_collect_ptrs(zephyr_display_display_obj_t *self);
void zephyr_display_display_reset(zephyr_display_display_obj_t *self);
void release_zephyr_display(zephyr_display_display_obj_t *self);
