// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "peripherals/spi.h"

#include "py/mpconfig.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/background_callback.h"

#include "hardware/dma.h"
#include "hardware/gpio.h"

#define NO_INSTANCE 0xff


rp2_spi_error_t rp2_spi_construct(rp2_spi_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    size_t instance_index = NO_INSTANCE;

    // Ensure the object starts in its deinit state.
    self->clock = NULL;

    if (clock->number % 4 == 2) {
        instance_index = (clock->number / 8) % 2;
    }
    if (mosi != NULL) {
        // Make sure the set MOSI matches the clock settings.
        if (mosi->number % 4 != 3 ||
            (mosi->number / 8) % 2 != instance_index) {
            instance_index = NO_INSTANCE;
        }
    }
    if (miso != NULL) {
        // Make sure the set MOSI matches the clock settings.
        if (miso->number % 4 != 0 ||
            (miso->number / 8) % 2 != instance_index) {
            instance_index = NO_INSTANCE;
        }
    }

    // TODO: Check to see if we're sharing the SPI with a native APA102.

    if (instance_index > 1) {
        return RP2_SPI_INVALID_PINS;
    }

    if (instance_index == 0) {
        self->peripheral = spi0;
    } else if (instance_index == 1) {
        self->peripheral = spi1;
    }

    if ((spi_get_hw(self->peripheral)->cr1 & SPI_SSPCR1_SSE_BITS) != 0) {
        return RP2_SPI_IN_USE;
    }

    self->async_active = false;
    self->dma_kept = false;
    self->target_frequency = 250000;
    self->real_frequency = spi_init(self->peripheral, self->target_frequency);

    gpio_set_function(clock->number, GPIO_FUNC_SPI);
    claim_pin(clock);
    self->clock = clock;

    self->MOSI = mosi;
    if (mosi != NULL) {
        gpio_set_function(mosi->number, GPIO_FUNC_SPI);
        claim_pin(mosi);
    }

    self->MISO = miso;
    if (miso != NULL) {
        gpio_set_function(miso->number, GPIO_FUNC_SPI);
        claim_pin(miso);
    }
    return RP2_SPI_OK;
}

void rp2_spi_never_reset(rp2_spi_t *self) {
    common_hal_never_reset_pin(self->clock);
    common_hal_never_reset_pin(self->MOSI);
    common_hal_never_reset_pin(self->MISO);
}


void rp2_spi_deinit(rp2_spi_t *self) {
    if (rp2_spi_deinited(self)) {
        return;
    }
    if (self->dma_kept) {
        dma_channel_unclaim(self->dma_tx);
        dma_channel_unclaim(self->dma_rx);
        self->dma_kept = false;
    }
    spi_deinit(self->peripheral);

    common_hal_reset_pin(self->clock);
    common_hal_reset_pin(self->MOSI);
    common_hal_reset_pin(self->MISO);

    self->clock = NULL;
}

static void _apply(rp2_spi_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase,
    uint8_t bits) {
    if (baudrate == self->target_frequency &&
        polarity == self->polarity &&
        phase == self->phase &&
        bits == self->bits) {
        return;
    }

    spi_set_format(self->peripheral, bits, polarity, phase, SPI_MSB_FIRST);

    // Workaround to start with clock line high if polarity=1. The hw SPI peripheral does not do this
    // automatically. See https://github.com/raspberrypi/pico-sdk/issues/868 and
    // https://forums.raspberrypi.com/viewtopic.php?t=336142
    // TODO: scheduled to be be fixed in pico-sdk 1.5.0.
    if (polarity) {
        hw_clear_bits(&spi_get_hw(self->peripheral)->cr1, SPI_SSPCR1_SSE_BITS); // disable the SPI
        hw_set_bits(&spi_get_hw(self->peripheral)->cr1, SPI_SSPCR1_SSE_BITS); // re-enable the SPI
    }

    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
    self->target_frequency = baudrate;
    self->real_frequency = spi_set_baudrate(self->peripheral, baudrate);
}

void rp2_spi_configure(rp2_spi_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase,
    uint8_t bits) {
    // A running async transfer finishes with the old settings.
    rp2_spi_end(self);
    _apply(self, baudrate, polarity, phase, bits);
}


