// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include "py/runtime.h"

#if CIRCUITPY_ALARM
#include "common-hal/alarm/__init__.h"
#endif

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

#include "max32_port.h"

// No means of getting core temperature for currently supported devices
float common_hal_mcu_processor_get_temperature(void) {
    return NAN;
}

// MAX32690 can measure VCORE
// TODO: (low prior.) Implement ADC API under "peripherals" and use API to measure VCORE
float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return SystemCoreClock;
}

// NOTE: COMMON_HAL_MCU_PROCESSOR_UID_LENGTH is defined in mpconfigboard.h
// Use this per device to make sure raw_id is an appropriate minimum number of bytes
void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    MXC_SYS_GetUSN(raw_id, NULL); // NULL checksum will not be verified by AES
    return;
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    #if CIRCUITPY_ALARM
    // TODO: (low prior.) add reset reason in alarm / deepsleep cases (should require alarm peripheral API in "peripherals")
    #endif
    return RESET_REASON_UNKNOWN;
}
