ifeq ($(IDF_TARGET),esp32c2)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else ifeq ($(IDF_TARGET),esp32c3)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else ifeq ($(IDF_TARGET),esp32p4)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else ifeq ($(IDF_TARGET),esp32c6)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else ifeq ($(IDF_TARGET),esp32h2)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else
IDF_TARGET_ARCH = xtensa
CROSS_COMPILE = xtensa-$(IDF_TARGET)-elf-
endif

# Use internal flash for CIRCUITPY drive
INTERNAL_FLASH_FILESYSTEM = 1

# Internal math library is substantially smaller than toolchain one
INTERNAL_LIBM = 0

# Longints can be implemented as mpz, as longlong, or not
LONGINT_IMPL = MPZ

# Default to no-psram
CIRCUITPY_ESP_PSRAM_SIZE ?= 0

# Some 4MB non-USB boards were initially defined with 2MB firmware, almost 2MB user_fs partitions.
# Others were defined with 1.4M+1.4M (now a single 2.8MB) firmware partitions / 1.2MB user_fs.
# Keep the former as is, so that the user filesystem will be unchanged.
CIRCUITPY_4MB_FLASH_LARGE_USER_FS_LAYOUT ?= 0

# Enable more features
CIRCUITPY_FULL_BUILD ?= 1

# If SSL is enabled, it's mbedtls
CIRCUITPY_SSL_MBEDTLS = 1

# Never use our copy of MBEDTLS
CIRCUITPY_HASHLIB_MBEDTLS_ONLY = 0

CIRCUITPY_PORT_SERIAL = 1

# These modules are implemented in ports/<port>/common-hal:
CIRCUITPY_ALARM ?= 1
CIRCUITPY_ALARM_TOUCH ?= 0
CIRCUITPY_ANALOGBUFIO ?= 1
CIRCUITPY_AUDIOBUSIO ?= 1
CIRCUITPY_AUDIOBUSIO_PDMIN ?= 0
CIRCUITPY_AUDIOIO ?= 0
CIRCUITPY_BLEIO_HCI = 0
CIRCUITPY_CANIO ?= 1
CIRCUITPY_COUNTIO ?= 1
CIRCUITPY_ESPCAMERA ?= 1
CIRCUITPY_ESPIDF ?= 1
CIRCUITPY_ESPULP ?= 1
CIRCUITPY_FRAMEBUFFERIO ?= 1
CIRCUITPY_FREQUENCYIO ?= 1
CIRCUITPY_HASHLIB ?= 1
CIRCUITPY_I2CTARGET ?= 0
CIRCUITPY_MAX3421E ?= 1
CIRCUITPY_MEMORYMAP ?= 1
CIRCUITPY_RCLCPY ?= 0
CIRCUITPY_NVM ?= 1
CIRCUITPY_PARALLELDISPLAYBUS ?= 1
CIRCUITPY_PS2IO ?= 1
CIRCUITPY_RGBMATRIX ?= 1
CIRCUITPY_ROTARYIO ?= 1
CIRCUITPY_SDIOIO ?= 1
CIRCUITPY_SYNTHIO_MAX_CHANNELS ?= 12
CIRCUITPY_TOUCHIO_USE_NATIVE ?= 1
CIRCUITPY_WATCHDOG ?= 1
CIRCUITPY_WIFI ?= 1
CIRCUITPY_SOCKETPOOL_IPV6 ?= 1

# Enable _eve module
CIRCUITPY__EVE ?= 1

# Conditionally turn off modules/features
ifeq ($(IDF_TARGET),esp32)
# Modules
CIRCUITPY_ALARM_TOUCH = 1
CIRCUITPY_AUDIOIO = 1
CIRCUITPY_RGBMATRIX = 0

# SDMMC not supported yet
CIRCUITPY_SDIOIO = 0

# Features
CIRCUITPY_USB_DEVICE = 0

else ifeq ($(IDF_TARGET),esp32c2)

# C2 ROM spits out the UART at 74880 when connected to a 26mhz crystal! Debug
# prints will default to that too.
# Modules
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_MEMORYMAP = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No PCNT peripheral
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_COUNTIO = 0
CIRCUITPY_ROTARYIO = 0

# No two wire automotive
CIRCUITPY_CANIO = 0

# No DMA from ADC
CIRCUITPY_ANALOGBUFIO = 0

# No I2S
CIRCUITPY_AUDIOBUSIO = 0

# No RMT
CIRCUITPY_NEOPIXEL_WRITE = 0
CIRCUITPY_PULSEIO = 0
CIRCUITPY_RGBMATRIX = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0
# Features
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG = 0

else ifeq ($(IDF_TARGET),esp32c3)
# Modules
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_MEMORYMAP = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No PCNT peripheral
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_COUNTIO = 0
CIRCUITPY_ROTARYIO = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0
# Features
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 1

# No room in flash.
CIRCUITPY_AESIO = 0
CIRCUITPY_KEYPAD_DEMUX = 0

else ifeq ($(IDF_TARGET),esp32c6)
# Modules
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_MEMORYMAP = 0
CIRCUITPY_RGBMATRIX = 0

# No space for this
CIRCUITPY_AUDIOBUSIO = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0
# Features
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 1

# Remove temporarily until 10265 is merged
CIRCUITPY_ULAB = 0

else ifeq ($(IDF_TARGET),esp32h2)
# Modules
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_MEMORYMAP = 0
CIRCUITPY_RGBMATRIX = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0

# Features
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 1
CIRCUITPY_WIFI = 0

