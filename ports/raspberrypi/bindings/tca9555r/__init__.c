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
#if TCA9555R_LOCAL_MEMORY
uint8_t tca9555r_output_state[TCA9555R_CHIP_COUNT * 2] = {0};
uint8_t tca9555r_config_state[TCA9555R_CHIP_COUNT * 2] = {0};
uint8_t tca9555r_polarity_state[TCA9555R_CHIP_COUNT * 2] = {0};
#endif

bool tca_gpio_get_input(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? INPUT_PORT1 : INPUT_PORT0;
    uint8_t input_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &input_state, 1);
    return (input_state & GPIO_BIT_MASK(tca_gpio)) != 0;
}

bool tca_gpio_get_output(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? OUTPUT_PORT1 : OUTPUT_PORT0;
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &output_state, 1);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_output_state[GPIO_BYTE(tca_gpio)] = output_state;
    #endif
    return (output_state & GPIO_BIT_MASK(tca_gpio)) != 0;
}

bool tca_gpio_get_config(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? CONFIGURATION_PORT1 : CONFIGURATION_PORT0;
    uint8_t config_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &config_state, 1);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_config_state[GPIO_BYTE(tca_gpio)] = config_state;
    #endif
    return (config_state & GPIO_BIT_MASK(tca_gpio)) == 0;
}

bool tca_gpio_get_polarity(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? POLARITY_PORT1 : POLARITY_PORT0;
    uint8_t polarity_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &polarity_state, 1);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_polarity_state[GPIO_BYTE(tca_gpio)] = polarity_state;
    #endif
    return (polarity_state & GPIO_BIT_MASK(tca_gpio)) != 0;
}

void tca_gpio_set_output(uint tca_gpio, bool value) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? OUTPUT_PORT1 : OUTPUT_PORT0;
    #if TCA9555R_LOCAL_MEMORY
    uint8_t output_state = tca9555r_output_state[GPIO_BYTE(tca_gpio)];
    #else
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &output_state, 1);
    #endif
    uint8_t new_output_state;
    if (value) {
        new_output_state = output_state | GPIO_BIT_MASK(tca_gpio);
    } else {
        new_output_state = output_state & ~GPIO_BIT_MASK(tca_gpio);
    }

    if (new_output_state != output_state) {
        uint8_t reg_and_data[2] = { reg, new_output_state };
        common_hal_busio_i2c_write(i2c, address, reg_and_data, 2);
        #if TCA9555R_LOCAL_MEMORY
        tca9555r_output_state[GPIO_BYTE(tca_gpio)] = new_output_state;
        #endif
    }
}

void tca_gpio_set_config(uint tca_gpio, bool output) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? CONFIGURATION_PORT1 : CONFIGURATION_PORT0;
    #if TCA9555R_LOCAL_MEMORY
    uint8_t config_state = tca9555r_config_state[GPIO_BYTE(tca_gpio)];
    #else
    uint8_t config_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &config_state, 1);
    #endif
    uint8_t new_config_state;
    if (output) {
        new_config_state = config_state & ~GPIO_BIT_MASK(tca_gpio);
    } else {
        new_config_state = config_state | GPIO_BIT_MASK(tca_gpio);
    }

    if (new_config_state != config_state) {
        uint8_t reg_and_data[2] = { reg, new_config_state };
        common_hal_busio_i2c_write(i2c, address, reg_and_data, 2);
        #if TCA9555R_LOCAL_MEMORY
        tca9555r_config_state[GPIO_BYTE(tca_gpio)] = new_config_state;
        #endif
    }
}

void tca_gpio_set_polarity(uint tca_gpio, bool polarity) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? POLARITY_PORT1 : POLARITY_PORT0;
    #if TCA9555R_LOCAL_MEMORY
    uint8_t polarity_state = tca9555r_polarity_state[GPIO_BYTE(tca_gpio)];
    #else
    uint8_t polarity_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &polarity_state, 1);
    #endif
    uint8_t new_polarity_state;
    if (polarity) {
        new_polarity_state = polarity_state | GPIO_BIT_MASK(tca_gpio);
    } else {
        new_polarity_state = polarity_state & ~GPIO_BIT_MASK(tca_gpio);
    }

    if (new_polarity_state != polarity_state) {
        uint8_t reg_and_data[2] = { reg, new_polarity_state };
        common_hal_busio_i2c_write(i2c, address, reg_and_data, 2);
        #if TCA9555R_LOCAL_MEMORY
        tca9555r_output_state[GPIO_BYTE(tca_gpio)] = new_polarity_state;
        #endif
    }
}

