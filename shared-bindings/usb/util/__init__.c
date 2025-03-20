// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdarg.h>
#include <string.h>

#include "py/obj.h"
#include "py/objexcept.h"
#include "py/misc.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/usb/util/__init__.h"

//| """USB Util
//|
//| This is a subset of the PyUSB util module.
//| """
//|
//| SPEED_LOW: int = ...
//| """A low speed, 1.5 Mbit/s device."""
//|
//| SPEED_FULL: int = ...
//| """A full speed, 12 Mbit/s device."""
//|
//| SPEED_HIGH: int = ...
//| """A high speed, 480 Mbit/s device."""
//|

static mp_rom_map_elem_t usb_util_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_usb_dot_util) },
    // Speed constants
    { MP_ROM_QSTR(MP_QSTR_SPEED_LOW),       MP_OBJ_NEW_SMALL_INT(PYUSB_SPEED_LOW) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_FULL),      MP_OBJ_NEW_SMALL_INT(PYUSB_SPEED_FULL) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_HIGH),      MP_OBJ_NEW_SMALL_INT(PYUSB_SPEED_HIGH) },
};

static MP_DEFINE_CONST_DICT(usb_util_module_globals, usb_util_module_globals_table);

const mp_obj_module_t usb_util_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_util_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_dot_util, usb_util_module);
