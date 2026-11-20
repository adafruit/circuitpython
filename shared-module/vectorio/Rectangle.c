// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#include "shared-module/vectorio/__init__.h"
#include "shared-bindings/vectorio/Rectangle.h"
#include "shared-module/displayio/area.h"

#include "py/runtime.h"
#include "stdlib.h"


void common_hal_vectorio_rectangle_construct(vectorio_rectangle_t *self, uint32_t width, uint32_t height, uint16_t color_index, uint16_t radius, uint16_t outline_width, int32_t outline_color_index) {
    self->width = width;
    self->height = height;
    self->color_index = color_index + 1;
    self->radius = radius;
    self->outline_width = outline_width;
    self->outline_color_index = outline_color_index < 0 ? 0 : outline_color_index + 1;
    // set_on_dirty() uses this to detect a second parent, so it has to start empty.
    self->on_dirty.obj = NULL;
}

void common_hal_vectorio_rectangle_set_on_dirty(vectorio_rectangle_t *self, vectorio_event_t on_dirty) {
    if (self->on_dirty.obj != NULL) {
        mp_raise_TypeError(MP_ERROR_TEXT("can only have one parent"));
    }
    self->on_dirty = on_dirty;
}

// Coverage of a rounded rectangle of the given extent whose top left pixel is (0, 0). The
// corner arcs are centered one radius in from each corner, which is the same center
// display_shapes' midpoint circle uses, so swapping implementations moves few pixels.
//
// Distances are doubled so that pixel centers land on integers instead of half pixels.
// The bindings ensure that `radius` is at most half of the shortest side.
static bool rectangle_covers(int32_t x, int32_t y, uint16_t width, uint16_t height, uint16_t radius) {
    if (x < 0 || y < 0 || x >= (int32_t)width || y >= (int32_t)height) {
        return false;
    }
    if (radius == 0) {
        return true;
    }
    // 2x+1 is twice the pixel's center, so these are the distances from the nearest corner
    // arc's center in halves of a pixel. 0 means the pixel is on one of the flat sides.
    int32_t twice_radius = 2 * (int32_t)radius;
    int32_t twice_x = 2 * x + 1;
    int32_t twice_y = 2 * y + 1;
    int32_t dx = 0;
    int32_t dy = 0;
    if (twice_x < twice_radius) {
        dx = twice_radius - twice_x;
    } else if (twice_x > 2 * (int32_t)width - twice_radius) {
        dx = twice_x - (2 * (int32_t)width - twice_radius);
    }
    if (twice_y < twice_radius) {
        dy = twice_radius - twice_y;
    } else if (twice_y > 2 * (int32_t)height - twice_radius) {
        dy = twice_y - (2 * (int32_t)height - twice_radius);
    }
    if (dx == 0 || dy == 0) {
        return true;
    }
    return (int64_t)dx * dx + (int64_t)dy * dy <= (int64_t)twice_radius * twice_radius;
}

uint32_t common_hal_vectorio_rectangle_get_pixel(void *obj, int16_t x, int16_t y) {
    vectorio_rectangle_t *self = obj;
    uint16_t radius = self->radius;
    if (!rectangle_covers(x, y, self->width, self->height, radius)) {
        return 0;
    }
    uint16_t outline_width = self->outline_width;
    // A zero outline_color_index means no outline was set, and then the fill reaches the edge.
    if (outline_width > 0 && self->outline_color_index != 0) {
        // The outline is the band between the outer edge and a copy of the rectangle inset by
        // the outline width. Inset corners stay concentric, so the inner radius shrinks too.
        uint16_t inner_radius = radius > outline_width ? radius - outline_width : 0;
        if (!rectangle_covers(x - outline_width, y - outline_width,
            self->width - 2 * outline_width, self->height - 2 * outline_width, inner_radius)) {
            return self->outline_color_index;
        }
    }
    return self->color_index;
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

uint16_t common_hal_vectorio_rectangle_get_radius(void *obj) {
    vectorio_rectangle_t *self = obj;
    return self->radius;
}

void common_hal_vectorio_rectangle_set_radius(void *obj, uint16_t radius) {
    vectorio_rectangle_t *self = obj;
    self->radius = radius;
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

uint16_t common_hal_vectorio_rectangle_get_outline_width(void *obj) {
    vectorio_rectangle_t *self = obj;
    return self->outline_width;
}

void common_hal_vectorio_rectangle_set_outline_width(void *obj, uint16_t outline_width) {
    vectorio_rectangle_t *self = obj;
    self->outline_width = outline_width;
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

int32_t common_hal_vectorio_rectangle_get_outline_color_index(void *obj) {
    vectorio_rectangle_t *self = obj;
    return self->outline_color_index == 0 ? -1 : self->outline_color_index - 1;
}

void common_hal_vectorio_rectangle_set_outline_color_index(void *obj, int32_t outline_color_index) {
    vectorio_rectangle_t *self = obj;
    self->outline_color_index = outline_color_index < 0 ? 0 : outline_color_index + 1;
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}
