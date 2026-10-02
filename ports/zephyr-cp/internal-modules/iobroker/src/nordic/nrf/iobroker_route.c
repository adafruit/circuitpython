// iobroker: nRF runtime pin routing.
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

// <vendor>/<soc> = nordic/nrf: one implementation covers every nRF SoC,
// because their pin control encoding is a simple bit-packed value (see
// zephyr/dt-bindings/pinctrl/nrf-pinctrl.h and soc/nordic/common/pinctrl_soc.h)
// that can be computed at runtime, and any peripheral function can be routed
// to (almost) any pin via PSEL.
//
// Compiled whenever CONFIG_PINCTRL_NRF is on. The allocate/release
// definitions here only exist with the full feature set
// (CONFIG_PINCTRL_DYNAMIC and CONFIG_DEVICE_DEINIT_SUPPORT); otherwise the
// stubs in the core report -ENOSYS.

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include <zephyr/dt-bindings/pinctrl/nrf-pinctrl.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "iobroker_internal.h"

LOG_MODULE_DECLARE(iobroker, CONFIG_LOG_DEFAULT_LEVEL);

#if IOBROKER_ROUTING

// Bit positions/fields replicated from nrf-pinctrl.h so that entries can be
// encoded at runtime instead of by the DT macros.
#define NRF_PSEL_FUN(fun)       (((uint32_t)(fun) & NRF_FUN_MSK) << NRF_FUN_POS)
#define NRF_PSEL_PIN(pin)       (((uint32_t)(pin) & NRF_PIN_MSK) << NRF_PIN_POS)
#define NRF_PSEL_DISCONNECT(fun) \
    ((uint32_t)NRF_PIN_DISCONNECTED | NRF_PSEL_FUN(fun))
#define NRF_PSEL_PULL_UP        ((uint32_t)NRF_PULL_UP << NRF_PULL_POS)
#define NRF_PSEL_LP             ((uint32_t)NRF_LP_ENABLE << NRF_LP_POS)
#define NRF_PSEL_CLOCKPIN       BIT(NRF_CLOCKPIN_ENABLE_POS)
// Pin + function bits, used to compare a request against a devicetree state.
#define NRF_PSEL_PINFUN_MASK    \
    (((uint32_t)NRF_PIN_MSK << NRF_PIN_POS) | ((uint32_t)NRF_FUN_MSK << NRF_FUN_POS))

// Log decode helpers: turn a fun code and a SoC pad into readable names, so
// that the logs say which peripheral function goes to which GPIO.
static const char *nrf_fun_name(uint32_t fun) {
    static const char *const names[] = {
        "UART_TX", "UART_RX", "UART_RTS", "UART_CTS",
        "SPIM_SCK", "SPIM_MOSI", "SPIM_MISO",
        "SPIS_SCK", "SPIS_MOSI", "SPIS_MISO", "SPIS_CSN",
        "TWIM_SCL", "TWIM_SDA",
        "I2S_SCK_M", "I2S_SCK_S", "I2S_LRCK_M", "I2S_LRCK_S", "I2S_SDIN",
        "I2S_SDOUT", "I2S_MCK",
        "PDM_CLK", "PDM_DIN",
        "PWM_OUT0", "PWM_OUT1", "PWM_OUT2", "PWM_OUT3",
    };
    if (fun < ARRAY_SIZE(names)) {
        return names[fun];
    }
    return "(other)";
}

// Render a SoC pad as a GPIO name ("P0.09"), or "disconnected" for
// IOBROKER_NO_PIN. Writes into buf; returns buf for chaining.
static const char *nrf_pad_name(uint32_t soc_pad, char *buf, size_t size) {
    if (soc_pad == IOBROKER_NO_PIN) {
        return "disconnected";
    }
    snprintf(buf, size, "P%u.%02u", (unsigned)(soc_pad / 32U),
        (unsigned)(soc_pad % 32U));
    return buf;
}

