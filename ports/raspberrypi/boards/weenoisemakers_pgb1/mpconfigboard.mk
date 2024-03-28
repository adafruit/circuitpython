USB_VID = 0x1209
USB_PID = 0xA182
USB_PRODUCT = "PGB-1"
USB_MANUFACTURER = "Wee Noise Makers"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "GD25Q64C"

CIRCUITPY__EVE = 1

FROZEN_MPY_DIRS += $(TOP)/ports/raspberrypi/boards/weenoisemakers_pgb1
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ImageLoad
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DisplayIO_SSD1306
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Text
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_MIDI
