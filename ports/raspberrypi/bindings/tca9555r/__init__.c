/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
 * Copyright (c) 2016 Scott Shawcroft
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
#include "bindings/tca9555r/__init__.h"
#include "shared-bindings/busio/I2C.h"

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_TCA9555R, Enable/disable assertions in the pico_tca9555r module, type=bool, default=0, group=pico_tca9555r
#ifndef PARAM_ASSERTIONS_ENABLED_TCA9555R
#define PARAM_ASSERTIONS_ENABLED_TCA9555R 0
#endif

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

void shared_bindings_tca9555r_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
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

//| class TcaPin:
//|     """A class that represents a GPIO pin attached to a TCA9555R IO expander chip.
//|
//|     Cannot be constructed at runtime, but may be the type of a pin object
//|     in :py:mod:`board`. A `TcaPin` can be used as a DigitalInOut, but not with other
//|     peripherals such as `PWMOut`."""
//|
const mp_obj_type_t tca_pin_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_TcaPin,
    .print = shared_bindings_tca9555r_pin_print,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = mp_generic_unary_op,
        )
};

const mcu_pin_obj_t *validate_obj_is_pin_including_tca(mp_obj_t obj, qstr arg_name) {
    if (!mp_obj_is_type(obj, &mcu_pin_type) && !mp_obj_is_type(obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q or %q, not %q"), arg_name, mcu_pin_type.name, tca_pin_type.name, mp_obj_get_type(obj)->name);
    }
    return MP_OBJ_TO_PTR(obj);
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

static const uint8_t tca9555r_addresses[TCA9555R_CHIP_COUNT] = TCA9555R_CHIP_ADDRESSES;

#define TCA9555R_GPIO_COUNT     16
#define TCA9555R_VIRTUAL_GPIO_COUNT     (TCA9555R_GPIO_COUNT * TCA9555R_CHIP_COUNT)

uint8_t tca9555r_output_state[TCA9555R_CHIP_COUNT * 2] = {0};
uint8_t tca9555r_config_state[TCA9555R_CHIP_COUNT * 2] = {0};

uint8_t tca_get_chip_from_pin(uint tca_gpio) {
    return tca_gpio / TCA9555R_GPIO_COUNT;
}

uint8_t tca_get_address_from_pin(uint tca_gpio) {
    uint8_t index = tca_gpio / TCA9555R_GPIO_COUNT;
    return tca9555r_addresses[index];
}

uint8_t tca_get_byte_from_pin(uint tca_gpio) {
    return (tca_gpio % TCA9555R_GPIO_COUNT) >> 3u;
}

uint8_t tca_pin_to_bit_mask(uint tca_gpio) {
    return 1u << (tca_gpio % 8u);
}

bool tca_gpio_get_input(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);
    tca_gpio = (tca_gpio % TCA9555R_GPIO_COUNT);

    uint8_t reg = (tca_gpio >= 8) ? INPUT_PORT1 : INPUT_PORT0;
    uint8_t input_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &input_state, 1);
    return (input_state & tca_pin_to_bit_mask(tca_gpio)) != 0;
}

bool tca_gpio_get_output(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);
    tca_gpio = (tca_gpio % TCA9555R_GPIO_COUNT);

    uint8_t reg = (tca_gpio >= 8) ? OUTPUT_PORT1 : OUTPUT_PORT0;
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &output_state, 1);
    tca9555r_output_state[tca_gpio / 8] = output_state;
    return (output_state & tca_pin_to_bit_mask(tca_gpio)) != 0;
}

void tca_gpio_set_output(uint tca_gpio, bool value) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);
    tca_gpio = (tca_gpio % TCA9555R_GPIO_COUNT);

    uint8_t reg = (tca_gpio >= 8) ? OUTPUT_PORT1 : OUTPUT_PORT0;
    uint8_t output_state = tca9555r_output_state[tca_gpio / 8];
    //common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &output_state, 1);
    uint8_t new_output_state;
    if (value) {
        new_output_state = output_state | tca_pin_to_bit_mask(tca_gpio);
    } else {
        new_output_state = output_state & ~tca_pin_to_bit_mask(tca_gpio);
    }

    if (new_output_state != output_state) {
        uint8_t reg_and_data[2] = { reg, new_output_state };
        common_hal_busio_i2c_write(i2c, address, reg_and_data, 2);
        tca9555r_output_state[tca_gpio / 8] = new_output_state;
    }
}

