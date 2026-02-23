// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/i2cioexpander/IOPin.h"
#include "shared-module/i2cioexpander/IOPin.h"

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DigitalInOutProtocol.h"
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"
#include "shared-bindings/util.h"

static void check_result(digitalinout_result_t result) {
    switch (result) {
        case DIGITALINOUT_OK:
            return;
        case DIGITALINOUT_PIN_BUSY:
            mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_Pin);
        #if CIRCUITPY_DIGITALIO_HAVE_INPUT_ONLY
        case DIGITALINOUT_INPUT_ONLY:
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_direction);
        #endif
        #if CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL
        case DIGITALINOUT_INVALID_PULL:
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_pull);
        #endif
        #if CIRCUITPY_DIGITALIO_HAVE_INVALID_DRIVE_MODE
        case DIGITALINOUT_INVALID_DRIVE_MODE:
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_drive_mode);
        #endif
    }
}

static inline void check_for_deinit(i2cioexpander_iopin_obj_t *self) {
    if (common_hal_i2cioexpander_iopin_deinited(self)) {
        raise_deinited_error();
    }
}
//| class IOPin:
//|     """Control a single pin on an `IOExpander` in the same way as `DigitalInOut`.
//|
//|        Not constructed directly. Get from `IOExpander.pins` instead.
//|     """
//|

//|     def switch_to_output(
//|         self, value: bool = False, drive_mode: digitalio.DriveMode = digitalio.DriveMode.PUSH_PULL
//|     ) -> None:
//|         """Set the drive mode and value and then switch to writing out digital values.
//|
//|         :param bool value: default value to set upon switching
//|         :param digitalio.DriveMode drive_mode: drive mode for the output"""
//|         ...
static mp_obj_t i2cioexpander_iopin_switch_to_output(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_value, ARG_drive_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_value,      MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_drive_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&digitalio_drive_mode_push_pull_obj)} },
    };
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    digitalio_drive_mode_t drive_mode = DRIVE_MODE_PUSH_PULL;
    if (args[ARG_drive_mode].u_rom_obj == MP_ROM_PTR(&digitalio_drive_mode_open_drain_obj)) {
        drive_mode = DRIVE_MODE_OPEN_DRAIN;
    }
    check_result(common_hal_i2cioexpander_iopin_switch_to_output(self, args[ARG_value].u_bool, drive_mode));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(i2cioexpander_iopin_switch_to_output_obj, 1, i2cioexpander_iopin_switch_to_output);

//|     def switch_to_input(self, pull: Optional[digitalio.Pull] = None) -> None:
//|         """Set the pull and then switch to read in digital values.
//|
//|         :param digitalio.Pull pull: pull configuration for the input"""
//|         ...
static mp_obj_t i2cioexpander_iopin_switch_to_input(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_pull };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
    };
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    check_result(common_hal_i2cioexpander_iopin_switch_to_input(self, validate_pull(args[ARG_pull].u_rom_obj, MP_QSTR_pull)));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(i2cioexpander_iopin_switch_to_input_obj, 1, i2cioexpander_iopin_switch_to_input);

//|     direction: digitalio.Direction
//|     """The direction of the pin."""
static mp_obj_t i2cioexpander_iopin_obj_get_direction(mp_obj_t self_in) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    digitalio_direction_t direction = common_hal_i2cioexpander_iopin_get_direction(self);
    if (direction == DIRECTION_INPUT) {
        return MP_OBJ_FROM_PTR(&digitalio_direction_input_obj);
    }
    return MP_OBJ_FROM_PTR(&digitalio_direction_output_obj);
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cioexpander_iopin_get_direction_obj, i2cioexpander_iopin_obj_get_direction);

