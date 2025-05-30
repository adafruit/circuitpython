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

static mp_obj_t rclcpy_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_agent_ip, ARG_agent_port, ARG_domain_id};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_agent_ip, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_agent_port, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_domain_id, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    const char *agent_ip = mp_obj_str_get_str(args[ARG_agent_ip].u_obj);
    const char *agent_port = mp_obj_str_get_str(args[ARG_agent_port].u_obj);
    int16_t domain_id = args[ARG_domain_id].u_int;

    common_hal_rclcpy_init(agent_ip, agent_port, domain_id);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(rclcpy_init_obj, 2, rclcpy_init);

static mp_obj_t rclcpy_create_node(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_node_name, ARG_namespace };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_node_name, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_namespace, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    const char *node_name = mp_obj_str_get_str(args[ARG_node_name].u_obj);
    const char *namespace = "";
    if (args[ARG_namespace].u_obj != mp_const_none) {
        namespace = mp_obj_str_get_str(args[ARG_namespace].u_obj);
    }

    rclcpy_node_obj_t *self = mp_obj_malloc_with_finaliser(rclcpy_node_obj_t, &rclcpy_node_type);
    common_hal_rclcpy_node_construct(self, node_name, namespace);
    return (mp_obj_t)self;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(rclcpy_create_node_obj, 2, rclcpy_create_node);

static const mp_rom_map_elem_t rclcpy_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rclcpy) },
    { MP_ROM_QSTR(MP_QSTR_Node),   MP_ROM_PTR(&rclcpy_node_type) },
    { MP_ROM_QSTR(MP_QSTR_Publisher),   MP_ROM_PTR(&rclcpy_publisher_type) },
    { MP_ROM_QSTR(MP_QSTR_init),   MP_ROM_PTR(&rclcpy_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_node),   MP_ROM_PTR(&rclcpy_create_node_obj) },
};

static MP_DEFINE_CONST_DICT(rclcpy_module_globals, rclcpy_module_globals_table);

const mp_obj_module_t rclcpy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rclcpy_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rclcpy, rclcpy_module);
