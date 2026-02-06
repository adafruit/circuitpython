USB_VID = 0x2E8A
USB_PID = 0x1029
USB_PRODUCT = "W5500-EVB-Pico"
USB_MANUFACTURER = "WIZnet"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY__EVE = 1
CIRCUITPY_FLOPPYIO = 0
CIRCUITPY_SSL_NATIVE = 1
CIRCUITPY_USB_HOST = 0
# cannot have both AirLift and Ethernet support simultaneously, due to SSL conflicts.
CIRCUITPY_WIFI_AIRLIFT = 0

# The default is -O3. Change to -O2 because the build was overflowing.
OPTIMIZATION_FLAGS = -O2
