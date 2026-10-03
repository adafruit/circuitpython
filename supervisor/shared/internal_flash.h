// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft, for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT
#pragma once

#include "supervisor/internal_flash.h" // This is per-port.

void port_internal_flash_flush(void);

#if CIRCUITPY_STORAGE_MAP_FILE
// The memory-mapped address of a drive block, NULL if the port cannot map it; *contiguous
// receives how many blocks from it are contiguous in the mapping. A port that sets
// CIRCUITPY_STORAGE_MAP_FILE implements this.
const uint8_t *port_internal_flash_xip_address(uint32_t block, uint32_t *contiguous);
#endif
