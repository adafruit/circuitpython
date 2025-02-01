// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <inttypes.h>

#include "py/gc.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/alarm/touch/TouchAlarm.h"

#include "shared-bindings/microcontroller/__init__.h"

#if CIRCUITPY_CYW43
#include "bindings/cyw43/__init__.h"
#include "common-hal/wifi/__init__.h"
#endif

#include "supervisor/port.h"
#include "supervisor/shared/workflow.h"
#include "supervisor/shared/serial.h"  // serial_connected()

#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "hardware/clocks.h"
#include "hardware/xosc.h"
#include "hardware/structs/scb.h"
#include "hardware/watchdog.h"
#include "hardware/structs/watchdog.h"
#include "hardware/pll.h"
#include "hardware/regs/io_bank0.h"

#ifdef PICO_RP2350
#include "hardware/powman.h"
#endif

#include "pico.h"
#include "pico/runtime_init.h"
#include "hardware/regs/clocks.h"
#include "rosc.h"

#ifdef __riscv
#include "hardware/riscv.h"
#endif

#ifdef SLEEP_DEBUG
#include "py/mpprint.h"
#include "py/mphal.h"
#define DEBUG_PRINT(fmt, ...) ((void)mp_printf(&mp_plat_print, "DBG:%s:%04d: " fmt "\n", __FILE__, __LINE__,##__VA_ARGS__))
#define SLEEP(ms) mp_hal_delay_ms(ms)
#else
#define DEBUG_PRINT(fmt, ...)((void)0)
#define SLEEP(ms)((void)0)
#endif

// This module uses code from pico-extras/src/rp2_common/pico_sleep.[ch]
// Naming conventions in the source is not uniform. Here, all functions
// from sleep.c are prefixed with _sleep. The functions are not 1:1 copies,
// since some of the coding is already part of e.g. time/TimeAlarm.c or
// pin/PinAlarm.c.
//
// Additional code (rosc.[cħ]) is from pico-extras/src/rp2_common/hardware_rosc.
// Since pico-extras is currently not pulled in as a submodule, these
// two files are copied into common-hal/alarm and used as is.
//
// The pico-SDK/pico-extras use the two terms "sleep" and "dormant", that
// are not identical to the terms "light-sleep" and "deep-sleep" from CP.
//
// The main difference between sleep and dormant is that the latter stops
// all clocks, preventing time-based alarms to fire. At least for the RP2040.
// The RP2350 gained a third clock "lposc", that allows dormant-mode with
// time-based alarms.

typedef enum {
    DORMANT_SOURCE_NONE,
    DORMANT_SOURCE_XOSC,
    DORMANT_SOURCE_ROSC,
    DORMANT_SOURCE_LPOSC, // rp2350 only
} dormant_source_t;

static void _sleep_run_from_dormant_source(dormant_source_t dormant_source);

static inline void _sleep_run_from_xosc(void) {
    _sleep_run_from_dormant_source(DORMANT_SOURCE_XOSC);
}

#ifdef PICO_RP2350
static inline void _sleep_run_from_lposc(void) {
    _sleep_run_from_dormant_source(DORMANT_SOURCE_LPOSC);
}
#endif

static dormant_source_t _dormant_source;

// State of the serial connection
static bool _serial_connected;

