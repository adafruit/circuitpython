# This file is part of Adafruit for EFR32 project
#
# The MIT License (MIT)
#
# Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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
#


.SUFFIXES:				# ignore builtin rules
.PHONY: all clean slc-clean slc-generate finalbuild dependents

# Select the board to build for.
ifeq ($(BOARD),)
$(error You must provide a BOARD parameter)
else
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif
endif

# Values set by the initial generation
PROJECTNAME = circuitpython_efr32

# If the build directory is not given, make it reflect the board name.
SILABS_BUILD = build-$(BOARD)
# Build dir for CircuitPython
BUILD ?= $(SILABS_BUILD)
# Override Build Directories
OUTPUT_DIR = $(SILABS_BUILD)
# Python script to generate pins and pins functionalities code
PY_GEN_PINS_SRC ?= tools/make_pins.py

FLASH_TOML_DIR = ../../data/nvm.toml/flash/macronix/

export PATH := $(realpath $(CURDIR))/tools/build-tools/slc_cli:$(PATH)
export PATH := $(shell python3 -c "import jinja2; print(jinja2.__path__[0])"):$(PATH)
export PATH := $(realpath $(CURDIR))/tools/build-tools/slc_cli/bin/slc-cli/developer/adapter_packs/python/lib/python3.6/site-packages/jinja2:$(PATH)

# Default goal
all: $(OUTPUT_DIR)/firmware.bin

$(OUTPUT_DIR)/firmware.bin:
	-@java --version
	-@pip show jinja2
	-@echo $(PATH)
ifeq (,$(wildcard  $(SILABS_BUILD)/$(PROJECTNAME).Makefile))
	+@$(MAKE) --no-print-directory slc-generate
	-@echo 'Project is generated. Run make again'
	+@$(MAKE) --no-print-directory finalbuild_$(BOARD)
else
	+@$(MAKE) --no-print-directory finalbuild_$(BOARD)
endif

# Include sub-makefiles
-include $(SILABS_BUILD)/$(PROJECTNAME).project.mak

CFLAGS = $(INCLUDES) $(C_DEFS) $(C_FLAGS) \
	-Wno-expansion-to-defined \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-Wno-type-limits

ASMFLAGS = $(INCLUDES) $(ASM_DEFS) $(ASM_FLAGS) $(DEPFLAGS)

# Env-specific
include ../../py/mkenv.mk
# Board-specific
include boards/$(BOARD)/mpconfigboard.mk
# Port-specific
include mpconfigport.mk

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

MCU_SERIES_LOWER = $(shell echo $(MCU_SERIES) | tr '[:upper:]' '[:lower:]')
MCU_VARIANT_LOWER = $(shell echo $(MCU_VARIANT) | tr '[:upper:]' '[:lower:]')

# Header files folders include
INC += -I.
INC += -I../..
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr
INC += -I$(SILABS_BUILD)/autogen
INC += -I$(SILABS_BUILD)/config
INC += -I./boards
INC += -I./peripherals
INC += -I../../lib/mp-readline
#D INC += -I../../supervisor/shared/usb

#Debugging/Optimization
ifeq ($(DEBUG), 1)
	CFLAGS += -g3
	# You may want to enable these flags to make setting breakpoints easier.
	CFLAGS += -fno-inline -fno-ipa-sra -Og
else
	CFLAGS += -DNDEBUG
	OPTIMIZATION_FLAGS ?= -Os -fno-inline-functions
	CFLAGS += -g
endif

# to override compiler optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)
CFLAGS += $(INC) $(BASE_CFLAGS) $(C_DEFS) $(CFLAGS_MOD) $(COPT)
CFLAGS += -DEFR32_SERIES_LOWER='"$(MCU_VARIANT)"'
CFLAGS += -Wno-undef -Wno-shadow -Wno-cast-align -Wno-nested-externs -Wno-strict-prototypes


SRC_C += \
	background.c \
	mphalport.c \
	$(SILABS_BUILD)/pins.c\

ifeq ('$(BOARD)','brd2601b')
SRC_C += boards/$(BOARD)/sensor.c
endif

ifneq ($(CIRCUITPY_USB),0)
SRC_C += lib/tinyusb/src/portable/st/synopsys/dcd_synopsys.c
endif

SRC_S = boards/mp_efr32xg24_gchelper.s

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
						  $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
						  $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
							 $(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
							 $(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

# There may be duplicates between SRC_COMMON_HAL_EXPANDED and SRC_SHARED_MODULE_EXPANDED,
# because a few modules have files both in common-hal/ and shared-module/.
# Doing a $(sort ...) removes duplicates as part of sorting.
SRC_COMMON_HAL_SHARED_MODULE_EXPANDED = $(sort $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED))

ifneq ($(FROZEN_MPY_DIR),)
FROZEN_MPY_PY_FILES := $(shell find -L $(FROZEN_MPY_DIR) -type f -name '*.py')
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))
endif

