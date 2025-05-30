// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "shared-bindings/rclcpy/Publisher.h"
#include "shared-bindings/util.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"

static mp_obj_t rclcpy_publisher_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Publishers can only be created from a parent node"));
}

static mp_obj_t rclcpy_publisher_obj_deinit(mp_obj_t self_in) {
    rclcpy_publisher_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_rclcpy_publisher_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rclcpy_publisher_deinit_obj, rclcpy_publisher_obj_deinit);

static void check_for_deinit(rclcpy_publisher_obj_t *self) {
    if (common_hal_rclcpy_publisher_deinited(self)) {
        raise_deinited_error();
    }
}

static mp_obj_t rclcpy_publisher_publish_int32(mp_obj_t self_in, mp_obj_t in_msg) {
    rclcpy_publisher_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    int32_t msg = mp_obj_get_int(in_msg);
    common_hal_rclcpy_publisher_publish_int32(self,msg);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(rclcpy_publisher_publish_int32_obj, rclcpy_publisher_publish_int32);

static mp_obj_t rclcpy_publisher_get_topic_name(mp_obj_t self_in) {
    rclcpy_publisher_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    const char * topic_str = common_hal_rclcpy_publisher_get_topic_name(self);
    return mp_obj_new_str(topic_str,strlen(topic_str));
}
static MP_DEFINE_CONST_FUN_OBJ_1(rclcpy_publisher_get_topic_name_obj, rclcpy_publisher_get_topic_name);


static const mp_rom_map_elem_t rclcpy_publisher_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&rclcpy_publisher_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&rclcpy_publisher_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_publish_int32), MP_ROM_PTR(&rclcpy_publisher_publish_int32_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_topic_name), MP_ROM_PTR(&rclcpy_publisher_get_topic_name_obj) },
};
static MP_DEFINE_CONST_DICT(rclcpy_publisher_locals_dict, rclcpy_publisher_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    rclcpy_publisher_type,
    MP_QSTR_Publisher,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, rclcpy_publisher_make_new,
    locals_dict, &rclcpy_publisher_locals_dict
);
