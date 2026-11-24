// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"
#include "shared-module/vectorio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    uint16_t width;
    uint16_t height;
    uint16_t color_index;
    // The corner radius and the outline width are in the rectangle's own pixels, and neither
    // may exceed half of the shortest side. The outline is drawn inside the rectangle's
    // bounds, so neither one changes the rectangle's footprint.
    //
    // outline_color_index is biased by one like color_index, so that 0 can mean "no outline"
    // as well as "not covered" (see VectorShape.h). A radius or outline width change is a
    // pure appearance change: the coverage stays inside the same bounds.
    uint16_t radius;
    uint16_t outline_width;
    uint16_t outline_color_index;
    vectorio_event_t on_dirty;
    mp_obj_t draw_protocol_instance;
} vectorio_rectangle_t;
