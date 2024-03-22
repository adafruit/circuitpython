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
#include "bindings/rp2clock/InputPin.h"
#include "common-hal/rp2clock/InputPin.h"

STATIC void check_for_deinit(rp2clock_inputpin_obj_t *self) {
    if (common_hal_rp2clock_inputpin_deinited(self)) {
        raise_deinited_error();
    }
}

//| class InputPin:
//|     def __init__(
//|         self,
//|         pin: microcontroller.Pin,
//|         *,
//|         index: rp2clock.Index,
//|         src_freq: int,
//|         target_freq: int
//|     ) -> None:
//|         """Creates a clock input pin object.
//|         pin: Pin to be used as clock input, allowed pins: 20,22
//|         index: points to the destination clock to be connected to the input pin.
//|         src_freq: External input frequency at the pin.
//|         target_freq: Desired frequency for index.
//|         """

STATIC mp_obj_t rp2clock_inputpin_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_index, ARG_src_freq, ARG_target_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,         MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index,       MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_rom_obj = mp_const_none} },
        { MP_QSTR_src_freq,    MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 0} },
        { MP_QSTR_target_freq, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    rp2clock_inputpin_obj_t *self = m_new_obj(rp2clock_inputpin_obj_t);
    self->base.type = &rp2clock_inputpin_type;

    // Validate pin number
    common_hal_rp2clock_inputpin_validate_index_pin(args[ARG_pin].u_rom_obj);
    self->pin = args[ARG_pin].u_rom_obj;

    // Validate pin based on clock
    if (args[ARG_index].u_rom_obj != mp_const_none) {
        self->index = validate_index(args[ARG_index].u_rom_obj, MP_QSTR_index);
        self->src_freq = args[ARG_src_freq].u_int;
        self->target_freq = args[ARG_target_freq].u_int;
        // Validate frequencies if set
        if (self->src_freq && self->target_freq) {
            common_hal_rp2clock_inputpin_validate_freqs(args[ARG_src_freq].u_int, args[ARG_target_freq].u_int);
        }
    } else {
        self->index = INDEX_NONE;
    }
    self->enabled = false;
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
//|         """Configures the pin and enables the internal clock"""
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

//|     def set_freq(self, src_freq: int, target_freq: int) -> None:
//|         """Configures the src and target frequency. Must be set before enable() is called."""
STATIC mp_obj_t rp2clock_inputpin_set_freq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_src_freq, ARG_target_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_src_freq,  MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_target_freq,  MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    rp2clock_inputpin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    common_hal_rp2clock_inputpin_validate_freqs(args[ARG_src_freq].u_int, args[ARG_target_freq].u_int);
    self->src_freq = args[ARG_src_freq].u_int;
    self->target_freq = args[ARG_target_freq].u_int;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2clock_inputpin_set_freq_obj, 1, rp2clock_inputpin_set_freq);

//|     def get_freq(self) -> tuple[int, int]:
//|         """Returns the (src, target) frequency as tuple."""
STATIC mp_obj_t rp2clock_inputpin_get_freq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rp2clock_inputpin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_obj_t tup[] = {
        mp_obj_new_int(self->src_freq),
        mp_obj_new_int(self->target_freq)
    };
    // Return tuple with (src, target)
    return mp_obj_new_tuple(2, tup);
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2clock_inputpin_get_freq_obj, 1, rp2clock_inputpin_get_freq);

//|     index: rp2clock.Index
//|     """Clock that will be driven from external pin."""
//|
static mp_obj_t rp2clock_inputpin_index_get(mp_obj_t self_in) {
    rp2clock_inputpin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return index_get_obj(self->index);
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2clock_inputpin_index_get_obj, rp2clock_inputpin_index_get);

static mp_obj_t rp2clock_inputpin_index_set(mp_obj_t self_in, mp_obj_t index_obj) {
    rp2clock_inputpin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    self->index = validate_index(index_obj, MP_QSTR_index);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rp2clock_inputpin_index_set_obj, rp2clock_inputpin_index_set);
MP_PROPERTY_GETSET(rp2clock_inputpin_index_obj,
    (mp_obj_t)&rp2clock_inputpin_index_get_obj,
    (mp_obj_t)&rp2clock_inputpin_index_set_obj);


STATIC const mp_rom_map_elem_t rp2clock_inputpin_locals_dict_table[] = {
    // Functions
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&rp2clock_inputpin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),          MP_ROM_PTR(&rp2clock_inputpin_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable),         MP_ROM_PTR(&rp2clock_inputpin_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_freq),        MP_ROM_PTR(&rp2clock_inputpin_set_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_freq),        MP_ROM_PTR(&rp2clock_inputpin_get_freq_obj) },
    // Properties
    { MP_ROM_QSTR(MP_QSTR_index),        MP_ROM_PTR(&rp2clock_inputpin_index_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rp2clock_inputpin_locals_dict, rp2clock_inputpin_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rp2clock_inputpin_type,
    MP_QSTR_InputPin,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, rp2clock_inputpin_make_new,
    locals_dict, &rp2clock_inputpin_locals_dict
    );