// Start a transfer. With DMA it runs in the background and _end() finishes it; otherwise it is
// done in software before this returns. An out or in buffer shorter than the transfer is one
// byte repeated or dropped. An async transfer passes its done flag, and keeps the DMA channels
// until deinit, for buses that send often. Returns whether DMA is running.
static bool _start(rp2_spi_t *self,
    const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len, circuitpy_async_flag_t *done) {
    size_t len = MAX(out_len, in_len);
    // Only use DMA if both data buffers are in SRAM. Otherwise, we'll stall the DMA with PSRAM or flash cache misses.
    bool use_dma = len >= 32 && data_in >= (uint8_t *)SRAM_BASE && data_out >= (uint8_t *)SRAM_BASE;
    if (use_dma && !self->dma_kept) {
        int chan_tx = dma_claim_unused_channel(false);
        int chan_rx = dma_claim_unused_channel(false);
        if (chan_tx >= 0 && chan_rx >= 0) {
            self->dma_tx = chan_tx;
            self->dma_rx = chan_rx;
            self->dma_kept = done != NULL;
        } else {
            // If we have claimed only one channel successfully, release it.
            if (chan_tx >= 0) {
                dma_channel_unclaim(chan_tx);
            }
            if (chan_rx >= 0) {
                dma_channel_unclaim(chan_rx);
            }
            use_dma = false;
        }
    }
    if (use_dma) {
        dma_channel_config c = dma_channel_get_default_config(self->dma_tx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->peripheral) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
        channel_config_set_read_increment(&c, out_len == len);
        channel_config_set_write_increment(&c, false);
        dma_channel_configure(self->dma_tx, &c,
            &spi_get_hw(self->peripheral)->dr,
            data_out,
            len,
            false);

        c = dma_channel_get_default_config(self->dma_rx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->peripheral) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, in_len == len);
        dma_channel_configure(self->dma_rx, &c,
            data_in,
            &spi_get_hw(self->peripheral)->dr,
            len,
            false);

        dma_start_channel_mask((1u << self->dma_rx) | (1u << self->dma_tx));
        return true;
    }

    // Use software for small transfers, or if couldn't claim two DMA channels
    // Never have more transfers in flight than will fit into the RX FIFO,
    // else FIFO will overflow if this code is heavily interrupted.
    const size_t fifo_depth = 8;
    size_t rx_remaining = len;
    size_t tx_remaining = len;

    while (rx_remaining || tx_remaining) {
        if (tx_remaining && spi_is_writable(self->peripheral) && rx_remaining - tx_remaining < fifo_depth) {
            spi_get_hw(self->peripheral)->dr = (uint32_t)*data_out;
            // Increment only if the buffer is the transfer length. It's 1 otherwise.
            if (out_len == len) {
                data_out++;
            }
            --tx_remaining;
        }
        if (rx_remaining && spi_is_readable(self->peripheral)) {
            *data_in = (uint8_t)spi_get_hw(self->peripheral)->dr;
            // Increment only if the buffer is the transfer length. It's 1 otherwise.
            if (in_len == len) {
                data_in++;
            }
            --rx_remaining;
        }
        RUN_BACKGROUND_TASKS;
    }
    return false;
}

// Wait for a DMA transfer started by _start(). The RX channel finishes last.
static void _end(rp2_spi_t *self, bool background_tasks) {
    while (dma_channel_is_busy(self->dma_rx)) {
        if (background_tasks) {
            RUN_BACKGROUND_TASKS;
        }
    }
    if (!self->dma_kept) {
        dma_channel_unclaim(self->dma_tx);
        dma_channel_unclaim(self->dma_rx);
    }
}

void rp2_spi_transfer(rp2_spi_t *self, const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len) {
    if (_start(self, data_out, out_len, data_in, in_len, NULL)) {
        // TODO: We should idle here until we get a DMA interrupt or something else.
        _end(self, true);
    }
}

// Start an async transfer; done is set when it has finished, here if it ran in software.
static __attribute__((noinline)) void _async_start(rp2_spi_t *self, const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len, circuitpy_async_flag_t *done) {
    CIRCUITPY_ASYNC_FLAG_INIT(done);
    self->async_done = done;
    // An empty data side means there is nothing to send, whatever the one-byte side holds.
    self->async_active = out_len != 0 && in_len != 0 &&
        _start(self, data_out, out_len, data_in, in_len, done);
    if (!self->async_active) {
        CIRCUITPY_ASYNC_FLAG_SET(done);
    }
}

void rp2_spi_write_start(rp2_spi_t *self, const uint8_t *data, size_t len,
    circuitpy_async_flag_t *done) {
    rp2_spi_end(self);
    _async_start(self, data, len, &self->one_byte, 1, done);
}


void rp2_spi_end(rp2_spi_t *self) {
    if (self->async_active) {
        // No background tasks here: the caller holds the bus, and one of them may want it.
        _end(self, false);
        self->async_active = false;
        CIRCUITPY_ASYNC_FLAG_SET(self->async_done);
    }
}