bool tca_gpio_get_dir(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);
    tca_gpio = (tca_gpio % TCA9555R_GPIO_COUNT);

    uint8_t reg = (tca_gpio >= 8) ? CONFIGURATION_PORT1 : CONFIGURATION_PORT0;
    uint8_t config_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &config_state, 1);
    tca9555r_config_state[tca_gpio / 8] = config_state;
    return (config_state & tca_pin_to_bit_mask(tca_gpio)) == 0;
}

void tca_gpio_set_dir(uint tca_gpio, bool output) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);
    tca_gpio = (tca_gpio % TCA9555R_GPIO_COUNT);

    uint8_t reg = (tca_gpio >= 8) ? CONFIGURATION_PORT1 : CONFIGURATION_PORT0;
    uint8_t config_state = tca9555r_config_state[tca_gpio / 8];
    //common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &config_state, 1);
    uint8_t new_config_state;
    if (output) {
        new_config_state = config_state & ~tca_pin_to_bit_mask(tca_gpio);
    } else {
        new_config_state = config_state | tca_pin_to_bit_mask(tca_gpio);
    }

    if (new_config_state != config_state) {
        uint8_t reg_and_data[2] = { reg, new_config_state };
        common_hal_busio_i2c_write(i2c, address, reg_and_data, 2);
        tca9555r_config_state[tca_gpio / 8] = new_config_state;
    }
}

uint16_t tca_gpio_get_input_port(uint tca_address) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = INPUT_PORT0;
    uint16_t input_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, (uint8_t *)&input_state, 2);
    return input_state;
}

uint16_t tca_gpio_get_output_port(uint tca_address) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT0;
    uint16_t output_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, (uint8_t *)&output_state, 2);
    return output_state;
}

uint8_t tca_gpio_get_low_output_port(uint tca_address) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT0;
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, &output_state, 1);
    return output_state;
}

uint8_t tca_gpio_get_high_output_port(uint tca_address) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT1;
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, &output_state, 1);
    return output_state;
}

void tca_gpio_set_output_port(uint tca_address, uint16_t output_state) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { OUTPUT_PORT0, output_state & 0xFF, (output_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca_address, reg_and_data, 3);
}

void tca_gpio_set_low_output_port(uint tca_address, uint8_t output_state) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { OUTPUT_PORT0, output_state };
    common_hal_busio_i2c_write(i2c, tca_address, reg_and_data, 2);
}

void tca_gpio_set_high_output_port(uint tca_address, uint8_t output_state) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { OUTPUT_PORT1, output_state };
    common_hal_busio_i2c_write(i2c, tca_address, reg_and_data, 2);
}

uint16_t tca_gpio_get_dir_port(uint tca_address) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = CONFIGURATION_PORT0;
    uint16_t config_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, (uint8_t *)&config_state, 2);
    return config_state;
}

void tca_gpio_set_dir_port(uint tca_address, uint16_t config_state) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { CONFIGURATION_PORT0, config_state & 0xFF, (config_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca_address, reg_and_data, 3);
}

uint16_t tca_gpio_get_polarity_port(uint tca_address) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = POLARITY_PORT0;
    uint16_t polarity_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, (uint8_t *)&polarity_state, 2);
    return polarity_state;
}

void tca_gpio_set_polarity_port(uint tca_address, uint16_t polarity_state) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { POLARITY_PORT0, polarity_state & 0xFF, (polarity_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca_address, reg_and_data, 3);
}

void tca_populate_mask(mp_obj_t pins, uint16_t *mask, qstr arg_name) {
    if (mp_obj_is_type(pins, &tca_pin_type)) {
        mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(pins);
        uint8_t tca_gpio = pin->number;
        invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

        uint8_t index = tca_gpio >> 4;
        mask[index] |= (1 << (tca_gpio % 16));
    } else if (mp_obj_is_type(pins, &mp_type_tuple) || mp_obj_is_type(pins, &mp_type_list)) {
        size_t len;
        mp_obj_t *items;
        mp_obj_get_array(pins, &len, &items);

        // go through each of the tuple/list items
        for (size_t i = 0; i < len; i++) {
            // ensure each is a tca pin
            if (!mp_obj_is_type(items[i], &tca_pin_type)) {
                mp_raise_TypeError_varg(translate("pin in list or tuple must be of type %q, not %q"), tca_pin_type.name, mp_obj_get_type(items[i])->name);
            }
            mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(items[i]);
            uint8_t tca_gpio = pin->number;
            invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

            uint8_t index = tca_gpio >> 4;
            mask[index] |= (1 << (tca_gpio % 16));
        }
    } else {
        mp_raise_TypeError_varg(translate("%q must be of type %q, %q or %q, not %q"), arg_name, tca_pin_type.name, mp_type_tuple.name, mp_type_list.name, mp_obj_get_type(pins)->name);
    }
}

