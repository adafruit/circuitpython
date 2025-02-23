# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
# SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
#
# SPDX-License-Identifier: MIT

# Includes mpconfigboard.mk & mpconfigport.mk,
# along with numerous other shared environment makefiles.
include ../../py/circuitpy_mkenv.mk

CROSS_COMPILE = arm-none-eabi-

# MCU_SERIES e.g. 	"max32"
# MCU_VARIANT e.g. 	"max32690"
# defined in mpconfigboard.mk
MCU_SERIES_LOWER := $(shell echo $(MCU_SERIES) | tr '[:upper:]' '[:lower:]')
MCU_SERIES_UPPER := $(shell echo $(MCU_SERIES) | tr '[:lower:]' '[:upper:]')
MCU_VARIANT_LOWER := $(shell echo $(MCU_VARIANT) | tr '[:upper:]' '[:lower:]')
MCU_VARIANT_UPPER := $(shell echo $(MCU_VARIANT) | tr '[:lower:]' '[:upper:]')

# *******************************************************************************
#### MSDK INCLUDES ####
# Necessary for msdk makefiles
TARGET := $(MCU_VARIANT_UPPER)
TARGET_UC := $(MCU_VARIANT_UPPER)
TARGET_LC := $(MCU_VARIANT_LOWER)

MSDK_ROOT = ./msdk
MSDK_LIBS = $(MSDK_ROOT)/Libraries
CMSIS_ROOT = $(MSDK_LIBS)/CMSIS
ADI_PERIPH = $(MSDK_ROOT)/Libraries/PeriphDrivers
ADI_MISC_DRIVERS_DIR ?= $(MSDK_LIBS)/MiscDrivers
ADI_BOARD_DIR = $(MSDK_LIBS)/Boards/$(MCU_VARIANT_UPPER)/$(BOARD)

# For debugging the build
ifneq ($(BUILD_VERBOSE),"")
$(info MSDK_ROOT is $(MSDK_ROOT))
$(info MSDK_LIBS is $(MSDK_LIBS))
$(info CMSIS_ROOT is $(CMSIS_ROOT))
$(info ADI_PERIPH is $(ADI_PERIPH))
$(info ADI_MISC_DRIVERS_DIR is $(ADI_MISC_DRIVERS_DIR))
$(info ADI_BOARD_DIR is $(ADI_BOARD_DIR))
$(info MAXIM_PATH is $(MAXIM_PATH))
endif

# -----------------
# Sources & Include
# -----------------
# Define max32 die type for PeriphDriver Includes
# default to me18 for max32690
# more info:
# https://analogdevicesinc.github.io/msdk//USERGUIDE/#die-types-to-part-numbers
ifeq ($(MCU_VARIANT_LOWER), "max32690")
DIE_TYPE=me18
else
DIE_TYPE=me18
endif

PERIPH_SRC = $(ADI_PERIPH)/Source

INC += -I.
INC += -I../..
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr
INC += -I./../../lib/cmsis/inc
INC += -I./boards/
INC += -I./boards/$(BOARD)
INC += -I./peripherals/
INC += -I../../lib/mp-readline

INC += \
	-I$(TOP)/$(BOARD_PATH) \
	-I$(TOP)/lib/cmsis/inc \
	-I$(CMSIS_ROOT)/Include \
	-I$(CMSIS_ROOT)/Device/Maxim/$(MCU_VARIANT_UPPER)/Include \
	-I$(ADI_PERIPH)/Include/$(MCU_VARIANT_UPPER) \
	-I$(PERIPH_SRC)/SYS \
	-I$(PERIPH_SRC)/CTB \
	-I$(PERIPH_SRC)/DMA \
	-I$(PERIPH_SRC)/FLC \
	-I$(PERIPH_SRC)/GPIO \
	-I$(PERIPH_SRC)/ICC \
	-I$(PERIPH_SRC)/TMR \
	-I$(PERIPH_SRC)/RTC \
	-I$(PERIPH_SRC)/UART

INC += -I$(CMSIS_ROOT)/Device/Maxim/$(MCU_VARIANT_UPPER)/Source/GCC

