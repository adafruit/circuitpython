// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include <string.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"
#include "shared-bindings/time/__init__.h"

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/watchdog.h"

#ifdef CIRCUITPY_PSRAM_CHIP_SELECT
#include "hardware/regs/qmi.h"
#include "hardware/structs/qmi.h"
#endif

#if PICO_RP2040
#include "hardware/regs/vreg_and_chip_reset.h"
#include "hardware/structs/vreg_and_chip_reset.h"
#endif
#if PICO_RP2350
#include "hardware/regs/powman.h"
#include "hardware/structs/powman.h"
#endif
#include "hardware/regs/watchdog.h"
#include "hardware/structs/watchdog.h"

float common_hal_mcu_processor_get_temperature(void) {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL_NUM);
    uint16_t value = adc_read();
    adc_set_temp_sensor_enabled(false);
    float voltage = value * 3.3 / (1 << 12);
    // TODO: turn the ADC back off
    return 27 - (voltage - 0.706) / 0.001721;
}

float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return clock_get_hz(clk_sys);
}

#ifdef CIRCUITPY_PSRAM_CHIP_SELECT
void __no_inline_not_in_flash_func(mcu_processor_update_psram_timing)(uint32_t sys_clk_khz) {
    // MAX_SELECT is in units of 64 system clock cycles; PSRAM allows 8 us max CS
    // assertion. Use 7.5 us so there is margin at any clk_sys.
    uint32_t max_select = (75 * sys_clk_khz) / 640000;
    // MIN_DESELECT is in system clock cycles; PSRAM needs 50 ns min CS
    // deassertion. Round up so we are never under.
    uint32_t min_deselect = (sys_clk_khz + 19999) / 20000;

    qmi_hw->m[1].timing =
        QMI_M0_TIMING_PAGEBREAK_VALUE_1024 << QMI_M0_TIMING_PAGEBREAK_LSB | // Break between pages.
            3 << QMI_M0_TIMING_SELECT_HOLD_LSB | // Delay releasing CS for 3 extra system cycles.
            1 << QMI_M0_TIMING_COOLDOWN_LSB |
            1 << QMI_M0_TIMING_RXDELAY_LSB |
            max_select << QMI_M0_TIMING_MAX_SELECT_LSB |
            min_deselect << QMI_M0_TIMING_MIN_DESELECT_LSB |
            2 << QMI_M0_TIMING_CLKDIV_LSB;
}
#endif

void common_hal_mcu_processor_set_frequency(mcu_processor_obj_t *self, uint32_t frequency) {
    uint vco, postdiv1, postdiv2;
    uint32_t freq_khz = frequency / 1000;
    if (!check_sys_clock_khz(freq_khz, &vco, &postdiv1, &postdiv2)) {
        mp_arg_error_invalid(MP_QSTR_frequency);
    }
    // These voltages are approximate based on the PicoDVI examples.
    enum vreg_voltage voltage = VREG_VOLTAGE_1_10;
    if (freq_khz >= 400000) {
        voltage = VREG_VOLTAGE_1_30;
    } else if (freq_khz >= 300000) {
        voltage = VREG_VOLTAGE_1_20;
    } else if (freq_khz > 133000) {
        voltage = VREG_VOLTAGE_1_20;
    }
    vreg_set_voltage(voltage);
    // Wait for a stable voltage
    common_hal_time_delay_ms(10);

    #ifdef CIRCUITPY_PSRAM_CHIP_SELECT
    // Prevent interrupt handlers from accessing PSRAM until its timing matches
    // the new system clock.
    common_hal_mcu_disable_interrupts();
    #endif
    set_sys_clock_khz(freq_khz, false);
    #ifdef CIRCUITPY_PSRAM_CHIP_SELECT
    mcu_processor_update_psram_timing(freq_khz);
    common_hal_mcu_enable_interrupts();
    #endif
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    pico_unique_board_id_t retrieved_id;
    pico_get_unique_board_id(&retrieved_id);
    memcpy(raw_id, retrieved_id.id, COMMON_HAL_MCU_PROCESSOR_UID_LENGTH);
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    mcu_reset_reason_t reason = MCU_RESET_REASON_UNKNOWN;

    #if PICO_RP2040
    uint32_t chip_reset_reg = vreg_and_chip_reset_hw->chip_reset;

    if (chip_reset_reg & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_PSM_RESTART_BITS) {
        reason = MCU_RESET_REASON_RESCUE_DEBUG;
    }

    if (chip_reset_reg & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_RUN_BITS) {
        reason = MCU_RESET_REASON_RESET_PIN;
    }

    if (chip_reset_reg & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_POR_BITS) {
        // NOTE: This register is also used for brownout, but there is no way to differentiate between power on and brown out
        reason = MCU_RESET_REASON_POWER_ON;
    }
    #endif
    #if PICO_RP2350
    uint32_t chip_reset_reg = powman_hw->chip_reset;

    if (chip_reset_reg & POWMAN_CHIP_RESET_HAD_RESCUE_BITS) {
        reason = MCU_RESET_REASON_RESCUE_DEBUG;
    }

    if (chip_reset_reg & POWMAN_CHIP_RESET_HAD_RUN_LOW_BITS) {
        reason = MCU_RESET_REASON_RESET_PIN;
    }

    if (chip_reset_reg & POWMAN_CHIP_RESET_HAD_BOR_BITS) {
        reason = MCU_RESET_REASON_BROWNOUT;
    }

    if (chip_reset_reg & POWMAN_CHIP_RESET_HAD_POR_BITS) {
        reason = MCU_RESET_REASON_POWER_ON;
    }
    #endif

    // Check watchdog after chip reset since watchdog doesn't clear chip_reset, while chip_reset clears the watchdog

    // The watchdog is used for software reboots such as resetting after copying a UF2 via the bootloader.
    if (watchdog_caused_reboot()) {
        reason = MCU_RESET_REASON_SOFTWARE;
    }

    // Actual watchdog usage will set a special value that this function detects.
    if (watchdog_enable_caused_reboot()) {
        reason = MCU_RESET_REASON_WATCHDOG;
    }

    return reason;
}
