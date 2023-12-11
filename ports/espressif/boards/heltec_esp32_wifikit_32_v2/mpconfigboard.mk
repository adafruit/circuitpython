# Product URL: disappeared/discontinued
# Datasheet:    https://resource.heltec.cn/download/WiFi_Kit_32/WiFi%20Kit32.pdf
#               There is apparently a typo with TX/RX (and other) GPIO descriptions
#               in the Pin Description table: It should be GPIO1 / GPIO3 and all above 39 seem not to be available on esp32
#
# Schematic:    https://resource.heltec.cn/download/WiFi_Kit_32/WIFI_Kit_32_Schematic_diagram_V2.pdf
#               https://resource.heltec.cn/download/WiFi_Kit_32/WIFI_Kit_32_Schematic_diagram_V2.1.PDF
#               (same as V2, but V2.1 is better readable)
# Pinout:   https://resource.heltec.cn/download/WiFi_Kit_32/WIFI_Kit_32_pinoutDiagram_V2.pdf
#           https://resource.heltec.cn/download/WiFi_Kit_32/WIFI_Kit_32_pinoutDiagram_V2.1.pdf
#           (exactly same as V2 [md5sum of pdf is 9ee18bb5abe0a41fe98825cc756e8e3e])

CIRCUITPY_CREATOR_ID = 0x148E173C
# TODO: What value should it be?
CIRCUITPY_CREATION_ID = 0x00000000

USB_PRODUCT = "ESP32-WIFIKIT-32-V2"
USB_MANUFACTURER = "Heltec"

IDF_TARGET = esp32

DEBUG = 1
ESPTOOL_FLAGS = --before=default_reset --after=hard_reset --baud 921600

# This board doesn't have USB by default, it
# instead uses a CP2102 USB-to-Serial chip
CIRCUITPY_USB = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG = 0

# Todo: qio or dio?
CIRCUITPY_ESP_FLASH_MODE = dio
# esp-idf-config/sdkconfig-flash-26m.defaults does not exist in main branch
CIRCUITPY_ESP_FLASH_FREQ = 26m

# it's 64mBit = 8MB, right? The datasheet says 64mBit = 4MB
CIRCUITPY_ESP_FLASH_SIZE = 8MB

CIRCUITPY_ESPCAMERA = 0

CIRCUITPY_DISPLAYIO = 1

# This has something to do with XTAL Config support?:
CIRCUITPY_CANIO = 0

FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Shapes
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Text
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_RFM9x