// Decode one pinctrl entry into flags text, e.g. "pull-up clockpin". buf
// receives an empty string when neither flag is set.
static const char *nrf_pin_flags(uint32_t psel, char *buf, size_t size) {
    buf[0] = '\0';
    size_t used = 0;
    uint32_t pull = NRF_GET_PULL(psel);
    if (pull == NRF_PULL_UP) {
        used += (size_t)snprintf(buf + used, size - used, "pull-up");
    } else if (pull == NRF_PULL_DOWN) {
        used += (size_t)snprintf(buf + used, size - used, "pull-down");
    }
    if (NRF_GET_CLOCKPIN_ENABLE(psel)) {
        if (used > 0) {
            used += (size_t)snprintf(buf + used, size - used, " ");
        }
        snprintf(buf + used, size - used, "clockpin");
    }
    return buf;
}

// Encode one nRF pin control entry. soc_pad may be IOBROKER_NO_PIN to
// leave the signal disconnected. pull_up enables the internal pull resistor;
// the caller picks it per bus signal (I2C SDA/SCL and UART RX idle high).
static pinctrl_soc_pin_t nrf_psel_encode(uint32_t fun, uint16_t soc_pad,
    bool pull_up) {
    uint32_t psel;

    if (soc_pad != IOBROKER_NO_PIN) {
        psel = NRF_PSEL_PIN(soc_pad) | NRF_PSEL_FUN(fun);
        // On nRF54 the GPIO pin clock must be enabled for signals that drive
        // the pad. The pinctrl driver ignores this bit where unsupported
        // (nRF52/nRF53).
        switch (fun) {
            case NRF_FUN_TWIM_SDA:
            case NRF_FUN_TWIM_SCL:
            case NRF_FUN_SPIM_SCK:
            case NRF_FUN_SPIM_MOSI:
            case NRF_FUN_UART_TX:
                psel |= NRF_PSEL_CLOCKPIN;
                break;
            default:
                break;
        }
    } else {
        psel = NRF_PSEL_DISCONNECT(fun);
    }

    if (pull_up) {
        psel |= NRF_PSEL_PULL_UP;
    }

    return psel;
}

// Whether an instance can drive a pad, given as its global GPIO number
// (port index * 32 + pin). nRF52 and nRF53 are a full crossbar.
// On nRF54L, peripherals and GPIO controllers are grouped in power domains
// and a peripheral can only drive pads of its own domain; the domain is
// encoded in the register address (each occupies its own 256 KiB window:
// 0x4xxxx for the "00" instances and P2, 0xcxxxx-0xfxxxx for the "2x"
// instances and P1/P3, 0x10xxxx for the "30" instances and P0).
static bool nrf_instance_reaches_pad(uint32_t reg_addr, uint16_t gpio_pad) {
    #if defined(CONFIG_SOC_SERIES_NRF54L)
    uint8_t port = (uint8_t)(gpio_pad / 32U);
    for (size_t i = 0; i < iobroker_gpio_port_count; i++) {
        if (iobroker_gpio_port_indexes[i] == port) {
            return (reg_addr >> 18) == (iobroker_gpio_port_addrs[i] >> 18);
        }
    }
    return false;
    #else
    (void)reg_addr;
    (void)gpio_pad;
    return true;
    #endif
}

// Whether an instance can drive every connected pad of a set of encoded
// entries (disconnected entries pass).
static bool nrf_instance_reaches_all(uint32_t reg_addr,
    const pinctrl_soc_pin_t *pins, uint8_t pin_count) {
    for (uint8_t i = 0; i < pin_count; i++) {
        uint32_t pad = NRF_GET_PIN(pins[i]);
        if (pad != NRF_PIN_DISCONNECTED && !nrf_instance_reaches_pad(reg_addr, (uint16_t)pad)) {
            return false;
        }
    }
    return true;
}

