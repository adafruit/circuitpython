cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS lib/zephyr)
project(circuitpython)

target_sources(app PRIVATE zephyr_main.c)

# Add I2C emulator control for native_sim testing
if(CONFIG_BOARD_NATIVE_SIM)
    target_sources(app PRIVATE native_sim_i2c_emul_control.c)
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
zephyr_get(BOARD_ALIAS SYSBUILD GLOBAL)

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
