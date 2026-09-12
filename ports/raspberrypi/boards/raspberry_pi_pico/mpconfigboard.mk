USB_VID = 0x239A
USB_PID = 0x80F4
USB_PRODUCT = "Pico"
USB_MANUFACTURER = "Raspberry Pi"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY__EVE = 1

CIRCUITPY_PICOGAME = 1
CIRCUITPY_PICOGAME_FAST_DISPLAY = 1
CIRCUITPY_PICODVI = 1

# The default is -O3. picogame does not fit at -O3; these loop passes keep the render
# kernels within 1% of it.
OPTIMIZATION_FLAGS = -O2 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-partial-pre -fsplit-paths
