// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/rclcpy/__init__.h"

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

void common_hal_rclcpy_init(void) {
    // Get empty allocator
    rcl_allocator_t custom_allocator = rcutils_get_zero_initialized_allocator();

    // Set custom allocation methods
    custom_allocator.allocate = microros_allocate;
    custom_allocator.deallocate = microros_deallocate;
    custom_allocator.reallocate = microros_reallocate;
    custom_allocator.zero_allocate =  microros_zero_allocate;

    // Set custom allocator as default
    if (!rcutils_set_default_allocator(&custom_allocator)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("ROS memory allocator failure"));
    }

    rclcpy_default_context.rcl_allocator = custom_allocator;
    rcl_ret_t ret;
    ret = rclc_support_init(&rclcpy_default_context.rcl_support, 0, NULL, &rclcpy_default_context.rcl_allocator);
    if (ret != RCL_RET_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("ROS failed to initialize"));
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