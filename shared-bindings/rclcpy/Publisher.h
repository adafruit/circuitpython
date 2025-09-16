// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#pragma once
#include "common-hal/rclcpy/Publisher.h"


extern const mp_obj_type_t rclcpy_publisher_type;

void common_hal_rclcpy_publisher_construct(rclcpy_publisher_obj_t *self, rclcpy_node_obj_t *node,
    const char *topic_name);
bool common_hal_rclcpy_publisher_deinited(rclcpy_publisher_obj_t *self);
void common_hal_rclcpy_publisher_deinit(rclcpy_publisher_obj_t *self);
void common_hal_rclcpy_publisher_publish_int32(rclcpy_publisher_obj_t *self, int32_t data);
const char *common_hal_rclcpy_publisher_get_topic_name(rclcpy_publisher_obj_t *self);
