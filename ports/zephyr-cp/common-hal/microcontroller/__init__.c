// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/microcontroller/Processor.h"

// #include "shared-bindings/nvm/ByteArray.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"

#include "supervisor/filesystem.h"
#include "supervisor/port.h"
#include "supervisor/shared/safe_mode.h"

#include <zephyr/kernel.h>

// This routine should work even when interrupts are disabled. Used by OneWire
// for precise timing.
void common_hal_mcu_delay_us(uint32_t delay) {
}

static uint32_t _irq_key;

static volatile uint32_t nesting_count = 0;
void common_hal_mcu_disable_interrupts() {
    if (nesting_count == 0) {
        // Unlike __disable_irq(), this should only be called the first time
        // "is_nested_critical_region" is sd's equivalent of our nesting count
        // so a nested call would store 0 in the global and make the later
        // exit call not actually re-enable interrupts
        //
        // This only disables interrupts of priority 2 through 7; levels 0, 1,
        // and 4, are exclusive to softdevice and should never be used, so
        // this limitation is not important.
        // sd_nvic_critical_region_enter(&is_nested_critical_region);
        if (!k_is_in_isr()) {
            k_sched_lock();
        }
        _irq_key = irq_lock();
    }
    nesting_count++;
}

void common_hal_mcu_enable_interrupts() {
    if (nesting_count == 0) {
        // This is very very bad because it means there was mismatched disable/enables.
        reset_into_safe_mode(SAFE_MODE_INTERRUPT_ERROR);
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    irq_unlock(_irq_key);
    if (!k_is_in_isr()) {
        k_sched_unlock();
    }
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    enum { DFU_MAGIC_UF2_RESET = 0x57 };
    uint8_t new_value = 0;
    if (runmode == RUNMODE_BOOTLOADER || runmode == RUNMODE_UF2) {
        new_value = DFU_MAGIC_UF2_RESET;
    }
    // int err_code = sd_power_gpregret_set(0, DFU_MAGIC_UF2_RESET);
    // if (err_code != NRF_SUCCESS) {
    //     // Set it without the soft device if the SD failed. (It may be off.)
    //     nrf_power_gpregret_set(NRF_POWER, new_value);
    // }
    if (runmode == RUNMODE_SAFE_MODE) {
        safe_mode_on_next_reset(SAFE_MODE_PROGRAMMATIC);
    }
}

void common_hal_mcu_reset(void) {
    filesystem_flush();
    reset_cpu();
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

#if CIRCUITPY_NVM && CIRCUITPY_INTERNAL_NVM_SIZE > 0
// The singleton nvm.ByteArray object.
const nvm_bytearray_obj_t common_hal_mcu_nvm_obj = {
    .base = {
        .type = &nvm_bytearray_type,
    },
    .start_address = (uint8_t *)CIRCUITPY_INTERNAL_NVM_START_ADDR,
    .len = CIRCUITPY_INTERNAL_NVM_SIZE,
};
#endif

#if CIRCUITPY_WATCHDOG
// The singleton watchdog.WatchDogTimer object.
watchdog_watchdogtimer_obj_t common_hal_mcu_watchdogtimer_obj = {
    .base = {
        .type = &watchdog_watchdogtimer_type,
    },
    .timeout = 0.0f,
    .mode = WATCHDOGMODE_NONE,
};
#endif
