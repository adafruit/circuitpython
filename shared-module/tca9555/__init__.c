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

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/tca9555/__init__.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-module/tca9555/__init__.h"

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_TCA9555, Enable/disable assertions in the pico_tca9555 module, type=bool, default=0, group=pico_tca9555
#ifndef PARAM_ASSERTIONS_ENABLED_TCA9555
#define PARAM_ASSERTIONS_ENABLED_TCA9555 0
#endif

static const uint8_t tca9555_addresses[TCA9555_CHIP_COUNT] = TCA9555_CHIP_ADDRESSES;
#if TCA9555_LOCAL_MEMORY
uint8_t tca9555_output_state[TCA9555_CHIP_COUNT * 2] = {0};
uint8_t tca9555_config_state[TCA9555_CHIP_COUNT * 2] = {0};
uint8_t tca9555_polarity_state[TCA9555_CHIP_COUNT * 2] = {0};
#endif

bool common_hal_tca_gpio_get_input(uint tca_gpio) {
    invalid_params_if(TCA9555, tca_gpio >= TCA9555_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? INPUT_PORT1 : INPUT_PORT0;
    uint8_t input_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &input_state, 1);
    return (input_state & GPIO_BIT_MASK(tca_gpio)) != 0;
}

bool common_hal_tca_gpio_get_output(uint tca_gpio) {
    invalid_params_if(TCA9555, tca_gpio >= TCA9555_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? OUTPUT_PORT1 : OUTPUT_PORT0;
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &output_state, 1);
    #if TCA9555_LOCAL_MEMORY
    tca9555_output_state[GPIO_BYTE(tca_gpio)] = output_state;
    #endif
    return (output_state & GPIO_BIT_MASK(tca_gpio)) != 0;
}

bool common_hal_tca_gpio_get_config(uint tca_gpio) {
    invalid_params_if(TCA9555, tca_gpio >= TCA9555_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? CONFIGURATION_PORT1 : CONFIGURATION_PORT0;
    uint8_t config_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &config_state, 1);
    #if TCA9555_LOCAL_MEMORY
    tca9555_config_state[GPIO_BYTE(tca_gpio)] = config_state;
    #endif
    return (config_state & GPIO_BIT_MASK(tca_gpio)) == 0;
}

bool common_hal_tca_gpio_get_polarity(uint tca_gpio) {
    invalid_params_if(TCA9555, tca_gpio >= TCA9555_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? POLARITY_PORT1 : POLARITY_PORT0;
    uint8_t polarity_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, address, &reg, 1, &polarity_state, 1);
    #if TCA9555_LOCAL_MEMORY
    tca9555_polarity_state[GPIO_BYTE(tca_gpio)] = polarity_state;
    #endif
    return (polarity_state & GPIO_BIT_MASK(tca_gpio)) != 0;
}

void common_hal_tca_gpio_set_output(uint tca_gpio, bool value) {
    invalid_params_if(TCA9555, tca_gpio >= TCA9555_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? OUTPUT_PORT1 : OUTPUT_PORT0;
    #if TCA9555_LOCAL_MEMORY
    uint8_t output_state = tca9555_output_state[GPIO_BYTE(tca_gpio)];
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
        #if TCA9555_LOCAL_MEMORY
        tca9555_output_state[GPIO_BYTE(tca_gpio)] = new_output_state;
        #endif
    }
}

void common_hal_tca_gpio_set_config(uint tca_gpio, bool output) {
    invalid_params_if(TCA9555, tca_gpio >= TCA9555_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? CONFIGURATION_PORT1 : CONFIGURATION_PORT0;
    #if TCA9555_LOCAL_MEMORY
    uint8_t config_state = tca9555_config_state[GPIO_BYTE(tca_gpio)];
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
        #if TCA9555_LOCAL_MEMORY
        tca9555_config_state[GPIO_BYTE(tca_gpio)] = new_config_state;
        #endif
    }
}

void common_hal_tca_gpio_set_polarity(uint tca_gpio, bool polarity) {
    invalid_params_if(TCA9555, tca_gpio >= TCA9555_VIRTUAL_GPIO_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t address = ADDRESS_FROM_GPIO(tca_gpio);

    uint8_t reg = IS_PORT1(tca_gpio) ? POLARITY_PORT1 : POLARITY_PORT0;
    #if TCA9555_LOCAL_MEMORY
    uint8_t polarity_state = tca9555_polarity_state[GPIO_BYTE(tca_gpio)];
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
        #if TCA9555_LOCAL_MEMORY
        tca9555_output_state[GPIO_BYTE(tca_gpio)] = new_polarity_state;
        #endif
    }
}

