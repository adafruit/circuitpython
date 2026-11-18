// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

// Everything a vectorio shape has to provide to the compositor, independent of backend:
//
//   get_pixel(x, y) - the shape's coverage test, in the shape's own coordinate space where
//     (0, 0) is the shape's origin. Returns 0 for "not covered" and the shape's color
//     index biased by one otherwise. (The bias keeps 0, which means "not covered", from
//     ever naming a palette entry.)
//
// Everything else is backend specific:
//
//   legacy/ - the scanline compositor. Shapes report a bounding area, the compositor fills
//     screen spans through get_pixel(), and shapes track dirty areas so the display core
//     knows what to repaint.
//   lvgl/   - the LVGL compositor. Shapes are mirrored into lv_obj widgets that LVGL draws
//     and invalidates itself, so there is no area and no span filling at all.
typedef uint32_t get_pixel_function(mp_obj_t shape, int16_t x, int16_t y);

#if !CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/displayio/area.h"

typedef void get_area_function(mp_obj_t shape, displayio_area_t *out_area);
#endif

// This struct binds a shape's support functions to its instance pointer. A shape's type is
// only inspected when its vector shape is constructed; after that the compositor can call
// these unconditionally. That keeps the shape implementations and the vector shape
// wrapper responsible for their own pieces.
typedef struct {
    mp_obj_t shape;
    #if !CIRCUITPY_DISPLAYIO_LVGL
    get_area_function *get_area;
    #endif
    get_pixel_function *get_pixel;
} vectorio_ishape_t;

#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/vectorio/lvgl/VectorShape.h"
// The protocol slot an LVGL-backed shape exposes to its Group. Legacy shapes are found by
// type through protocol_draw, so the slot stays empty there. See
// displayio/lvgl/lvgl_protocol.h.
#define VECTORIO_VECTOR_SHAPE_LVGL_PROTOCOL &displayio_lvgl_vector_shape_protocol
#else
#include "shared-module/vectorio/legacy/VectorShape.h"
#define VECTORIO_VECTOR_SHAPE_LVGL_PROTOCOL NULL
#endif
