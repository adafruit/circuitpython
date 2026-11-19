// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "zephyr/input/input.h"

typedef struct {
    int32_t x;
    int32_t y;
    bool pressed;
} zephyr_input_touch_state_t;
static zephyr_input_touch_state_t touch_state;
static zephyr_input_touch_state_t buffered_touch_state;

static void zephyr_input_callback(struct input_event *event, void *user_data) {
    (void)user_data;
    if (event->type == INPUT_EV_ABS && event->code == INPUT_ABS_X) {
        touch_state.x = event->value;
    } else if (event->type == INPUT_EV_ABS && event->code == INPUT_ABS_Y) {
        touch_state.y = event->value;
    } else if (event->type == INPUT_EV_KEY && event->code == INPUT_BTN_TOUCH) {
        touch_state.pressed = event->value != 0;
    }
    // Like the GT911 driver, retain the latest complete touch sample.
    if (event->sync) {
        buffered_touch_state = touch_state;
    }
}

INPUT_CALLBACK_DEFINE(NULL, zephyr_input_callback, NULL);

//| """Access input devices provided by Zephyr."""
//|
//| def touch() -> tuple[int, int] | None:
//|     """Return the latest buffered touch coordinates, or ``None`` when released.
//|
//|     On native_sim, SDL mouse button and motion events are reported as touch.
//|     """
//|     ...
static mp_obj_t zephyr_input_touch(void) {
    if (!buffered_touch_state.pressed) {
        return mp_const_none;
    }
    mp_obj_t coordinates[] = {
        mp_obj_new_int(buffered_touch_state.x),
        mp_obj_new_int(buffered_touch_state.y),
    };
    return mp_obj_new_tuple(2, coordinates);
}
static MP_DEFINE_CONST_FUN_OBJ_0(zephyr_input_touch_obj, zephyr_input_touch);

static const mp_rom_map_elem_t zephyr_input_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_zephyr_input) },
    { MP_ROM_QSTR(MP_QSTR_touch), MP_ROM_PTR(&zephyr_input_touch_obj) },
};
static MP_DEFINE_CONST_DICT(zephyr_input_module_globals, zephyr_input_module_globals_table);

const mp_obj_module_t zephyr_input_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&zephyr_input_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_zephyr_input, zephyr_input_module);
