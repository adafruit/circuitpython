// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// Fast display backend (atmel-samd): wraps an existing busdisplay and streams pixels with a DMAC
// channel of our own, double-buffered so the CPU blits the next strip while the current one is on
// the wire. Reuses the busdisplay's SERCOM, window opcodes and dimensions -- controller and
// resolution agnostic.
//
// Why this pays on a SAMD51: busio already sends through the DMAC, but it busy-waits for each
// transfer to land, so the compose time is added to the wire time instead of hidden under it.

#pragma once

#include "py/obj.h"

#include "include/sam.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-module/picogame/Sprite.h"

typedef struct {
    mp_obj_base_t base;
    busdisplay_busdisplay_obj_t *display;
    Sercom *sercom;            // the busdisplay's SERCOM; strips go out on the port's shared DMA
    bool rgb444;
} picogame_display_obj_t;

void common_hal_picogame_display_construct(picogame_display_obj_t *self,
    busdisplay_busdisplay_obj_t *display, bool rgb444);

void common_hal_picogame_display_render(picogame_display_obj_t *self,
    mp_obj_t *items, uint8_t *kinds, size_t n,
    uint16_t *buf_a, uint16_t *buf_b, size_t buf_pixels,
    int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t background,
    int ox, int oy);