uint16_t tca_get_input_port(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = INPUT_PORT0;
    uint16_t input_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, (uint8_t *)&input_state, 2);
    return input_state;
}

uint8_t tca_get_input_port_low(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = INPUT_PORT0;
    uint8_t input_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, &input_state, 1);
    return input_state;
}

uint8_t tca_get_input_port_high(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = INPUT_PORT1;
    uint8_t input_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, &input_state, 1);
    return input_state;
}

uint16_t tca_get_output_port(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT0;
    uint16_t output_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, (uint8_t *)&output_state, 2);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_output_state[HIGH_BYTE(tca_index)] = (output_state >> 8);
    tca9555r_output_state[LOW_BYTE(tca_index)] = (output_state & 0xFF);
    #endif
    return output_state;
}

uint8_t tca_get_output_port_low(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT0;
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, &output_state, 1);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_output_state[LOW_BYTE(tca_index)] = output_state;
    #endif
    return output_state;
}

uint8_t tca_get_output_port_high(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT1;
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, &output_state, 1);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_output_state[HIGH_BYTE(tca_index)] = output_state;
    #endif
    return output_state;
}

uint16_t tca_get_config_port(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = CONFIGURATION_PORT0;
    uint16_t config_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, (uint8_t *)&config_state, 2);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_config_state[HIGH_BYTE(tca_index)] = (config_state >> 8);
    tca9555r_config_state[LOW_BYTE(tca_index)] = (config_state & 0xFF);
    #endif
    return config_state;
}

uint8_t tca_get_config_port_low(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = CONFIGURATION_PORT0;
    uint8_t config_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, &config_state, 1);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_config_state[LOW_BYTE(tca_index)] = config_state;
    #endif
    return config_state;
}

uint8_t tca_get_config_port_high(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = CONFIGURATION_PORT1;
    uint8_t config_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, &config_state, 1);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_config_state[HIGH_BYTE(tca_index)] = config_state;
    #endif
    return config_state;
}

uint16_t tca_get_polarity_port(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = POLARITY_PORT0;
    uint16_t polarity_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, (uint8_t *)&polarity_state, 2);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_polarity_state[HIGH_BYTE(tca_index)] = (polarity_state >> 8);
    tca9555r_polarity_state[LOW_BYTE(tca_index)] = (polarity_state & 0xFF);
    #endif
    return polarity_state;
}

uint8_t tca_get_polarity_port_low(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = POLARITY_PORT0;
    uint8_t polarity_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, &polarity_state, 1);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_polarity_state[LOW_BYTE(tca_index)] = polarity_state;
    #endif
    return polarity_state;
}

uint8_t tca_get_polarity_port_high(uint tca_index) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = POLARITY_PORT1;
    uint8_t polarity_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555r_addresses[tca_index], &reg, 1, &polarity_state, 1);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_polarity_state[HIGH_BYTE(tca_index)] = polarity_state;
    #endif
    return polarity_state;
}

void tca_set_output_port(uint tca_index, uint16_t output_state) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { OUTPUT_PORT0, output_state & 0xFF, (output_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca9555r_addresses[tca_index], reg_and_data, 3);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_output_state[HIGH_BYTE(tca_index)] = (output_state >> 8);
    tca9555r_output_state[LOW_BYTE(tca_index)] = (output_state & 0xFF);
    #endif
}

void tca_set_output_port_low(uint tca_index, uint8_t output_state) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { OUTPUT_PORT0, output_state };
    common_hal_busio_i2c_write(i2c, tca9555r_addresses[tca_index], reg_and_data, 2);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_output_state[LOW_BYTE(tca_index)] = output_state;
    #endif
}

void tca_set_output_port_high(uint tca_index, uint8_t output_state) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { OUTPUT_PORT1, output_state };
    common_hal_busio_i2c_write(i2c, tca9555r_addresses[tca_index], reg_and_data, 2);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_output_state[HIGH_BYTE(tca_index)] = output_state;
    #endif
}

void tca_set_config_port(uint tca_index, uint16_t config_state) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { CONFIGURATION_PORT0, config_state & 0xFF, (config_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca9555r_addresses[tca_index], reg_and_data, 3);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_config_state[HIGH_BYTE(tca_index)] = (config_state >> 8);
    tca9555r_config_state[LOW_BYTE(tca_index)] = (config_state & 0xFF);
    #endif
}

