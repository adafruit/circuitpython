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

#define TCA9555R_GPIO_COUNT     16
#define TCA9555R_VIRTUAL_GPIO_COUNT     (TCA9555R_GPIO_COUNT * TCA9555R_CHIP_COUNT)

#ifndef TCA9555R_LOCAL_MEMORY
#define TCA9555R_LOCAL_MEMORY (0)
#endif

#ifndef TCA9555R_READ_INTERNALS
#define TCA9555R_READ_INTERNALS (0)
#endif

#if TCA9555R_LOCAL_MEMORY
extern uint8_t tca9555r_output_state[TCA9555R_CHIP_COUNT * 2];
extern uint8_t tca9555r_config_state[TCA9555R_CHIP_COUNT * 2];
extern uint8_t tca9555r_polarity_state[TCA9555R_CHIP_COUNT * 2];
#endif

#define HIGH_BYTE(index) (((index) * 2u) + 1u)
#define LOW_BYTE(index) (((index) * 2u))
#define IS_PORT1(gpio) (((gpio) % TCA9555R_GPIO_COUNT) >= 8u)
#define GPIO_BYTE(gpio) ((gpio) >> 3u)
#define GPIO_BIT_MASK(gpio) (1u << ((gpio) % 8u))
#define CHIP_FROM_GPIO(gpio) ((gpio) / TCA9555R_GPIO_COUNT)
#define ADDRESS_FROM_GPIO(gpio) (tca9555r_addresses[CHIP_FROM_GPIO(gpio)])


extern const mp_obj_dict_t tca_module_globals;
extern const mp_obj_type_t tca_pin_type;

void shared_bindings_tca9555r_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);

const mcu_pin_obj_t *validate_obj_is_free_pin_including_tca(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_free_pin_including_tca_or_none(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_pin_including_tca(mp_obj_t obj, qstr arg_name);

bool tca_gpio_get_input(uint tca_gpio);
bool tca_gpio_get_output(uint tca_gpio);
bool tca_gpio_get_config(uint tca_gpio);
bool tca_gpio_get_polarity(uint tca_gpio);

void tca_gpio_set_output(uint tca_gpio, bool value);
void tca_gpio_set_config(uint tca_gpio, bool output);
void tca_gpio_set_polarity(uint tca_gpio, bool polarity);

uint16_t tca_get_input_port(uint tca_index);
uint8_t tca_get_input_port_low(uint tca_index);
uint8_t tca_get_input_port_high(uint tca_index);

uint16_t tca_get_output_port(uint tca_index);
uint8_t tca_get_output_port_low(uint tca_index);
uint8_t tca_get_output_port_high(uint tca_index);

uint16_t tca_get_config_port(uint tca_index);
uint8_t tca_get_config_port_low(uint tca_index);
uint8_t tca_get_config_port_high(uint tca_index);

uint16_t tca_get_polarity_port(uint tca_index);
uint8_t tca_get_polarity_port_low(uint tca_index);
uint8_t tca_get_polarity_port_high(uint tca_index);

void tca_set_output_port(uint tca_index, uint16_t output_state);
void tca_set_output_port_low(uint tca_index, uint8_t output_state);
void tca_set_output_port_high(uint tca_index, uint8_t output_state);

void tca_set_config_port(uint tca_index, uint16_t config_state);
void tca_set_config_port_low(uint tca_index, uint8_t config_state);
void tca_set_config_port_high(uint tca_index, uint8_t config_state);

void tca_set_polarity_port(uint tca_index, uint16_t polarity_state);
void tca_set_polarity_port_low(uint tca_index, uint8_t polarity_state);
void tca_set_polarity_port_high(uint tca_index, uint8_t polarity_state);

void tca_change_output_mask(uint8_t chip, uint16_t mask, uint16_t state);



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
