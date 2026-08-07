// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#define NUM_BLEIO_ADDRESS_BYTES 6

typedef struct {
    mp_obj_base_t base;
    uint8_t type;
    mp_obj_t bytes;    // a bytes() object
} bleio_address_obj_t;

// A BLE address without the object wrapper, for code that must not allocate or raise --
// notably supervisor/shared, which runs outside the VM. A NULL pointer to one of these
// means "no address", just as a NULL bleio_address_obj_t * does.
typedef struct {
    uint8_t bytes[NUM_BLEIO_ADDRESS_BYTES];
    uint8_t type;                            // one of BLEIO_ADDRESS_TYPE_*
} bleio_raw_address_t;

// Copies `address` into `*raw`. Raises if `address->bytes` is not a readable buffer, in
// which case `*raw` is left partly or wholly untouched, so only call this where raising
// is acceptable: not from supervisor/shared.
void bleio_address_to_raw(const bleio_address_obj_t *address, bleio_raw_address_t *raw);
