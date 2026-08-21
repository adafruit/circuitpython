// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// Fast display backend (espressif): wraps an existing busdisplay and streams
// pixels with the esp-idf SPI master's queued DMA, double-buffered so the CPU
// blits the next strip while the current one transfers. Reuses the busdisplay's
// SPI device, window opcodes and dimensions -- controller/resolution agnostic.

#pragma once

#include "py/obj.h"
#include "driver/spi_master.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-module/picogame/Sprite.h"

typedef struct {
    mp_obj_base_t base;
    busdisplay_busdisplay_obj_t *display;
    spi_device_handle_t spi;   // the busdisplay's SPI device (raw DMA queueing)
    bool rgb444;               // RGB444 strip packing (not yet implemented on this backend)
} picogame_display_obj_t;

void common_hal_picogame_display_construct(picogame_display_obj_t *self,
    busdisplay_busdisplay_obj_t *display, bool rgb444);

void common_hal_picogame_display_render(picogame_display_obj_t *self,
    mp_obj_t *items, uint8_t *kinds, size_t n,
    uint16_t *buf_a, uint16_t *buf_b, size_t buf_pixels,
    int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t background,
    int ox, int oy);
