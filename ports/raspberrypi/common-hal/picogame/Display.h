// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// Fast display backend (RP2040): wraps an existing busdisplay and pushes pixels
// with asynchronous, double-buffered DMA so the CPU can blit the next strip
// while the current strip transfers over SPI. Reuses the busdisplay's SPI
// peripheral, window opcodes and dimensions - controller/resolution agnostic.

#pragma once

#include "py/obj.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-module/picogame/Sprite.h"

typedef struct {
    mp_obj_base_t base;
    busdisplay_busdisplay_obj_t *display;
    void *spi;        // spi_inst_t* (kept opaque to avoid pico-sdk in this header)
    int dma_chan;
    bool rgb444;      // pack strips to 12-bit RGB444 before sending (~25% less SPI traffic)
} picogame_display_obj_t;

void common_hal_picogame_display_construct(picogame_display_obj_t *self,
    busdisplay_busdisplay_obj_t *display, bool rgb444);

void common_hal_picogame_display_render(picogame_display_obj_t *self,
    mp_obj_t *items, uint8_t *kinds, size_t n,
    uint16_t *buf_a, uint16_t *buf_b, size_t buf_pixels,
    int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t background,
    int ox, int oy);
