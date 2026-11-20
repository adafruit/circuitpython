USB_VID = 0x239A
USB_PID = 0x80AC
USB_PRODUCT = "FeatherS2"
USB_MANUFACTURER = "UnexpectedMaker"

IDF_TARGET = esp32s2

CIRCUITPY_ESP_FLASH_SIZE = 16MB
CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m

CIRCUITPY_ESP_PSRAM_SIZE = 8MB
CIRCUITPY_ESP_PSRAM_MODE = qio
CIRCUITPY_ESP_PSRAM_FREQ = 80m

CIRCUITPY_BITBANG_APA102 = 1

# Include these Python libraries in firmware.
# FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DotStar

# Fast e-paper does not fit in internal RAM.
CIRCUITPY_EPAPERDISPLAY_FAST = 0

# The ESP32-S2 shares its 152KB SRAM between IRAM and DRAM. -O2 overflows
# dram0_0_seg, so use the same optimization level as the smaller-flash boards.
OPTIMIZATION_FLAGS ?= -Os
