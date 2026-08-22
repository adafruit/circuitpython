# All linking can be done with this common templated linker script, which has
# parameters that vary based on chip and/or board.
LD_TEMPLATE_FILE = boards/common.template.ld

INTERNAL_LIBM = 1

CIRCUITPY_BUILD_EXTENSIONS ?= uf2

# Number of USB endpoint pairs.
USB_NUM_ENDPOINT_PAIRS = 8

# The nRF52 USBD implements isochronous transfers only on the fixed, dedicated
# endpoint number 8 (EP_ISO_NUM in TinyUSB's dcd_nrf5x.c); dcd_edpt_open() rejects
# an ISO endpoint on any other number, so a usb_audio mic/speaker placed on a
# sequentially-allocated endpoint would enumerate but never transfer data. Force
# the usb_audio isochronous endpoint onto endpoint 8. (Only read by usb_audio
# code, so it is harmless when CIRCUITPY_USB_AUDIO is off; defined unconditionally
# to stay independent of where that flag gets set in the include order.)
CFLAGS += -DUSB_AUDIO_ISO_EP_NUM=8

# All nRF ports have longints.
LONGINT_IMPL = MPZ

# The ?='s allow overriding in mpconfigboard.mk.

# Audio via PWM
CIRCUITPY_AUDIOIO = 0
CIRCUITPY_AUDIOBUSIO ?= 1
CIRCUITPY_AUDIOCORE ?= 1
CIRCUITPY_AUDIOMIXER ?= 1
CIRCUITPY_AUDIOPWMIO ?= 1
CIRCUITPY_SYNTHIO_MAX_CHANNELS = 12

# Native BLEIO is not compatible with HCI _bleio.
CIRCUITPY_BLEIO_HCI = 0

CIRCUITPY_BLEIO_NATIVE ?= 1

# No I2CPeripheral implementation
CIRCUITPY_I2CTARGET = 0

CIRCUITPY_RTC ?= 1

CIRCUITPY_SETTINGS_TOML ?= 1

# frequencyio not yet implemented
CIRCUITPY_FREQUENCYIO = 0

CIRCUITPY_ROTARYIO_SOFTENCODER = 1

# Sleep and Wakeup
CIRCUITPY_ALARM ?= 1

# Turn on the BLE file and serial services for BLE workflow
CIRCUITPY_BLE_FILE_SERVICE ?= 1
CIRCUITPY_BLE_SERIAL_SERVICE ?= 1

CIRCUITPY_COMPUTED_GOTO_SAVE_SPACE ?= 1


# nRF52840-specific

ifeq ($(MCU_CHIP),nrf52840)
MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52840

# Fits on nrf52840 but space is tight on nrf52833.
CIRCUITPY_AESIO ?= 1
CIRCUITPY_MEMORYMAP ?= 1

CIRCUITPY_RGBMATRIX ?= 1
CIRCUITPY_FRAMEBUFFERIO ?= 1

CIRCUITPY_HASHLIB ?= 1

CIRCUITPY_COUNTIO ?= 1
CIRCUITPY_WATCHDOG ?= 1

SD ?= s140
SOFTDEV_VERSION ?= 6.1.0

BOOT_SETTING_ADDR = 0xFF000
NRF_DEFINES += -DNRF52840_XXAA -DNRF52840

# CircuitPython doesn't yet support NFC so force the NFC antenna pins to be GPIO.
# See https://github.com/adafruit/circuitpython/issues/1300
# Defined here because system_nrf52840.c doesn't #include any of our own include files.
#
# One of the three UICR writes on the boot path.  if NFCPINS.PROTECT
# still says NFC, SystemInit clears it and resets, permanently.
# A board whose UICR must not be touched sets NRF_NFCT_PINS_AS_GPIOS = 0
NRF_NFCT_PINS_AS_GPIOS ?= 1
ifeq ($(NRF_NFCT_PINS_AS_GPIOS),1)
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
endif

ifeq ($(INTERNAL_FLASH_FILESYSTEM),1)
  OPTIMIZATION_FLAGS ?= -Os
  CIRCUITPY_LTO = 1
  CIRCUITPY_LTO_PARTITION = balanced
else
  CIRCUITPY_USB_AUDIO ?= 1
endif

else
ifeq ($(MCU_CHIP),nrf52833)
MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52833

# Need the space
SUPEROPT_GC ?= 0
SUPEROPT_VM ?= 0

CIRCUITPY_SYNTHIO ?= 0

SD ?= s140
SOFTDEV_VERSION ?= 7.0.1

BOOT_SETTING_ADDR = 0x7F000
NRF_DEFINES += -DNRF52833_XXAA -DNRF52833

OPTIMIZATION_FLAGS ?= -Os

CIRCUITPY_LTO = 1
CIRCUITPY_LTO_PARTITION = one
ifeq ($(INTERNAL_FLASH_FILESYSTEM),1)
  CIRCUITPY_FULL_BUILD ?= 0
  CIRCUITPY_PULSEIO ?= 1
endif
endif
endif

# Cannot have BLEIO without SoftDevice
ifeq ($(SD), )
CIRCUITPY_BLEIO_NATIVE = 0
CIRCUITPY_BLE_FILE_SERVICE = 0
CIRCUITPY_BLE_SERIAL_SERVICE = 0
endif
