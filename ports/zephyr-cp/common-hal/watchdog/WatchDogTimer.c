// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/watchdog/__init__.h"
#include "shared-bindings/watchdog/WatchDogMode.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "common-hal/watchdog/WatchDogTimer.h"

#include "bindings/zephyr_kernel/__init__.h"

#include "supervisor/background_callback.h"
#include "supervisor/port.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

// Register the CircuitPython watchdog port code as its own Zephyr log module
// but share the WDT module's log level (CONFIG_WDT_LOG_LEVEL, set to DBG via
// CONFIG_WDT_LOG_LEVEL_DBG=y), so driver and port logs turn on together.
LOG_MODULE_REGISTER(cp_watchdog, CONFIG_WDT_LOG_LEVEL);

// The hardware watchdog device, when the board provides one, is selected by
// the devicetree `watchdog0` alias (the same selection Zephyr's own watchdog
// sample, samples/drivers/watchdog, uses). RAISE mode works on every board via
// the software timer; only RESET mode needs the hardware device, so the alias
// is optional and boards without it raise NotImplementedError when RESET is
// requested. Use status-okay rather than node-exists so that boards which
// deliberately disable the alias target (e.g. Renesas RA boards, where the RA
// WDT driver doesn't build) still compile; a disabled node has no device
// struct, so DEVICE_DT_GET would fail for it.
#if DT_NODE_EXISTS(DT_ALIAS(watchdog0)) && DT_NODE_HAS_STATUS_OKAY(DT_ALIAS(watchdog0))
#define CP_WDT_HAS_HW 1
static const struct device *const wdt_dev = DEVICE_DT_GET(DT_ALIAS(watchdog0));
#else
#define CP_WDT_HAS_HW 0
static const struct device *const wdt_dev = NULL;
#endif

static background_callback_t wdt_timeout_callback;

// Raises the WatchDogTimeout exception in thread context. Runs from the
// background callback machinery after the watchdog's interrupt callback fed
// the hardware, so the VM state is never touched from interrupt context.
// DEBUG leds used to prove which watchdog paths actually run on hardware.
// Only compiled when the board's devicetree provides both led0 and led1
// (native_sim, e.g., only has led0).
#if DT_NODE_EXISTS(DT_NODELABEL(led0)) && DT_NODE_EXISTS(DT_NODELABEL(led1))
#define CP_WDT_DEBUG_LEDS 1
#else
#define CP_WDT_DEBUG_LEDS 0
#endif

#if CP_WDT_DEBUG_LEDS
static const struct gpio_dt_spec debug_led_red = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);
static const struct gpio_dt_spec debug_led_blue = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);

// Red LED ON: the raise timer expired in ISR context. Blue LED ON: the
// thread-context raise ran.
static void debug_isr_mark(void) {
    gpio_pin_set_dt(&debug_led_red, 1);
}

static void debug_thread_mark(void) {
    gpio_pin_set_dt(&debug_led_blue, 1);
}
#else
static void debug_isr_mark(void) {
}

static void debug_thread_mark(void) {
}
#endif

static void wdt_raise_in_thread(void *unused) {
    debug_thread_mark();
    LOG_DBG("raise WatchDogTimeout in thread context");

    mp_obj_exception_clear_traceback(MP_OBJ_FROM_PTR(&mp_watchdog_timeout_exception));
    MP_STATE_THREAD(mp_pending_exception) = &mp_watchdog_timeout_exception;
    #if MICROPY_ENABLE_SCHEDULER
    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
    #endif
}

// k_timer expiry, runs from the system clock timer ISR. Schedule the
// WatchDogTimeout raise in thread context via the background callback, which
// also wakes the VM task if it is idle. There is nothing to feed: the timer
// just stops on its own until the next k_timer_start().
static void wdt_raise_timer_expiry(struct k_timer *unused) {
    // LOG_* calls are ISR-safe in deferred logging mode (CONFIG_LOG_MODE_DEFERRED=y).
    debug_isr_mark();
    LOG_DBG("raise timer expired, scheduling thread raise");

    watchdog_watchdogtimer_obj_t *self = &common_hal_mcu_watchdogtimer_obj;
    self->mode = WATCHDOGMODE_NONE;

    background_callback_add(&wdt_timeout_callback, wdt_raise_in_thread, NULL);
}

