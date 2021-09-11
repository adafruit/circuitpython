# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Select the board to build for.
ifeq ($(BOARD),)
  $(error You must provide a BOARD parameter)
else
  ifeq ($(wildcard boards/$(BOARD)/.),)
    $(error Invalid BOARD specified)
  endif
endif

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

include ../../py/mkenv.mk

# Board-specific
include boards/$(BOARD)/mpconfigboard.mk

# Port-specific
include mpconfigport.mk

ifndef USB_NUM_ENDPOINT_PAIRS
$(error "USB_NUM_ENDPOINT_PAIRS (number of USB endpoint pairs)must be defined")
endif

# CircuitPython-specific
include $(TOP)/py/circuitpy_mpconfig.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

include $(TOP)/supervisor/supervisor.mk

# Include make rules and variables common across CircuitPython builds.
include $(TOP)/py/circuitpy_defns.mk

CROSS_COMPILE = arm-none-eabi-

CHIP_FAMILY_LOWER = $(shell echo $(CHIP_FAMILY) | tr '[:upper:]' '[:lower:]')
CHIP_VARIANT_LOWER = $(shell echo $(CHIP_VARIANT) | tr '[:upper:]' '[:lower:]')
BOARD_LOWER = $(shell echo $(BOARD) | tr '[:upper:]' '[:lower:]')

INC += -I. \
       -I../.. \
       -I../lib/mp-readline \
       -I../lib/timeutils \
       -Iboards/$(BOARD) \
       -Iboards/

ifeq ($(CHIP_FAMILY_LOWER), lpc1700)
INC += -isystem cmsis5/CMSIS/Core/Include \
       -isystem cmsis5/CMSIS/Driver/Include \
       -isystem nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver \
       -isystem boards/$(BOARD)/CMSIS/Driver/Config \
       -I lpc17xx \
       -I $(BUILD)
else
  ifeq ($(CHIP_FAMILY_LOWER), lpc55s28)
  INC += -isystem cmsis5/CMSIS/Core/Include \
         -isystem cmsis5/CMSIS/Driver/Include \
         -I mcux-sdk/devices/LPC55S28 \
         -I lpc55xx \
         -I $(BUILD)
  endif
endif


CFLAGS += -mabi=aapcs-linux


ifeq ($(CHIP_FAMILY_LOWER), lpc1700)
# lpc1700 specific configuration
CFLAGS += -mthumb \
          -mcpu=cortex-m3 \
          -msoft-float \
          -mfloat-abi=soft
else
  ifeq ($(CHIP_FAMILY_LOWER), lpc55s28)
  # lpc55s28 specific configuration
  CFLAGS += -mthumb \
            -mcpu=cortex-m33 \
            -mfpu=fpv5-sp-d16 \
            -mfloat-abi=hard
  endif
endif

ifeq ($(CHIP_VARIANT_LOWER), lpc1768)
# LPC1768 specific configuration
INC += -isystem lpc17xx/Keil/$(CHIP_FAMILY)_DFP/2.6.0/Device/Include
INC += -isystem nxp_lpcopen/lpc175x_6x/lpc_chip_175x_6x/inc

CFLAGS += -DCORE_M3=1

# -Wno-error=attributes otherwise USB driver generates errors
# -Wno-error=maybe-uninitialized otherwise UART driver generates errors
# -Wno-error=int-conversion otherwise UART driver generates errors
# -Wno-error=implicit-fallthrough otherwise I2C driver generates errors
CFLAGS += -DLPC175x_6x=1 \
          -DRTE_SSP2=0 \
          -DRTE_UART4=0 \
          -D__USE_CMSIS=1 \
          -Wno-error=attributes \
          -Wno-error=maybe-uninitialized \
          -Wno-error=int-conversion \
          -Wno-error=implicit-fallthrough

SRC_C += lpc17xx/cr_startup_lpc175x_6x.c
SRC_C += nxp_lpcopen/lpc175x_6x/lpc_chip_175x_6x/src/ritimer_17xx_40xx.c \
         nxp_lpcopen/lpc175x_6x/lpc_chip_175x_6x/src/clock_17xx_40xx.c \
         supervisor/port_lpc17xx.c

else
  ifeq ($(CHIP_VARIANT_LOWER), lpc55s28)
  CFLAGS += -DLPC55Sxx=1 \
            -D__USE_CMSIS=1

  SRC_C += lpc55xx/startup_lpc55s28.c
  SRC_C += mcux-sdk/devices/LPC55S28/system_LPC55S28.c

  SRC_C += supervisor/port_lpc55sxx.c

  endif
endif

