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
#if CIRCUITPY_ALARM
#include "common-hal/alarm/__init__.h"
#endif

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/watchdog.h"

#include "genhdr/flash_info.h"

#if PICO_RP2350
#include "hardware/regs/io_qspi.h"
#include "hardware/regs/qmi.h"
#include "hardware/structs/io_qspi.h"
#include "hardware/structs/qmi.h"
#endif

#if PICO_RP2040
#include "hardware/regs/ssi.h"
#include "hardware/regs/vreg_and_chip_reset.h"
#include "hardware/structs/ssi.h"
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
    // PSRAM SCK is clk_sys / CLKDIV and must stay at or below 133 MHz.
    // Divisor 1 is not used above 100 MHz because the RXDELAY it needs is
    // too late. RXDELAY samples later as the divisor grows, plus one more
    // cycle once SCK is above 100 MHz. Same rule as arduino-pico.
    uint32_t max_sck_khz = 133000;
    #ifdef CIRCUITPY_PSRAM_MAX_SCK_HZ
    // Keep a board's lower PSRAM clock limit when the CPU frequency changes.
    max_sck_khz = MIN(max_sck_khz, CIRCUITPY_PSRAM_MAX_SCK_HZ / 1000);
    #endif
    uint32_t clkdiv = (sys_clk_khz + max_sck_khz - 1) / max_sck_khz;
    if (clkdiv == 1 && sys_clk_khz > 100000) {
        clkdiv = 2;
    }
    uint32_t rxdelay = clkdiv;
    if (sys_clk_khz / clkdiv > 100000) {
        rxdelay++;
    }
    #ifdef CIRCUITPY_PSRAM_MAX_SCK_HZ
    // Slower PSRAM clocks can require a delay beyond this three-bit field.
    rxdelay = MIN(rxdelay, QMI_M0_TIMING_RXDELAY_BITS >> QMI_M0_TIMING_RXDELAY_LSB);
    #endif

    qmi_hw->m[1].timing =
        QMI_M0_TIMING_PAGEBREAK_VALUE_1024 << QMI_M0_TIMING_PAGEBREAK_LSB | // Break between pages.
            3 << QMI_M0_TIMING_SELECT_HOLD_LSB | // Delay releasing CS for 3 extra system cycles.
            1 << QMI_M0_TIMING_COOLDOWN_LSB |
            rxdelay << QMI_M0_TIMING_RXDELAY_LSB |
            max_select << QMI_M0_TIMING_MAX_SELECT_LSB |
            min_deselect << QMI_M0_TIMING_MIN_DESELECT_LSB |
            clkdiv << QMI_M0_TIMING_CLKDIV_LSB;
}
#endif

#if PICO_RP2350
// Saved flash timing for the current system clock: the value of the timing
// register for QMI window 0. QMI is the Quad Memory Interface, which reads the
// flash. A flash erase, program or command leaves stage 2's slow timing in
// place, and mcu_processor_restore_flash_timing() writes this value back.
static uint32_t _flash_timing;

void __no_inline_not_in_flash_func(mcu_processor_restore_flash_timing)(void) {
    // QMI has no busy flag. Instead, wait until the flash chip select pad (SS,
    // active low) is deasserted, so the timing doesn't change during a read.
    while ((io_qspi_hw->io[1].status & IO_QSPI_GPIO_QSPI_SS_STATUS_OUTTOPAD_BITS) == 0) {
    }
    qmi_hw->m[0].timing = _flash_timing;
    // The new timing applies from the next read. Read once through XIP
    // (execute in place) so that it is in use before returning.
    (void)*(volatile uint32_t *)XIP_NOCACHE_NOALLOC_BASE;
}

// Maximum delays inside the RP2350 at VDDIO = 3.3 V, from RP2350 datasheet
// Table 1291 (QMI timing delay).
#define QMI_CLOCK_TO_OUTPUT_PS (2500)
#define QMI_INPUT_TO_SAMPLE_PS (1500)
// Allowance for the board trace between the RP2350 and the flash, each way.
#define FLASH_TRACE_DELAY_PS (500)
// Margin required beyond the flash's output hold time.
#define FLASH_HOLD_MARGIN_PS (500)

