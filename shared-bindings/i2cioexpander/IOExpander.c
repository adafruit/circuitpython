// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/i2cioexpander/IOExpander.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/util.h"
#include "shared/runtime/context_manager_helpers.h"

//| class IOExpander:
//|     """Control a generic I2C-based GPIO expander
//|
//|     IOExpander provides a simple interface to I2C-based GPIO expanders that
//|     use basic register reads and writes for control. The expander provides
//|     individual pins through the `pins` attribute that implement the
//|     DigitalInOutProtocol.
//|     """
//|
//|     def __init__(
//|         self,
//|         i2c: busio.I2C,
//|         address: int,
//|         num_pins: int,
//|         set_value_reg: Optional[int] = None,
//|         get_value_reg: Optional[int] = None,
//|         set_direction_reg: Optional[int] = None,
//|     ) -> None:
//|         """Initialize an I2C GPIO expander
//|
//|         :param busio.I2C i2c: The I2C bus the expander is connected to
//|         :param int address: The I2C device address
//|         :param int num_pins: The number of GPIO pins (8 or 16)
//|         :param int set_value_reg: Register address to write pin values (optional)
//|         :param int get_value_reg: Register address to read pin values (optional)
//|         :param int set_direction_reg: Register address to set pin directions (optional)
//|         """
//|         ...

static mp_obj_t i2cioexpander_ioexpander_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_i2c, ARG_address, ARG_num_pins, ARG_set_value_reg, ARG_get_value_reg, ARG_set_direction_reg };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_num_pins, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_set_value_reg, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_get_value_reg, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_set_direction_reg, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Validate I2C object
    mp_obj_t i2c = mp_arg_validate_type(args[ARG_i2c].u_obj, &busio_i2c_type, MP_QSTR_i2c);

    // Validate address
    int address = args[ARG_address].u_int;
    if (address < 0 || address > 0x7F) {
        mp_raise_ValueError(MP_ERROR_TEXT("address out of range"));
    }

    // Validate num_pins
    int num_pins = args[ARG_num_pins].u_int;
    if (num_pins != 8 && num_pins != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("num_pins must be 8 or 16"));
    }

    i2cioexpander_ioexpander_obj_t *self = mp_obj_malloc(i2cioexpander_ioexpander_obj_t, &i2cioexpander_ioexpander_type);

    // Convert and validate register parameters
    uint16_t set_value_reg = NO_REGISTER;
    if (args[ARG_set_value_reg].u_obj != mp_const_none) {
        mp_int_t reg = mp_obj_get_int(args[ARG_set_value_reg].u_obj);
        mp_arg_validate_int_range(reg, 0, 255, MP_QSTR_set_value_reg);
        set_value_reg = reg;
    }

    uint16_t get_value_reg = NO_REGISTER;
    if (args[ARG_get_value_reg].u_obj != mp_const_none) {
        mp_int_t reg = mp_obj_get_int(args[ARG_get_value_reg].u_obj);
        mp_arg_validate_int_range(reg, 0, 255, MP_QSTR_get_value_reg);
        get_value_reg = reg;
    }

    uint16_t set_direction_reg = NO_REGISTER;
    if (args[ARG_set_direction_reg].u_obj != mp_const_none) {
        mp_int_t reg = mp_obj_get_int(args[ARG_set_direction_reg].u_obj);
        mp_arg_validate_int_range(reg, 0, 255, MP_QSTR_set_direction_reg);
        set_direction_reg = reg;
    }

    common_hal_i2cioexpander_ioexpander_construct(
        self,
        i2c,
        address,
        num_pins,
        set_value_reg,
        get_value_reg,
        set_direction_reg);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialize the expander. No further operations are possible."""
//|         ...
static mp_obj_t i2cioexpander_ioexpander_deinit(mp_obj_t self_in) {
    i2cioexpander_ioexpander_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_i2cioexpander_ioexpander_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(i2cioexpander_ioexpander_deinit_obj, i2cioexpander_ioexpander_deinit);

//|     def __enter__(self) -> IOExpander:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
static mp_obj_t i2cioexpander_ioexpander___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_i2cioexpander_ioexpander_deinit(MP_OBJ_TO_PTR(args[0]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(i2cioexpander_ioexpander___exit___obj, 4, 4, i2cioexpander_ioexpander___exit__);

//|     @property
//|     def input_value(self) -> int:
//|         """Read the live value of all pins at once. Returns an integer where each
//|         bit represents a pin's current state."""
//|         ...
static mp_obj_t i2cioexpander_ioexpander_obj_get_input_value(mp_obj_t self_in) {
    i2cioexpander_ioexpander_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t value;
    if (!common_hal_i2cioexpander_ioexpander_get_input_value(self, &value)) {
        mp_raise_OSError(MP_EIO);
    }
    return MP_OBJ_NEW_SMALL_INT(value);
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cioexpander_ioexpander_get_input_value_obj, i2cioexpander_ioexpander_obj_get_input_value);