static mp_obj_t i2cioexpander_iopin_obj_set_direction(mp_obj_t self_in, mp_obj_t value) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (value == MP_ROM_PTR(&digitalio_direction_input_obj)) {
        check_result(common_hal_i2cioexpander_iopin_switch_to_input(self, PULL_NONE));
    } else if (value == MP_ROM_PTR(&digitalio_direction_output_obj)) {
        check_result(common_hal_i2cioexpander_iopin_switch_to_output(self, false, DRIVE_MODE_PUSH_PULL));
    } else {
        mp_arg_error_invalid(MP_QSTR_direction);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(i2cioexpander_iopin_set_direction_obj, i2cioexpander_iopin_obj_set_direction);

MP_PROPERTY_GETSET(i2cioexpander_iopin_direction_obj,
    (mp_obj_t)&i2cioexpander_iopin_get_direction_obj,
    (mp_obj_t)&i2cioexpander_iopin_set_direction_obj);

//|     value: bool
//|     """The digital logic level of the pin."""
static mp_obj_t i2cioexpander_iopin_obj_get_value(mp_obj_t self_in) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    bool value;
    mp_negative_errno_t res = common_hal_i2cioexpander_iopin_get_value(self, &value);
    if (res != 0) {
        mp_raise_OSError(-res);
    }
    return mp_obj_new_bool(value);
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cioexpander_iopin_get_value_obj, i2cioexpander_iopin_obj_get_value);

static mp_obj_t i2cioexpander_iopin_obj_set_value(mp_obj_t self_in, mp_obj_t value) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_i2cioexpander_iopin_get_direction(self) == DIRECTION_INPUT) {
        mp_raise_AttributeError(MP_ERROR_TEXT("Cannot set value when direction is input."));
        return mp_const_none;
    }
    mp_negative_errno_t res = common_hal_i2cioexpander_iopin_set_value(self, mp_obj_is_true(value));
    if (res != 0) {
        mp_raise_OSError(-res);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(i2cioexpander_iopin_set_value_obj, i2cioexpander_iopin_obj_set_value);

MP_PROPERTY_GETSET(i2cioexpander_iopin_value_obj,
    (mp_obj_t)&i2cioexpander_iopin_get_value_obj,
    (mp_obj_t)&i2cioexpander_iopin_set_value_obj);

//|     drive_mode: digitalio.DriveMode
//|     """The pin drive mode."""
static mp_obj_t i2cioexpander_iopin_obj_get_drive_mode(mp_obj_t self_in) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_i2cioexpander_iopin_get_direction(self) == DIRECTION_INPUT) {
        mp_raise_AttributeError(MP_ERROR_TEXT("Drive mode not used when direction is input."));
        return mp_const_none;
    }
    digitalio_drive_mode_t drive_mode = common_hal_i2cioexpander_iopin_get_drive_mode(self);
    if (drive_mode == DRIVE_MODE_PUSH_PULL) {
        return MP_OBJ_FROM_PTR(&digitalio_drive_mode_push_pull_obj);
    }
    return MP_OBJ_FROM_PTR(&digitalio_drive_mode_open_drain_obj);
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cioexpander_iopin_get_drive_mode_obj, i2cioexpander_iopin_obj_get_drive_mode);

static mp_obj_t i2cioexpander_iopin_obj_set_drive_mode(mp_obj_t self_in, mp_obj_t drive_mode) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_i2cioexpander_iopin_get_direction(self) == DIRECTION_INPUT) {
        mp_raise_AttributeError(MP_ERROR_TEXT("Drive mode not used when direction is input."));
        return mp_const_none;
    }
    digitalio_drive_mode_t c_drive_mode = DRIVE_MODE_PUSH_PULL;
    if (drive_mode == MP_ROM_PTR(&digitalio_drive_mode_open_drain_obj)) {
        c_drive_mode = DRIVE_MODE_OPEN_DRAIN;
    }
    check_result(common_hal_i2cioexpander_iopin_set_drive_mode(self, c_drive_mode));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(i2cioexpander_iopin_set_drive_mode_obj, i2cioexpander_iopin_obj_set_drive_mode);

MP_PROPERTY_GETSET(i2cioexpander_iopin_drive_mode_obj,
    (mp_obj_t)&i2cioexpander_iopin_get_drive_mode_obj,
    (mp_obj_t)&i2cioexpander_iopin_set_drive_mode_obj);

//|     pull: Optional[digitalio.Pull]
//|     """The pin pull direction."""
static mp_obj_t i2cioexpander_iopin_obj_get_pull(mp_obj_t self_in) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_i2cioexpander_iopin_get_direction(self) == DIRECTION_OUTPUT) {
        mp_raise_AttributeError(MP_ERROR_TEXT("Pull not used when direction is output."));
        return mp_const_none;
    }
    digitalio_pull_t pull = common_hal_i2cioexpander_iopin_get_pull(self);
    if (pull == PULL_UP) {
        return MP_OBJ_FROM_PTR(&digitalio_pull_up_obj);
    } else if (pull == PULL_DOWN) {
        return MP_OBJ_FROM_PTR(&digitalio_pull_down_obj);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cioexpander_iopin_get_pull_obj, i2cioexpander_iopin_obj_get_pull);