uint16_t common_hal_tca_get_input_port(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = INPUT_PORT0;
    uint16_t input_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, (uint8_t *)&input_state, 2);
    return input_state;
}

uint8_t common_hal_tca_get_input_port_low(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = INPUT_PORT0;
    uint8_t input_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, &input_state, 1);
    return input_state;
}

uint8_t common_hal_tca_get_input_port_high(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = INPUT_PORT1;
    uint8_t input_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, &input_state, 1);
    return input_state;
}

uint16_t common_hal_tca_get_output_port(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT0;
    uint16_t output_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, (uint8_t *)&output_state, 2);
    #if TCA9555_LOCAL_MEMORY
    tca9555_output_state[HIGH_BYTE(tca_index)] = (output_state >> 8);
    tca9555_output_state[LOW_BYTE(tca_index)] = (output_state & 0xFF);
    #endif
    return output_state;
}

uint8_t common_hal_tca_get_output_port_low(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT0;
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, &output_state, 1);
    #if TCA9555_LOCAL_MEMORY
    tca9555_output_state[LOW_BYTE(tca_index)] = output_state;
    #endif
    return output_state;
}

uint8_t common_hal_tca_get_output_port_high(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = OUTPUT_PORT1;
    uint8_t output_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, &output_state, 1);
    #if TCA9555_LOCAL_MEMORY
    tca9555_output_state[HIGH_BYTE(tca_index)] = output_state;
    #endif
    return output_state;
}

uint16_t common_hal_tca_get_config_port(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = CONFIGURATION_PORT0;
    uint16_t config_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, (uint8_t *)&config_state, 2);
    #if TCA9555_LOCAL_MEMORY
    tca9555_config_state[HIGH_BYTE(tca_index)] = (config_state >> 8);
    tca9555_config_state[LOW_BYTE(tca_index)] = (config_state & 0xFF);
    #endif
    return config_state;
}

uint8_t common_hal_tca_get_config_port_low(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = CONFIGURATION_PORT0;
    uint8_t config_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, &config_state, 1);
    #if TCA9555_LOCAL_MEMORY
    tca9555_config_state[LOW_BYTE(tca_index)] = config_state;
    #endif
    return config_state;
}

uint8_t common_hal_tca_get_config_port_high(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = CONFIGURATION_PORT1;
    uint8_t config_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, &config_state, 1);
    #if TCA9555_LOCAL_MEMORY
    tca9555_config_state[HIGH_BYTE(tca_index)] = config_state;
    #endif
    return config_state;
}

uint16_t common_hal_tca_get_polarity_port(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = POLARITY_PORT0;
    uint16_t polarity_state = 0x0000;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, (uint8_t *)&polarity_state, 2);
    #if TCA9555_LOCAL_MEMORY
    tca9555_polarity_state[HIGH_BYTE(tca_index)] = (polarity_state >> 8);
    tca9555_polarity_state[LOW_BYTE(tca_index)] = (polarity_state & 0xFF);
    #endif
    return polarity_state;
}

uint8_t common_hal_tca_get_polarity_port_low(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = POLARITY_PORT0;
    uint8_t polarity_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, &polarity_state, 1);
    #if TCA9555_LOCAL_MEMORY
    tca9555_polarity_state[LOW_BYTE(tca_index)] = polarity_state;
    #endif
    return polarity_state;
}

uint8_t common_hal_tca_get_polarity_port_high(uint tca_index) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);

    uint8_t reg = POLARITY_PORT1;
    uint8_t polarity_state = 0x00;
    common_hal_busio_i2c_write_read(i2c, tca9555_addresses[tca_index], &reg, 1, &polarity_state, 1);
    #if TCA9555_LOCAL_MEMORY
    tca9555_polarity_state[HIGH_BYTE(tca_index)] = polarity_state;
    #endif
    return polarity_state;
}

void common_hal_tca_set_output_port(uint tca_index, uint16_t output_state) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { OUTPUT_PORT0, output_state & 0xFF, (output_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca9555_addresses[tca_index], reg_and_data, 3);
    #if TCA9555_LOCAL_MEMORY
    tca9555_output_state[HIGH_BYTE(tca_index)] = (output_state >> 8);
    tca9555_output_state[LOW_BYTE(tca_index)] = (output_state & 0xFF);
    #endif
}

void common_hal_tca_set_output_port_low(uint tca_index, uint8_t output_state) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { OUTPUT_PORT0, output_state };
    common_hal_busio_i2c_write(i2c, tca9555_addresses[tca_index], reg_and_data, 2);
    #if TCA9555_LOCAL_MEMORY
    tca9555_output_state[LOW_BYTE(tca_index)] = output_state;
    #endif
}

