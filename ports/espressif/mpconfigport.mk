# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2025 Adafruit Industries
#
# SPDX-License-Identifier: MIT


ifeq ($(IDF_TARGET),esp32c2)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else ifeq ($(IDF_TARGET),esp32c3)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else ifeq ($(IDF_TARGET),esp32p4)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else ifeq ($(IDF_TARGET),esp32c6)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else ifeq ($(IDF_TARGET),esp32c61)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else ifeq ($(IDF_TARGET),esp32h2)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else
IDF_TARGET_ARCH = xtensa
CROSS_COMPILE = xtensa-$(IDF_TARGET)-elf-
endif

# Use "internal flash" for CIRCUITPY drive.
# Even thought it's an external flash chip, it appears as internal.
INTERNAL_FLASH_FILESYSTEM = 1

# Internal math library is substantially smaller than toolchain one
INTERNAL_LIBM = 0

# Longints can be implemented as MPZ, as LONGLONG, or NONE
LONGINT_IMPL = MPZ

# Default to no-psram
CIRCUITPY_ESP_PSRAM_SIZE ?= 0

# Some 4MB non-USB boards were initially defined with 2MB firmware, almost 2MB user_fs partitions.
# Others were defined with 1.4M+1.4M (now a single 2.8MB) firmware partitions / 1.2MB user_fs.
# Keep the former as is, so that the user filesystem will be unchanged.
CIRCUITPY_4MB_FLASH_LARGE_USER_FS_LAYOUT ?= 0

# Enable more features
CIRCUITPY_FULL_BUILD ?= 1

# If SSL is enabled, it's mbedtls
CIRCUITPY_SSL_MBEDTLS = 1

# Never use our copy of MBEDTLS
CIRCUITPY_HASHLIB_MBEDTLS_ONLY = 0

CIRCUITPY_PORT_SERIAL = 1

CIRCUITPY_LIB_TLSF = 0

CIRCUITPY_LIBC_STRING0 = 0

# These modules are implemented in ports/<port>/common-hal:
CIRCUITPY__EVE ?= 1
CIRCUITPY_ALARM ?= 1
CIRCUITPY_ALARM_TOUCH ?= 0
CIRCUITPY_ANALOGBUFIO ?= 1
CIRCUITPY_AUDIOBUSIO ?= 1
CIRCUITPY_AUDIOBUSIO_PDMIN ?= 0
CIRCUITPY_AUDIOIO ?= 1
CIRCUITPY_BLEIO_HCI = 0
CIRCUITPY_BLEIO_NATIVE ?= 1
CIRCUITPY_CANIO ?= 1
CIRCUITPY_COUNTIO ?= 1
CIRCUITPY_ESPCAMERA ?= 1
CIRCUITPY_ESPIDF ?= 1
CIRCUITPY_ESPULP ?= 1
CIRCUITPY_FRAMEBUFFERIO ?= 1
CIRCUITPY_FREQUENCYIO ?= 1
CIRCUITPY_HASHLIB ?= 1
CIRCUITPY_I2CTARGET ?= 0
CIRCUITPY_MAX3421E ?= 1
CIRCUITPY_MEMORYMAP ?= 1
CIRCUITPY_RCLCPY ?= 0
CIRCUITPY_NVM ?= 1
CIRCUITPY_PARALLELDISPLAYBUS ?= 1
CIRCUITPY_PS2IO ?= 1
CIRCUITPY_RGBMATRIX ?= 1
CIRCUITPY_ROTARYIO ?= 1
CIRCUITPY_SDIOIO ?= 1
CIRCUITPY_SETTABLE_PROCESSOR_FREQUENCY ?= 1
CIRCUITPY_SYNTHIO_MAX_CHANNELS ?= 12
CIRCUITPY_TOUCHIO_USE_NATIVE ?= 1
CIRCUITPY_WATCHDOG ?= 1
CIRCUITPY_WIFI ?= 1
CIRCUITPY_SOCKETPOOL_IPV6 ?= 1

