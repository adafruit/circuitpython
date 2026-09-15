// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Early-boot hygiene for the Teenage Engineering SP-1.
//
// Unlike a normal CircuitPython board, this one is entered from a bootloader
// that has already brought hardware up: it starts HFCLK and LFCLK, and leaves
// PWM2, PWM3 and the SAADC enabled. It also starts a watchdog that we cannot
// stop. So the app has to take the machine over from a *running* state rather
// than a reset state, and it has to do so quickly.
//
// The bootloader's watchdog is the reason for the feeds scattered through this
// file: it is running before we are, its configuration is locked, and nothing
// but a reload keeps it from resetting the board. See wdt.h.

#include "supervisor/board.h"

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/busio/I2C.h"

#include "background.h"
#include "board.h"
#include "common-hal/microcontroller/Pin.h"
#include "flash_protect.h"
#include "peripherals/nrf/pins.h"
#include "power_off.h"
#include "py/misc.h"
#include "supervisor/shared/safe_mode.h"
#include "shared-module/emmcio/__init__.h"
#include "wdt.h"
#include "nrfx/drivers/include/nrfx_rtc.h"
#include "nrfx/hal/nrf_gpio.h"

#define PIN_EMMC_RESET      (DEFAULT_EMMC_RESET->number)   // active low
#define PIN_EMMC_VCCQ_EN    (DEFAULT_EMMC_VCCQ->number)    // eMMC I/O rail

// Pins this file drives directly.
#define PIN_OSC_EN          NRF_GPIO_PIN_MAP(0, 13)  // 3.072 MHz oscillator
#define PIN_TAS_RESET       NRF_GPIO_PIN_MAP(0, 9)   // TAS2505, active low
#define PIN_CS42_RESET      NRF_GPIO_PIN_MAP(0, 15)  // CS42L42, active low
#define PIN_BT_RESET        NRF_GPIO_PIN_MAP(0, 10)  // CYBT-353027-02, active low
#define PIN_CONTROL_RAIL    NRF_GPIO_PIN_MAP(1, 10)  // feeds faders + ladders
#define PIN_FUNCTION_BUTTON NRF_GPIO_PIN_MAP(0, 27)  // active low, only GPIO button
#define PIN_CHARGE_ENABLE   NRF_GPIO_PIN_MAP(0, 21)  // BQ24232, active low
#define PIN_I2C_SCL         NRF_GPIO_PIN_MAP(1, 11)  // shared by both codecs
#define PIN_I2C_SDA         NRF_GPIO_PIN_MAP(1, 7)

// Both LED rows, active high. PIN_LED_HEARTBEAT is the first track LED, which
// is also MICROPY_HW_LED_STATUS and BOARD_POWER_OFF_CONFIRM_LED_PIN
#define PIN_LED_HEARTBEAT   NRF_GPIO_PIN_MAP(0, 29)
static const uint8_t led_pins[] = {
    NRF_GPIO_PIN_MAP(1, 13), NRF_GPIO_PIN_MAP(0, 0),   // playback row (side)
    NRF_GPIO_PIN_MAP(1, 12), NRF_GPIO_PIN_MAP(0, 1),
    PIN_LED_HEARTBEAT, NRF_GPIO_PIN_MAP(0, 26),        // track row (front)
    NRF_GPIO_PIN_MAP(1, 15), NRF_GPIO_PIN_MAP(1, 14),
};

// Whether the boot up blink heartbeat still owns PIN_LED_HEARTBEAT.
static bool heartbeat_lit;

// The bootloader's watchdog is fed from board_background_task(), so the CPU
// must not stay in WFI for longer than the bootloader will wait. The port owns
// RTC2 and this board builds no SoftDevice (which would own RTC0), so RTC1 is
// free to hand the main loop its turn back on a fixed period.
//
// Waking is the whole job. The feed itself deliberately stays in the main loop,
// so that a wedge there still becomes a reset.
static const nrfx_rtc_t wake_rtc = NRFX_RTC_INSTANCE(1);
#define WAKE_RTC_CHANNEL (0)
// Run the counter straight off the LFCLK, as the port's own RTC does.
#define WAKE_RTC_FREQUENCY_HZ (32768)
// How long the CPU may stay in WFI before the main loop must get another look
// in.
#define WAKE_RTC_PERIOD_TICKS (WAKE_RTC_FREQUENCY_HZ)

static void arm_wake_rtc(void) {
    nrfx_rtc_cc_set(&wake_rtc, WAKE_RTC_CHANNEL,
        nrfx_rtc_counter_get(&wake_rtc) + WAKE_RTC_PERIOD_TICKS, true);
}

