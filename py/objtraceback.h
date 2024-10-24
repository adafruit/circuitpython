// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct _mp_obj_traceback_t {
    mp_obj_base_t base;
    size_t alloc : (8 * sizeof(size_t) / 2);
    size_t len : (8 * sizeof(size_t) / 2);
    size_t *data;
} mp_obj_traceback_t;
