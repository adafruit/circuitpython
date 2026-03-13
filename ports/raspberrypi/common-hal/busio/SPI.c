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

struct spi_transfer_state {
    uint dma_channel;
    uint paired_rx_channel;
    uint8_t repeated_tx_data;
    uint8_t discard_rx_data;
};

typedef struct spi_transfer_state spi_transfer_state;

static spi_transfer_state spi_dma_channel_state[NUM_DMA_CHANNELS];

#define NO_INSTANCE 0xff

static spi_transfer_state *common_hal_spi_write_dma(spi_inst_t *spi, const uint8_t *src, size_t len) {
    invalid_params_if(HARDWARE_SPI, 0 > (int)len);
    invalid_params_if(HARDWARE_SPI, len == 0);

    uint tx_dma_channel = dma_claim_unused_channel(true);
    uint rx_dma_channel = dma_claim_unused_channel(true);
    spi_transfer_state *state = &spi_dma_channel_state[tx_dma_channel];
    state->dma_channel = tx_dma_channel;

    state->paired_rx_channel = rx_dma_channel;
    state->discard_rx_data = 0;

    dma_channel_config tx_config = dma_channel_get_default_config(tx_dma_channel);
    channel_config_set_transfer_data_size(&tx_config, DMA_SIZE_8);
    channel_config_set_read_increment(&tx_config, true);
    channel_config_set_write_increment(&tx_config, false);
    channel_config_set_dreq(&tx_config, SPI_DREQ_NUM(spi, true));
    dma_channel_configure(tx_dma_channel, &tx_config, &spi_get_hw(spi)->dr, src, len, false);

    dma_channel_config rx_config = dma_channel_get_default_config(rx_dma_channel);
    channel_config_set_transfer_data_size(&rx_config, DMA_SIZE_8);
    channel_config_set_read_increment(&rx_config, false);
    channel_config_set_write_increment(&rx_config, false);
    channel_config_set_dreq(&rx_config, SPI_DREQ_NUM(spi, false));
    dma_channel_configure(rx_dma_channel, &rx_config, &state->discard_rx_data,
        &spi_get_hw(spi)->dr, len, false);

    dma_start_channel_mask((1u << tx_dma_channel) | (1u << rx_dma_channel));
    return state;
}

static spi_transfer_state *common_hal_spi_read_dma(spi_inst_t *spi, uint8_t repeated_tx_data, uint8_t *dst, size_t len) {
    invalid_params_if(HARDWARE_SPI, 0 > (int)len);
    invalid_params_if(HARDWARE_SPI, len == 0);

    uint tx_dma_channel = dma_claim_unused_channel(true);
    uint rx_dma_channel = dma_claim_unused_channel(true);
    spi_transfer_state *state = &spi_dma_channel_state[tx_dma_channel];
    state->dma_channel = tx_dma_channel;

    state->paired_rx_channel = rx_dma_channel;
    state->repeated_tx_data = repeated_tx_data;

    dma_channel_config tx_config = dma_channel_get_default_config(tx_dma_channel);
    channel_config_set_transfer_data_size(&tx_config, DMA_SIZE_8);
    channel_config_set_read_increment(&tx_config, false);
    channel_config_set_write_increment(&tx_config, false);
    channel_config_set_dreq(&tx_config, SPI_DREQ_NUM(spi, true));
    dma_channel_configure(tx_dma_channel, &tx_config, &spi_get_hw(spi)->dr,
        &state->repeated_tx_data, len, false);

    dma_channel_config rx_config = dma_channel_get_default_config(rx_dma_channel);
    channel_config_set_transfer_data_size(&rx_config, DMA_SIZE_8);
    channel_config_set_read_increment(&rx_config, false);
    channel_config_set_write_increment(&rx_config, true);
    channel_config_set_dreq(&rx_config, SPI_DREQ_NUM(spi, false));
    dma_channel_configure(rx_dma_channel, &rx_config, dst, &spi_get_hw(spi)->dr, len, false);

    dma_start_channel_mask((1u << tx_dma_channel) | (1u << rx_dma_channel));
    return state;
}

