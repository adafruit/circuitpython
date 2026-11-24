// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "pico/unique_id.h"

#define COMMON_HAL_MCU_PROCESSOR_UID_LENGTH PICO_UNIQUE_BOARD_ID_SIZE_BYTES

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // Stores no state currently.
} mcu_processor_obj_t;

#ifdef CIRCUITPY_PSRAM_CHIP_SELECT
void mcu_processor_update_psram_timing(uint32_t sys_clk_khz);
#endif

// Set the flash clock divider and sample delay for a system clock of
// sys_clk_khz, and save them for mcu_processor_restore_flash_timing(). Returns
// nothing.
//
// The flash clock is limited by the flash's datasheet timing (flash_info.h,
// generated from data/nvm.toml). A board can lower it further with
// CIRCUITPY_FLASH_MAX_SCK_HZ. On RP2350, the sample delay is also worked out
// from the datasheet timing. On RP2040, the flash clock is also limited to
// 62.5 MHz, and the sample delay is always 1.
//
// Call with interrupts disabled and no flash operation in progress.
void mcu_processor_update_flash_timing(uint32_t sys_clk_khz);

// Put back the flash timing last set by mcu_processor_update_flash_timing().
// Call after each flash erase, program or command (such as reading the flash
// ID). Those leave stage 2's slow timing in place. Returns nothing.
//
// Runs from RAM. Call with interrupts disabled.
void mcu_processor_restore_flash_timing(void);
