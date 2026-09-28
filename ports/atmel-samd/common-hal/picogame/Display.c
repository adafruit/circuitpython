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

#include "peripherals/samd/dma.h"

// shared_dma_transfer_start truncates the beat count to 16 bits, so one descriptor tops out here.
#define PICOGAME_DMA_MAX_BYTES 65535

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

    // The fast path needs the raw SERCOM; only FourWire SPI buses are supported.
    if (!mp_obj_is_type(display->bus.bus, &fourwire_fourwire_type)) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q, not %q"),
            MP_QSTR_display, MP_QSTR_FourWire, mp_obj_get_type(display->bus.bus)->name);
    }
    fourwire_fourwire_obj_t *fw = MP_OBJ_TO_PTR(display->bus.bus);
    self->sercom = (Sercom *)fw->bus->spi_desc.dev.prvt;

    #if CIRCUITPY_PICOGAME_RGB444
    // COLMOD; also resets a panel left in the other format by a previous program.
    picogame_set_pixel_format(display, rgb444);
    #endif

}

// Strips go through shared_dma_transfer_*, which handles the DMAC start errata and the RX
// overflow a TX-only transfer leaves on the SERCOM.
static void dma_finish(dma_transfer_t *xfer) {
    // Bounded wait: a strip takes well under 1 ms, so a timeout tears one frame instead of
    // hanging the board. close() releases the channel either way.
    for (uint32_t spins = 0; spins < 2000000; spins++) {
        if (shared_dma_transfer_finished(xfer)) {
            break;
        }
    }
    shared_dma_transfer_close(xfer);
}

void common_hal_picogame_display_render(picogame_display_obj_t *self,
    mp_obj_t *items, uint8_t *kinds, size_t n,
    uint16_t *buf_a, uint16_t *buf_b, size_t buf_pixels,
    int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t background,
    int ox, int oy) {

    busdisplay_busdisplay_obj_t *display = self->display;
    Sercom *sercom = self->sercom;

    // RGB444 packs 2 px into 3 bytes, so widen the region to even x bounds.
    #if CIRCUITPY_PICOGAME_RGB444
    if (self->rgb444) {
        x0 &= ~1;
        x1 = (x1 + 1) & ~1;
        if (x1 > display->core.width) {
            x1 = display->core.width;
        }
    }
    #endif

    // Sets the window and starts the transaction; the strips follow as data.
    int region_w, strip_h;
    int cx0 = x0, cy0 = y0, cx1 = x1, cy1 = y1;   // clamped to the panel by strip_begin
    if (!picogame_strip_begin(display, &cx0, &cy0, &cx1, &cy1, buf_pixels, &region_w, &strip_h)) {
        return;
    }

    uint16_t *bufs[2] = { buf_a, buf_b };
    int cur = 0;
    bool first = true;
    dma_transfer_t xfer;
    bool xfer_active = false;
    #if CIRCUITPY_PICOGAME_RGB444
    const bool rgb444 = self->rgb444;
    #endif

    // An exception from a StripDraw callback is re-raised after the DMA finishes and the
    // transaction ends.
    mp_obj_t pending = MP_OBJ_NULL;

    for (int sy = cy0; sy < cy1; sy += strip_h) {
        int sh = picogame_imin(strip_h, cy1 - sy);
        uint16_t *buf = bufs[cur];

        // Composes into the buffer the in-flight DMA is not reading.
        pending = picogame_blit_strip_layers(buf, region_w, sy, sh, cx0, items, kinds, n, background, ox, oy);

        // Packed in place, 3/4 the bytes.
        #if CIRCUITPY_PICOGAME_RGB444
        size_t nbytes = rgb444
            ? picogame_pack_rgb444(buf, (size_t)region_w * sh)
            : (size_t)region_w * sh * 2;
        #else
        size_t nbytes = (size_t)region_w * sh * 2;
        #endif

        if (xfer_active) {
            dma_finish(&xfer);
            xfer_active = false;
        }

        if (first || nbytes > PICOGAME_DMA_MAX_BYTES) {
            // The first strip sets DC for data through busdisplay; the DMA strips that follow
            // leave it. Strips over the descriptor limit also go this way (busio splits them).
            display->bus.send(display->bus.bus, DISPLAY_DATA,
                CHIP_SELECT_UNTOUCHED, (uint8_t *)buf, nbytes);
            first = false;
        } else {
            shared_dma_transfer_start(&xfer, sercom, (const uint8_t *)buf,
                &sercom->SPI.DATA.reg, NULL, NULL, nbytes, 0);
            if (xfer.failure != 0) {    // no channel free
                shared_dma_transfer_close(&xfer);
                display->bus.send(display->bus.bus, DISPLAY_DATA,
                    CHIP_SELECT_UNTOUCHED, (uint8_t *)buf, nbytes);
            } else {
                xfer_active = true;
            }
        }
        cur ^= 1;
        if (pending != MP_OBJ_NULL) {
            break;
        }
    }

    if (xfer_active) {
        dma_finish(&xfer);
    }

    displayio_display_bus_end_transaction(&display->bus);

    if (pending != MP_OBJ_NULL) {
        nlr_raise(MP_OBJ_TO_PTR(pending));
    }
}