// Check whether a set of requested entries matches the devicetree default
// state of an instance (pin + function only; configuration bits ignored).
static bool nrf_psels_match(const pinctrl_soc_pin_t *psels, uint8_t count,
    const pinctrl_soc_pin_t *requested, uint8_t requested_count) {
    uint32_t mask = NRF_PSEL_PINFUN_MASK;

    for (uint8_t i = 0; i < requested_count; i++) {
        bool found = false;
        for (uint8_t j = 0; j < count; j++) {
            if ((psels[j] & mask) == (requested[i] & mask)) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    // The DT state must not use extra pins either.
    for (uint8_t j = 0; j < count; j++) {
        bool found = false;
        for (uint8_t i = 0; i < requested_count; i++) {
            if ((psels[j] & mask) == (requested[i] & mask)) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

// Re-point an instance at the requested pins: ensure the device is
// de-initialized (initialization is the caller's job), then swap its pinctrl
// states for entries built at runtime. The caller initializes the device
// afterwards, which applies the new "default" state.
static int iobroker_route(const iobroker_instance_t *inst, iobroker_state_t *state,
    const pinctrl_soc_pin_t *pins, uint8_t pin_count) {
    if (pin_count > IOBROKER_MAX_PINS) {
        return -EINVAL;
    }

    LOG_INF("routing %s: de-initializing then swapping in %u runtime psels",
        inst->dev->name, (unsigned)pin_count);
    int ret = device_deinit(inst->dev);
    if (ret < 0 && ret != -ENOSYS && ret != -EPERM) {
        LOG_WRN("routing %s: device_deinit failed: %d", inst->dev->name, ret);
        return ret;
    }

    // The Zephyr pinctrl core requires the same set of state ids to be
    // provided, so mirror the ids the device currently has.
    uint8_t state_cnt = MIN(inst->pcfg->state_cnt, ARRAY_SIZE(state->states));
    if (state_cnt == 0) {
        return -ENOENT;
    }

    for (uint8_t i = 0; i < pin_count; i++) {
        state->default_pins[i] = pins[i];
        // Sleep state: same pins, low power (input buffer disconnected).
        state->sleep_pins[i] = pins[i] | NRF_PSEL_LP;
    }
    for (uint8_t s = 0; s < state_cnt; s++) {
        uint8_t id = inst->pcfg->states[s].id;
        state->states[s].id = id;
        if (id == PINCTRL_STATE_SLEEP) {
            state->states[s].pins = state->sleep_pins;
        } else {
            state->states[s].pins = state->default_pins;
        }
        state->states[s].pin_cnt = pin_count;
    }

    ret = pinctrl_update_states(inst->pcfg, state->states, state_cnt);
    if (ret < 0) {
        LOG_WRN("routing %s: pinctrl_update_states failed: %d",
            inst->dev->name, ret);
    } else {
        for (uint8_t i = 0; i < pin_count; i++) {
            uint32_t psel = pins[i];
            char pad[12];
            char flags[24];
            LOG_INF("routing %s: psel[%u] = 0x%08x -> %s %s %s",
                inst->dev->name, (unsigned)i, (unsigned)psel,
                nrf_fun_name(NRF_GET_FUN(psel)),
                nrf_pad_name(NRF_GET_PIN(psel), pad, sizeof(pad)),
                nrf_pin_flags(psel, flags, sizeof(flags)));
        }
    }
    return ret;
}

// Allocate an instance from a pool. Instances that the board enabled with
// disconnected pins are routed dynamically to the requested pins. Instances
// with fixed devicetree pins are only used when the request matches their
// existing state exactly. The requested pins are recorded in the instance's
// state so that iobroker_pin_in_use() can report them.
static int iobroker_allocate(const char *kind, const iobroker_instance_t *buses,
    size_t count, iobroker_state_t *states, const package_pin_t *requested,
    const pinctrl_soc_pin_t *pins, uint8_t pin_count, const struct device **dev_out) {
    bool reachable = false;
    for (size_t i = 0; i < count; i++) {
        iobroker_state_t *state = &states[i];
        if (!nrf_instance_reaches_all(buses[i].reg_addr, pins, pin_count)) {
            continue;
        }
        reachable = true;
        if (state->in_use) {
            continue;
        }

        if (buses[i].dt_psels == NULL) {
            int ret = iobroker_route(&buses[i], state, pins, pin_count);
            if (ret < 0) {
                continue;
            }
            state->in_use = true;
            state->routed = true;
            state->pin_count = pin_count;
            for (uint8_t j = 0; j < pin_count; j++) {
                state->pins[j] = requested[j];
            }
            *dev_out = buses[i].dev;
            LOG_INF("%s: allocated %s, routed to %u package pins",
                kind, buses[i].dev->name, (unsigned)pin_count);
            return 0;
        }

        if (nrf_psels_match(buses[i].dt_psels, buses[i].dt_psel_count, pins, pin_count)) {
            // Already routed to these pins by the devicetree; just claim it.
            state->in_use = true;
            state->routed = false;
            state->pin_count = pin_count;
            for (uint8_t j = 0; j < pin_count; j++) {
                state->pins[j] = requested[j];
            }
            *dev_out = buses[i].dev;
            LOG_INF("%s: allocated %s, devicetree pins match request",
                kind, buses[i].dev->name);
            return 0;
        }
    }
    if (!reachable) {
        LOG_WRN("%s: no instance can be routed to the requested pins", kind);
        return -EINVAL;
    }
    LOG_WRN("%s: no free instance for the requested pins", kind);
    return -ENODEV;
}

// Returns -EBUSY when any requested pin is already claimed by an allocated
// instance, so that a pin is only ever used by one allocate() call at a time.
static int iobroker_check_request(const char *kind, const package_pin_t *pins,
    size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (iobroker_pin_in_use(pins[i])) {
            LOG_WRN("%s: package pin %u already in use", kind,
                (unsigned)pins[i]);
            return -EBUSY;
        }
    }
    return 0;
}

static bool iobroker_state_find(const struct device *dev, iobroker_state_t **state_out) {
    for (size_t i = 0; i < iobroker_i2c_bus_count; i++) {
        if (iobroker_i2c_buses[i].dev == dev) {
            *state_out = &iobroker_i2c_bus_states[i];
            return true;
        }
    }
    for (size_t i = 0; i < iobroker_spi_bus_count; i++) {
        if (iobroker_spi_buses[i].dev == dev) {
            *state_out = &iobroker_spi_bus_states[i];
            return true;
        }
    }
    for (size_t i = 0; i < iobroker_uart_bus_count; i++) {
        if (iobroker_uart_buses[i].dev == dev) {
            *state_out = &iobroker_uart_bus_states[i];
            return true;
        }
    }
    for (size_t i = 0; i < iobroker_pwm_bus_count; i++) {
        if (iobroker_pwm_buses[i].dev == dev) {
            *state_out = &iobroker_pwm_bus_states[i];
            return true;
        }
    }
    return false;
}

bool iobroker_release(const struct device *dev) {
    iobroker_state_t *state = NULL;
    if (!iobroker_state_find(dev, &state)) {
        LOG_WRN("release: %s is not an iobroker-managed instance",
            dev == NULL ? "(null)" : dev->name);
        return false;
    }
    bool routed = state->routed;
    // De-init so that the device ends up de-initialized (like deferred-init
    // devices are after boot); the caller initializes it again when it
    // allocates the instance next.
    // TODO: pwm_nrfx has no deinit hook (-ENOTSUP), which matters once pwmio
    // initializes PWM instances.
    (void)device_deinit(dev);
    state->in_use = false;
    state->routed = false;
    state->pin_count = 0;
    LOG_INF("released %s (was dynamically routed: %u)", dev->name,
        (unsigned)routed);
    return routed;
}

int iobroker_i2c_allocate(package_pin_t sda, package_pin_t scl,
    const struct device **dev_out) {
    LOG_INF("i2c allocate: sda=%u scl=%u", (unsigned)sda, (unsigned)scl);
    const package_pin_t requested[] = { sda, scl };
    int ret = iobroker_check_request("i2c", requested, 2);
    if (ret < 0) {
        return ret;
    }
    uint16_t sda_pad;
    uint16_t scl_pad;
    uint16_t sda_gpio;
    uint16_t scl_gpio;
    if (iobroker_package_pin_soc_pad(sda, &sda_pad) < 0 ||
        iobroker_package_pin_soc_pad(scl, &scl_pad) < 0 ||
        iobroker_pad_gpio(sda_pad, &sda_gpio) < 0 ||
        iobroker_pad_gpio(scl_pad, &scl_gpio) < 0) {
        LOG_WRN("i2c allocate: package pin %u or %u is unknown or has no GPIO",
            (unsigned)sda, (unsigned)scl);
        return -EINVAL;
    }
    char sda_name[12];
    char scl_name[12];
    LOG_INF("i2c allocate: SDA package pin %u -> %s, SCL package pin %u -> %s",
        (unsigned)sda, nrf_pad_name(sda_gpio, sda_name, sizeof(sda_name)),
        (unsigned)scl, nrf_pad_name(scl_gpio, scl_name, sizeof(scl_name)));
    pinctrl_soc_pin_t pins[2];
    // Open-drain bus: both lines idle high via the internal pull-up.
    pins[0] = nrf_psel_encode(NRF_FUN_TWIM_SDA, sda_gpio, true);
    pins[1] = nrf_psel_encode(NRF_FUN_TWIM_SCL, scl_gpio, true);
    return iobroker_allocate("i2c", iobroker_i2c_buses, iobroker_i2c_bus_count,
        iobroker_i2c_bus_states, requested, pins, 2, dev_out);
}

int iobroker_spi_allocate(package_pin_t clock, package_pin_t mosi,
    package_pin_t miso, const struct device **dev_out) {
    LOG_INF("spi allocate: clock=%u mosi=%u miso=%u", (unsigned)clock,
        (unsigned)mosi, (unsigned)miso);
    const package_pin_t requested[] = { clock, mosi, miso };
    int ret = iobroker_check_request("spi", requested, 3);
    if (ret < 0) {
        return ret;
    }
    uint16_t clock_pad;
    uint16_t mosi_pad;
    uint16_t miso_pad;
    uint16_t clock_gpio;
    uint16_t mosi_gpio;
    uint16_t miso_gpio;
    if (iobroker_package_pin_soc_pad(clock, &clock_pad) < 0 ||
        iobroker_package_pin_soc_pad(mosi, &mosi_pad) < 0 ||
        iobroker_package_pin_soc_pad(miso, &miso_pad) < 0 ||
        iobroker_pad_gpio(clock_pad, &clock_gpio) < 0 ||
        iobroker_pad_gpio(mosi_pad, &mosi_gpio) < 0 ||
        iobroker_pad_gpio(miso_pad, &miso_gpio) < 0) {
        LOG_WRN("spi allocate: a package pin (%u/%u/%u) is unknown or has no GPIO",
            (unsigned)clock, (unsigned)mosi, (unsigned)miso);
        return -EINVAL;
    }
    char clock_name[12];
    char mosi_name[12];
    char miso_name[12];
    LOG_INF("spi allocate: SCK package pin %u -> %s, MOSI %u -> %s, MISO %u -> %s",
        (unsigned)clock, nrf_pad_name(clock_gpio, clock_name, sizeof(clock_name)),
        (unsigned)mosi, nrf_pad_name(mosi_gpio, mosi_name, sizeof(mosi_name)),
        (unsigned)miso, nrf_pad_name(miso_gpio, miso_name, sizeof(miso_name)));
    pinctrl_soc_pin_t pins[3];
    // All signals are push-pull outputs (MISO from the peripheral's view).
    pins[0] = nrf_psel_encode(NRF_FUN_SPIM_SCK, clock_gpio, false);
    pins[1] = nrf_psel_encode(NRF_FUN_SPIM_MOSI, mosi_gpio, false);
    pins[2] = nrf_psel_encode(NRF_FUN_SPIM_MISO, miso_gpio, false);
    return iobroker_allocate("spi", iobroker_spi_buses, iobroker_spi_bus_count,
        iobroker_spi_bus_states, requested, pins, 3, dev_out);
}

int iobroker_uart_allocate(package_pin_t tx, package_pin_t rx,
    package_pin_t rts, package_pin_t cts, const struct device **dev_out) {
    LOG_INF("uart allocate: tx=%u rx=%u rts=%u cts=%u", (unsigned)tx,
        (unsigned)rx, (unsigned)rts, (unsigned)cts);
    const package_pin_t requested[] = { tx, rx, rts, cts };
    int ret = iobroker_check_request("uart", requested, 4);
    if (ret < 0) {
        return ret;
    }
    uint16_t tx_pad;
    uint16_t rx_pad;
    uint16_t rts_pad;
    uint16_t cts_pad;
    uint16_t tx_gpio;
    uint16_t rx_gpio;
    uint16_t rts_gpio;
    uint16_t cts_gpio;
    if (iobroker_package_pin_soc_pad(tx, &tx_pad) < 0 ||
        iobroker_package_pin_soc_pad(rx, &rx_pad) < 0 ||
        iobroker_package_pin_soc_pad(rts, &rts_pad) < 0 ||
        iobroker_package_pin_soc_pad(cts, &cts_pad) < 0 ||
        iobroker_pad_gpio(tx_pad, &tx_gpio) < 0 ||
        iobroker_pad_gpio(rx_pad, &rx_gpio) < 0 ||
        iobroker_pad_gpio(rts_pad, &rts_gpio) < 0 ||
        iobroker_pad_gpio(cts_pad, &cts_gpio) < 0) {
        LOG_WRN("uart allocate: a package pin (%u/%u/%u/%u) is unknown or has no GPIO",
            (unsigned)tx, (unsigned)rx, (unsigned)rts, (unsigned)cts);
        return -EINVAL;
    }
    char tx_name[12];
    char rx_name[12];
    char rts_name[12];
    char cts_name[12];
    LOG_INF("uart allocate: TX package pin %u -> %s, RX %u -> %s, RTS %u -> %s, CTS %u -> %s",
        (unsigned)tx, nrf_pad_name(tx_gpio, tx_name, sizeof(tx_name)),
        (unsigned)rx, nrf_pad_name(rx_gpio, rx_name, sizeof(rx_name)),
        (unsigned)rts, nrf_pad_name(rts_gpio, rts_name, sizeof(rts_name)),
        (unsigned)cts, nrf_pad_name(cts_gpio, cts_name, sizeof(cts_name)));
    pinctrl_soc_pin_t pins[4];
    // RX floats until the peer drives it, so pull it up internally.
    pins[0] = nrf_psel_encode(NRF_FUN_UART_TX, tx_gpio, false);
    pins[1] = nrf_psel_encode(NRF_FUN_UART_RX, rx_gpio, true);
    pins[2] = nrf_psel_encode(NRF_FUN_UART_RTS, rts_gpio, false);
    pins[3] = nrf_psel_encode(NRF_FUN_UART_CTS, cts_gpio, false);
    return iobroker_allocate("uart", iobroker_uart_buses, iobroker_uart_bus_count,
        iobroker_uart_bus_states, requested, pins, 4, dev_out);
}

int iobroker_pwm_allocate(package_pin_t pin, const struct device **dev_out) {
    LOG_INF("pwm allocate: pin=%u", (unsigned)pin);
    // The pin goes to OUT0; OUT1..OUT3 stay disconnected, and are recorded as
    // disconnected so the instance's pin list is complete.
    const package_pin_t requested[] = { pin, IOBROKER_NO_PIN, IOBROKER_NO_PIN, IOBROKER_NO_PIN };
    int ret = iobroker_check_request("pwm", requested, 1);
    if (ret < 0) {
        return ret;
    }
    uint16_t pad;
    if (pin == IOBROKER_NO_PIN || iobroker_package_pin_gpio_pad(pin, &pad) < 0) {
        LOG_WRN("pwm allocate: package pin %u is unknown or has no GPIO", (unsigned)pin);
        return -EINVAL;
    }
    char name[12];
    LOG_INF("pwm allocate: OUT0 package pin %u -> %s", (unsigned)pin,
        nrf_pad_name(pad, name, sizeof(name)));
    pinctrl_soc_pin_t pins[4];
    // Push-pull output, no pull.
    pins[0] = nrf_psel_encode(NRF_FUN_PWM_OUT0, pad, false);
    pins[1] = nrf_psel_encode(NRF_FUN_PWM_OUT1, IOBROKER_NO_PIN, false);
    pins[2] = nrf_psel_encode(NRF_FUN_PWM_OUT2, IOBROKER_NO_PIN, false);
    pins[3] = nrf_psel_encode(NRF_FUN_PWM_OUT3, IOBROKER_NO_PIN, false);
    return iobroker_allocate("pwm", iobroker_pwm_buses, iobroker_pwm_bus_count,
        iobroker_pwm_bus_states, requested, pins, 4, dev_out);
}

#endif // IOBROKER_ROUTING
