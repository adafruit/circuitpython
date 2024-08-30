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


//| def circle_rectangle_intersects(
//|     cx: int, cy: int, cr: int, rx: int, ry: int, rw: int, rh: int
//|     :param int cx: Circle center x coordinate
//|     :param int cy: Circle center y coordinate
//|     :param int cr: Circle radius
//|     :param int rx: Rectangle x coordinate
//|     :param int ry: Rectangle y coordinate
//|     :param int rw: Rectangle width
//|     :param int rh: Rectangle height
//| ) -> None:
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
    if (result){
        return mp_const_true;
    }else{
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_circle_rectangle_intersects_obj, 0, vectorio_circle_rectangle_intersects);


//| def rectangle_rectangle_intersects(
//|     r1x: int, r1y: int, r1w: int, r1h: int,
//      r2x: int, r2y: int, r2w: int, r2h: int
//|
//|     :param int r1x: Rectangle x coordinate
//|     :param int r1y: Rectangle y coordinate
//|     :param int r1w: Rectangle width
//|     :param int r1h: Rectangle height
//|     :param int r2x: Other Rectangle x coordinate
//|     :param int r2y: Other Rectangle y coordinate
//|     :param int r2w: Other Rectangle width
//|     :param int r2h: Other Rectangle height
//| ) -> None:
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
    if (result){
        return mp_const_true;
    }else{
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_rectangle_rectangle_intersects_obj, 0, vectorio_rectangle_rectangle_intersects);


//| def circle_circle_intersects(
//|     cx: int, cy: int, cr: int, rx: int, ry: int, rw: int, rh: int
//|     :param int c1x: Circle center x coordinate
//|     :param int c1y: Circle center y coordinate
//|     :param int c1r: Circle radius
//|     :param int c2x: Other Circle center x coordinate
//|     :param int c2y: Other Circle center y coordinate
//|     :param int c2r: Other Circle radius
//| ) -> None:
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
    if (result){
        return mp_const_true;
    }else{
        return mp_const_false;
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(vectorio_circle_circle_intersects_obj, 0, vectorio_circle_circle_intersects);

static const mp_rom_map_elem_t vectorio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_vectorio) },
    { MP_ROM_QSTR(MP_QSTR_circle_rectangle_intersects), MP_ROM_PTR(&vectorio_circle_rectangle_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle_circle_intersects), MP_ROM_PTR(&vectorio_circle_circle_intersects_obj) },
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