/*
STATIC mp_obj_t tca_pin_change_output(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_set, ARG_clear };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_set, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_clear, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // args will contain only the value for preserve_dios. The *alarms args are in pos_args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(0, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t set_pins = args[ARG_set].u_obj;
    mp_obj_t clear_pins = args[ARG_clear].u_obj;
    if (set_pins == mp_const_none) {
        if (clear_pins == mp_const_none) {
            mp_raise_TypeError_varg(translate("set or clear must be of type %q, %q or %q"), tca_pin_type.name, mp_type_tuple.name, mp_type_list.name);
        }
        set_pins = mp_const_empty_tuple;
    } else if (clear_pins == mp_const_none) {
        clear_pins = mp_const_empty_tuple;
    }

    // create a set mask and zero it
    uint16_t set_mask[TCA9555R_CHIP_COUNT];
    uint16_t clear_mask[TCA9555R_CHIP_COUNT];
    for (size_t i = 0; i < TCA9555R_CHIP_COUNT; i++) {
        set_mask[i] = 0x0000;
        clear_mask[i] = 0x0000;
    }

    tca_populate_mask(set_pins, set_mask, MP_QSTR_set);
    tca_populate_mask(clear_pins, clear_mask, MP_QSTR_clear);

    for (size_t i = 0; i < TCA9555R_CHIP_COUNT; i++) {
        uint8_t address = tca9555r_addresses[i];
        uint8_t low_set_mask = (uint8_t)(set_mask[i] & 0xFF);
        uint8_t low_clear_mask = (uint8_t)(clear_mask[i] & 0xFF);
        uint8_t high_set_mask = (uint8_t)(set_mask[i] >> 8);
        uint8_t high_clear_mask = (uint8_t)(clear_mask[i] >> 8);
        bool low_changed = (low_set_mask | low_clear_mask) > 0;
        bool high_changed = (high_set_mask | high_clear_mask) > 0;
        if (low_changed && high_changed) {
            //uint16_t output_state = tca_gpio_get_output_port(address);
            uint16_t output_state = (tca9555r_output_state[(i * 2) + 1] << 8) | tca9555r_output_state[(i * 2)];
            uint16_t new_output_state = output_state;
            new_output_state |= (uint16_t)low_set_mask | ((uint16_t)high_set_mask << 8);
            new_output_state &= ~((uint16_t)low_clear_mask | ((uint16_t)high_clear_mask << 8));
            if (new_output_state != output_state) {
                tca_gpio_set_output_port(address, new_output_state);
                tca9555r_output_state[(i * 2) + 1] = (new_output_state >> 8);
                tca9555r_output_state[(i * 2)] = (new_output_state & 0xFF);
            }
        } else if (low_changed) {
            //uint8_t output_state = tca_gpio_get_low_output_port(address);
            uint8_t output_state = tca9555r_output_state[(i * 2)];
            uint8_t new_output_state = (output_state | low_set_mask) & ~low_clear_mask;
            if (new_output_state != output_state) {
                tca_gpio_set_low_output_port(address, new_output_state);
                tca9555r_output_state[(i * 2)] = new_output_state;
            }
        } else if (high_changed) {
            //uint8_t output_state = tca_gpio_get_high_output_port(address);
            uint8_t output_state = tca9555r_output_state[(i * 2) + 1];
            uint8_t new_output_state = (output_state | high_set_mask) & ~high_clear_mask;
            if (new_output_state != output_state) {
                tca_gpio_set_high_output_port(address, new_output_state);
                tca9555r_output_state[(i * 2) + 1] = new_output_state;
            }
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(tca_pin_change_output_obj, 0, tca_pin_change_output);
*/

STATIC mp_obj_t tca_pin_get_number(mp_obj_t pin_obj) {
    if (!mp_obj_is_type(pin_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_pin, tca_pin_type.name, mp_obj_get_type(pin_obj)->name);
    }

    mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_obj);
    uint8_t tca_gpio = pin->number;
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    return mp_obj_new_int(tca_gpio % TCA9555R_GPIO_COUNT);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_pin_get_number_obj, tca_pin_get_number);

STATIC mp_obj_t tca_pin_get_chip(mp_obj_t pin_obj) {
    if (!mp_obj_is_type(pin_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_pin, tca_pin_type.name, mp_obj_get_type(pin_obj)->name);
    }

    mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_obj);
    uint8_t tca_gpio = pin->number;
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    return mp_obj_new_int(tca_gpio / TCA9555R_GPIO_COUNT);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_pin_get_chip_obj, tca_pin_get_chip);

