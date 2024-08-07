// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#include "shared-module/vectorio/__init__.h"
#include "shared-bindings/vectorio/Rectangle.h"
#include "shared-bindings/vectorio/VectorShape.h"
#include "shared-bindings/vectorio/Circle.h"
#include "shared-module/displayio/area.h"

#include "py/runtime.h"
#include "stdlib.h"
#include <math.h>


void common_hal_vectorio_rectangle_construct(vectorio_rectangle_t *self, uint32_t width, uint32_t height, uint16_t color_index) {
    self->width = width;
    self->height = height;
    self->color_index = color_index + 1;
}

void common_hal_vectorio_rectangle_set_on_dirty(vectorio_rectangle_t *self, vectorio_event_t on_dirty) {
    if (self->on_dirty.obj != NULL) {
        mp_raise_TypeError(MP_ERROR_TEXT("can only have one parent"));
    }
    self->on_dirty = on_dirty;
}

uint32_t common_hal_vectorio_rectangle_get_pixel(void *obj, int16_t x, int16_t y) {
    vectorio_rectangle_t *self = obj;
    if (x >= 0 && y >= 0 && x < self->width && y < self->height) {
        return self->color_index;
    }
    return 0;
}


void common_hal_vectorio_rectangle_get_area(void *rectangle, displayio_area_t *out_area) {
    vectorio_rectangle_t *self = rectangle;
    out_area->x1 = 0;
    out_area->y1 = 0;
    out_area->x2 = self->width;
    out_area->y2 = self->height;
}


mp_obj_t common_hal_vectorio_rectangle_get_draw_protocol(void *rectangle) {
    vectorio_rectangle_t *self = rectangle;
    return self->draw_protocol_instance;
}

bool common_hal_vectorio_rectangle_intersecting(vectorio_rectangle_t *self, mp_obj_t other_shape) {

    mp_obj_t possible_rect = mp_obj_cast_to_native_base(other_shape, &vectorio_rectangle_type);
    if (possible_rect != MP_OBJ_NULL) {

        vectorio_rectangle_t *other_rect = possible_rect;

        mp_int_t self_left = common_hal_vectorio_vector_shape_get_x(self->draw_protocol_instance);
        mp_int_t self_right = self_left + self->width;
        mp_int_t self_top = common_hal_vectorio_vector_shape_get_y(self->draw_protocol_instance);
        mp_int_t self_bottom = self_top + self->height;

        mp_int_t other_left = common_hal_vectorio_vector_shape_get_x(other_rect->draw_protocol_instance);
        mp_int_t other_right = other_left + other_rect->width;
        mp_int_t other_top = common_hal_vectorio_vector_shape_get_y(other_rect->draw_protocol_instance);
        mp_int_t other_bottom = other_top + other_rect->height;

        return self_left < other_right && self_right > other_left &&
               self_top < other_bottom && self_bottom > other_top;
    }
    mp_obj_t possible_circle = mp_obj_cast_to_native_base(other_shape, &vectorio_circle_type);
    if (possible_circle != MP_OBJ_NULL) {
        vectorio_circle_t *other_circle = possible_circle;

        mp_int_t self_left = common_hal_vectorio_vector_shape_get_x(self->draw_protocol_instance);
        mp_int_t self_right = self_left + self->width;
        mp_int_t self_top = common_hal_vectorio_vector_shape_get_y(self->draw_protocol_instance);
        mp_int_t self_bottom = self_top + self->height;

        mp_int_t other_circle_x = common_hal_vectorio_vector_shape_get_x(other_circle->draw_protocol_instance);
        mp_int_t other_circle_y = common_hal_vectorio_vector_shape_get_y(other_circle->draw_protocol_instance);

        mp_int_t test_x = other_circle_x;
        mp_int_t test_y = other_circle_y;

        if (other_circle_x < self_left) {
            test_x = self_left;
        } else if (other_circle_x > self_right) {
            test_x = self_right;
        }

        if (other_circle_y < self_top) {
            test_y = self_top;
        } else if (other_circle_y > self_bottom) {
            test_y = self_bottom;
        }

        mp_int_t dist_x = other_circle_x - test_x;
        mp_int_t dist_y = other_circle_y - test_y;
        mp_int_t dist = sqrtf((dist_x * dist_x) + (dist_y * dist_y));

        return dist <= other_circle->radius;
    }

    return false;
}

int16_t common_hal_vectorio_rectangle_get_width(void *obj) {
    vectorio_rectangle_t *self = obj;
    return self->width;
}

void common_hal_vectorio_rectangle_set_width(void *obj, int16_t width) {
    vectorio_rectangle_t *self = obj;
    self->width = abs(width);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

int16_t common_hal_vectorio_rectangle_get_height(void *obj) {
    vectorio_rectangle_t *self = obj;
    return self->height;
}

void common_hal_vectorio_rectangle_set_height(void *obj, int16_t height) {
    vectorio_rectangle_t *self = obj;
    self->height = abs(height);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

uint16_t common_hal_vectorio_rectangle_get_color_index(void *obj) {
    vectorio_rectangle_t *self = obj;
    return self->color_index - 1;
}

void common_hal_vectorio_rectangle_set_color_index(void *obj, uint16_t color_index) {
    vectorio_rectangle_t *self = obj;
    self->color_index = abs(color_index + 1);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}
