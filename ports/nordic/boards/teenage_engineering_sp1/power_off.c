// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "power_off.h"

#ifdef BOARD_POWER_OFF_BUTTON_PIN

#include "py/misc.h"

#include "supervisor/filesystem.h"
#include "supervisor/flash.h"

#include "wdt.h"
#include "nrfx/hal/nrf_gpio.h"
#include "nrfx/hal/nrf_power.h"

#ifndef BOARD_POWER_OFF_HOLD_SECONDS
#define BOARD_POWER_OFF_HOLD_SECONDS (3)
#endif

// Timings in RTC subticks (32.768 kHz). The counter is 24 bits, so every
// comparison is masked; it wraps every 512s, longer than needed.
#define RTC_COUNTER_MASK            (0xFFFFFF)
#define POLL_INTERVAL_SUBTICKS      (1024)      // ~31 ms
#define POWER_OFF_HOLD_SUBTICKS     (BOARD_POWER_OFF_HOLD_SECONDS * 32768)
#define RELEASE_DEBOUNCE_SUBTICKS   (1638)      // ~50 ms

// How long the gesture will wait for a filesystem that is being built
#ifndef BOARD_POWER_OFF_FILESYSTEM_GRACE_SECONDS
#define BOARD_POWER_OFF_FILESYSTEM_GRACE_SECONDS (30)
#endif
#define FILESYSTEM_GRACE_SUBTICKS   (BOARD_POWER_OFF_FILESYSTEM_GRACE_SECONDS * 32768)

// The RTC that port.c runs the tick from, read straight out of its counter.
#define POWER_OFF_RTC (NRF_RTC2)

// Do-nothing default; a board with hardware to quiesce overrides this.
MP_WEAK void board_power_off_prepare(void) {
}

#ifdef BOARD_POWER_OFF_CONFIRM_LED_PIN

#ifndef BOARD_POWER_OFF_CONFIRM_LED_MS
#define BOARD_POWER_OFF_CONFIRM_LED_MS (200)
#endif
#define CONFIRM_BLINK_SUBTICKS ((BOARD_POWER_OFF_CONFIRM_LED_MS) * 32768 / 1000)

// One flash to say the gesture landed.
static void power_off_confirm_blink(void) {
    nrf_gpio_cfg_output(BOARD_POWER_OFF_CONFIRM_LED_PIN);
    nrf_gpio_pin_set(BOARD_POWER_OFF_CONFIRM_LED_PIN);
    // Busy-wait on the same RTC the release loop uses, feeding the watchdog:
    // 200 ms is comfortably longer than a bootloader-armed dog's patience.
    uint32_t started = POWER_OFF_RTC->COUNTER;
    while (((POWER_OFF_RTC->COUNTER - started) & RTC_COUNTER_MASK) < CONFIRM_BLINK_SUBTICKS) {
        bootloader_wdt_feed();
    }
    nrf_gpio_pin_clear(BOARD_POWER_OFF_CONFIRM_LED_PIN);
}

#endif // BOARD_POWER_OFF_CONFIRM_LED_PIN