ifeq ($(BOARD_LOWER), mcb1700)
# MCB1700 specific configuration
# CFLAGS += -DPICO_ON_DEVICE=1 -DPICO_NO_BINARY_INFO=0 -DPICO_TIME_DEFAULT_ALARM_POOL_DISABLED=1 -DPICO_DIVIDER_CALL_IDIV0=0 -DPICO_DIVIDER_CALL_LDIV0=0 -DPICO_DIVIDER_HARDWARE=1 -DPICO_DOUBLE_ROM=1 -DPICO_FLOAT_ROM=1 -DPICO_MULTICORE=1 -DPICO_BITS_IN_RAM=0 -DPICO_DIVIDER_IN_RAM=0 -DPICO_DOUBLE_PROPAGATE_NANS=0 -DPICO_DOUBLE_IN_RAM=0 -DPICO_MEM_IN_RAM=0 -DPICO_FLOAT_IN_RAM=0 -DPICO_FLOAT_PROPAGATE_NANS=1 -DPICO_NO_FLASH=0 -DPICO_COPY_TO_RAM=0 -DPICO_DISABLE_SHARED_IRQ_HANDLERS=0
CFLAGS += -DBOARD_MCB1700=1
else
  ifeq ($(BOARD_LOWER), lpcxpresso55s28)
  CFLAGS += -DBOARD_LPCEXPRESSO55S28=1
  endif
endif

OPTIMIZATION_FLAGS ?= -Os -ffreestanding -pipe

# CIRCUITPY_USB_VENDOR, CIRCUITPY_USB_HID, CIRCUITPY_USB_MIDI
ifeq ($(CIRCUITPY_USB_CDC), 1)
# FIXME: Requires nxp_lpcopn drivers

INC += -I../../lib/tinyusb/src \
       -I../../supervisor/shared/usb

ifeq ($(CHIP_VARIANT_LOWER), lpc1768)
INC += -Inxp_lpcopen/lpc175x_6x/lpc_chip_175x_6x/inc/../
else
ifeq ($(CHIP_VARIANT_LOWER), lpc55s28)
# INC += -I
endif
endif

CFLAGS +=-DCORE_M3=1 \
         -DRTC_EV_SUPPORT=0

# TinyUSB specific configuration
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_LPC175X_6X \
          -DCFG_TUD_MIDI_RX_BUFSIZE=128 \
          -DCFG_TUD_CDC_RX_BUFSIZE=256 \
          -DCFG_TUD_MIDI_TX_BUFSIZE=128 \
          -DCFG_TUD_CDC_TX_BUFSIZE=256 \
          -DCFG_TUD_MSC_BUFSIZE=1024
endif

# option to override default optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -ggdb3 -Og
  # No LTO because we may place some functions in RAM instead of flash.
else
  CFLAGS += -DNDEBUG

  # No LTO because we may place some functions in RAM instead of flash.

  ifdef CFLAGS_BOARD
    CFLAGS += $(CFLAGS_BOARD)
  endif
endif

# Remove -Wno-stringop-overflow after we can test with CI's GCC 10. Mac's looks weird.
DISABLE_WARNINGS = -Wno-stringop-overflow -Wno-unused-function -Wno-unused-variable -Wno-strict-overflow -Wno-cast-align -Wno-strict-prototypes -Wno-nested-externs -Wno-double-promotion -Wno-sign-compare

CFLAGS += $(INC) -Wall -Werror -std=gnu11 -nostdlib -fshort-enums $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT) $(DISABLE_WARNINGS)

# LDFLAGS = $(CFLAGS) -nostartfiles -Wl,-nostdlib -Wl,-T,$(GENERATED_LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs
LDFLAGS = $(CFLAGS) \
          -Wl,-T,$(GENERATED_LD_FILE) -Wl,-Map=$@.map -Wl,-cref -specs=nano.specs -specs=nosys.specs

LIBS := -lgcc -lc

# LDFLAGS += -nostartfiles
LDFLAGS += -Wl,-nostdlib
# LDFLAGS += -Wl,-gc-sections

