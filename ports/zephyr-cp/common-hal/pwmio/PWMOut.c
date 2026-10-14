// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

// pwmio on Zephyr's PWM API. The iobroker module picks the PWM device and
// channel for the pin; channels that share a period (an nRF instance's
// outputs) are grouped there, so nothing here is SoC-specific. The period is
// computed here from the device's clock. Checking that the hardware can
// produce it is left to the driver, which doesn't reliably do so yet (pwm_nrfx
// skips the check at 0 % and 100 % duty).

#include <errno.h>

#include <iobroker/iobroker.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#include "py/runtime.h"

#include "shared-bindings/pwmio/PWMOut.h"

// Shortest period accepted, in clock cycles: a period needs a high and a low
// part.
#define MIN_PERIOD_CYCLES 2U

// Truncate a frequency to a whole number of clock cycles, so the actual
// frequency is the same or slightly higher. Returns 0, or a negative errno
// when the period would be out of range or the clock is unknown.
static int frequency_to_period(const struct device *dev, uint32_t channel,
    uint32_t frequency, uint32_t *period_cycles_out) {
    uint64_t cycles_per_sec;
    int ret = pwm_get_cycles_per_sec(dev, channel, &cycles_per_sec);
    if (ret < 0) {
        return ret;
    }
    uint64_t period = cycles_per_sec / frequency;
    if (period < MIN_PERIOD_CYCLES || period > UINT32_MAX) {
        return -EINVAL;
    }
    *period_cycles_out = (uint32_t)period;
    return 0;
}

static uint32_t pulse_cycles(uint32_t period_cycles, uint16_t duty) {
    return (uint32_t)(((uint64_t)period_cycles * duty) / 0xffff);
}

// Program the channel's period and the stored duty cycle.
static int set_period_and_duty(pwmio_pwmout_obj_t *self) {
    return pwm_set_cycles(self->dev, self->channel, self->period_cycles,
        pulse_cycles(self->period_cycles, self->duty_cycle), 0);
}

pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t duty, uint32_t frequency,
    bool variable_frequency) {
    // Deinited until construction succeeds.
    self->dev = NULL;
    if (frequency == 0) {
        return PWMOUT_INVALID_FREQUENCY;
    }
    const struct device *dev;
    uint32_t channel;
    // A variable frequency needs a group of its own, since changing the
    // period would change it for every channel in the group.
    int ret = iobroker_pwm_channel_allocate(pin->package_pin, frequency,
        variable_frequency, &dev, &channel);
    switch (ret) {
        case 0:
            break;
        case -ENODEV:
            return PWMOUT_INTERNAL_RESOURCES_IN_USE;
        case -EINVAL:
        case -EBUSY:
        case -ENOSYS:
            return PWMOUT_INVALID_PIN;
        default:
            return PWMOUT_INITIALIZATION_ERROR;
    }

    // The first channel of a group initializes the device; an existing
    // group reports -EALREADY.
    ret = device_init(dev);
    if (ret < 0 && ret != -EALREADY) {
        (void)iobroker_pwm_channel_release(dev, channel);
        return PWMOUT_INITIALIZATION_ERROR;
    }
    uint32_t period_cycles;
    if (frequency_to_period(dev, channel, frequency, &period_cycles) < 0) {
        (void)iobroker_pwm_channel_release(dev, channel);
        return PWMOUT_INVALID_FREQUENCY;
    }

    self->pin = pin;
    self->dev = dev;
    self->channel = channel;
    self->period_cycles = period_cycles;
    self->duty_cycle = duty;
    self->variable_frequency = variable_frequency;
    // The driver may reject a period its hardware can't produce.
    if (set_period_and_duty(self) < 0) {
        (void)iobroker_pwm_channel_release(dev, channel);
        self->dev = NULL;
        return PWMOUT_INVALID_FREQUENCY;
    }
    return PWMOUT_OK;
}

bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self) {
    return self->dev == NULL;
}

void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        return;
    }
    // Stop the output, then hand the channel back.
    (void)pwm_set_cycles(self->dev, self->channel, self->period_cycles, 0, 0);
    (void)iobroker_pwm_channel_release(self->dev, self->channel);
    self->dev = NULL;
    self->pin = NULL;
}

void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self, uint16_t duty) {
    self->duty_cycle = duty;
    (void)set_period_and_duty(self);
}

uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self) {
    return self->duty_cycle;
}

void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self, uint32_t frequency) {
    // shared-bindings raises AttributeError unless variable_frequency=True,
    // so the channel's group is exclusive and no other channel shares the
    // period. It also rejects 0 Hz, so frequency_to_period() can divide.
    uint32_t period_cycles;
    if (frequency_to_period(self->dev, self->channel, frequency, &period_cycles) < 0) {
        common_hal_pwmio_pwmout_raise_error(PWMOUT_INVALID_FREQUENCY);
    }
    uint32_t old_period_cycles = self->period_cycles;
    self->period_cycles = period_cycles;
    if (set_period_and_duty(self) < 0) {
        // The driver rejected the period: keep the old one.
        self->period_cycles = old_period_cycles;
        (void)set_period_and_duty(self);
        common_hal_pwmio_pwmout_raise_error(PWMOUT_INVALID_FREQUENCY);
    }
}

uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t *self) {
    // The frequency the stored period produces. The driver may truncate the
    // period further to fit its registers (on nRF, to a multiple of the
    // prescaler), which Zephyr's PWM API doesn't report, so at low
    // frequencies this can be very slightly below the actual output (under
    // 0.01 % on nRF).
    uint64_t cycles_per_sec;
    if (pwm_get_cycles_per_sec(self->dev, self->channel, &cycles_per_sec) < 0) {
        return 0;
    }
    return (uint32_t)((cycles_per_sec + self->period_cycles / 2) / self->period_cycles);
}

bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t *self) {
    return self->variable_frequency;
}

const mcu_pin_obj_t *common_hal_pwmio_pwmout_get_pin(pwmio_pwmout_obj_t *self) {
    return self->pin;
}
