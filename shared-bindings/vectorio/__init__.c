// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"


#include "shared-bindings/vectorio/Circle.h"
#include "shared-bindings/vectorio/Polygon.h"
#include "shared-bindings/vectorio/Rectangle.h"
#include "shared-bindings/vectorio/__init__.h"

//| """Lightweight 2D shapes for displays
//|
//| The :py:attr:`vectorio` module provide simple filled drawing primitives for
//| use with `displayio`.
//|
//| .. code-block:: python
//|
//|     group = displayio.Group()
//|
//|     palette = displayio.Palette(1)
//|     palette[0] = 0x125690
//|
//|     circle = vectorio.Circle(pixel_shader=palette, radius=25, x=70, y=40)
//|     group.append(circle)
//|
//|     rectangle = vectorio.Rectangle(pixel_shader=palette, width=40, height=30, x=55, y=45)
//|     group.append(rectangle)
//|
//|     points=[(5, 5), (100, 20), (20, 20), (20, 100)]
//|     polygon = vectorio.Polygon(pixel_shader=palette, points=points, x=0, y=0)
//|     group.append(polygon)
//|
//| """
//|


//| def circle_rectangle_intersects(
//|     cx: int, cy: int, cr: int, rx: int, ry: int, rw: int, rh: int
//| ) -> bool:
//|     """Checks for intersection between a cricle and a rectangle.
//|
//|     :param int cx: Circle center x coordinate
//|     :param int cy: Circle center y coordinate
//|     :param int cr: Circle radius
//|     :param int rx: Rectangle x coordinate
//|     :param int ry: Rectangle y coordinate
//|     :param int rw: Rectangle width
//|     :param int rh: Rectangle height"""
//|     ...
//|
static mp_obj_t vectorio_circle_rectangle_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_cx, ARG_cy, ARG_cr, ARG_rx, ARG_ry, ARG_rw, ARG_rh};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_cx, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cy, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rx, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_ry, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rw, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rh, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int16_t cx = args[ARG_cx].u_int;
    int16_t cy = args[ARG_cy].u_int;
    int16_t cr = args[ARG_cr].u_int;
    int16_t rx = args[ARG_rx].u_int;
    int16_t ry = args[ARG_ry].u_int;
    int16_t rw = args[ARG_rw].u_int;
    int16_t rh = args[ARG_rh].u_int;

    bool result = common_hal_vectorio_circle_rectangle_intersects(cx, cy, cr, rx, ry, rw, rh);
    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_circle_rectangle_intersects_obj, 0, vectorio_circle_rectangle_intersects);


//| def rectangle_rectangle_intersects(
//|     r1x: int, r1y: int, r1w: int, r1h: int, r2x: int, r2y: int, r2w: int, r2h: int
//| ) -> bool:
//|     """Checks for intersection between a two rectangles.
//|
//|     :param int r1x: First Rectangle x coordinate
//|     :param int r1y: First Rectangle y coordinate
//|     :param int r1w: First Rectangle width
//|     :param int r1h: First Rectangle height
//|     :param int r2x: Second Rectangle x coordinate
//|     :param int r2y: Second Rectangle y coordinate
//|     :param int r2w: Second Rectangle width
//|     :param int r2h: Second Rectangle height"""
//|     ...
//|
static mp_obj_t vectorio_rectangle_rectangle_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_r1x, ARG_r1y, ARG_r1w, ARG_r1h, ARG_r2x, ARG_r2y, ARG_r2w, ARG_r2h};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_r1x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_r1y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_r1w, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_r1h, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_r2x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_r2y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_r2w, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_r2h, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int16_t r1x = args[ARG_r1x].u_int;
    int16_t r1y = args[ARG_r1y].u_int;
    int16_t r1w = args[ARG_r1w].u_int;
    int16_t r1h = args[ARG_r1h].u_int;

    int16_t r2x = args[ARG_r2x].u_int;
    int16_t r2y = args[ARG_r2y].u_int;
    int16_t r2w = args[ARG_r2w].u_int;
    int16_t r2h = args[ARG_r2h].u_int;


    bool result = common_hal_vectorio_rectangle_rectangle_intersects(r1x, r1y, r1w, r1h, r2x, r2y, r2w, r2h);
    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_rectangle_rectangle_intersects_obj, 0, vectorio_rectangle_rectangle_intersects);

