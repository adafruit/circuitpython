USB_VID = 0x04D8
USB_PID = 0xE52B
USB_PRODUCT = "Microchip Curiosity CircuitPython Nano"
USB_MANUFACTURER = "Microchip Technology Inc"

CHIP_VARIANT = SAME51J20A
CHIP_FAMILY = same51

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "SST26VF016B,SST26VF032B,SST26VF064B"
LONGINT_IMPL = MPZ
CIRCUITPY_ULAB = 0

CIRCUITPY__EVE = 1
CIRCUITPY_BITMAPFILTER = 0
CIRCUITPY_CANIO = 1
CIRCUITPY_FLOPPYIO = 0
CIRCUITPY_SYNTHIO = 0
CIRCUITPY_GIFIO = 0
CIRCUITPY_JPEGIO = 0

CIRCUITPY_LTO_PARTITION = one

# We don't have room for the fonts for terminalio for certain languages,
# so turn off terminalio, and if it's off and displayio is on,
# force a clean build.
# Note that we cannot test $(CIRCUITPY_DISPLAYIO) directly with an
# ifeq, because it's not set yet.
ifneq (,$(filter $(TRANSLATION),ja ko ru))
CIRCUITPY_TERMINALIO = 0
RELEASE_NEEDS_CLEAN_BUILD = $(CIRCUITPY_DISPLAYIO)
endif
