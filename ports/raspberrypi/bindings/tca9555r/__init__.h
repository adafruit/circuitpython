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

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"

#define INPUT_PORT0  0x00
#define INPUT_PORT1  0x01
#define OUTPUT_PORT0  0x02
#define OUTPUT_PORT1  0x03
#define POLARITY_PORT0  0x04
#define POLARITY_PORT1  0x05
#define CONFIGURATION_PORT0  0x06
#define CONFIGURATION_PORT1  0x07

extern uint8_t tca9555r_output_state[TCA9555R_CHIP_COUNT * 2];
extern uint8_t tca9555r_config_state[TCA9555R_CHIP_COUNT * 2];


extern const mp_obj_dict_t tca_module_globals;
extern const mp_obj_type_t tca_pin_type;
const mcu_pin_obj_t *validate_obj_is_free_pin_including_tca(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_free_pin_including_tca_or_none(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_pin_including_tca(mp_obj_t obj, qstr arg_name);

uint8_t tca_get_address_from_pin(uint tca_gpio);
bool tca_gpio_get_input(uint tca_gpio);
bool tca_gpio_get_output(uint tca_gpio);
void tca_gpio_set_output(uint tca_gpio, bool value);
bool tca_gpio_get_dir(uint tca_gpio);
void tca_gpio_set_dir(uint tca_gpio, bool output);

uint16_t tca_gpio_get_input_port(uint tca_address);
uint16_t tca_gpio_get_output_port(uint tca_address);
void tca_gpio_set_output_port(uint tca_address, uint16_t output_state);
uint16_t tca_gpio_get_dir_port(uint tca_address);
void tca_gpio_set_dir_port(uint tca_address, uint16_t config_state);
uint16_t tca_gpio_get_polarity_port(uint tca_address);
void tca_gpio_set_polarity_port(uint tca_address, uint16_t polarity_state);

void shared_bindings_tca9555r_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);

#if CIRCUITPY_TCA9555R
extern const mcu_pin_obj_t pin_TCA0_0;
extern const mcu_pin_obj_t pin_TCA0_1;
extern const mcu_pin_obj_t pin_TCA0_2;
extern const mcu_pin_obj_t pin_TCA0_3;
extern const mcu_pin_obj_t pin_TCA0_4;
extern const mcu_pin_obj_t pin_TCA0_5;
extern const mcu_pin_obj_t pin_TCA0_6;
extern const mcu_pin_obj_t pin_TCA0_7;
extern const mcu_pin_obj_t pin_TCA0_8;
extern const mcu_pin_obj_t pin_TCA0_9;
extern const mcu_pin_obj_t pin_TCA0_10;
extern const mcu_pin_obj_t pin_TCA0_11;
extern const mcu_pin_obj_t pin_TCA0_12;
extern const mcu_pin_obj_t pin_TCA0_13;
extern const mcu_pin_obj_t pin_TCA0_14;
extern const mcu_pin_obj_t pin_TCA0_15;

extern const mcu_pin_obj_t pin_TCA1_0;
extern const mcu_pin_obj_t pin_TCA1_1;
extern const mcu_pin_obj_t pin_TCA1_2;
extern const mcu_pin_obj_t pin_TCA1_3;
extern const mcu_pin_obj_t pin_TCA1_4;
extern const mcu_pin_obj_t pin_TCA1_5;
extern const mcu_pin_obj_t pin_TCA1_6;
extern const mcu_pin_obj_t pin_TCA1_7;
extern const mcu_pin_obj_t pin_TCA1_8;
extern const mcu_pin_obj_t pin_TCA1_9;
extern const mcu_pin_obj_t pin_TCA1_10;
extern const mcu_pin_obj_t pin_TCA1_11;
extern const mcu_pin_obj_t pin_TCA1_12;
extern const mcu_pin_obj_t pin_TCA1_13;
extern const mcu_pin_obj_t pin_TCA1_14;
extern const mcu_pin_obj_t pin_TCA1_15;
#endif
