// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "W6300-EVB-Pico2"
#define MICROPY_HW_MCU_NAME "rp2350"

#define MICROPY_HW_LED_STATUS (&pin_GPIO25)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO17)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO18)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO19)

#define DEFAULT_UART_BUS_RX (&pin_GPIO1)
#define DEFAULT_UART_BUS_TX (&pin_GPIO0)

// Wiznet HW config.
#define MICROPY_HW_WIZNET_PIO_SPI_ID        (0)
#define MICROPY_HW_WIZNET_PIO_SPI_BAUDRATE  (20 * 1000 * 1000)
#define MICROPY_HW_WIZNET_PIO_SPI_SCK       (17)
#define MICROPY_HW_WIZNET_PIO_SPI_MOSI      (18)
#define MICROPY_HW_WIZNET_PIO_SPI_MISO      (19)
#define MICROPY_HW_WIZNET_PIO_PIN_CS        (16)
#define MICROPY_HW_WIZNET_PIO_PIN_RST       (22)
// Connecting the INTN pin enables RECV interrupt handling of incoming data.
#define MICROPY_HW_WIZNET_PIN_INTN          (15)
