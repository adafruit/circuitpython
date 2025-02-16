// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/zephyr_serial/__init__.h"
#include "bindings/zephyr_serial/UART.h"

#include "py/runtime.h"

//| """Zephyr UART driver for fixed busses."""

static const mp_rom_map_elem_t zephyr_serial_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_zephyr_serial) },
    { MP_ROM_QSTR(MP_QSTR_UART),   MP_ROM_PTR(&zephyr_serial_uart_type) },
};

static MP_DEFINE_CONST_DICT(zephyr_serial_module_globals, zephyr_serial_module_globals_table);

const mp_obj_module_t zephyr_serial_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&zephyr_serial_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_zephyr_serial, zephyr_serial_module);
