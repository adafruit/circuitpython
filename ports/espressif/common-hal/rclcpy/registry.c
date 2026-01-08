// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/rclcpy/registry.h"
#include "shared-bindings/rclcpy/std_msgs/__init__.h"

static rclcpy_registry_msg_entry_t msg_registry[RCLCPY_MSG_TYPE_COUNT];
static bool registry_initialized = false;

// static const rclcpy_registry_msg_entry_t msg_registry[] = {
//     // stg_msg
//     {
//         .cpy_type = &rclcpy_std_msgs_bool_type,
//         .ros_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
//         .msg_kind = RCLCPY_MSG_TYPE_BOOL
//     },
//     {
//         .cpy_type = &rclcpy_std_msgs_int32_type,
//         .ros_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//         .msg_kind = RCLCPY_MSG_TYPE_INT32
//     },
// };

// Called on demand, not at init
static void initialize_registry(void) {
    if (registry_initialized) {
        return;
    }

    msg_registry[0] = (rclcpy_registry_msg_entry_t) {
        .cpy_type = &rclcpy_std_msgs_bool_type,
        .ros_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
        .msg_kind = RCLCPY_MSG_TYPE_BOOL
    };

    msg_registry[1] = (rclcpy_registry_msg_entry_t) {
        .cpy_type = &rclcpy_std_msgs_int32_type,
        .ros_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        .msg_kind = RCLCPY_MSG_TYPE_INT32
    };

    registry_initialized = true;
}

// Used at reset (just to be careful with ROS support pointers)
void deinitialize_registry(void) {
    registry_initialized = false;
}

const rclcpy_registry_msg_entry_t * common_hal_rclcpy_registry_get_msg_entry(const mp_obj_type_t *cpy_type) {
    initialize_registry();
    for (size_t i = 0; i < RCLCPY_MSG_TYPE_COUNT; i++) {
        if (msg_registry[i].cpy_type == cpy_type) {
            return &msg_registry[i];
        }
    }
    return NULL;
}

const rosidl_message_type_support_t * common_hal_rclcpy_registry_get_msg_ros_typesupport(const mp_obj_type_t *cpy_type) {
    initialize_registry();
    const rclcpy_registry_msg_entry_t *entry = common_hal_rclcpy_registry_get_msg_entry(cpy_type);
    return entry ? entry->ros_type_support : NULL;
}
