USB_VID = 0x2E8A
USB_PID = 0x107E
USB_PRODUCT = "Cytron EDU PICO 2 for Pico 2W"
USB_MANUFACTURER = "Cytron"

CHIP_VARIANT = RP2350
CHIP_PACKAGE = A
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY_SDCARDIO = 1

CIRCUITPY__EVE = 1

CIRCUITPY_CYW43 = 1
CIRCUITPY_SSL = 1
CIRCUITPY_HASHLIB = 1
CIRCUITPY_WEB_WORKFLOW = 1
CIRCUITPY_MDNS = 1
CIRCUITPY_SOCKETPOOL = 1
CIRCUITPY_WIFI = 1

CFLAGS += \
    -DCYW43_PIN_WL_DYNAMIC=0 \
	-DCYW43_DEFAULT_PIN_WL_HOST_WAKE=24 \
	-DCYW43_DEFAULT_PIN_WL_REG_ON=23 \
	-DCYW43_DEFAULT_PIN_WL_CLOCK=29 \
	-DCYW43_DEFAULT_PIN_WL_DATA_IN=24 \
	-DCYW43_DEFAULT_PIN_WL_DATA_OUT=24 \
	-DCYW43_DEFAULT_PIN_WL_CS=25 \
	-DCYW43_WL_GPIO_COUNT=3 \
	-DCYW43_WL_GPIO_LED_PIN=0
	-DCYW43_PIO_CLOCK_DIV_INT=3

# The default is -O3.
OPTIMIZATION_FLAGS = -Os

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Text
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Motor
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SimpleIO
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_framebuf
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_OPT4048
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SSD1306
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ImageLoad
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_AHTx0
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SD
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HTTPServer
FROZEN_MPY_DIRS += $(TOP)/frozen/CircuitPython_edupico2_paj7620

CFLAGS += -DCIRCUITPY_FIRMWARE_SIZE='(1536 * 1024)'
