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


// ... GPIO0
PIN(0,0);   // CAN RD1
PIN(0,1);   // CAN TD1
PIN(0,2);   // UART0 TXD0
PIN(0,3);   // UART0 RXD0
PIN(0,4);   //
PIN(0,5);   //
PIN(0,6);   //
PIN(0,7);   //
PIN(0,8);   //
PIN(0,9);   //
PIN(0,10);  //
PIN(0,11);  //
PIN(0,12);  //
PIN(0,13);  //
PIN(0,14);  //
PIN(0,15);  // SPI0 SCK
PIN(0,16);  // SPI0 SSEL
PIN(0,17);  // SPI0 MISO
PIN(0,18);  // SPI0 MOSI
PIN(0,19);  //
PIN(0,20);  //
PIN(0,21);  //
PIN(0,22);  //
PIN(0,23);  //
PIN(0,24);  //
PIN(0,25);  //
PIN(0,26);  //
PIN(0,27);  // I2C0 SDA
PIN(0,28);  // I2C0 SCL
PIN(0,29);  //
PIN(0,30);  //
PIN(0,31);  //

// ... GPIO1
PIN(1,0);   //
PIN(1,1);   //
PIN(1,2);   //
PIN(1,3);   //
PIN(1,4);   //
PIN(1,5);   //
PIN(1,6);   //
PIN(1,7);   //
PIN(1,8);   //
PIN(1,9);   //
PIN(1,10);  //
PIN(1,11);  //
PIN(1,12);  //
PIN(1,13);  //
PIN(1,14);  //
PIN(1,15);  //
PIN(1,16);  //
PIN(1,17);  //
PIN(1,18);  //
PIN(1,19);  //
PIN(1,20);  //
PIN(1,21);  //
PIN(1,22);  //
PIN(1,23);  //
PIN(1,24);  //
PIN(1,25);  //
PIN(1,26);  //
PIN(1,27);  //
PIN(1,28);  //
PIN(1,29);  //
PIN(1,30);  //
PIN(1,31);  //

// ... GPIO2
PIN(2,0);   // UART1 TXD1
PIN(2,1);   // UARt1 RXD1
PIN(2,2);   //
PIN(2,3);   //
PIN(2,4);   //
PIN(2,5);   //
PIN(2,6);   //
PIN(2,7);   //
PIN(2,8);   //
PIN(2,9);   //
PIN(2,10);  //
PIN(2,11);  //
PIN(2,12);  //
PIN(2,13);  //
PIN(2,14);  //
PIN(2,15);  //
PIN(2,16);  //
PIN(2,17);  //
PIN(2,18);  //
PIN(2,19);  //
PIN(2,20);  //
PIN(2,21);  //
PIN(2,22);  //
PIN(2,23);  //
PIN(2,24);  //
PIN(2,25);  //
PIN(2,26);  //
PIN(2,27);  //
PIN(2,28);  //
PIN(2,29);  //
PIN(2,30);  //
PIN(2,31);  //

// ... GPIO3
PIN(3,25);
PIN(3,26);

// ... GPIO4
PIN(4,0);   //
PIN(4,1);   //
PIN(4,2);   //
PIN(4,3);   //
PIN(4,4);   //
PIN(4,5);   //
PIN(4,6);   //
PIN(4,7);   //
PIN(4,8);   //
PIN(4,9);   //
PIN(4,10);  //
PIN(4,11);  //
PIN(4,12);  //
PIN(4,13);  //
PIN(4,14);  //
PIN(4,15);  //
PIN(4,16);  //
PIN(4,17);  //
PIN(4,18);  //
PIN(4,19);  //
PIN(4,20);  //
PIN(4,21);  //
PIN(4,22);  //
PIN(4,23);  //
PIN(4,24);  //
PIN(4,25);  //
PIN(4,26);  //
PIN(4,27);  //
PIN(4,28);  //
PIN(4,29);  //
PIN(4,30);  //
PIN(4,31);  //


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
