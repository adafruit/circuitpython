# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2024 Bradán Lane STUDIO
#
# SPDX-License-Identifier: MIT

# TODO new VID:PID not yet approved via pidcodes.github.com
USB_VID = 0x1209
USB_PID = 0x5687

USB_PRODUCT = "Coin M0"
USB_MANUFACTURER = "Bradán Lane STUDIO"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

#CIRCUITPY_BUILD_EXTENSIONS = bin,uf2

SPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "W25Q32JVxQ"
LONGINT_IMPL = NONE

# the M0 Coin has limited functionality and many modules can be eliminated

# there may be more modules which are of no used but will require further digging

# Disable modules that are unusable on this special-purpose board.

CIRCUITPY_FULL_BUILD = 0

CIRCUITPY_AUDIOIO = 1
CIRCUITPY_DISPLAYIO = 0
CIRCUITPY_FRAMEBUFFERIO = 0
CIRCUITPY_ONEWIREIO = 0
CIRCUITPY_PULSEIO = 0
CIRCUITPY_RGBMATRIX = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_RTC = 0
CIRCUITPY_USB_HID = 1
CIRCUITPY_USB_MIDI = 0


# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HID
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