CIRCUITPY_MAX3421E = 0

else ifeq ($(IDF_TARGET),esp32p4)

# No wifi
# TODO: Support ESP32-C6 coprocessor on some boards.
CIRCUITPY_BLEIO_NATIVE = 0
CIRCUITPY_WIFI = 0
CIRCUITPY_SSL = 0

CIRCUITPY_TOUCHIO = 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0

# Second stage bootloader doesn't work when the factory partition is empty due to
# UF2 missing.
UF2_BOOTLOADER = 0
USB_HIGHSPEED = 1
CIRCUITPY_USB_HID = 0
CIRCUITPY_USB_MIDI = 0
CIRCUITPY_TUSB_MEM_ALIGN = 64

CIRCUITPY_MAX3421E = 0

# Update this for the 40mhz processor.
CIRCUITPY_ESPULP = 0

# Update this for multiple TWAI?
CIRCUITPY_CANIO = 0

# Protomatter needs an update
CIRCUITPY_RGBMATRIX = 0

# No I80 support from the IDF
CIRCUITPY_PARALLELDISPLAYBUS = 0

# Library doesn't support P4 yet it seems
CIRCUITPY_ESPCAMERA = 0

else ifeq ($(IDF_TARGET),esp32s2)
# Modules
CIRCUITPY_ALARM_TOUCH = 1
CIRCUITPY_AUDIOIO = 1
# No BLE in hw
CIRCUITPY_BLEIO_NATIVE = 0

# No SDMMC
CIRCUITPY_SDIOIO = 0

CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 0

else ifeq ($(IDF_TARGET),esp32s3)
# Modules
CIRCUITPY_ALARM_TOUCH = 1
CIRCUITPY_AUDIOBUSIO_PDMIN = 1
CIRCUITPY_ESP_USB_SERIAL_JTAG ?= 0

# No room for _bleio on boards with 4MB flash
ifeq ($(CIRCUITPY_ESP_FLASH_SIZE),4MB)
CIRCUITPY_BLEIO_NATIVE ?= 0
endif

endif

# bitmapfilter does not fit on 4MB boards unless they are set up as camera boards
ifeq ($(CIRCUITPY_ESP_FLASH_SIZE),4MB)
CIRCUITPY_BITMAPFILTER ?= 0
OPTIMIZATION_FLAGS ?= -Os
CIRCUITPY_DUALBANK ?= 0
else
CIRCUITPY_DUALBANK ?= 1
endif

# We used to default to OTA partition layout but are moving away from it so that
# BLE and alarm can be included. This setting prevents the partition layout from
# changing.
ifeq ($(CIRCUITPY_LEGACY_4MB_FLASH_LAYOUT), 1)
ifeq ($(IDF_TARGET_ARCH), xtensa)
	CIRCUITPY_ALARM ?= 1
else
CIRCUITPY_ALARM = 0
endif
CIRCUITPY_DUALBANK = 1
CIRCUITPY_BLEIO_NATIVE ?= 0
CIRCUITPY_SETTABLE_PROCESSOR_FREQUENCY = 0
else
CIRCUITPY_SETTABLE_PROCESSOR_FREQUENCY = 1
endif

# No room for dualbank or mp3 on boards with 2MB flash
ifeq ($(CIRCUITPY_ESP_FLASH_SIZE),2MB)
CIRCUITPY_BITMAPFILTER ?= 0
CIRCUITPY_DUALBANK = 0
CIRCUITPY_AUDIOMP3 = 0
CIRCUITPY_BLEIO_NATIVE ?= 0
endif

# No room for _eve on boards with 4MB flash
ifeq ($(CIRCUITPY_ESP_FLASH_SIZE),4MB)
CIRCUITPY__EVE = 0
endif

# default BLEIO after flash-size based defaults
CIRCUITPY_BLEIO_NATIVE ?= 1

# Modules dependent on other modules
CIRCUITPY_ESPNOW ?= $(CIRCUITPY_WIFI)
CIRCUITPY_GIFIO ?= $(CIRCUITPY_DISPLAYIO)
CIRCUITPY_JPEGIO ?= $(CIRCUITPY_DISPLAYIO)
CIRCUITPY_QRIO ?= $(CIRCUITPY_ESPCAMERA)

CIRCUITPY_BLE_FILE_SERVICE ?= $(CIRCUITPY_BLEIO_NATIVE)
CIRCUITPY_SERIAL_BLE ?= $(CIRCUITPY_BLEIO_NATIVE)

# Features dependent on other features
ifneq ($(CIRCUITPY_USB_DEVICE),0)
CIRCUITPY_BUILD_EXTENSIONS ?= bin,uf2
else
CIRCUITPY_BUILD_EXTENSIONS ?= bin
endif

# From ESP32-S2/S3 Technical Reference Manual:
#
# Endpoint number 0 always present (bi-directional, consisting of EP0 IN and EP0 OUT)
# Six additional endpoints (endpoint numbers 1 to 6), configurable as IN or OUT
# Maximum of five IN endpoints concurrently active at any time (including EP0 IN)
#
# Due to the limited number of endpoints, some USB devices will be off by default.
# For instance MIDI is available, but the device is turned off. It can be turned on
# only if something else is turned off, such as HID.
USB_NUM_ENDPOINT_PAIRS = 7
USB_NUM_IN_ENDPOINTS = 5

# Usually lots of flash space available
CIRCUITPY_MESSAGE_COMPRESSION_LEVEL ?= 1
