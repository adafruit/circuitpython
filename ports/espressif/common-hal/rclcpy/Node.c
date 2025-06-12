// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/rclcpy/Node.h"
#include "shared-bindings/rclcpy/__init__.h"

#include "esp_log.h"

void common_hal_rclcpy_node_construct(rclcpy_node_obj_t *self,
    const char *node_name, const char *node_namespace) {

    rclc_node_init_default(&self->rcl_node, node_name, node_namespace, &rclcpy_default_context.rcl_support);
    if (!rcl_node_is_valid(&self->rcl_node)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("ROS node failed to initialize"));
    }
    self->deinited = false;
}

bool common_hal_rclcpy_node_deinited(rclcpy_node_obj_t *self) {
    return self->deinited;
}

void common_hal_rclcpy_node_deinit(rclcpy_node_obj_t *self) {
    if (common_hal_rclcpy_node_deinited(self)) {
        return;
    }
    // Clean up Micro-ROS object
    rcl_ret_t ret = rcl_node_fini(&self->rcl_node);
    if (ret != RCL_RET_OK) {
        // TODO: node_fini returns a fail here, but doesn't impede microros
        // from restarting. Debug left for future investigation.
        ESP_LOGW("RCLCPY", "Node cleanup error: %d", ret);
        // rclcpy_default_context.critical_fail=RCLCPY_NODE_FAIL;
    }
    self->deinited = true;
}

const char *common_hal_rclcpy_node_get_name(rclcpy_node_obj_t *self) {
    return rcl_node_get_name(&self->rcl_node);
}

const char *common_hal_rclcpy_node_get_namespace(rclcpy_node_obj_t *self) {
    return rcl_node_get_namespace(&self->rcl_node);
}
