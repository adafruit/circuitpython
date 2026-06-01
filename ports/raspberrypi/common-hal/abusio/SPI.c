// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// rp2040/rp2350 common-hal implementation of abusio.SPI async DMA transfers.
//
// Each transfer (write, readinto, write_readinto) claims two DMA channels,
// configures them exactly as busio/SPI.c's static DMA helpers do, then enables
// the DMA_IRQ_0 completion interrupt on the RX channel.  The shared IRQ handler
// sets the circuitpy_async_flag_t so the awaitable loop can proceed.
//
// The IRQ is installed with irq_add_shared_handler() so it coexists with the
// audio DMA handler in audio_dma.c.

#include "ports/raspberrypi/common-hal/abusio/SPI.h"
#include "shared-bindings/abusio/SPI.h"

#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/circuitpy_objawaitable.h"

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/spi.h"

#if MICROPY_PY_ASYNC_AWAIT

// ---------------------------------------------------------------------------
// IRQ completion table
// One slot per DMA channel; indexed by the RX channel number.
// The IRQ handler fires when the RX channel finishes (i.e. the full transfer
// is complete, because TX always finishes at the same time or before RX).
// ---------------------------------------------------------------------------

static circuitpy_async_flag_t *abusio_spi_rx_flags[NUM_DMA_CHANNELS];

static void __not_in_flash_func(abusio_spi_dma_irq)(void) {
    uint32_t ints = dma_hw->ints1;
    // Clear ALL pending bits upfront.  If we only clear bits for channels we
    // own and skip unowned ones, the interrupt re-fires immediately and the
    // CPU is stuck in an infinite ISR loop.
    dma_hw->ints1 = ints;
    for (uint i = 0; i < NUM_DMA_CHANNELS; i++) {
        uint32_t mask = 1u << i;
        if ((ints & mask) == 0) {
            continue;
        }
        if (abusio_spi_rx_flags[i] == NULL) {
            continue;
        }
        CIRCUITPY_ASYNC_FLAG_SET(abusio_spi_rx_flags[i]);
        abusio_spi_rx_flags[i] = NULL;
    }
}

static void ensure_irq_installed(void) {
    static bool installed = false;
    if (!installed) {
        irq_set_exclusive_handler(DMA_IRQ_1, abusio_spi_dma_irq);
        irq_set_enabled(DMA_IRQ_1, true);
        installed = true;
    }
}

// ---------------------------------------------------------------------------
// Internal DMA setup helpers
// ---------------------------------------------------------------------------

static abusio_spi_transfer_ctx_t *setup_dma_write(
    circuitpy_async_flag_t *flag, busio_spi_obj_t *spi,
    const uint8_t *src, size_t len) {

    abusio_spi_transfer_ctx_t *ctx = m_new_obj(abusio_spi_transfer_ctx_t);
    ctx->spi = spi;
    ctx->out_data = src;
    ctx->in_data = NULL;
    ctx->len = len;
    ctx->flag = flag;
    ctx->discard_rx_data = 0;

    uint tx = dma_claim_unused_channel(true);
    uint rx = dma_claim_unused_channel(true);
    ctx->tx_channel = tx;
    ctx->rx_channel = rx;

    // TX: stream src → SPI DR
    dma_channel_config tc = dma_channel_get_default_config(tx);
    channel_config_set_transfer_data_size(&tc, DMA_SIZE_8);
    channel_config_set_read_increment(&tc, true);
    channel_config_set_write_increment(&tc, false);
    channel_config_set_dreq(&tc, SPI_DREQ_NUM(spi->peripheral, true));
    dma_channel_configure(tx, &tc, &spi_get_hw(spi->peripheral)->dr, src, len, false);

    // RX: drain SPI DR → discard sink (drives pacing)
    dma_channel_config rc = dma_channel_get_default_config(rx);
    channel_config_set_transfer_data_size(&rc, DMA_SIZE_8);
    channel_config_set_read_increment(&rc, false);
    channel_config_set_write_increment(&rc, false);
    channel_config_set_dreq(&rc, SPI_DREQ_NUM(spi->peripheral, false));
    dma_channel_configure(rx, &rc, &ctx->discard_rx_data,
        &spi_get_hw(spi->peripheral)->dr, len, false);

    // Enable IRQ on RX channel completion.
    abusio_spi_rx_flags[rx] = flag;
    dma_channel_set_irq1_enabled(rx, true);

    dma_start_channel_mask((1u << tx) | (1u << rx));
    return ctx;
}

