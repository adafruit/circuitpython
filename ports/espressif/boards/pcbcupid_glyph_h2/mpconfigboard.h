// SPDX-FileCopyrightText: Copyright (c) 2024 PCBCupid
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "Pcbcupid GLYPH H2"
#define MICROPY_HW_MCU_NAME         "ESP32H2"

#define MICROPY_HW_LED_STATUS (&pin_GPIO0)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO5)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO4)

#define DEFAULT_SPI_BUS_SCK  (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO22)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO25)

#define DEFAULT_UART_BUS_RX (&pin_GPIO23)
#define DEFAULT_UART_BUS_TX (&pin_GPIO24)
