// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "common-hal/busio/SPI.h"
#include "py/circuitpy_objawaitable.h"

// Context passed through the awaitable for every SPI DMA operation.
typedef struct {
    busio_spi_obj_t *spi;
    const uint8_t *out_data;
    uint8_t *in_data;
    size_t len;
    uint tx_channel;
    uint rx_channel;
    circuitpy_async_flag_t *flag;
    uint8_t repeated_tx_data;           // TX fill byte when writing only
    uint8_t discard_rx_data;            // RX sink byte when reading only
} abusio_spi_transfer_ctx_t;
