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

#include "supervisor/board.h"

#include "py/gc.h"
#include "py/runtime.h"

#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/board/__init__.h"
#include "bindings/tca9555r/__init__.h"

void board_init(void) {
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    common_hal_busio_i2c_never_reset(i2c);
    // Our object is statically allocated off the heap so make sure the bus object lives to the end
    // of the heap as well.
    gc_never_free(i2c);

    // Probe the bus to see if a device acknowledges the given address.
    if (!common_hal_busio_i2c_probe(i2c, 0x20)) {
        //mp_raise_ValueError_varg(translate("Unable to find I2C Expander at %x"), 0x20);
        return;
    }

    if (!common_hal_busio_i2c_probe(i2c, 0x26)) {
        //mp_raise_ValueError_varg(translate("Unable to find I2C Expander at %x"), 0x26);
        return;
    }

    tca_gpio_get_dir_port(0x20);
    tca_gpio_get_polarity_port(0x20);
    tca_gpio_get_input_port(0x20);
    tca_gpio_get_output_port(0x20);

    tca_gpio_set_output_port(0x20, 0x0000);
    if(tca_gpio_get_output_port(0x20) != 0x0000) {
        sleep_ms(60000);
    }

    tca_gpio_get_dir_port(0x26);
    tca_gpio_get_polarity_port(0x26);
    tca_gpio_get_input_port(0x26);
    tca_gpio_get_output_port(0x26);

    tca_gpio_set_output_port(0x26, 0x0000);
    if(tca_gpio_get_output_port(0x26) != 0x0000) {
        sleep_ms(60000);
    }

    tca_gpio_set_dir_port(0x20, 0x07BF);
    if(tca_gpio_get_dir_port(0x20) != 0x07BF) {
        sleep_ms(60000);
    }

    tca_gpio_set_dir_port(0x20, 0xFCE6);
    if(tca_gpio_get_dir_port(0x20) != 0xFCE6) {
        sleep_ms(60000);
    }
}

void board_deinit(void) {
}

void reset_board(void) {
    tca_gpio_set_output_port(0x20, 0x0000);
    tca_gpio_set_dir_port(0x20, 0x07BF);

    tca_gpio_set_output_port(0x26, 0x0000);
    tca_gpio_set_dir_port(0x20, 0xFCE6);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
