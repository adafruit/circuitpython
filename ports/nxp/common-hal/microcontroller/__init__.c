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

#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/microcontroller/Processor.h"

#if CIRCUITPY_NVM
#include "shared-bindings/nvm/ByteArray.h"
#endif
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"

#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"

#include "device.h"

static volatile uint32_t nesting_count = 0;

static void __mcu_reset(void) {
    filesystem_flush();
    NVIC_SystemReset();
}

extern uint32_t _srelocate;
bool bootloader_available(void) {
    #if (1)
    return false;
    #else
    return &_bootloader_dbl_tap >= &_srelocate;
    #endif
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    #if (1)
    return;
    #else
    if (runmode == RUNMODE_BOOTLOADER) {
        if (!bootloader_available()) {
            mp_raise_ValueError(translate("Cannot reset into bootloader because no bootloader is present."));
        }
        // Pretend to be the first of the two reset presses needed to enter the
        // bootloader. That way one reset will end in the bootloader.
        _bootloader_dbl_tap = DBL_TAP_MAGIC;
    } else {
        // Set up the default.
        _bootloader_dbl_tap = DBL_TAP_MAGIC_QUICK_BOOT;
    }
    if (runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(PROGRAMMATIC_SAFE_MODE);
    }
    #endif
}

void common_hal_mcu_reset(void) {
    __mcu_reset();
}

void common_hal_mcu_disable_interrupts(void) {
    __disable_irq();
    __DMB();
    nesting_count++;
}

void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        // This is very very bad because it means there was mismatched disable/enables so we
        // "HardFault".
        asm ("bkpt");
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    __DMB();
    __enable_irq();
}

void common_hal_mcu_delay_us(uint32_t delay) {
    uint32_t ticks_per_us = SystemCoreClock / 1000000UL;
    delay *= ticks_per_us;

    SysTick->VAL = 0UL;
    SysTick->LOAD = delay;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0UL) {
    }
    SysTick->CTRL = 0UL;

    return;
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

#if CIRCUITPY_INTERNAL_NVM_SIZE > 0
// The singleton nvm.ByteArray object.
const nvm_bytearray_obj_t common_hal_mcu_nvm_obj = {
    .base = {
        .type = &nvm_bytearray_type,
    },
    .len = CIRCUITPY_INTERNAL_NVM_SIZE,
    .start_address = (uint8_t *)(CIRCUITPY_INTERNAL_NVM_START_ADDR)
};
#endif