SRC_MAX32 += \
	$(CMSIS_ROOT)/Device/Maxim/$(MCU_VARIANT_UPPER)/Source/heap.c \
	$(CMSIS_ROOT)/Device/Maxim/$(MCU_VARIANT_UPPER)/Source/system_$(MCU_VARIANT_LOWER).c \
	$(PERIPH_SRC)/SYS/mxc_assert.c \
	$(PERIPH_SRC)/SYS/mxc_delay.c \
	$(PERIPH_SRC)/SYS/mxc_lock.c \
	$(PERIPH_SRC)/SYS/nvic_table.c \
	$(PERIPH_SRC)/SYS/pins_$(DIE_TYPE).c \
	$(PERIPH_SRC)/SYS/sys_$(DIE_TYPE).c \
	$(PERIPH_SRC)/CTB/ctb_$(DIE_TYPE).c \
	$(PERIPH_SRC)/CTB/ctb_reva.c \
	$(PERIPH_SRC)/CTB/ctb_common.c \
	$(PERIPH_SRC)/DMA/dma_reva.c \
	$(PERIPH_SRC)/DMA/dma_$(DIE_TYPE).c \
	$(PERIPH_SRC)/FLC/flc_common.c \
	$(PERIPH_SRC)/FLC/flc_$(DIE_TYPE).c \
	$(PERIPH_SRC)/FLC/flc_reva.c \
	$(PERIPH_SRC)/GPIO/gpio_common.c \
	$(PERIPH_SRC)/GPIO/gpio_$(DIE_TYPE).c \
	$(PERIPH_SRC)/GPIO/gpio_reva.c \
	$(PERIPH_SRC)/ICC/icc_$(DIE_TYPE).c \
	$(PERIPH_SRC)/ICC/icc_reva.c \
	$(PERIPH_SRC)/RTC/rtc_$(DIE_TYPE).c \
	$(PERIPH_SRC)/RTC/rtc_reva.c \
	$(PERIPH_SRC)/TMR/tmr_common.c \
	$(PERIPH_SRC)/TMR/tmr_revb.c \
	$(PERIPH_SRC)/TMR/tmr_$(DIE_TYPE).c \
	$(PERIPH_SRC)/UART/uart_common.c \
	$(PERIPH_SRC)/UART/uart_$(DIE_TYPE).c \
	$(PERIPH_SRC)/UART/uart_revb.c

SRC_C += $(SRC_MAX32) \
			boards/$(BOARD)/board.c \
			boards/$(BOARD)/pins.c \
			peripherals/$(MCU_VARIANT_LOWER)/pins.c \
			peripherals/$(MCU_VARIANT_LOWER)/gpios.c

# *******************************************************************************
### Compiler & Linker Flags ###
COMPILER ?= GCC

ifeq ($(COMPILER), GCC)

STARTUPFILE = $(CMSIS_ROOT)/Device/Maxim/$(MCU_VARIANT_UPPER)/Source/GCC/startup_$(MCU_VARIANT_LOWER).s
# STARTUPFILE = $(ADI_BOARD_DIR)/Source/startup_$(MCU_VARIANT_LOWER).s

# CircuitPython custom linkerfile (necessary for build steps & filesystems)
LINKERFILE = linking/$(MCU_VARIANT_LOWER)_cktpy.ld
LDFLAGS += -nostartfiles -specs=nano.specs
endif

SRC_S += supervisor/cpu.s \
			$(STARTUPFILE)

# Needed to compile some MAX32 headers
CFLAGS += 	-D$(MCU_VARIANT_UPPER) \
			-DTARGET_REV=0x4131 \
			-DTARGET=$(MCU_VARIANT_UPPER) \
			-DIAR_PRAGMAS=0 \
			-DRISCV_LOAD=0 \
			-DCONFIG_TRUSTED_EXECUTION_SECURE=0

# todo: add these for linkerfiles later on so that it's easier to add new boards
# -DFLASH_ORIGIN \
# -DFLASH_SIZE \
# -DSRAM_ORIGIN \
# -DSRAM_SIZE

CPU_CORE=cortex-m4
CFLAGS += -mthumb -mcpu=$(CPU_CORE) -mfloat-abi=softfp -mfpu=fpv4-sp-d16

# NOTE: Start with DEBUG=1 defaults for now
ifeq ($(DEBUG),)
DEBUG ?= 1
endif

ifeq ($(DEBUG),1)
COPT = -ggdb3 -Og -Os
else
COPT += -Os
endif

