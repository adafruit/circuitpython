// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

// Don't need anything in here yet

#include "shared-bindings/vectorio/__init__.h"
#include "py/runtime.h"
#include "stdlib.h"
#include <math.h>

bool common_hal_vectorio_circle_rectangle_intersects(
    int16_t cx, int16_t cy, int16_t cr,
    int16_t rx, int16_t ry, int16_t rw, int16_t rh){

    mp_int_t rect_left = rx;
    mp_int_t rect_right = rect_left + rw;
    mp_int_t rect_top = ry;
    mp_int_t rect_bottom = rect_top + rh;

    mp_int_t test_x = cx;
    mp_int_t test_y = cy;

    if (cx < rect_left) {
        test_x = rect_left;
    } else if (cx > rect_right) {
        test_x = rect_right;
    }

    if (cy < rect_top) {
        test_y = rect_top;
    } else if (cy > rect_bottom) {
        test_y = rect_bottom;
    }

    mp_int_t dist_x = cx - test_x;
    mp_int_t dist_y = cy - test_y;
    mp_int_t dist = (dist_x * dist_x) + (dist_y * dist_y);

    return dist <= cr*cr;
}

bool common_hal_vectorio_circle_circle_intersects(
    int16_t c1x, int16_t c1y, int16_t c1r,
    int16_t c2x, int16_t c2y, int16_t c2r
){

    mp_int_t dist_x = c1x - c2x;
    mp_int_t dist_y = c1y - c2y;

    mp_int_t dist = (dist_x * dist_x) + (dist_y * dist_y);

    return dist <= (c1r + c2r) * (c1r + c2r);

}

bool common_hal_vectorio_rectangle_rectangle_intersects(
    int16_t r1x, int16_t r1y, int16_t r1w, int16_t r1h,
    int16_t r2x, int16_t r2y, int16_t r2w, int16_t r2h){


    mp_int_t r1_left = r1x;
    mp_int_t r1_right = r1_left + r1w;
    mp_int_t r1_top = r1y;
    mp_int_t r1_bottom = r1_top + r1h;

    mp_int_t r2_left = r2x;
    mp_int_t r2_right = r2_left + r2w;
    mp_int_t r2_top = r2y;
    mp_int_t r2_bottom = r2_top + r2h;

    return r1_left < r2_right && r1_right > r2_left &&
           r1_top < r2_bottom && r1_bottom > r2_top;
}