//| def circle_circle_intersects(
//|     c1x: int, c1y: int, c1r: int, c2x: int, c2y: int, c2r: int
//| ) -> bool:
//|     """Checks for intersection between two circles.
//|
//|     :param int c1x: First Circle center x coordinate
//|     :param int c1y: First Circle center y coordinate
//|     :param int c1r: First Circle radius
//|     :param int c2x: Second Circle center x coordinate
//|     :param int c2y: Second Circle center y coordinate
//|     :param int c2r: Second Circle radius"""
//|     ...
//|
static mp_obj_t vectorio_circle_circle_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_c1x, ARG_c1y, ARG_c1r, ARG_c2x, ARG_c2y, ARG_c2r};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_c1x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_c1y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_c1r, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_c2x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_c2y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_c2r, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int16_t c1x = args[ARG_c1x].u_int;
    int16_t c1y = args[ARG_c1y].u_int;
    int16_t c1r = args[ARG_c1r].u_int;
    int16_t c2x = args[ARG_c2x].u_int;
    int16_t c2y = args[ARG_c2y].u_int;
    int16_t c2r = args[ARG_c2r].u_int;

    bool result = common_hal_vectorio_circle_circle_intersects(c1x, c1y, c1r, c2x, c2y, c2r);
    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_circle_circle_intersects_obj, 0, vectorio_circle_circle_intersects);

//| def circle_contains_point(cx: int, cy: int, cr: int, px: int, py: int) -> bool:
//|     """Checks whether a circle contains the given point
//|
//|     :param int cx: Circle center x coordinate
//|     :param int cy: Circle center y coordinate
//|     :param int cr: Circle radius
//|     :param int px: Point x coordinate
//|     :param int py: Point y coordinate"""
//|     ...
//|
static mp_obj_t vectorio_circle_contains_point(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_cx, ARG_cy, ARG_cr, ARG_px, ARG_py};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_cx, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cy, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_px, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_py, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int16_t cx = args[ARG_cx].u_int;
    int16_t cy = args[ARG_cy].u_int;
    int16_t cr = args[ARG_cr].u_int;
    int16_t px = args[ARG_px].u_int;
    int16_t py = args[ARG_py].u_int;

    bool result = common_hal_vectorio_circle_contains_point(cx, cy, cr, px, py);
    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_circle_contains_point_obj, 0, vectorio_circle_contains_point);

//| def rectangle_contains_point(rx: int, ry: int, rw: int, rh: int, px: int, py: int) -> bool:
//|     """Checks whether a rectangle contains the given point
//|
//|     :param int rx: Rectangle x coordinate
//|     :param int ry: Rectangle y coordinate
//|     :param int rw: Rectangle width
//|     :param int rh: Rectangle height
//|     :param int px: Point x coordinate
//|     :param int py: Point y coordinate"""
//|     ...
//|
static mp_obj_t vectorio_rectangle_contains_point(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_rx, ARG_ry, ARG_rw, ARG_rh, ARG_px, ARG_py};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_rx, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_ry, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rw, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rh, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_px, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_py, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int16_t rx = args[ARG_rx].u_int;
    int16_t ry = args[ARG_ry].u_int;
    int16_t rw = args[ARG_rw].u_int;
    int16_t rh = args[ARG_rh].u_int;
    int16_t px = args[ARG_px].u_int;
    int16_t py = args[ARG_py].u_int;

    bool result = common_hal_vectorio_rectangle_contains_point(rx, ry, rw, rh, px, py);
    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_rectangle_contains_point_obj, 0, vectorio_rectangle_contains_point);