ifeq ($(CHIP_FAMILY), lpc1700)
# PICO_LDFLAGS = --specs=nosys.specs -Wl,--wrap=__aeabi_ldiv0 -Wl,--wrap=__aeabi_idiv0 -Wl,--wrap=__aeabi_lmul -Wl,--wrap=__clzsi2 -Wl,--wrap=__clzdi2 -Wl,--wrap=__ctzsi2 -Wl,--wrap=__ctzdi2 -Wl,--wrap=__popcountsi2 -Wl,--wrap=__popcountdi2 -Wl,--wrap=__clz -Wl,--wrap=__clzl -Wl,--wrap=__clzll -Wl,--wrap=__aeabi_idiv -Wl,--wrap=__aeabi_idivmod -Wl,--wrap=__aeabi_ldivmod -Wl,--wrap=__aeabi_uidiv -Wl,--wrap=__aeabi_uidivmod -Wl,--wrap=__aeabi_uldivmod -Wl,--wrap=__aeabi_dadd -Wl,--wrap=__aeabi_ddiv -Wl,--wrap=__aeabi_dmul -Wl,--wrap=__aeabi_drsub -Wl,--wrap=__aeabi_dsub -Wl,--wrap=__aeabi_cdcmpeq -Wl,--wrap=__aeabi_cdrcmple -Wl,--wrap=__aeabi_cdcmple -Wl,--wrap=__aeabi_dcmpeq -Wl,--wrap=__aeabi_dcmplt -Wl,--wrap=__aeabi_dcmple -Wl,--wrap=__aeabi_dcmpge -Wl,--wrap=__aeabi_dcmpgt -Wl,--wrap=__aeabi_dcmpun -Wl,--wrap=__aeabi_i2d -Wl,--wrap=__aeabi_l2d -Wl,--wrap=__aeabi_ui2d -Wl,--wrap=__aeabi_ul2d -Wl,--wrap=__aeabi_d2iz -Wl,--wrap=__aeabi_d2lz -Wl,--wrap=__aeabi_d2uiz -Wl,--wrap=__aeabi_d2ulz -Wl,--wrap=__aeabi_d2f -Wl,--wrap=sqrt -Wl,--wrap=cos -Wl,--wrap=sin -Wl,--wrap=tan -Wl,--wrap=atan2 -Wl,--wrap=exp -Wl,--wrap=log -Wl,--wrap=ldexp -Wl,--wrap=copysign -Wl,--wrap=trunc -Wl,--wrap=floor -Wl,--wrap=ceil -Wl,--wrap=round -Wl,--wrap=sincos -Wl,--wrap=asin -Wl,--wrap=acos -Wl,--wrap=atan -Wl,--wrap=sinh -Wl,--wrap=cosh -Wl,--wrap=tanh -Wl,--wrap=asinh -Wl,--wrap=acosh -Wl,--wrap=atanh -Wl,--wrap=exp2 -Wl,--wrap=log2 -Wl,--wrap=exp10 -Wl,--wrap=log10 -Wl,--wrap=pow -Wl,--wrap=powint -Wl,--wrap=hypot -Wl,--wrap=cbrt -Wl,--wrap=fmod -Wl,--wrap=drem -Wl,--wrap=remainder -Wl,--wrap=remquo -Wl,--wrap=expm1 -Wl,--wrap=log1p -Wl,--wrap=fma -Wl,--wrap=__aeabi_fadd -Wl,--wrap=__aeabi_fdiv -Wl,--wrap=__aeabi_fmul -Wl,--wrap=__aeabi_frsub -Wl,--wrap=__aeabi_fsub -Wl,--wrap=__aeabi_cfcmpeq -Wl,--wrap=__aeabi_cfrcmple -Wl,--wrap=__aeabi_cfcmple -Wl,--wrap=__aeabi_fcmpeq -Wl,--wrap=__aeabi_fcmplt -Wl,--wrap=__aeabi_fcmple -Wl,--wrap=__aeabi_fcmpge -Wl,--wrap=__aeabi_fcmpgt -Wl,--wrap=__aeabi_fcmpun -Wl,--wrap=__aeabi_i2f -Wl,--wrap=__aeabi_l2f -Wl,--wrap=__aeabi_ui2f -Wl,--wrap=__aeabi_ul2f -Wl,--wrap=__aeabi_f2iz -Wl,--wrap=__aeabi_f2lz -Wl,--wrap=__aeabi_f2uiz -Wl,--wrap=__aeabi_f2ulz -Wl,--wrap=__aeabi_f2d -Wl,--wrap=sqrtf -Wl,--wrap=cosf -Wl,--wrap=sinf -Wl,--wrap=tanf -Wl,--wrap=atan2f -Wl,--wrap=expf -Wl,--wrap=logf -Wl,--wrap=ldexpf -Wl,--wrap=copysignf -Wl,--wrap=truncf -Wl,--wrap=floorf -Wl,--wrap=ceilf -Wl,--wrap=roundf -Wl,--wrap=sincosf -Wl,--wrap=asinf -Wl,--wrap=acosf -Wl,--wrap=atanf -Wl,--wrap=sinhf -Wl,--wrap=coshf -Wl,--wrap=tanhf -Wl,--wrap=asinhf -Wl,--wrap=acoshf -Wl,--wrap=atanhf -Wl,--wrap=exp2f -Wl,--wrap=log2f -Wl,--wrap=exp10f -Wl,--wrap=log10f -Wl,--wrap=powf -Wl,--wrap=powintf -Wl,--wrap=hypotf -Wl,--wrap=cbrtf -Wl,--wrap=fmodf -Wl,--wrap=dremf -Wl,--wrap=remainderf -Wl,--wrap=remquof -Wl,--wrap=expm1f -Wl,--wrap=log1pf -Wl,--wrap=fmaf -Wl,--wrap=memcpy -Wl,--wrap=memset -Wl,--wrap=__aeabi_memcpy -Wl,--wrap=__aeabi_memset -Wl,--wrap=__aeabi_memcpy4 -Wl,--wrap=__aeabi_memset4 -Wl,--wrap=__aeabi_memcpy8 -Wl,--wrap=__aeabi_memset8
  LDFLAGS += -mthumb \
             -mcpu=cortex-m3
