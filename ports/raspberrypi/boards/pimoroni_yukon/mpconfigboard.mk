USB_VID = 0x2E8A
USB_PID = 0x105B
USB_PRODUCT = "Yukon"
USB_MANUFACTURER = "Pimoroni"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q128JVxQ"

CIRCUITPY__EVE = 1

CIRCUITPY_TCA9555R = 1

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Motor
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DotStar
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
