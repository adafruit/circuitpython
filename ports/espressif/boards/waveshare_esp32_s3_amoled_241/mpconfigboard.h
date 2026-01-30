// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Waveshare ESP32-S3-Touch-AMOLED-2.41"
#define MICROPY_HW_MCU_NAME   "ESP32S3"

// USB identifiers
#define USB_VID 0x303A
#define USB_PID 0x82CE
#define USB_MANUFACTURER "Waveshare"
#define USB_PRODUCT "ESP32-S3-Touch-AMOLED-2.41"

// I2C bus - Disabled on boot to avoid conflicts. User must manually initialize I2C.
#define CIRCUITPY_BOARD_I2C         (0)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO48, .sda = &pin_GPIO47}}

// AMOLED Display
#define CIRCUITPY_BOARD_DISPLAY 1
#define CIRCUITPY_DISPLAY_INIT_SEQUENCE  rm690b0_rm690b0_init_sequence
#define CIRCUITPY_RM690B0_QSPI_CS        (&pin_GPIO9)
#define CIRCUITPY_RM690B0_QSPI_CLK       (&pin_GPIO10)
#define CIRCUITPY_RM690B0_QSPI_D0        (&pin_GPIO11)
#define CIRCUITPY_RM690B0_QSPI_D1        (&pin_GPIO12)
#define CIRCUITPY_RM690B0_QSPI_D2        (&pin_GPIO13)
#define CIRCUITPY_RM690B0_QSPI_D3        (&pin_GPIO14)
#define CIRCUITPY_RM690B0_RESET          (&pin_GPIO21)
#define CIRCUITPY_RM690B0_POWER          (&pin_GPIO16)
#define CIRCUITPY_RM690B0_POWER_ON_LEVEL (true)
#define CIRCUITPY_RM690B0_WIDTH          (600)
#define CIRCUITPY_RM690B0_HEIGHT         (450)
#define CIRCUITPY_RM690B0_BITS_PER_PIXEL (16)
#define CIRCUITPY_RM690B0_USE_QSPI       (1)
#define CIRCUITPY_RM690B0_PIXEL_CLOCK_HZ (80 * 1000 * 1000)

// SPI bus for SD Card
#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO4, .mosi = &pin_GPIO5, .miso = &pin_GPIO6}}

// Default UART bus
#define DEFAULT_UART_BUS_RX (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX (&pin_GPIO43)

// Disable unnecessary modules to save space
#define CIRCUITPY_ESP32_CAMERA (0)