void common_hal_tca_set_output_port_high(uint tca_index, uint8_t output_state) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { OUTPUT_PORT1, output_state };
    common_hal_busio_i2c_write(i2c, tca9555_addresses[tca_index], reg_and_data, 2);
    #if TCA9555_LOCAL_MEMORY
    tca9555_output_state[HIGH_BYTE(tca_index)] = output_state;
    #endif
}

void common_hal_tca_set_config_port(uint tca_index, uint16_t config_state) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { CONFIGURATION_PORT0, config_state & 0xFF, (config_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca9555_addresses[tca_index], reg_and_data, 3);
    #if TCA9555_LOCAL_MEMORY
    tca9555_config_state[HIGH_BYTE(tca_index)] = (config_state >> 8);
    tca9555_config_state[LOW_BYTE(tca_index)] = (config_state & 0xFF);
    #endif
}

void common_hal_tca_set_config_port_low(uint tca_index, uint8_t config_state) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { CONFIGURATION_PORT0, config_state };
    common_hal_busio_i2c_write(i2c, tca9555_addresses[tca_index], reg_and_data, 2);
    #if TCA9555_LOCAL_MEMORY
    tca9555_config_state[LOW_BYTE(tca_index)] = config_state;
    #endif
}

void common_hal_tca_set_config_port_high(uint tca_index, uint8_t config_state) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { CONFIGURATION_PORT1, config_state };
    common_hal_busio_i2c_write(i2c, tca9555_addresses[tca_index], reg_and_data, 2);
    #if TCA9555_LOCAL_MEMORY
    tca9555_config_state[HIGH_BYTE(tca_index)] = config_state;
    #endif
}

void common_hal_tca_set_polarity_port(uint tca_index, uint16_t polarity_state) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[3] = { POLARITY_PORT0, polarity_state & 0xFF, (polarity_state >> 8) };
    common_hal_busio_i2c_write(i2c, tca9555_addresses[tca_index], reg_and_data, 3);
    #if TCA9555_LOCAL_MEMORY
    tca9555_polarity_state[HIGH_BYTE(tca_index)] = (polarity_state >> 8);
    tca9555_polarity_state[LOW_BYTE(tca_index)] = (polarity_state & 0xFF);
    #endif
}

void common_hal_tca_set_polarity_port_low(uint tca_index, uint8_t polarity_state) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { POLARITY_PORT0, polarity_state };
    common_hal_busio_i2c_write(i2c, tca9555_addresses[tca_index], reg_and_data, 2);
    #if TCA9555_LOCAL_MEMORY
    tca9555_polarity_state[LOW_BYTE(tca_index)] = polarity_state;
    #endif
}

void common_hal_tca_set_polarity_port_high(uint tca_index, uint8_t polarity_state) {
    invalid_params_if(TCA9555, tca_index >= TCA9555_CHIP_COUNT);
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    uint8_t reg_and_data[2] = { POLARITY_PORT1, polarity_state };
    common_hal_busio_i2c_write(i2c, tca9555_addresses[tca_index], reg_and_data, 2);
    #if TCA9555_LOCAL_MEMORY
    tca9555_polarity_state[HIGH_BYTE(tca_index)] = polarity_state;
    #endif
}

void common_hal_tca_change_output_mask(uint8_t chip, uint16_t mask, uint16_t state) {
    uint8_t low_mask = (uint8_t)(mask & 0xFF);
    uint8_t low_state = (uint8_t)(state & 0xFF);
    uint8_t high_mask = (uint8_t)(mask >> 8);
    uint8_t high_state = (uint8_t)(state >> 8);
    bool low_changed = low_mask > 0;
    bool high_changed = high_mask > 0;
    if (low_changed && high_changed) {
        #if TCA9555_LOCAL_MEMORY
        uint16_t output_state = (tca9555_output_state[HIGH_BYTE(chip)] << 8) | tca9555_output_state[LOW_BYTE(chip)];
        #else
        uint16_t output_state = common_hal_tca_get_output_port(chip);
        #endif
        uint16_t new_output_state = output_state;
        new_output_state &= ~mask; // Clear the mask bits
        new_output_state |= state; // Set the state bits
        if (new_output_state != output_state) {
            common_hal_tca_set_output_port(chip, new_output_state);
        }
    } else if (low_changed) {
        #if TCA9555_LOCAL_MEMORY
        uint8_t output_state = tca9555_output_state[LOW_BYTE(chip)];
        #else
        uint8_t output_state = common_hal_tca_get_output_port_low(chip);
        #endif
        uint8_t new_output_state = (output_state & ~low_mask) | low_state;
        if (new_output_state != output_state) {
            common_hal_tca_set_output_port_low(chip, new_output_state);
        }
    } else if (high_changed) {
        #if TCA9555_LOCAL_MEMORY
        uint8_t output_state = tca9555_output_state[HIGH_BYTE(chip)];
        #else
        uint8_t output_state = common_hal_tca_get_output_port_high(chip);
        #endif
        uint8_t new_output_state = (output_state & ~high_mask) | high_state;
        if (new_output_state != output_state) {
            common_hal_tca_set_output_port_high(chip, new_output_state);
        }
    }
}

