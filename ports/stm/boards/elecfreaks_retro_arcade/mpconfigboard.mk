# pid.codes test PID, for development only. Needs a real VID/PID before release.
USB_VID = 0x1209
USB_PID = 0x0001
USB_PRODUCT = "Retro Arcade"
USB_MANUFACTURER = "ELECFREAKS"

INTERNAL_FLASH_FILESYSTEM = 1

BOOTLOADER_OFFSET = 0x8010000

MCU_SERIES = F4
MCU_VARIANT = STM32F412Rx
MCU_PACKAGE = LQFP64

CIRCUITPY_BUILD_EXTENSIONS = uf2

LD_COMMON = boards/common_default.ld
LD_FILE = boards/STM32F412xE_boot.ld

# The bootloader clears RTC->BKP0R on every boot, and alarm keeps its wake flag there.
CIRCUITPY_ALARM = 0

# ILI9341 on the FSMC 8-bit parallel bus.
CIRCUITPY_PARALLELDISPLAYBUS = 1

# Not usable with this hardware.
CIRCUITPY_BLEIO_HCI = 0
CIRCUITPY_EPAPERDISPLAY = 0
CIRCUITPY_FRAMEBUFFERIO = 0
CIRCUITPY_I2CDISPLAYBUS = 0
CIRCUITPY_KEYPAD_DEMUX = 0
CIRCUITPY_NEOPIXEL_WRITE = 0
CIRCUITPY_ONEWIREIO = 0
CIRCUITPY_PIXELBUF = 0
CIRCUITPY_PIXELMAP = 0
CIRCUITPY_RAINBOWIO = 0
CIRCUITPY_SDCARDIO = 0
CIRCUITPY_SHARPDISPLAY = 0
CIRCUITPY_TOUCHIO = 0

# Too large for the 447K firmware region.
CIRCUITPY_AESIO = 0
CIRCUITPY_AUDIOMP3 = 0
CIRCUITPY_BITMAPFILTER = 0
CIRCUITPY_CODEOP = 0
CIRCUITPY_MSGPACK = 0
CIRCUITPY_ULAB = 0
CIRCUITPY_ZLIB = 0

# Stage renderer and board adapter from circuitpython-stage.
CIRCUITPY_STAGE = 1
FROZEN_MPY_DIRS += $(TOP)/frozen/circuitpython-stage/elecfreaks_retro_arcade

# Native picogame renderer alongside Stage; use the generic parallel-display path.
CIRCUITPY_PICOGAME = 1

# picogame's core Python helpers, frozen because CIRCUITPY (32 KiB) can't hold them;
# see frozen/picogame-libs/frozen/core/README.md.
FROZEN_MPY_DIRS += $(TOP)/frozen/picogame-libs/frozen/core
# Room for them: modules a game console doesn't need.
CIRCUITPY_BITBANGIO = 0
CIRCUITPY_GIFIO = 0
CIRCUITPY_JPEGIO = 0
CIRCUITPY_PULSEIO = 0
CIRCUITPY_USB_MIDI = 0
CIRCUITPY_ATEXIT = 0
CIRCUITPY_GETPASS = 0
CIRCUITPY_LOCALE = 0
CIRCUITPY_WARNINGS = 0
