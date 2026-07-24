USB_VID = 0x239A
USB_PID = 0x8176
USB_PRODUCT = "ting fx EP-2350"
USB_MANUFACTURER = "Teenage Engineering"

CHIP_VARIANT = RP2350
CHIP_PACKAGE = A
CHIP_FAMILY = rp2

# Winbond W25Q16JV, 2MB (16Mbit) on CS0, nothing on CS1.
EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

# Only 2MB of flash: firmware gets the default 1020kB and the CIRCUITPY drive
# gets the rest (~1MB).

# GPIO12-19 are needed for picodvi, but they are all in use here (I2S, I2C,
# LEDs, handle switches) and no pins are broken out.
CIRCUITPY_PICODVI = 0

CIRCUITPY__EVE = 1
