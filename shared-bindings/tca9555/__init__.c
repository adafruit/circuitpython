/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Christopher Parrott for Pimoroni
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/runtime.h"

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/tca9555/__init__.h"
#include "shared-module/tca9555/__init__.h"
#include "shared-bindings/busio/I2C.h"

//| class TcaPin:
//|     """A class that represents a GPIO pin attached to a TCA9555 IO expander chip.
//|
//|     Cannot be constructed at runtime, but may be the type of a pin object
//|     in :py:mod:`board`. A `TcaPin` can be used as a DigitalInOut, but not with other
//|     peripherals such as `PWMOut`."""
//|

const mp_obj_type_t tca_pin_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_TcaPin,
    .print = shared_bindings_tca9555_pin_print,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = mp_generic_unary_op,
        )
};

const mcu_pin_obj_t *validate_obj_is_pin_including_tca(mp_obj_t obj, qstr arg_name) {
    return MP_OBJ_TO_PTR(mp_arg_validate_type_or_type(obj, &mcu_pin_type, &tca_pin_type, arg_name));
}

const mcu_pin_obj_t *validate_obj_is_free_pin_including_tca(mp_obj_t obj, qstr arg_name) {
    const mcu_pin_obj_t *pin = validate_obj_is_pin_including_tca(obj, arg_name);
    assert_pin_free(pin);
    return pin;
}

// Validate that the obj is a free pin or None. Return an mcu_pin_obj_t* or NULL, correspondingly.
const mcu_pin_obj_t *validate_obj_is_free_pin_including_tca_or_none(mp_obj_t obj, qstr arg_name) {
    if (obj == mp_const_none) {
        return NULL;
    }
    return validate_obj_is_free_pin_including_tca(obj, arg_name);
}

//| def get_number(pin: TcaPin) -> int:
//|     """Get the TCA9555 pin number (from 0 to 15) of the provided TcaPin.
//|
//|     :param TcaPin pin: a TCA pin object"""
//|     ...
//|
STATIC mp_obj_t tca_pin_get_number(mp_obj_t pin_obj) {
    mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(mp_arg_validate_type(pin_obj, &tca_pin_type, MP_QSTR_pin));
    uint8_t tca_gpio = mp_arg_validate_index_range(pin->number, 0, TCA9555_VIRTUAL_GPIO_COUNT - 1, MP_QSTR_pin_number);

    return mp_obj_new_int(tca_gpio % TCA9555_GPIO_COUNT);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_pin_get_number_obj, tca_pin_get_number);

//| def get_chip(pin: TcaPin) -> int:
//|     """Get the TCA9555 chip number of the provided TcaPin.
//|
//|     :param TcaPin pin: a TCA pin object"""
//|     ...
//|
STATIC mp_obj_t tca_pin_get_chip(mp_obj_t pin_obj) {
    mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(mp_arg_validate_type(pin_obj, &tca_pin_type, MP_QSTR_pin));
    uint8_t tca_gpio = mp_arg_validate_index_range(pin->number, 0, TCA9555_VIRTUAL_GPIO_COUNT - 1, MP_QSTR_pin_number);

    return mp_obj_new_int(CHIP_FROM_GPIO(tca_gpio));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_pin_get_chip_obj, tca_pin_get_chip);

//| def change_output_mask(chip: int, mask: int, state: int) -> None:
//|     """Change the output state of a set of pins on a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to use
//|     :param int mask: which of the 16 pins on the chip to modify
//|     :param int state: the state the masked pins should have"""
//|     ...
//|
STATIC mp_obj_t tca_pin_change_output_mask(mp_obj_t chip_obj, mp_obj_t mask_obj, mp_obj_t state_obj) {
    uint8_t chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    uint16_t mask = mp_arg_validate_int_range(mp_obj_get_int(mask_obj), 0, UINT16_MAX, MP_QSTR_mask);
    uint16_t state = mp_arg_validate_int_range(mp_obj_get_int(state_obj), 0, UINT16_MAX, MP_QSTR_state);

    common_hal_tca_change_output_mask(chip, mask, state);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tca_pin_change_output_mask_obj, tca_pin_change_output_mask);

//| def change_config_mask(chip: int, mask: int, state: int) -> None:
//|     """Change the config state of a set of pins on a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to use
//|     :param int mask: which of the 16 pins on the chip to change the direction of
//|     :param int state: the directions the masked pins should have"""
//|     ...
//|
STATIC mp_obj_t tca_pin_change_config_mask(mp_obj_t chip_obj, mp_obj_t mask_obj, mp_obj_t state_obj) {
    uint8_t chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    uint16_t mask = mp_arg_validate_int_range(mp_obj_get_int(mask_obj), 0, UINT16_MAX, MP_QSTR_mask);
    uint16_t state = mp_arg_validate_int_range(mp_obj_get_int(state_obj), 0, UINT16_MAX, MP_QSTR_state);

    common_hal_tca_change_config_mask(chip, mask, state);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tca_pin_change_config_mask_obj, tca_pin_change_config_mask);

