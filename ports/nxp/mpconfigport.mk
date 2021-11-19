# All linking can be done with this common templated linker script, which has
# parameters that vary based on chip and/or board.
LD_TEMPLATE_FILE = boards/common.template.ld

# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.

ifeq ($(LONGINT_IMPL),NONE)
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=none
endif

ifeq ($(LONGINT_IMPL),MPZ)
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz
endif

ifeq ($(LONGINT_IMPL),LONGLONG)
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=longlong
endif

# INTERNAL_LIBM = 1

USB_SERIAL_NUMBER_LENGTH = 32

# Number of USB endpoint pairs.
USB_NUM_ENDPOINT_PAIRS = 8

ifndef USB_NUM_ENDPOINT_PAIRS
$(error "USB_NUM_ENDPOINT_PAIRS (number of USB endpoint pairs)must be defined")
endif

CIRCUITPY_ROTARYIO_SOFTENCODER = 1

######################################################################
# Put lpc1700-only choices here.

ifeq ($(CHIP_FAMILY),lpc1700)

# The ?='s allow overriding in mpconfigboard.mk.

CIRCUITPY_AUDIOMIXER ?= 0
CIRCUITPY_BINASCII ?= 0
CIRCUITPY_AUDIOMP3 ?= 0
CIRCUITPY_BUILTINS_POW3 ?= 0
CIRCUITPY_COMPUTED_GOTO_SAVE_SPACE ?= 1
CIRCUITPY_FREQUENCYIO ?= 0
CIRCUITPY_JSON ?= 0
CIRCUITPY_SYNTHIO ?= 0
CIRCUITPY_TOUCHIO_USE_NATIVE ?= 1

# No room for HCI _bleio on SAMD21.
CIRCUITPY_BLEIO_HCI = 0

CIRCUITPY_SDCARDIO ?= 0

# Not enough RAM for framebuffers
CIRCUITPY_FRAMEBUFFERIO ?= 0

# Not enough room in 192kB or 256kB builds for secondary CDC.
CIRCUITPY_USB_CDC ?= 0

CIRCUITPY_ULAB = 0

DISABLE_FILESYSTEM = 0

ifeq ($(TRANSLATION), ja)
RELEASE_NEEDS_CLEAN_BUILD = 1
CIRCUITPY_TERMINALIO = 0
endif

ifeq ($(TRANSLATION), ko)
RELEASE_NEEDS_CLEAN_BUILD = 1
CIRCUITPY_TERMINALIO = 0
endif

endif # lpc1700
######################################################################

CIRCUITPY_CANIO ?= 1
