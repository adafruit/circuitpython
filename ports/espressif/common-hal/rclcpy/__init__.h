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
#include <rmw_microros/rmw_microros.h>


typedef struct {
    bool initialized;
    uint8_t critical_fail;
    rcl_allocator_t rcl_allocator;
    rclc_support_t rcl_support;
    rcl_init_options_t init_options;
    rmw_init_options_t *rmw_options;
} rclcpy_context_t;

extern rclcpy_context_t rclcpy_default_context;
void rclcpy_reset(void);