//| def change_polarity_mask(chip: int, mask: int, state: int) -> None:
//|     """Change the output polarity state of a set of pins on a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to use
//|     :param int mask: which of the 16 pins on the chip to modify
//|     :param int state: the polarity state the masked pins should have"""
//|     ...
//|
STATIC mp_obj_t tca_pin_change_polarity_mask(mp_obj_t chip_obj, mp_obj_t mask_obj, mp_obj_t state_obj) {
    uint8_t chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    uint16_t mask = mp_arg_validate_int_range(mp_obj_get_int(mask_obj), 0, UINT16_MAX, MP_QSTR_mask);
    uint16_t state = mp_arg_validate_int_range(mp_obj_get_int(state_obj), 0, UINT16_MAX, MP_QSTR_state);

    common_hal_tca_change_polarity_mask(chip, mask, state);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tca_pin_change_polarity_mask_obj, tca_pin_change_polarity_mask);

#if TCA9555_READ_INTERNALS
//| def read_input(chip: int) -> int:
//|     """Read the input state of a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to read"""
//|     ...
//|
STATIC mp_obj_t tca_port_read_input_state(mp_obj_t chip_obj) {
    uint chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    return mp_obj_new_int(common_hal_tca_get_input_port(chip));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_read_input_state_obj, tca_port_read_input_state);

//| def read_output(chip: int) -> int:
//|     """Read the output state of a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to read"""
//|     ...
//|
STATIC mp_obj_t tca_port_read_output_state(mp_obj_t chip_obj) {
    uint chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    return mp_obj_new_int(common_hal_tca_get_output_port(chip));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_read_output_state_obj, tca_port_read_output_state);

//| def read_config(chip: int) -> int:
//|     """Read the config state of a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to read"""
//|     ...
//|
STATIC mp_obj_t tca_port_read_config_state(mp_obj_t chip_obj) {
    uint chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    return mp_obj_new_int(common_hal_tca_get_config_port(chip));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_read_config_state_obj, tca_port_read_config_state);

//| def read_polarity(chip: int) -> int:
//|     """Read the polarity state of a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to read"""
//|     ...
//|
STATIC mp_obj_t tca_port_read_polarity_state(mp_obj_t chip_obj) {
    uint chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    return mp_obj_new_int(common_hal_tca_get_polarity_port(chip));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_read_polarity_state_obj, tca_port_read_polarity_state);

