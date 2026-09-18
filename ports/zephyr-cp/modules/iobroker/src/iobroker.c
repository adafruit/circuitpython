// iobroker: dynamic peripheral allocation and runtime pin routing.
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

// SoC-agnostic core: package pin resolution, GPIO controller lookup and the
// claim registry. The bus allocate/release functions are SoC-specific and
// live in src/<vendor>/<soc>/; the -ENOSYS stubs below stand in for SoCs
// with no implementation.

#include <errno.h>
#include <stddef.h>

#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "iobroker_internal.h"

// One log instance for the whole module; iobroker_route.c declares it.
// Compiles away when CONFIG_LOG is off.
LOG_MODULE_REGISTER(iobroker, CONFIG_LOG_DEFAULT_LEVEL);

int iobroker_gpio_split(uint16_t number, const struct device **port_out,
    gpio_pin_t *pin_out) {
    for (size_t i = 0; i < iobroker_gpio_port_count; i++) {
        uint32_t base = (uint32_t)iobroker_gpio_port_indexes[i] * 32U;
        if (number >= base && number < base + 32U) {
            *port_out = iobroker_gpio_port_devices[i];
            *pin_out = (gpio_pin_t)(number - base);
            return 0;
        }
    }
    return -EINVAL;
}

int iobroker_gpio_package_pin(uint8_t port, gpio_pin_t pin,
    package_pin_t *package_pin_out) {
    uint16_t soc_pad = (uint16_t)((uint32_t)port * 32U + pin);
    for (size_t i = 0; i < iobroker_package_pin_count; i++) {
        if (iobroker_package_pins[i].soc_pad == soc_pad) {
            *package_pin_out = iobroker_package_pins[i].package_pin;
            return 0;
        }
    }
    return -EINVAL;
}

int iobroker_package_pin_soc_pad(package_pin_t pin, uint16_t *soc_pad_out) {
    if (pin == IOBROKER_NO_PIN) {
        *soc_pad_out = IOBROKER_NO_PIN;
        return 0;
    }
    for (size_t i = 0; i < iobroker_package_pin_count; i++) {
        if (iobroker_package_pins[i].package_pin == pin) {
            *soc_pad_out = iobroker_package_pins[i].soc_pad;
            return 0;
        }
    }
    return -EINVAL;
}

#if !IOBROKER_ROUTING

// SoCs without runtime routing: the bus allocate/release API still exists so
// busio can call it, but every allocate reports -ENOSYS. The implementations
// for routing SoCs live in src/<vendor>/<soc>/.
int iobroker_i2c_allocate(package_pin_t sda, package_pin_t scl,
    const struct device **dev_out) {
    (void)sda;
    (void)scl;
    (void)dev_out;
    return -ENOSYS;
}

int iobroker_spi_allocate(package_pin_t clock, package_pin_t mosi,
    package_pin_t miso, const struct device **dev_out) {
    (void)clock;
    (void)mosi;
    (void)miso;
    (void)dev_out;
    return -ENOSYS;
}

int iobroker_uart_allocate(package_pin_t tx, package_pin_t rx,
    package_pin_t rts, package_pin_t cts, const struct device **dev_out) {
    (void)tx;
    (void)rx;
    (void)rts;
    (void)cts;
    (void)dev_out;
    return -ENOSYS;
}

bool iobroker_release(const struct device *dev) {
    (void)dev;
    LOG_DBG("release: no routing support on this SoC, nothing to release");
    return false;
}

#endif // !IOBROKER_ROUTING

// Pins currently claimed for plain GPIO use. The module leaves the pad alone
// on allocate (the caller configures it) but returns it to a quiescent state
// on release. Claims store the GPIO controller device and pin number that the
// allocate call resolved and returned.
typedef struct {
    const struct device *port;
    gpio_pin_t number;
    bool in_use;
} gpio_claim_t;

static gpio_claim_t gpio_claims[CONFIG_IOBROKER_GPIO_MAX_PINS];

