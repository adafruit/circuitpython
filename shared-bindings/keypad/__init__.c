// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2011 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"

#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/keypad/Event.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/keypad/KeyMatrix.h"
#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/keypad/ShiftRegisterKeys.h"
#include "shared-bindings/util.h"

static void check_for_deinit(keypad_keymatrix_obj_t *self) {
    if (common_hal_keypad_deinited(self)) {
        raise_deinited_error();
    }
}

static mp_obj_t keypad_generic_reset(mp_obj_t self_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_keypad_generic_reset(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_generic_reset_obj, keypad_generic_reset);

static mp_obj_t keypad_generic_get_key_count(mp_obj_t self_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return MP_OBJ_NEW_SMALL_INT(common_hal_keypad_generic_get_key_count(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_generic_get_key_count_obj, keypad_generic_get_key_count);

MP_PROPERTY_GETTER(keypad_generic_key_count_obj,
    (mp_obj_t)&keypad_generic_get_key_count_obj);

static mp_obj_t keypad_generic_get_events(mp_obj_t self_in) {
    keypad_keymatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return common_hal_keypad_generic_get_events(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_generic_get_events_obj, keypad_generic_get_events);

MP_PROPERTY_GETTER(keypad_generic_events_obj,
    (mp_obj_t)&keypad_generic_get_events_obj);

//| """Support for scanning keys and key matrices
//|
//| The `keypad` module provides native support to scan sets of keys or buttons,
//| connected independently to individual pins,
//| connected to a shift register,
//| or connected in a row-and-column matrix.
//|
//| For more information about working with the `keypad` module in CircuitPython,
//| see `this Learn guide <https://learn.adafruit.com/key-pad-matrix-scanning-in-circuitpython>`_.
//|
//| .. warning:: Using pull-downs with `keypad` on Raspberry Pi RP2350 A2 stepping has some limitations
//|    due to a GPIO hardware issue that causes excessive leakage current (~120uA).
//|    A pin can read as high even when driven or pulled low, if the input signal is high
//|    impedance or if an attached pull-down resistor is too weak (has too high a value).
//|    See the warning in `digitalio` for more information.
//|
//| .. jinja
//| """

static mp_rom_map_elem_t keypad_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),          MP_OBJ_NEW_QSTR(MP_QSTR_keypad) },
    { MP_ROM_QSTR(MP_QSTR_Event),             MP_OBJ_FROM_PTR(&keypad_event_type) },
    { MP_ROM_QSTR(MP_QSTR_EventQueue),        MP_OBJ_FROM_PTR(&keypad_eventqueue_type) },
    { MP_ROM_QSTR(MP_QSTR_KeyMatrix),         MP_OBJ_FROM_PTR(&keypad_keymatrix_type) },
    { MP_ROM_QSTR(MP_QSTR_Keys),              MP_OBJ_FROM_PTR(&keypad_keys_type) },
    { MP_ROM_QSTR(MP_QSTR_ShiftRegisterKeys), MP_OBJ_FROM_PTR(&keypad_shiftregisterkeys_type) },
};

static MP_DEFINE_CONST_DICT(keypad_module_globals, keypad_module_globals_table);

const mp_obj_module_t keypad_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&keypad_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_keypad, keypad_module);
