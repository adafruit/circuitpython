// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mpconfig.h"
#if CIRCUITPY_BUSIO_SPI_ASYNC
#include "peripherals/samd/dma.h"
#endif
#include "common-hal/microcontroller/Pin.h"

#include "hal/include/hal_spi_m_sync.h"

#include "py/obj.h"
#include "supervisor/shared/async_flag.h"

typedef struct {
    mp_obj_base_t base;
    struct spi_m_sync_descriptor spi_desc;
    bool has_lock;
    uint8_t clock_pin;
    uint8_t MOSI_pin;
    uint8_t MISO_pin;
    #if CIRCUITPY_BUSIO_SPI_ASYNC
    bool async_active;          // a write_start DMA transfer may still be running
    dma_transfer_t async_xfer;
    circuitpy_async_flag_t *async_done;
    #endif
} busio_spi_obj_t;
