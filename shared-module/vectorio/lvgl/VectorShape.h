// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

#include "shared-module/displayio/lvgl/lvgl_protocol.h"

#include "lvgl.h"

// LVGL compositor state for a vector shape.
//
// Each shape is mirrored into exactly one lv_obj, so LVGL owns drawing, clipping and
// invalidation. There are no dirty areas, no masks and no screen-space footprint here;
// that bookkeeping only exists because the scanline compositor has no invalidation.
//
//   Rectangle, Circle - a plain lv_obj drawn entirely with styles (background color and
//     opacity, plus LV_RADIUS_CIRCLE for circles). No draw callback and no per-shape
//     pixel memory.
//   Polygon - an lv_image whose source is an A8 coverage mask rasterized from the shape's
//     winding rule, tinted with the image recolor styles. The mask is rebuilt when the
//     shape changes, so a frame costs one A8 blend.
//
// Group scale is applied by scaling the mirror's position, size and mask resolution
// instead of using lv_obj transform styles. Transform styles route the object through an
// ARGB8888 transform layer that cannot be sliced, which would defeat partial redraw and
// replace displayio's integer nearest-neighbor scaling with bilinear filtering. See
// shared-module/displayio/lvgl/Group.c for the same reasoning applied to TileGrids.

typedef struct {
    mp_obj_base_t base;
    vectorio_ishape_t ishape;
    mp_obj_t pixel_shader;
    int16_t x;
    int16_t y;
    // Absolute position of the parent group's origin plus the group scale inherited from
    // it. Groups are screen sized objects at the screen origin, so the mirror is placed
    // in screen coordinates from these.
    int32_t abs_x;
    int32_t abs_y;
    uint16_t abs_scale;
    lv_obj_t *lv_obj;    // Mirror: lv_obj for rectangles and circles, lv_image for polygons.
    lv_draw_buf_t *mask; // Polygon coverage (A8), NULL for the style-drawn shapes.
    bool hidden : 1;
    bool hidden_by_parent : 1;
    bool in_group : 1;
    uint8_t padding : 5;
} vectorio_vector_shape_t;

// Mirror lifecycle. Implemented in shared-module/vectorio/lvgl/VectorShape.c and called
// from shared-module/displayio/lvgl/Group.c, which owns the object tree.
void displayio_lvgl_vector_shape_attach(vectorio_vector_shape_t *self, lv_obj_t *parent);
void displayio_lvgl_vector_shape_detach(vectorio_vector_shape_t *self);
void displayio_lvgl_vector_shape_set_transform(vectorio_vector_shape_t *self, int32_t abs_x,
    int32_t abs_y, uint16_t scale);
void displayio_lvgl_vector_shape_set_hidden_by_parent(vectorio_vector_shape_t *self, bool hidden);
// A pixel shader this shape registered with has changed.
void displayio_lvgl_vector_shape_shader_changed(vectorio_vector_shape_t *self);

// How Group attaches, positions and hides a shape. See lvgl_protocol.h.
extern const lvgl_protocol_t displayio_lvgl_vector_shape_protocol;
