// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

#include "lvgl.h"

// Descriptor-level invalidation. An image descriptor can be drawn by several lv_objs (an
// OnDiskBitmap hands one descriptor per decode mode to every TileGrid that shows the file),
// so the owner keeps the list of objects to invalidate when the descriptor's content changes.
//
// The composed TileGrid path doesn't need this: its descriptors belong to the TileGrid, which
// invalidates its own objects.

typedef struct displayio_lvgl_image_root {
    lv_obj_t *obj;
    struct displayio_lvgl_image_root *next;
} displayio_lvgl_image_root_t;

// Add/remove an object that draws a descriptor. Idempotent. Caller holds the LVGL lock.
void displayio_lvgl_image_root_add(displayio_lvgl_image_root_t **roots, lv_obj_t *obj);
void displayio_lvgl_image_root_remove(displayio_lvgl_image_root_t **roots, lv_obj_t *obj);
// Invalidate every object that draws the descriptor. Caller holds the LVGL lock.
void displayio_lvgl_image_invalidate_all(displayio_lvgl_image_root_t *roots,
    const lv_image_dsc_t *dsc);