// Power-off is a sequence, not a register write, and the order matters.
static void power_off(void) {
    // 0. Commit the filesystem. Hold-to-power-off is this device's normal
    //    "off", so the dirty page sitting in the flash cache is typically the
    //    last thing FAT wrote.
    if (filesystem_present()) {
        supervisor_flash_flush();
    }

    // 1. Let the board put its own hardware to bed first, while everything is
    //    still powered and predictable.
    board_power_off_prepare();

    // 1a. Confirm the gesture with one flash, before anything else changes.
    //     Deliberately after the prepare hook, so it is drawing on a board that
    //     is already quiesced and the LED it leaves behind is off.
    #ifdef BOARD_POWER_OFF_CONFIRM_LED_PIN
    power_off_confirm_blink();
    #endif

    // 2. Detach from USB
    NRF_USBD->USBPULLUP = 0;
    NRF_USBD->ENABLE = 0;

    // 3. Wait for the button to be released, feeding the watchdog meanwhile.
    uint32_t released_since = POWER_OFF_RTC->COUNTER;
    while (true) {
        bootloader_wdt_feed();
        uint32_t now = POWER_OFF_RTC->COUNTER;
        if (nrf_gpio_pin_read(BOARD_POWER_OFF_BUTTON_PIN) == 0) {
            released_since = now;
        } else if (((now - released_since) & RTC_COUNTER_MASK) >= RELEASE_DEBOUNCE_SUBTICKS) {
            break;
        }
    }

    // 4. Clear RESETREAS so the next boot can tell a wake-from-off from a
    //    watchdog reset.
    NRF_POWER->RESETREAS = NRF_POWER->RESETREAS;

    // 5. Arm the wake. Clear any latched DETECT first.
    NRF_P0->LATCH = 0xFFFFFFFF;
    NRF_P1->LATCH = 0xFFFFFFFF;
    nrf_gpio_cfg_sense_input(BOARD_POWER_OFF_BUTTON_PIN,
        NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

    // 6. Off. Note that the spin below deliberately does not feed the
    //    watchdog. If SYSTEM_OFF does not take a bootloader-armed dog
    //    bites within seconds and the board comes back up normally.
    __DSB();
    NRF_POWER->SYSTEMOFF = 1;
    __DSB();
    while (true) {
    }
}

// Reading a pin whose input buffer is disconnected returns 0, which is
// indistinguishable from the button being held. power_off_tick() would see a
// button that is down on the very first poll and never released, so the gesture
// would arm itself off permanently and the board would silently lose its only
// way to power down.
//
// So check the buffer every poll and reconnect it if it
// has gone away. Anything already configured is left exactly as it is.
static void ensure_input_buffer_connected(void) {
    uint32_t pin_number = BOARD_POWER_OFF_BUTTON_PIN;
    NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin_number);
    if ((reg->PIN_CNF[pin_number] & GPIO_PIN_CNF_INPUT_Msk) ==
        (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)) {
        // Pull-up, matching the active-low switch-to-ground the header
        // documents. Without a pull the line floats and the read is noise.
        nrf_gpio_cfg_input(BOARD_POWER_OFF_BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
    }
}

static bool power_off_deferred(uint32_t now) {
    static uint32_t absent_since_subticks;
    static bool was_absent;

    if (filesystem_present()) {
        was_absent = false;
        return false;
    }

    if (!was_absent) {
        was_absent = true;
        absent_since_subticks = now;
    }
    return ((now - absent_since_subticks) & RTC_COUNTER_MASK) < FILESYSTEM_GRACE_SUBTICKS;
}

void power_off_tick(void) {
    uint32_t now = POWER_OFF_RTC->COUNTER;
    static uint32_t last_poll_subticks;
    if (((now - last_poll_subticks) & RTC_COUNTER_MASK) < POLL_INTERVAL_SUBTICKS) {
        return;
    }
    last_poll_subticks = now;

    if (power_off_deferred(now)) {
        return;
    }

    ensure_input_buffer_connected();

    // Reading IN never disturbs the pin, so the gesture still works if user
    // code has claimed the button.
    bool pressed = nrf_gpio_pin_read(BOARD_POWER_OFF_BUTTON_PIN) == 0;

    // Waking from SYSTEM_OFF happens with the button still held, and the
    // bootloader plus start up take far less than the hold time, so a fresh
    // boot would otherwise see a hold already in progress and power straight
    // back off. Require the button to be seen released once first.
    static bool gesture_armed;
    static bool was_pressed;
    if (!pressed) {
        gesture_armed = true;
        was_pressed = false;
        return;
    }
    if (!gesture_armed) {
        return;
    }

    static uint32_t press_started_subticks;
    if (!was_pressed) {
        was_pressed = true;
        press_started_subticks = now;
        return;
    }
    if (((now - press_started_subticks) & RTC_COUNTER_MASK) >= POWER_OFF_HOLD_SUBTICKS) {
        power_off();
    }
}

#endif // BOARD_POWER_OFF_BUTTON_PIN