//| def line_contains_point(x1: int, y1: int, x2: int, y2: int, px: int, py: int) -> bool:
//|     """Checks whether a line contains the given point
//|
//|     :param int x1: Line x1 coordinate
//|     :param int y1: Line y1 coordinate
//|     :param int x2: Line x2 coordinate
//|     :param int y2: Line y2 coordinate
//|     :param int px: Point x coordinate
//|     :param int py: Point y coordinate
//|     :param float padding: Extra padding outside of the line to consider as positive intersection
//|     """
//|     ...
//|
static mp_obj_t vectorio_line_contains_point(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_px, ARG_py, ARG_padding};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_px, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_py, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_padding, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to 0.0
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int16_t x1 = args[ARG_x1].u_int;
    int16_t y1 = args[ARG_y1].u_int;
    int16_t x2 = args[ARG_x2].u_int;
    int16_t y2 = args[ARG_y2].u_int;
    int16_t px = args[ARG_px].u_int;
    int16_t py = args[ARG_py].u_int;

    // Use default padding if None was passed
    mp_float_t padding = 0.0;
    if (args[ARG_padding].u_obj != mp_const_none) {
        padding = mp_obj_get_float(args[ARG_padding].u_obj);
    }

    bool result = common_hal_vectorio_line_contains_point(x1, y1, x2, y2, px, py, padding);
    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_line_contains_point_obj, 0, vectorio_line_contains_point);


//| def line_circle_intersects(
//|     x1: int,
//|     y1: int,
//|     x2: int,
//|     y2: int,
//|     cx: int,
//|     cy: int,
//|     cr: int,
//|     padding: float
//| ) -> bool:
//|     """Checks whether a line intersects with a circle
//|
//|     :param int x1: Line x1 coordinate
//|     :param int y1: Line y1 coordinate
//|     :param int x2: Line x2 coordinate
//|     :param int y2: Line y2 coordinate
//|     :param int cx: Circle center x coordinate
//|     :param int cy: Circle center y coordinate
//|     :param int cr: Circle radius
//|     :param float padding: Extra padding outside of the line to consider as positive intersection
//|     """
//|     ...
//|
static mp_obj_t vectorio_line_circle_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_cx, ARG_cy, ARG_cr, ARG_padding};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cx, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cy, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_padding, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to 0.0
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int16_t x1 = args[ARG_x1].u_int;
    int16_t y1 = args[ARG_y1].u_int;
    int16_t x2 = args[ARG_x2].u_int;
    int16_t y2 = args[ARG_y2].u_int;
    int16_t cx = args[ARG_cx].u_int;
    int16_t cy = args[ARG_cy].u_int;
    int16_t cr = args[ARG_cr].u_int;

    // Use default padding if None was passed
    mp_float_t padding = 0.0;
    if (args[ARG_padding].u_obj != mp_const_none) {
        padding = mp_obj_get_float(args[ARG_padding].u_obj);
    }

    bool result = common_hal_vectorio_line_circle_intersects(x1, y1, x2, y2, cx, cy, cr, padding);

    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_line_circle_intersects_obj, 0, vectorio_line_circle_intersects);