// Returns true when the package pin is claimed by a currently allocated
// instance or a GPIO allocation. Disconnected signals (IOBROKER_NO_PIN)
// and package pins with no entry in the map claim nothing. The bus loops
// below exist whenever the nRF pinctrl driver is built (the board emits the
// instance tables); without routing the tables are never marked in use.
bool iobroker_pin_in_use(package_pin_t pin) {
    if (pin == IOBROKER_NO_PIN) {
        return false;
    }
    #if defined(CONFIG_PINCTRL_NRF)
    for (size_t i = 0; i < iobroker_i2c_bus_count; i++) {
        if (!iobroker_i2c_bus_states[i].in_use) {
            continue;
        }
        for (uint8_t j = 0; j < iobroker_i2c_bus_states[i].pin_count; j++) {
            if (iobroker_i2c_bus_states[i].pins[j] == pin) {
                return true;
            }
        }
    }
    for (size_t i = 0; i < iobroker_spi_bus_count; i++) {
        if (!iobroker_spi_bus_states[i].in_use) {
            continue;
        }
        for (uint8_t j = 0; j < iobroker_spi_bus_states[i].pin_count; j++) {
            if (iobroker_spi_bus_states[i].pins[j] == pin) {
                return true;
            }
        }
    }
    for (size_t i = 0; i < iobroker_uart_bus_count; i++) {
        if (!iobroker_uart_bus_states[i].in_use) {
            continue;
        }
        for (uint8_t j = 0; j < iobroker_uart_bus_states[i].pin_count; j++) {
            if (iobroker_uart_bus_states[i].pins[j] == pin) {
                return true;
            }
        }
    }
    #endif // CONFIG_PINCTRL_NRF
    uint16_t soc_pad;
    if (iobroker_package_pin_soc_pad(pin, &soc_pad) < 0) {
        return false;
    }
    #if defined(CONFIG_PINCTRL_NRF)
    // Pads owned by fixed peripherals (console UART, flash instance, ...) are
    // always in use: their pinctrl state drives them from boot.
    for (size_t i = 0; i < iobroker_reserved_pads_count; i++) {
        if (iobroker_reserved_pads[i] == soc_pad) {
            return true;
        }
    }
    #endif
    const struct device *port;
    gpio_pin_t number;
    if (iobroker_gpio_split(soc_pad, &port, &number) < 0) {
        return false;
    }
    for (size_t i = 0; i < ARRAY_SIZE(gpio_claims); i++) {
        if (gpio_claims[i].in_use && gpio_claims[i].port == port &&
            gpio_claims[i].number == number) {
            return true;
        }
    }
    return false;
}

// Claim a package pin for GPIO use. The caller configures the pad itself
// while the claim is held; release returns it to a quiescent state. The pin
// is resolved through the package pin map, and the GPIO controller device and
// pin number within it are returned.
int iobroker_gpio_allocate(package_pin_t pin,
    const struct device **port_out, gpio_pin_t *pin_out) {
    if (pin == IOBROKER_NO_PIN) {
        LOG_WRN("gpio allocate: pin is disconnected");
        return -EINVAL;
    }
    if (iobroker_pin_in_use(pin)) {
        LOG_WRN("gpio allocate: package pin %u already claimed", (unsigned)pin);
        return -EBUSY;
    }
    uint16_t soc_pad;
    int ret = iobroker_package_pin_soc_pad(pin, &soc_pad);
    if (ret < 0) {
        LOG_WRN("gpio allocate: package pin %u not in package map", (unsigned)pin);
        return ret;
    }
    ret = iobroker_gpio_split(soc_pad, port_out, pin_out);
    if (ret < 0) {
        LOG_WRN("gpio allocate: no GPIO controller for pad %u", (unsigned)soc_pad);
        return ret;
    }
    for (size_t i = 0; i < ARRAY_SIZE(gpio_claims); i++) {
        if (gpio_claims[i].in_use) {
            continue;
        }
        gpio_claims[i].port = *port_out;
        gpio_claims[i].number = *pin_out;
        gpio_claims[i].in_use = true;
        LOG_DBG("gpio allocate: package pin %u -> %s pin %u",
            (unsigned)pin, (*port_out)->name, (unsigned)*pin_out);
        return 0;
    }
    LOG_WRN("gpio allocate: claim registry full (%u pins)",
        (unsigned)CONFIG_IOBROKER_GPIO_MAX_PINS);
    return -ENOMEM;
}

// Return a pad to a quiescent state: disconnected from any peripheral
// routing, input buffer and driver off, no pulls.
static void gpio_deconfigure(const struct device *port, gpio_pin_t number) {
    if (gpio_pin_configure(port, number, GPIO_DISCONNECTED) == -ENOTSUP) {
        // SoCs without GPIO_DISCONNECTED support settle for a plain input.
        gpio_pin_configure(port, number, GPIO_INPUT);
    }
}

// Release a GPIO claim and return the pad to a quiescent state. Pass the
// device and pin number that the allocate call returned. Returns true when a
// claim was held.
bool iobroker_gpio_release(const struct device *port, gpio_pin_t number) {
    for (size_t i = 0; i < ARRAY_SIZE(gpio_claims); i++) {
        if (gpio_claims[i].in_use && gpio_claims[i].port == port &&
            gpio_claims[i].number == number) {
            gpio_claims[i].in_use = false;
            gpio_deconfigure(port, number);
            LOG_DBG("gpio release: %s pin %u back to quiescent state",
                port->name, (unsigned)number);
            return true;
        }
    }
    return false;
}