static void wake_rtc_handler(nrfx_rtc_int_type_t int_type) {
    (void)int_type;
    arm_wake_rtc();
}

// Bounded wait for a peripheral to acknowledge a STOP.
#define STOP_TIMEOUT_ITERATIONS (20000)

static void wait_for_event(volatile uint32_t *event) {
    for (uint32_t i = 0; i < STOP_TIMEOUT_ITERATIONS && *event == 0; i++) {
        __NOP();
    }
    *event = 0;
    // Read back to flush the write buffer, per the nRF52 errata guidance for
    // clearing events.
    (void)*event;
}

static void stop_pwm(NRF_PWM_Type *pwm) {
    if (pwm->ENABLE == 0) {
        return;
    }
    pwm->EVENTS_STOPPED = 0;
    pwm->TASKS_STOP = 1;
    wait_for_event(&pwm->EVENTS_STOPPED);
    pwm->INTENCLR = 0xFFFFFFFF;
    pwm->ENABLE = 0;
}

void board_early_init(void) {
    // Feed the bootloader's watchdog before anything else. This is the first
    // CircuitPython code to run on the board: port_init() calls it before it
    // touches a peripheral.
    bootloader_wdt_feed();

    // Lock the bootloader out of NVMC's reach for the rest of this boot. First,
    // because from here on every line of CircuitPython that runs is one more
    // thing that could get it wrong, and the bootloader is this board's only
    // way back in. See flash_protect.h.
    board_flash_protect();

    // A watchdog reset can only mean the main loop stopped, and on this board
    // that is worth safe mode with or without a host attached: there is no
    // reset pin and no removable battery, so re-running the same wedging
    // `code.py` is the one thing that can make the device unrecoverable.
    // port_init() asks for safe mode itself when USB is attached; this covers
    // the battery case. RESETREAS is still untouched here -- port_init() reads
    // and clears it after we return -- and the request is picked up by
    // wait_for_safe_mode_reset() later in this same boot, not after a reset.
    if ((NRF_POWER->RESETREAS & POWER_RESETREAS_DOG_Msk) != 0 &&
        (NRF_POWER->USBREGSTATUS & POWER_USBREGSTATUS_VBUSDETECT_Msk) == 0) {
        safe_mode_on_next_reset(SAFE_MODE_WATCHDOG);
    }

    // First light boot up status blink.
    //
    //   never lights          the bootloader did not jump here, or we died in
    //                         the reset handler / SystemInit
    //   lights and stays on   we are running, but did not reach board_init():
    //                         suspect the filesystem format or something
    //                         before the workflow starts
    //   lights, then goes out  board_init() reached; USB is next, so from here
    //                         on the absence of a tty is a USB problem
    //
    nrf_gpio_cfg_output(PIN_LED_HEARTBEAT);
    nrf_gpio_pin_set(PIN_LED_HEARTBEAT);
    heartbeat_lit = true;

    for (size_t i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    __DSB();
    __ISB();

    // Break any PPI wiring before stopping peripherals, so nothing we stop can
    // be restarted by a leftover event->task connection.
    NRF_PPI->CHENCLR = 0xFFFFFFFF;

    // The bootloader drives the LEDs with PWM2 and PWM3. PWM0/PWM1 are stopped
    // too so that pwmio starts from a known state.
    stop_pwm(NRF_PWM0);
    stop_pwm(NRF_PWM1);
    stop_pwm(NRF_PWM2);
    stop_pwm(NRF_PWM3);

    // The bootloader reads the button ladders with the SAADC and leaves it
    // enabled, so clear it.
    if (NRF_SAADC->ENABLE != 0) {
        NRF_SAADC->EVENTS_STOPPED = 0;
        NRF_SAADC->TASKS_STOP = 1;
        wait_for_event(&NRF_SAADC->EVENTS_STOPPED);
        NRF_SAADC->INTENCLR = 0xFFFFFFFF;
        NRF_SAADC->EVENTS_END = 0;
        NRF_SAADC->EVENTS_STARTED = 0;
        NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
        NRF_SAADC->ENABLE = 0;
    }

    // Last, because the blanket NVIC clear above would undo it: the periodic
    // wake-up that keeps the main loop feeding the watchdog. LFCLK is not
    // running yet, so the counter starts when port_init() starts it, a few
    // instructions from here.
    static const nrfx_rtc_config_t wake_rtc_config = {
        .prescaler = RTC_FREQ_TO_PRESCALER(WAKE_RTC_FREQUENCY_HZ),
        .reliable = 0,
        .tick_latency = 0,
        .interrupt_priority = 6,
    };
    nrfx_rtc_init(&wake_rtc, &wake_rtc_config, wake_rtc_handler);
    arm_wake_rtc();
    nrfx_rtc_enable(&wake_rtc);
}

// Pins that must not float. reset_all_pins() and reset_pin_number() ask the
// board for each pin, so this configuration is re-applied after every reset
// rather than the pin being left in its default (disconnected) state.
//
// None of these are marked never-reset, so Python can still claim them. This
// only makes the resting state between runs a defined, safe one.
static const uint8_t default_low_pins[] = {
    // 3.072 MHz oscillator enable. Held low: it draws current straight through
    // SYSTEM_OFF, so a floating pin here would drain battery.
    PIN_OSC_EN,

    // Codecs and the Bluetooth module held in reset (all active low) so that
    // nothing downstream of us starts making noise or driving a shared bus on
    // its own. P0.09/P0.10 are the NFC pins; UICR NFCPINS reads with PROTECT
    // already cleared on this board, so they are usable as GPIO.
    PIN_TAS_RESET,
    PIN_CS42_RESET,
    PIN_BT_RESET,

    // Rail feeding the faders and both button ladders. Off unless something is
    // actually reading them.
    PIN_CONTROL_RAIL,

    // BQ24232 charge enable, active low: drive it low so a plugged-in device
    // charges. P1.00 (CHARGE_ISET) is deliberately left untouched. It is the
    // charge-current programming node (ICHG = 870 AΩ / RISET) and doubles as
    // the current monitor.
    PIN_CHARGE_ENABLE,
};

// Returns false for a pin this board has no opinion about.
static bool apply_pin_default(uint8_t pin_number) {
    // Function button: the only GPIO button, active low, and the only wake
    // source out of SYSTEM_OFF. Keep it readable at all times, the
    // supervisor's power-off gesture depends on it.
    if (pin_number == PIN_FUNCTION_BUTTON) {
        nrf_gpio_cfg_input(PIN_FUNCTION_BUTTON, NRF_GPIO_PIN_PULLUP);
        return true;
    }

    // Both LED rows, off.
    for (size_t i = 0; i < MP_ARRAY_SIZE(led_pins); i++) {
        if (led_pins[i] == pin_number) {
            nrf_gpio_cfg_output(pin_number);
            nrf_gpio_pin_clear(pin_number);
            return true;
        }
    }

    // eMMC held in reset with its VCCQ rail off. The contents of the chip are
    // unaffected; this only keeps the rail from floating. The exception is a
    // card the supervisor has automounted: it is a live filesystem between
    // runs, so leave its reset and rail exactly as the driver left them.
    if (pin_number == PIN_EMMC_RESET || pin_number == PIN_EMMC_VCCQ_EN) {
        if (!emmcio_is_automounted()) {
            nrf_gpio_cfg_output(pin_number);
            nrf_gpio_pin_clear(pin_number);
        }
        return true;
    }

    for (size_t i = 0; i < MP_ARRAY_SIZE(default_low_pins); i++) {
        if (default_low_pins[i] == pin_number) {
            nrf_gpio_cfg_output(pin_number);
            nrf_gpio_pin_clear(pin_number);
            return true;
        }
    }

    return false;
}

// Put every pin this board has an opinion about into its resting state at once.
static void apply_all_pin_defaults(void) {
    apply_pin_default(PIN_FUNCTION_BUTTON);
    apply_pin_default(PIN_EMMC_RESET);
    apply_pin_default(PIN_EMMC_VCCQ_EN);
    for (size_t i = 0; i < MP_ARRAY_SIZE(led_pins); i++) {
        apply_pin_default(led_pins[i]);
    }
    for (size_t i = 0; i < MP_ARRAY_SIZE(default_low_pins); i++) {
        apply_pin_default(default_low_pins[i]);
    }
}

bool board_reset_pin_number(uint8_t pin_number) {
    // main() calls reset_all_pins() immediately after port_init(), so without
    // this the boot up heartbeat blink would last microseconds and show
    // nothing. board_init() hands the pin back.
    if (heartbeat_lit && pin_number == PIN_LED_HEARTBEAT) {
        return true;
    }

    return apply_pin_default(pin_number);
}

// Called once at start up, after the filesystem is mounted and immediately
// before the USB workflow starts. Where we end the heartbeat status blink.
void board_init(void) {
    heartbeat_lit = false;
    nrf_gpio_pin_clear(PIN_LED_HEARTBEAT);
}

void board_background_task(void) {
    bootloader_wdt_feed();

    #ifdef BOARD_POWER_OFF_BUTTON_PIN
    // Never returns if the hold completes.
    power_off_tick();
    #endif
}

// -- muting the codecs on the way out --------------------------------------
//
// Dropping the reset lines and the oscillator (apply_all_pin_defaults()) is
// enough to make the board quiet and to save the battery, but it cuts both
// codecs off mid-signal: the CS42L42 loses its clock and the TAS2505's class-D
// driver loses its reset with whatever was on the output still on it.
//
// Bit-banged rather than driven through TWIM.

#define I2C_FREQUENCY (100000)
#define I2C_TIMEOUT_MS (255)

static busio_i2c_obj_t codec_i2c;

// two-byte write
static bool i2c_write2(uint8_t address, uint8_t first, uint8_t second) {
    const uint8_t data[2] = { first, second };
    return common_hal_busio_i2c_write(&codec_i2c, address, data, sizeof(data)) == 0;
}

// Register addresses
#define PAGE_SELECT_REG     (0x00)  // register 0 selects the page, on both

#define CS42L42_ADDRESS     (0x48)
#define CS_HP_CTL_PAGE      (0x20)  // CS_HP_CTL = 0x2001, page = high byte
#define CS_HP_CTL_REG       (0x01)
#define CS_HP_MUTE          (0x0D)

#define TAS2505_ADDRESS     (0x18)
#define TAS_SW_RESET        (0x01)  // page 0
#define TAS_DAC_MUTE        (0x40)  // page 0
#define TAS_MUTED           (0x0C)
#define TAS_SPK_POWER       (0x2D)  // page 1

// Mute the CS42L42, then mute the TAS2505, power its class-D driver down and
// soft-reset it.
static void quiesce_codecs(void) {
    bootloader_wdt_feed();

    #if CIRCUITPY_BOARD_I2C

    busio_i2c_obj_t *shared = common_hal_board_get_i2c(0);
    if (shared != NULL) {
        common_hal_busio_i2c_unlock(shared);
        common_hal_busio_i2c_deinit(shared);
    }
    #endif

    if (!pin_number_is_free(PIN_I2C_SCL) || !pin_number_is_free(PIN_I2C_SDA)) {
        return;
    }

    codec_i2c.base.type = &busio_i2c_type;
    common_hal_busio_i2c_construct(&codec_i2c, DEFAULT_I2C_BUS_SCL,
        DEFAULT_I2C_BUS_SDA, I2C_FREQUENCY, I2C_TIMEOUT_MS);

    if (i2c_write2(CS42L42_ADDRESS, PAGE_SELECT_REG, CS_HP_CTL_PAGE)) {
        i2c_write2(CS42L42_ADDRESS, CS_HP_CTL_REG, CS_HP_MUTE);
    }
    bootloader_wdt_feed();

    if (i2c_write2(TAS2505_ADDRESS, PAGE_SELECT_REG, 0x00)) {
        i2c_write2(TAS2505_ADDRESS, TAS_DAC_MUTE, TAS_MUTED);
        bootloader_wdt_feed();
        if (i2c_write2(TAS2505_ADDRESS, PAGE_SELECT_REG, 0x01)) {
            i2c_write2(TAS2505_ADDRESS, TAS_SPK_POWER, 0x00);
        }
        bootloader_wdt_feed();
        // Back to page 0 for the software reset
        if (i2c_write2(TAS2505_ADDRESS, PAGE_SELECT_REG, 0x00)) {
            i2c_write2(TAS2505_ADDRESS, TAS_SW_RESET, 0x01);
        }
    }

    common_hal_busio_i2c_deinit(&codec_i2c);
    bootloader_wdt_feed();
}

void reset_board(void) {
    quiesce_codecs();
}

// The board half of the power-off sequence
// Runs with the Function button still held, before the wake is armed.
void board_power_off_prepare(void) {
    quiesce_codecs();

    // Codecs into reset, CS42L42 first: it drives the I2S frames, so
    // stopping it stops the bus the TAS2505 is listening to.
    nrf_gpio_cfg_output(PIN_CS42_RESET);
    nrf_gpio_pin_clear(PIN_CS42_RESET);
    nrf_gpio_cfg_output(PIN_TAS_RESET);
    nrf_gpio_pin_clear(PIN_TAS_RESET);

    // eMMC I/O rail, before the oscillator, so nothing is left half-powered
    // against a clock that has stopped.
    nrf_gpio_cfg_output(PIN_EMMC_RESET);
    nrf_gpio_pin_clear(PIN_EMMC_RESET);
    nrf_gpio_cfg_output(PIN_EMMC_VCCQ_EN);
    nrf_gpio_pin_clear(PIN_EMMC_VCCQ_EN);

    // 3.072 MHz oscillator
    nrf_gpio_cfg_output(PIN_OSC_EN);
    nrf_gpio_pin_clear(PIN_OSC_EN);

    // Everything else, LEDs included.
    apply_all_pin_defaults();
}
