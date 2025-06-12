// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/rclcpy/Publisher.h"

#include "esp_log.h"

void common_hal_rclcpy_publisher_construct(rclcpy_publisher_obj_t *self, rclcpy_node_obj_t *node,
    const char *topic_name) {

    // Create Int32 type object
    // TODO: support other message types through class imports
    const rosidl_message_type_support_t *type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32);

    // Creates a reliable Int32 publisher
    rcl_ret_t rc = rclc_publisher_init_default(
        &self->rcl_publisher, &node->rcl_node,
        type_support, topic_name);
    if (RCL_RET_OK != rc) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("ROS topic failed to initialize"));
    }

    self->node = node;
}

bool common_hal_rclcpy_publisher_deinited(rclcpy_publisher_obj_t *self) {
    return self->node == NULL;
}

void common_hal_rclcpy_publisher_deinit(rclcpy_publisher_obj_t *self) {
    if (common_hal_rclcpy_publisher_deinited(self)) {
        return;
    }
    // Clean up Micro-ROS object
    rcl_ret_t ret = rcl_publisher_fini(&self->rcl_publisher, &self->node->rcl_node);
    if (ret != RCL_RET_OK) {
        // TODO: publisher_fini returns a fail here, but doesn't impede microros
        // from restarting. Debug left for future investigation.
        ESP_LOGW("RCLCPY", "Publisher cleanup error: %d", ret);
        // rclcpy_default_context.critical_fail=RCLCPY_PUB_FAIL;
    }
    self->node = NULL;
}

void common_hal_rclcpy_publisher_publish_int32(rclcpy_publisher_obj_t *self, int32_t data) {
    // Int32 message object
    std_msgs__msg__Int32 msg;

    // Set message value
    msg.data = data;

    // Publish message
    rcl_ret_t rc = rcl_publish(&self->rcl_publisher, &msg, NULL);
    if (RCL_RET_OK != rc) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Could not publish to ROS topic"));
    }
}

const char *common_hal_rclcpy_publisher_get_topic_name(rclcpy_publisher_obj_t *self) {
    return rcl_publisher_get_topic_name(&self->rcl_publisher);
}
