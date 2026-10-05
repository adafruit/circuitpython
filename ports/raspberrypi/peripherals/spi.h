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
    #if CIRCUITPY_ASYNC_SPI
    // A configure() waiting for the transfer to finish, and its settings.
    circuitpy_async_flag_t *idle_done;
    uint32_t pending_baudrate;
    uint8_t pending_polarity;
    uint8_t pending_phase;
    uint8_t pending_bits;
    #endif
} rp2_spi_t;

typedef enum {
    RP2_SPI_OK,
    RP2_SPI_INVALID_PINS,       // the pins do not make an SPI bus
    RP2_SPI_IN_USE,             // the peripheral is in use
} rp2_spi_error_t;

rp2_spi_error_t rp2_spi_construct(rp2_spi_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso);
// Stops a running async transfer, and sets its done flag.
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

// Async transfers: done is set once the transfer has finished, by the DMA interrupt with
// CIRCUITPY_ASYNC_SPI and in any case by rp2_spi_end(). The buffers must stay valid until then.
void rp2_spi_write_start(rp2_spi_t *self, const uint8_t *data, size_t len,
    circuitpy_async_flag_t *done);
void rp2_spi_read_start(rp2_spi_t *self, uint8_t *data, size_t len, uint8_t write_value,
    circuitpy_async_flag_t *done);
void rp2_spi_transfer_start(rp2_spi_t *self, const uint8_t *data_out, uint8_t *data_in,
    size_t len, circuitpy_async_flag_t *done);
// Waits for the running async transfer, if any, and sets its done flag.
void rp2_spi_end(rp2_spi_t *self);
// Stops the transfer started with done, or finishes it if done is set. Does nothing for an
// earlier transfer, and does not allocate.
void rp2_spi_stop(rp2_spi_t *self, circuitpy_async_flag_t *done);
// Configures the bus once the running transfer, if any, has finished, and sets done then.
void rp2_spi_configure_start(rp2_spi_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase,
    uint8_t bits, circuitpy_async_flag_t *done);
// Drops the waiting configure() started with done.
void rp2_spi_configure_cancel(rp2_spi_t *self, circuitpy_async_flag_t *done);