MP_PROPERTY_GETTER(i2cioexpander_ioexpander_input_value_obj,
    (mp_obj_t)&i2cioexpander_ioexpander_get_input_value_obj);

//|     @property
//|     def output_value(self) -> int:
//|         """Get or set the cached output value. Reading returns the last value written,
//|         not the live pin state. Writing updates the output pins."""
//|         ...
//|     @output_value.setter
//|     def output_value(self, val: int) -> None: ...
static mp_obj_t i2cioexpander_ioexpander_obj_get_output_value(mp_obj_t self_in) {
    i2cioexpander_ioexpander_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t value;
    common_hal_i2cioexpander_ioexpander_get_output_value(self, &value);
    return mp_obj_new_int(value);
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cioexpander_ioexpander_get_output_value_obj, i2cioexpander_ioexpander_obj_get_output_value);

static mp_obj_t i2cioexpander_ioexpander_obj_set_output_value(mp_obj_t self_in, mp_obj_t value) {
    i2cioexpander_ioexpander_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_negative_errno_t result = common_hal_i2cioexpander_ioexpander_set_output_value(self, mp_obj_get_int(value));
    if (result != 0) {
        mp_raise_OSError(result);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(i2cioexpander_ioexpander_set_output_value_obj, i2cioexpander_ioexpander_obj_set_output_value);

MP_PROPERTY_GETSET(i2cioexpander_ioexpander_output_value_obj,
    (mp_obj_t)&i2cioexpander_ioexpander_get_output_value_obj,
    (mp_obj_t)&i2cioexpander_ioexpander_set_output_value_obj);

//|     @property
//|     def output_mask(self) -> int:
//|         """Get or set which pins are configured as outputs. Each bit in the mask
//|         represents a pin: 1 for output, 0 for input."""
//|         ...
//|     @output_mask.setter
//|     def output_mask(self, val: int) -> None: ...
static mp_obj_t i2cioexpander_ioexpander_obj_get_output_mask(mp_obj_t self_in) {
    i2cioexpander_ioexpander_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t output_mask;
    common_hal_i2cioexpander_ioexpander_get_output_mask(self, &output_mask);
    return mp_obj_new_int(output_mask);
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cioexpander_ioexpander_get_output_mask_obj, i2cioexpander_ioexpander_obj_get_output_mask);

static mp_obj_t i2cioexpander_ioexpander_obj_set_output_mask(mp_obj_t self_in, mp_obj_t value) {
    i2cioexpander_ioexpander_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_negative_errno_t result = common_hal_i2cioexpander_ioexpander_set_output_mask(self, mp_obj_get_int(value));
    if (result != 0) {
        mp_raise_OSError(result);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(i2cioexpander_ioexpander_set_output_mask_obj, i2cioexpander_ioexpander_obj_set_output_mask);

MP_PROPERTY_GETSET(i2cioexpander_ioexpander_output_mask_obj,
    (mp_obj_t)&i2cioexpander_ioexpander_get_output_mask_obj,
    (mp_obj_t)&i2cioexpander_ioexpander_set_output_mask_obj);

//|     @property
//|     def pins(self) -> Tuple[IOPin, ...]:
//|         """A tuple of `IOPin` objects that implement the DigitalInOutProtocol.
//|         Each pin can be used like a digitalio.DigitalInOut object."""
//|         ...
static mp_obj_t i2cioexpander_ioexpander_obj_get_pins(mp_obj_t self_in) {
    i2cioexpander_ioexpander_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_i2cioexpander_ioexpander_get_pins(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cioexpander_ioexpander_get_pins_obj, i2cioexpander_ioexpander_obj_get_pins);

MP_PROPERTY_GETTER(i2cioexpander_ioexpander_pins_obj,
    (mp_obj_t)&i2cioexpander_ioexpander_get_pins_obj);

static const mp_rom_map_elem_t i2cioexpander_ioexpander_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&i2cioexpander_ioexpander_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&i2cioexpander_ioexpander___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_input_value), MP_ROM_PTR(&i2cioexpander_ioexpander_input_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_output_value), MP_ROM_PTR(&i2cioexpander_ioexpander_output_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_output_mask), MP_ROM_PTR(&i2cioexpander_ioexpander_output_mask_obj) },
    { MP_ROM_QSTR(MP_QSTR_pins), MP_ROM_PTR(&i2cioexpander_ioexpander_pins_obj) },
};
static MP_DEFINE_CONST_DICT(i2cioexpander_ioexpander_locals_dict, i2cioexpander_ioexpander_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    i2cioexpander_ioexpander_type,
    MP_QSTR_IOExpander,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, i2cioexpander_ioexpander_make_new,
    locals_dict, &i2cioexpander_ioexpander_locals_dict
    );