// Static definition includes the k_timer_start()-installed expiry function.
K_TIMER_DEFINE(wdt_raise_timer, wdt_raise_timer_expiry, NULL);

// Install the timeout for the current mode and start the watchdog. For RAISE
// mode (software timer) restarting while running is fine; for RESET mode
// (hardware WDT) the watchdog must not be running (timeouts are installed
// before wdt_setup()).
static void watchdog_install(watchdog_watchdogtimer_obj_t *self) {
    // DEBUG: configure the status LEDs as outputs so the marks are visible.
    #if CP_WDT_DEBUG_LEDS
    gpio_pin_configure_dt(&debug_led_red, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&debug_led_blue, GPIO_OUTPUT_ACTIVE);
    gpio_pin_set_dt(&debug_led_red, 0);
    gpio_pin_set_dt(&debug_led_blue, 0);
    #endif

    // Zephyr's watchdog API is millisecond based, shared with CircuitPython's
    // float seconds through this window.
    uint32_t timeout_ms = (uint32_t)(self->timeout * 1000);

    if (self->mode == WATCHDOGMODE_RAISE) {
        // Nordic's WDT (and other hardware watchdogs) reset unconditionally
        // a few LFCLK cycles after the timeout event, callback or not, and
        // feeding cannot cancel that reset (nRF54 product spec). So RAISE
        // mode never touches the hardware watchdog: a Zephyr k_timer armed
        // with the timeout raises the exception instead, exactly what the
        // nordic port does with a TIMER peripheral. RESET mode keeps the
        // true hardware watchdog.
        LOG_DBG("raise mode: starting software timer for %u ms", timeout_ms);
        k_timer_start(&wdt_raise_timer, K_MSEC(timeout_ms), K_NO_WAIT);
        self->channel_id = -1;
        return;
    }

    // WATCHDOGMODE_RESET: no callback, reset the SoC on expiry. window.min is
    // left at 0, which drivers without feed windows require.
    if (!device_is_ready(wdt_dev)) {
        raise_zephyr_error(-ENODEV);
    }
    LOG_DBG("reset mode: installing hardware WDT with %u ms timeout", timeout_ms);
    struct wdt_timeout_cfg cfg = {
        .window.min = 0,
        .window.max = timeout_ms,
        .callback = NULL,
        .flags = WDT_FLAG_RESET_SOC,
    };
    int channel_id = wdt_install_timeout(wdt_dev, &cfg);
    CHECK_ZEPHYR_RESULT(channel_id);

    self->channel_id = channel_id;
    CHECK_ZEPHYR_RESULT(wdt_setup(wdt_dev, 0));
    // Start the first timeout period.
    CHECK_ZEPHYR_RESULT(wdt_feed(wdt_dev, channel_id));
}

void common_hal_watchdog_feed(watchdog_watchdogtimer_obj_t *self) {
    if (self->mode == WATCHDOGMODE_NONE) {
        return;
    }
    if (self->mode == WATCHDOGMODE_RAISE) {
        // Restart the timer: k_timer_start() on a running timer discards the
        // time already elapsed and arms the full period again.
        LOG_DBG("RAISE mode feed: restarting software timer for %u ms",
            (uint32_t)(self->timeout * 1000));
        k_timer_start(&wdt_raise_timer,
            K_MSEC((uint32_t)(self->timeout * 1000)), K_NO_WAIT);
        return;
    }
    int err = wdt_feed(wdt_dev, self->channel_id);
    if (err != 0) {
        LOG_ERR("feed failed: %d", err);
    }
    CHECK_ZEPHYR_RESULT(err);
}

