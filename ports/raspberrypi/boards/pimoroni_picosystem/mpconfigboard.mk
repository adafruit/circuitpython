USB_VID = 0x16D0
USB_PID = 0x08C8
USB_PRODUCT = "PicoSystem"
USB_MANUFACTURER = "Pimoroni"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q128JVxQ"

CIRCUITPY__EVE = 1

CIRCUITPY_KEYPAD = 1
CIRCUITPY_STAGE = 1
CIRCUITPY_PICOGAME = 1
CIRCUITPY_PICOGAME_FAST_DISPLAY = 1

CIRCUITPY_AUDIOIO = 1

FROZEN_MPY_DIRS += $(TOP)/frozen/circuitpython-stage/picosystem

# The default is -O3. picogame does not fit at -O3; these loop passes keep the render
# kernels within 1% of it.
OPTIMIZATION_FLAGS = -O2 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-partial-pre -fsplit-paths
