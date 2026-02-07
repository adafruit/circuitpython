// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#pragma once
#include "common-hal/rclcpy/registry.h"

const rclcpy_registry_msg_entry_t * common_hal_rclcpy_registry_get_msg_entry(const mp_obj_type_t *cpy_type);
const rosidl_message_type_support_t * common_hal_rclcpy_registry_get_msg_ros_typesupport(const mp_obj_type_t *cpy_type);
