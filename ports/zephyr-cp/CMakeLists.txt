cmake_minimum_required(VERSION 3.20.0)

# The dynamic peripheral allocation / runtime pin routing code lives in a
# Zephyr module kept in-tree for now (modules/iobroker, see its README.md).
# It must be set before find_package(Zephyr). Repoint this at the module's new

# home to consume an externalized copy.
set(ZEPHYR_EXTRA_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/internal-modules/iobroker
    # NeoPixel (WS2812-style) transmit on a runtime-chosen pin (see
    # internal-modules/neopixel/README.md).
    ${CMAKE_CURRENT_SOURCE_DIR}/internal-modules/neopixel
    # Emulated PWM controller for native_sim tests (see
    # internal-modules/pwm_emul/src/pwm_emul.c).
    ${CMAKE_CURRENT_SOURCE_DIR}/internal-modules/pwm_emul
    # Kconfig-only module with defaults that must merge before Zephyr's
    # subsystem Kconfig files (see internal-modules/circuitpython-kconfig/Kconfig).
    ${CMAKE_CURRENT_SOURCE_DIR}/internal-modules/circuitpython-kconfig
    # The same LVGL copy the Espressif port builds (lib/lvgl), instead of the one
    # west checks out for Zephyr. It has the same module name, which zephyr_module.py
    # resolves in favor of extra modules, so both ports build the same LVGL.
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/lvgl
)

find_package(Zephyr REQUIRED HINTS ${ZEPHYR_BASE} $ENV{ZEPHYR_BASE} lib/zephyr)
project(circuitpython)

include(ExternalProject)

target_sources(app PRIVATE zephyr_main.c)

# The i.MX RT10xx FlexRAM is split into OCRAM, DTCM and ITCM (64/32/32 KB on the
# RT1011), and only OCRAM is zephyr,sram. Put the kernel's no-init stacks (the
# main thread's, where the VM runs, plus the interrupt and idle stacks) in DTCM,
# as ports/mimxrt10xx does, so OCRAM is left for the heap.
if(CONFIG_SOC_SERIES_IMXRT10XX)
  zephyr_code_relocate(FILES ${ZEPHYR_BASE}/kernel/init.c LOCATION DTCM_NOINIT)
endif()

# Add native_sim test support helpers.
if(CONFIG_BOARD_NATIVE_SIM)
    target_sources(app PRIVATE native_sim_i2c_emul_control.c native_sim_spi_loopback_emul.c)
endif()

if(CONFIG_TRACING_PERFETTO)
  zephyr_include_directories(${ZEPHYR_BINARY_DIR}/subsys/tracing/perfetto/proto)
endif()

# From: https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/application_development/external_lib/CMakeLists.txt
# The external static library that we are linking with does not know
# how to build for this platform so we export all the flags used in
# this zephyr build to the external build system.
#
# Other external build systems may be self-contained enough that they
# do not need any build information from zephyr. Or they may be
# incompatible with certain zephyr options and need them to be
# filtered out.
zephyr_get_include_directories_for_lang_as_string(       C includes)
zephyr_get_system_include_directories_for_lang_as_string(C system_includes)
zephyr_get_compile_definitions_for_lang_as_string(       C definitions)
zephyr_get_compile_options_for_lang_as_string(           C options)

if(DEFINED CMAKE_C_COMPILER_TARGET)
  set(target_flag "--target=${CMAKE_C_COMPILER_TARGET}")
endif()

set(external_project_cflags
  "${target_flag} ${includes} ${definitions} ${options} ${system_includes}"
  )

zephyr_get(TRANSLATION SYSBUILD GLOBAL)
zephyr_get(CONFIG_LTO SYSBUILD GLOBAL)
zephyr_get(CONFIG_LVGL SYSBUILD GLOBAL)
zephyr_get(BOARD_ALIAS SYSBUILD GLOBAL)

# displayio hands indexed bitmaps to LVGL as I1/I2/I4/I8 images. Decode them fully
# (instead of line by line) so they can be scaled and cached. LVGL's lv_conf_internal.h
# doesn't map a Kconfig option for this, so define it directly.
if(CONFIG_LVGL)
    zephyr_compile_definitions(LV_BIN_DECODER_RAM_LOAD=1)
endif()

# LVGL module (used by the displayio LVGL compositing core) passes its include paths to the
# CircuitPython static library build explicitly, since the core build does not consume CMake
# target information.
set(LVGL_MODULE_DIR "")
if(CONFIG_LVGL AND DEFINED ZEPHYR_LVGL_MODULE_DIR)
    set(LVGL_MODULE_DIR ${ZEPHYR_LVGL_MODULE_DIR})
endif()

ExternalProject_Add(circuitpython
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/cptools/build_circuitpython.py
    CC=${CMAKE_C_COMPILER}
    AR=${CMAKE_AR}
    CFLAGS=${external_project_cflags}
    BOARD=${BOARD}
    BOARD_ALIAS=${BOARD_ALIAS}
    BOARD_REVISION=${BOARD_REVISION}
    BOARD_QUALIFIERS=${BOARD_QUALIFIERS}
    SOC_DIRECTORIES=${SOC_DIRECTORIES}
    OUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/libcircuitpython.a
    PORT_SRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
    TRANSLATION=${TRANSLATION}
    LTO=${CONFIG_LTO}
    LVGL=${CONFIG_LVGL}
    LVGL_DIR=${LVGL_MODULE_DIR}
    BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/libcircuitpython.a
    BUILD_JOB_SERVER_AWARE TRUE
    BUILD_ALWAYS TRUE
    DEPENDS zephyr
    INSTALL_COMMAND ""
    )

add_library(circuitpython_wrapper STATIC IMPORTED GLOBAL)
add_dependencies(
  circuitpython_wrapper
  circuitpython
  )
set_target_properties(circuitpython_wrapper PROPERTIES IMPORTED_LOCATION             ${CMAKE_CURRENT_BINARY_DIR}/libcircuitpython.a)
target_link_libraries(circuitpython_wrapper INTERFACE kernel)
target_link_libraries(app PRIVATE circuitpython_wrapper)
