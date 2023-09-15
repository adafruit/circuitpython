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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_TCA9555___INIT___H
#define MICROPY_INCLUDED_SHARED_MODULE_TCA9555___INIT___H

#define INPUT_PORT0  0x00
#define INPUT_PORT1  0x01
#define OUTPUT_PORT0  0x02
#define OUTPUT_PORT1  0x03
#define POLARITY_PORT0  0x04
#define POLARITY_PORT1  0x05
#define CONFIGURATION_PORT0  0x06
#define CONFIGURATION_PORT1  0x07

#define TCA9555_GPIO_COUNT     16
#define TCA9555_VIRTUAL_GPIO_COUNT     (TCA9555_GPIO_COUNT * TCA9555_CHIP_COUNT)

#ifndef TCA9555_LOCAL_MEMORY
#define TCA9555_LOCAL_MEMORY (0)
#endif

#ifndef TCA9555_READ_INTERNALS
#define TCA9555_READ_INTERNALS (0)
#endif

#if TCA9555_LOCAL_MEMORY
extern uint8_t tca9555_output_state[TCA9555_CHIP_COUNT * 2];
extern uint8_t tca9555_config_state[TCA9555_CHIP_COUNT * 2];
extern uint8_t tca9555_polarity_state[TCA9555_CHIP_COUNT * 2];
#endif

#define HIGH_BYTE(index) (((index) * 2u) + 1u)
#define LOW_BYTE(index) (((index) * 2u))
#define IS_PORT1(gpio) (((gpio) % TCA9555_GPIO_COUNT) >= 8u)
#define GPIO_BYTE(gpio) ((gpio) >> 3u)
#define GPIO_BIT_MASK(gpio) (1u << ((gpio) % 8u))
#define CHIP_FROM_GPIO(gpio) ((gpio) / TCA9555_GPIO_COUNT)
#define ADDRESS_FROM_GPIO(gpio) (tca9555_addresses[CHIP_FROM_GPIO(gpio)])

#endif  // MICROPY_INCLUDED_SHARED_MODULE_TCA9555___INIT___H
