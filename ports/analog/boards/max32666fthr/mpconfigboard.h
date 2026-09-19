// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices Inc.
//
// SPDX-License-Identifier: MIT

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "MAX32666FTHR"
#define MICROPY_HW_MCU_NAME         "max32665"

#define FLASH_SIZE                  (0x100000) // 1MiB
#define FLASH_PAGE_SIZE             (0x4000)   // 16384 byte pages (16 KiB)

#define BOARD_HAS_CRYSTAL           1
#define NUM_GPIO_PORTS              2
#define CONSOLE_UART                MXC_UART1

// #if INTERNAL_FLASH_FILESYSTEM
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR (0x100E0000) // for MAX32666
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (128 * 1024)       // 128K

#define MAX32_FLASH_SIZE  0x100000 // 1 MiB
#define INTERNAL_FLASH_FILESYSTEM_SIZE CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE
#define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x100E0000 // Load into the last MiB of code/data storage

// #else
// #define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (0)
// #endif

#define MICROPY_HW_LED_STATUS   (&pin_P0_31)