void tca_change_output_mask(uint8_t chip, uint16_t mask, uint16_t state) {
    uint8_t address = tca9555r_addresses[chip];
    uint8_t low_mask = (uint8_t)(mask & 0xFF);
    uint8_t low_state = (uint8_t)(state & 0xFF);
    uint8_t high_mask = (uint8_t)(mask >> 8);
    uint8_t high_state = (uint8_t)(state >> 8);
    bool low_changed = low_mask > 0;
    bool high_changed = high_mask > 0;
    if (low_changed && high_changed) {
        //uint16_t output_state = tca_gpio_get_output_port(address);
        uint16_t output_state = (tca9555r_output_state[(chip * 2) + 1] << 8) | tca9555r_output_state[(chip * 2)];
        uint16_t new_output_state = output_state;
        new_output_state &= ~mask; // Clear the mask bits
        new_output_state |= state; // Set the state bits
        if (new_output_state != output_state) {
            tca_gpio_set_output_port(address, new_output_state);
            tca9555r_output_state[(chip * 2) + 1] = (new_output_state >> 8);
            tca9555r_output_state[(chip * 2)] = (new_output_state & 0xFF);
        }
    } else if (low_changed) {
        //uint8_t output_state = tca_gpio_get_low_output_port(address);
        uint8_t output_state = tca9555r_output_state[(chip * 2)];
        uint8_t new_output_state = (output_state & ~low_mask) | low_state;
        if (new_output_state != output_state) {
            tca_gpio_set_low_output_port(address, new_output_state);
            tca9555r_output_state[(chip * 2)] = new_output_state;
        }
    } else if (high_changed) {
        //uint8_t output_state = tca_gpio_get_high_output_port(address);
        uint8_t output_state = tca9555r_output_state[(chip * 2) + 1];
        uint8_t new_output_state = (output_state & ~high_mask) | high_state;
        if (new_output_state != output_state) {
            tca_gpio_set_high_output_port(address, new_output_state);
            tca9555r_output_state[(chip * 2) + 1] = new_output_state;
        }
    }
}
/*
void tca_change_output_low_mask(uint8_t chip, uint8_t mask, uint8_t state) {
    uint8_t address = tca9555r_addresses[chip];
    //bool changed = mask > 0;
    //if (changed) {
        //uint8_t output_state = tca_gpio_get_low_output_port(address);
        uint8_t output_state = tca9555r_output_state[(chip * 2)];
        uint8_t new_output_state = (output_state & ~mask) | state;
        if (new_output_state != output_state) {
            tca_gpio_set_low_output_port(address, new_output_state);
            tca9555r_output_state[(chip * 2)] = new_output_state;
        }
    //}
}

void tca_change_output_high_mask(uint8_t chip, uint8_t mask, uint8_t state) {
    uint8_t address = tca9555r_addresses[chip];
    //bool changed = mask > 0;
    //if (changed) {
        //uint8_t output_state = tca_gpio_get_high_output_port(address);
        uint8_t output_state = tca9555r_output_state[(chip * 2) + 1];
        uint8_t new_output_state = (output_state & ~mask) | state;
        if (new_output_state != output_state) {
            tca_gpio_set_high_output_port(address, new_output_state);
            tca9555r_output_state[(chip * 2) + 1] = new_output_state;
        }
    //}
}*/

STATIC mp_obj_t tca_pin_change_output_mask(mp_obj_t chip_obj, mp_obj_t mask_obj, mp_obj_t state_obj) {
    int chip = mp_obj_get_int(chip_obj);
    int mask = mp_obj_get_int(mask_obj);
    int state = mp_obj_get_int(state_obj);
    if (chip < 0 || chip > TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT);
    }
    if (mask < 0 || mask > UINT16_MAX) {
        mp_raise_TypeError(translate("mask only supports 16 bits"));
    }
    if (state < 0 || state > UINT16_MAX) {
        mp_raise_TypeError(translate("state only supports 16 bits"));
    }

    tca_change_output_mask(chip, mask, state);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tca_pin_change_output_mask_obj, tca_pin_change_output_mask);