# Conditionally turn off modules/features per chip type
#### esp32 ############################################################
ifeq ($(IDF_TARGET),esp32)
# Modules
CIRCUITPY_ALARM_TOUCH = 1
CIRCUITPY_RGBMATRIX = 0

# SDMMC not supported yet
CIRCUITPY_SDIOIO = 0

# Has no USB
CIRCUITPY_USB_DEVICE = 0

#### esp32c2 ##########################################################
else ifeq ($(IDF_TARGET),esp32c2)

# C2 ROM spits out the UART at 74880 when connected to a 26mhz crystal!
# Debug prints will default to that too.
# Modules
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_MEMORYMAP = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No PCNT peripheral
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_COUNTIO = 0
CIRCUITPY_ROTARYIO = 0

# No two wire automotive
CIRCUITPY_CANIO = 0

# No DMA from ADC
CIRCUITPY_ANALOGBUFIO = 0

# No I2S
CIRCUITPY_AUDIOBUSIO = 0

# No DAC
CIRCUITPY_AUDIOIO = 0

# No RMT
CIRCUITPY_NEOPIXEL_WRITE = 0
CIRCUITPY_PULSEIO = 0
CIRCUITPY_RGBMATRIX = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0
# Features
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG = 0

#### esp32c3 ##########################################################
else ifeq ($(IDF_TARGET),esp32c3)
# Modules
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_MEMORYMAP = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No DAC
CIRCUITPY_AUDIOIO = 0

# No PCNT peripheral
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_COUNTIO = 0
CIRCUITPY_ROTARYIO = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0
# Features
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 1

#### esp32c6 ##########################################################
else ifeq ($(IDF_TARGET),esp32c6)
# Modules
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_MEMORYMAP = 0
CIRCUITPY_RGBMATRIX = 0

# No DAC
CIRCUITPY_AUDIOIO = 0

# No space for this
CIRCUITPY_AUDIOBUSIO = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0
# Features
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 1

#### esp32c6 ##########################################################
else ifeq ($(IDF_TARGET),esp32c61)
# Modules
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_MEMORYMAP = 0
CIRCUITPY_RGBMATRIX = 0

# No DAC
CIRCUITPY_AUDIOIO = 0

# No space for this
CIRCUITPY_AUDIOBUSIO = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0
# Features
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 1

# No TWAI on chip
CIRCUITPY_CANIO = 0

# No RMT on chip
CIRCUITPY_NEOPIXEL_WRITE = 0
CIRCUITPY_PULSEIO = 0
CIRCUITPY_RGBMATRIX = 0

# No PCNT on chip
CIRCUITPY_COUNTIO = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_FREQUENCYIO = 0

#### esp32h2 ##########################################################
else ifeq ($(IDF_TARGET),esp32h2)
# Modules
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_MEMORYMAP = 0
CIRCUITPY_RGBMATRIX = 0

# No DAC
CIRCUITPY_AUDIOIO = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0

# Features
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 1
CIRCUITPY_WIFI = 0

CIRCUITPY_MAX3421E = 0

#### esp32p4 ##########################################################
else ifeq ($(IDF_TARGET),esp32p4)

# No DAC
CIRCUITPY_AUDIOIO = 0

# No wifi
# TODO: Support ESP32-C6 coprocessor on some boards.
CIRCUITPY_BLEIO_NATIVE = 0
CIRCUITPY_WIFI = 0
CIRCUITPY_SSL = 0

CIRCUITPY_TOUCHIO = 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0

# Second stage bootloader doesn't work when the factory partition is empty due to
# UF2 missing.
UF2_BOOTLOADER = 0
CIRCUITPY_USB_HID = 0
CIRCUITPY_USB_MIDI = 0
CIRCUITPY_TUSB_MEM_ALIGN = 64

CIRCUITPY_MAX3421E = 0

# Update this for the 40mhz processor.
CIRCUITPY_ESPULP = 0

