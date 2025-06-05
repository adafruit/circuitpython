// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "shared-bindings/rclcpy/Node.h"
#include "shared-bindings/rclcpy/Publisher.h"
#include "shared-bindings/util.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"


//| class Node:
//|     """A ROS2 Node"""
//|
//|     def __init__(
//|         self,
//|         node_name: str,
//|         *,
//|         namespace: str | None = None,
//|     ) -> None:
//|         """Create a Node.
//|
//|         Creates an instance of a ROS2 Node. Nodes can be used to create other ROS
//|         entities like publishers or subscribers. Nodes must have a unique name, and
//|         may also be constructed from their class.
//|
//|         :param str node_name: The name of the node. Must be a valid ROS 2 node name
//|         :param str namespace: The namespace for the node. If None, the node will be
//|             created in the root namespace
//|         """
//|         ...
//|
static mp_obj_t rclcpy_node_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_node_name, ARG_namespace };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_node_name, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_namespace, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    const char *node_name = mp_obj_str_get_str(args[ARG_node_name].u_obj);
    const char *namespace = "";
    if (args[ARG_namespace].u_obj != mp_const_none) {
        namespace = mp_obj_str_get_str(args[ARG_namespace].u_obj);
    }

    rclcpy_node_obj_t *self = mp_obj_malloc_with_finaliser(rclcpy_node_obj_t, &rclcpy_node_type);
    common_hal_rclcpy_node_construct(self, node_name, namespace);
    return (mp_obj_t)self;
}

//|     def deinit(self) -> None:
//|         """Deinitializes the node and frees any hardware or remote agent resources
//|         used by it. Deinitialized nodes cannot be used again.
//|         """
//|         ...
//|
static mp_obj_t rclcpy_node_obj_deinit(mp_obj_t self_in) {
    rclcpy_node_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_rclcpy_node_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rclcpy_node_deinit_obj, rclcpy_node_obj_deinit);

static void check_for_deinit(rclcpy_node_obj_t *self) {
    if (common_hal_rclcpy_node_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def create_publisher(self, topic: str) -> Publisher:
//|         """Create a publisher for a given topic string.
//|
//|         Creates an instance of a ROS2 Publisher.
//|
//|         :param str topic: The name of the topic
//|         :return: A new Publisher object for the specified topic
//|         :rtype: Publisher
//|         """
//|         ...
//|
static mp_obj_t rclcpy_node_create_publisher(mp_obj_t self_in, mp_obj_t topic) {
    rclcpy_node_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    const char *topic_name = mp_obj_str_get_str(topic);

    rclcpy_publisher_obj_t *publisher = mp_obj_malloc_with_finaliser(rclcpy_publisher_obj_t, &rclcpy_publisher_type);
    common_hal_rclcpy_publisher_construct(publisher, self, topic_name);
    return (mp_obj_t)publisher;
}
static MP_DEFINE_CONST_FUN_OBJ_2(rclcpy_node_create_publisher_obj, rclcpy_node_create_publisher);

//|     def get_name(self) -> str:
//|         """Get the name of the node.
//|
//|         :return: The node's name
//|         :rtype: str
//|         """
//|         ...
//|
static mp_obj_t rclcpy_node_get_name(mp_obj_t self_in) {
    rclcpy_node_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    const char *name_str = common_hal_rclcpy_node_get_name(self);
    return mp_obj_new_str(name_str, strlen(name_str));
}
static MP_DEFINE_CONST_FUN_OBJ_1(rclcpy_node_get_name_obj, rclcpy_node_get_name);

//|     def get_namespace(self) -> str:
//|         """Get the namespace of the node.
//|
//|         :return: The node's namespace
//|         :rtype: str
//|         """
//|         ...
//|
static mp_obj_t rclcpy_node_get_namespace(mp_obj_t self_in) {
    rclcpy_node_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    const char *namespace_str = common_hal_rclcpy_node_get_namespace(self);
    return mp_obj_new_str(namespace_str, strlen(namespace_str));
}
static MP_DEFINE_CONST_FUN_OBJ_1(rclcpy_node_get_namespace_obj, rclcpy_node_get_namespace);

static const mp_rom_map_elem_t rclcpy_node_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&rclcpy_node_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&rclcpy_node_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_publisher), MP_ROM_PTR(&rclcpy_node_create_publisher_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_name), MP_ROM_PTR(&rclcpy_node_get_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_namespace), MP_ROM_PTR(&rclcpy_node_get_namespace_obj) },
};
static MP_DEFINE_CONST_DICT(rclcpy_node_locals_dict, rclcpy_node_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    rclcpy_node_type,
    MP_QSTR_Node,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, rclcpy_node_make_new,
    locals_dict, &rclcpy_node_locals_dict
    );
