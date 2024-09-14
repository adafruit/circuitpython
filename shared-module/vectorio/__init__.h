// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef void event_function(mp_obj_t obj);

typedef struct {
    mp_obj_t obj;
    event_function *event;
} vectorio_event_t;

float measure_distance(
    int x1, int y1, int x2, int y2);
