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
#include "bindings/rp2clock/OutputPin.h"
#include "common-hal/rp2clock/OutputPin.h"

STATIC void check_for_deinit(rp2clock_outputpin_obj_t *self) {
    if (common_hal_rp2clock_outputpin_deinited(self)) {
        raise_deinited_error();
    }
}

//| class OutputPin:
//|     def __init__(
//|         self, pin: microcontroller.Pin, *, src: rp2clock.AuxSrc, divisor: float
//|     ) -> None:
//|         """Creates a clock output pip object.
//|         pin: Pin to be used as clock input, allowed pins: 22,23,24,25
//|         src: points to the source clock to be connected to the output pin.
//|         divisor: Divisor for clock before it's driven onto pin.
//|         """
STATIC mp_obj_t rp2clock_outputpin_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_src, ARG_divisor };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,         MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_src,         MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_rom_obj = mp_const_none} },
        { MP_QSTR_divisor,     MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    rp2clock_outputpin_obj_t *self = m_new_obj(rp2clock_outputpin_obj_t);
    self->base.type = &rp2clock_outputpin_type;

    // Validate pin number
    common_hal_rp2clock_outputpin_validate_src_pin(args[ARG_pin].u_rom_obj);
    self->pin = args[ARG_pin].u_rom_obj;
    self->divisor = common_hal_rp2clock_outputpin_validate_divisor(mp_obj_get_float(args[ARG_divisor].u_obj));

    // Validate pin based on clock
    if (args[ARG_src].u_rom_obj != mp_const_none) {
        self->src = validate_auxsrc(args[ARG_src].u_rom_obj, MP_QSTR_src);
    } else {
        self->src = AUXSRC_NONE;
    }
    self->enabled = false;
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Releases the pin and frees any resources."""
STATIC mp_obj_t rp2clock_outputpin_deinit(mp_obj_t self_in) {
    rp2clock_outputpin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Release pin
    common_hal_rp2clock_outputpin_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rp2clock_outputpin_deinit_obj, rp2clock_outputpin_deinit);

//|     def enable(self) -> None:
//|         """Configures the pin and enables the clock output"""
STATIC mp_obj_t rp2clock_outputpin_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rp2clock_outputpin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    common_hal_rp2clock_outputpin_enable(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2clock_outputpin_enable_obj, 1, rp2clock_outputpin_enable);

//|     def disable(self) -> None:
//|         """Disableds the pin and external clock"""
STATIC mp_obj_t rp2clock_outputpin_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rp2clock_outputpin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    common_hal_rp2clock_outputpin_disable(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2clock_outputpin_disable_obj, 1, rp2clock_outputpin_disable);

//|     src: rp2clock.AuxSrc
//|     """Auxiliary source clock to be driven to pin."""
static mp_obj_t rp2clock_outputpin_src_get(mp_obj_t self_in) {
    rp2clock_outputpin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return auxsrc_get_obj(self->src);
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2clock_outputpin_src_get_obj, rp2clock_outputpin_src_get);

static mp_obj_t rp2clock_outputpin_src_set(mp_obj_t self_in, mp_obj_t src_obj) {
    rp2clock_outputpin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    self->src = validate_auxsrc(src_obj, MP_QSTR_src);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rp2clock_outputpin_src_set_obj, rp2clock_outputpin_src_set);
MP_PROPERTY_GETSET(rp2clock_outputpin_src_obj,
    (mp_obj_t)&rp2clock_outputpin_src_get_obj,
    (mp_obj_t)&rp2clock_outputpin_src_set_obj);

//|     divisor: float
//|     """Divisor used to divide the clock before it's output to pin."""
//|
static mp_obj_t rp2clock_outputpin_divisor_get(mp_obj_t self_in) {
    rp2clock_outputpin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(self->divisor);
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2clock_outputpin_divisor_get_obj, rp2clock_outputpin_divisor_get);

static mp_obj_t rp2clock_outputpin_divisor_set(mp_obj_t self_in, mp_obj_t divisor_obj) {
    rp2clock_outputpin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    self->divisor = common_hal_rp2clock_outputpin_validate_divisor(mp_obj_get_float(divisor_obj));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rp2clock_outputpin_divisor_set_obj, rp2clock_outputpin_divisor_set);
MP_PROPERTY_GETSET(rp2clock_outputpin_divisor_obj,
    (mp_obj_t)&rp2clock_outputpin_divisor_get_obj,
    (mp_obj_t)&rp2clock_outputpin_divisor_set_obj);


STATIC const mp_rom_map_elem_t rp2clock_outputpin_locals_dict_table[] = {
    // Functions
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&rp2clock_outputpin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),          MP_ROM_PTR(&rp2clock_outputpin_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable),         MP_ROM_PTR(&rp2clock_outputpin_disable_obj) },
    // Properties
    { MP_ROM_QSTR(MP_QSTR_src),             MP_ROM_PTR(&rp2clock_outputpin_src_obj) },
    { MP_ROM_QSTR(MP_QSTR_divisor),         MP_ROM_PTR(&rp2clock_outputpin_divisor_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rp2clock_outputpin_locals_dict, rp2clock_outputpin_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rp2clock_outputpin_type,
    MP_QSTR_OutputPin,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, rp2clock_outputpin_make_new,
    locals_dict, &rp2clock_outputpin_locals_dict
    );
