// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include <string.h>
#include "nrf.h"

#include "shared-bindings/memorymap/__init__.h"

#include "py/runtime.h"


#ifdef NRF51_SERIES
const static memorymap_range_t memorymap_ranges[] = {
    // FLASH
    {(uint8_t *)0x00000000, 0x00040000},
    // FICR & UICR ranges
    {(uint8_t *)0x10000000, 0x00002000},
    // RAM
    {(uint8_t *)0x20000000, 0x00010000},
    // PERIPHERALS
    {(uint8_t *)0x40000000, 0x20000000}
};
#elif defined NRF52_SERIES
const static memorymap_range_t memorymap_ranges[] = {
    // FLASH
    {(uint8_t *)0x00000000, 0x00100000},
    // FICR & UICR ranges
    {(uint8_t *)0x10000000, 0x00002000},
    // RAM
    {(uint8_t *)0x20000000, 0x00040000},
    // PERIPHERALS
    {(uint8_t *)0x40000000, 0x20000000}
};
#elif defined NRF53_SERIES
const static memorymap_range_t memorymap_ranges[] = {
    // FLASH
    {(uint8_t *)0x00000000, 0x00100000},
    // FICR & UICR ranges
    {(uint8_t *)0x00FF0000, 0x00010000},
    // RAM
    {(uint8_t *)0x20000000, 0x00080000},
    // PERIPHERALS
    {(uint8_t *)0x40000000, 0x20000000},
    {(uint8_t *)0xE0000000, 0x00100000}
};
#else
#error "Unsupported nRF variant"
#endif
