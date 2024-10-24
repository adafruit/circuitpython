USB_VID = 0x2886
USB_PID = 0xF004
USB_PRODUCT = "iMX RT1011 Nano Kit"
USB_MANUFACTURER = "Makerdiary"

CHIP_VARIANT = MIMXRT1011DAE5A
CHIP_FAMILY = MIMXRT1011
FLASH = W25Q128JV

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HID
