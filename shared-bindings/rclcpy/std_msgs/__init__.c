// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "shared-bindings/rclcpy/std_msgs/__init__.h"

static const mp_rom_map_elem_t rclcpy_std_msgs_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rclcpy_dot_std_msgs) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Bool), (mp_obj_t)&rclcpy_std_msgs_bool_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Int32), (mp_obj_t)&rclcpy_std_msgs_int32_type },
};
static MP_DEFINE_CONST_DICT(rclcpy_std_msgs_globals, rclcpy_std_msgs_globals_table);

const mp_obj_module_t rclcpy_std_msgs_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rclcpy_std_msgs_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rclcpy_dot_std_msgs, rclcpy_std_msgs_module);
