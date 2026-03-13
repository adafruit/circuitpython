// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/Group.h"
#include "common-hal/zephyr_display/Display.h"

extern const mp_obj_type_t zephyr_display_display_type;

#define NO_FPS_LIMIT 0xffffffff

void common_hal_zephyr_display_display_construct_from_device(zephyr_display_display_obj_t *self,
    const struct device *device,
    uint16_t rotation,
    bool auto_refresh);

bool common_hal_zephyr_display_display_refresh(zephyr_display_display_obj_t *self,
    uint32_t target_ms_per_frame,
    uint32_t maximum_ms_per_real_frame);

bool common_hal_zephyr_display_display_get_auto_refresh(zephyr_display_display_obj_t *self);
void common_hal_zephyr_display_display_set_auto_refresh(zephyr_display_display_obj_t *self, bool auto_refresh);

uint16_t common_hal_zephyr_display_display_get_width(zephyr_display_display_obj_t *self);
uint16_t common_hal_zephyr_display_display_get_height(zephyr_display_display_obj_t *self);
uint16_t common_hal_zephyr_display_display_get_rotation(zephyr_display_display_obj_t *self);
void common_hal_zephyr_display_display_set_rotation(zephyr_display_display_obj_t *self, int rotation);

mp_float_t common_hal_zephyr_display_display_get_brightness(zephyr_display_display_obj_t *self);
bool common_hal_zephyr_display_display_set_brightness(zephyr_display_display_obj_t *self, mp_float_t brightness);

mp_obj_t common_hal_zephyr_display_display_get_root_group(zephyr_display_display_obj_t *self);
bool common_hal_zephyr_display_display_set_root_group(zephyr_display_display_obj_t *self, displayio_group_t *root_group);
