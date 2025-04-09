// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/rclcpy/__init__.h"
#include "shared-bindings/rclcpy/Node.h"
#include "shared-bindings/rclcpy/Publisher.h"

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

static mp_obj_t rclcpy_init(void) {
    common_hal_rclcpy_init();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(rclcpy_init_obj, rclcpy_init);


// TODO: parallel implementation to Node constructor
// static mp_obj_t rclcpy_create_node(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
// }

static const mp_rom_map_elem_t rclcpy_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rclcpy) },
    { MP_ROM_QSTR(MP_QSTR_Node),   MP_ROM_PTR(&rclcpy_node_type) },
    { MP_ROM_QSTR(MP_QSTR_Publisher),   MP_ROM_PTR(&rclcpy_publisher_type) },
    { MP_ROM_QSTR(MP_QSTR_init),   MP_ROM_PTR(&rclcpy_init_obj) },
};

static MP_DEFINE_CONST_DICT(rclcpy_module_globals, rclcpy_module_globals_table);

const mp_obj_module_t rclcpy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rclcpy_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rclcpy, rclcpy_module);