/*
void start_i2c(uint8_t sda_gpio, uint8_t scl_gpio) {
    tca_gpio_set_output(sda_gpio, false);
    tca_gpio_set_output(scl_gpio, false);
}

void end_i2c(uint8_t sda_gpio, uint8_t scl_gpio) {
    tca_gpio_set_output(scl_gpio, true);
    tca_gpio_set_output(sda_gpio, true);

    // This kinda worked but caused occasional glitches

    //uint8_t mask_and_state = tca_pin_to_bit_mask(scl_gpio) | tca_pin_to_bit_mask(sda_gpio);
    //if (tca_get_byte_from_pin(sda_gpio) > 0) {
    //    tca_change_output_high_mask(sda_gpio / TCA9555R_GPIO_COUNT, mask_and_state, mask_and_state);
    //}
    //else {
    //    tca_change_output_low_mask(sda_gpio / TCA9555R_GPIO_COUNT, mask_and_state, mask_and_state);
    //}
}

void bits_i2c(uint8_t sda_gpio, uint8_t scl_gpio, uint8_t number) {
    uint8_t bit = 0x80;
    while (bit > 0) {
        uint8_t mask = 0x00;
        uint8_t state = 0x00;

        //tca_gpio_set_output(sda_gpio, number & bit);

        if (bit < 0x80) {
            //slow_scl.value = False
            uint8_t b = tca_pin_to_bit_mask(scl_gpio);
            mask |= b;
        }

        //slow_sda.value = number & bit
        uint8_t b2 = tca_pin_to_bit_mask(sda_gpio);
        mask |= b2;
        if (number & bit) {
            state |= b2;
        }

        if (tca_get_byte_from_pin(sda_gpio) > 0) {
            tca_change_output_high_mask(sda_gpio / TCA9555R_GPIO_COUNT, mask, state);
        }
        else {
            tca_change_output_low_mask(sda_gpio / TCA9555R_GPIO_COUNT, mask, state);
        }

        //slow_scl.value = True
        tca_gpio_set_output(scl_gpio, true);
        //tca_gpio_set_output(scl_gpio, false);
        bit >>= 1;
    }
    //slow_scl.value = False
    tca_gpio_set_output(scl_gpio, false);

    // Do ACK
    //slow_sda.switch_to_input()
    tca_gpio_set_dir(sda_gpio, false);

    //slow_scl.value = True
    tca_gpio_set_output(scl_gpio, true);
    //slow_scl.value = False
    tca_gpio_set_output(scl_gpio, false);
    //slow_sda.switch_to_output()
    tca_gpio_set_dir(sda_gpio, true);
}

void bit_i2c(uint8_t sda_gpio, uint8_t scl_gpio, uint8_t number, uint8_t bit) {
    uint8_t mask = 0x00;
    uint8_t state = 0x00;

    //tca_gpio_set_output(sda_gpio, number & bit);

    //slow_scl.value = False
    uint8_t b = tca_pin_to_bit_mask(scl_gpio);
    mask |= b;

    //slow_sda.value = number & bit
    uint8_t b2 = tca_pin_to_bit_mask(sda_gpio);
    mask |= b2;
    if (number & bit) {
        state |= b2;
    }

    if (tca_get_byte_from_pin(sda_gpio) > 0) {
        tca_change_output_high_mask(sda_gpio / TCA9555R_GPIO_COUNT, mask, state);
    }
    else {
        tca_change_output_low_mask(sda_gpio / TCA9555R_GPIO_COUNT, mask, state);
    }

    //slow_scl.value = True
    tca_gpio_set_output(scl_gpio, true);
    //tca_gpio_set_output(scl_gpio, false);
}

void ack_bit_i2c(uint8_t sda_gpio, uint8_t scl_gpio) {
    //slow_scl.value = False
    tca_gpio_set_output(scl_gpio, false);

    // Do ACK
    //slow_sda.switch_to_input()
    tca_gpio_set_dir(sda_gpio, false);

    //slow_scl.value = True
    tca_gpio_set_output(scl_gpio, true);
    //slow_scl.value = False
    tca_gpio_set_output(scl_gpio, false);
    //slow_sda.switch_to_output()
    tca_gpio_set_dir(sda_gpio, true);
}

uint8_t read_bits_i2c(uint8_t sda_gpio, uint8_t scl_gpio) {
    //slow_sda.switch_to_input()
    tca_gpio_set_dir(sda_gpio, false);

    uint8_t number = 0;
    uint8_t bit = 0x80;
    while (bit > 0) {
        //print(number & bit)
        //slow_scl.value = True
        tca_gpio_set_output(scl_gpio, true);

        if (tca_gpio_get_input(sda_gpio)) {
            number |= bit;
        }

        //slow_scl.value = False
        tca_gpio_set_output(scl_gpio, false);
        bit >>= 1;
    }

    // Do ACK
    //slow_scl.value = True
    tca_gpio_set_output(scl_gpio, true);
    //slow_scl.value = False
    tca_gpio_set_output(scl_gpio, false);
    //slow_sda.switch_to_output()
    tca_gpio_set_dir(sda_gpio, true);

    return number;
}


STATIC mp_obj_t tca_pin_soft_i2c_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sda, ARG_scl, ARG_address, ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sda, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_scl, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_address, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_data, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sda_obj = args[ARG_sda].u_obj;
    mp_obj_t scl_obj = args[ARG_scl].u_obj;
    int address = args[ARG_address].u_int;
    if (!mp_obj_is_type(sda_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(sda_obj)->name);
    }
    if (!mp_obj_is_type(scl_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(scl_obj)->name);
    }
    if (address < 0 || address > 127) {
        mp_raise_ValueError_varg(translate("%q must be from 0 to 127"), MP_QSTR_address);
    }

    mcu_pin_obj_t *sda_pin = MP_OBJ_TO_PTR(sda_obj);
    mcu_pin_obj_t *scl_pin = MP_OBJ_TO_PTR(scl_obj);
    uint8_t sda_gpio = sda_pin->number;
    uint8_t scl_gpio = scl_pin->number;
    invalid_params_if(TCA9555R, sda_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    invalid_params_if(TCA9555R, scl_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    if (tca_get_byte_from_pin(sda_gpio) != tca_get_byte_from_pin(scl_gpio)) {
        mp_raise_ValueError_varg(translate("%q and %q must be on the same io expander byte"), MP_QSTR_sda, MP_QSTR_scl);
    }

    start_i2c(sda_gpio, scl_gpio);
    bits_i2c(sda_gpio, scl_gpio, address << 1);

    mp_obj_t data_obj = args[ARG_data].u_obj;
    if (mp_obj_is_int(data_obj)) {
        uint8_t data = mp_obj_get_int(data_obj);
        // Send data here
        bits_i2c(sda_gpio, scl_gpio, mp_obj_get_int(data_obj));
    }
    else if (mp_obj_is_type(data_obj, &mp_type_tuple) || mp_obj_is_type(data_obj, &mp_type_list)) {
        size_t len;
        mp_obj_t *items;
        mp_obj_get_array(data_obj, &len, &items);

        // go through each of the tuple/list items
        for (size_t i = 0; i < len; i++) {
            // Send data here
            bits_i2c(sda_gpio, scl_gpio, mp_obj_get_int(items[i]));
        }

    } else {
        mp_raise_TypeError_varg(translate("data must be of type int, %q or %q, not %q"), tca_pin_type.name, mp_type_tuple.name, mp_type_list.name, mp_obj_get_type(data_obj)->name);
    }

    end_i2c(sda_gpio, scl_gpio);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(tca_pin_soft_i2c_write_obj, 4, tca_pin_soft_i2c_write);

STATIC mp_obj_t tca_pin_soft_i2c_start(mp_obj_t sda_obj, mp_obj_t scl_obj) {
    if (!mp_obj_is_type(sda_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(sda_obj)->name);
    }
    if (!mp_obj_is_type(scl_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(scl_obj)->name);
    }

    mcu_pin_obj_t *sda_pin = MP_OBJ_TO_PTR(sda_obj);
    mcu_pin_obj_t *scl_pin = MP_OBJ_TO_PTR(scl_obj);
    uint8_t sda_gpio = sda_pin->number;
    uint8_t scl_gpio = scl_pin->number;
    invalid_params_if(TCA9555R, sda_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    invalid_params_if(TCA9555R, scl_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    if (tca_get_byte_from_pin(sda_gpio) != tca_get_byte_from_pin(scl_gpio)) {
        mp_raise_ValueError_varg(translate("%q and %q must be on the same io expander byte"), MP_QSTR_sda, MP_QSTR_scl);
    }

    start_i2c(sda_gpio, scl_gpio);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tca_pin_soft_i2c_start_obj, tca_pin_soft_i2c_start);

STATIC mp_obj_t tca_pin_soft_i2c_write_byte(mp_obj_t sda_obj, mp_obj_t scl_obj, mp_obj_t data_obj) {
    if (!mp_obj_is_type(sda_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(sda_obj)->name);
    }
    if (!mp_obj_is_type(scl_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(scl_obj)->name);
    }

    mcu_pin_obj_t *sda_pin = MP_OBJ_TO_PTR(sda_obj);
    mcu_pin_obj_t *scl_pin = MP_OBJ_TO_PTR(scl_obj);
    uint8_t sda_gpio = sda_pin->number;
    uint8_t scl_gpio = scl_pin->number;
    invalid_params_if(TCA9555R, sda_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    invalid_params_if(TCA9555R, scl_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    if (tca_get_byte_from_pin(sda_gpio) != tca_get_byte_from_pin(scl_gpio)) {
        mp_raise_ValueError_varg(translate("%q and %q must be on the same io expander byte"), MP_QSTR_sda, MP_QSTR_scl);
    }

    bits_i2c(sda_gpio, scl_gpio, mp_obj_get_int(data_obj));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tca_pin_soft_i2c_write_byte_obj, tca_pin_soft_i2c_write_byte);

STATIC mp_obj_t tca_pin_soft_i2c_write_bit(size_t n_args, const mp_obj_t *args) {
    mp_obj_t sda_obj = args[0];
    mp_obj_t scl_obj = args[1];
    mp_obj_t data_obj = args[2];
    mp_obj_t bit_obj = args[3];
    if (!mp_obj_is_type(sda_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(sda_obj)->name);
    }
    if (!mp_obj_is_type(scl_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(scl_obj)->name);
    }

    mcu_pin_obj_t *sda_pin = MP_OBJ_TO_PTR(sda_obj);
    mcu_pin_obj_t *scl_pin = MP_OBJ_TO_PTR(scl_obj);
    uint8_t sda_gpio = sda_pin->number;
    uint8_t scl_gpio = scl_pin->number;
    invalid_params_if(TCA9555R, sda_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    invalid_params_if(TCA9555R, scl_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    if (tca_get_byte_from_pin(sda_gpio) != tca_get_byte_from_pin(scl_gpio)) {
        mp_raise_ValueError_varg(translate("%q and %q must be on the same io expander byte"), MP_QSTR_sda, MP_QSTR_scl);
    }

    bit_i2c(sda_gpio, scl_gpio, mp_obj_get_int(data_obj), mp_obj_get_int(bit_obj));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(tca_pin_soft_i2c_write_bit_obj, 4, 4, tca_pin_soft_i2c_write_bit);

STATIC mp_obj_t tca_pin_soft_i2c_ack_bit(mp_obj_t sda_obj, mp_obj_t scl_obj) {
    if (!mp_obj_is_type(sda_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(sda_obj)->name);
    }
    if (!mp_obj_is_type(scl_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(scl_obj)->name);
    }

    mcu_pin_obj_t *sda_pin = MP_OBJ_TO_PTR(sda_obj);
    mcu_pin_obj_t *scl_pin = MP_OBJ_TO_PTR(scl_obj);
    uint8_t sda_gpio = sda_pin->number;
    uint8_t scl_gpio = scl_pin->number;
    invalid_params_if(TCA9555R, sda_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    invalid_params_if(TCA9555R, scl_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    if (tca_get_byte_from_pin(sda_gpio) != tca_get_byte_from_pin(scl_gpio)) {
        mp_raise_ValueError_varg(translate("%q and %q must be on the same io expander byte"), MP_QSTR_sda, MP_QSTR_scl);
    }

    ack_bit_i2c(sda_gpio, scl_gpio);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tca_pin_soft_i2c_ack_bit_obj, tca_pin_soft_i2c_ack_bit);

STATIC mp_obj_t tca_pin_soft_i2c_read_byte(mp_obj_t sda_obj, mp_obj_t scl_obj) {
    if (!mp_obj_is_type(sda_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(sda_obj)->name);
    }
    if (!mp_obj_is_type(scl_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(scl_obj)->name);
    }

    mcu_pin_obj_t *sda_pin = MP_OBJ_TO_PTR(sda_obj);
    mcu_pin_obj_t *scl_pin = MP_OBJ_TO_PTR(scl_obj);
    uint8_t sda_gpio = sda_pin->number;
    uint8_t scl_gpio = scl_pin->number;
    invalid_params_if(TCA9555R, sda_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    invalid_params_if(TCA9555R, scl_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    if (tca_get_byte_from_pin(sda_gpio) != tca_get_byte_from_pin(scl_gpio)) {
        mp_raise_ValueError_varg(translate("%q and %q must be on the same io expander byte"), MP_QSTR_sda, MP_QSTR_scl);
    }

    return mp_obj_new_int(read_bits_i2c(sda_gpio, scl_gpio));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tca_pin_soft_i2c_read_byte_obj, tca_pin_soft_i2c_read_byte);


STATIC mp_obj_t tca_pin_soft_i2c_end(mp_obj_t sda_obj, mp_obj_t scl_obj) {
    if (!mp_obj_is_type(sda_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(sda_obj)->name);
    }
    if (!mp_obj_is_type(scl_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(scl_obj)->name);
    }

    mcu_pin_obj_t *sda_pin = MP_OBJ_TO_PTR(sda_obj);
    mcu_pin_obj_t *scl_pin = MP_OBJ_TO_PTR(scl_obj);
    uint8_t sda_gpio = sda_pin->number;
    uint8_t scl_gpio = scl_pin->number;
    invalid_params_if(TCA9555R, sda_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    invalid_params_if(TCA9555R, scl_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    if (tca_get_byte_from_pin(sda_gpio) != tca_get_byte_from_pin(scl_gpio)) {
        mp_raise_ValueError_varg(translate("%q and %q must be on the same io expander byte"), MP_QSTR_sda, MP_QSTR_scl);
    }

    end_i2c(sda_gpio, scl_gpio);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tca_pin_soft_i2c_end_obj, tca_pin_soft_i2c_end);

STATIC mp_obj_t tca_pin_soft_i2c_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sda, ARG_scl, ARG_address, ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sda, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_scl, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_address, MP_ARG_INT | MP_ARG_REQUIRED },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sda_obj = args[ARG_sda].u_obj;
    mp_obj_t scl_obj = args[ARG_scl].u_obj;
    int address = args[ARG_address].u_int;
    if (!mp_obj_is_type(sda_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(sda_obj)->name);
    }
    if (!mp_obj_is_type(scl_obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("%q must be of type %q, not %q"), MP_QSTR_sda, tca_pin_type.name, mp_obj_get_type(scl_obj)->name);
    }
    if (address < 0 || address > 127) {
        mp_raise_ValueError_varg(translate("%q must be from 0 to 127"), MP_QSTR_address);
    }

    mcu_pin_obj_t *sda_pin = MP_OBJ_TO_PTR(sda_obj);
    mcu_pin_obj_t *scl_pin = MP_OBJ_TO_PTR(scl_obj);
    uint8_t sda_gpio = sda_pin->number;
    uint8_t scl_gpio = scl_pin->number;
    invalid_params_if(TCA9555R, sda_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    invalid_params_if(TCA9555R, scl_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);

    if (tca_get_byte_from_pin(sda_gpio) != tca_get_byte_from_pin(scl_gpio)) {
        mp_raise_ValueError_varg(translate("%q and %q must be on the same io expander byte"), MP_QSTR_sda, MP_QSTR_scl);
    }

    start_i2c(sda_gpio, scl_gpio);
    bits_i2c(sda_gpio, scl_gpio, (address << 1) | 0x01);
    uint8_t data = read_bits_i2c(sda_gpio, scl_gpio);

    end_i2c(sda_gpio, scl_gpio);

    return mp_obj_new_int(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(tca_pin_soft_i2c_read_obj, 3, tca_pin_soft_i2c_read);
*/
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

