// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "common-hal/alarm/SleepMemory.h"
#include "shared-bindings/alarm/SleepMemory.h"

#include "esp_sleep.h"
#include "esp_system.h"

// Data storage for singleton instance of SleepMemory.
// Might be RTC_SLOW_MEM or RTC_FAST_MEM, depending on setting of CONFIG_ESP32S2_RTCDATA_IN_FAST_MEM.
static RTC_NOINIT_ATTR uint8_t _sleep_mem[SLEEP_MEMORY_LENGTH];

void alarm_sleep_memory_reset(void) {
    // With RTC_NOINIT_ATTR, the bootloader does not initialize sleep memory.
    // Preserve contents on resets where the RTC domain stays powered (software
    // reset, watchdog, panic, deep sleep wake). Clear on everything else —
    // after power-on, SRAM contents are undefined; after brown-out, the RTC
    // domain was reset by hardware (System Reset scope per TRM Figure 6.1-1).
    esp_reset_reason_t reason = esp_reset_reason();
    switch (reason) {
        case ESP_RST_SW:        // microcontroller.reset() / esp_restart()
        case ESP_RST_DEEPSLEEP: // deep sleep wake
        case ESP_RST_PANIC:     // unhandled exception
        case ESP_RST_INT_WDT:   // interrupt watchdog
        case ESP_RST_TASK_WDT:  // task watchdog
        case ESP_RST_WDT:       // other watchdog
            // RTC domain was not reset — sleep memory is intact.
            break;
        default:
            // Power-on, brown-out, unknown, or any other reason where
            // RTC SRAM contents may be undefined. Clear to zero.
            memset(_sleep_mem, 0, sizeof(_sleep_mem));
            break;
    }
}

uint32_t common_hal_alarm_sleep_memory_get_length(alarm_sleep_memory_obj_t *self) {
    return sizeof(_sleep_mem);
}

bool common_hal_alarm_sleep_memory_set_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, const uint8_t *values, uint32_t len) {

    if (start_index + len > sizeof(_sleep_mem)) {
        return false;
    }

    memcpy((uint8_t *)(_sleep_mem + start_index), values, len);
    return true;
}

void common_hal_alarm_sleep_memory_get_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, uint8_t *values, uint32_t len) {

    if (start_index + len > sizeof(_sleep_mem)) {
        return;
    }
    memcpy(values, (uint8_t *)(_sleep_mem + start_index), len);
}
