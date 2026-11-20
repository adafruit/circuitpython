// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/zephyr_display/Display.h"

#include <zephyr/devicetree.h>

#include "bindings/zephyr_kernel/__init__.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"

//| class Display:
//|     """Manage the display selected by the Zephyr devicetree
//|
//|     There is no display bus argument because the device comes from the
//|     ``zephyr,display`` chosen devicetree node.
//|
//|     Creating a `Display` releases the display built during startup
//|     (``supervisor.runtime.display``), as with other display classes."""
//|
// The current implementation composites with LVGL, which drives only one display and draws
// from its own fixed-size heap (CONFIG_LV_Z_MEM_POOL_SIZE). Those are implementation details,
// so they stay out of the docs above, but they explain why only one Display can exist and why
// complex scenes can fail inside the draw code instead of raising MemoryError.
//|     def __init__(self, *, rotation: int = 0, auto_refresh: bool = True) -> None:
//|         """Create a `Display` for the ``zephyr,display`` device.
//|
//|         :param int rotation: The rotation of the display in degrees. Must be a multiple
//|             of 90.
//|         :param bool auto_refresh: Automatically refresh the screen.
//|         """
//|         ...
//|
static mp_obj_t zephyr_display_display_make_new(const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *all_args) {
    enum { ARG_rotation, ARG_auto_refresh };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_rotation, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_auto_refresh, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_bool = true } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t rotation = args[ARG_rotation].u_int;
    if (rotation % 90 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Display rotation must be in 90 degree increments"));
    }

    const struct device *display_dev = device_get_binding(DEVICE_DT_NAME(DT_CHOSEN(zephyr_display)));
    if (display_dev == NULL || !device_is_ready(display_dev)) {
        raise_zephyr_error(-ENODEV);
    }

    // The display device is a singleton and LVGL drives only one display at a time, so the one
    // board_init() made has to be released first (as with any other display class).
    primary_display_t *display_obj = allocate_display_or_raise();
    zephyr_display_display_obj_t *self = &display_obj->zephyr_display;
    self->base.type = &zephyr_display_display_type;
    common_hal_zephyr_display_display_construct_from_device(self, display_dev, rotation,
        args[ARG_auto_refresh].u_bool);
    return MP_OBJ_FROM_PTR(self);
}

// Helper to ensure we have the native super class instead of a subclass.
// Raises if the display has been released.
static zephyr_display_display_obj_t *native_display(mp_obj_t display_obj) {
    mp_obj_t native = mp_obj_cast_to_native_base(display_obj, &zephyr_display_display_type);
    if (native == MP_OBJ_NULL) {
        raise_deinited_error();
    }
    mp_obj_assert_native_inited(native);
    zephyr_display_display_obj_t *self = MP_OBJ_TO_PTR(native);
    if (self->base.type != &zephyr_display_display_type) {
        raise_deinited_error();
    }
    return self;
}

//|     def deinit(self) -> None:
//|         """Release the display. A new `Display` can then be created.
//|
//|         To release the display built during startup, use
//|         ``supervisor.runtime.display.deinit()``.
//|         As of CircuitPython 11.0.0, this replaces `displayio.release_displays()`, which is deprecated.
//|         """
//|         ...
//|
static mp_obj_t zephyr_display_display_obj_deinit(mp_obj_t self_in) {
    mp_obj_t native = mp_obj_cast_to_native_base(self_in, &zephyr_display_display_type);
    if (native != MP_OBJ_NULL) {
        common_hal_displayio_deinit_display(native);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_display_deinit_obj, zephyr_display_display_obj_deinit);

static mp_obj_t zephyr_display_display_obj_show(mp_obj_t self_in, mp_obj_t group_in) {
    (void)self_in;
    (void)group_in;
    mp_raise_AttributeError(MP_ERROR_TEXT(".show(x) removed. Use .root_group = x"));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(zephyr_display_display_show_obj, zephyr_display_display_obj_show);

//|     def refresh(
//|         self,
//|         *,
//|         target_frames_per_second: Optional[int] = None,
//|         minimum_frames_per_second: int = 0,
//|     ) -> bool:
//|         """When auto_refresh is off, and ``target_frames_per_second`` is not `None` this
//|         waits for the target frame rate and then refreshes the display, returning
//|         `True`. If the call has taken too long since the last refresh call for the given
//|         target frame rate, then the refresh returns `False` immediately without updating
//|         the screen to try to keep latency low.
//|
//|         If the time since the last successful refresh is below the minimum frame rate,
//|         then an exception will be raised. The default ``minimum_frames_per_second`` of 0
//|         disables this behavior.
//|
//|         When auto_refresh is off, and ``target_frames_per_second`` is `None` this will
//|         update the display immediately. When auto_refresh is on, it updates the display
//|         immediately too; the display also updates without calls to this.
//|
//|         :param Optional[int] target_frames_per_second: The target frame rate that
//|             :py:func:`refresh` should try to achieve. Set to `None` for immediate
//|             refresh.
//|         :param int minimum_frames_per_second: The minimum number of times the screen
//|             should be updated per second.
//|         """
//|         ...
//|
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

//|     auto_refresh: bool
//|     """True when the display is refreshed automatically."""
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

//|     brightness: float
//|     """The brightness of the display as a float. 0.0 is off and 1.0 is full brightness.
//|
//|     .. note:: Zephyr displays do not implement brightness control yet, so reading or
//|         writing this raises `RuntimeError`."""
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

//|     width: int
//|     """The width of the display in pixels, with rotation applied."""
static mp_obj_t zephyr_display_display_obj_get_width(mp_obj_t self_in) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_zephyr_display_display_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_display_get_width_obj, zephyr_display_display_obj_get_width);
MP_PROPERTY_GETTER(zephyr_display_display_width_obj, (mp_obj_t)&zephyr_display_display_get_width_obj);

//|     height: int
//|     """The height of the display in pixels, with rotation applied."""
static mp_obj_t zephyr_display_display_obj_get_height(mp_obj_t self_in) {
    zephyr_display_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_zephyr_display_display_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_display_display_get_height_obj, zephyr_display_display_obj_get_height);
MP_PROPERTY_GETTER(zephyr_display_display_height_obj, (mp_obj_t)&zephyr_display_display_get_height_obj);

//|     rotation: int
//|     """The rotation of the display in degrees."""
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

//|     root_group: Optional[displayio.Group]
//|     """The `displayio.Group` that is drawn on the display, or `None` when the display
//|     is cleared.
//|
//|     A `displayio.Group` can only be the root group of one display at a time. Setting a
//|     group that is already in use raises `ValueError`."""
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
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&zephyr_display_display_deinit_obj) },
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
