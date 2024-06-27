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

CIRCUITPY_CREATOR_ID  = 0x148E173C
CIRCUITPY_CREATION_ID = 0x00320001

USB_PRODUCT = "esp32-wifikit-32-v2"
USB_MANUFACTURER = "Heltec"

IDF_TARGET = esp32

DEBUG = 0
ESPTOOL_FLAGS = --before=default_reset --after=hard_reset --baud 921600

CIRCUITPY_ESP_FLASH_MODE = dio
# esp-idf-config/sdkconfig-flash-26m.defaults does not exist in main branch
CIRCUITPY_ESP_FLASH_FREQ = 26m

# This has something to do with XTAL Config support?:
# However, without his setting cp refuses to build
CIRCUITPY_CANIO = 0

CIRCUITPY_ESP_FLASH_SIZE = 8MB

CIRCUITPY_ESPCAMERA = 0

CIRCUITPY_DISPLAYIO = 1
