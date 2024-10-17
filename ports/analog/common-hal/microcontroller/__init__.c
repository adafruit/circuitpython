// This file is part of the CircuitPython project: https://circuitpython.org
//
// SP3_X-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
// SP3_X-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
// SP3_X-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc
//
// SP3_X-License-Identifier: MIT

#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/microcontroller/Processor.h"

// #include "shared-bindings/nvm/ByteArray.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "supervisor/port.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"


#include "max32690.h"
#include "mxc_delay.h"

/** NOTE: It is not advised to directly include the below!
 * These are includes taken care of by the core cmsis file.
 * e.g. "max32690.h". Since CMSIS is compiled as lib, these are
 * included there as <core_cm4.h> for example.
*/
// #include <core_cmFunc.h>    // For enable/disable interrupts
// #include <core_cm4.h>       // For NVIC_SystemReset
// #include <core_cmInstr.h>   // For __DMB Data Memory Barrier (flush DBUS activity)

void common_hal_mcu_delay_us(uint32_t delay) {

    MXC_Delay(MXC_DELAY_USEC(delay));
}

volatile uint32_t nesting_count = 0;

void common_hal_mcu_disable_interrupts(void) {
    __disable_irq();
    __DMB();
    nesting_count++;
}

void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        // This is very very bad because it means there was mismatched disable/enables.
        reset_into_safe_mode(SAFE_MODE_INTERRUPT_ERROR);
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    __DMB(); // flush internal DBUS before proceeding
    __enable_irq();
}

static bool next_reset_to_bootloader = false;

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    if (runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(SAFE_MODE_PROGRAMMATIC);
    }
    if (runmode == RUNMODE_BOOTLOADER) {
        next_reset_to_bootloader = true;
    }
}

