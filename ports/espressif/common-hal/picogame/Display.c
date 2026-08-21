// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "common-hal/picogame/Display.h"

#include <string.h>

#include "py/runtime.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/displayio/display_core.h"
#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "common-hal/busio/SPI.h"

#include "driver/spi_master.h"

// The esp-idf SPI device queue holds MAX_SPI_TRANSACTIONS (10) outstanding
// transactions. We keep up to two strips in flight (current transferring while
// the next is blitted), so each strip may use at most this many DMA chunks and
// still leave room: 2 * 5 <= 10. Strips needing more chunks fall back to a
// blocking send (correct, just no overlap for that strip).
#define PICOGAME_MAX_STRIP_CHUNKS 5

void common_hal_picogame_display_construct(picogame_display_obj_t *self,
    busdisplay_busdisplay_obj_t *display, bool rgb444) {
    self->display = display;
    // RGB444 strip packing isn't implemented on this backend yet. Raise rather than silently
    // ignore it: a no-op would leave the panel in RGB565 while the caller expects 444 (garbled
    // output / wrong byte count). The rpi backend implements it; until this one does, fail loud.
    if (rgb444) {
        mp_arg_error_invalid(MP_QSTR_rgb444);    // not implemented on this port yet
    }
    self->rgb444 = false;

    // The fast path queues raw DMA on the display's SPI device; only FourWire
    // SPI buses expose one.
    if (!mp_obj_is_type(display->bus.bus, &fourwire_fourwire_type)) {
        mp_arg_error_invalid(MP_QSTR_display);   // the fast backend drives a FourWire SPI panel only
    }
    fourwire_fourwire_obj_t *fw = MP_OBJ_TO_PTR(display->bus.bus);
    self->spi = common_hal_busio_spi_get_device_handle(fw->bus);
}

// Retrieve all outstanding results for *count queued chunks, then zero the count.
static void drain(spi_device_handle_t spi, int *count) {
    spi_transaction_t *rtrans;
    while (*count > 0) {
        spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
        (*count)--;
    }
}

// Queue one strip (nbytes from buf) as up to PICOGAME_MAX_STRIP_CHUNKS DMA
// transactions. Returns the chunk count, or -1 if it would need more chunks.
static int queue_strip(spi_device_handle_t spi, spi_transaction_t *trans,
    const uint16_t *buf, size_t nbytes) {
    int needed = (int)((nbytes + SPI_MAX_DMA_LEN - 1) / SPI_MAX_DMA_LEN);
    if (needed > PICOGAME_MAX_STRIP_CHUNKS) {
        return -1;
    }
    const uint8_t *p = (const uint8_t *)buf;
    size_t off = 0;
    int n = 0;
    while (off < nbytes) {
        size_t chunk = nbytes - off;
        if (chunk > SPI_MAX_DMA_LEN) {
            chunk = SPI_MAX_DMA_LEN;
        }
        memset(&trans[n], 0, sizeof(spi_transaction_t));
        trans[n].length = chunk * 8;            // in bits
        trans[n].tx_buffer = p + off;
        spi_device_queue_trans(spi, &trans[n], portMAX_DELAY);
        off += chunk;
        n++;
    }
    return n;
}

void common_hal_picogame_display_render(picogame_display_obj_t *self,
    mp_obj_t *items, uint8_t *kinds, size_t n,
    uint16_t *buf_a, uint16_t *buf_b, size_t buf_pixels,
    int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t background,
    int ox, int oy) {

    busdisplay_busdisplay_obj_t *display = self->display;
    spi_device_handle_t spi = self->spi;

    // Open the GRAM window (set region, begin transaction, RAMWR). After the
    // first DATA send raises DC, DC stays high for the raw queued strips that
    // follow, and CS stays low until end_transaction.
    int region_w, strip_h;
    int cx0 = x0, cy0 = y0, cx1 = x1, cy1 = y1;   // strip_begin clamps these to the panel in place
    if (!picogame_strip_begin(display, &cx0, &cy0, &cx1, &cy1, buf_pixels, &region_w, &strip_h)) {
        return;
    }

    uint16_t *bufs[2] = { buf_a, buf_b };
    spi_transaction_t trans[2][PICOGAME_MAX_STRIP_CHUNKS];
    int inflight[2] = { 0, 0 };   // chunks queued from bufs[i], awaiting result

    // A StripDraw callback may latch a BaseException (Ctrl-C / auto-reload). Like the portable
    // renderer and the RP backend, re-raise it -- but only AFTER the queued transfers drain and
    // the bus transaction closes, so hold it here and propagate below.
    mp_obj_t pending = MP_OBJ_NULL;

    int cur = 0;
    bool first = true;
    for (int sy = cy0; sy < cy1; sy += strip_h) {
        int sh = picogame_imin(strip_h, cy1 - sy);
        size_t nbytes = (size_t)region_w * sh * 2;
        uint16_t *buf = bufs[cur];

        // This buffer must be free before we overwrite it.
        drain(spi, &inflight[cur]);

        // Blit this strip. If the OTHER buffer has a strip in flight, its DMA
        // transfer overlaps this CPU work -- the whole point of the fast path.
        pending = picogame_blit_strip_layers(buf, region_w, sy, sh, cx0, items, kinds, n,
            background, ox, oy);

        int nch;
        if (first) {
            // First DATA send goes through the busdisplay so it raises DC.
            display->bus.send(display->bus.bus, DISPLAY_DATA,
                CHIP_SELECT_UNTOUCHED, (uint8_t *)buf, nbytes);
            first = false;
        } else if ((nch = queue_strip(spi, trans[cur], buf, nbytes)) >= 0) {
            inflight[cur] = nch;
        } else {
            // Strip too large to keep two in flight: drain everything and send
            // it blocking (DC already high, CS untouched).
            drain(spi, &inflight[cur ^ 1]);
            display->bus.send(display->bus.bus, DISPLAY_DATA,
                CHIP_SELECT_UNTOUCHED, (uint8_t *)buf, nbytes);
        }
        cur ^= 1;
        if (pending != MP_OBJ_NULL) {   // callback interrupted: this strip is queued, now stop + flush
            break;
        }
    }

    drain(spi, &inflight[0]);
    drain(spi, &inflight[1]);

    displayio_display_bus_end_transaction(&display->bus);

    if (pending != MP_OBJ_NULL) {        // bus now closed -> safe to re-raise (Ctrl-C / reload)
        nlr_raise(MP_OBJ_TO_PTR(pending));
    }
}
