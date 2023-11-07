# Create an INTERFACE library for our C module.
add_library(usermod_opus INTERFACE)

# Add our source files to the lib
target_sources(usermod_opus INTERFACE
    # ${CMAKE_CURRENT_LIST_DIR}/opusmodule.c
    ${CMAKE_CURRENT_LIST_DIR}/oggzmodule.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_opus INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

add_compile_definitions(INCLUDE_STDINT_H HAVE_CONFIG_H ARDUINO)

# opus
set(OPUS_FIXED_POINT ON)
set(OPUS_DISABLE_FLOAT_API ON)
set(OPUS_DISABLE_EXAMPLES ON)
set(OPUS_DISABLE_DOCS ON)

# opusfile
set(OP_DISABLE_HTTP ON)
set(OP_DISABLE_FLOAT_API ON)
set(OP_FIXED_POINT ON)
set(OP_DISABLE_EXAMPLES ON)
set(OP_DISABLE_DOCS ON)
set(OP_HAVE_LIBM OFF)

# set_property(TARGET ogg PROPERTY C_STANDARD 99)
# set_property(TARGET opus PROPERTY C_STANDARD 99)

# build with libopus
include(FetchContent)
FetchContent_Declare(arduino_libopus GIT_REPOSITORY "https://github.com/pschatzmann/arduino-libopus.git" GIT_TAG main )
FetchContent_GetProperties(arduino_libopus)
if(NOT arduino_libopus_POPULATED)
    FetchContent_Populate(arduino_libopus)
    add_subdirectory(${arduino_libopus_SOURCE_DIR})
endif()

# # the arduino-libopus distribution doesn't include opusfile
# include(FetchContent)
# FetchContent_Declare(opusfile GIT_REPOSITORY "https://github.com/dholth/opusfile.git" GIT_TAG micropython )
# FetchContent_GetProperties(opusfile)
# if(NOT opusfile)
#     FetchContent_Populate(opusfile)
#     add_subdirectory(${opusfile_SOURCE_DIR})
# endif()

# set_property(TARGET opusfile PROPERTY C_STANDARD 99)

# target_include_directories(opusfile PUBLIC $<BUILD_INTERFACE:${arduino_libopus_SOURCE_DIR}/src>)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_opus arduino_libopus)
