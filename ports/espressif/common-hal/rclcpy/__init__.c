// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/rclcpy/__init__.h"

#include "esp_log.h"

rclcpy_context_t rclcpy_default_context = {
    .initialized = false,
};

static void* microros_allocate(size_t size, void* state) {
    (void) state;
    return m_malloc(size);
}

static void microros_deallocate(void* pointer, void* state) {
    (void) state;
    m_free(pointer);
}

static void* microros_reallocate(void* pointer, size_t size, void* state) {
    (void) state;
    return m_realloc(pointer, size);
}

static void* microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void* state) {
    (void) state;
    size_t total_size = number_of_elements * size_of_element;
    void* ptr = m_malloc(total_size);
    if (ptr != NULL) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void rclcpy_reset(void) {
    if (rclcpy_default_context.initialized) {
        // Clean up support context
        rcl_ret_t ret = rclc_support_fini(&rclcpy_default_context.rcl_support);
        if (ret != RCL_RET_OK) {
            ESP_LOGW("RCLCPY", "Support cleanup error: %d", ret);
        }

        // Clean up init options
        ret = rcl_init_options_fini(&rclcpy_default_context.init_options);
        if (ret != RCL_RET_OK) {
            ESP_LOGW("RCLCPY", "Init options cleanup error: %d", ret);
        }

        // Reset context state
        memset(&rclcpy_default_context, 0, sizeof(rclcpy_default_context));
        rclcpy_default_context.initialized = false;
    }
}

void common_hal_rclcpy_init(const char *agent_ip, const char *agent_port, int16_t domain_id) {
    // Get empty allocator
    rcl_allocator_t custom_allocator = rcutils_get_zero_initialized_allocator();

    // Set custom allocation methods
    custom_allocator.allocate = microros_allocate;
    custom_allocator.deallocate = microros_deallocate;
    custom_allocator.reallocate = microros_reallocate;
    custom_allocator.zero_allocate =  microros_zero_allocate;
    // Set custom allocator as default
    if (!rcutils_set_default_allocator(&custom_allocator)) {
        ESP_LOGW("RCLCPY", "allocator failure");
        mp_raise_RuntimeError(MP_ERROR_TEXT("ROS memory allocator failure"));
    }
    rclcpy_default_context.rcl_allocator = custom_allocator;

    rcl_ret_t ret;

    // Options Init
    rclcpy_default_context.init_options = rcl_get_zero_initialized_init_options();
	ret = rcl_init_options_init(&rclcpy_default_context.init_options, rclcpy_default_context.rcl_allocator);
    if (ret != RCL_RET_OK) {
        ESP_LOGW("RCLCPY", "Options init failure: %d", ret);
    }
    if (domain_id < 0) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid ROS domain ID"));
    }
	ret = rcl_init_options_set_domain_id(&rclcpy_default_context.init_options, domain_id);
    if (ret != RCL_RET_OK) {
        ESP_LOGW("RCLCPY", "Options domain failure: %d", ret);
    }

    // Set up Agent
    rclcpy_default_context.rmw_options = rcl_init_options_get_rmw_init_options(&rclcpy_default_context.init_options);
	ret = rmw_uros_options_set_udp_address(agent_ip, agent_port, rclcpy_default_context.rmw_options);
    if (ret != RCL_RET_OK) {
        ESP_LOGW("RCLCPY", "Agent options failure: %d", ret);
    }

    // Support Init
    ret = rclc_support_init_with_options(&rclcpy_default_context.rcl_support,
            0,
            NULL,
            &rclcpy_default_context.init_options,
            &rclcpy_default_context.rcl_allocator);
    if (ret != RCL_RET_OK) {
        ESP_LOGW("RCLCPY", "Initialization failure: %d", ret);
        mp_raise_RuntimeError(MP_ERROR_TEXT("ROS failed to initialize. Is agent connected?"));
    } else {
        rclcpy_default_context.initialized = true;
    }
}

rclcpy_context_t * common_hal_rclcpy_get_default_context(void) {
    return &rclcpy_default_context;
}

bool common_hal_rclcpy_default_context_is_initialized(void) {
    return rclcpy_default_context.initialized;
}
