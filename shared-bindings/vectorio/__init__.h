// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"

#include "shared-module/displayio/area.h"
#include "shared-module/displayio/Palette.h"

// Returns the object on which the rest of the draw protocol methods are invoked.
typedef mp_obj_t (*draw_get_protocol_self_fun)(mp_obj_t protocol_container);

typedef bool (*draw_fill_area_fun)(mp_obj_t draw_protocol_self, const _displayio_colorspace_t *colorspace, const displayio_area_t *area, uint32_t *mask, uint32_t *buffer);
typedef bool (*draw_get_dirty_area_fun)(mp_obj_t draw_protocol_self, displayio_area_t *current_dirty_area);
typedef void (*draw_update_transform_fun)(mp_obj_t draw_protocol_self, displayio_buffer_transform_t *group_transform);
typedef void (*draw_finish_refresh_fun)(mp_obj_t draw_protocol_self);
typedef void (*draw_set_dirty_fun)(mp_obj_t draw_protocol_self);
typedef displayio_area_t *(*draw_get_refresh_areas_fun)(mp_obj_t draw_protocol_self, displayio_area_t *tail);

typedef struct _vectorio_draw_protocol_impl_t {
    draw_fill_area_fun draw_fill_area;
    draw_get_dirty_area_fun draw_get_dirty_area;
    draw_update_transform_fun draw_update_transform;
    draw_finish_refresh_fun draw_finish_refresh;
    draw_get_refresh_areas_fun draw_get_refresh_areas;
    draw_set_dirty_fun draw_set_dirty;
} vectorio_draw_protocol_impl_t;

// Draw protocol
typedef struct _vectorio_draw_protocol_t {
    MP_PROTOCOL_HEAD // MP_QSTR_protocol_draw

    // Instance of the draw protocol
    draw_get_protocol_self_fun draw_get_protocol_self;

    // Implementation functions for the draw protocol
    vectorio_draw_protocol_impl_t *draw_protocol_impl;
} vectorio_draw_protocol_t;

bool common_hal_vectorio_circle_rectangle_intersects(
    int16_t cx, int16_t cy, int16_t cr,
    int16_t rx, int16_t ry, int16_t rw, int16_t rh);

bool common_hal_vectorio_rectangle_rectangle_intersects(
    int16_t r1x, int16_t r1y, int16_t r1w, int16_t r1h,
    int16_t r2x, int16_t r2y, int16_t r2w, int16_t r2h);

bool common_hal_vectorio_circle_circle_intersects(
    int16_t c1x, int16_t c1y, int16_t c1r,
    int16_t c2x, int16_t c2y, int16_t c2r);

bool common_hal_vectorio_circle_contains_point(
    int16_t cx, int16_t cy, int16_t cr,
    int16_t px, int16_t py);

bool common_hal_vectorio_rectangle_contains_point(
    int16_t rx, int16_t ry, int16_t rw, int16_t rh,
    int16_t px, int16_t py);

bool common_hal_vectorio_line_contains_point(
    int16_t x1, int16_t y1, int16_t x2, int16_t y2,
    int16_t px, int16_t py, mp_float_t padding);

bool common_hal_vectorio_polygon_circle_intersects(
	mp_obj_t points_list, int16_t cx, int16_t cy, int16_t cr);
