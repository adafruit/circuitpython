USB_VID = 0x2E8A
USB_PID = 0x112F
USB_PRODUCT = "Maker Sumo RP2350"
USB_MANUFACTURER = "Cytron"

CHIP_VARIANT = RP2350
CHIP_PACKAGE = A
CHIP_FAMILY = rp2

# VERIFY against the flash chip on the board (W25Q16JVxQ = 2 MB)
EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY__EVE = 1

# GPIO12-19 needed for picodvi, but they are used by the buzzer, motors and servos.
CIRCUITPY_PICODVI = 0
