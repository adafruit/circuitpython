// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/sdioio/sdfat_pio/shim.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // In-place storage for the vendored C++ PioSdioCard instance, constructed
    // and torn down through the extern "C" shim in sdfat_pio/.
    sdioio_pio_card_storage_t card;
    uint32_t frequency;
    uint32_t capacity;  // Number of 512-byte blocks.
    uint8_t num_data;
    uint8_t command;
    uint8_t clock;
    uint8_t data[4];
    bool never_reset;
} sdioio_sdcard_obj_t;

// Called by the supervisor on soft reset to release any card that is not
// protected with never_reset.
void sdioio_reset(void);
