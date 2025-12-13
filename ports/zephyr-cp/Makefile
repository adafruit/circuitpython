ALL_BOARDS_IN_PORT := $($(wildcard boards/*/*):boards/=:/=)
# An incorrect BOARD might have been specified, so check against the list.
# There is deliberately no space after the :=
VALID_BOARD :=$(filter $(BOARD),$(ALL_BOARDS_IN_PORT))

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

TRANSLATION ?= en_US

.PHONY: $(BUILD)/zephyr-cp/zephyr/zephyr.elf flash debug clean menuconfig all clean-all test

$(BUILD)/zephyr-cp/zephyr/zephyr.elf:
	python cptools/pre_zephyr_build_prep.py $(BOARD)
	west build -b $(BOARD) -d $(BUILD) --sysbuild -- -DZEPHYR_BOARD_ALIASES=$(CURDIR)/boards/board_aliases.cmake -Dzephyr-cp_TRANSLATION=$(TRANSLATION)

$(BUILD)/firmware.elf: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	cp $^ $@

$(BUILD)/firmware.hex: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	cp $(BUILD)/zephyr-cp/zephyr/zephyr.hex $@

flash: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	west flash -d $(BUILD)

debug: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	west debug -d $(BUILD)

menuconfig:
	west build --sysbuild -d $(BUILD) -t menuconfig

clean:
	rm -rf $(BUILD)

# Build all boards. The + prefix allows jobserver file descriptors to be passed through.
# This enables parallel builds across all boards when invoked with `make -jN all`.
all:
	+python cptools/build_all_boards.py --continue-on-error

clean-all:
	rm -rf build build-*

test:
	pytest cptools/tests
