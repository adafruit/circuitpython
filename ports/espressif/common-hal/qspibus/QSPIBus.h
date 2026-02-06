// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

#include "esp-idf/components/esp_lcd/include/esp_lcd_panel_io.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct {
    mp_obj_base_t base;

    // ESP LCD panel IO handle used for QSPI transactions.
    esp_lcd_panel_io_handle_t io_handle;

    // SPI host (SPI2_HOST on ESP32-S3).
    spi_host_device_t host_id;

    // Claimed GPIO numbers.
    int8_t clock_pin;
    int8_t data0_pin;
    int8_t data1_pin;
    int8_t data2_pin;
    int8_t data3_pin;
    int8_t cs_pin;
    int8_t reset_pin; // -1 when reset line is not provided.

    uint32_t frequency;
    bool bus_initialized;

    // Signaled from ISR when panel IO transfer completes.
    SemaphoreHandle_t transfer_done_sem;
} qspibus_qspibus_obj_t;