# TinyUSB CFLAGS
CFLAGS += \
	-DCFG_TUSB_MCU=OPT_MCU_$(MCU_VARIANT_UPPER) \
	-DBOARD_TUD_MAX_SPEED=OPT_MODE_HIGH_SPEED \
	-DCFG_TUSB_OS=OPT_OS_NONE \
	-DCFG_TUD_CDC_TX_BUFSIZE=1024 \
	-DCFG_TUD_CDC_RX_BUFSIZE=1024 \
	-DCFG_TUD_MSC_BUFSIZE=4096 \
	-DCFG_TUD_MIDI_RX_BUFSIZE=128 \
	-DCFG_TUD_MIDI_TX_BUFSIZE=128 \
	-DCFG_TUD_VENDOR_RX_BUFSIZE=1024 \
	-DCFG_TUD_VENDOR_TX_BUFSIZE=1024

# Add TinyUSB sources
INC += -I../../lib/tinyusb/src
INC += -I../../supervisor/shared/usb
SRC_C += lib/tinyusb/src/portable/mentor/musb/dcd_musb.c

# Add port sources incl. any board functions
SRC_C += \
	boards/$(BOARD)/board.c \
	background.c \
	mphalport.c \

CFLAGS += $(INC) -Werror -Wall -std=gnu11 -nostartfiles $(BASE_CFLAGS) $(COPT)

# Suppress some errors for MSDK
# 	cast-align warning will be suppressed;
# 	it gets generated by CircuitPy's TLSF memory allocator lib
CFLAGS += -Wno-error=unused-parameter \
			-Wno-error=old-style-declaration \
			-Wno-error=sign-compare \
			-Wno-error=strict-prototypes \
			-Wno-error=cast-qual \
			-Wno-error=unused-variable \
			-Wno-error=lto-type-mismatch \
			-Wno-error=cast-align \
			-Wno-error=nested-externs \
			-Wno-error=sign-compare \
			-Wno-cast-align \
			-Wno-sign-compare \

ENTRY = Reset_Handler
LDFLAGS += $(CFLAGS) --entry $(ENTRY) -Wl,-nostdlib -Wl,-T,$(LINKERFILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections
LIBS := -lgcc -lc

# If not using CKTPY mathlib, use toolchain mathlib
ifndef INTERNAL_LIBM
LIBS += -lm
endif

# *******************************************************************************
### PORT-DEFINED BUILD RULES ###
# This section attempts to build the Python core, the supervisor, and any
# port-provided source code.
#
# QSTR sources are provided for the initial build step, which generates
# Python constants to represent C data which gets passed into the GC.

# OBJ includes
OBJ += $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_CIRCUITPY_COMMON) \
			$(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED) $(SRC_MOD)
# Sources that only hold QSTRs after pre-processing.
SRC_QSTR_PREPROCESSOR +=

# Default build target
all: $(BUILD)/firmware.elf $(BUILD)/firmware.hex $(BUILD)/firmware.bin

clean-all:
	rm -rf build-*

# Optional flash option when running within an installed MSDK to use OpenOCD
# Mainline OpenOCD does not yet have the MAX32's flash algorithm integrated.
# If the MSDK is installed, flash-msdk can be run to utilize the the modified
# openocd with the algorithms
MAXIM_PATH := $(subst \,/,$(MAXIM_PATH))
OPENOCD ?= $(MAXIM_PATH)/Tools/OpenOCD/openocd
OPENOCD_SCRIPTS ?= $(MAXIM_PATH)/Tools/OpenOCD/scripts
flash-msdk:
	$(OPENOCD) -s $(OPENOCD_SCRIPTS) \
		-f interface/cmsis-dap.cfg -f target/$(MCU_VARIANT_LOWER).cfg \
		-c "program $(BUILD)/firmware.elf verify; init; reset; exit"

# flash target using JLink
JLINK_DEVICE = $(MCU_VARIANT_LOWER)

JLINKEXE 		?= JLink.exe
JLINKEXE     	+= -if SWD -device ${JLINK_DEVICE} -speed 10000
COMMAND_FILE 	:= tools/flash_max32.jlink

flash-jlink: $(BUILD)/firmware.bin
	@$(JLINKEXE) -device $(MCU_VARIANT_UPPER) -NoGui 1 -CommandFile ${COMMAND_FILE}

$(BUILD)/firmware.elf: $(OBJ)
	$(STEPECHO) "LINK $@"
	$(Q)echo $^ > $(BUILD)/firmware.objs
	$(Q)$(CC) -o $@ $(LDFLAGS) @$(BUILD)/firmware.objs -Wl,--print-memory-usage -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@ | $(PYTHON) $(TOP)/tools/build_memory_info.py $(LINKERFILE) $(BUILD)

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O ihex $^ $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $^ $@

# *******************************************************************************
### CKTPY BUILD RULES ###
include $(TOP)/py/mkrules.mk