void common_hal_mcu_reset(void) {
    if (next_reset_to_bootloader) {
        reset_to_bootloader();
    } else {
        NVIC_SystemReset();
    }
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

// This maps MCU pin names to pin objects.
static const mp_rom_map_elem_t mcu_pin_global_dict_table[] = {
    #if defined(PIN_P0_01) && !defined(IGNORE_PIN_P0_01)
    { MP_ROM_QSTR(MP_QSTR_P0_01), MP_ROM_PTR(&pin_P0_01) },
    #endif
    #if defined(PIN_P0_02) && !defined(IGNORE_PIN_P0_02)
    { MP_ROM_QSTR(MP_QSTR_P0_02), MP_ROM_PTR(&pin_P0_02) },
    #endif
    #if defined(PIN_P0_03) && !defined(IGNORE_PIN_P0_03)
    { MP_ROM_QSTR(MP_QSTR_P0_03), MP_ROM_PTR(&pin_P0_03) },
    #endif
    #if defined(PIN_P0_04) && !defined(IGNORE_PIN_P0_04)
    { MP_ROM_QSTR(MP_QSTR_P0_04), MP_ROM_PTR(&pin_P0_04) },
    #endif
    #if defined(PIN_P0_05) && !defined(IGNORE_PIN_P0_05)
    { MP_ROM_QSTR(MP_QSTR_P0_05), MP_ROM_PTR(&pin_P0_05) },
    #endif
    #if defined(PIN_P0_06) && !defined(IGNORE_PIN_P0_06)
    { MP_ROM_QSTR(MP_QSTR_P0_06), MP_ROM_PTR(&pin_P0_06) },
    #endif
    #if defined(PIN_P0_07) && !defined(IGNORE_PIN_P0_07)
    { MP_ROM_QSTR(MP_QSTR_P0_07), MP_ROM_PTR(&pin_P0_07) },
    #endif
    #if defined(PIN_P0_08) && !defined(IGNORE_PIN_P0_08)
    { MP_ROM_QSTR(MP_QSTR_P0_08), MP_ROM_PTR(&pin_P0_08) },
    #endif
    #if defined(PIN_P0_09) && !defined(IGNORE_PIN_P0_09)
    { MP_ROM_QSTR(MP_QSTR_P0_09), MP_ROM_PTR(&pin_P0_09) },
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

    #if defined(PIN_P1_01) && !defined(IGNORE_PIN_P1_01)
    { MP_ROM_QSTR(MP_QSTR_P1_01), MP_ROM_PTR(&pin_P1_01) },
    #endif
    #if defined(PIN_P1_02) && !defined(IGNORE_PIN_P1_02)
    { MP_ROM_QSTR(MP_QSTR_P1_02), MP_ROM_PTR(&pin_P1_02) },
    #endif
    #if defined(PIN_P1_03) && !defined(IGNORE_PIN_P1_03)
    { MP_ROM_QSTR(MP_QSTR_P1_03), MP_ROM_PTR(&pin_P1_03) },
    #endif
    #if defined(PIN_P1_04) && !defined(IGNORE_PIN_P1_04)
    { MP_ROM_QSTR(MP_QSTR_P1_04), MP_ROM_PTR(&pin_P1_04) },
    #endif
    #if defined(PIN_P1_05) && !defined(IGNORE_PIN_P1_05)
    { MP_ROM_QSTR(MP_QSTR_P1_05), MP_ROM_PTR(&pin_P1_05) },
    #endif
    #if defined(PIN_P1_06) && !defined(IGNORE_PIN_P1_06)
    { MP_ROM_QSTR(MP_QSTR_P1_06), MP_ROM_PTR(&pin_P1_06) },
    #endif
    #if defined(PIN_P1_07) && !defined(IGNORE_PIN_P1_07)
    { MP_ROM_QSTR(MP_QSTR_P1_07), MP_ROM_PTR(&pin_P1_07) },
    #endif
    #if defined(PIN_P1_08) && !defined(IGNORE_PIN_P1_08)
    { MP_ROM_QSTR(MP_QSTR_P1_08), MP_ROM_PTR(&pin_P1_08) },
    #endif
    #if defined(PIN_P1_09) && !defined(IGNORE_PIN_P1_09)
    { MP_ROM_QSTR(MP_QSTR_P1_09), MP_ROM_PTR(&pin_P1_09) },
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

    #if defined(PIN_P2_01) && !defined(IGNORE_PIN_P2_01)
    { MP_ROM_QSTR(MP_QSTR_P2_01), MP_ROM_PTR(&pin_P2_01) },
    #endif
    #if defined(PIN_P2_02) && !defined(IGNORE_PIN_P2_02)
    { MP_ROM_QSTR(MP_QSTR_P2_02), MP_ROM_PTR(&pin_P2_02) },
    #endif
    #if defined(PIN_P2_03) && !defined(IGNORE_PIN_P2_03)
    { MP_ROM_QSTR(MP_QSTR_P2_03), MP_ROM_PTR(&pin_P2_03) },
    #endif
    #if defined(PIN_P2_04) && !defined(IGNORE_PIN_P2_04)
    { MP_ROM_QSTR(MP_QSTR_P2_04), MP_ROM_PTR(&pin_P2_04) },
    #endif
    #if defined(PIN_P2_05) && !defined(IGNORE_PIN_P2_05)
    { MP_ROM_QSTR(MP_QSTR_P2_05), MP_ROM_PTR(&pin_P2_05) },
    #endif
    #if defined(PIN_P2_06) && !defined(IGNORE_PIN_P2_06)
    { MP_ROM_QSTR(MP_QSTR_P2_06), MP_ROM_PTR(&pin_P2_06) },
    #endif
    #if defined(PIN_P2_07) && !defined(IGNORE_PIN_P2_07)
    { MP_ROM_QSTR(MP_QSTR_P2_07), MP_ROM_PTR(&pin_P2_07) },
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

    #if defined(PIN_P3_01) && !defined(IGNORE_PIN_P3_01)
    { MP_ROM_QSTR(MP_QSTR_P3_01), MP_ROM_PTR(&pin_P3_01) },
    #endif
    #if defined(PIN_P3_02) && !defined(IGNORE_PIN_P3_02)
    { MP_ROM_QSTR(MP_QSTR_P3_02), MP_ROM_PTR(&pin_P3_02) },
    #endif
    #if defined(PIN_P3_03) && !defined(IGNORE_PIN_P3_03)
    { MP_ROM_QSTR(MP_QSTR_P3_03), MP_ROM_PTR(&pin_P3_03) },
    #endif
    #if defined(PIN_P3_04) && !defined(IGNORE_PIN_P3_04)
    { MP_ROM_QSTR(MP_QSTR_P3_04), MP_ROM_PTR(&pin_P3_04) },
    #endif
    #if defined(PIN_P3_05) && !defined(IGNORE_PIN_P3_05)
    { MP_ROM_QSTR(MP_QSTR_P3_05), MP_ROM_PTR(&pin_P3_05) },
    #endif
    #if defined(PIN_P3_06) && !defined(IGNORE_PIN_P3_06)
    { MP_ROM_QSTR(MP_QSTR_P3_06), MP_ROM_PTR(&pin_P3_06) },
    #endif
    #if defined(PIN_P3_07) && !defined(IGNORE_PIN_P3_07)
    { MP_ROM_QSTR(MP_QSTR_P3_07), MP_ROM_PTR(&pin_P3_07) },
    #endif
    #if defined(PIN_P3_08) && !defined(IGNORE_PIN_P3_08)
    { MP_ROM_QSTR(MP_QSTR_P3_08), MP_ROM_PTR(&pin_P3_08) },
    #endif
    #if defined(PIN_P3_09) && !defined(IGNORE_PIN_P3_09)
    { MP_ROM_QSTR(MP_QSTR_P3_09), MP_ROM_PTR(&pin_P3_09) },
    #endif

    #if defined(PIN_P4_01) && !defined(IGNORE_PIN_P4_01)
    { MP_ROM_QSTR(MP_QSTR_P4_01), MP_ROM_PTR(&pin_P4_02) },
    #endif
    #if defined(PIN_P4_02) && !defined(IGNORE_PIN_P4_02)
    { MP_ROM_QSTR(MP_QSTR_P4_01), MP_ROM_PTR(&pin_P4_02) },
    #endif

};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);


/** NOTE: Not implemented yet */
// #if CIRCUITPY_INTERNAL_NVM_SIZE > 0
// // The singleton nvm.ByteArray object.
// const nvm_bytearray_obj_t common_hal_mcu_nvm_obj = {
//     .base = {
//         .type = &nvm_bytearray_type,
//     },
//     .len = NVM_BYTEARRAY_BUFFER_SIZE,
//     .start_address = (uint8_t *)(CIRCUITPY_INTERNAL_NVM_START_ADDR)
// };
// #endif
