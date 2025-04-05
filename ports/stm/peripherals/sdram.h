// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 snkYmkrct
//
// SPDX-License-Identifier: MIT

#pragma once

#include "stm32h7xx_ll_fmc.h"
#include <stdbool.h>
#include <stdint.h>

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/** FMC SDRAM controller bank configuration fields. */
struct stm32_sdram_bank_config {
    FMC_SDRAM_InitTypeDef init;
    FMC_SDRAM_TimingTypeDef timing;
};

/** FMC SDRAM controller configuration fields. */
struct stm32_sdram_config {
    FMC_SDRAM_TypeDef *sdram;
    uint32_t power_up_delay;
    uint8_t num_auto_refresh;
    uint16_t mode_register;
    uint16_t refresh_rate;
    const struct stm32_sdram_bank_config *banks;
    size_t banks_len;
};

void sdram_init(const struct stm32_sdram_config *config);
void sdram_deinit(void);
void *sdram_start(void);
void *sdram_end(void);
uint32_t sdram_size(void);
bool sdram_test(bool exhaustive);
