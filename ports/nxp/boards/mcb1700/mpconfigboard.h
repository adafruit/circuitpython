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

#if !defined(PORTS_NXP_BOARDS_MCB1700_MPCONFIGBOARD_H_)
#define PORTS_NXP_BOARDS_MCB1700_MPCONFIGBOARD_H_


#define MICROPY_HW_BOARD_NAME "Keil MCB1700"
#define MICROPY_HW_MCU_NAME "lpc1768"

#define CIRCUITPY_INTERNAL_NVM_SIZE                 (0)
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE    (128 * 1024)
#define SRAM_ADDR                                   (0x10000000)
#define SRAM_SIZE                                   (32 * 1024)
#define AHBRAM_ADDR                                 (0x2007c000)
#define AHBRAM_SIZE                                 (32 * 1024)
#define RAM_ADDR                                    (SRAM_ADDR)
#define RAM_SIZE                                    (SRAM_SIZE + AHBRAM_SIZE)
#define CIRCUITPY_DEFAULT_STACK_SIZE                (8 * 1024)
#define CIRCUITPY_DEFAULT_HEAP_SIZE                 (AHBRAM_SIZE - CIRCUITPY_DEFAULT_STACK_SIZE)

#define DEBUG_UART_TX                               (&pin_P2_0)
#define DEBUG_UART_RX                               (&pin_P0_2)

#define CIRCUITPY_STATUS_LED_POWER                  (&pin_P2_6)
#define MICROPY_HW_LED_STATUS                       (&pin_P2_5)
#define MICROPY_HW_LED_TX                           (&pin_P2_4)
#define MICROPY_HW_LED_RX                           (&pin_P2_3)

#endif // PORTS_NXP_BOARDS_MCB1700_MPCONFIGBOARD_H_
