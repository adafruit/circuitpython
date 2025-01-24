CIRCUITPY_CREATOR_ID =  0x10151015
CIRCUITPY_CREATION_ID = 0x0032000C

IDF_TARGET = esp32

CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m
CIRCUITPY_ESP_FLASH_SIZE = 8MB

CIRCUITPY_ESP_PSRAM_MODE = qio
CIRCUITPY_ESP_PSRAM_FREQ = 80m
CIRCUITPY_ESP_PSRAM_SIZE = 2MB

# The safe mode wait gets us very close to the 3s time for the board to shut
# down when BTN_C/PWR is held down. We skip the wait and instead enter safe
# mode if BTN_A is held down during boot with no timeout.
CIRCUITPY_SKIP_SAFE_MODE_WAIT = 1

# Enable PDMIn for the microphone
CIRCUITPY_AUDIOBUSIO_PDMIN = 1