void tca_set_config_port_low(uint tca_index, uint8_t config_state) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { CONFIGURATION_PORT0, config_state };
    common_hal_busio_i2c_write(i2c, tca9555r_addresses[tca_index], reg_and_data, 2);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_config_state[LOW_BYTE(tca_index)] = config_state;
    #endif
}

void tca_set_config_port_high(uint tca_index, uint8_t config_state) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { CONFIGURATION_PORT1, config_state };
    common_hal_busio_i2c_write(i2c, tca9555r_addresses[tca_index], reg_and_data, 2);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_config_state[HIGH_BYTE(tca_index)] = config_state;
    #endif
}

void tca_set_polarity_port(uint tca_index, uint16_t polarity_state) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { POLARITY_PORT0, polarity_state & 0xFF, (polarity_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca9555r_addresses[tca_index], reg_and_data, 3);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_polarity_state[HIGH_BYTE(tca_index)] = (polarity_state >> 8);
    tca9555r_polarity_state[LOW_BYTE(tca_index)] = (polarity_state & 0xFF);
    #endif
}

void tca_set_polarity_port_low(uint tca_index, uint8_t polarity_state) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { POLARITY_PORT0, polarity_state };
    common_hal_busio_i2c_write(i2c, tca9555r_addresses[tca_index], reg_and_data, 2);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_polarity_state[LOW_BYTE(tca_index)] = polarity_state;
    #endif
}

