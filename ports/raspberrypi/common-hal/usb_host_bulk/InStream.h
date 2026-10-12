// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"

#include "pio_usb_bulk_stream.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t device;                 // keeps the usb.core.Device alive; NULL once deinited
    pio_usb_bulk_ring_t *ring;       // port heap; NULL once freed
    uint8_t *storage;
    uint8_t *leftover;               // VM heap: bytes unread when the host ended the stream
    uint32_t leftover_len;
    uint32_t leftover_pos;
    uint32_t lost_packets;           // latched when the ring is freed
    uint8_t device_address;          // copies, so deinit never touches `device`
    uint8_t endpoint;
} usb_host_bulk_instream_obj_t;
