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

#include <boards/lpc55s28_evk/pin_mux.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "peripherals/pins.h"
#include "fsl_common.h"
#include "fsl_iocon.h"
#include "fsl_gpio_cmsis.h"
#include "RTE_Device.h"


// FC3 SPI
PIN(0,2);   // MISO
PIN(0,3);   // MOSI
PIN(0,4);   // SSEL
PIN(0,6);   // SCK

// FC1 I2C
PIN(0,13);  // SDA
PIN(0,14);  // SCL

// FC0 USART
PIN(0,29);  // RXD
PIN(0,30);  // TXD

// ... GPIO
PIN(1,4);   // LED Blue
PIN(1,6);   // LED Red
PIN(1,7);   // LED Green


#if defined(RTE_GPIO_PORT0) && RTE_GPIO_PORT0
extern ARM_DRIVER_GPIO Driver_GPIO_PORT0;
#else
#define Driver_GPIO_PORT0 *(ARM_DRIVER_GPIO *)(NULL)
#endif

#if defined(RTE_GPIO_PORT1) && RTE_GPIO_PORT1
extern ARM_DRIVER_GPIO Driver_GPIO_PORT1;
#else
#define Driver_GPIO_PORT1 *(ARM_DRIVER_GPIO *)(NULL)
#endif


gpio_port_obj_t gpio_ports[2U] =
{
    // P0
    {
        .available_pin_mask = 0xFFFFFFFFU,
        .reserved_pin_mask = 0U,
        .used_pin_mask = 0U,
    },

    // P1
    {
        .available_pin_mask = 0x000000D0U,
        .reserved_pin_mask = 0xFFFFFF2FU,
        .used_pin_mask = 0U,
    },
};


int gpio_pin_init(uint8_t port, uint8_t number, gpio_pin_config_t *config) {
    const size_t port_cnt = sizeof(gpio_ports) / sizeof(gpio_ports[0U]);
    ARM_DRIVER_GPIO *gpio_port = NULL;

    int rc = -1;

    if (port < port_cnt) {
        if (0U == port) {
            gpio_port = &Driver_GPIO_PORT0;
        } else if (1U == port) {
            gpio_port = &Driver_GPIO_PORT1;
        }

        gpio_port->Initialize();

        const uint32_t pin_config = (/* Pin is configured as GPIO */
            IOCON_PIO_FUNC_GPIO |
            /* No addition pin function */
            IOCON_PIO_MODE_INACT |
            /* Standard mode, output slew rate control is enabled */
            IOCON_PIO_SLEW_STANDARD |
            /* Input function is not inverted */
            IOCON_PIO_INV_DI |
            /* Enables digital function */
            IOCON_PIO_DIGITAL_EN |
            /* Open drain is disabled */
            IOCON_PIO_OPENDRAIN_DI);

        IOCON_PinMuxSet(IOCON, port, number, pin_config);

        rc = 0;
    }

    return rc;
}


int gpio_pin_dir(uint8_t port, uint8_t number, bool input) {
    if (input) {
        Driver_GPIO_PORT1.InitPinAsInput(number, ARM_GPIO_INTERRUPT_NONE, NULL);
    } else {
        Driver_GPIO_PORT1.InitPinAsOutput(number, 1U);
    }

    return 0;
}


int gpio_pin_write(uint8_t port, uint8_t number, bool value) {
    Driver_GPIO_PORT1.PinWrite(number, value);

    return 0;
}


bool gpio_pin_read(uint8_t port, uint8_t number) {
    bool value = (bool)Driver_GPIO_PORT1.PinRead(number);

    return value;
}