//| def polygon_circle_intersects(
//|     points: List[Tuple[int, int]],
//|     polygon_x: int,
//|     polygon_y: int,
//|     cx: int,
//|     cy: int,
//|     cr: int,
//|     padding: float,
//| ) -> bool:
//|     """Checks for intersection between a polygon and a cricle.
//|
//|     :param List[Tuple[int,int]] points: Vertices for the polygon
//|     :param int polygon_x: Polygon x coordinate. All other polygon points are relative to this
//|     :param int polygon_y: Polygon y coordinate. All other polygon points are relative to this
//|     :param int cx: Circle center x coordinate
//|     :param int cy: Circle center y coordinate
//|     :param int cr: Circle radius
//|     :param float padding: Extra padding outside of the line to consider as positive intersection
//|     """
//|     ...
//|
static mp_obj_t vectorio_polygon_circle_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_points_list, ARG_polygon_x, ARG_polygon_y, ARG_cx, ARG_cy, ARG_cr, ARG_padding};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_points, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_polygon_x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_polygon_y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cx, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cy, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_cr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_padding, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to 0.0
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t points_list = mp_arg_validate_type(args[ARG_points_list].u_obj, &mp_type_list, MP_QSTR_points);

    int16_t polygon_x = args[ARG_polygon_x].u_int;
    int16_t polygon_y = args[ARG_polygon_y].u_int;

    int16_t cx = args[ARG_cx].u_int;
    int16_t cy = args[ARG_cy].u_int;
    int16_t cr = args[ARG_cr].u_int;

    // Use default padding if None was passed
    mp_float_t padding = 0.0;
    if (args[ARG_padding].u_obj != mp_const_none) {
        padding = mp_obj_get_float(args[ARG_padding].u_obj);
    }

    bool result = common_hal_vectorio_polygon_circle_intersects(
        points_list, polygon_x, polygon_y,
        cx, cy, cr, padding
        );

    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_polygon_circle_intersects_obj, 0, vectorio_polygon_circle_intersects);


//| def line_line_intersects(
//|     x1: int,
//|     y1: int,
//|     x2: int,
//|     y2: int,
//|     x3: int,
//|     y3: int,
//|     x4: int,
//|     y4: int,
//| ) -> bool:
//|     """Checks whether a line intersects with another line
//|
//|     :param int x1: Line x1 coordinate
//|     :param int y1: Line y1 coordinate
//|     :param int x2: Line x2 coordinate
//|     :param int y2: Line y2 coordinate
//|     :param int x3: Other Line x3 coordinate
//|     :param int y3: Other Line y3 coordinate
//|     :param int x4: Other Line x4 coordinate
//|     :param int y4: Other Line y4 coordinate"""
//|     ...
//|
static mp_obj_t vectorio_line_line_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_x3, ARG_y3, ARG_x4, ARG_y4 };

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x3, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y3, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x4, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y4, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int16_t x1 = args[ARG_x1].u_int;
    int16_t y1 = args[ARG_y1].u_int;
    int16_t x2 = args[ARG_x2].u_int;
    int16_t y2 = args[ARG_y2].u_int;
    int16_t x3 = args[ARG_x3].u_int;
    int16_t y3 = args[ARG_y3].u_int;
    int16_t x4 = args[ARG_x4].u_int;
    int16_t y4 = args[ARG_y4].u_int;

    bool result = common_hal_vectorio_line_line_intersects(x1, y1, x2, y2, x3, y3, x4, y4);

    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_line_line_intersects_obj, 0, vectorio_line_line_intersects);


//| def line_rectangle_intersects(
//|     x1: int, y1: int, x2: int, y2: int, rx: int, ry: int, rw: int, rh: int
//| ) -> bool:
//|     """Checks whether a line intersects with another line
//|
//|     :param int x1: Line x1 coordinate
//|     :param int y1: Line y1 coordinate
//|     :param int x2: Line x2 coordinate
//|     :param int y2: Line y2 coordinate
//|     :param int rx: Rectangle x coordinate
//|     :param int ry: Rectangle y coordinate
//|     :param int rw: Rectangle width
//|     :param int rh: Rectangle height"""
//|     ...
//|
static mp_obj_t vectorio_line_rectangle_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_rx, ARG_ry, ARG_rw, ARG_rh };

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_x2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_y2, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rx, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_ry, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rw, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rh, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int16_t x1 = args[ARG_x1].u_int;
    int16_t y1 = args[ARG_y1].u_int;
    int16_t x2 = args[ARG_x2].u_int;
    int16_t y2 = args[ARG_y2].u_int;
    int16_t rx = args[ARG_rx].u_int;
    int16_t ry = args[ARG_ry].u_int;
    int16_t rw = args[ARG_rw].u_int;
    int16_t rh = args[ARG_rh].u_int;

    bool result = common_hal_vectorio_line_rectangle_intersects(x1, y1, x2, y2, rx, ry, rw, rh);

    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_line_rectangle_intersects_obj, 0, vectorio_line_rectangle_intersects);