// In order to go into dormant mode we need to be running from a stoppable clock source:
// either the xosc or rosc with no PLLs running. This means we disable the USB and ADC clocks
// and all PLLs
static void _sleep_run_from_dormant_source(dormant_source_t dormant_source) {
    _dormant_source = dormant_source;

    uint src_hz;
    uint clk_ref_src;
    switch (dormant_source) {
        case DORMANT_SOURCE_XOSC:
            src_hz = XOSC_HZ;
            clk_ref_src = CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC;
            break;
        case DORMANT_SOURCE_ROSC:
            src_hz = 6500 * KHZ; // todo
            clk_ref_src = CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH;
            break;
        #ifdef PICO_RP2350
        case DORMANT_SOURCE_LPOSC:
            src_hz = 32 * KHZ;
            clk_ref_src = CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC;
            break;
        #endif
        default:
            hard_assert(false);
    }

    // CLK_REF = XOSC or ROSC
    clock_configure(clk_ref,
        clk_ref_src,
        0,             // No aux mux
        src_hz,
        src_hz);

    // CLK SYS = CLK_REF
    clock_configure(clk_sys,
        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
        0,             // Using glitchless mux
        src_hz,
        src_hz);

    // CLK ADC = 0MHz
    clock_stop(clk_adc);
    clock_stop(clk_usb);
    #ifdef PICO_RP2350
    clock_stop(clk_hstx);
    #endif

    #ifdef PICO_RP2040
    // CLK RTC = ideally XOSC (12MHz) / 256 = 46875Hz but could be rosc
    uint clk_rtc_src = (dormant_source == DORMANT_SOURCE_XOSC) ?
        CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_XOSC_CLKSRC :
        CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_ROSC_CLKSRC_PH;

    clock_configure(clk_rtc,
        0,             // No GLMUX
        clk_rtc_src,
        src_hz,
        46875);
    #endif

    // CLK PERI = clk_sys. Used as reference clock for Peripherals. No dividers so just select and enable
    clock_configure(clk_peri,
        0,
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
        src_hz,
        src_hz);

    pll_deinit(pll_sys);
    pll_deinit(pll_usb);

    // Assuming both xosc and rosc are running at the moment
    if (dormant_source == DORMANT_SOURCE_XOSC) {
        // Can disable rosc
        rosc_disable();
    } else {
        // Can disable xosc
        xosc_disable();
    }
}

static void _sleep_processor_deep_sleep(void) {
    // Enable deep sleep at the proc
    #ifdef __riscv
    uint32_t bits = RVCSR_MSLEEP_POWERDOWN_BITS;
    if (!get_core_num()) {
        bits |= RVCSR_MSLEEP_DEEPSLEEP_BITS;
    }
    riscv_set_csr(RVCSR_MSLEEP_OFFSET, bits);
    #else
    scb_hw->scr |= ARM_CPU_PREFIXED(SCR_SLEEPDEEP_BITS);
    #endif
}

// saved values of the clocks
uint32_t _saved_sleep_en0;
uint32_t _saved_sleep_en1;

// slightly modified compared to pico-extras, since we set the
// alarm and callback elsewhere and only use it with RP2040
#ifdef PICO_RP2040
static void _sleep_goto_sleep_until(void) {
    DEBUG_PRINT("_sleep_goto_sleep_until");
    SLEEP(10);

    _saved_sleep_en0 = clocks_hw->sleep_en0;
    _saved_sleep_en1 = clocks_hw->sleep_en1;
    clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS;
    clocks_hw->sleep_en1 = 0x0;

    // Enable deep sleep at the proc
    _sleep_processor_deep_sleep();

    // Go to sleep
    __wfi();
}
#endif

static void _sleep_go_dormant(void) {
    if (_dormant_source == DORMANT_SOURCE_XOSC) {
        xosc_dormant();
    } else {
        rosc_set_dormant();
    }
    // at this point we are in dormant state
}

#ifdef PICO_RP2350
// slightly modified compared to pico-extras, since we set the
// alarm and callback elsewhere. We also don't expect this to work
// for the RP2040 (no external crystal)
static void _sleep_goto_dormant_until(void) {
    // We should have already called the _sleep_run_from_dormant_source function

    assert(_dormant_source == DORMANT_SOURCE_LPOSC);
    uint64_t restore_ms = powman_timer_get_ms();
    powman_timer_set_1khz_tick_source_lposc();
    powman_timer_set_ms(restore_ms);

    _saved_sleep_en0 = clocks_hw->sleep_en0;
    _saved_sleep_en1 = clocks_hw->sleep_en1;
    clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_REF_POWMAN_BITS;
    clocks_hw->sleep_en1 = 0x0;

    // Enable deep sleep at the proc
    _sleep_processor_deep_sleep();

    // Go dormant
    // _sleep_go_dormant();  (not here, moved to _goto_sleep_or_dormant)
}
#endif

