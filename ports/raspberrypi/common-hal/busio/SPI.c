// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/SPI.h"

#include "shared/runtime/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "supervisor/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "hardware/dma.h"
#include "hardware/gpio.h"
#if CIRCUITPY_ASYNC_SPI
#include "hardware/irq.h"
#endif

#define NO_INSTANCE 0xff

#if CIRCUITPY_ASYNC_SPI
// Done flag of the async transfer on each kept RX channel, set when that channel finishes.
static circuitpy_async_flag_t *async_flags[NUM_DMA_CHANNELS];
// RX channels with the interrupt enabled.
static uint32_t async_irq_mask;

// Shared DMA_IRQ_0 handler: acknowledges and services only our channels, like audio_dma and rp2pio.
static void __not_in_flash_func(spi_dma_irq_handler)(void) {
    uint32_t pending = dma_hw->ints0 & async_irq_mask;
    dma_hw->ints0 = pending;
    for (uint chan = 0; pending != 0; chan++, pending >>= 1) {
        if ((pending & 1) && async_flags[chan] != NULL) {
            CIRCUITPY_ASYNC_FLAG_SET(async_flags[chan]);
        }
    }
}

static void async_irq_enable(uint chan) {
    dma_hw->ints0 = 1u << chan;
    if (async_irq_mask == 0) {
        irq_add_shared_handler(DMA_IRQ_0, spi_dma_irq_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    }
    async_irq_mask |= 1u << chan;
    dma_irqn_set_channel_enabled(0, chan, true);
    irq_set_enabled(DMA_IRQ_0, true);
}

static void async_irq_disable(uint chan) {
    dma_irqn_set_channel_enabled(0, chan, false);
    async_irq_mask &= ~(1u << chan);
    if (async_irq_mask == 0) {
        irq_remove_handler(DMA_IRQ_0, spi_dma_irq_handler);
    }
    if (dma_hw->inte0 == 0) {
        irq_set_enabled(DMA_IRQ_0, false);
    }
}
#endif

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex) {
    size_t instance_index = NO_INSTANCE;

    // Ensure the object starts in its deinit state.
    common_hal_busio_spi_mark_deinit(self);

    if (half_duplex) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_half_duplex);
    }

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
        raise_ValueError_invalid_pins();
    }

    if (instance_index == 0) {
        self->peripheral = spi0;
    } else if (instance_index == 1) {
        self->peripheral = spi1;
    }

    if ((spi_get_hw(self->peripheral)->cr1 & SPI_SSPCR1_SSE_BITS) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("SPI peripheral in use"));
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
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    common_hal_never_reset_pin(self->clock);
    common_hal_never_reset_pin(self->MOSI);
    common_hal_never_reset_pin(self->MISO);
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->clock == NULL;
}

void common_hal_busio_spi_mark_deinit(busio_spi_obj_t *self) {
    self->clock = NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
    common_hal_busio_spi_end(self);
    if (self->dma_kept) {
        #if CIRCUITPY_ASYNC_SPI
        async_irq_disable(self->dma_rx);
        #endif
        dma_channel_unclaim(self->dma_tx);
        dma_channel_unclaim(self->dma_rx);
        self->dma_kept = false;
    }
    spi_deinit(self->peripheral);

    common_hal_reset_pin(self->clock);
    common_hal_reset_pin(self->MOSI);
    common_hal_reset_pin(self->MISO);

    common_hal_busio_spi_mark_deinit(self);
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    if (baudrate == self->target_frequency &&
        polarity == self->polarity &&
        phase == self->phase &&
        bits == self->bits) {
        return true;
    }

    // A running async transfer finishes with the old settings.
    common_hal_busio_spi_end(self);

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

    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return false;
    }
    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

// Start a transfer. With DMA it runs in the background and _end() finishes it; otherwise it is
// done in software before this returns. An out or in buffer shorter than the transfer is one
// byte repeated or dropped. An async transfer passes its done flag, and keeps the DMA channels
// until deinit, for buses that send often. Returns whether DMA is running.
static bool _start(busio_spi_obj_t *self,
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
            #if CIRCUITPY_ASYNC_SPI
            if (done != NULL) {
                async_irq_enable(chan_rx);
            }
            #endif
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

        #if CIRCUITPY_ASYNC_SPI
        // Before the start, so that a short transfer cannot finish unseen.
        async_flags[self->dma_rx] = done;
        #endif
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
static void _end(busio_spi_obj_t *self, bool background_tasks) {
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

static bool _transfer(busio_spi_obj_t *self,
    const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len) {
    if (_start(self, data_out, out_len, data_in, in_len, NULL)) {
        // TODO: We should idle here until we get a DMA interrupt or something else.
        _end(self, true);
    }
    return true;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    uint32_t data_in;
    return _transfer(self, data, len, (uint8_t *)&data_in, MIN(len, 4));
}

// Start an async transfer once the previous one has ended; done is set when it has finished,
// here if it ran in software.
static MP_NOINLINE void _async_start(busio_spi_obj_t *self, const uint8_t *data_out, size_t out_len,
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

void common_hal_busio_spi_write_start(busio_spi_obj_t *self, const uint8_t *data, size_t len,
    circuitpy_async_flag_t *done) {
    common_hal_busio_spi_end(self);
    _async_start(self, data, len, &self->one_byte, 1, done);
}

#if CIRCUITPY_ASYNC_SPI
void common_hal_busio_spi_read_start(busio_spi_obj_t *self, uint8_t *data, size_t len,
    uint8_t write_value, circuitpy_async_flag_t *done) {
    // End the previous transfer first: an async write may still be receiving into one_byte.
    common_hal_busio_spi_end(self);
    self->one_byte = write_value;
    _async_start(self, &self->one_byte, 1, data, len, done);
}

void common_hal_busio_spi_transfer_start(busio_spi_obj_t *self, const uint8_t *data_out,
    uint8_t *data_in, size_t len, circuitpy_async_flag_t *done) {
    common_hal_busio_spi_end(self);
    _async_start(self, data_out, len, data_in, len, done);
}

void common_hal_busio_spi_stop(busio_spi_obj_t *self, circuitpy_async_flag_t *done) {
    if (!self->async_active || self->async_done != done) {
        return;
    }
    if (CIRCUITPY_ASYNC_FLAG_IS_SET(done)) {
        common_hal_busio_spi_end(self);
        return;
    }
    async_flags[self->dma_rx] = NULL;
    dma_channel_abort(self->dma_tx);
    dma_channel_abort(self->dma_rx);
    self->async_active = false;
    while (spi_is_busy(self->peripheral)) {
    }
    while (spi_is_readable(self->peripheral)) {
        (void)spi_get_hw(self->peripheral)->dr;
    }
    spi_get_hw(self->peripheral)->icr = SPI_SSPICR_RORIC_BITS;
}
#endif

void common_hal_busio_spi_end(busio_spi_obj_t *self) {
    if (self->async_active) {
        // No background tasks here: the caller holds the bus, and one of them may want it.
        _end(self, false);
        self->async_active = false;
        #if CIRCUITPY_ASYNC_SPI
        async_flags[self->dma_rx] = NULL;
        #endif
        CIRCUITPY_ASYNC_FLAG_SET(self->async_done);
    }
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    uint32_t data_out = write_value << 24 | write_value << 16 | write_value << 8 | write_value;
    return _transfer(self, (const uint8_t *)&data_out, MIN(4, len), data, len);
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len) {
    return _transfer(self, data_out, len, data_in, len);
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->real_frequency;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}
