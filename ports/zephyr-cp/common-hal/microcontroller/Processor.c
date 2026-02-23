// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/Processor.h"

#include "shared-bindings/microcontroller/ResetReason.h"

#include <sys/types.h>
#include <zephyr/drivers/hwinfo.h>


float common_hal_mcu_processor_get_temperature(void) {
    return 0.0;
}

extern uint32_t SystemCoreClock;
uint32_t common_hal_mcu_processor_get_frequency(void) {
    #ifdef __ARM__
    return SystemCoreClock;
    #else
    return CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC;
    #endif
}

float common_hal_mcu_processor_get_voltage(void) {
    return 3.3f;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    ssize_t len = hwinfo_get_device_id(raw_id, COMMON_HAL_MCU_PROCESSOR_UID_LENGTH);
    if (len < 0) {
        printk("UID retrieval failed: %d\n", len);
        len = 0;
    }
    if (len < COMMON_HAL_MCU_PROCESSOR_UID_LENGTH) {
        printk("UID shorter %d than defined length %d\n", len, COMMON_HAL_MCU_PROCESSOR_UID_LENGTH);
        memset(raw_id + len, 0, COMMON_HAL_MCU_PROCESSOR_UID_LENGTH - len);
    }
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    mcu_reset_reason_t r = RESET_REASON_UNKNOWN;
    return r;
}