// To be called after waking up from sleep/dormant mode to restore system clocks properly
static void _sleep_power_up(void) {
    // Re-enable the ring oscillator, which will essentially kickstart the proc
    rosc_enable();

    // Reset the sleep enable register so peripherals and other hardware can be used
    clocks_hw->sleep_en0 = _saved_sleep_en0;
    clocks_hw->sleep_en1 = _saved_sleep_en1;

    // Restore all clocks
    clocks_init();

    #ifdef PICO_RP2350
    // make powerman use xosc again
    uint64_t restore_ms = powman_timer_get_ms();
    powman_timer_set_1khz_tick_source_xosc();
    powman_timer_set_ms(restore_ms);
    #endif

    #if CIRCUITPY_CYW43
    bindings_cyw43_power_up();
    wifi_power_up_reset();
    #endif
}

// enter sleep or dormant mode
// There are the following different cases:
//   - RP2040: TimeAlarm -> use sleep with aon-wakeup
//             PinAlarm  -> use dormant with gpio-wakeup
//   - RP2350: TimeAlarm -> use dormant with aon-wakeup
//             PinAlarm  -> use dormant with gpio-wakeup
//
// The low-level implementation does not differentiate between light-sleep
// and deep-sleep.
static void _goto_sleep_or_dormant(void) {
    DEBUG_PRINT("_goto_sleep_or_dormant");
    bool timealarm_set = alarm_time_timealarm_is_set();
    _serial_connected = serial_connected();
    DEBUG_PRINT("time-alarm: %s", timealarm_set ? "true": "false");
    DEBUG_PRINT("serial: %s", _serial_connected ? "true": "false");
    SLEEP(10);

    // Just before sleep, enable the pinalarm interrupt.
    alarm_pin_pinalarm_entering_deep_sleep();

    // when serial is connected, only fake sleep/dormant
    if (_serial_connected) {
        __wfi();
        return;
    }

    #if CIRCUITPY_CYW43
    bindings_cyw43_power_down();
    #endif

    #ifdef PICO_RP2040
    _sleep_run_from_xosc();     // calls _sleep_run_from_dormant_source
    if (timealarm_set) {
        _sleep_goto_sleep_until();
    } else {
        _sleep_go_dormant();
    }
    _sleep_power_up();
    #else
    _sleep_run_from_lposc();
    if (timealarm_set) {
        _sleep_goto_dormant_until();
    }
    _sleep_go_dormant();
    _sleep_power_up();
    #endif
}

// Watchdog scratch register
// Not used elsewhere in the SDK for now, keep an eye on it
#define RP_WKUP_SCRATCH_REG 0

// Singleton instance of SleepMemory.
const alarm_sleep_memory_obj_t alarm_sleep_memory_obj = {
    .base = {
        .type = &alarm_sleep_memory_type,
    },
};

// Non-heap alarm object recording alarm (if any) that woke up CircuitPython after light or deep sleep.
// This object lives across VM instantiations, so none of these objects can contain references to the heap.
alarm_wake_alarm_union_t alarm_wake_alarm;

void alarm_reset(void) {
    DEBUG_PRINT("alarm_reset");
    SLEEP(10);
    alarm_sleep_memory_reset();
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();

    // Reset the scratch source
    watchdog_hw->scratch[RP_WKUP_SCRATCH_REG] = RP_SLEEP_WAKEUP_UNDEF;
}

