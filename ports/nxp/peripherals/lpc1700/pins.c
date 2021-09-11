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

#include "py/obj.h"
#include "py/mphal.h"
#include "peripherals/pins.h"
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"


// ... CAN1
PIN(0,0); // RD1
PIN(0,1); // TD1

// ... UART0
PIN(0,2); // TXD0
PIN(0,3); // RXD0

// ... SPI0
PIN(0,15); // SCK
PIN(0,16); // SSEL
PIN(0,17); // MISO
PIN(0,18); // MOSI

// ... I2C0
PIN(0,27); // SDA
PIN(0,28); // SCL

// GPIO
PIN(1,28);  // LED1
PIN(1,29);  // LED2

// ... UART1
PIN(2,0); // TXD1
PIN(2,1); // RXD1


int gpio_pin_init(uint8_t port, uint8_t number, gpio_pin_config_t *config) {
    gpio_pin_mode_t mode = config->pinMode;

    const uint8_t open_drain = (GPIO_Mode_OpenDrain == mode) ? PIN_PINMODE_OPENDRAIN : PIN_PINMODE_NORMAL;
    uint8_t pin_mode = PIN_PINMODE_PULLUP;
    if (GPIO_Mode_PullDown == mode) {
        pin_mode = PIN_PINMODE_PULLDOWN;
    } else if (GPIO_Mode_PullNone == mode) {
        pin_mode = PIN_PINMODE_TRISTATE;
    }

    PIN_Configure(port, number, PIN_FUNC_0, pin_mode, open_drain);

    if (config->input) {
        GPIO_SetDir(port, number, GPIO_DIR_INPUT);
    } else {
        gpio_pin_write(port, number, config->outputLogic);
        GPIO_SetDir(port, number, GPIO_DIR_OUTPUT);
    }

    return 0;
}


int gpio_pin_dir(uint8_t port, uint8_t number, bool input) {
    if (input) {
        GPIO_SetDir(port, number, GPIO_DIR_INPUT);
    } else {
        GPIO_SetDir(port, number, GPIO_DIR_OUTPUT);
    }

    return 0;
}


int gpio_pin_write(uint8_t port, uint8_t number, bool value) {
    if (value) {
        GPIO_PinWrite(port, number, 1U);
    } else {
        GPIO_PinWrite(port, number, 0U);
    }

    return 0;
}


bool gpio_pin_read(uint8_t port, uint8_t number) {
    bool value = (bool)GPIO_PinRead(port, number);

    return value;
}
