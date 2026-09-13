# All raspberrypi ports have longints.
LONGINT_IMPL = MPZ

CIRCUITPY_OPTIMIZE_PROPERTY_FLASH_SIZE ?= 1
# CYW43 support does not provide settable MAC addresses for station or AP.
CIRCUITPY_WIFI_RADIO_SETTABLE_MAC_ADDRESS = 0

CIRCUITPY_RP2PIO ?= 1
CIRCUITPY_NEOPIXEL_WRITE ?= $(CIRCUITPY_RP2PIO)
CIRCUITPY_FLOPPYIO ?= 1
CIRCUITPY_FRAMEBUFFERIO ?= $(CIRCUITPY_DISPLAYIO)
CIRCUITPY_FULL_BUILD ?= 1
CIRCUITPY_AUDIOMP3 ?= 1
CIRCUITPY_AUDIOSPEED ?= 1
CIRCUITPY_AUDIOEFFECTS ?= 1
CIRCUITPY_AUDIOFILEWRITER ?= 1
CIRCUITPY_BITOPS ?= 1
CIRCUITPY_HASHLIB ?= 1
CIRCUITPY_HASHLIB_MBEDTLS ?= 1
CIRCUITPY_IMAGECAPTURE ?= 1
CIRCUITPY_LOAD_NATIVE ?= 1
CIRCUITPY_MAX3421E ?= 0
CIRCUITPY_MCP4822 ?= 0
CIRCUITPY_MEMORYMAP ?= 1
CIRCUITPY_PWMIO ?= 1
CIRCUITPY_RGBMATRIX ?= $(CIRCUITPY_DISPLAYIO)
CIRCUITPY_ROTARYIO ?= 1
CIRCUITPY_ROTARYIO_SOFTENCODER = 1
CIRCUITPY_SYNTHIO_MAX_CHANNELS = 24
CIRCUITPY_USB_HOST ?= 1
CIRCUITPY_USB_VIDEO ?= 1
CIRCUITPY_USB_AUDIO ?= 1

# Things that need to be implemented.
CIRCUITPY_FREQUENCYIO = 0

# Use PWM internally
CIRCUITPY_I2CTARGET = 1
CIRCUITPY_NVM = 1
# Use PIO internally
CIRCUITPY_PULSEIO ?= 1
CIRCUITPY_WATCHDOG ?= 1

# Use of analogbufio
CIRCUITPY_ANALOGBUFIO = 1

# Audio via PWM
CIRCUITPY_AUDIOIO = 0
CIRCUITPY_AUDIOBUSIO ?= 1
CIRCUITPY_AUDIOI2SIN ?= $(CIRCUITPY_AUDIOBUSIO)
CIRCUITPY_AUDIOCORE ?= 1
CIRCUITPY_AUDIOPWMIO ?= 1

CIRCUITPY_AUDIOMIXER ?= 1

ifeq ($(CHIP_VARIANT),RP2040)
CIRCUITPY_ALARM ?= 1

# Default PICODVI off because it uses RAM to store code run on the second CPU for RP2040.
CIRCUITPY_PICODVI ?= 0

CIRCUITPY_TOUCHIO ?= 1

# delay in ms before calling cyw43_arch_init_with_country
CIRCUITPY_CYW43_INIT_DELAY ?= 1000

# -O2 plus the five -O3 loop passes that help here, leaving out the eight that make loops
# faster by duplicating them: function cloning, peeling, unroll-and-jam, splitting,
# interchange and versioning for strides. The vectorizer stays on - it is already on at -O2 -
# but without those passes it rarely finds a loop to work on, and on this core it was not
# winning anything anyway. Measured on a full build: benchmarks within 1 % of -O3 and about
# 150 KB less flash, which is what lets a board fit.
#   -funswitch-loops        moves a test that cannot change inside the loop out of it
#   -fpredictive-commoning  reuses what the previous iteration already loaded
#   -fgcse-after-reload     drops loads still redundant once registers are assigned
#   -ftree-partial-pre      computes a value once when only some paths need it
#   -fsplit-paths           duplicates a small piece of a loop so the passes above see through it
# RP2350 keeps plain -O3: there the duplicating passes are what lets the vectorizer pair
# 16-bit writes, and fill loops run 24-41 % slower without them.
OPTIMIZATION_FLAGS ?= -O2 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-partial-pre -fsplit-paths
endif

ifeq ($(CHIP_VARIANT),RP2350)
# RP2350 has PSRAM that is not DMA-capable
CIRCUITPY_ALL_MEMORY_DMA_CAPABLE = 0

# This needs to be implemented.
CIRCUITPY_ALARM = 0
# Default PICODVI on because it doesn't require much code in RAM to talk to HSTX.
CIRCUITPY_PICODVI ?= 1

# delay in ms before calling cyw43_arch_init_with_country
CIRCUITPY_CYW43_INIT_DELAY ?= 0

# Audio effects
CIRCUITPY_AUDIOEFFECTS ?= 1
endif

INTERNAL_LIBM = 1

CIRCUITPY_BUILD_EXTENSIONS ?= uf2

# Number of USB endpoint pairs.
USB_NUM_ENDPOINT_PAIRS = 8

INTERNAL_FLASH_FILESYSTEM = 1
CIRCUITPY_SETTABLE_PROCESSOR_FREQUENCY = 1

# Usually lots of flash space available
CIRCUITPY_MESSAGE_COMPRESSION_LEVEL ?= 1

# (ssl is selectively enabled but it's always the mbedtls implementation)
CIRCUITPY_SSL_MBEDTLS = 1