void tca_set_polarity_port_high(uint tca_index, uint8_t polarity_state) {
    invalid_params_if(TCA9555R, tca_index >= TCA9555R_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { POLARITY_PORT1, polarity_state };
    common_hal_busio_i2c_write(i2c, tca9555r_addresses[tca_index], reg_and_data, 2);
    #if TCA9555R_LOCAL_MEMORY
    tca9555r_polarity_state[HIGH_BYTE(tca_index)] = polarity_state;
    #endif
}

void tca_change_output_mask(uint8_t chip, uint16_t mask, uint16_t state) {
    uint8_t low_mask = (uint8_t)(mask & 0xFF);
    uint8_t low_state = (uint8_t)(state & 0xFF);
    uint8_t high_mask = (uint8_t)(mask >> 8);
    uint8_t high_state = (uint8_t)(state >> 8);
    bool low_changed = low_mask > 0;
    bool high_changed = high_mask > 0;
    if (low_changed && high_changed) {
        #if TCA9555R_LOCAL_MEMORY
        uint16_t output_state = (tca9555r_output_state[HIGH_BYTE(chip)] << 8) | tca9555r_output_state[LOW_BYTE(chip)];
        #else
        uint16_t output_state = tca_get_output_port(chip);
        #endif
        uint16_t new_output_state = output_state;
        new_output_state &= ~mask; // Clear the mask bits
        new_output_state |= state; // Set the state bits
        if (new_output_state != output_state) {
            tca_set_output_port(chip, new_output_state);
        }
    } else if (low_changed) {
        #if TCA9555R_LOCAL_MEMORY
        uint8_t output_state = tca9555r_output_state[LOW_BYTE(chip)];
        #else
        uint8_t output_state = tca_get_output_port_low(chip);
        #endif
        uint8_t new_output_state = (output_state & ~low_mask) | low_state;
        if (new_output_state != output_state) {
            tca_set_output_port_low(chip, new_output_state);
        }
    } else if (high_changed) {
        #if TCA9555R_LOCAL_MEMORY
        uint8_t output_state = tca9555r_output_state[HIGH_BYTE(chip)];
        #else
        uint8_t output_state = tca_get_output_port_high(chip);
        #endif
        uint8_t new_output_state = (output_state & ~high_mask) | high_state;
        if (new_output_state != output_state) {
            tca_set_output_port_high(chip, new_output_state);
        }
    }
}

void tca_change_config_mask(uint8_t chip, uint16_t mask, uint16_t state) {
    uint8_t low_mask = (uint8_t)(mask & 0xFF);
    uint8_t low_state = (uint8_t)(state & 0xFF);
    uint8_t high_mask = (uint8_t)(mask >> 8);
    uint8_t high_state = (uint8_t)(state >> 8);
    bool low_changed = low_mask > 0;
    bool high_changed = high_mask > 0;
    if (low_changed && high_changed) {
        #if TCA9555R_LOCAL_MEMORY
        uint16_t config_state = (tca9555r_config_state[HIGH_BYTE(chip)] << 8) | tca9555r_config_state[LOW_BYTE(chip)];
        #else
        uint16_t config_state = tca_get_config_port(chip);
        #endif
        uint16_t new_config_state = config_state;
        new_config_state &= ~mask; // Clear the mask bits
        new_config_state |= state; // Set the state bits
        if (new_config_state != config_state) {
            tca_set_config_port(chip, new_config_state);
        }
    } else if (low_changed) {
        #if TCA9555R_LOCAL_MEMORY
        uint8_t config_state = tca9555r_config_state[LOW_BYTE(chip)];
        #else
        uint8_t config_state = tca_get_config_port_low(chip);
        #endif
        uint8_t new_config_state = (config_state & ~low_mask) | low_state;
        if (new_config_state != config_state) {
            tca_set_config_port_low(chip, new_config_state);
        }
    } else if (high_changed) {
        #if TCA9555R_LOCAL_MEMORY
        uint8_t config_state = tca9555r_config_state[HIGH_BYTE(chip)];
        #else
        uint8_t config_state = tca_get_config_port_high(chip);
        #endif
        uint8_t new_config_state = (config_state & ~high_mask) | high_state;
        if (new_config_state != config_state) {
            tca_set_config_port_high(chip, new_config_state);
        }
    }
}

void tca_change_polarity_mask(uint8_t chip, uint16_t mask, uint16_t state) {
    uint8_t low_mask = (uint8_t)(mask & 0xFF);
    uint8_t low_state = (uint8_t)(state & 0xFF);
    uint8_t high_mask = (uint8_t)(mask >> 8);
    uint8_t high_state = (uint8_t)(state >> 8);
    bool low_changed = low_mask > 0;
    bool high_changed = high_mask > 0;
    if (low_changed && high_changed) {
        #if TCA9555R_LOCAL_MEMORY
        uint16_t polarity_state = (tca9555r_polarity_state[HIGH_BYTE(chip)] << 8) | tca9555r_polarity_state[LOW_BYTE(chip)];
        #else
        uint16_t polarity_state = tca_get_polarity_port(chip);
        #endif
        uint16_t new_polarity_state = polarity_state;
        new_polarity_state &= ~mask; // Clear the mask bits
        new_polarity_state |= state; // Set the state bits
        if (new_polarity_state != polarity_state) {
            tca_set_polarity_port(chip, new_polarity_state);
        }
    } else if (low_changed) {
        #if TCA9555R_LOCAL_MEMORY
        uint8_t polarity_state = tca9555r_polarity_state[LOW_BYTE(chip)];
        #else
        uint8_t polarity_state = tca_get_polarity_port_low(chip);
        #endif
        uint8_t new_polarity_state = (polarity_state & ~low_mask) | low_state;
        if (new_polarity_state != polarity_state) {
            tca_set_polarity_port_low(chip, new_polarity_state);
        }
    } else if (high_changed) {
        #if TCA9555R_LOCAL_MEMORY
        uint8_t polarity_state = tca9555r_polarity_state[HIGH_BYTE(chip)];
        #else
        uint8_t polarity_state = tca_get_polarity_port_high(chip);
        #endif
        uint8_t new_polarity_state = (polarity_state & ~high_mask) | high_state;
        if (new_polarity_state != polarity_state) {
            tca_set_polarity_port_high(chip, new_polarity_state);
        }
    }
}

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

    return mp_obj_new_int(CHIP_FROM_GPIO(tca_gpio));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_pin_get_chip_obj, tca_pin_get_chip);



