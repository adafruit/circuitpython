// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mpconfig.h"
#include "supervisor/shared/async_flag.h"
#include "driver/spi_master.h"
#include "shared-bindings/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;

    const mcu_pin_obj_t *MOSI;
    const mcu_pin_obj_t *MISO;
    const mcu_pin_obj_t *clock;

    spi_host_device_t host_id;

    uint8_t bits;
    uint8_t phase;
    uint8_t polarity;
    uint32_t baudrate;            // Actual frequency, reported by the frequency property.
    uint32_t requested_baudrate;  // Value passed to configure(); used for the cache-hit check.

    SemaphoreHandle_t mutex;
    #if CIRCUITPY_BUSIO_SPI_ASYNC
    spi_transaction_t async_trans[2];   // queued by write_start, collected by end
    uint8_t async_count;
    circuitpy_async_flag_t *async_done;
    #endif
} busio_spi_obj_t;
