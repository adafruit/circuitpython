// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/rclcpy/Node.h"
#include "common-hal/rclcpy/__init__.h"

#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <stdio.h>
#include <std_msgs/msg/int32.h>


typedef struct {
    mp_obj_base_t base;
    rclcpy_node_obj_t *node;
    rcl_publisher_t rcl_publisher;
} rclcpy_publisher_obj_t;
