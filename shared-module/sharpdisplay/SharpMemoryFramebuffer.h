// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/lifetime.h"

#include "py/obj.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

typedef struct {
    mp_obj_base_t base;
    displayio_bus_lifetime_t bus_lifetime;
    busio_spi_obj_t *bus;
    busio_spi_obj_t inline_bus;
    digitalio_digitalinout_obj_t chip_select;
    mp_buffer_info_t bufinfo;

    uint16_t width, height;
    uint32_t baudrate;

    bool full_refresh;
    bool jdi_display;
} sharpdisplay_framebuffer_obj_t;

extern const framebuffer_p_t sharpdisplay_framebuffer_proto;
extern const displayio_protocol_t sharpdisplay_framebuffer_protocol;

mp_obj_t sharpdisplay_framebuffer_start_vm(sharpdisplay_framebuffer_obj_t *self);

void common_hal_sharpdisplay_framebuffer_collect_ptrs(sharpdisplay_framebuffer_obj_t *);
