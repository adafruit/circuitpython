// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc
//
// SPDX-License-Identifier: MIT

#define CIRCUITPY_DIGITALIO_HAVE_INVALID_DRIVE_MODE 1
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "max32_port.h"
#include "gpio_reva.h"
#include "mxc_errors.h"

extern mxc_gpio_regs_t *gpio_ports[NUM_GPIO_PORTS];

void common_hal_digitalio_digitalinout_never_reset(
    digitalio_digitalinout_obj_t *self) {
    common_hal_never_reset_pin(self->pin);
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == NULL;
}

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
    digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {

    common_hal_mcu_pin_claim(pin);
    self->pin = pin;

    mxc_gpio_cfg_t new_gpio_cfg = {
        .port = gpio_ports[self->pin->port],
        .mask = (self->pin->mask),
        .vssel = self->pin->level,
        .func = MXC_GPIO_FUNC_IN,
        .drvstr = MXC_GPIO_DRVSTR_0,
        .pad = MXC_GPIO_PAD_NONE,
    };
    MXC_GPIO_Config(&new_gpio_cfg);

    return DIGITALINOUT_OK;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }

    reset_pin_number(self->pin->port, self->pin->mask);
    self->pin = NULL;
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_input(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    mxc_gpio_regs_t *port = gpio_ports[self->pin->port];
    uint32_t mask = self->pin->mask;
    int err = E_NO_ERROR;

    if (self->pin->port == 4) {
        // Set GPIO(s) to input mode
        MXC_MCR->gpio4_ctrl &= ~GPIO4_OUTEN_MASK(mask);
        MXC_MCR->outen &= ~GPIO4_AFEN_MASK(mask);
    } else {
        err = MXC_GPIO_RevA_SetAF((mxc_gpio_reva_regs_t *)port, MXC_GPIO_FUNC_IN, mask);
    }
    if (err != E_NO_ERROR) {
        return DIGITALINOUT_PIN_BUSY;
    }
    return common_hal_digitalio_digitalinout_set_pull(self, pull);
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(
    digitalio_digitalinout_obj_t *self, bool value,
    digitalio_drive_mode_t drive_mode) {
    mxc_gpio_regs_t *port = gpio_ports[self->pin->port];
    uint32_t mask = self->pin->mask;

    if (self->pin->port == 4) {
        // Set GPIO(s) to output mode
        MXC_MCR->gpio4_ctrl |= GPIO4_OUTEN_MASK(mask);
        MXC_MCR->outen &= ~GPIO4_AFEN_MASK(mask);
    } else {
        MXC_GPIO_RevA_SetAF((mxc_gpio_reva_regs_t *)port, MXC_GPIO_FUNC_OUT, mask);
    }
    common_hal_digitalio_digitalinout_set_value(self, value);

    // todo (low): MSDK Hardware does not support open-drain configuration except
    // todo (low): when directly managed by a peripheral such as I2C.
    // todo (low): find a way to signal this to any upstream code
    if (drive_mode != DRIVE_MODE_PUSH_PULL) {
        return DIGITALINOUT_INVALID_DRIVE_MODE;
    }
    return DIGITALINOUT_OK;
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
    digitalio_digitalinout_obj_t *self) {

    mxc_gpio_regs_t *port = gpio_ports[self->pin->port];
    uint32_t mask = self->pin->mask;

    if (self->pin->port < 4) {
        // Check that I/O mode is enabled and we don't have in AND out on at the same time
        MP_STATIC_ASSERT(!((port->en0 & mask) && (port->inen & mask) && (port->outen & mask)));

        if ((port->en0 & mask) && (port->outen & mask)) {
            return DIRECTION_OUTPUT;
        } else if ((port->en0 & mask) && (port->inen & mask)) {
            return DIRECTION_INPUT;
            // do not try to drive a pin which has an odd configuration here
        } else {
            return DIRECTION_INPUT;
        }
    } else {
        if (MXC_MCR->gpio4_ctrl & GPIO4_OUTEN_MASK(mask)) {
            return DIRECTION_OUTPUT;
        } else {
            return DIRECTION_INPUT;
        }
    }
}

void common_hal_digitalio_digitalinout_set_value(
    digitalio_digitalinout_obj_t *self, bool value) {
    digitalio_direction_t dir =
        common_hal_digitalio_digitalinout_get_direction(self);

    mxc_gpio_regs_t *port = gpio_ports[self->pin->port];
    uint32_t mask = self->pin->mask;

    if (dir == DIRECTION_OUTPUT) {
        if (value == true) {
            MXC_GPIO_OutSet(port, mask);
        } else {
            MXC_GPIO_OutClr(port, mask);
        }
    }
}

bool common_hal_digitalio_digitalinout_get_value(digitalio_digitalinout_obj_t *self) {
    digitalio_direction_t dir =
        common_hal_digitalio_digitalinout_get_direction(self);

    mxc_gpio_regs_t *port = gpio_ports[self->pin->port];
    uint32_t mask = self->pin->mask;

    if (dir == DIRECTION_INPUT) {
        if (self->pin->port == 4) {
            return (bool)(MXC_MCR->gpio4_ctrl & GPIO4_DATAIN_MASK(mask));
        }
        return MXC_GPIO_InGet(port, mask) && mask;
    } else {
        return MXC_GPIO_OutGet(port, mask) && mask;
    }
}

/** FIXME: Implement open-drain by switching to input WITHOUT re-labeling the pin */
digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(
    digitalio_digitalinout_obj_t *self, digitalio_drive_mode_t drive_mode) {

    if (drive_mode == DRIVE_MODE_OPEN_DRAIN) {
        common_hal_digitalio_digitalinout_switch_to_input(self, PULL_NONE);
    }
    // On MAX32, drive mode is not configurable
    // and should always be push-pull unless managed by a peripheral like I2C
    return DIGITALINOUT_OK;
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
    digitalio_digitalinout_obj_t *self) {
    return DRIVE_MODE_PUSH_PULL;
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_pull(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {

    mxc_gpio_regs_t *port = gpio_ports[self->pin->port];
    uint32_t mask = self->pin->mask;

    // padctrl registers only work in input mode
    if (self->pin->port == 4) {
        MXC_MCR->gpio4_ctrl &= ~(GPIO4_PULLDIS_MASK(mask));
        return DIGITALINOUT_OK;
    } else {
        if ((mask & port->en0) & (mask & ~(port->outen))) {
            // PULL_NONE, PULL_UP, or PULL_DOWN
            switch (pull) {
                case PULL_NONE:
                    port->padctrl0 &= ~(mask);
                    port->padctrl1 &= ~(mask);
                    break;
                case PULL_UP:
                    port->padctrl0 |= mask;
                    port->padctrl1 &= ~(mask);
                    break;
                case PULL_DOWN:
                    port->padctrl0 &= ~(mask);
                    port->padctrl1 |= mask;
                    break;
                default:
                    break;
            }
            return DIGITALINOUT_OK;
        } else {
            return DIGITALINOUT_PIN_BUSY;
        }
    }
}

/** FIXME: Account for GPIO4 handling here */
digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
    digitalio_digitalinout_obj_t *self) {

    bool pin_padctrl0 = (gpio_ports[self->pin->port]->padctrl0) & (self->pin->mask);
    bool pin_padctrl1 = (gpio_ports[self->pin->port]->padctrl1) & (self->pin->mask);

    if ((pin_padctrl0) && !(pin_padctrl1)) {
        return PULL_UP;
    } else if (!(pin_padctrl0) && pin_padctrl1) {
        return PULL_DOWN;
    } else if (!(pin_padctrl0) && !(pin_padctrl1)) {
        return PULL_NONE;
    } else {
        return PULL_NONE;
    }
}
