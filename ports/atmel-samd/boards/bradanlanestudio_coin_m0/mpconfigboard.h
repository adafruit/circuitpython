// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Bradán Lane STUDIO
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Bradán Lane STUDIO Coin M0"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_HW_LED_STATUS   (&pin_PA17)

#define SPI_FLASH_MOSI_PIN          &pin_PB22
#define SPI_FLASH_MISO_PIN          &pin_PB03
#define SPI_FLASH_SCK_PIN           &pin_PB23
#define SPI_FLASH_CS_PIN            &pin_PA27

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1   // USB_D+
#define IGNORE_PIN_PA25     1   // USB_D-

#define IGNORE_PIN_PA30     1   // SWCLK
#define IGNORE_PIN_PA31     1   // SWDIO