//| def polygon_rectangle_intersects(
//|     points: List[Tuple[int, int]],
//|     polygon_x: int,
//|     polygon_y: int,
//|     rx: int,
//|     ry: int,
//|     rw: int,
//|     rh: int,
//| ) -> bool:
//|     """Checks for intersection between a polygon and a cricle.
//|
//|     :param List[Tuple[int,int]] points: Vertices for the polygon
//|     :param int polygon_x: Polygon x coordinate. All other polygon points are relative to this
//|     :param int polygon_y: Polygon y coordinate. All other polygon points are relative to this
//|     :param int rx: Rectangle x coordinate
//|     :param int ry: Rectangle y coordinate
//|     :param int rw: Rectangle width
//|     :param int rh: Rectangle height"""
//|     ...
//|
static mp_obj_t vectorio_polygon_rectangle_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_points_list, ARG_polygon_x, ARG_polygon_y, ARG_rx, ARG_ry, ARG_rw, ARG_rh};

    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_points, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_polygon_x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_polygon_y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rx, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_ry, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rw, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_rh, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_padding, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} }, // None convert to 0.0
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t points_list = mp_arg_validate_type(args[ARG_points_list].u_obj, &mp_type_list, MP_QSTR_points);

    int16_t polygon_x = args[ARG_polygon_x].u_int;
    int16_t polygon_y = args[ARG_polygon_y].u_int;

    int16_t rx = args[ARG_rx].u_int;
    int16_t ry = args[ARG_ry].u_int;
    int16_t rw = args[ARG_rw].u_int;
    int16_t rh = args[ARG_rh].u_int;

    bool result = common_hal_vectorio_polygon_rectangle_intersects(
        points_list, polygon_x, polygon_y,
        rx, ry, rw, rh
        );

    if (result) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_polygon_rectangle_intersects_obj, 0, vectorio_polygon_rectangle_intersects);


static const mp_rom_map_elem_t vectorio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_vectorio) },
    { MP_ROM_QSTR(MP_QSTR_circle_rectangle_intersects), MP_ROM_PTR(&vectorio_circle_rectangle_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_polygon_circle_intersects), MP_ROM_PTR(&vectorio_polygon_circle_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle_circle_intersects), MP_ROM_PTR(&vectorio_circle_circle_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_line_circle_intersects), MP_ROM_PTR(&vectorio_line_circle_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_line_line_intersects), MP_ROM_PTR(&vectorio_line_line_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_line_rectangle_intersects), MP_ROM_PTR(&vectorio_line_rectangle_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_polygon_rectangle_intersects), MP_ROM_PTR(&vectorio_polygon_rectangle_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle_contains_point), MP_ROM_PTR(&vectorio_circle_contains_point_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle_contains_point), MP_ROM_PTR(&vectorio_rectangle_contains_point_obj) },
    { MP_ROM_QSTR(MP_QSTR_line_contains_point), MP_ROM_PTR(&vectorio_line_contains_point_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle_rectangle_intersects), MP_ROM_PTR(&vectorio_rectangle_rectangle_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_Circle), MP_ROM_PTR(&vectorio_circle_type) },
    { MP_ROM_QSTR(MP_QSTR_Polygon), MP_ROM_PTR(&vectorio_polygon_type) },
    { MP_ROM_QSTR(MP_QSTR_Rectangle), MP_ROM_PTR(&vectorio_rectangle_type) },
};

static MP_DEFINE_CONST_DICT(vectorio_module_globals, vectorio_module_globals_table);

const mp_obj_module_t vectorio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&vectorio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_vectorio, vectorio_module);
