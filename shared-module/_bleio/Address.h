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
    // Little-endian BD_ADDR order: bytes[0] is the least-significant byte.
    // Stored inline so an Address can live in static storage (e.g. embedded in
    // an adapter) without allocating a separate bytes object.
    uint8_t bytes[NUM_BLEIO_ADDRESS_BYTES];
} bleio_address_obj_t;
