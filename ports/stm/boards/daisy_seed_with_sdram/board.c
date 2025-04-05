// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 snkYmkrct
//
// SPDX-License-Identifier: MIT
#include STM32_HAL_H

#include "supervisor/board.h"
#include "sdram.h"

extern void port_add_sdram_to_heap(void);

/** SDRAM banks configuration. */
static const struct stm32_sdram_bank_config bank_config[] = {
    { .init = {
          .SDBank = FMC_SDRAM_BANK1,
          .ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9,
          .RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13,
          .MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32,
          .InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4,
          .CASLatency = FMC_SDRAM_CAS_LATENCY_3,
          .WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE,
          .SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2,
          .ReadBurst = FMC_SDRAM_RBURST_ENABLE,
          .ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0
      },
      .timing = {
          .LoadToActiveDelay = 2,
          .ExitSelfRefreshDelay = 8,
          .SelfRefreshTime = 5,
          .RowCycleDelay = 6,
          .WriteRecoveryTime = 3,
          .RPDelay = 2,
          .RCDDelay = 2
      }}
};

/* SDRAM configuration. */
static const struct stm32_sdram_config config = {
    .sdram = FMC_SDRAM_DEVICE,
    .power_up_delay = 0,
    .num_auto_refresh = 8,
    .mode_register = SDRAM_MODEREG_BURST_LENGTH_4 |
        SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
        SDRAM_MODEREG_CAS_LATENCY_3 |
        SDRAM_MODEREG_WRITEBURST_MODE_SINGLE,
    /* Set the device refresh rate based on the RM0433 STM reference manual
        refresh_rate = [(SDRAM self refresh time / number of rows) x  SDRAM CLK] – 20
                     = [(64ms/8192) * 100MHz] - 20 = 781.25 - 20
    */
    .refresh_rate = (64 * 100000 / 8192 - 20),
    .banks = bank_config,
    .banks_len = 1,
};

void board_init(void) {
    sdram_init(&config);
//    sdram_test(true);
    port_add_sdram_to_heap();
}
