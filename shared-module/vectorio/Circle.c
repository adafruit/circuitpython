// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/vectorio/Circle.h"
#include "shared-module/vectorio/__init__.h"
#include "shared-module/displayio/area.h"
#include "shared-bindings/vectorio/Rectangle.h"
#include "shared-bindings/vectorio/VectorShape.h"
#include "py/runtime.h"
#include "stdlib.h"
#include <math.h>


void common_hal_vectorio_circle_construct(vectorio_circle_t *self, uint16_t radius, uint16_t color_index) {
    self->radius = radius;
    self->on_dirty.obj = NULL;
    self->color_index = color_index + 1;
}

void common_hal_vectorio_circle_set_on_dirty(vectorio_circle_t *self, vectorio_event_t on_dirty) {
    if (self->on_dirty.obj != NULL) {
        mp_raise_TypeError(MP_ERROR_TEXT("can only have one parent"));
    }
    self->on_dirty = on_dirty;
}


uint32_t common_hal_vectorio_circle_get_pixel(void *obj, int16_t x, int16_t y) {
    vectorio_circle_t *self = obj;
    int16_t radius = self->radius;
    x = abs(x);
    y = abs(y);
    if (x + y <= radius) {
        return self->color_index;
    }
    if (x > radius) {
        return 0;
    }
    if (y > radius) {
        return 0;
    }
    const bool pythagorasSmallerThanRadius = (int32_t)x * x + (int32_t)y * y <= (int32_t)radius * radius;
    return pythagorasSmallerThanRadius ? self->color_index : 0;
}


void common_hal_vectorio_circle_get_area(void *circle, displayio_area_t *out_area) {
    vectorio_circle_t *self = circle;
    out_area->x1 = -1 * self->radius - 1;
    out_area->y1 = -1 * self->radius - 1;
    out_area->x2 = self->radius + 1;
    out_area->y2 = self->radius + 1;
}

int16_t common_hal_vectorio_circle_get_radius(void *obj) {
    vectorio_circle_t *self = obj;
    return self->radius;
}

void common_hal_vectorio_circle_set_radius(void *obj, int16_t radius) {
    vectorio_circle_t *self = obj;
    self->radius = abs(radius);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

uint16_t common_hal_vectorio_circle_get_color_index(void *obj) {
    vectorio_circle_t *self = obj;
    return self->color_index - 1;
}

void common_hal_vectorio_circle_set_color_index(void *obj, uint16_t color_index) {
    vectorio_circle_t *self = obj;
    self->color_index = abs(color_index + 1);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

bool common_hal_vectorio_circle_intersects(vectorio_circle_t *self, mp_obj_t other_shape) {

    mp_obj_t possible_rect = mp_obj_cast_to_native_base(other_shape, &vectorio_rectangle_type);
    if (possible_rect != MP_OBJ_NULL) {

        vectorio_rectangle_t *other_rect = possible_rect;

        mp_int_t self_x = common_hal_vectorio_vector_shape_get_x(self->draw_protocol_instance);
        mp_int_t self_y = common_hal_vectorio_vector_shape_get_y(self->draw_protocol_instance);

        mp_int_t other_left = common_hal_vectorio_vector_shape_get_x(other_rect->draw_protocol_instance);
        mp_int_t other_right = other_left + other_rect->width;
        mp_int_t other_top = common_hal_vectorio_vector_shape_get_y(other_rect->draw_protocol_instance);
        mp_int_t other_bottom = other_top + other_rect->height;

        mp_int_t test_x = self_x;
        mp_int_t test_y = self_y;

        if (self_x < other_left) {
            test_x = other_left;
        } else if (self_x > other_right) {
            test_x = other_right;
        }

        if (self_y < other_top) {
            test_y = other_top;
        } else if (self_y > other_bottom) {
            test_y = other_bottom;
        }

        mp_int_t dist_x = self_x - test_x;
        mp_int_t dist_y = self_y - test_y;
        mp_int_t dist = sqrtf((dist_x * dist_x) + (dist_y * dist_y));

        return dist <= self->radius;
    }
    mp_obj_t possible_circle = mp_obj_cast_to_native_base(other_shape, &vectorio_circle_type);
    if (possible_circle != MP_OBJ_NULL) {
        vectorio_circle_t *other_circle = possible_circle;

        mp_int_t self_x = common_hal_vectorio_vector_shape_get_x(self->draw_protocol_instance);
        mp_int_t self_y = common_hal_vectorio_vector_shape_get_y(self->draw_protocol_instance);

        mp_int_t other_circle_x = common_hal_vectorio_vector_shape_get_x(other_circle->draw_protocol_instance);
        mp_int_t other_circle_y = common_hal_vectorio_vector_shape_get_y(other_circle->draw_protocol_instance);

        mp_int_t dist_x = self_x - other_circle_x;
        mp_int_t dist_y = self_y - other_circle_y;

        mp_int_t dist = sqrtf((dist_x * dist_x) + (dist_y * dist_y));

        return (dist <= self->radius + other_circle->radius);

    }

    return false;
}

mp_obj_t common_hal_vectorio_circle_get_draw_protocol(void *circle) {
    vectorio_circle_t *self = circle;
    return self->draw_protocol_instance;
}
