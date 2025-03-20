USB_VID = 0x239A
USB_PID = 0x802C
USB_PRODUCT = "ItsyBitsy M4 Express"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD51G19A
CHIP_FAMILY = samd51

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "GD25Q16C,W25Q16JVxQ"
LONGINT_IMPL = MPZ

CIRCUITPY_CODEOP = 0
CIRCUITPY_FLOPPYIO = 0
CIRCUITPY_GIFIO = 0
CIRCUITPY_JPEGIO = 0
CIRCUITPY_TILEPALETTEMAPPER = 0

CIRCUITPY_BITBANG_APA102 = 1

# We don't have room for the fonts for terminalio for certain languages,
# so turn off terminalio, and if it's off and displayio is on,
# force a clean build.
# Note that we cannot test $(CIRCUITPY_DISPLAYIO) directly with an
# ifeq, because it's not set yet.
ifneq (,$(filter $(TRANSLATION),ja ko ru))
CIRCUITPY_TERMINALIO = 0
RELEASE_NEEDS_CLEAN_BUILD = $(CIRCUITPY_DISPLAYIO)
endif