// This maps MCU pin names to pin objects.
STATIC const mp_rom_map_elem_t mcu_pin_global_dict_table[] = {
    #if defined(PIN_P0_0) && !defined(IGNORE_PIN_P0_0)
    { MP_ROM_QSTR(MP_QSTR_P0_0), MP_ROM_PTR(&pin_P0_0) },
    #endif
    #if defined(PIN_P0_1) && !defined(IGNORE_PIN_P0_1)
    { MP_ROM_QSTR(MP_QSTR_P0_1), MP_ROM_PTR(&pin_P0_1) },
    #endif
    #if defined(PIN_P0_2) && !defined(IGNORE_PIN_P0_2)
    { MP_ROM_QSTR(MP_QSTR_P0_2), MP_ROM_PTR(&pin_P0_2) },
    #endif
    #if defined(PIN_P0_3) && !defined(IGNORE_PIN_P0_3)
    { MP_ROM_QSTR(MP_QSTR_P0_3), MP_ROM_PTR(&pin_P0_3) },
    #endif
    #if defined(PIN_P0_4) && !defined(IGNORE_PIN_P0_4)
    { MP_ROM_QSTR(MP_QSTR_P0_4), MP_ROM_PTR(&pin_P0_4) },
    #endif
    #if defined(PIN_P0_5) && !defined(IGNORE_PIN_P0_5)
    { MP_ROM_QSTR(MP_QSTR_P0_5), MP_ROM_PTR(&pin_P0_5) },
    #endif
    #if defined(PIN_P0_6) && !defined(IGNORE_PIN_P0_6)
    { MP_ROM_QSTR(MP_QSTR_P0_6), MP_ROM_PTR(&pin_P0_6) },
    #endif
    #if defined(PIN_P0_7) && !defined(IGNORE_PIN_P0_7)
    { MP_ROM_QSTR(MP_QSTR_P0_7), MP_ROM_PTR(&pin_P0_7) },
    #endif
    #if defined(PIN_P0_8) && !defined(IGNORE_PIN_P0_8)
    { MP_ROM_QSTR(MP_QSTR_P0_8), MP_ROM_PTR(&pin_P0_8) },
    #endif
    #if defined(PIN_P0_9) && !defined(IGNORE_PIN_P0_9)
    { MP_ROM_QSTR(MP_QSTR_P0_9), MP_ROM_PTR(&pin_P0_9) },
    #endif
    #if defined(PIN_P0_10) && !defined(IGNORE_PIN_P0_10)
    { MP_ROM_QSTR(MP_QSTR_P0_10), MP_ROM_PTR(&pin_P0_10) },
    #endif
    #if defined(PIN_P0_11) && !defined(IGNORE_PIN_P0_11)
    { MP_ROM_QSTR(MP_QSTR_P0_11), MP_ROM_PTR(&pin_P0_11) },
    #endif
    #if defined(PIN_P0_12) && !defined(IGNORE_PIN_P0_12)
    { MP_ROM_QSTR(MP_QSTR_P0_12), MP_ROM_PTR(&pin_P0_12) },
    #endif
    #if defined(PIN_P0_13) && !defined(IGNORE_PIN_P0_13)
    { MP_ROM_QSTR(MP_QSTR_P0_13), MP_ROM_PTR(&pin_P0_13) },
    #endif
    #if defined(PIN_P0_14) && !defined(IGNORE_PIN_P0_14)
    { MP_ROM_QSTR(MP_QSTR_P0_14), MP_ROM_PTR(&pin_P0_14) },
    #endif
    #if defined(PIN_P0_15) && !defined(IGNORE_PIN_P0_15)
    { MP_ROM_QSTR(MP_QSTR_P0_15), MP_ROM_PTR(&pin_P0_15) },
    #endif
    #if defined(PIN_P0_16) && !defined(IGNORE_PIN_P0_16)
    { MP_ROM_QSTR(MP_QSTR_P0_16), MP_ROM_PTR(&pin_P0_16) },
    #endif
    #if defined(PIN_P0_17) && !defined(IGNORE_PIN_P0_17)
    { MP_ROM_QSTR(MP_QSTR_P0_17), MP_ROM_PTR(&pin_P0_17) },
    #endif
    #if defined(PIN_P0_18) && !defined(IGNORE_PIN_P0_18)
    { MP_ROM_QSTR(MP_QSTR_P0_18), MP_ROM_PTR(&pin_P0_18) },
    #endif
    #if defined(PIN_P0_19) && !defined(IGNORE_PIN_P0_19)
    { MP_ROM_QSTR(MP_QSTR_P0_19), MP_ROM_PTR(&pin_P0_19) },
    #endif
    #if defined(PIN_P0_20) && !defined(IGNORE_PIN_P0_20)
    { MP_ROM_QSTR(MP_QSTR_P0_20), MP_ROM_PTR(&pin_P0_20) },
    #endif
    #if defined(PIN_P0_21) && !defined(IGNORE_PIN_P0_21)
    { MP_ROM_QSTR(MP_QSTR_P0_21), MP_ROM_PTR(&pin_P0_21) },
    #endif
    #if defined(PIN_P0_22) && !defined(IGNORE_PIN_P0_22)
    { MP_ROM_QSTR(MP_QSTR_P0_22), MP_ROM_PTR(&pin_P0_22) },
    #endif
    #if defined(PIN_P0_23) && !defined(IGNORE_PIN_P0_23)
    { MP_ROM_QSTR(MP_QSTR_P0_23), MP_ROM_PTR(&pin_P0_23) },
    #endif
    #if defined(PIN_P0_24) && !defined(IGNORE_PIN_P0_24)
    { MP_ROM_QSTR(MP_QSTR_P0_24), MP_ROM_PTR(&pin_P0_24) },
    #endif
    #if defined(PIN_P0_25) && !defined(IGNORE_PIN_P0_25)
    { MP_ROM_QSTR(MP_QSTR_P0_25), MP_ROM_PTR(&pin_P0_25) },
    #endif
    #if defined(PIN_P0_27) && !defined(IGNORE_PIN_P0_27)
    { MP_ROM_QSTR(MP_QSTR_P0_27), MP_ROM_PTR(&pin_P0_27) },
    #endif
    #if defined(PIN_P0_28) && !defined(IGNORE_PIN_P0_28)
    { MP_ROM_QSTR(MP_QSTR_P0_28), MP_ROM_PTR(&pin_P0_28) },
    #endif
    #if defined(PIN_P0_30) && !defined(IGNORE_PIN_P0_30)
    { MP_ROM_QSTR(MP_QSTR_P0_30), MP_ROM_PTR(&pin_P0_30) },
    #endif
    #if defined(PIN_P0_31) && !defined(IGNORE_PIN_P0_31)
    { MP_ROM_QSTR(MP_QSTR_P0_31), MP_ROM_PTR(&pin_P0_31) },
    #endif

    #if defined(PIN_P1_0) && !defined(IGNORE_PIN_P1_0)
    { MP_ROM_QSTR(MP_QSTR_P1_0), MP_ROM_PTR(&pin_P1_0) },
    #endif
    #if defined(PIN_P1_1) && !defined(IGNORE_PIN_P1_1)
    { MP_ROM_QSTR(MP_QSTR_P1_1), MP_ROM_PTR(&pin_P1_1) },
    #endif
    #if defined(PIN_P1_2) && !defined(IGNORE_PIN_P1_2)
    { MP_ROM_QSTR(MP_QSTR_P1_2), MP_ROM_PTR(&pin_P1_2) },
    #endif
    #if defined(PIN_P1_3) && !defined(IGNORE_PIN_P1_3)
    { MP_ROM_QSTR(MP_QSTR_P1_3), MP_ROM_PTR(&pin_P1_3) },
    #endif
    #if defined(PIN_P1_4) && !defined(IGNORE_PIN_P1_4)
    { MP_ROM_QSTR(MP_QSTR_P1_4), MP_ROM_PTR(&pin_P1_4) },
    #endif
    #if defined(PIN_P1_5) && !defined(IGNORE_PIN_P1_5)
    { MP_ROM_QSTR(MP_QSTR_P1_5), MP_ROM_PTR(&pin_P1_5) },
    #endif
    #if defined(PIN_P1_6) && !defined(IGNORE_PIN_P1_6)
    { MP_ROM_QSTR(MP_QSTR_P1_6), MP_ROM_PTR(&pin_P1_6) },
    #endif
    #if defined(PIN_P1_7) && !defined(IGNORE_PIN_P1_7)
    { MP_ROM_QSTR(MP_QSTR_P1_7), MP_ROM_PTR(&pin_P1_7) },
    #endif
    #if defined(PIN_P1_8) && !defined(IGNORE_PIN_P1_8)
    { MP_ROM_QSTR(MP_QSTR_P1_8), MP_ROM_PTR(&pin_P1_8) },
    #endif
    #if defined(PIN_P1_9) && !defined(IGNORE_PIN_P1_9)
    { MP_ROM_QSTR(MP_QSTR_P1_9), MP_ROM_PTR(&pin_P1_9) },
    #endif
    #if defined(PIN_P1_10) && !defined(IGNORE_PIN_P1_10)
    { MP_ROM_QSTR(MP_QSTR_P1_10), MP_ROM_PTR(&pin_P1_10) },
    #endif
    #if defined(PIN_P1_11) && !defined(IGNORE_PIN_P1_11)
    { MP_ROM_QSTR(MP_QSTR_P1_11), MP_ROM_PTR(&pin_P1_11) },
    #endif
    #if defined(PIN_P1_12) && !defined(IGNORE_PIN_P1_12)
    { MP_ROM_QSTR(MP_QSTR_P1_12), MP_ROM_PTR(&pin_P1_12) },
    #endif
    #if defined(PIN_P1_13) && !defined(IGNORE_PIN_P1_13)
    { MP_ROM_QSTR(MP_QSTR_P1_13), MP_ROM_PTR(&pin_P1_13) },
    #endif
    #if defined(PIN_P1_14) && !defined(IGNORE_PIN_P1_14)
    { MP_ROM_QSTR(MP_QSTR_P1_14), MP_ROM_PTR(&pin_P1_14) },
    #endif
    #if defined(PIN_P1_15) && !defined(IGNORE_PIN_P1_15)
    { MP_ROM_QSTR(MP_QSTR_P1_15), MP_ROM_PTR(&pin_P1_15) },
    #endif
    #if defined(PIN_P1_16) && !defined(IGNORE_PIN_P1_16)
    { MP_ROM_QSTR(MP_QSTR_P1_16), MP_ROM_PTR(&pin_P1_16) },
    #endif
    #if defined(PIN_P1_17) && !defined(IGNORE_PIN_P1_17)
    { MP_ROM_QSTR(MP_QSTR_P1_17), MP_ROM_PTR(&pin_P1_17) },
    #endif
    #if defined(PIN_P1_18) && !defined(IGNORE_PIN_P1_18)
    { MP_ROM_QSTR(MP_QSTR_P1_18), MP_ROM_PTR(&pin_P1_18) },
    #endif
    #if defined(PIN_P1_19) && !defined(IGNORE_PIN_P1_19)
    { MP_ROM_QSTR(MP_QSTR_P1_19), MP_ROM_PTR(&pin_P1_19) },
    #endif
    #if defined(PIN_P1_20) && !defined(IGNORE_PIN_P1_20)
    { MP_ROM_QSTR(MP_QSTR_P1_20), MP_ROM_PTR(&pin_P1_20) },
    #endif
    #if defined(PIN_P1_21) && !defined(IGNORE_PIN_P1_21)
    { MP_ROM_QSTR(MP_QSTR_P1_21), MP_ROM_PTR(&pin_P1_21) },
    #endif
    #if defined(PIN_P1_22) && !defined(IGNORE_PIN_P1_22)
    { MP_ROM_QSTR(MP_QSTR_P1_22), MP_ROM_PTR(&pin_P1_22) },
    #endif
    #if defined(PIN_P1_23) && !defined(IGNORE_PIN_P1_23)
    { MP_ROM_QSTR(MP_QSTR_P1_23), MP_ROM_PTR(&pin_P1_23) },
    #endif
    #if defined(PIN_P1_24) && !defined(IGNORE_PIN_P1_24)
    { MP_ROM_QSTR(MP_QSTR_P1_24), MP_ROM_PTR(&pin_P1_24) },
    #endif
    #if defined(PIN_P1_25) && !defined(IGNORE_PIN_P1_25)
    { MP_ROM_QSTR(MP_QSTR_P1_25), MP_ROM_PTR(&pin_P1_25) },
    #endif
    #if defined(PIN_P1_26) && !defined(IGNORE_PIN_P1_26)
    { MP_ROM_QSTR(MP_QSTR_P1_26), MP_ROM_PTR(&pin_P1_26) },
    #endif
    #if defined(PIN_P1_27) && !defined(IGNORE_PIN_P1_27)
    { MP_ROM_QSTR(MP_QSTR_P1_27), MP_ROM_PTR(&pin_P1_27) },
    #endif
    #if defined(PIN_P1_28) && !defined(IGNORE_PIN_P1_28)
    { MP_ROM_QSTR(MP_QSTR_P1_28), MP_ROM_PTR(&pin_P1_28) },
    #endif
    #if defined(PIN_P1_29) && !defined(IGNORE_PIN_P1_29)
    { MP_ROM_QSTR(MP_QSTR_P1_29), MP_ROM_PTR(&pin_P1_29) },
    #endif
    #if defined(PIN_P1_30) && !defined(IGNORE_PIN_P1_30)
    { MP_ROM_QSTR(MP_QSTR_P1_30), MP_ROM_PTR(&pin_P1_30) },
    #endif
    #if defined(PIN_P1_31) && !defined(IGNORE_PIN_P1_31)
    { MP_ROM_QSTR(MP_QSTR_P1_31), MP_ROM_PTR(&pin_P1_31) },
    #endif

    #if defined(PIN_P2_0) && !defined(IGNORE_PIN_P2_0)
    { MP_ROM_QSTR(MP_QSTR_P2_0), MP_ROM_PTR(&pin_P2_0) },
    #endif
    #if defined(PIN_P2_1) && !defined(IGNORE_PIN_P2_1)
    { MP_ROM_QSTR(MP_QSTR_P2_1), MP_ROM_PTR(&pin_P2_1) },
    #endif
    #if defined(PIN_P2_2) && !defined(IGNORE_PIN_P2_2)
    { MP_ROM_QSTR(MP_QSTR_P2_2), MP_ROM_PTR(&pin_P2_2) },
    #endif
    #if defined(PIN_P2_3) && !defined(IGNORE_PIN_P2_3)
    { MP_ROM_QSTR(MP_QSTR_P2_3), MP_ROM_PTR(&pin_P2_3) },
    #endif
    #if defined(PIN_P2_4) && !defined(IGNORE_PIN_P2_4)
    { MP_ROM_QSTR(MP_QSTR_P2_4), MP_ROM_PTR(&pin_P2_4) },
    #endif
    #if defined(PIN_P2_5) && !defined(IGNORE_PIN_P2_5)
    { MP_ROM_QSTR(MP_QSTR_P2_5), MP_ROM_PTR(&pin_P2_5) },
    #endif
    #if defined(PIN_P2_6) && !defined(IGNORE_PIN_P2_6)
    { MP_ROM_QSTR(MP_QSTR_P2_6), MP_ROM_PTR(&pin_P2_6) },
    #endif
    #if defined(PIN_P2_7) && !defined(IGNORE_PIN_P2_7)
    { MP_ROM_QSTR(MP_QSTR_P2_7), MP_ROM_PTR(&pin_P2_7) },
    #endif
    #if defined(PIN_P2_10) && !defined(IGNORE_PIN_P2_10)
    { MP_ROM_QSTR(MP_QSTR_P2_10), MP_ROM_PTR(&pin_P2_10) },
    #endif
    #if defined(PIN_P2_11) && !defined(IGNORE_PIN_P2_11)
    { MP_ROM_QSTR(MP_QSTR_P2_11), MP_ROM_PTR(&pin_P2_11) },
    #endif
    #if defined(PIN_P2_12) && !defined(IGNORE_PIN_P2_12)
    { MP_ROM_QSTR(MP_QSTR_P2_12), MP_ROM_PTR(&pin_P2_12) },
    #endif
    #if defined(PIN_P2_13) && !defined(IGNORE_PIN_P2_13)
    { MP_ROM_QSTR(MP_QSTR_P2_13), MP_ROM_PTR(&pin_P2_13) },
    #endif
    #if defined(PIN_P2_14) && !defined(IGNORE_PIN_P2_14)
    { MP_ROM_QSTR(MP_QSTR_P2_14), MP_ROM_PTR(&pin_P2_14) },
    #endif
    #if defined(PIN_P2_15) && !defined(IGNORE_PIN_P2_15)
    { MP_ROM_QSTR(MP_QSTR_P2_15), MP_ROM_PTR(&pin_P2_15) },
    #endif
    #if defined(PIN_P2_16) && !defined(IGNORE_PIN_P2_16)
    { MP_ROM_QSTR(MP_QSTR_P2_16), MP_ROM_PTR(&pin_P2_16) },
    #endif
    #if defined(PIN_P2_17) && !defined(IGNORE_PIN_P2_17)
    { MP_ROM_QSTR(MP_QSTR_P2_17), MP_ROM_PTR(&pin_P2_17) },
    #endif
    #if defined(PIN_P2_18) && !defined(IGNORE_PIN_P2_18)
    { MP_ROM_QSTR(MP_QSTR_P2_18), MP_ROM_PTR(&pin_P2_18) },
    #endif
    #if defined(PIN_P2_19) && !defined(IGNORE_PIN_P2_19)
    { MP_ROM_QSTR(MP_QSTR_P2_19), MP_ROM_PTR(&pin_P2_19) },
    #endif
    #if defined(PIN_P2_20) && !defined(IGNORE_PIN_P2_20)
    { MP_ROM_QSTR(MP_QSTR_P2_20), MP_ROM_PTR(&pin_P2_20) },
    #endif
    #if defined(PIN_P2_21) && !defined(IGNORE_PIN_P2_21)
    { MP_ROM_QSTR(MP_QSTR_P2_21), MP_ROM_PTR(&pin_P2_21) },
    #endif
    #if defined(PIN_P2_22) && !defined(IGNORE_PIN_P2_22)
    { MP_ROM_QSTR(MP_QSTR_P2_22), MP_ROM_PTR(&pin_P2_22) },
    #endif
    #if defined(PIN_P2_23) && !defined(IGNORE_PIN_P2_23)
    { MP_ROM_QSTR(MP_QSTR_P2_23), MP_ROM_PTR(&pin_P2_23) },
    #endif
    #if defined(PIN_P2_24) && !defined(IGNORE_PIN_P2_24)
    { MP_ROM_QSTR(MP_QSTR_P2_24), MP_ROM_PTR(&pin_P2_24) },
    #endif
    #if defined(PIN_P2_25) && !defined(IGNORE_PIN_P2_25)
    { MP_ROM_QSTR(MP_QSTR_P2_25), MP_ROM_PTR(&pin_P2_25) },
    #endif
    #if defined(PIN_P2_26) && !defined(IGNORE_PIN_P2_26)
    { MP_ROM_QSTR(MP_QSTR_P2_26), MP_ROM_PTR(&pin_P2_26) },
    #endif
    #if defined(PIN_P2_27) && !defined(IGNORE_PIN_P2_27)
    { MP_ROM_QSTR(MP_QSTR_P2_27), MP_ROM_PTR(&pin_P2_27) },
    #endif
    #if defined(PIN_P2_28) && !defined(IGNORE_PIN_P2_28)
    { MP_ROM_QSTR(MP_QSTR_P2_28), MP_ROM_PTR(&pin_P2_28) },
    #endif
    #if defined(PIN_P2_30) && !defined(IGNORE_PIN_P2_30)
    { MP_ROM_QSTR(MP_QSTR_P2_30), MP_ROM_PTR(&pin_P2_30) },
    #endif
    #if defined(PIN_P2_31) && !defined(IGNORE_PIN_P2_31)
    { MP_ROM_QSTR(MP_QSTR_P2_31), MP_ROM_PTR(&pin_P2_31) },
    #endif

    #if defined(PIN_P3_25) && !defined(IGNORE_PIN_P3_25)
    { MP_ROM_QSTR(MP_QSTR_P3_25), MP_ROM_PTR(&pin_P3_25) },
    #endif
    #if defined(PIN_P3_21) && !defined(IGNORE_PIN_P3_26)
    { MP_ROM_QSTR(MP_QSTR_P3_26), MP_ROM_PTR(&pin_P3_26) },
    #endif

    #if defined(PIN_P4_0) && !defined(IGNORE_PIN_P4_0)
    { MP_ROM_QSTR(MP_QSTR_P4_0), MP_ROM_PTR(&pin_P4_0) },
    #endif
    #if defined(PIN_P4_1) && !defined(IGNORE_PIN_P4_1)
    { MP_ROM_QSTR(MP_QSTR_P4_1), MP_ROM_PTR(&pin_P4_1) },
    #endif
    #if defined(PIN_P4_2) && !defined(IGNORE_PIN_P4_2)
    { MP_ROM_QSTR(MP_QSTR_P4_2), MP_ROM_PTR(&pin_P4_2) },
    #endif
    #if defined(PIN_P4_3) && !defined(IGNORE_PIN_P4_3)
    { MP_ROM_QSTR(MP_QSTR_P4_3), MP_ROM_PTR(&pin_P4_3) },
    #endif
    #if defined(PIN_P4_4) && !defined(IGNORE_PIN_P4_4)
    { MP_ROM_QSTR(MP_QSTR_P4_4), MP_ROM_PTR(&pin_P4_4) },
    #endif
    #if defined(PIN_P4_5) && !defined(IGNORE_PIN_P4_5)
    { MP_ROM_QSTR(MP_QSTR_P4_5), MP_ROM_PTR(&pin_P4_5) },
    #endif
    #if defined(PIN_P4_6) && !defined(IGNORE_PIN_P4_6)
    { MP_ROM_QSTR(MP_QSTR_P4_6), MP_ROM_PTR(&pin_P4_6) },
    #endif
    #if defined(PIN_P4_7) && !defined(IGNORE_PIN_P4_7)
    { MP_ROM_QSTR(MP_QSTR_P4_7), MP_ROM_PTR(&pin_P4_7) },
    #endif
    #if defined(PIN_P4_10) && !defined(IGNORE_PIN_P4_10)
    { MP_ROM_QSTR(MP_QSTR_P4_10), MP_ROM_PTR(&pin_P4_10) },
    #endif
    #if defined(PIN_P4_11) && !defined(IGNORE_PIN_P4_11)
    { MP_ROM_QSTR(MP_QSTR_P4_11), MP_ROM_PTR(&pin_P4_11) },
    #endif
    #if defined(PIN_P4_12) && !defined(IGNORE_PIN_P4_12)
    { MP_ROM_QSTR(MP_QSTR_P4_12), MP_ROM_PTR(&pin_P4_12) },
    #endif
    #if defined(PIN_P4_13) && !defined(IGNORE_PIN_P4_13)
    { MP_ROM_QSTR(MP_QSTR_P4_13), MP_ROM_PTR(&pin_P4_13) },
    #endif
    #if defined(PIN_P4_14) && !defined(IGNORE_PIN_P4_14)
    { MP_ROM_QSTR(MP_QSTR_P4_14), MP_ROM_PTR(&pin_P4_14) },
    #endif
    #if defined(PIN_P4_15) && !defined(IGNORE_PIN_P4_15)
    { MP_ROM_QSTR(MP_QSTR_P4_15), MP_ROM_PTR(&pin_P4_15) },
    #endif
    #if defined(PIN_P4_16) && !defined(IGNORE_PIN_P4_16)
    { MP_ROM_QSTR(MP_QSTR_P4_16), MP_ROM_PTR(&pin_P4_16) },
    #endif
    #if defined(PIN_P4_17) && !defined(IGNORE_PIN_P4_17)
    { MP_ROM_QSTR(MP_QSTR_P4_17), MP_ROM_PTR(&pin_P4_17) },
    #endif
    #if defined(PIN_P4_18) && !defined(IGNORE_PIN_P4_18)
    { MP_ROM_QSTR(MP_QSTR_P4_18), MP_ROM_PTR(&pin_P4_18) },
    #endif
    #if defined(PIN_P4_19) && !defined(IGNORE_PIN_P4_19)
    { MP_ROM_QSTR(MP_QSTR_P4_19), MP_ROM_PTR(&pin_P4_19) },
    #endif
    #if defined(PIN_P4_20) && !defined(IGNORE_PIN_P4_20)
    { MP_ROM_QSTR(MP_QSTR_P4_20), MP_ROM_PTR(&pin_P4_20) },
    #endif
    #if defined(PIN_P4_21) && !defined(IGNORE_PIN_P4_21)
    { MP_ROM_QSTR(MP_QSTR_P4_21), MP_ROM_PTR(&pin_P4_21) },
    #endif
    #if defined(PIN_P4_22) && !defined(IGNORE_PIN_P4_22)
    { MP_ROM_QSTR(MP_QSTR_P4_22), MP_ROM_PTR(&pin_P4_22) },
    #endif
    #if defined(PIN_P4_23) && !defined(IGNORE_PIN_P4_23)
    { MP_ROM_QSTR(MP_QSTR_P4_23), MP_ROM_PTR(&pin_P4_23) },
    #endif
    #if defined(PIN_P4_24) && !defined(IGNORE_PIN_P4_24)
    { MP_ROM_QSTR(MP_QSTR_P4_24), MP_ROM_PTR(&pin_P4_24) },
    #endif
    #if defined(PIN_P4_25) && !defined(IGNORE_PIN_P4_25)
    { MP_ROM_QSTR(MP_QSTR_P4_25), MP_ROM_PTR(&pin_P4_25) },
    #endif
    #if defined(PIN_P4_26) && !defined(IGNORE_PIN_P4_26)
    { MP_ROM_QSTR(MP_QSTR_P4_26), MP_ROM_PTR(&pin_P4_26) },
    #endif
    #if defined(PIN_P4_27) && !defined(IGNORE_PIN_P4_27)
    { MP_ROM_QSTR(MP_QSTR_P4_27), MP_ROM_PTR(&pin_P4_27) },
    #endif
    #if defined(PIN_P4_28) && !defined(IGNORE_PIN_P4_28)
    { MP_ROM_QSTR(MP_QSTR_P4_28), MP_ROM_PTR(&pin_P4_28) },
    #endif
    #if defined(PIN_P4_30) && !defined(IGNORE_PIN_P4_30)
    { MP_ROM_QSTR(MP_QSTR_P4_30), MP_ROM_PTR(&pin_P4_30) },
    #endif
    #if defined(PIN_P4_31) && !defined(IGNORE_PIN_P4_31)
    { MP_ROM_QSTR(MP_QSTR_P4_31), MP_ROM_PTR(&pin_P4_31) },
    #endif

};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);