OBJ += $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_MOD) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)
# Sources that only hold QSTRs after pre-processing.
SRC_QSTR_PREPROCESSOR +=

MCU_SECTIONS = $^ $@

finalbuild_devkit_xg24_brd2601b: $(FLASH_TOML_DIR)/MX25R3235F.toml $(SILABS_BUILD)/pin_functions.h $(SILABS_BUILD)/pins.c $(OUTPUT_DIR)/firmware.out

finalbuild_explorerkit_xg24_brd2703a: $(SILABS_BUILD)/pin_functions.h $(SILABS_BUILD)/pins.c $(OUTPUT_DIR)/firmware.out

finalbuild_sparkfun_thingplus_matter_mgm240p_brd2704a: $(SILABS_BUILD)/pin_functions.h $(SILABS_BUILD)/pins.c $(OUTPUT_DIR)/firmware.out

$(OUTPUT_DIR)/firmware.out: $(OBJ) $(OBJS) $(LIB_FILES)
	@echo 'Linking $(OUTPUT_DIR)/firmware.out'
	@echo "$(OBJS) $(OBJ)" > $(OUTPUT_DIR)/linker_objs
	$(CC) $(LD_FLAGS) @$(OUTPUT_DIR)/linker_objs $(LIBS) -o $(OUTPUT_DIR)/firmware.out
	$(OBJCOPY) $(OUTPUT_DIR)/firmware.out -O binary $(OUTPUT_DIR)/firmware.bin
	$(OBJCOPY) $(OUTPUT_DIR)/firmware.out -O ihex $(OUTPUT_DIR)/firmware.hex
	$(OBJCOPY) $(OUTPUT_DIR)/firmware.out -O srec $(OUTPUT_DIR)/firmware.s37
	@echo 'Done.'

slc-generate:
	+@$(MAKE) -C tools all
	@echo 'SLC generates project'
	@slc configuration --sdk gecko_sdk
	@slc signature trust -extpath cp_efr32_extension
	@slc signature trust --sdk gecko_sdk
	@slc generate -name=$(PROJECTNAME) $(PROJECTNAME).slcp --sdk gecko_sdk --with $(BOARD_BRD) -tlcn gcc -d=$(SILABS_BUILD)
	@sed -i 's/ autogen\// $(SILABS_BUILD)\/autogen\//g' $(SILABS_BUILD)/circuitpython_efr32.project.mak
	@sed -i 's/-T"autogen\//-T"$(SILABS_BUILD)\/autogen\//g' $(SILABS_BUILD)/circuitpython_efr32.project.mak
slc-clean:
	@echo 'SLC cleaning'
	+@$(MAKE) clean
	$(RM) -fr $(SILABS_BUILD)
	$(RM) -fr ./tools/build-tools

$(SILABS_BUILD)/pin_functions.h:
	$(STEPECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_GEN_PINS_SRC) -e $@ boards/$(BOARD)/pins.csv boards/$(BOARD)/pin_functions.csv
	@-rm pins.c

$(SILABS_BUILD)/pins.c:
	$(STEPECHO) "GEN $@"
	$(Q)$(PYTHON) $(PY_GEN_PINS_SRC) -s $@ boards/$(BOARD)/pins.csv boards/$(BOARD)/pin_functions.csv
	@-rm pin_functions.h

$(FLASH_TOML_DIR)/MX25R3235F.toml:
	cp -f boards/$(BOARD)/MX25R3235F.toml  $(FLASH_TOML_DIR)/MX25R3235F.toml

#Override ECHO
$(OBJS): ECHO =
$(OBJS):

$(OUTPUT_DIR)/%.o: %.c
	@echo 'Building $<'
	@mkdir -p $(@D)
	$(ECHO)$(CC) $(CFLAGS) -c -o $@ $<

$(OUTPUT_DIR)/%.o: %.cpp
	@echo 'Building $<'
	@mkdir -p $(@D)
	$(ECHO)$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUTPUT_DIR)/%.o: %.cc
	@echo 'Building $<'
	@mkdir -p $(@D)
	$(ECHO)$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUTPUT_DIR)/%.o: %.s
	@echo 'Building $<'
	@mkdir -p $(@D)
	$(ECHO)$(CC) $(ASMFLAGS) -c -o $@ $<

$(OUTPUT_DIR)/%.o: %.S
	@echo 'Building $<'
	@mkdir -p $(@D)
	$(ECHO)$(CC) $(ASMFLAGS) -c -o $@ $<

include $(TOP)/py/mkrules.mk

# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo $* = $($*)
