// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/PinConfiguration.h"

#if CIRCUITPY_PIN_CONFIGURATION

//| class PinConfiguration:
//|     """The settings requested for a pin, given in place of the pin to an object that uses it.
//|
//|     A `PinConfiguration` does not change the pin itself. It only carries the settings.
//|     The object that it is given to applies the settings when it sets up the pin.
//|     The same `PinConfiguration` can be given to more than one object, one after another.
//|
//|     These constructors accept a `PinConfiguration` where they accept a `Pin`:
//|     `digitalio.DigitalInOut` and `keypad.Keys`.
//|
//|     .. code-block:: python
//|
//|         import board
//|         import keypad
//|         from digitalio import Pull
//|         from microcontroller import PinConfiguration
//|
//|         # Key 0 is wired to ground, key 1 to 3.3V.
//|         keys = keypad.Keys(
//|             (PinConfiguration(board.D5, pull=Pull.UP), PinConfiguration(board.D6, pull=Pull.DOWN)),
//|             value_when_pressed=False,
//|         )
//|     """
//|
//|     def __init__(self, pin: Pin, *, pull: Optional[digitalio.Pull] = None) -> None:
//|         """Create the settings for one pin.
//|
//|         :param Pin pin: The pin that the settings are for.
//|         :param Optional[digitalio.Pull] pull: The pull to use on the pin.
//|           ``None`` means that the object receiving this configuration chooses the pull,
//|           as it would for a plain `Pin`.
//|         """
//|         ...
//|
static mp_obj_t mcu_pin_configuration_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_pull };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_pull, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *pin = validate_obj_is_pin(args[ARG_pin].u_obj, MP_QSTR_pin);
    validate_pull(args[ARG_pull].u_obj, MP_QSTR_pull);

    mcu_pin_configuration_obj_t *self = mp_obj_malloc(mcu_pin_configuration_obj_t, &mcu_pin_configuration_type);
    self->pin = pin;
    self->pull = args[ARG_pull].u_obj;
    return MP_OBJ_FROM_PTR(self);
}

//|     pin: Pin
//|     """The pin that the settings are for. (read-only)"""
static mp_obj_t mcu_pin_configuration_get_pin(mp_obj_t self_in) {
    mcu_pin_configuration_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->pin);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcu_pin_configuration_get_pin_obj, mcu_pin_configuration_get_pin);

MP_PROPERTY_GETTER(mcu_pin_configuration_pin_obj,
    (mp_obj_t)&mcu_pin_configuration_get_pin_obj);

//|     pull: Optional[digitalio.Pull]
//|     """The pull to use on the pin, or ``None`` if the object receiving this configuration
//|     chooses the pull. (read-only)"""
//|
static mp_obj_t mcu_pin_configuration_get_pull(mp_obj_t self_in) {
    mcu_pin_configuration_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->pull;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcu_pin_configuration_get_pull_obj, mcu_pin_configuration_get_pull);

MP_PROPERTY_GETTER(mcu_pin_configuration_pull_obj,
    (mp_obj_t)&mcu_pin_configuration_get_pull_obj);

static const mp_rom_map_elem_t mcu_pin_configuration_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&mcu_pin_configuration_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_pull), MP_ROM_PTR(&mcu_pin_configuration_pull_obj) },
};
static MP_DEFINE_CONST_DICT(mcu_pin_configuration_locals_dict, mcu_pin_configuration_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mcu_pin_configuration_type,
    MP_QSTR_PinConfiguration,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, mcu_pin_configuration_make_new,
    locals_dict, &mcu_pin_configuration_locals_dict
    );

mp_obj_t mcu_pin_configuration_unpack(mp_obj_t obj, digitalio_pull_t *pull) {
    if (!mp_obj_is_type(obj, &mcu_pin_configuration_type)) {
        return obj;
    }
    const mcu_pin_configuration_obj_t *configuration = MP_OBJ_TO_PTR(obj);
    if (pull != NULL && configuration->pull != mp_const_none) {
        *pull = validate_pull(configuration->pull, MP_QSTR_pull);
    }
    return MP_OBJ_FROM_PTR(configuration->pin);
}

#endif