STATIC mp_obj_t tca_pin_change_output_mask(mp_obj_t chip_obj, mp_obj_t mask_obj, mp_obj_t state_obj) {
    int chip = mp_obj_get_int(chip_obj);
    int mask = mp_obj_get_int(mask_obj);
    int state = mp_obj_get_int(state_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
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

STATIC mp_obj_t tca_pin_change_config_mask(mp_obj_t chip_obj, mp_obj_t mask_obj, mp_obj_t state_obj) {
    int chip = mp_obj_get_int(chip_obj);
    int mask = mp_obj_get_int(mask_obj);
    int state = mp_obj_get_int(state_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
    }
    if (mask < 0 || mask > UINT16_MAX) {
        mp_raise_TypeError(translate("mask only supports 16 bits"));
    }
    if (state < 0 || state > UINT16_MAX) {
        mp_raise_TypeError(translate("state only supports 16 bits"));
    }

    tca_change_config_mask(chip, mask, state);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tca_pin_change_config_mask_obj, tca_pin_change_config_mask);

STATIC mp_obj_t tca_pin_change_polarity_mask(mp_obj_t chip_obj, mp_obj_t mask_obj, mp_obj_t state_obj) {
    int chip = mp_obj_get_int(chip_obj);
    int mask = mp_obj_get_int(mask_obj);
    int state = mp_obj_get_int(state_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
    }
    if (mask < 0 || mask > UINT16_MAX) {
        mp_raise_TypeError(translate("mask only supports 16 bits"));
    }
    if (state < 0 || state > UINT16_MAX) {
        mp_raise_TypeError(translate("state only supports 16 bits"));
    }

    tca_change_polarity_mask(chip, mask, state);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tca_pin_change_polarity_mask_obj, tca_pin_change_polarity_mask);

#if TCA9555R_READ_INTERNALS
STATIC mp_obj_t tca_port_read_input_state(mp_obj_t chip_obj) {
    int chip = mp_obj_get_int(chip_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
    }

    return mp_obj_new_int(tca_get_input_port(chip));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_read_input_state_obj, tca_port_read_input_state);

STATIC mp_obj_t tca_port_read_output_state(mp_obj_t chip_obj) {
    int chip = mp_obj_get_int(chip_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
    }

    return mp_obj_new_int(tca_get_output_port(chip));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_read_output_state_obj, tca_port_read_output_state);

STATIC mp_obj_t tca_port_read_config_state(mp_obj_t chip_obj) {
    int chip = mp_obj_get_int(chip_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
    }

    return mp_obj_new_int(tca_get_config_port(chip));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_read_config_state_obj, tca_port_read_config_state);

STATIC mp_obj_t tca_port_read_polarity_state(mp_obj_t chip_obj) {
    int chip = mp_obj_get_int(chip_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
    }

    return mp_obj_new_int(tca_get_polarity_port(chip));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_read_polarity_state_obj, tca_port_read_polarity_state);

#if TCA9555R_LOCAL_MEMORY
STATIC mp_obj_t tca_port_stored_output_state(mp_obj_t chip_obj) {
    int chip = mp_obj_get_int(chip_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
    }

    return mp_obj_new_int((tca9555r_output_state[HIGH_BYTE(chip)] << 8) | tca9555r_output_state[LOW_BYTE(chip)]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_stored_output_state_obj, tca_port_stored_output_state);

STATIC mp_obj_t tca_port_stored_config_state(mp_obj_t chip_obj) {
    int chip = mp_obj_get_int(chip_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
    }

    return mp_obj_new_int((tca9555r_config_state[HIGH_BYTE(chip)] << 8) | tca9555r_config_state[LOW_BYTE(chip)]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tca_port_stored_config_state_obj, tca_port_stored_config_state);

STATIC mp_obj_t tca_port_stored_polarity_state(mp_obj_t chip_obj) {
    int chip = mp_obj_get_int(chip_obj);
    if (chip < 0 || chip >= TCA9555R_CHIP_COUNT) {
        mp_raise_TypeError_varg(translate("chip can only be 0 to %q"), TCA9555R_CHIP_COUNT - 1);
    }

    return mp_obj_new_int((tca9555r_polarity_state[HIGH_BYTE(chip)] << 8) | tca9555r_polarity_state[LOW_BYTE(chip)]);
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
    { MP_ROM_QSTR(MP_QSTR_get_number), &tca_pin_get_number_obj },
    { MP_ROM_QSTR(MP_QSTR_get_chip), &tca_pin_get_chip_obj },
    { MP_ROM_QSTR(MP_QSTR_change_output_mask), &tca_pin_change_output_mask_obj },
    { MP_ROM_QSTR(MP_QSTR_change_config_mask), &tca_pin_change_config_mask_obj },
    { MP_ROM_QSTR(MP_QSTR_change_polarity_mask), &tca_pin_change_polarity_mask_obj },
    #if TCA9555R_READ_INTERNALS
    { MP_ROM_QSTR(MP_QSTR_read_input), &tca_port_read_input_state_obj },
    { MP_ROM_QSTR(MP_QSTR_read_output), &tca_port_read_output_state_obj },
    { MP_ROM_QSTR(MP_QSTR_read_config), &tca_port_read_config_state_obj },
    { MP_ROM_QSTR(MP_QSTR_read_polarity), &tca_port_read_polarity_state_obj },
    #if TCA9555R_LOCAL_MEMORY
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
