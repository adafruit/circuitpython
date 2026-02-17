// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/zephyr_display/Display.h"

#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-module/displayio/__init__.h"

static mp_obj_t zephyr_display_display_make_new(const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *all_args) {
    (void)type;
    (void)n_args;
    (void)n_kw;
    (void)all_args;
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Use board.DISPLAY"));
    return mp_const_none;
}

static zephyr_display_display_obj_t *native_display(mp_obj_t display_obj) {
    mp_obj_t native = mp_obj_cast_to_native_base(display_obj, &zephyr_display_display_type);
    mp_obj_assert_native_inited(native);
    return MP_OBJ_TO_PTR(native);
}

static mp_obj_t zephyr_display_display_obj_show(mp_obj_t self_in, mp_obj_t group_in) {
    (void)self_in;
    (void)group_in;
    mp_raise_AttributeError(MP_ERROR_TEXT(".show(x) removed. Use .root_group = x"));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_display_show_obj, zephyr_display_display_obj_show);

static mp_obj_t zephyr_display_display_obj_refresh(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_target_frames_per_second,
        ARG_minimum_frames_per_second,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_target_frames_per_second, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_minimum_frames_per_second, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    zephyr_display_display_obj_t *self = native_display(pos_args[0]);

    uint32_t maximum_ms_per_real_frame = NO_FPS_LIMIT;
    mp_int_t minimum_frames_per_second = args[ARG_minimum_frames_per_second].u_int;
    if (minimum_frames_per_second > 0) {
        maximum_ms_per_real_frame = 1000 / minimum_frames_per_second;
    }

    uint32_t target_ms_per_frame;
    if (args[ARG_target_frames_per_second].u_obj == mp_const_none) {
        target_ms_per_frame = NO_FPS_LIMIT;
    } else {
        target_ms_per_frame = 1000 / mp_obj_get_int(args[ARG_target_frames_per_second].u_obj);
    }

    return mp_obj_new_bool(common_hal_zephyr_display_display_refresh(
        self,
        target_ms_per_frame,
        maximum_ms_per_real_frame));
}
MP_DEFINE_CONST_FUN_OBJ_KW(zephyr_display_display_refresh_obj, 1, zephyr_display_display_obj_refresh);

static mp_obj_t zephyr_display_display_obj_get_auto_refresh(mp_obj_t self_in) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    return mp_obj_new_bool(common_hal_zephyr_display_display_get_auto_refresh(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_display_get_auto_refresh_obj, zephyr_display_display_obj_get_auto_refresh);

static mp_obj_t zephyr_display_display_obj_set_auto_refresh(mp_obj_t self_in, mp_obj_t auto_refresh) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    common_hal_zephyr_display_display_set_auto_refresh(self, mp_obj_is_true(auto_refresh));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_display_set_auto_refresh_obj, zephyr_display_display_obj_set_auto_refresh);

MP_PROPERTY_GETSET(zephyr_display_display_auto_refresh_obj,
    (mp_obj_t)&zephyr_display_display_get_auto_refresh_obj,
    (mp_obj_t)&zephyr_display_display_set_auto_refresh_obj);

static mp_obj_t zephyr_display_display_obj_get_brightness(mp_obj_t self_in) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    mp_float_t brightness = common_hal_zephyr_display_display_get_brightness(self);
    if (brightness < 0) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Brightness not adjustable"));
    }
    return mp_obj_new_float(brightness);
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_display_get_brightness_obj, zephyr_display_display_obj_get_brightness);

static mp_obj_t zephyr_display_display_obj_set_brightness(mp_obj_t self_in, mp_obj_t brightness_obj) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    mp_float_t brightness = mp_obj_get_float(brightness_obj);
    if (brightness < 0.0f || brightness > 1.0f) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be %d-%d"), MP_QSTR_brightness, 0, 1);
    }
    bool ok = common_hal_zephyr_display_display_set_brightness(self, brightness);
    if (!ok) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Brightness not adjustable"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_display_set_brightness_obj, zephyr_display_display_obj_set_brightness);

MP_PROPERTY_GETSET(zephyr_display_display_brightness_obj,
    (mp_obj_t)&zephyr_display_display_get_brightness_obj,
    (mp_obj_t)&zephyr_display_display_set_brightness_obj);

static mp_obj_t zephyr_display_display_obj_get_width(mp_obj_t self_in) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_zephyr_display_display_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_display_get_width_obj, zephyr_display_display_obj_get_width);
MP_PROPERTY_GETTER(zephyr_display_display_width_obj, (mp_obj_t)&zephyr_display_display_get_width_obj);

static mp_obj_t zephyr_display_display_obj_get_height(mp_obj_t self_in) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_zephyr_display_display_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_display_get_height_obj, zephyr_display_display_obj_get_height);
MP_PROPERTY_GETTER(zephyr_display_display_height_obj, (mp_obj_t)&zephyr_display_display_get_height_obj);

static mp_obj_t zephyr_display_display_obj_get_rotation(mp_obj_t self_in) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_zephyr_display_display_get_rotation(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_display_get_rotation_obj, zephyr_display_display_obj_get_rotation);

static mp_obj_t zephyr_display_display_obj_set_rotation(mp_obj_t self_in, mp_obj_t value) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    common_hal_zephyr_display_display_set_rotation(self, mp_obj_get_int(value));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_display_set_rotation_obj, zephyr_display_display_obj_set_rotation);

MP_PROPERTY_GETSET(zephyr_display_display_rotation_obj,
    (mp_obj_t)&zephyr_display_display_get_rotation_obj,
    (mp_obj_t)&zephyr_display_display_set_rotation_obj);

static mp_obj_t zephyr_display_display_obj_get_root_group(mp_obj_t self_in) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    return common_hal_zephyr_display_display_get_root_group(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_display_get_root_group_obj, zephyr_display_display_obj_get_root_group);

static mp_obj_t zephyr_display_display_obj_set_root_group(mp_obj_t self_in, mp_obj_t group_in) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    displayio_group_t *group = NULL;
    if (group_in != mp_const_none) {
        group = MP_OBJ_TO_PTR(native_group(group_in));
    }

    bool ok = common_hal_zephyr_display_display_set_root_group(self, group);
    if (!ok) {
        mp_raise_ValueError(MP_ERROR_TEXT("Group already used"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_display_set_root_group_obj, zephyr_display_display_obj_set_root_group);

MP_PROPERTY_GETSET(zephyr_display_display_root_group_obj,
    (mp_obj_t)&zephyr_display_display_get_root_group_obj,
    (mp_obj_t)&zephyr_display_display_set_root_group_obj);

static const mp_rom_map_elem_t zephyr_display_display_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&zephyr_display_display_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&zephyr_display_display_refresh_obj) },

    { MP_ROM_QSTR(MP_QSTR_auto_refresh), MP_ROM_PTR(&zephyr_display_display_auto_refresh_obj) },
    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&zephyr_display_display_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&zephyr_display_display_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&zephyr_display_display_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&zephyr_display_display_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_root_group), MP_ROM_PTR(&zephyr_display_display_root_group_obj) },
};
static MP_DEFINE_CONST_DICT(zephyr_display_display_locals_dict, zephyr_display_display_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_display_display_type,
    MP_QSTR_Display,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, zephyr_display_display_make_new,
    locals_dict, &zephyr_display_display_locals_dict);
