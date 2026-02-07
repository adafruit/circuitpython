// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#pragma once
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    int32_t data;
} rclcpy_std_msgs_int32_obj_t;

extern const mp_obj_type_t rclcpy_std_msgs_int32_type;
