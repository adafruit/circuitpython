USB_VID = 0x303a
USB_PID = 0x83A2
USB_PRODUCT = "ESP32-S3-ETH-8DI-8RO"
USB_MANUFACTURER = "Waveshare"

IDF_TARGET = esp32s3

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SD
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Wiznet5k
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Ticks
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Requests
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_FakeRequests
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ConnectionManager
FROZEN_MPY_DIRS += $(TOP)/frozen/circuitpython-pcf85063a
FROZEN_MPY_DIRS += $(TOP)/frozen/circuitpython_ef_music

CIRCUITPY_I2CIOEXPANDER = 1

CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m
CIRCUITPY_ESP_FLASH_SIZE = 16MB

CIRCUITPY_ESP_PSRAM_MODE = opi
CIRCUITPY_ESP_PSRAM_FREQ = 80m
CIRCUITPY_ESP_PSRAM_SIZE = 8MB