#if TCA9555_LOCAL_MEMORY
//| def stored_output(chip: int) -> int:
//|     """Read the stored output state of a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to read the stored state of"""
//|     ...
//|
STATIC mp_obj_t tca_port_stored_output_state(mp_obj_t chip_obj) {
    uint chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    return mp_obj_new_int((tca9555_output_state[HIGH_BYTE(chip)] << 8) | tca9555_output_state[LOW_BYTE(chip)]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_stored_output_state_obj, tca_port_stored_output_state);

//| def stored_config(chip: int) -> int:
//|     """Read the stored config state of a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to read the stored state of"""
//|     ...
//|
STATIC mp_obj_t tca_port_stored_config_state(mp_obj_t chip_obj) {
    uint chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    return mp_obj_new_int((tca9555_config_state[HIGH_BYTE(chip)] << 8) | tca9555_config_state[LOW_BYTE(chip)]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_stored_config_state_obj, tca_port_stored_config_state);

//| def stored_polarity(chip: int) -> int:
//|     """Read the stored polarity state of a single TCA9555 chip.
//|
//|     :param int chip: the number of the TCA9555 chip to read the stored state of"""
//|     ...
//|
STATIC mp_obj_t tca_port_stored_polarity_state(mp_obj_t chip_obj) {
    uint chip = mp_arg_validate_int_range(mp_obj_get_int(chip_obj), 0, TCA9555_CHIP_COUNT - 1, MP_QSTR_chip);
    return mp_obj_new_int((tca9555_polarity_state[HIGH_BYTE(chip)] << 8) | tca9555_polarity_state[LOW_BYTE(chip)]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_stored_polarity_state_obj, tca_port_stored_polarity_state);
#endif
#endif

#define TCA_PIN(exp_number, p_number) \
    const mcu_pin_obj_t pin_TCA##exp_number##_##p_number = { \
        { &tca_pin_type }, \
        .number = p_number + (exp_number << 4) \
    }

#define TCA_PINS(exp_number) \
    TCA_PIN(exp_number, 0); \
    TCA_PIN(exp_number, 1); \
    TCA_PIN(exp_number, 2); \
    TCA_PIN(exp_number, 3); \
    TCA_PIN(exp_number, 4); \
    TCA_PIN(exp_number, 5); \
    TCA_PIN(exp_number, 6); \
    TCA_PIN(exp_number, 7); \
    TCA_PIN(exp_number, 8); \
    TCA_PIN(exp_number, 9); \
    TCA_PIN(exp_number, 10); \
    TCA_PIN(exp_number, 11); \
    TCA_PIN(exp_number, 12); \
    TCA_PIN(exp_number, 13); \
    TCA_PIN(exp_number, 14); \
    TCA_PIN(exp_number, 15)

#if CIRCUITPY_TCA9555
TCA_PINS(0);
TCA_PINS(1);
#endif

#define TCA_ENTRY(exp_number, p_number) \
    { MP_ROM_QSTR(MP_QSTR_TCA##exp_number##_##p_number), MP_ROM_PTR(&pin_TCA##exp_number##_##p_number) }

#define TCA_ENTRIES(exp_number) \
    TCA_ENTRY(exp_number, 0), \
    TCA_ENTRY(exp_number, 1), \
    TCA_ENTRY(exp_number, 2), \
    TCA_ENTRY(exp_number, 3), \
    TCA_ENTRY(exp_number, 4), \
    TCA_ENTRY(exp_number, 5), \
    TCA_ENTRY(exp_number, 6), \
    TCA_ENTRY(exp_number, 7), \
    TCA_ENTRY(exp_number, 8), \
    TCA_ENTRY(exp_number, 9), \
    TCA_ENTRY(exp_number, 10), \
    TCA_ENTRY(exp_number, 11), \
    TCA_ENTRY(exp_number, 12), \
    TCA_ENTRY(exp_number, 13), \
    TCA_ENTRY(exp_number, 14), \
    TCA_ENTRY(exp_number, 15)

STATIC const mp_rom_map_elem_t tca_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tca) },
    { MP_ROM_QSTR(MP_QSTR_TcaPin), MP_ROM_PTR(&tca_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_get_number), &tca_pin_get_number_obj },
    { MP_ROM_QSTR(MP_QSTR_get_chip), &tca_pin_get_chip_obj },
    { MP_ROM_QSTR(MP_QSTR_change_output_mask), &tca_pin_change_output_mask_obj },
    { MP_ROM_QSTR(MP_QSTR_change_config_mask), &tca_pin_change_config_mask_obj },
    { MP_ROM_QSTR(MP_QSTR_change_polarity_mask), &tca_pin_change_polarity_mask_obj },
    #if TCA9555_READ_INTERNALS
    { MP_ROM_QSTR(MP_QSTR_read_input), &tca_port_read_input_state_obj },
    { MP_ROM_QSTR(MP_QSTR_read_output), &tca_port_read_output_state_obj },
    { MP_ROM_QSTR(MP_QSTR_read_config), &tca_port_read_config_state_obj },
    { MP_ROM_QSTR(MP_QSTR_read_polarity), &tca_port_read_polarity_state_obj },
    #if TCA9555_LOCAL_MEMORY
    { MP_ROM_QSTR(MP_QSTR_stored_output), &tca_port_stored_output_state_obj },
    { MP_ROM_QSTR(MP_QSTR_stored_config), &tca_port_stored_config_state_obj },
    { MP_ROM_QSTR(MP_QSTR_stored_polarity), &tca_port_stored_polarity_state_obj },
    #endif
    #endif
    TCA_ENTRIES(0),
    TCA_ENTRIES(1)
};

MP_DEFINE_CONST_DICT(tca_module_globals, tca_module_globals_table);

const mp_obj_module_t tca_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&tca_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_tca, tca_module);

static void get_pin_name(const mcu_pin_obj_t *self, qstr *package, qstr *module, qstr *name) {
    const mp_map_t *board_map = &board_module_globals.map;
    for (uint8_t i = 0; i < board_map->alloc; i++) {
        if (board_map->table[i].value == MP_OBJ_FROM_PTR(self)) {
            *package = 0;
            *module = MP_QSTR_board;
            *name = MP_OBJ_QSTR_VALUE(board_map->table[i].key);
            return;
        }
    }
    const mp_map_t *tca_map = &tca_module_globals.map;
    for (uint8_t i = 0; i < tca_map->alloc; i++) {
        if (tca_map->table[i].value == MP_OBJ_FROM_PTR(self)) {
            *package = 0;
            *module = MP_QSTR_tca;
            *name = MP_OBJ_QSTR_VALUE(tca_map->table[i].key);
            return;
        }
    }
}

void shared_bindings_tca9555_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mcu_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    qstr package = MP_QSTR_Pin;
    qstr module = MP_QSTR_pin;
    qstr name = MP_QSTR_Pin;

    get_pin_name(self, &package, &module, &name);
    if (package) {
        mp_printf(print, "%q.%q.%q", package, module, name);
    } else {
        mp_printf(print, "%q.%q", module, name);
    }
}
