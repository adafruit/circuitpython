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
#include "shared-module/displayio/__init__.h"

void board_init(void) {
}

void board_deinit(void) {
}

void reset_board(void) {
    tca_gpio_set_output_port(0x20, 0x0000);
    tca_gpio_set_polarity_port(0x20, 0x0000);
    tca_gpio_set_dir_port(0x20, 0x07BF);

    tca_gpio_set_output_port(0x26, 0x0000);
    tca_gpio_set_polarity_port(0x26, 0x0000);
    tca_gpio_set_dir_port(0x26, 0xFCE6);

    // Releasing displays, as if one is set up with the
    // intended IO expander LCD pins then it will carry over
    // into other user program runs, causing poor performance
    // if they don't realise the screen is still being driven
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
