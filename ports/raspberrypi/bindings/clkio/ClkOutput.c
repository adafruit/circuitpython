/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Elliot Buller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "shared-bindings/util.h"
#include "bindings/clkio/ClkOutput.h"
#include "common-hal/clkio/ClkOutput.h"

STATIC void check_for_deinit(clkio_clkoutput_obj_t *self) {
    if (common_hal_clkio_clkoutput_deinited(self)) {
        raise_deinited_error();
    }
}

//| class ClkOutput:
//|     def __init__(
//|         self, pin: microcontroller.Pin, *, clksrc: clkio.ClkAuxSrc, divisor: float
//|     ) -> None:
//|         """Creates a clock output pip object.
//|         pin: Pin to be used as clock input, allowed pins: 22,23,24,25
//|         clksrc: points to the source clock to be connected to the output pin.
//|         divisor: Divisor for clock before it's driven onto pin.
//|         """
STATIC mp_obj_t clkio_clkoutput_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_clksrc, ARG_divisor };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,         MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_clksrc,      MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_rom_obj = mp_const_none} },
        { MP_QSTR_divisor,     MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    clkio_clkoutput_obj_t *self = m_new_obj(clkio_clkoutput_obj_t);
    self->base.type = &clkio_clkoutput_type;

    // Validate pin number
    common_hal_clkio_clkoutput_validate_clksrc_pin(args[ARG_pin].u_rom_obj);
    self->pin = args[ARG_pin].u_rom_obj;
    self->divisor = common_hal_clkio_clkoutput_validate_divisor(mp_obj_get_float(args[ARG_divisor].u_obj));

    // Validate pin based on clock
    if (args[ARG_clksrc].u_rom_obj != mp_const_none) {
        self->clksrc = validate_clkauxsrc(args[ARG_clksrc].u_rom_obj, MP_QSTR_clksrc);
    } else {
        self->clksrc = CLKAUXSRC_NONE;
    }
    self->enabled = false;
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Releases the pin and frees any resources."""
STATIC mp_obj_t clkio_clkoutput_deinit(mp_obj_t self_in) {
    clkio_clkoutput_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Release pin
    common_hal_clkio_clkoutput_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(clkio_clkoutput_deinit_obj, clkio_clkoutput_deinit);

//|     def enable(self) -> None:
//|         """Configures the pin and enables the clock output"""
STATIC mp_obj_t clkio_clkoutput_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    clkio_clkoutput_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    common_hal_clkio_clkoutput_enable(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(clkio_clkoutput_enable_obj, 1, clkio_clkoutput_enable);

//|     def disable(self) -> None:
//|         """Disableds the pin and external clock"""
STATIC mp_obj_t clkio_clkoutput_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    clkio_clkoutput_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    common_hal_clkio_clkoutput_disable(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(clkio_clkoutput_disable_obj, 1, clkio_clkoutput_disable);

//|     clksrc: clkio.ClkAuxSrc
//|     """Auxiliary source clock to be driven to pin."""
static mp_obj_t clkio_clkoutput_clksrc_get(mp_obj_t self_in) {
    clkio_clkoutput_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return clkauxsrc_get_obj(self->clksrc);
}
MP_DEFINE_CONST_FUN_OBJ_1(clkio_clkoutput_clksrc_get_obj, clkio_clkoutput_clksrc_get);

static mp_obj_t clkio_clkoutput_clksrc_set(mp_obj_t self_in, mp_obj_t clksrc_obj) {
    clkio_clkoutput_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    self->clksrc = validate_clkauxsrc(clksrc_obj, MP_QSTR_clksrc);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(clkio_clkoutput_clksrc_set_obj, clkio_clkoutput_clksrc_set);
MP_PROPERTY_GETSET(clkio_clkoutput_clksrc_obj,
    (mp_obj_t)&clkio_clkoutput_clksrc_get_obj,
    (mp_obj_t)&clkio_clkoutput_clksrc_set_obj);

//|     divisor: float
//|     """Divisor used to divide the clock before it's output to pin."""
//|
static mp_obj_t clkio_clkoutput_divisor_get(mp_obj_t self_in) {
    clkio_clkoutput_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(self->divisor);
}
MP_DEFINE_CONST_FUN_OBJ_1(clkio_clkoutput_divisor_get_obj, clkio_clkoutput_divisor_get);

static mp_obj_t clkio_clkoutput_divisor_set(mp_obj_t self_in, mp_obj_t divisor_obj) {
    clkio_clkoutput_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    self->divisor = common_hal_clkio_clkoutput_validate_divisor(mp_obj_get_float(divisor_obj));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(clkio_clkoutput_divisor_set_obj, clkio_clkoutput_divisor_set);
MP_PROPERTY_GETSET(clkio_clkoutput_divisor_obj,
    (mp_obj_t)&clkio_clkoutput_divisor_get_obj,
    (mp_obj_t)&clkio_clkoutput_divisor_set_obj);


STATIC const mp_rom_map_elem_t clkio_clkoutput_locals_dict_table[] = {
    // Functions
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&clkio_clkoutput_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),          MP_ROM_PTR(&clkio_clkoutput_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable),         MP_ROM_PTR(&clkio_clkoutput_disable_obj) },
    // Properties
    { MP_ROM_QSTR(MP_QSTR_clksrc),          MP_ROM_PTR(&clkio_clkoutput_clksrc_obj) },
    { MP_ROM_QSTR(MP_QSTR_divisor),         MP_ROM_PTR(&clkio_clkoutput_divisor_obj) },
};
STATIC MP_DEFINE_CONST_DICT(clkio_clkoutput_locals_dict, clkio_clkoutput_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    clkio_clkoutput_type,
    MP_QSTR_ClkOutput,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, clkio_clkoutput_make_new,
    locals_dict, &clkio_clkoutput_locals_dict
    );