// Stop the watchdog if the implementation allows it, leaving the mode state
// untouched when it doesn't (some hardware, like STM32's IWDG, can't be
// disabled once started). `mode = None` is the only Python-facing way to stop
// the watchdog since watchdog.deinit() was removed in 10.0.0.
static void watchdog_stop(watchdog_watchdogtimer_obj_t *self) {
    if (self->mode == WATCHDOGMODE_NONE) {
        return;
    }
    if (self->mode == WATCHDOGMODE_RAISE) {
        LOG_DBG("stopping software raise timer");
        k_timer_stop(&wdt_raise_timer);
        self->channel_id = -1;
        self->mode = WATCHDOGMODE_NONE;
        return;
    }
    LOG_DBG("disabling hardware WDT");
    if (!device_is_ready(wdt_dev) || wdt_disable(wdt_dev) < 0) {
        LOG_WRN("hw watchdog not stoppable; keeps running");
        return;
    }
    self->channel_id = -1;
    self->mode = WATCHDOGMODE_NONE;
}

mp_float_t common_hal_watchdog_get_timeout(watchdog_watchdogtimer_obj_t *self) {
    return self->timeout;
}

void common_hal_watchdog_set_timeout(watchdog_watchdogtimer_obj_t *self, mp_float_t new_timeout) {
    // Zephyr's API takes the timeout in milliseconds through a uint32_t.
    mp_arg_validate_int_max((mp_int_t)new_timeout, UINT32_MAX / 1000, MP_QSTR_timeout);
    self->timeout = new_timeout;

    if (self->mode == WATCHDOGMODE_NONE) {
        return;
    }

    if (self->mode == WATCHDOGMODE_RAISE) {
        // For the software timer a restart with the new value is enough.
        watchdog_install(self);
        return;
    }

    // Zephyr only accepts new timeout configurations while the hardware
    // watchdog is stopped. A hardware watchdog that can't be stopped keeps
    // running with the old configuration (same failure mode as failing
    // watchdog_stop()).
    int err = wdt_disable(wdt_dev);
    if (err != 0) {
        LOG_WRN("wdt_disable failed: %d; cannot install new timeout, watchdog "
            "keeps running with the old configuration", err);
    }
    CHECK_ZEPHYR_RESULT(err);
    self->channel_id = -1;

    watchdog_install(self);
}

watchdog_watchdogmode_t common_hal_watchdog_get_mode(watchdog_watchdogtimer_obj_t *self) {
    return self->mode;
}

void common_hal_watchdog_set_mode(watchdog_watchdogtimer_obj_t *self, watchdog_watchdogmode_t new_mode) {
    watchdog_watchdogmode_t current_mode = self->mode;
    if (new_mode == current_mode) {
        return;
    }

    if (new_mode == WATCHDOGMODE_NONE) {
        watchdog_stop(self);
        return;
    }

    if (new_mode != WATCHDOGMODE_RAISE && new_mode != WATCHDOGMODE_RESET) {
        return;
    }

    if (new_mode == WATCHDOGMODE_RESET) {
        // RESET mode needs the hardware watchdog: it must reset a locked-up
        // system, which no software timer can do. RAISE mode works on every
        // board; boards without a watchdog0 alias only support RAISE.
        #if CP_WDT_HAS_HW
        if (!device_is_ready(wdt_dev)) {
            raise_zephyr_error(-ENODEV);
        }
        #else
        mp_raise_NotImplementedError(NULL);
        #endif
    }

    if (current_mode != WATCHDOGMODE_NONE) {
        // Bring down the previous implementation. The software timer always
        // stops; a hardware watchdog only stops when the driver allows it.
        // Bringing a hardware watchdog down isn't always possible, and a
        // failure there leaves it running with the old configuration.
        if (current_mode == WATCHDOGMODE_RAISE) {
            k_timer_stop(&wdt_raise_timer);
            LOG_DBG("stopped software raise timer for mode switch");
        } else {
            int err = wdt_disable(wdt_dev);
            if (err != 0) {
                LOG_WRN("wdt_disable failed: %d; watchdog keeps running with "
                    "the old configuration", err);
            }
            CHECK_ZEPHYR_RESULT(err);
        }
    }
    self->channel_id = -1;

    self->mode = new_mode;

    watchdog_install(self);
}
