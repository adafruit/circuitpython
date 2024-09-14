// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

// Don't need anything in here yet

#include "shared-bindings/vectorio/__init__.h"
#include "shared-module/vectorio/__init__.h"

#include "py/runtime.h"
#include "stdlib.h"
#include <math.h>

bool common_hal_vectorio_circle_rectangle_intersects(
    int16_t cx, int16_t cy, int16_t cr,
    int16_t rx, int16_t ry, int16_t rw, int16_t rh) {

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

    return dist <= cr * cr;
}

bool common_hal_vectorio_circle_circle_intersects(
    int16_t c1x, int16_t c1y, int16_t c1r,
    int16_t c2x, int16_t c2y, int16_t c2r
    ) {

    mp_int_t dist_x = c1x - c2x;
    mp_int_t dist_y = c1y - c2y;

    mp_int_t dist = (dist_x * dist_x) + (dist_y * dist_y);

    return dist <= (c1r + c2r) * (c1r + c2r);

}

bool common_hal_vectorio_circle_contains_point(
    int16_t cx, int16_t cy, int16_t cr,
    int16_t px, int16_t py
    ) {

    mp_int_t dist_x = px - cx;
    mp_int_t dist_y = py - cy;

	mp_int_t dist_sq = (dist_x * dist_x) + (dist_y * dist_y);

    return dist_sq <= cr * cr;
}

bool common_hal_vectorio_rectangle_contains_point(
    int16_t rx, int16_t ry, int16_t rw, int16_t rh,
    int16_t px, int16_t py
    ) {

	if (rx > px){
		return false;
	}
	if (px > rx + rw){
		return false;
	}
	if (ry > py){
		return false;
	}
	if (py > ry + rh){
		return false;
	}
    return true;
}

float measure_distance(int x1, int y1, int x2, int y2){
	int dist_x = x1 - x2;
	int dist_y = y1 - y2;
	return sqrtf((dist_x * dist_x) + (dist_y * dist_y));
}

bool common_hal_vectorio_line_contains_point(
    int16_t x1, int16_t y1, int16_t x2, int16_t y2,
    int16_t px, int16_t py, mp_float_t padding
    ) {

	float line_length = measure_distance(x1, y1, x2, y2);
	float d1 = measure_distance(x1, y1, px, py);
	float d2 = measure_distance(x2, y2, px, py);

	if (d1+d2 >= line_length-padding && d1+d2 <= line_length+padding) {
    	return true;
	}
	return false;

}



bool common_hal_vectorio_rectangle_rectangle_intersects(
    int16_t r1x, int16_t r1y, int16_t r1w, int16_t r1h,
    int16_t r2x, int16_t r2y, int16_t r2w, int16_t r2h) {


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

bool common_hal_vectorio_polygon_circle_intersects(
	mp_obj_t points_list, int16_t cx, int16_t cy, int16_t cr
) {
	size_t len = 0;
	mp_obj_t *points_list_items;
    mp_obj_list_get(points_list, &len, &points_list_items);

	for(uint16_t i = 0; i < len; i++){
		size_t cur_tuple_len = 0;
		mp_obj_t *cur_point_tuple;
        mp_arg_validate_type(points_list_items[i], &mp_type_tuple, MP_QSTR_point);
        mp_obj_tuple_get(points_list_items[i], &cur_tuple_len, &cur_point_tuple);

		mp_int_t cur_x = mp_arg_validate_type_int(cur_point_tuple[0], MP_QSTR_x);
		mp_int_t cur_y = mp_arg_validate_type_int(cur_point_tuple[1], MP_QSTR_y);

		size_t next_tuple_len = 0;
		mp_obj_t *next_point_tuple;
		int next_index = (i + 1) % len;
        mp_arg_validate_type(points_list_items[next_index], &mp_type_tuple, MP_QSTR_point);
        mp_obj_tuple_get(points_list_items[next_index], &next_tuple_len, &next_point_tuple);

		mp_int_t next_x = mp_arg_validate_type_int(next_point_tuple[0], MP_QSTR_x);
		mp_int_t next_y = mp_arg_validate_type_int(next_point_tuple[1], MP_QSTR_y);

		mp_printf(&mp_plat_print, "%d,%d - %d,%d \n", cur_x, cur_y, next_x, next_y);



	}

	return false;
}