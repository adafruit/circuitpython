// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "common-hal/picogame/Display.h"

#include "py/runtime.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/displayio/display_core.h"
#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/fourwire/FourWire.h"

#include "hardware/spi.h"
#include "hardware/dma.h"

// Claimed once and REUSED across every Display construct (incl. across soft resets / each game
// launched via supervisor.set_next_code_file). We claim via the raw pico-sdk, which CircuitPython
// does NOT release on a soft reset -- so claiming per-construct leaked a channel every game until
// "*** PANIC *** No DMA channels available" (and starved board.DISPLAY's own DMA, halving FPS).
// A C static survives a soft reset, so we remember and reuse our one channel; a hard reset frees it.
static int s_picogame_dma_chan = -1;

void common_hal_picogame_display_construct(picogame_display_obj_t *self,
    busdisplay_busdisplay_obj_t *display, bool rgb444) {
    self->display = display;
    #if CIRCUITPY_PICOGAME_RGB444
    self->rgb444 = rgb444;
    #else
    if (rgb444) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Operation or feature not supported"));
    }
    self->rgb444 = false;
    #endif

    // The fast path needs raw SPI access; only FourWire SPI buses are supported.
    if (!mp_obj_is_type(display->bus.bus, &fourwire_fourwire_type)) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q, not %q"),
            MP_QSTR_display, MP_QSTR_FourWire, mp_obj_get_type(display->bus.bus)->name);
    }
    fourwire_fourwire_obj_t *fw = MP_OBJ_TO_PTR(display->bus.bus);
    self->spi = fw->bus->peripheral;

    #if CIRCUITPY_PICOGAME_RGB444
    // Tell the panel which pixel format we'll send (COLMOD). Asserting it here also recovers from
    // a previous program that left the panel in the other format (the setting survives soft reset).
    picogame_set_pixel_format(display, rgb444);
    #endif

    if (s_picogame_dma_chan < 0) {
        s_picogame_dma_chan = dma_claim_unused_channel(true);
    }
    self->dma_chan = s_picogame_dma_chan;

    // Configure the channel ONCE (dreq, 8-bit, read-incr, write addr = SPI data reg). Per-strip we
    // then only set the read address + transfer count and trigger -- no per-strip reconfiguration.
    dma_channel_config c = dma_channel_get_default_config(self->dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq((spi_inst_t *)self->spi, true));
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    dma_channel_configure(self->dma_chan, &c,
        &spi_get_hw((spi_inst_t *)self->spi)->dr, NULL, 0, false);
}

// Kick off an asynchronous TX-only DMA of `nbytes` from `buf` to the SPI FIFO (channel already
// configured in construct; just point it at `buf`, set the count, and trigger).
static void dma_start(int chan, const uint16_t *buf, size_t nbytes) {
    dma_channel_set_read_addr(chan, buf, false);
    dma_channel_set_trans_count(chan, nbytes, true);   // true = trigger
}

void common_hal_picogame_display_render(picogame_display_obj_t *self,
    mp_obj_t *items, uint8_t *kinds, size_t n,
    uint16_t *buf_a, uint16_t *buf_b, size_t buf_pixels,
    int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t background,
    int ox, int oy) {

    busdisplay_busdisplay_obj_t *display = self->display;
    spi_inst_t *spi = (spi_inst_t *)self->spi;

    // RGB444 packs 2 px into 3 bytes, so each strip row must be an even number of pixels
    // (whole bytes). Widen the region to even bounds; the extra <=1 px per side just repaints.
    #if CIRCUITPY_PICOGAME_RGB444
    if (self->rgb444) {
        x0 &= ~1;
        x1 = (x1 + 1) & ~1;
        if (x1 > display->core.width) {
            x1 = display->core.width;
        }
    }
    #endif

    // Compute geometry + open the GRAM window (set region, begin transaction,
    // RAMWR). DC stays high for data after the first DATA send below, so the
    // raw-DMA strips that follow need no DC toggling.
    int region_w, strip_h;
    int cx0 = x0, cy0 = y0, cx1 = x1, cy1 = y1;   // strip_begin clamps these to the panel in place
    if (!picogame_strip_begin(display, &cx0, &cy0, &cx1, &cy1, buf_pixels, &region_w, &strip_h)) {
        return;
    }

    uint16_t *bufs[2] = { buf_a, buf_b };
    int cur = 0;
    bool first = true;
    bool dma_inflight = false;
    #if CIRCUITPY_PICOGAME_RGB444
    const bool rgb444 = self->rgb444;        // hoist: invariant across all strips
    #endif

    // A StripDraw callback may latch a BaseException (Ctrl-C / auto-reload). Like the portable
    // picogame_render_region, we must re-raise it -- but only AFTER the in-flight DMA finishes and
    // the bus transaction closes, so hold it here and propagate below (see the end of the function).
    mp_obj_t pending = MP_OBJ_NULL;

    for (int sy = cy0; sy < cy1; sy += strip_h) {
        int sh = picogame_imin(strip_h, cy1 - sy);
        uint16_t *buf = bufs[cur];

        // Blit this strip. When a DMA is in flight it transfers the *other*
        // buffer, so this CPU work overlaps the SPI transfer.
        pending = picogame_blit_strip_layers(buf, region_w, sy, sh, cx0, items, kinds, n, background, ox, oy);

        // RGB444: pack the just-blitted RGB565 strip in place (2 px -> 3 bytes) before sending.
        // The pack overlaps the previous strip's DMA (we're transfer-bound), so it's ~free.
        #if CIRCUITPY_PICOGAME_RGB444
        size_t nbytes = rgb444
            ? picogame_pack_rgb444(buf, (size_t)region_w * sh)
            : (size_t)region_w * sh * 2;
        #else
        size_t nbytes = (size_t)region_w * sh * 2;
        #endif

        if (first) {
            // First strip goes through busdisplay: it drives DC high for data,
            // which then stays high for the subsequent raw-DMA strips.
            display->bus.send(display->bus.bus, DISPLAY_DATA,
                CHIP_SELECT_UNTOUCHED, (uint8_t *)buf, nbytes);
            first = false;
        } else {
            if (dma_inflight) {
                dma_channel_wait_for_finish_blocking(self->dma_chan);
            }
            dma_start(self->dma_chan, buf, nbytes);
            dma_inflight = true;
        }
        cur ^= 1;
        if (pending != MP_OBJ_NULL) {   // callback interrupted: this strip is queued, now stop + flush
            break;
        }
    }

    if (dma_inflight) {
        dma_channel_wait_for_finish_blocking(self->dma_chan);
    }
    while (spi_is_busy(spi)) {
        // wait for the last bytes to leave the shift register before releasing CS
    }
    displayio_display_bus_end_transaction(&display->bus);

    if (pending != MP_OBJ_NULL) {        // bus now closed -> safe to re-raise (Ctrl-C / reload)
        nlr_raise(MP_OBJ_TO_PTR(pending));
    }
}
