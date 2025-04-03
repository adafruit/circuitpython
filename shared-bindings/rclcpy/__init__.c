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

static mp_obj_t rclcpy_init(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // allocate object
    rclcpy_context_obj_t *self = mp_obj_malloc_with_finaliser(rclcpy_context_obj_t, &rclcpy_context_type);

    // common hal generation and return
    common_hal_rclcpy_init(self);
    return mp_const_none;
}

// TODO: parallel implementation to Node constructor
// static mp_obj_t rclcpy_create_node(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
// }

static const mp_rom_map_elem_t rclcpy_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rclcpy) },
    { MP_ROM_QSTR(MP_QSTR_Node),   MP_ROM_PTR(&rclcpy_node_type) },
    { MP_ROM_QSTR(MP_QSTR_Publisher),   MP_ROM_PTR(&rclcpy_publisher_type) },
};

static MP_DEFINE_CONST_DICT(rclcpy_module_globals, rclcpy_module_globals_table);

const mp_obj_module_t rclcpy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rclcpy_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rclcpy, rclcpy_module);
