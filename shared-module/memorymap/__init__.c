// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/runtime.h"

#include "shared-bindings/memorymap/__init__.h"
#include "shared-module/memorymap/__init__.h"
#include "common-hal/memorymap/ranges.h"

mp_obj_t common_hal_memorymap_addressrange_make_new(uint8_t *start_address, size_t length) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(memorymap_ranges); i++) {
        const memorymap_range_t *range = &memorymap_ranges[i];
        if (start_address <= range->start_address) {
            uint8_t *range_end_address = range->start_address + range->len - 1;
            uint8_t *end_address = start_address + length - 1;
            if (start_address > range_end_address || end_address > range_end_address) {
                break;
            }
            return mp_obj_new_memoryview(range->readonly ? 'B' : 0x80 | 'B', length, start_address);
        }
    }

    mp_raise_ValueError(MP_ERROR_TEXT("Address range not allowed"));
}
