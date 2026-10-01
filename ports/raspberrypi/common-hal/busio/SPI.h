// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"
#include "supervisor/shared/async_flag.h"

#include "hardware/spi.h"

typedef struct {
    mp_obj_base_t base;
    spi_inst_t *peripheral;
    bool has_lock;
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *MOSI;
    const mcu_pin_obj_t *MISO;
    uint32_t target_frequency;
    int32_t real_frequency;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    bool async_active;          // an async DMA transfer may still be running
    bool dma_kept;              // dma_tx and dma_rx are ours until deinit
    uint8_t dma_tx;
    uint8_t dma_rx;
    uint8_t one_byte;           // the one-byte side of an async write (RX) or read (TX)
    circuitpy_async_flag_t *async_done;
} busio_spi_obj_t;
