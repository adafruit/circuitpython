// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 snkYmkrct
//
// SPDX-License-Identifier: MIT

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "DAISY_SEED"
#define MICROPY_HW_MCU_NAME         "STM32H750xx"

#define MICROPY_HW_LED_STATUS (&pin_PC07)

// H7 and F7 MPU definitions
#define CPY_FLASH_REGION_SIZE   ARM_MPU_REGION_SIZE_8MB
#define CPY_ITCM_REGION_SIZE    ARM_MPU_REGION_SIZE_64KB
#define CPY_DTCM_REGION_SIZE    ARM_MPU_REGION_SIZE_128KB
#define CPY_SRAM_REGION_SIZE    ARM_MPU_REGION_SIZE_512KB
#define CPY_SRAM_SUBMASK        0x00
#define CPY_SRAM_START_ADDR     0x24000000

#define HSE_VALUE ((uint32_t)16000000)
#define BOARD_HSE_SOURCE (RCC_HSE_ON) // use external oscillator
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

#define CIRCUITPY_CONSOLE_UART_TX (&pin_PB09)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_PB08)

// USB
#define BOARD_NO_USB_OTG_ID_SENSE (1)

// for RNG not audio
#define CPY_CLK_USB_USES_AUDIOPLL (1)

// SDRAM and MPU region

#define CIRCUITPY_HW_SDRAM_SIZE               (64 * 1024 * 1024)  // 64 MByte

#define CPY_SDRAM_REGION        MPU_REGION_NUMBER10
#define CPY_SDRAM_REGION_SIZE   MPU_REGION_SIZE_64MB
