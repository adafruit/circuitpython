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
    uint16_t radius;
    uint16_t color_index;
    // The outline is a ring inside the circle's outer edge, so it changes the circle's
    // appearance but not its footprint. outline_color_index is biased by one like
    // color_index, so that 0 can mean "no outline" as well as "not covered".
    uint16_t outline_width;
    uint16_t outline_color_index;
    vectorio_event_t on_dirty;
    mp_obj_t draw_protocol_instance;
} vectorio_circle_t;
