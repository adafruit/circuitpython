CIRCUITPY_CREATOR_ID = 0x000C303A
CIRCUITPY_CREATION_ID = 0x00C61001

IDF_TARGET = esp32c61

# Freeze the collector dependencies to match the other Env Hub builds.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BLE
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HTTPServer
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Requests
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ConnectionManager
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NTP
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Ticks
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SEN6x
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SCD4X
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SCD30
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_MAX1704x
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LC709203F
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_JD79667
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_IL0373
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Text
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Bitmap_Font
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_PCF8563
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DS3231
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_PCF8523
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DS1307
IDF_TARGET_ARCH = riscv

CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m
CIRCUITPY_ESP_FLASH_SIZE = 8MB

CIRCUITPY_ESP_PSRAM_SIZE = 2MB
CIRCUITPY_ESP_PSRAM_MODE = qio
CIRCUITPY_ESP_PSRAM_FREQ = 80m
