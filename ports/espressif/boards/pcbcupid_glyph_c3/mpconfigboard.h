// SPDX-FileCopyrightText: Copyright (c) 2026 PCBCupid
//
// SPDX-License-Identifier: MIT
#pragma once

#define MICROPY_HW_BOARD_NAME       "PCBCupid GLYPH C3"
#define MICROPY_HW_MCU_NAME         "ESP32-C3"

#define MICROPY_HW_LED_STATUS       (&pin_GPIO1)

#define DEFAULT_UART_BUS_TX         (&pin_GPIO21)
#define DEFAULT_UART_BUS_RX         (&pin_GPIO20)

#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO5)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO4)

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO10)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO6)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO7)
