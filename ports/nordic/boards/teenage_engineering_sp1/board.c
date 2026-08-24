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

#include "supervisor/board.h"

#include "background.h"
#include "power_off.h"
#include "py/misc.h"
#include "wdt.h"
#include "nrfx/hal/nrf_gpio.h"

// For the eMMC's supervisor mount: board_reset_pin_defaults() leaves
// the card's reset and rail alone while it is up. The header stubs
// emmcio_is_automounted() to false when the automount is not built, so the
// call site needs no #if of its own.
#include "shared-module/emmcio/__init__.h"
#include "peripherals/nrf/pins.h"
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
static const uint32_t led_pins[] = {
    NRF_GPIO_PIN_MAP(1, 13), NRF_GPIO_PIN_MAP(0, 0),   // playback row (side)
    NRF_GPIO_PIN_MAP(1, 12), NRF_GPIO_PIN_MAP(0, 1),
    PIN_LED_HEARTBEAT, NRF_GPIO_PIN_MAP(0, 26),        // track row (front)
    NRF_GPIO_PIN_MAP(1, 15), NRF_GPIO_PIN_MAP(1, 14),
};

// Whether the boot up blink heartbeat still owns PIN_LED_HEARTBEAT.
static bool heartbeat_lit;

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
}

// Pins that must not float, re-applied after every reset_all_pins().
// None of these are marked never-reset, so Python can
// still claim them. Done so that the resting
// state between runs is a defined, safe one.
void board_reset_pin_defaults(void) {
    // 3.072 MHz oscillator enable. Held low: it draws current straight through
    // SYSTEM_OFF, so a floating pin here would drain battery
    nrf_gpio_cfg_output(PIN_OSC_EN);
    nrf_gpio_pin_clear(PIN_OSC_EN);

    // Codecs and the Bluetooth module held in reset (all active low) so that
    // nothing downstream of us starts making noise or driving a shared bus on
    // its own. P0.09/P0.10 are the NFC pins, and this board deliberately does
    // *not* build with CONFIG_NFCT_PINS_AS_GPIOS (mpconfigboard.mk), because
    // that option writes UICR.
    nrf_gpio_cfg_output(PIN_TAS_RESET);
    nrf_gpio_pin_clear(PIN_TAS_RESET);
    nrf_gpio_cfg_output(PIN_CS42_RESET);
    nrf_gpio_pin_clear(PIN_CS42_RESET);
    nrf_gpio_cfg_output(PIN_BT_RESET);
    nrf_gpio_pin_clear(PIN_BT_RESET);

    // eMMC held in reset with its VCCQ rail off. The albums live on the chip
    // itself and are unaffected; this only keeps the rail from floating.
    if (!emmcio_is_automounted()) {
        nrf_gpio_cfg_output(PIN_EMMC_RESET);
        nrf_gpio_pin_clear(PIN_EMMC_RESET);
        nrf_gpio_cfg_output(PIN_EMMC_VCCQ_EN);
        nrf_gpio_pin_clear(PIN_EMMC_VCCQ_EN);
    }

    // Rail feeding the faders and both button ladders. Off unless something is
    // actually reading them.
    nrf_gpio_cfg_output(PIN_CONTROL_RAIL);
    nrf_gpio_pin_clear(PIN_CONTROL_RAIL);

    // Both LED rows off except the first-light heartbeat status blink, if it is still
    // running. main() calls reset_all_pins() immediately after port_init(), so
    // without this the heartbeat would last microseconds and show nothing.
    for (size_t i = 0; i < MP_ARRAY_SIZE(led_pins); i++) {
        if (heartbeat_lit && led_pins[i] == PIN_LED_HEARTBEAT) {
            continue;
        }
        nrf_gpio_cfg_output(led_pins[i]);
        nrf_gpio_pin_clear(led_pins[i]);
    }

    // Function button: the only GPIO button, active low, and the only wake
    // source out of SYSTEM_OFF. Keep it readable at all times, the
    // supervisor's power-off gesture depends on it.
    nrf_gpio_cfg_input(PIN_FUNCTION_BUTTON, NRF_GPIO_PIN_PULLUP);

    // BQ24232 charge enable, active low: drive it low so a plugged-in device
    // charges. P1.00 (CHARGE_ISET) is deliberately left untouched.
    // It is the charge-current programming node (ICHG = 870 AΩ / RISET) and
    // doubles as the current monitor.
    nrf_gpio_cfg_output(PIN_CHARGE_ENABLE);
    nrf_gpio_pin_clear(PIN_CHARGE_ENABLE);
}

// Called once at start up, after the filesystem is mounted and immediately
// before the USB workflow starts. Where we end the heartbeat status blink.
void board_init(void) {
    heartbeat_lit = false;
    nrf_gpio_pin_clear(PIN_LED_HEARTBEAT);
}

