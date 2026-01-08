// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#pragma once
#include "py/obj.h"

#include <std_msgs/msg/bool.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/string.h>

typedef enum {
    RCLCPY_MSG_TYPE_BOOL,
    RCLCPY_MSG_TYPE_INT32,
    RCLCPY_MSG_TYPE_COUNT
} rclcpy_msg_kind_t;

typedef struct {
    const mp_obj_type_t *cpy_type;
    const rosidl_message_type_support_t *ros_type_support;
    rclcpy_msg_kind_t msg_kind;
} rclcpy_registry_msg_entry_t;

void deinitialize_registry(void);
