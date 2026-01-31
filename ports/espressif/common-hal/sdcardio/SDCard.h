// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"

// ESP-specific sdcardio implementation using ESP-IDF's sdspi_host driver.
// This provides ~2x better performance compared to the generic shared-module
// implementation by using hardware-optimized SD protocol handling.

typedef struct {
    mp_obj_base_t base;

    // SD card state
    sdmmc_card_t *card;
    sdspi_dev_handle_t slot;
    int spi_host;

    // Pin storage (for cleanup)
    gpio_num_t cs_pin;
    gpio_num_t miso_pin;
    gpio_num_t mosi_pin;
    gpio_num_t clk_pin;

    // State flags
    bool mounted;
    bool spi_bus_initialized;
} sdcardio_sdcard_obj_t;

// Reset function for module cleanup
void sdcardio_reset(void);