# Update this for multiple TWAI?
CIRCUITPY_CANIO = 0

# Protomatter needs an update
CIRCUITPY_RGBMATRIX = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# Library doesn't support P4 yet it seems
CIRCUITPY_ESPCAMERA = 0

# P4 has MIPI-DSI
CIRCUITPY_MIPIDSI = 1

#### esp32s2 ##########################################################
else ifeq ($(IDF_TARGET),esp32s2)
# Modules
CIRCUITPY_ALARM_TOUCH = $(CIRCUITPY_ALARM)
CIRCUITPY_AUDIOIO ?= 1

# No BLE in hw
CIRCUITPY_BLEIO_NATIVE = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 0

#### esp32s3 ##########################################################
else ifeq ($(IDF_TARGET),esp32s3)

# Modules
CIRCUITPY_ALARM_TOUCH = $(CIRCUITPY_ALARM)
CIRCUITPY_AUDIOBUSIO_PDMIN = 1
CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 0

# No DAC
CIRCUITPY_AUDIOIO = 0

endif
#### end chip-specific choices ########################################

# No room for large modules on 2MB boards
# 2MB boards have a single firmware partition, and can't do dualbank.
ifeq ($(CIRCUITPY_ESP_FLASH_SIZE),2MB)
CIRCUITPY__EVE = 0
CIRCUITPY_AUDIOMP3 = 0
CIRCUITPY_BITMAPFILTER = 0
CIRCUITPY_BLEIO_NATIVE = 0
CIRCUITPY_DUALBANK = 0
CIRCUITPY_MAX3421E = 0
CIRCUITPY_RGBMATRIX = 0
CIRCUITPY_SETTABLE_PROCESSOR_FREQUENCY = 0
OPTIMIZATION_FLAGS ?= -Os
endif

# 4MB boards have a single firmware partition, and can't do dualbank.
ifeq ($(CIRCUITPY_ESP_FLASH_SIZE),4MB)
CIRCUITPY_DUALBANK = 0
OPTIMIZATION_FLAGS ?= -Os
endif

# espcamera does not work on boards without SPIRAM
ifeq ($(CIRCUITPY_ESP_PSRAM_SIZE),0)
CIRCUITPY_ESPCAMERA = 0
endif

# Modules dependent on other modules
CIRCUITPY_ESPNOW ?= $(CIRCUITPY_WIFI)
CIRCUITPY_GIFIO ?= $(CIRCUITPY_DISPLAYIO)
CIRCUITPY_JPEGIO ?= $(CIRCUITPY_DISPLAYIO)
CIRCUITPY_QRIO ?= $(CIRCUITPY_ESPCAMERA)

CIRCUITPY_BLE_FILE_SERVICE ?= $(CIRCUITPY_BLEIO_NATIVE)
CIRCUITPY_SERIAL_BLE ?= $(CIRCUITPY_BLEIO_NATIVE)

# Features dependent on other features
ifneq ($(CIRCUITPY_USB_DEVICE),0)
CIRCUITPY_BUILD_EXTENSIONS ?= bin,uf2
else
CIRCUITPY_BUILD_EXTENSIONS ?= bin
endif

# From ESP32-S2/S3 Technical Reference Manual:
#
# Endpoint number 0 always present (bi-directional, consisting of EP0 IN and EP0 OUT)
# Six additional endpoints (endpoint numbers 1 to 6), configurable as IN or OUT
# Maximum of five IN endpoints concurrently active at any time (including EP0 IN)
#
# Due to the limited number of endpoints, some USB devices will be off by default.
# For instance MIDI is available, but the device is turned off. It can be turned on
# only if something else is turned off, such as HID.
USB_NUM_ENDPOINT_PAIRS = 7
USB_NUM_IN_ENDPOINTS = 5

# Usually lots of flash space available
CIRCUITPY_MESSAGE_COMPRESSION_LEVEL ?= 1