static uint8_t _get_wakeup_cause(void) {
    // First check if the modules remember what last woke up
    if (alarm_pin_pinalarm_woke_this_cycle()) {
        DEBUG_PRINT("_get_wakeup_cause: pin-alarm");
        SLEEP(10);
        return RP_SLEEP_WAKEUP_GPIO;
    }
    if (alarm_time_timealarm_woke_this_cycle()) {
        DEBUG_PRINT("_get_wakeup_cause: time-alarm");
        SLEEP(10);
        return RP_SLEEP_WAKEUP_RTC;
    }
    // If waking from true deep sleep, modules will have lost their state,
    // so check the deep wakeup cause manually
    if (watchdog_hw->scratch[RP_WKUP_SCRATCH_REG] != RP_SLEEP_WAKEUP_UNDEF) {
        return watchdog_hw->scratch[RP_WKUP_SCRATCH_REG];
    }
    return RP_SLEEP_WAKEUP_UNDEF;
}

// Set up light sleep or deep sleep alarms.
static void _setup_sleep_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    DEBUG_PRINT("_setup_sleep_alarms (start)");
    alarm_pin_pinalarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_time_timealarm_set_alarms(deep_sleep, n_alarms, alarms);
    DEBUG_PRINT("_setup_sleep_alarms (finished)");
    SLEEP(10);
}

bool common_hal_alarm_woken_from_sleep(void) {
    return _get_wakeup_cause() != RP_SLEEP_WAKEUP_UNDEF;
}

mp_obj_t common_hal_alarm_record_wake_alarm(void) {
    // If woken from deep sleep, create a copy alarm similar to what would have
    // been passed in originally. Otherwise, just return none
    uint8_t cause = _get_wakeup_cause();
    switch (cause) {
        case RP_SLEEP_WAKEUP_RTC: {
            return alarm_time_timealarm_record_wake_alarm();
        }

        case RP_SLEEP_WAKEUP_GPIO: {
            return alarm_pin_pinalarm_record_wake_alarm();
        }

        case RP_SLEEP_WAKEUP_UNDEF:
        default:
            // Not a deep sleep reset.
            break;
    }
    return mp_const_none;
}

mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    DEBUG_PRINT("common_hal_alarm_light_sleep_until_alarms (start)");
    SLEEP(10);
    _setup_sleep_alarms(false, n_alarms, alarms);

    mp_obj_t wake_alarm = mp_const_none;

    while (!mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        // Detect if interrupt was alarm or ctrl-C interrupt.
        if (common_hal_alarm_woken_from_sleep()) {
            uint8_t cause = _get_wakeup_cause();
            switch (cause) {
                case RP_SLEEP_WAKEUP_RTC: {
                    wake_alarm = alarm_time_timealarm_find_triggered_alarm(n_alarms, alarms);
                    break;
                }
                case RP_SLEEP_WAKEUP_GPIO: {
                    wake_alarm = alarm_pin_pinalarm_find_triggered_alarm(n_alarms, alarms);
                    break;
                }
                default:
                    // Should not reach this, if all light sleep types are covered correctly
                    break;
            }
            shared_alarm_save_wake_alarm(wake_alarm);
            break;
        }
        _goto_sleep_or_dormant();
    }

    if (mp_hal_is_interrupted()) {
        return mp_const_none; // Shouldn't be given to python code because exception handling should kick in.
    }

    alarm_reset();
    return wake_alarm;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms, size_t n_dios, digitalio_digitalinout_obj_t **preserve_dios) {
    if (n_dios > 0) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_preserve_dios);
    }
    _setup_sleep_alarms(true, n_alarms, alarms);
}

void NORETURN common_hal_alarm_enter_deep_sleep(void) {

    _goto_sleep_or_dormant();

    // Reset uses the watchdog. Use scratch registers to store wake reason
    watchdog_hw->scratch[RP_WKUP_SCRATCH_REG] = _get_wakeup_cause();

    reset_cpu();
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(shared_alarm_get_wake_alarm());
}