static abusio_spi_transfer_ctx_t *setup_dma_read(
    circuitpy_async_flag_t *flag, busio_spi_obj_t *spi,
    uint8_t *dst, size_t len, uint8_t write_value) {

    abusio_spi_transfer_ctx_t *ctx = m_new_obj(abusio_spi_transfer_ctx_t);
    ctx->spi = spi;
    ctx->out_data = NULL;
    ctx->in_data = dst;
    ctx->len = len;
    ctx->flag = flag;
    ctx->repeated_tx_data = write_value;

    uint tx = dma_claim_unused_channel(true);
    uint rx = dma_claim_unused_channel(true);
    ctx->tx_channel = tx;
    ctx->rx_channel = rx;

    // TX: repeat write_value → SPI DR
    dma_channel_config tc = dma_channel_get_default_config(tx);
    channel_config_set_transfer_data_size(&tc, DMA_SIZE_8);
    channel_config_set_read_increment(&tc, false);
    channel_config_set_write_increment(&tc, false);
    channel_config_set_dreq(&tc, SPI_DREQ_NUM(spi->peripheral, true));
    dma_channel_configure(tx, &tc, &spi_get_hw(spi->peripheral)->dr,
        &ctx->repeated_tx_data, len, false);

    // RX: SPI DR → dst
    dma_channel_config rc = dma_channel_get_default_config(rx);
    channel_config_set_transfer_data_size(&rc, DMA_SIZE_8);
    channel_config_set_read_increment(&rc, false);
    channel_config_set_write_increment(&rc, true);
    channel_config_set_dreq(&rc, SPI_DREQ_NUM(spi->peripheral, false));
    dma_channel_configure(rx, &rc, dst,
        &spi_get_hw(spi->peripheral)->dr, len, false);

    abusio_spi_rx_flags[rx] = flag;
    dma_channel_set_irq1_enabled(rx, true);

    dma_start_channel_mask((1u << tx) | (1u << rx));
    return ctx;
}

static abusio_spi_transfer_ctx_t *setup_dma_write_read(
    circuitpy_async_flag_t *flag, busio_spi_obj_t *spi,
    const uint8_t *src, uint8_t *dst, size_t len) {

    abusio_spi_transfer_ctx_t *ctx = m_new_obj(abusio_spi_transfer_ctx_t);
    ctx->spi = spi;
    ctx->out_data = src;
    ctx->in_data = dst;
    ctx->len = len;
    ctx->flag = flag;

    uint tx = dma_claim_unused_channel(true);
    uint rx = dma_claim_unused_channel(true);
    ctx->tx_channel = tx;
    ctx->rx_channel = rx;

    dma_channel_config tc = dma_channel_get_default_config(tx);
    channel_config_set_transfer_data_size(&tc, DMA_SIZE_8);
    channel_config_set_read_increment(&tc, true);
    channel_config_set_write_increment(&tc, false);
    channel_config_set_dreq(&tc, SPI_DREQ_NUM(spi->peripheral, true));
    dma_channel_configure(tx, &tc, &spi_get_hw(spi->peripheral)->dr, src, len, false);

    dma_channel_config rc = dma_channel_get_default_config(rx);
    channel_config_set_transfer_data_size(&rc, DMA_SIZE_8);
    channel_config_set_read_increment(&rc, false);
    channel_config_set_write_increment(&rc, true);
    channel_config_set_dreq(&rc, SPI_DREQ_NUM(spi->peripheral, false));
    dma_channel_configure(rx, &rc, dst,
        &spi_get_hw(spi->peripheral)->dr, len, false);

    abusio_spi_rx_flags[rx] = flag;
    dma_channel_set_irq1_enabled(rx, true);

    dma_start_channel_mask((1u << tx) | (1u << rx));
    return ctx;
}

// Unclaim both DMA channels if they haven't been freed already.
static void cancel_dma(abusio_spi_transfer_ctx_t *ctx) {
    if (ctx->rx_channel < NUM_DMA_CHANNELS) {
        dma_channel_set_irq1_enabled(ctx->rx_channel, false);
        abusio_spi_rx_flags[ctx->rx_channel] = NULL;
        dma_channel_abort(ctx->rx_channel);
        dma_channel_unclaim(ctx->rx_channel);
        ctx->rx_channel = NUM_DMA_CHANNELS;
    }
    if (ctx->tx_channel < NUM_DMA_CHANNELS) {
        dma_channel_abort(ctx->tx_channel);
        dma_channel_unclaim(ctx->tx_channel);
        ctx->tx_channel = NUM_DMA_CHANNELS;
    }
}

// ---------------------------------------------------------------------------
// Arg unpacking helpers
// ---------------------------------------------------------------------------