void common_hal_tca_change_config_mask(uint8_t chip, uint16_t mask, uint16_t state) {
    uint8_t low_mask = (uint8_t)(mask & 0xFF);
    uint8_t low_state = (uint8_t)(state & 0xFF);
    uint8_t high_mask = (uint8_t)(mask >> 8);
    uint8_t high_state = (uint8_t)(state >> 8);
    bool low_changed = low_mask > 0;
    bool high_changed = high_mask > 0;
    if (low_changed && high_changed) {
        #if TCA9555_LOCAL_MEMORY
        uint16_t config_state = (tca9555_config_state[HIGH_BYTE(chip)] << 8) | tca9555_config_state[LOW_BYTE(chip)];
        #else
        uint16_t config_state = common_hal_tca_get_config_port(chip);
        #endif
        uint16_t new_config_state = config_state;
        new_config_state &= ~mask; // Clear the mask bits
        new_config_state |= state; // Set the state bits
        if (new_config_state != config_state) {
            common_hal_tca_set_config_port(chip, new_config_state);
        }
    } else if (low_changed) {
        #if TCA9555_LOCAL_MEMORY
        uint8_t config_state = tca9555_config_state[LOW_BYTE(chip)];
        #else
        uint8_t config_state = common_hal_tca_get_config_port_low(chip);
        #endif
        uint8_t new_config_state = (config_state & ~low_mask) | low_state;
        if (new_config_state != config_state) {
            common_hal_tca_set_config_port_low(chip, new_config_state);
        }
    } else if (high_changed) {
        #if TCA9555_LOCAL_MEMORY
        uint8_t config_state = tca9555_config_state[HIGH_BYTE(chip)];
        #else
        uint8_t config_state = common_hal_tca_get_config_port_high(chip);
        #endif
        uint8_t new_config_state = (config_state & ~high_mask) | high_state;
        if (new_config_state != config_state) {
            common_hal_tca_set_config_port_high(chip, new_config_state);
        }
    }
}

void common_hal_tca_change_polarity_mask(uint8_t chip, uint16_t mask, uint16_t state) {
    uint8_t low_mask = (uint8_t)(mask & 0xFF);
    uint8_t low_state = (uint8_t)(state & 0xFF);
    uint8_t high_mask = (uint8_t)(mask >> 8);
    uint8_t high_state = (uint8_t)(state >> 8);
    bool low_changed = low_mask > 0;
    bool high_changed = high_mask > 0;
    if (low_changed && high_changed) {
        #if TCA9555_LOCAL_MEMORY
        uint16_t polarity_state = (tca9555_polarity_state[HIGH_BYTE(chip)] << 8) | tca9555_polarity_state[LOW_BYTE(chip)];
        #else
        uint16_t polarity_state = common_hal_tca_get_polarity_port(chip);
        #endif
        uint16_t new_polarity_state = polarity_state;
        new_polarity_state &= ~mask; // Clear the mask bits
        new_polarity_state |= state; // Set the state bits
        if (new_polarity_state != polarity_state) {
            common_hal_tca_set_polarity_port(chip, new_polarity_state);
        }
    } else if (low_changed) {
        #if TCA9555_LOCAL_MEMORY
        uint8_t polarity_state = tca9555_polarity_state[LOW_BYTE(chip)];
        #else
        uint8_t polarity_state = common_hal_tca_get_polarity_port_low(chip);
        #endif
        uint8_t new_polarity_state = (polarity_state & ~low_mask) | low_state;
        if (new_polarity_state != polarity_state) {
            common_hal_tca_set_polarity_port_low(chip, new_polarity_state);
        }
    } else if (high_changed) {
        #if TCA9555_LOCAL_MEMORY
        uint8_t polarity_state = tca9555_polarity_state[HIGH_BYTE(chip)];
        #else
        uint8_t polarity_state = common_hal_tca_get_polarity_port_high(chip);
        #endif
        uint8_t new_polarity_state = (polarity_state & ~high_mask) | high_state;
        if (new_polarity_state != polarity_state) {
            common_hal_tca_set_polarity_port_high(chip, new_polarity_state);
        }
    }
}
