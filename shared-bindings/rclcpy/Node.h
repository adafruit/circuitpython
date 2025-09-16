// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#pragma once
#include "common-hal/rclcpy/Node.h"


extern const mp_obj_type_t rclcpy_node_type;

void common_hal_rclcpy_node_construct(rclcpy_node_obj_t *self,
    const char *node_name, const char *namespace);
bool common_hal_rclcpy_node_deinited(rclcpy_node_obj_t *self);
void common_hal_rclcpy_node_deinit(rclcpy_node_obj_t *self);
const char *common_hal_rclcpy_node_get_name(rclcpy_node_obj_t *self);
const char *common_hal_rclcpy_node_get_namespace(rclcpy_node_obj_t *self);