// Unpack a 2-tuple (self_mp_obj, buf_mp_obj) with a READ buffer.
static void unpack2_read(mp_obj_t data,
    abusio_spi_obj_t **self_out,
    const uint8_t **buf_out, size_t *len_out) {
    mp_obj_t *items;
    size_t len;
    mp_obj_tuple_get(data, &len, &items);
    *self_out = MP_OBJ_TO_PTR(items[0]);
    mp_buffer_info_t bi;
    mp_get_buffer_raise(items[1], &bi, MP_BUFFER_READ);
    *buf_out = bi.buf;
    *len_out = bi.len;
}

// Unpack a 3-tuple (self, buf, write_value) with a WRITE buffer.
static void unpack3_write(mp_obj_t data,
    abusio_spi_obj_t **self_out,
    uint8_t **buf_out, size_t *len_out, uint8_t *write_value_out) {
    mp_obj_t *items;
    size_t len;
    mp_obj_tuple_get(data, &len, &items);
    *self_out = MP_OBJ_TO_PTR(items[0]);
    mp_buffer_info_t bi;
    mp_get_buffer_raise(items[1], &bi, MP_BUFFER_WRITE);
    *buf_out = bi.buf;
    *len_out = bi.len;
    *write_value_out = (uint8_t)mp_obj_get_int(items[2]);
}

// Unpack a 3-tuple (self, out_buf, in_buf).
static void unpack3_readwrite(mp_obj_t data,
    abusio_spi_obj_t **self_out,
    const uint8_t **out_out, uint8_t **in_out, size_t *len_out) {
    mp_obj_t *items;
    size_t len;
    mp_obj_tuple_get(data, &len, &items);
    *self_out = MP_OBJ_TO_PTR(items[0]);
    mp_buffer_info_t obi, ibi;
    mp_get_buffer_raise(items[1], &obi, MP_BUFFER_READ);
    mp_get_buffer_raise(items[2], &ibi, MP_BUFFER_WRITE);
    *out_out = obi.buf;
    *in_out = ibi.buf;
    *len_out = obi.len;
}

// ---------------------------------------------------------------------------
// write start / end / cancel
// ---------------------------------------------------------------------------

void *common_hal_abusio_spi_write_start(circuitpy_async_flag_t *flag, mp_obj_t data) {
    abusio_spi_obj_t *self;
    const uint8_t *buf;
    size_t len;
    unpack2_read(data, &self, &buf, &len);
    ensure_irq_installed();
    return setup_dma_write(flag, &self->spi, buf, len);
}

mp_obj_t common_hal_abusio_spi_write_end(void *ctx_in) {
    abusio_spi_transfer_ctx_t *ctx = ctx_in;
    cancel_dma(ctx);   // unclaims only; channels are already idle
    return mp_const_none;
}

void common_hal_abusio_spi_write_cancel(void *ctx_in) {
    cancel_dma(ctx_in);
}

// ---------------------------------------------------------------------------
// readinto start / end / cancel
// ---------------------------------------------------------------------------

void *common_hal_abusio_spi_readinto_start(circuitpy_async_flag_t *flag, mp_obj_t data) {
    abusio_spi_obj_t *self;
    uint8_t *buf;
    size_t len;
    uint8_t write_value;
    unpack3_write(data, &self, &buf, &len, &write_value);
    ensure_irq_installed();
    return setup_dma_read(flag, &self->spi, buf, len, write_value);
}

mp_obj_t common_hal_abusio_spi_readinto_end(void *ctx_in) {
    abusio_spi_transfer_ctx_t *ctx = ctx_in;
    cancel_dma(ctx);
    return mp_const_none;
}

void common_hal_abusio_spi_readinto_cancel(void *ctx_in) {
    cancel_dma(ctx_in);
}

// ---------------------------------------------------------------------------
// write_readinto start / end / cancel
// ---------------------------------------------------------------------------

void *common_hal_abusio_spi_write_readinto_start(circuitpy_async_flag_t *flag, mp_obj_t data) {
    abusio_spi_obj_t *self;
    const uint8_t *out;
    uint8_t *in;
    size_t len;
    unpack3_readwrite(data, &self, &out, &in, &len);
    ensure_irq_installed();
    return setup_dma_write_read(flag, &self->spi, out, in, len);
}

mp_obj_t common_hal_abusio_spi_write_readinto_end(void *ctx_in) {
    abusio_spi_transfer_ctx_t *ctx = ctx_in;
    cancel_dma(ctx);
    return mp_const_none;
}

void common_hal_abusio_spi_write_readinto_cancel(void *ctx_in) {
    cancel_dma(ctx_in);
}

#endif // MICROPY_PY_ASYNC_AWAIT