static mp_obj_t i2cioexpander_iopin_obj_set_pull(mp_obj_t self_in, mp_obj_t pull_obj) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_i2cioexpander_iopin_get_direction(self) == DIRECTION_OUTPUT) {
        mp_raise_AttributeError(MP_ERROR_TEXT("Pull not used when direction is output."));
        return mp_const_none;
    }
    check_result(common_hal_i2cioexpander_iopin_set_pull(self, validate_pull(pull_obj, MP_QSTR_pull)));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(i2cioexpander_iopin_set_pull_obj, i2cioexpander_iopin_obj_set_pull);

MP_PROPERTY_GETSET(i2cioexpander_iopin_pull_obj,
    (mp_obj_t)&i2cioexpander_iopin_get_pull_obj,
    (mp_obj_t)&i2cioexpander_iopin_set_pull_obj);

// Protocol implementation for DigitalInOutProtocol
static void iopin_protocol_deinit(mp_obj_t self_in) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_i2cioexpander_iopin_deinit(self);
}

static bool iopin_protocol_deinited(mp_obj_t self_in) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_deinited(self);
}

static digitalinout_result_t iopin_protocol_switch_to_input(mp_obj_t self_in, digitalio_pull_t pull) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_switch_to_input(self, pull);
}

static digitalinout_result_t iopin_protocol_switch_to_output(mp_obj_t self_in, bool value, digitalio_drive_mode_t drive_mode) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_switch_to_output(self, value, drive_mode);
}

static digitalio_direction_t iopin_protocol_get_direction(mp_obj_t self_in) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_get_direction(self);
}

static mp_negative_errno_t iopin_protocol_get_value(mp_obj_t self_in, bool *value) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_get_value(self, value);
}

static mp_negative_errno_t iopin_protocol_set_value(mp_obj_t self_in, bool value) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_set_value(self, value);
}

static digitalio_drive_mode_t iopin_protocol_get_drive_mode(mp_obj_t self_in) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_get_drive_mode(self);
}

static digitalinout_result_t iopin_protocol_set_drive_mode(mp_obj_t self_in, digitalio_drive_mode_t drive_mode) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_set_drive_mode(self, drive_mode);
}

static digitalio_pull_t iopin_protocol_get_pull(mp_obj_t self_in) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_get_pull(self);
}

static digitalinout_result_t iopin_protocol_set_pull(mp_obj_t self_in, digitalio_pull_t pull) {
    i2cioexpander_iopin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_iopin_set_pull(self, pull);
}

static const digitalinout_p_t iopin_digitalinout_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_DigitalInOut)
    .deinit = iopin_protocol_deinit,
    .deinited = iopin_protocol_deinited,
    .switch_to_input = iopin_protocol_switch_to_input,
    .switch_to_output = iopin_protocol_switch_to_output,
    .get_direction = iopin_protocol_get_direction,
    .get_value = iopin_protocol_get_value,
    .set_value = iopin_protocol_set_value,
    .get_drive_mode = iopin_protocol_get_drive_mode,
    .set_drive_mode = iopin_protocol_set_drive_mode,
    .get_pull = iopin_protocol_get_pull,
    .set_pull = iopin_protocol_set_pull,
};

static const mp_rom_map_elem_t i2cioexpander_iopin_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_switch_to_input), MP_ROM_PTR(&i2cioexpander_iopin_switch_to_input_obj) },
    { MP_ROM_QSTR(MP_QSTR_switch_to_output), MP_ROM_PTR(&i2cioexpander_iopin_switch_to_output_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_direction), MP_ROM_PTR(&i2cioexpander_iopin_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&i2cioexpander_iopin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_drive_mode), MP_ROM_PTR(&i2cioexpander_iopin_drive_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_pull), MP_ROM_PTR(&i2cioexpander_iopin_pull_obj) },
};
static MP_DEFINE_CONST_DICT(i2cioexpander_iopin_locals_dict, i2cioexpander_iopin_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    i2cioexpander_iopin_type,
    MP_QSTR_IOPin,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    protocol, &iopin_digitalinout_p,
    locals_dict, &i2cioexpander_iopin_locals_dict
    );
