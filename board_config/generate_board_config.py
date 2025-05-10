#!/usr/bin/env python3
"""
CircuitPython Board Configuration Generator

Converts board_setting.toml files into:
- mpconfigboard.h
- mpconfigboard.mk
- pins.c
- board.c
"""

import toml
import os
from pathlib import Path


def generate_config_files(board_dir):
    """Generate all configuration files from board_setting.toml"""
    toml_path = os.path.join(board_dir, "board_setting.toml")
    if not os.path.exists(toml_path):
        raise FileNotFoundError(f"No board_setting.toml found in {board_dir}")

    config = toml.load(toml_path)

    # Generate mpconfigboard.h
    with open(os.path.join(board_dir, "mpconfigboard.h"), "w") as f:
        f.write(f"""#pragma once

#define MICROPY_HW_BOARD_NAME "{config["metadata"]["name"]}"
#define MICROPY_HW_MCU_NAME "{config["metadata"]["mcu"]}"

// Neopixel configuration
#define MICROPY_HW_NEOPIXEL_ORDER_GRB (1)
#define MICROPY_HW_NEOPIXEL (&pin_GPIO{config["pins"]["neopixel"]["number"]})

// Default UART pins
#define DEFAULT_UART_BUS_RX (&pin_GPIO{config["default_interfaces"]["uart_rx"]})
#define DEFAULT_UART_BUS_TX (&pin_GPIO{config["default_interfaces"]["uart_tx"]})
""")

    # Generate mpconfigboard.mk
    with open(os.path.join(board_dir, "mpconfigboard.mk"), "w") as f:
        f.write(f"""USB_VID = {config["usb"]["vid"]}
USB_PID = {config["usb"]["pid"]}
USB_PRODUCT = "{config["usb"]["product"]}"
USB_MANUFACTURER = "{config["usb"]["manufacturer"]}"

IDF_TARGET = {config["build"]["idf_target"]}

CIRCUITPY_ESP_FLASH_MODE = {config["build"]["flash_mode"]}
CIRCUITPY_ESP_FLASH_FREQ = {config["build"]["flash_freq"]}
CIRCUITPY_ESP_FLASH_SIZE = {config["build"]["flash_size"]}

CIRCUITPY_ESP_PSRAM_SIZE = {config["build"]["psram_size"]}
CIRCUITPY_ESP_PSRAM_MODE = {config["build"]["psram_mode"]}
CIRCUITPY_ESP_PSRAM_FREQ = {config["build"]["psram_freq"]}

# Feature flags
CIRCUITPY_ESPCAMERA = {int(config["features"]["circuitpy_esp_camera"])}
CIRCUITPY_BITMAPFILTER = {int(config["features"]["circuitpy_bitmapfilter"])}
CIRCUITPY_CODEOP = {int(config["features"]["circuitpy_codeop"])}
CIRCUITPY_PARALLELDISPLAYBUS = {int(config["features"]["circuitpy_paralleldisplaybus"])}

# Frozen modules
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
""")

    # Generate pins.c (abbreviated for example)
    with open(os.path.join(board_dir, "pins.c"), "w") as f:
        f.write("""#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS
    \n""")

        # Add all pins
        for name, pin in config["pins"].items():
            f.write(
                f"    {{ MP_ROM_QSTR(MP_QSTR_{name}), MP_ROM_PTR(&pin_GPIO{pin['number']}) }},\n"
            )

        f.write("""    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) }
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
""")

    # Generate board.c
    with open(os.path.join(board_dir, "board.c"), "w") as f:
        f.write("""#include "supervisor/board.h"

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
""")


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print("Usage: generate_board_config.py <board_directory>")
        sys.exit(1)
    generate_config_files(sys.argv[1])
