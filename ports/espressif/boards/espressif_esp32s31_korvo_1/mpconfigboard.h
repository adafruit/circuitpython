// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ESP32-S31-Korvo-1"
#define MICROPY_HW_MCU_NAME         "ESP32S31"

// UART0 is wired to the onboard CP2102N USB-serial converter.
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO58)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO59)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO1)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO0)

#define CIRCUITPY_USB_HOST_INSTANCE 0
