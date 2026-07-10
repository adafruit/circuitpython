
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "shared-module/memorymap/__init__.h"
#include "soc/soc.h"

const static memorymap_range_t memorymap_ranges[] = {
    {(uint8_t *)SOC_RTC_DATA_LOW, SOC_RTC_DATA_HIGH - SOC_RTC_DATA_LOW},
    // CPU accessible RAM that is preserved during sleep if the RTC power domain is left on.
    {(uint8_t *)SOC_RTC_DRAM_LOW, SOC_RTC_DRAM_HIGH - SOC_RTC_DRAM_LOW},
    // RTC peripheral registers
    {(uint8_t *)0x60008000, 0x9000}
};
