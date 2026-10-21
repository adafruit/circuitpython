# USB_VID/USB_PID are borrowed from this board's fork parent rather than a pair registered to
# us: this board's USB is never exposed externally once shipped, so these values are only ever
# seen by internal tooling, not an end user's machine.
USB_VID = 0x303A
USB_PID = 0x7003
USB_PRODUCT = "Bionebicine PDS 2026"
USB_MANUFACTURER = "Bionebicine"

IDF_TARGET = esp32s3

CIRCUITPY_ESP_FLASH_SIZE = 8MB
CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m

CIRCUITPY_ESP_PSRAM_SIZE = 8MB
CIRCUITPY_ESP_PSRAM_MODE = opi
CIRCUITPY_ESP_PSRAM_FREQ = 80m
