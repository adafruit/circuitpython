# Resolve board aliases before sysbuild runs, so BOARD is the canonical Zephyr
# board id when we derive board-specific file names from it below.
include(${CMAKE_CURRENT_LIST_DIR}/../boards/board_aliases.cmake)

# Partitioning and bootloader policy are owned by the mcuboot fork
# (bootloader/mcuboot, see its dts/README.md): as a sysbuild module it applies
# dts/<vendor>/<board>.dtsi to both the application and bootloader images and
# defaults SB_CONFIG_BOOTLOADER_* per board. What is left here is CircuitPython's
# own bootloader UI configuration.
set(cp_boards_dir ${CMAKE_CURRENT_LIST_DIR}/../boards)
string(REGEX REPLACE "[@/].*" "" board_name "${BOARD}")

# Common MCUboot conf (UF2, double-tap, etc.) plus optional board fragment.
# Unused unless a mcuboot image is built for this board.
set(mcuboot_confs ${CMAKE_CURRENT_LIST_DIR}/mcuboot.conf)
if(EXISTS ${cp_boards_dir}/mcuboot_${board_name}.conf)
  list(APPEND mcuboot_confs ${cp_boards_dir}/mcuboot_${board_name}.conf)
endif()
set(mcuboot_EXTRA_CONF_FILE "${mcuboot_confs}"
    CACHE INTERNAL "MCUboot conf fragments"
)

# Escape hatch for board defaults we disagree with, e.g. forcing
# SB_CONFIG_BOOTLOADER_NONE=y on a board whose own Kconfig.sysbuild turns MCUboot
# on. Board Kconfig is parsed before module Kconfig, so the fork's defaults
# cannot express this.
if(EXISTS ${cp_boards_dir}/sysbuild_${board_name}.conf)
  set(SB_EXTRA_CONF_FILE ${cp_boards_dir}/sysbuild_${board_name}.conf
      CACHE INTERNAL "Board sysbuild override"
  )
endif()

find_package(Sysbuild REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sysbuild LANGUAGES)
