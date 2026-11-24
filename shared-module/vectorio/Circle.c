// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/vectorio/Circle.h"
#include "shared-module/vectorio/__init__.h"
#include "shared-module/displayio/area.h"

#include "py/runtime.h"
#include "stdlib.h"


void common_hal_vectorio_circle_construct(vectorio_circle_t *self, uint16_t radius, uint16_t color_index, uint16_t outline_width, int32_t outline_color_index) {
    self->radius = radius;
    self->on_dirty.obj = NULL;
    self->color_index = color_index + 1;
    self->outline_width = outline_width;
    self->outline_color_index = outline_color_index < 0 ? 0 : outline_color_index + 1;
}

void common_hal_vectorio_circle_set_on_dirty(vectorio_circle_t *self, vectorio_event_t on_dirty) {
    if (self->on_dirty.obj != NULL) {
        mp_raise_TypeError(MP_ERROR_TEXT("can only have one parent"));
    }
    self->on_dirty = on_dirty;
}


// Coverage of a circle of the given radius, for a point x/y pixels right of and below its
// center. (x + y <= radius) implies x*x + y*y <= radius*radius, which keeps the pixels nearest
// the center from paying for the multiplication.
static bool circle_covers(int32_t x, int32_t y, int32_t radius) {
    x = x < 0 ? -x : x;
    y = y < 0 ? -y : y;
    if (x + y <= radius) {
        return true;
    }
    if (x > radius || y > radius) {
        return false;
    }
    return x * x + y * y <= radius * radius;
}

uint32_t common_hal_vectorio_circle_get_pixel(void *obj, int16_t x, int16_t y) {
    vectorio_circle_t *self = obj;
    int32_t radius = self->radius;
    if (!circle_covers(x, y, radius)) {
        return 0;
    }
    // A zero outline_color_index means no outline was set, and then the fill reaches the edge.
    if (self->outline_width > 0 && self->outline_color_index != 0 &&
        !circle_covers(x, y, radius - self->outline_width)) {
        return self->outline_color_index;
    }
    return self->color_index;
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

uint16_t common_hal_vectorio_circle_get_outline_width(void *obj) {
    vectorio_circle_t *self = obj;
    return self->outline_width;
}

void common_hal_vectorio_circle_set_outline_width(void *obj, uint16_t outline_width) {
    vectorio_circle_t *self = obj;
    self->outline_width = outline_width;
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

int32_t common_hal_vectorio_circle_get_outline_color_index(void *obj) {
    vectorio_circle_t *self = obj;
    return self->outline_color_index == 0 ? -1 : self->outline_color_index - 1;
}

void common_hal_vectorio_circle_set_outline_color_index(void *obj, int32_t outline_color_index) {
    vectorio_circle_t *self = obj;
    self->outline_color_index = outline_color_index < 0 ? 0 : outline_color_index + 1;
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

mp_obj_t common_hal_vectorio_circle_get_draw_protocol(void *circle) {
    vectorio_circle_t *self = circle;
    return self->draw_protocol_instance;
}
