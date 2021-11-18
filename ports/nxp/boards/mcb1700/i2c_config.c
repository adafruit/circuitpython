/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/busio/I2C.h"
#include "boards/mcb1700/i2c_config.h"
#include "boards/mcb1700/CMSIS/Driver/Config/RTE_Device.h"

#include "device.h"


extern ARM_DRIVER_I2C Driver_I2C0;
extern ARM_DRIVER_I2C Driver_I2C1;
extern ARM_DRIVER_I2C Driver_I2C2;

const static i2c_pin_set_t I2C0_pin_set[] =
{
    /* Pin set 0 */
    {
        .scl = NXP_PORT_GPIO_PIN(RTE_I2C0_SCL_PORT, RTE_I2C0_SCL_PIN),
        .sda = NXP_PORT_GPIO_PIN(RTE_I2C0_SDA_PORT, RTE_I2C0_SDA_PORT),
    },
};

const static i2c_pin_set_t I2C1_pin_set[] =
{
    /* Pin set 0 */
    {
        .scl = NXP_PORT_GPIO_PIN(RTE_I2C1_SCL_PORT, RTE_I2C1_SCL_PIN),
        .sda = NXP_PORT_GPIO_PIN(RTE_I2C1_SDA_PORT, RTE_I2C1_SDA_PORT),
    },
};

const static i2c_pin_set_t I2C2_pin_set[] =
{
    /* Pin set 0 */
    {
        .scl = NXP_PORT_GPIO_PIN(RTE_I2C2_SCL_PORT, RTE_I2C2_SCL_PIN),
        .sda = NXP_PORT_GPIO_PIN(RTE_I2C2_SDA_PORT, RTE_I2C2_SDA_PORT),
    },
};

i2c_inst_t i2c_instances[I2C_INSTANCES_NUM] = {
    /* I2C Instance 0 */
    {
        .id = 0U,
        .is_used = false,
        .driver = &Driver_I2C0,
        .pin_map = &I2C0_pin_set[0U],
        .pin_map_len = MP_ARRAY_SIZE(I2C0_pin_set),
    },

    /* I2C Instance 1 */
    {
        .id = 1U,
        .is_used = false,
        .driver = &Driver_I2C1,
        .pin_map = &I2C1_pin_set[0U],
        .pin_map_len = MP_ARRAY_SIZE(I2C1_pin_set),
    },

    /* I2C Instance 2 */
    {
        .id = 2U,
        .is_used = false,
        .driver = &Driver_I2C2,
        .pin_map = &I2C2_pin_set[0U],
        .pin_map_len = MP_ARRAY_SIZE(I2C2_pin_set),
    },
};

#define PCI2C0  (1U << 7)
#define PCI2C1  (1U << 19)
#define PCI2C2  (1U << 26)

void i2c_enable(i2c_inst_t *i2c_instance) {
    if (0U == i2c_instance->id) {
        LPC_SC->PCONP |= PCI2C0;
    } else if (1U == i2c_instance->id) {
        LPC_SC->PCONP |= PCI2C1;
    } else if (2U == i2c_instance->id) {
        LPC_SC->PCONP |= PCI2C2;
    }

    return;
}

void i2c_disable(i2c_inst_t *i2c_instance) {
    if (0U == i2c_instance->id) {
        LPC_SC->PCONP &= ~PCI2C0;
    } else if (1U == i2c_instance->id) {
        LPC_SC->PCONP &= ~PCI2C1;
    } else if (2U == i2c_instance->id) {
        LPC_SC->PCONP &= ~PCI2C2;
    }

    return;
}
