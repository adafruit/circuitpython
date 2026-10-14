// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/usb_host_bulk/__init__.h"
#include "shared-bindings/usb_host_bulk/InStream.h"

//| """Continuous bulk transfers on USB host ports
//|
//| This module keeps an endpoint busy in the background so that
//| continuously-streaming devices, such as RTL2832U SDR dongles, never overflow.
//| Devices are still found and configured with `usb.core`; `usb_host` manages the
//| host ports themselves.
//|
//| Available only on boards built with the PIO USB host and this feature enabled.
//| """
//|

static const mp_rom_map_elem_t usb_host_bulk_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usb_host_bulk) },
    { MP_ROM_QSTR(MP_QSTR_InStream), MP_ROM_PTR(&usb_host_bulk_instream_type) },
};

static MP_DEFINE_CONST_DICT(usb_host_bulk_module_globals, usb_host_bulk_module_globals_table);

const mp_obj_module_t usb_host_bulk_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_host_bulk_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_host_bulk, usb_host_bulk_module);