void __no_inline_not_in_flash_func(mcu_processor_update_flash_timing)(uint32_t sys_clk_khz) {
    // Choose the flash clock divider (CLKDIV) and sample delay (RXDELAY) from
    // the flash's datasheet timing in flash_info.h, assuming worst-case delays.
    //
    // The flash drives each data bit after a falling clock edge. QMI samples
    // the bit (CLKDIV + RXDELAY) half system clocks after that edge.
    //   Setup: the sample must come after the bit is valid. That is the RP2350
    //          output delay, plus the flash's output valid time, plus the
    //          RP2350 input delay, plus the board trace delay both ways.
    //   Hold:  the sample must come before the flash starts driving the next
    //          bit. That is one SCK period plus the flash's output hold time,
    //          less FLASH_HOLD_MARGIN_PS. The RP2350's minimum delays aren't
    //          specified, so they count as 0.
    //
    // Use the smallest divider that keeps SCK within the flash's 0xeb rating
    // and has a sample delay meeting both checks. Of those sample delays, pick
    // the one furthest from failing either check.
    const int32_t period_ps = 1000000000 / sys_clk_khz;
    const int32_t setup_ps = QMI_CLOCK_TO_OUTPUT_PS + FLASH_MAX_OUTPUT_VALID_PS +
        QMI_INPUT_TO_SAMPLE_PS + 2 * FLASH_TRACE_DELAY_PS;
    uint32_t max_sck_khz = FLASH_EB_MAX_SCK_KHZ;
    #ifdef CIRCUITPY_FLASH_MAX_SCK_HZ
    max_sck_khz = MIN(max_sck_khz, CIRCUITPY_FLASH_MAX_SCK_HZ / 1000);
    #endif
    const uint32_t max_rxdelay = QMI_M0_TIMING_RXDELAY_BITS >> QMI_M0_TIMING_RXDELAY_LSB;

    uint32_t clkdiv = (sys_clk_khz + max_sck_khz - 1) / max_sck_khz;
    uint32_t rxdelay = max_rxdelay;
    for (; clkdiv <= 255; clkdiv++) {
        const int32_t hold_ps = (int32_t)clkdiv * period_ps + FLASH_MIN_OUTPUT_HOLD_PS - FLASH_HOLD_MARGIN_PS;
        int32_t best_margin_ps = -1;
        for (uint32_t r = 0; r <= max_rxdelay; r++) {
            const int32_t sample_ps = (int32_t)(clkdiv + r) * period_ps / 2;
            const int32_t margin_ps = MIN(sample_ps - setup_ps, hold_ps - sample_ps);
            if (margin_ps > best_margin_ps) {
                best_margin_ps = margin_ps;
                rxdelay = r;
            }
        }
        if (best_margin_ps >= 0) {
            break;
        }
    }
    if (clkdiv > 255) {
        // No divider up to 255 passes both checks. That can't happen at a real
        // clock speed, but fall back to the slowest timing.
        clkdiv = 255;
        rxdelay = max_rxdelay;
    }

    // COOLDOWN = 1 keeps chip select asserted briefly after a read, so the next
    // read from the following address doesn't resend the address. Stage 2
    // sets the same value.
    _flash_timing =
        1 << QMI_M0_TIMING_COOLDOWN_LSB |
            rxdelay << QMI_M0_TIMING_RXDELAY_LSB |
            clkdiv << QMI_M0_TIMING_CLKDIV_LSB;
    mcu_processor_restore_flash_timing();
}
#endif

#if PICO_RP2040
// Highest RP2040 flash clock. The RP2040 datasheet gives no QSPI pad delays,
// so this can't be worked out like the RP2350 timing. 62.5 MHz (divider 2 at
// 125 MHz) is the pico-sdk default for the Pico.
//
// On a Feather RP2040, a sample delay of 1 read correctly at every divider and
// every system clock from 48 to 250 MHz. At a 75 MHz flash clock (divider 2 at
// 150 MHz), no other sample delay worked, so this limit keeps some margin.
#define RP2040_FLASH_MAX_SCK_KHZ (62500)

// SSI clock divider for the current system clock. The SSI (synchronous serial
// interface) reads the flash. A flash erase, program or command reruns stage
// 2, which sets divider 4, and mcu_processor_restore_flash_timing() writes
// this value back. Until the first update, it is stage 2's value.
static uint32_t _flash_clock_divider = 4;

void __no_inline_not_in_flash_func(mcu_processor_restore_flash_timing)(void) {
    // The divider can change only while the SSI is disabled, which also stops
    // XIP (execute in place). So this runs from RAM.
    while (ssi_hw->sr & SSI_SR_BUSY_BITS) {
    }
    ssi_hw->ssienr = 0;
    ssi_hw->baudr = _flash_clock_divider;
    // Sample read data one system clock late, as stage 2 does.
    ssi_hw->rx_sample_dly = 1;
    ssi_hw->ssienr = 1;
}

void mcu_processor_update_flash_timing(uint32_t sys_clk_khz) {
    // Use the smallest divider that keeps the flash clock within the flash's
    // 0xeb rating and RP2040_FLASH_MAX_SCK_KHZ. The SSI supports only even
    // dividers, 2 or more.
    uint32_t max_sck_khz = MIN(FLASH_EB_MAX_SCK_KHZ, RP2040_FLASH_MAX_SCK_KHZ);
    #ifdef CIRCUITPY_FLASH_MAX_SCK_HZ
    max_sck_khz = MIN(max_sck_khz, CIRCUITPY_FLASH_MAX_SCK_HZ / 1000);
    #endif
    uint32_t divider = (sys_clk_khz + max_sck_khz - 1) / max_sck_khz;
    divider += divider & 1;
    _flash_clock_divider = MAX(divider, 2);
    mcu_processor_restore_flash_timing();
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
    // Raise the voltage before raising the clock, and lower it after lowering
    // the clock.
    enum vreg_voltage current_voltage = vreg_get_voltage();
    if (voltage > current_voltage) {
        vreg_set_voltage(voltage);
        // Wait for a stable voltage
        common_hal_time_delay_ms(10);
    }

    // Prevent interrupt handlers from accessing flash or PSRAM until their
    // timing matches the new system clock.
    common_hal_mcu_disable_interrupts();
    // While the clock changes, use the flash timing for the faster of the old
    // and new clocks. That keeps the flash within its timing limits at both.
    mcu_processor_update_flash_timing(MAX(freq_khz, clock_get_hz(clk_sys) / 1000));
    set_sys_clock_khz(freq_khz, false);
    #ifdef CIRCUITPY_PSRAM_CHIP_SELECT
    mcu_processor_update_psram_timing(freq_khz);
    #endif
    mcu_processor_update_flash_timing(freq_khz);
    common_hal_mcu_enable_interrupts();
    if (voltage < current_voltage) {
        vreg_set_voltage(voltage);
    }
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

    #if CIRCUITPY_ALARM
    // The chip reset flags above stay set through a powman wake, so check it last.
    if (alarm_woke_from_powman()) {
        reason = MCU_RESET_REASON_DEEP_SLEEP_ALARM;
    }
    #endif
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
