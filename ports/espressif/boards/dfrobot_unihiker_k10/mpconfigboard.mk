# This file is part of the CircuitPython project: https://circuitpython.org
# SPDX-FileCopyrightText: Copyright (c) 2026 Djair Guilherme
# SPDX-License-Identifier: MIT

USB_VID = 0x303A
USB_PID = 0x1001
USB_MANUFACTURER = "DFRobot"
USB_PRODUCT = "UNIHIKER K10"

IDF_TARGET = esp32s3

# Flash configuration - 16MB QSPI Flash
CIRCUITPY_ESP_FLASH_SIZE = 16MB
CIRCUITPY_ESP_FLASH_MODE = dio
CIRCUITPY_ESP_FLASH_FREQ = 80m

# PSRAM configuration - 8MB QSPI PSRAM
CIRCUITPY_ESP_PSRAM_SIZE = 8MB
CIRCUITPY_ESP_PSRAM_MODE = opi
CIRCUITPY_ESP_PSRAM_FREQ = 80m

OPTIMIZATION_FLAGS = -Os

# Display via SPI (ILI9341)
CIRCUITPY_PARALLELDISPLAYBUS = 0

# Camera (GC2145)
CIRCUITPY_ESPCAMERA = 1

# I2S for audio (microphone + speaker)
CIRCUITPY_AUDIOBUSIO = 1

# SD card via SDMMC - disabled until pin mapping verified
CIRCUITPY_SDIOIO = 0

# Frozen Python modules (built-in libraries)
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ILI9341
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_AHTx0
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SD
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Wave
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Shapes
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Text
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SimpleIO
FROZEN_MPY_DIRS += $(TOP)/frozen/adafruit_tca9555


