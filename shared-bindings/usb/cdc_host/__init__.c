// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: 2025 rianadon
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/objmodule.h"
#include "py/runtime.h"

#include "shared-bindings/usb/cdc_host/Serial.h"
#include "shared-bindings/usb/core/Device.h"

#include "tusb.h"
#include "class/cdc/cdc_host.h"

static mp_obj_t usb_cdc_host_find(mp_obj_t device_in, mp_obj_t interface_in) {
    if (!mp_obj_is_type(device_in, &usb_core_device_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("device must be a usb.core.Device object"));
    }

    usb_core_device_obj_t *device_obj = MP_OBJ_TO_PTR(device_in);
    uint8_t daddr = device_obj->device_address;

    mp_int_t interface_num = mp_obj_get_int(interface_in);
    uint8_t cdc_idx = tuh_cdc_itf_get_index(daddr, (uint8_t)interface_num);

    if (cdc_idx == TUSB_INDEX_INVALID_8) {
        return mp_const_none;
    }

    usb_cdc_host_serial_obj_t *serial_obj = mp_obj_malloc(usb_cdc_host_serial_obj_t, &usb_cdc_host_serial_type);
    serial_obj->idx = cdc_idx;
    serial_obj->timeout = -1.0f;
    serial_obj->write_timeout = -1.0f;

    return MP_OBJ_FROM_PTR(serial_obj);
}

static MP_DEFINE_CONST_FUN_OBJ_2(usb_cdc_host_find_obj, usb_cdc_host_find);

static const mp_rom_map_elem_t usb_cdc_host_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usb_dot_cdc_host) },
    { MP_ROM_QSTR(MP_QSTR_Serial),   MP_ROM_PTR(&usb_cdc_host_serial_type) },
    { MP_ROM_QSTR(MP_QSTR_find),   MP_ROM_PTR(&usb_cdc_host_find_obj) },
};

static MP_DEFINE_CONST_DICT(usb_cdc_host_module_globals, usb_cdc_host_module_globals_table);

const mp_obj_module_t usb_cdc_host_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_cdc_host_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_dot_cdc_host, usb_cdc_host_module);
