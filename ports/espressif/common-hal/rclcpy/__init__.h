// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/runtime.h"

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

typedef struct {
    bool initialized;
    rcl_allocator_t rcl_allocator;
    rclc_support_t rcl_support;
} rclcpy_context_t;

extern rclcpy_context_t rclcpy_default_context;