void board_wdt_feed(void) {
    bootloader_wdt_feed();
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
// Dropping the reset lines and the oscillator (board_reset_pin_defaults()) is
// enough to make the board quiet and to save the battery, but it cuts both
// codecs off mid-signal: the CS42L42 loses its clock and the TAS2505's class-D
// driver loses its reset with whatever was on the output still on it.
//
// Bit-banged rather than driven through TWIM.

#define I2C_HALF_PERIOD_ITERATIONS  (100)   // ~8 us at 64 MHz; slow is fine
#define I2C_STRETCH_TIMEOUT_ITERATIONS (20000)

static void i2c_delay(void) {
    for (volatile uint32_t i = 0; i < I2C_HALF_PERIOD_ITERATIONS; i++) {
    }
}

// Open drain, input buffer connected so ACK and clock stretching are readable.
// The internal pull-up is additional safety next to the board's own; it is
// removed again by i2c_release() so nothing pulls current in SYSTEM_OFF.
static void i2c_cfg_pin(uint32_t pin) {
    nrf_gpio_cfg(pin, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT,
        NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0D1, NRF_GPIO_PIN_NOSENSE);
}

static void i2c_claim(void) {
    // Whatever owned these pins through TWIM keeps driving them while the
    // peripheral is enabled, PIN_CNF notwithstanding. TWIM0/TWIM1 share their
    // base addresses with SPIM0/SPIM1, so both are disabled here.
    //
    // On the power-off path nothing after this returns to user code, so that
    // is free. On the soft-reset path, reset_board(), the VM
    // *does* come back -- and it is still free, because a busio object cannot
    // survive that reset.  i2c_release() below puts the
    // pins back to their reset configuration, internal pull-ups and all.
    NRF_TWIM0->ENABLE = 0;
    NRF_TWIM1->ENABLE = 0;
    __DSB();
    nrf_gpio_pin_set(PIN_I2C_SDA);
    nrf_gpio_pin_set(PIN_I2C_SCL);
    i2c_cfg_pin(PIN_I2C_SDA);
    i2c_cfg_pin(PIN_I2C_SCL);
    i2c_delay();
}

static void i2c_release(void) {
    nrf_gpio_cfg_default(PIN_I2C_SDA);
    nrf_gpio_cfg_default(PIN_I2C_SCL);
}

// Raise SCL and wait for it to actually read high, so a codec stretching the
// clock is honoured. A device holding SCL down forever must not be
// able to hold the whole power-off sequence, so we give up and carry on. The
// transfer is then garbage, which the caller finds out about at the next ACK.
static void i2c_scl_high(void) {
    nrf_gpio_pin_set(PIN_I2C_SCL);
    for (uint32_t i = 0; i < I2C_STRETCH_TIMEOUT_ITERATIONS &&
         nrf_gpio_pin_read(PIN_I2C_SCL) == 0; i++) {
        __NOP();
    }
    i2c_delay();
}

static void i2c_scl_low(void) {
    nrf_gpio_pin_clear(PIN_I2C_SCL);
    i2c_delay();
}

static void i2c_start(void) {
    nrf_gpio_pin_set(PIN_I2C_SDA);
    i2c_scl_high();
    nrf_gpio_pin_clear(PIN_I2C_SDA);
    i2c_delay();
    i2c_scl_low();
}

static void i2c_stop(void) {
    nrf_gpio_pin_clear(PIN_I2C_SDA);
    i2c_delay();
    i2c_scl_high();
    nrf_gpio_pin_set(PIN_I2C_SDA);
    i2c_delay();
}

// Returns true if the slave ACKed.
static bool i2c_write_byte(uint8_t value) {
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (value & 0x80) {
            nrf_gpio_pin_set(PIN_I2C_SDA);
        } else {
            nrf_gpio_pin_clear(PIN_I2C_SDA);
        }
        value <<= 1;
        i2c_delay();
        i2c_scl_high();
        i2c_scl_low();
    }
    nrf_gpio_pin_set(PIN_I2C_SDA);      // release for the ACK bit
    i2c_delay();
    i2c_scl_high();
    bool acked = nrf_gpio_pin_read(PIN_I2C_SDA) == 0;
    i2c_scl_low();
    return acked;
}

// two-byte write
static bool i2c_write2(uint8_t address, uint8_t first, uint8_t second) {
    i2c_start();
    bool ok = i2c_write_byte(address << 1) &&
        i2c_write_byte(first) &&
        i2c_write_byte(second);
    i2c_stop();
    return ok;
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
    i2c_claim();

    if (i2c_write2(CS42L42_ADDRESS, PAGE_SELECT_REG, CS_HP_CTL_PAGE)) {
        i2c_write2(CS42L42_ADDRESS, CS_HP_CTL_REG, CS_HP_MUTE);
    }

    if (i2c_write2(TAS2505_ADDRESS, PAGE_SELECT_REG, 0x00)) {
        i2c_write2(TAS2505_ADDRESS, TAS_DAC_MUTE, TAS_MUTED);
        if (i2c_write2(TAS2505_ADDRESS, PAGE_SELECT_REG, 0x01)) {
            i2c_write2(TAS2505_ADDRESS, TAS_SPK_POWER, 0x00);
        }
        // Back to page 0 for the software reset
        if (i2c_write2(TAS2505_ADDRESS, PAGE_SELECT_REG, 0x00)) {
            i2c_write2(TAS2505_ADDRESS, TAS_SW_RESET, 0x01);
        }
    }

    i2c_release();
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
    board_reset_pin_defaults();
}
