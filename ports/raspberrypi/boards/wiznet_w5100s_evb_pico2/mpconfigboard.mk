USB_VID = 0x2E8A
USB_PID = 0x109E
USB_PRODUCT = "W5100S-EVB-Pico2"
USB_MANUFACTURER = "WIZnet"

CHIP_VARIANT = RP2350
CHIP_PACKAGE = A
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q32JVxQ"

CIRCUITPY__EVE = 1
CIRCUITPY_SSL_NATIVE = 1
# cannot have both AirLift and Ethernet support simultaneously, due to SSL conflicts.
CIRCUITPY_WIFI_AIRLIFT = 0

# The default is -O3. Change to -O2 because the build was overflowing.
OPTIMIZATION_FLAGS = -O2
