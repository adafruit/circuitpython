// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
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
    int8_t dcx_pin;   // -1 when optional DCX line is not provided.
    int8_t reset_pin; // -1 when reset line is not provided.
    int8_t power_pin; // -1 when board has no explicit display power pin.

    uint32_t frequency;
    bool bus_initialized;
    bool in_transaction;
    bool has_pending_command;
    uint8_t pending_command;
    bool transfer_in_progress;
    uint8_t active_buffer;
    uint8_t inflight_transfers;
    size_t dma_buffer_size;
    uint8_t *dma_buffer[2];

    // Signaled from ISR when panel IO transfer completes.
    SemaphoreHandle_t transfer_done_sem;
} qspibus_qspibus_obj_t;
