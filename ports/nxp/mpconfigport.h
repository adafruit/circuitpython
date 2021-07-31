/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#ifndef __INCLUDED_MPCONFIGPORT_H
#define __INCLUDED_MPCONFIGPORT_H

#include "py/circuitpy_mpconfig.h"

#define CIRCUITPY_INTERNAL_CONFIG_SIZE      (0)

#ifndef CIRCUITPY_INTERNAL_NVM_SIZE
#define CIRCUITPY_INTERNAL_NVM_SIZE         (0)
#endif

#define MICROPY_PORT_ROOT_POINTERS \
    CIRCUITPY_COMMON_ROOT_POINTERS \

#define CIRCUITPY_DEFAULT_STACK_SIZE        (24 * 1024)

#if (1)
#define BOOTLOADER_SIZE                  (0 * 1024)
#else
#define BOOTLOADER_SIZE                  (16 * 1024)
#endif

// FIXME: Flash size is chip specific
#define FLASH_PAGE_SIZE                     (256)
#define FLASH_SIZE                          (512 * 1024)

// FIXME: RAM size is chip specific
// #define RAM_SIZE                            (64 * 1024)

// FIXME: Erase size is sector specific for LPC17
#define FLASH_ERASE_SIZE                    (8192)

// FIXME: Number of sectors is chip specific
#define INTERNAL_FLASH_PART1_NUM_BLOCKS (CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE / FILESYSTEM_BLOCK_SIZE)


// Flash layout, starting at 0x00000000
//
// - bootloader (8 or 16kB)
// - firmware
// - internal CIRCUITPY flash filesystem (optional)
// - internal config, used to store crystalless clock calibration info (optional)
// - microntroller.nvm (optional)

// Define these regions starting up from the bottom of flash:

#define BOOTLOADER_START_ADDR          (0x00000000)

#define CIRCUITPY_FIRMWARE_START_ADDR  (BOOTLOADER_START_ADDR + BOOTLOADER_SIZE)

// Define these regions start down from the top of flash:

#define CIRCUITPY_INTERNAL_NVM_START_ADDR \
    (FLASH_SIZE - CIRCUITPY_INTERNAL_NVM_SIZE)

#define CIRCUITPY_INTERNAL_CONFIG_START_ADDR \
    (CIRCUITPY_INTERNAL_NVM_START_ADDR - CIRCUITPY_INTERNAL_CONFIG_SIZE)

#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR \
    (CIRCUITPY_INTERNAL_CONFIG_START_ADDR - CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE)

// The firmware space is the space left over between the fixed lower and upper regions.
#define CIRCUITPY_FIRMWARE_SIZE \
    (CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR - CIRCUITPY_FIRMWARE_START_ADDR)

#if BOOTLOADER_START_ADDR % FLASH_PAGE_SIZE != 0
#error BOOTLOADER_START_ADDR must be on a flash page boundary.
#endif

#if CIRCUITPY_INTERNAL_NVM_START_ADDR % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_NVM_START_ADDR must be on a flash erase (row or block) boundary.
#endif
#if CIRCUITPY_INTERNAL_NVM_SIZE % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_NVM_SIZE must be a multiple of FLASH_ERASE_SIZE.
#endif

#if CIRCUITPY_INTERNAL_CONFIG_START_ADDR % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_CONFIG_SIZE must be on a flash erase (row or block) boundary.
#endif
#if CIRCUITPY_INTERNAL_CONFIG_SIZE % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_CONFIG_SIZE must be a multiple of FLASH_ERASE_SIZE.
#endif

#if CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE must be on a flash erase (row or block) boundary.
#endif
#if CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE must be a multiple of FLASH_ERASE_SIZE.
#endif

#if CIRCUITPY_FIRMWARE_SIZE < 0
#error No space left in flash for firmware after specifying other regions!
#endif


#endif  // __INCLUDED_MPCONFIGPORT_H
