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
#include "py/enum.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "shared-bindings/util.h"
#include "bindings/rp2clock/InputPin.h"
#include "common-hal/rp2clock/InputPin.h"

STATIC void check_for_deinit(rp2clock_inputpin_obj_t *self) {
    if (common_hal_rp2clock_inputpin_deinited(self)) {
        raise_deinited_error();
    }
}

//| class InputPin:
//|     """Route external clocks to internal clocks via dedicated pins."""
//|
//|     def __init__(
//|         self, pin: microcontroller.Pin, index: rp2clock.Index, src_freq: int, target_freq: int
//|     ) -> None:
//|         """Creates a clock input pin object.
//|
//|         .. note:: Valid pins are: GP20, GP22.
//|
//|         :param ~microcontroller.Pin pin: Pin to be used as clock input.
//|
//|         :param ~rp2clock.Index index: Destination clock to be connected to the input pin.
//|
//|         :param int src_freq: Frequency of clock input at the pin.
//|
//|         :param int target_freq: Desired frequency for ~rp2clock.Index clock."""
STATIC mp_obj_t rp2clock_inputpin_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_index, ARG_src_freq, ARG_target_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,         MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_src_freq,    MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_target_freq, MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    rp2clock_inputpin_obj_t *self = m_new_obj_with_finaliser(rp2clock_inputpin_obj_t);
    self->base.type = &rp2clock_inputpin_type;

    // Validate pin number
    common_hal_rp2clock_inputpin_validate_index_pin(args[ARG_pin].u_rom_obj);
    self->pin = args[ARG_pin].u_rom_obj;

    // Validate clock
    self->index = cp_enum_value(&rp2clock_index_type, args[ARG_index].u_obj, MP_QSTR_rp2clock_index);

    // Validate frequencies
    common_hal_rp2clock_inputpin_validate_freqs(args[ARG_src_freq].u_int, args[ARG_target_freq].u_int);
    self->src_freq = args[ARG_src_freq].u_int;
    self->target_freq = args[ARG_target_freq].u_int;

    // Enable and return
    common_hal_rp2clock_inputpin_enable(self);
    return MP_OBJ_FROM_PTR(self);
};

//|     def deinit(self) -> None:
//|         """Releases the pin and frees any resources."""
STATIC mp_obj_t rp2clock_inputpin_deinit(mp_obj_t self_in) {
    rp2clock_inputpin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Release pin
    common_hal_rp2clock_inputpin_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rp2clock_inputpin_deinit_obj, rp2clock_inputpin_deinit);

//|     def enable(self) -> None:
//|         """Configures the pin and enables the internal clock."""
STATIC mp_obj_t rp2clock_inputpin_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rp2clock_inputpin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    common_hal_rp2clock_inputpin_enable(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2clock_inputpin_enable_obj, 1, rp2clock_inputpin_enable);

//|     def disable(self) -> None:
//|         """Disables the pin and internal clock"""
STATIC mp_obj_t rp2clock_inputpin_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rp2clock_inputpin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    common_hal_rp2clock_inputpin_disable(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2clock_inputpin_disable_obj, 1, rp2clock_inputpin_disable);

//|     def enabled(self) -> bool:
//|         """Check if pin is enabled."""
//|
static mp_obj_t rp2clock_inputpin_get_enabled(mp_obj_t self_in) {
    rp2clock_inputpin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(self->enabled);
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2clock_inputpin_get_enabled_obj, rp2clock_inputpin_get_enabled);
MP_PROPERTY_GETTER(rp2clock_inputpin_enabled_obj,
    (mp_obj_t)&rp2clock_inputpin_get_enabled_obj);

STATIC const mp_rom_map_elem_t rp2clock_inputpin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__),         MP_ROM_PTR(&rp2clock_inputpin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&rp2clock_inputpin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),          MP_ROM_PTR(&rp2clock_inputpin_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable),         MP_ROM_PTR(&rp2clock_inputpin_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enabled),         MP_ROM_PTR(&rp2clock_inputpin_enabled_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rp2clock_inputpin_locals_dict, rp2clock_inputpin_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rp2clock_inputpin_type,
    MP_QSTR_InputPin,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, rp2clock_inputpin_make_new,
    locals_dict, &rp2clock_inputpin_locals_dict
    );
