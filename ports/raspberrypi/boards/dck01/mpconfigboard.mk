USB_VID = 0x1209
USB_PID = 0xA182
USB_PRODUCT = "RP2040 DCK01"
USB_MANUFACTURER = "Bradán Lane STUDIO"
CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "GD25Q64C"

CIRCUITPY__EVE = 1

FROZEN_MPY_DIRS += $(TOP)/ports/raspberrypi/boards/dck01
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HID
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
