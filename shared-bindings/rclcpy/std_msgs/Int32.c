// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include "py/runtime.h"
#include "py/obj.h"

#include "shared-bindings/rclcpy/std_msgs/Int32.h"

static void int32_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    rclcpy_std_msgs_int32_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "Int32(data=");
    mp_obj_print_helper(print, mp_obj_new_int(self->data), PRINT_REPR);
    mp_print_str(print, ")");
}

static mp_obj_t int32_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, true);
    rclcpy_std_msgs_int32_obj_t *self = m_new_obj(rclcpy_std_msgs_int32_obj_t);
    self->base.type = &rclcpy_std_msgs_int32_type;

    if (n_args == 1) {
        self->data = mp_obj_get_int(args[0]);
    } else {
        self->data = 0;
    }
    return MP_OBJ_FROM_PTR(self);
}

static void int32_attr(mp_obj_t self_in, qstr attribute, mp_obj_t *destination) {
    rclcpy_std_msgs_int32_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (attribute == MP_QSTR_data) {
        if (destination[0] == MP_OBJ_NULL) {
            // Read access: msg.data
            destination[0] = mp_obj_new_int(self->data);
        } else {
            // Write access: msg.data = value
            self->data = mp_obj_get_int(destination[1]);
            destination[0] = MP_OBJ_NULL;
        }
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    rclcpy_std_msgs_int32_type,
    MP_QSTR_Int32,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, int32_make_new,
    print, int32_print,
    attr, int32_attr
    );
