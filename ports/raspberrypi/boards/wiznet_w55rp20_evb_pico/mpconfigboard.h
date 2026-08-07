// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "W55RP20-EVB-Pico"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_LED_STATUS (&pin_GPIO19)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO21)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO23)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO22)

#define DEFAULT_UART_BUS_RX (&pin_GPIO1)
#define DEFAULT_UART_BUS_TX (&pin_GPIO0)

// Wiznet HW config.
#define MICROPY_HW_WIZNET_PIO_SPI_ID        (0)
#define MICROPY_HW_WIZNET_PIO_SPI_BAUDRATE  (20 * 1000 * 1000)
#define MICROPY_HW_WIZNET_PIO_SPI_SCK       (21)
#define MICROPY_HW_WIZNET_PIO_SPI_MOSI      (23)
#define MICROPY_HW_WIZNET_PIO_SPI_MISO      (22)
#define MICROPY_HW_WIZNET_PIO_PIN_CS        (20)
#define MICROPY_HW_WIZNET_PIO_PIN_RST       (25)
// Connecting the INTN pin enables RECV interrupt handling of incoming data.
#define MICROPY_HW_WIZNET_PIN_INTN          (24)
