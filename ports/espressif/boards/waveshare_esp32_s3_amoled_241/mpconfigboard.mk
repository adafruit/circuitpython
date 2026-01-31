# SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
#
# SPDX-License-Identifier: MIT

CIRCUITPY_CREATOR_ID  = 0x57415645  # 'WAVE' (Waveshare)
CIRCUITPY_CREATION_ID = 0x41323431  # 'A241' (AMOLED 2.41)

# USB identifiers - from Arduino pins_arduino.h
USB_VID = 0x303A
USB_PID = 0x82CE
USB_MANUFACTURER = "Waveshare"
USB_PRODUCT = "ESP32-S3-Touch-AMOLED-2.41"

# ESP32-S3 chip configuration
IDF_TARGET = esp32s3
CHIP_VARIANT = ESP32S3

# Flash configuration - 16MB QSPI Flash
CIRCUITPY_ESP_FLASH_SIZE = 16MB
CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m

# PSRAM configuration - 8MB Octal PSRAM
CIRCUITPY_ESP_PSRAM_SIZE = 8MB
CIRCUITPY_ESP_PSRAM_MODE = opi
CIRCUITPY_ESP_PSRAM_FREQ = 80m

# Add extra ESP-IDF components needed for board_init.c and rm690b0 module
ESP_IDF_EXTRA_COMPONENTS += driver freertos esp_lcd sdmmc esp_driver_sdmmc esp_driver_sdspi

# Build optimization
OPTIMIZATION_FLAGS = -Os

# Enable features for this board
CIRCUITPY_DISPLAYIO = 1
CIRCUITPY_PARALLELDISPLAY = 0
CIRCUITPY_RGBMATRIX = 0
CIRCUITPY_BITMAPTOOLS = 1
CIRCUITPY_JPEGIO = 1
CIRCUITPY_FRAMEBUFFERIO = 1
CIRCUITPY_RM690B0 = 1

# Enable ESP-NOW for peer-to-peer wireless communication
CIRCUITPY_ESPNOW = 1

# Disable camera support (not present on this board)
CIRCUITPY_ESPCAMERA = 0

# Enable I2C for touch, RTC, and IMU
CIRCUITPY_BITBANGIO = 1
CIRCUITPY_I2C = 1

# Enable SPI support
CIRCUITPY_SPI = 1

# Enable ADC for battery monitoring
CIRCUITPY_ANALOGIO = 1

# Enable WiFi and networking
CIRCUITPY_WIFI = 1
CIRCUITPY_SOCKETPOOL = 1
CIRCUITPY_SSL = 1
CIRCUITPY_HASHLIB = 1
CIRCUITPY_WEB_WORKFLOW = 1

# Enable USB features
CIRCUITPY_USB = 1
CIRCUITPY_USB_CDC = 1
CIRCUITPY_USB_HID = 1
CIRCUITPY_USB_MIDI = 1

# Storage configuration
CIRCUITPY_STORAGE = 1
CIRCUITPY_NVM = 1
CIRCUITPY_SDCARDIO = 1

# Enable touch support
CIRCUITPY_TOUCHIO = 0
CIRCUITPY_TOUCHSCREEN = 1

# Enable RTC support
CIRCUITPY_RTC = 1

# Enable JSON support for configuration
CIRCUITPY_JSON = 1

# Enable other useful modules
CIRCUITPY_RANDOM = 1
CIRCUITPY_STRUCT = 1
CIRCUITPY_ULAB = 1
CIRCUITPY_ZLIB = 1
