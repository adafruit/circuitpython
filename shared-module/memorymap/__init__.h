// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

typedef struct {
    uint8_t *start_address;
    uint32_t len : 31;
    uint32_t readonly : 1;
} memorymap_range_t;
