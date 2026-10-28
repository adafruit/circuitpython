USB_VID = 0x303A
USB_PID = 0x7003
USB_PRODUCT = "ESP32-S3-DevKitC-1-N8"
USB_MANUFACTURER = "Espressif"

IDF_TARGET = esp32s3

CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m
CIRCUITPY_ESP_FLASH_SIZE = 8MB

# --- environmental-collector frozen libraries -------------------------------
# Frozen bytecode executes in place from flash; the same library imported
# from lib/ is read into RAM and stays there. On a board without PSRAM that
# is the difference between running and not -- a Feather ESP32-S3 No PSRAM
# running this project, with its own code already cross-compiled to .mpy,
# had 6 KB of heap left once BLE + ESP-NOW + softAP were up and died on the
# next import.
#
# Listed literally rather than behind an include, even though the same 22
# lines repeat across every board this project targets: tools/
# ci_fetch_deps.py scans THIS file for "FROZEN_MPY_DIRS += $(TOP)/" to
# decide which submodules to check out, and it does not follow includes. An
# include builds locally and then fails CI at MKMANIFEST with empty frozen
# directories.
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
