// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vegard Wærp
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ELECFREAKS Retro Arcade"
#define MICROPY_HW_MCU_NAME         "STM32F412RE"

#define FLASH_SIZE                  (0x80000)
#define FLASH_PAGE_SIZE             (0x4000)

// The uf2-stm32f bootloader takes sectors 0-1 and MakeCode keeps its saves in
// sectors 2-3, so CIRCUITPY goes there and the firmware gets sectors 4-7.
#define INTERNAL_FLASH_FILESYSTEM_SIZE 0x8000 // 32KiB
#define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x08008000

#define HSE_VALUE ((uint32_t)8000000U)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

#define BOARD_NO_VBUS_SENSE (1)
#define BOARD_VTOR_DEFER (1) // Leave VTOR relocation to bootloader
#define BOARD_UF2_STM32F_BOOTLOADER (1)

#define DEFAULT_I2C_BUS_SCL (&pin_PB10)
#define DEFAULT_I2C_BUS_SDA (&pin_PB03)
