// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>

typedef struct {
    mp_obj_base_t base;
    const struct device *spi_device;
    struct k_mutex mutex;
    bool has_lock;
    struct spi_config config[2];  // Two configs for pointer comparison by driver
    uint8_t active_config;         // Index of currently active config (0 or 1)
    struct k_poll_signal signal;
} busio_spi_obj_t;

// Helper function for Zephyr-specific initialization from device tree
mp_obj_t common_hal_busio_spi_construct_from_device(busio_spi_obj_t *self, const struct device *spi_device);
