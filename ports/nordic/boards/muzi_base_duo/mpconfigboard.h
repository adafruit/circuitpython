// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Álvaro Figueroa
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "muzi works Base Duo"
#define MICROPY_HW_MCU_NAME         "nRF52840"

// Blue LED. Cathode is driven by the MCU, anode sits on 3V3, so this is active low.
#define MICROPY_HW_LED_STATUS       (&pin_P1_04)
#define MICROPY_HW_LED_STATUS_INVERTED (1)

// Y1 32.768 kHz crystal on P0.00 / P0.01.
#define BOARD_HAS_32KHZ_XTAL (1)

// User button, active low with a 100k pull-up.
#define CIRCUITPY_BOOT_BUTTON       (&pin_P0_10)

#if QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0          NRF_GPIO_PIN_MAP(0, 30)
#define MICROPY_QSPI_DATA1          NRF_GPIO_PIN_MAP(0, 29)
#define MICROPY_QSPI_DATA2          NRF_GPIO_PIN_MAP(0, 28)
#define MICROPY_QSPI_DATA3          NRF_GPIO_PIN_MAP(0, 2)
#define MICROPY_QSPI_SCK            NRF_GPIO_PIN_MAP(0, 3)
#define MICROPY_QSPI_CS             NRF_GPIO_PIN_MAP(0, 26)
#endif

#define CIRCUITPY_INTERNAL_NVM_SIZE (4096)

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

// 0: Qwiic / STEMMA QT and the castellated I2C pads.
// 1: the OLED / display header pads.
#define CIRCUITPY_BOARD_I2C         (2)
#define CIRCUITPY_BOARD_I2C_PIN     { \
        {.scl = &pin_P0_06, .sda = &pin_P0_04}, \
        {.scl = &pin_P0_25, .sda = &pin_P0_24}, \
}

// The only SPI bus is the module-internal one going to the LR1121.
#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     { \
        {.clock = &pin_P1_13, .mosi = &pin_P1_14, .miso = &pin_P1_15}, \
}

// Castellated UART. Net names on the silkscreen are from the peripheral's point of view.
#define DEFAULT_UART_BUS_TX         (&pin_P0_19)
#define DEFAULT_UART_BUS_RX         (&pin_P0_20)