#if CIRCUITPY_TCA9555R
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
    //{ MP_ROM_QSTR(MP_QSTR_change_output), &tca_pin_change_output_obj },
    { MP_ROM_QSTR(MP_QSTR_change_output_mask), &tca_pin_change_output_mask_obj },
    //{ MP_ROM_QSTR(MP_QSTR_soft_i2c_write), &tca_pin_soft_i2c_write_obj },
    //{ MP_ROM_QSTR(MP_QSTR_soft_i2c_start), &tca_pin_soft_i2c_start_obj },
    //{ MP_ROM_QSTR(MP_QSTR_soft_i2c_write_byte), &tca_pin_soft_i2c_write_byte_obj },
    //{ MP_ROM_QSTR(MP_QSTR_soft_i2c_write_bit), &tca_pin_soft_i2c_write_bit_obj },
    //{ MP_ROM_QSTR(MP_QSTR_soft_i2c_ack_bit), &tca_pin_soft_i2c_ack_bit_obj },
    //{ MP_ROM_QSTR(MP_QSTR_soft_i2c_read_byte), &tca_pin_soft_i2c_read_byte_obj },
    //{ MP_ROM_QSTR(MP_QSTR_soft_i2c_end), &tca_pin_soft_i2c_end_obj },
    { MP_ROM_QSTR(MP_QSTR_get_number), &tca_pin_get_number_obj },
    { MP_ROM_QSTR(MP_QSTR_get_chip), &tca_pin_get_chip_obj },
    TCA_ENTRIES(0),
    TCA_ENTRIES(1)
};

MP_DEFINE_CONST_DICT(tca_module_globals, tca_module_globals_table);

const mp_obj_module_t tca_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&tca_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_tca, tca_module, CIRCUITPY_TCA9555R);
