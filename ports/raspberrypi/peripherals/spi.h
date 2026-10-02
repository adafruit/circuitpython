// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// An SPI bus of this port, shared by busio.SPI and async_spi.SPI.

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "supervisor/shared/async_flag.h"

#include "hardware/spi.h"

typedef struct {
    spi_inst_t *peripheral;
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
} rp2_spi_t;

typedef enum {
    RP2_SPI_OK,
    RP2_SPI_INVALID_PINS,       // the pins do not make an SPI bus
    RP2_SPI_IN_USE,             // the peripheral is in use
} rp2_spi_error_t;

rp2_spi_error_t rp2_spi_construct(rp2_spi_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso);
// Finishes a running async transfer first.
void rp2_spi_deinit(rp2_spi_t *self);
static inline bool rp2_spi_deinited(rp2_spi_t *self) {
    return self->clock == NULL;
}
void rp2_spi_never_reset(rp2_spi_t *self);
// Waits for a running async transfer first.
void rp2_spi_configure(rp2_spi_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase,
    uint8_t bits);

// A blocking transfer. A one-byte side is sent repeatedly or overwritten.
void rp2_spi_transfer(rp2_spi_t *self, const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len);

// An async transfer: done is set by rp2_spi_end() once the transfer has finished. The buffer
// must stay valid until then.
void rp2_spi_write_start(rp2_spi_t *self, const uint8_t *data, size_t len,
    circuitpy_async_flag_t *done);
// Waits for the running async transfer, if any, and sets its done flag.
void rp2_spi_end(rp2_spi_t *self);