else
  ifeq ($(CHIP_FAMILY_LOWER), lpc55s28)
    LDFLAGS += -mthumb \
               -mcpu=cortex-m33
  endif
endif


# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif


ifeq ($(CHIP_FAMILY_LOWER), lpc1700)
SRC_SDK := \
	lpc17xx/Keil/$(CHIP_FAMILY)_DFP/2.6.0/Device/Source/system_LPC17xx.c \
	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/CAN_LPC17xx.c \
	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/GPDMA_LPC17xx.c \
	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/GPIO_LPC17xx.c \
	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/I2C_LPC17xx.c \
	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/PIN_LPC17xx.c \
	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/SPI_LPC17xx.c \
	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/SSP_LPC17xx.c \
	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/OTG_LPC17xx.c \
	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/USBD_LPC17xx.c

# DEBUG_UART_TX, DEBUG_UART_RX, CIRCUITPY_USB_CDC
ifeq ($(CIRCUITPY_BUSIO), 1)
  CFLAGS += -DRTE_UART1_RI_PIN=0 \
            -DRTE_UART1_DTR_PIN=0 \
            -DRTE_UART1_DSR_PIN=0 \
            -DRTE_UART1_DCD_PIN=0 \
            -DRTE_UART1_RTS_PIN=0 \
            -DRTE_UART1_CTS_PIN_EN=0

  SRC_C += nxp_lpc/LPC1700/CMSIS/Driver/UART_LPC17xx.c
endif

# Requires cmsis_os.h
#	nxp_lpc/$(CHIP_FAMILY)/CMSIS/Driver/EMAC_LPC17xx.c \

else
  ifeq ($(CHIP_FAMILY_LOWER), lpc55s28)
#    SRC_SDK := $(addprefix mcu-dk/, $(SRC_SDK))
  endif

endif

SRC_C += \
	mphalport.c \
	fatfs_port.c \
	background.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	peripherals/$(CHIP_FAMILY_LOWER)/pins.c

SRC_C += \
	diag/Trace.c \
	diag/trace_impl.c \
	diag/printf.c \
	diag/_putchar.c

ifeq ($(CIRCUITPY_USB_CDC), 1)
  ifeq ($(CHIP_FAMILY_LOWER), lpc1700)
    SRC_C += \
	    lib/tinyusb/src/portable/nxp/transdimension/dcd_transdimension.c \
	    lib/tinyusb/src/portable/nxp/lpc17_40/dcd_lpc17_40.c
  else ifeq ($(CHIP_FAMILY_LOWER), lpc55s28)
    SRC_C += \
	    lib/tinyusb/src/portable/nxp/transdimension/dcd_transdimension.c \
	    lib/tinyusb/src/portable/nxp/lpc55/dcd_lpc55.c
  endif
endif

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

# There may be duplicates between SRC_COMMON_HAL_EXPANDED and SRC_SHARED_MODULE_EXPANDED,
# because a few modules have files both in common-hal/ and shared-modules/.
# Doing a $(sort ...) removes duplicates as part of sorting.
SRC_COMMON_HAL_SHARED_MODULE_EXPANDED = $(sort $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED))

SRC_S = supervisor/$(CHIP_FAMILY_LOWER)_cpu.s

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SDK:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))


SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)

all: $(BUILD)/firmware.uf2

$(BUILD)/firmware.elf: $(OBJ) $(GENERATED_LD_FILE)
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $(OBJ) -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@ | $(PYTHON3) $(TOP)/tools/build_memory_info.py $(GENERATED_LD_FILE)

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.bin
	$(STEPECHO) "Create $@"
	$(Q)$(PYTHON3) $(TOP)/tools/uf2/utils/uf2conv.py -f 0xe48bff56 -b 0x10000000 -c -o $@ $^

include $(TOP)/py/mkrules.mk

# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo $* = $($*)
