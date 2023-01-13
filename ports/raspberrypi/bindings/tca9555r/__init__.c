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
    .print = shared_bindings_microcontroller_pin_print,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = mp_generic_unary_op,
        )
};

const mcu_pin_obj_t *validate_obj_is_pin_including_tca(mp_obj_t obj) {
    if (!mp_obj_is_type(obj, &mcu_pin_type) && !mp_obj_is_type(obj, &tca_pin_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q or %q"), mcu_pin_type.name, tca_pin_type.name);
    }
    return MP_OBJ_TO_PTR(obj);
}

const mcu_pin_obj_t *validate_obj_is_free_pin_including_tca(mp_obj_t obj) {
    const mcu_pin_obj_t *pin = validate_obj_is_pin_including_tca(obj);
    assert_pin_free(pin);
    return pin;
}

// Validate that the obj is a free pin or None. Return an mcu_pin_obj_t* or NULL, correspondingly.
const mcu_pin_obj_t *validate_obj_is_free_pin_including_tca_or_none(mp_obj_t obj) {
    if (obj == mp_const_none) {
        return NULL;
    }
    return validate_obj_is_free_pin_including_tca(obj);
}

void tca_init(void) {
}

#define TCA_ADDR0  0x20
#define TCA_ADDR1  0x26
#define TCA9555R_GPIO_COUNT     16
#define TCA9555R_CHIP_COUNT     2
#define TCA9555R_VIRTUAL_GPIO_COUNT     (TCA9555R_GPIO_COUNT * TCA9555R_CHIP_COUNT)

uint8_t tca_get_address_from_pin(uint tca_gpio) {
    if(tca_gpio < TCA9555R_GPIO_COUNT) {
        return TCA_ADDR0;
    }
    return TCA_ADDR1;
}

bool tca_gpio_get_input(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);

    uint8_t reg = INPUT_PORT0;
    uint16_t input_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, (uint8_t*)&input_state, 2);
    return (input_state & (1 << (tca_gpio % TCA9555R_GPIO_COUNT))) != 0;
}

bool tca_gpio_get_output(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);
    tca_gpio = (tca_gpio % TCA9555R_GPIO_COUNT);

    uint8_t reg = OUTPUT_PORT0;
    uint16_t output_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, (uint8_t*)&output_state, 2);
    return (output_state & (1 << tca_gpio)) != 0;
}

void tca_gpio_set_output(uint tca_gpio, bool value) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);
    tca_gpio = (tca_gpio % TCA9555R_GPIO_COUNT);

    uint8_t reg = OUTPUT_PORT0;
    uint16_t output_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, (uint8_t*)&output_state, 2);
    if(value)
        output_state = output_state | (1 << tca_gpio);
    else
        output_state = output_state & ~(1 << tca_gpio);

    uint8_t reg_and_data[3] = { reg, output_state & 0xFF, (output_state >> 8) };
    common_hal_busio_i2c_write(i2c, address, reg_and_data, 3);
}

bool tca_gpio_get_dir(uint tca_gpio) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);
    tca_gpio = (tca_gpio % TCA9555R_GPIO_COUNT);

    uint8_t reg = CONFIGURATION_PORT0;
    uint16_t config_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, (uint8_t*)&config_state, 2);
    return (config_state & (1 << tca_gpio)) != 0;
}

void tca_gpio_set_dir(uint tca_gpio, bool output) {
    invalid_params_if(TCA9555R, tca_gpio >= TCA9555R_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = tca_get_address_from_pin(tca_gpio);
    tca_gpio = (tca_gpio % TCA9555R_GPIO_COUNT);

    uint8_t reg = CONFIGURATION_PORT0;
    uint16_t config_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, (uint8_t*)&config_state, 2);
    if(output)
        config_state = config_state & ~(1 << tca_gpio);
    else
        config_state = config_state | (1 << tca_gpio);        

    uint8_t reg_and_data[3] = { reg, config_state & 0xFF, (config_state >> 8) };
    common_hal_busio_i2c_write(i2c, address, reg_and_data, 3);
}

uint16_t tca_gpio_get_input_port(uint tca_address) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = INPUT_PORT0;
    uint16_t input_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, (uint8_t*)&input_state, 2);
    return input_state;
}

uint16_t tca_gpio_get_output_port(uint tca_address) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT0;
    uint16_t output_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, (uint8_t*)&output_state, 2);
    return output_state;
}

void tca_gpio_set_output_port(uint tca_address, uint16_t output_state) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { OUTPUT_PORT0, output_state & 0xFF, (output_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca_address, reg_and_data, 3);
}

uint16_t tca_gpio_get_dir_port(uint tca_address) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = CONFIGURATION_PORT0;
    uint16_t config_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, (uint8_t*)&config_state, 2);
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
    common_hal_busio_i2c_write_read(i2c, tca_address, &reg, 1, (uint8_t*)&polarity_state, 2);
    return polarity_state;
}

void tca_gpio_set_polarity_port(uint tca_address, uint16_t polarity_state) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { POLARITY_PORT0, polarity_state & 0xFF, (polarity_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca_address, reg_and_data, 3);
}

STATIC const mp_rom_map_elem_t tca_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tca) },
    { MP_ROM_QSTR(MP_QSTR_TcaPin), MP_ROM_PTR(&tca_pin_type) },
};

STATIC MP_DEFINE_CONST_DICT(tca_module_globals, tca_module_globals_table);

const mp_obj_module_t tca_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&tca_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_tca, tca_module, CIRCUITPY_TCA9555R);
