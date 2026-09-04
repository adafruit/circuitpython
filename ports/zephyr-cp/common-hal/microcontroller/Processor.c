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

#if defined(CONFIG_WIFI)
#include <zephyr/net/net_if.h>

// Fallback for SoCs with no hwinfo driver, where hwinfo_get_device_id() returns
// -ENOSYS and cpu.uid would otherwise be all zeros. A WiFi link address is
// per-device, so it is a better answer than nothing, and at least one vendor
// (Silicon Labs, for the siwx91x) defines that part's unique ID as exactly this:
// the WiFi MAC zero-extended to eight bytes.
//
// This is not a burned-in serial. A MAC can generally be reprogrammed by vendor
// tooling, so cpu.uid must not be relied on as tamper proof where it comes from
// here. Boards whose SoC has a real hwinfo driver never reach this path.
static ssize_t uid_from_wifi_mac(uint8_t raw_id[]) {
    if (COMMON_HAL_MCU_PROCESSOR_UID_LENGTH < 8) {
        return -1;
    }
    struct net_if *iface = net_if_get_first_wifi();
    struct net_linkaddr *addr = (iface != NULL) ? net_if_get_link_addr(iface) : NULL;
    if (addr == NULL || addr->len != 6) {
        return -1;
    }
    raw_id[0] = 0;
    raw_id[1] = 0;
    memcpy(&raw_id[2], addr->addr, 6);
    return 8;
}
#endif


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
    #if defined(CONFIG_WIFI)
    if (len < 0) {
        ssize_t mac_len = uid_from_wifi_mac(raw_id);
        if (mac_len > 0) {
            len = mac_len;
        }
    }
    #endif
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
    #if defined(CONFIG_HWINFO)
    uint32_t cause = 0;
    if (hwinfo_get_reset_cause(&cause) == 0) {
        if (cause & RESET_POR) {
            return MCU_RESET_REASON_POWER_ON;
        } else if (cause & RESET_SOFTWARE) {
            return MCU_RESET_REASON_SOFTWARE;
        } else if (cause & RESET_WATCHDOG) {
            return MCU_RESET_REASON_WATCHDOG;
        } else if (cause & RESET_BROWNOUT) {
            return MCU_RESET_REASON_BROWNOUT;
        } else if (cause & RESET_PIN) {
            return MCU_RESET_REASON_RESET_PIN;
        }
    }
    #endif
    return MCU_RESET_REASON_UNKNOWN;
}