static spi_transfer_state *common_hal_spi_write_read_dma(spi_inst_t *spi, const uint8_t *src, uint8_t *dst, size_t len) {
    invalid_params_if(HARDWARE_SPI, 0 > (int)len);
    invalid_params_if(HARDWARE_SPI, len == 0);

    uint tx_dma_channel = dma_claim_unused_channel(true);
    uint rx_dma_channel = dma_claim_unused_channel(true);
    spi_transfer_state *state = &spi_dma_channel_state[tx_dma_channel];
    state->dma_channel = tx_dma_channel;

    state->paired_rx_channel = rx_dma_channel;

    dma_channel_config tx_config = dma_channel_get_default_config(tx_dma_channel);
    channel_config_set_transfer_data_size(&tx_config, DMA_SIZE_8);
    channel_config_set_read_increment(&tx_config, true);
    channel_config_set_write_increment(&tx_config, false);
    channel_config_set_dreq(&tx_config, SPI_DREQ_NUM(spi, true));
    dma_channel_configure(tx_dma_channel, &tx_config, &spi_get_hw(spi)->dr, src, len, false);

    dma_channel_config rx_config = dma_channel_get_default_config(rx_dma_channel);
    channel_config_set_transfer_data_size(&rx_config, DMA_SIZE_8);
    channel_config_set_read_increment(&rx_config, false);
    channel_config_set_write_increment(&rx_config, true);
    channel_config_set_dreq(&rx_config, SPI_DREQ_NUM(spi, false));
    dma_channel_configure(rx_dma_channel, &rx_config, dst, &spi_get_hw(spi)->dr, len, false);

    dma_start_channel_mask((1u << tx_dma_channel) | (1u << rx_dma_channel));
    return state;
}

static bool common_hal_spi_dma_is_busy(spi_transfer_state *state) {
    if (state == NULL) {
        return false;
    }
    uint dma_channel = state->dma_channel;
    if (dma_channel_is_busy(dma_channel)) {
        return true;
    }

    uint rx_dma_channel = state->paired_rx_channel;
    if (rx_dma_channel < NUM_DMA_CHANNELS && dma_channel_is_busy(rx_dma_channel)) {
        return true;
    }

    if (rx_dma_channel < NUM_DMA_CHANNELS) {
        dma_channel_unclaim(rx_dma_channel);
    }
    dma_channel_unclaim(dma_channel);

    state->paired_rx_channel = NUM_DMA_CHANNELS;
    return false;
}

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

static bool _transfer(busio_spi_obj_t *self,
    const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len) {
    // Use DMA for large transfers if channels are available
    const size_t dma_min_size_threshold = 32;
    int chan_tx = -1;
    int chan_rx = -1;
    size_t len = MAX(out_len, in_len);
    if (len >= dma_min_size_threshold) {
        // Use two DMA channels to service the two FIFOs
        chan_tx = dma_claim_unused_channel(false);
        chan_rx = dma_claim_unused_channel(false);
    }
    bool has_dma_channels = chan_rx >= 0 && chan_tx >= 0;
    // Only use DMA if both data buffers are in SRAM. Otherwise, we'll stall the DMA with PSRAM or flash cache misses.
    bool data_in_sram = data_in >= (uint8_t *)SRAM_BASE && data_out >= (uint8_t *)SRAM_BASE;
    bool use_dma = has_dma_channels && data_in_sram;
    if (use_dma) {
        dma_channel_config c = dma_channel_get_default_config(chan_tx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->peripheral) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
        channel_config_set_read_increment(&c, out_len == len);
        channel_config_set_write_increment(&c, false);
        dma_channel_configure(chan_tx, &c,
            &spi_get_hw(self->peripheral)->dr,
            data_out,
            len,
            false);

        c = dma_channel_get_default_config(chan_rx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->peripheral) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, in_len == len);
        dma_channel_configure(chan_rx, &c,
            data_in,
            &spi_get_hw(self->peripheral)->dr,
            len,
            false);

        dma_start_channel_mask((1u << chan_rx) | (1u << chan_tx));
        while (dma_channel_is_busy(chan_rx) || dma_channel_is_busy(chan_tx)) {
            // TODO: We should idle here until we get a DMA interrupt or something else.
            RUN_BACKGROUND_TASKS;
        }
    }

    // If we have claimed only one channel successfully, we should release immediately. This also
    // releases the DMA after use_dma has been done.
    if (chan_rx >= 0) {
        dma_channel_unclaim(chan_rx);
    }
    if (chan_tx >= 0) {
        dma_channel_unclaim(chan_tx);
    }

    if (!use_dma) {
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
    }
    return true;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    uint32_t data_in;
    return _transfer(self, data, len, (uint8_t *)&data_in, MIN(len, 4));
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

spi_transfer_state *common_hal_busio_spi_start_transfer(busio_spi_obj_t *spi, const uint8_t *out_data, uint8_t *in_data, size_t len) {
    if (out_data == NULL && in_data == NULL) {
        return NULL;
    }
    if (out_data == NULL) {
        return common_hal_spi_read_dma(spi->peripheral, 0, in_data, len);
    }
    if (in_data == NULL) {
        return common_hal_spi_write_dma(spi->peripheral, out_data, len);
    }
    return common_hal_spi_write_read_dma(spi->peripheral, out_data, in_data, len);
}

bool common_hal_busio_spi_transfer_isbusy(spi_transfer_state *state) {
    return common_hal_spi_dma_is_busy(state);
}
