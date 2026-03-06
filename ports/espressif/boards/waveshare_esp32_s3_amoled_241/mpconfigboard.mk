# This file is part of the CircuitPython project: https://circuitpython.org
# SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
# SPDX-License-Identifier: MIT

USB_VID = 0x303A
USB_PID = 0x8278
USB_MANUFACTURER = "Waveshare"
USB_PRODUCT = "ESP32-S3-Touch-AMOLED-2.41"

IDF_TARGET = esp32s3

# Flash configuration - 16MB QSPI Flash
CIRCUITPY_ESP_FLASH_SIZE = 16MB
CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m

# PSRAM configuration - 8MB Octal PSRAM
CIRCUITPY_ESP_PSRAM_SIZE = 8MB
CIRCUITPY_ESP_PSRAM_MODE = opi
CIRCUITPY_ESP_PSRAM_FREQ = 80m

OPTIMIZATION_FLAGS = -Os

# QSPI bus for RM690B0 AMOLED display
CIRCUITPY_QSPIBUS = 1
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No camera on this board
CIRCUITPY_ESPCAMERA = 0

# Capacitive touch not available; board uses I2C touch controller
CIRCUITPY_TOUCHIO = 0

# SD card via SDMMC interface
CIRCUITPY_SDIOIO = 1
