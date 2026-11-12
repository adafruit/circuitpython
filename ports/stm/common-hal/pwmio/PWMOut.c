// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
// SPDX-FileCopyrightText: Uses code from Micropython, Copyright (c) 2013-2016 Damien P. George
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "py/runtime.h"
#include "common-hal/pwmio/PWMOut.h"
#include "shared-bindings/pwmio/PWMOut.h"

#include STM32_HAL_H
#include "shared-bindings/microcontroller/Pin.h"

#include "timers.h"

// Bitmask of channels taken.
static uint8_t tim_channels_taken[TIM_BANK_ARRAY_LEN];
// Initial frequency timer is set to.
static uint32_t tim_frequencies[TIM_BANK_ARRAY_LEN];

static uint32_t timer_get_internal_duty(uint16_t duty, uint32_t period) {
    // duty cycle is duty/0xFFFF fraction x (number of pulses per period)
    return (duty * period) / 0xffff;
}

static bool timer_get_optimal_divisors(uint32_t *period, uint32_t *prescaler,
    uint32_t frequency, uint32_t source_freq) {
    // Find the largest possible period supported by this frequency
    *prescaler = 0;
    if (frequency == 0) {
        return false;
    }
    for (uint32_t i = 1; i <= 0xffff; i++) {
        *period = source_freq / ((uint64_t)i * frequency);
        if (*period < 2) {
            // A larger prescaler only makes the period shorter.
            break;
        }
        if (*period <= 0xffff) {
            *prescaler = i;
            break;
        }
    }
    // Return success or failure.
    return *prescaler != 0;
}

static void mark_deinit(pwmio_pwmout_obj_t *self) {
    self->tim = NULL;
}

pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint16_t duty,
    uint32_t frequency,
    bool variable_frequency) {
    // Default error is no timer at all on pin.
    pwmout_result_t last_failure = PWMOUT_INVALID_PIN;
    bool first_time_setup = true;

    mark_deinit(self);

    const mcu_tim_pin_obj_t *found = NULL;
    for (uint i = 0; i < MP_ARRAY_SIZE(mcu_tim_pin_list); i++) {
        const mcu_tim_pin_obj_t *tim = &mcu_tim_pin_list[i];
        uint8_t tim_index = tim->tim_index;
        uint8_t tim_channel_index = tim->channel_index;

        // if pin is same
        if (tim->pin == pin) {
            // PWMOut reserves its timers too, so a reserved timer with no PWM channels
            // taken belongs to an internal module (PulseIn, PulseOut, audio, ...).
            if (tim_index < TIM_BANK_ARRAY_LEN && tim_channels_taken[tim_index] == 0
                && stm_peripherals_timer_is_reserved(mcu_tim_banks[tim_index])) {
                last_failure = PWMOUT_INTERNAL_RESOURCES_IN_USE;
                continue; // keep looking
            }
            // check if the timer has a channel active
            if (tim_index < TIM_BANK_ARRAY_LEN && tim_channels_taken[tim_index] != 0) {
                // is it the same channel? (or all channels reserved by a var-freq)
                if (tim_channels_taken[tim_index] & (1 << tim_channel_index)) {
                    last_failure = PWMOUT_INTERNAL_RESOURCES_IN_USE;
                    continue; // keep looking, might be another viable option
                }
                // If the frequencies are the same it's ok
                if (tim_frequencies[tim_index] != frequency) {
                    last_failure = PWMOUT_INTERNAL_RESOURCES_IN_USE;
                    continue; // keep looking
                }
                // you can't put a variable frequency on a partially reserved timer
                if (variable_frequency) {
                    last_failure = PWMOUT_INTERNAL_RESOURCES_IN_USE;
                    continue; // keep looking
                }
                first_time_setup = false; // skip setting up the timer
            }
            // No problems taken, so set it up
            found = tim;
            break;
        }
    }

    if (found == NULL) {
        return last_failure;
    }
    uint8_t tim_index = found->tim_index;
    TIM_TypeDef *TIMx = mcu_tim_banks[tim_index];

    uint32_t prescaler = 0; // prescaler is 15 bit
    uint32_t period = 0; // period is 16 bit
    uint32_t source_freq = stm_peripherals_timer_get_source_freq(TIMx);
    if (!timer_get_optimal_divisors(&period, &prescaler, frequency, source_freq)) {
        return PWMOUT_INVALID_FREQUENCY;
    }

    // reserve timer/channel
    if (variable_frequency) {
        // Take all the channels.
        tim_channels_taken[tim_index] = 0x0F;
    } else {
        tim_channels_taken[tim_index] |= 1 << found->channel_index;
    }
    tim_frequencies[tim_index] = frequency;
    stm_peripherals_timer_reserve(TIMx);
    self->tim = found;
    self->variable_frequency = variable_frequency;

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = self->tim->altfn_index;
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);
    self->pin = pin;
    common_hal_mcu_pin_claim(pin);

    tim_clock_enable(1 << tim_index);

    // translate channel into handle value: TIM_CHANNEL_1, _2, _3, _4.
    self->channel = 4 * found->channel_index;

    // Timer init
    self->handle.Instance = TIMx;
    self->handle.Init.Period = period - 1;
    self->handle.Init.Prescaler = prescaler - 1;
    self->handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    self->handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    self->handle.Init.RepetitionCounter = 0;
    self->handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    // only run init if this is the first instance of this timer
    if (first_time_setup) {
        if (HAL_TIM_PWM_Init(&self->handle) != HAL_OK) {
            common_hal_pwmio_pwmout_deinit(self);
            return PWMOUT_INITIALIZATION_ERROR;
        }
    } else {
        // The timer is already running. HAL_TIM_PWM_Init() would restart it, but
        // HAL_TIM_PWM_Start() refuses a channel unless the handle says it's ready.
        self->handle.State = HAL_TIM_STATE_READY;
        TIM_CHANNEL_STATE_SET_ALL(&self->handle, HAL_TIM_CHANNEL_STATE_READY);
        TIM_CHANNEL_N_STATE_SET_ALL(&self->handle, HAL_TIM_CHANNEL_STATE_READY);
    }

    // Channel/PWM init
    self->chan_handle.OCMode = TIM_OCMODE_PWM1;
    self->chan_handle.Pulse = timer_get_internal_duty(duty, period);
    self->chan_handle.OCPolarity = TIM_OCPOLARITY_HIGH;
    self->chan_handle.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&self->handle, &self->chan_handle, self->channel) != HAL_OK) {
        common_hal_pwmio_pwmout_deinit(self);
        return PWMOUT_INITIALIZATION_ERROR;
    }
    if (HAL_TIM_PWM_Start(&self->handle, self->channel) != HAL_OK) {
        common_hal_pwmio_pwmout_deinit(self);
        return PWMOUT_INITIALIZATION_ERROR;
    }

    self->frequency = frequency;
    self->duty_cycle = duty;
    self->period = period;

    return PWMOUT_OK;
}


bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self) {
    return self->tim == NULL;
}

void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        return;
    }
    // var freq shuts down entire timer, others just their channel
    if (self->variable_frequency) {
        tim_channels_taken[self->tim->tim_index] = 0x00;
    } else {
        tim_channels_taken[self->tim->tim_index] &= ~(1 << self->tim->channel_index);
        HAL_TIM_PWM_Stop(&self->handle, self->channel);
    }
    common_hal_reset_pin(self->pin);

    // if reserved timer has no active channels, we can disable it
    if (tim_channels_taken[self->tim->tim_index] == 0) {
        tim_frequencies[self->tim->tim_index] = 0x00;
        HAL_TIM_PWM_DeInit(&self->handle);
        stm_peripherals_timer_free(self->handle.Instance);
    }

    mark_deinit(self);
}

void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self, uint16_t duty) {
    uint32_t internal_duty_cycle = timer_get_internal_duty(duty, self->period);
    __HAL_TIM_SET_COMPARE(&self->handle, self->channel, internal_duty_cycle);
    self->duty_cycle = duty;
}

uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self) {
    return self->duty_cycle;
}

void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self, uint32_t frequency) {
    // don't halt setup for the same frequency
    if (frequency == self->frequency) {
        return;
    }

    uint32_t prescaler = 0;
    uint32_t period = 0;
    uint32_t source_freq = stm_peripherals_timer_get_source_freq(self->handle.Instance);
    if (!timer_get_optimal_divisors(&period, &prescaler, frequency, source_freq)) {
        mp_arg_error_invalid(MP_QSTR_frequency);
    }

    // shut down
    HAL_TIM_PWM_Stop(&self->handle, self->channel);

    // Only change altered values
    self->handle.Init.Period = period - 1;
    self->handle.Init.Prescaler = prescaler - 1;

    // restart everything, adjusting for new speed
    if (HAL_TIM_PWM_Init(&self->handle) != HAL_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("timer re-init"));
    }

    self->chan_handle.Pulse = timer_get_internal_duty(self->duty_cycle, period);

    if (HAL_TIM_PWM_ConfigChannel(&self->handle, &self->chan_handle, self->channel) != HAL_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("channel re-init"));
    }
    if (HAL_TIM_PWM_Start(&self->handle, self->channel) != HAL_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("PWM restart"));
    }

    tim_frequencies[self->tim->tim_index] = frequency;
    self->frequency = frequency;
    self->period = period;
}

uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t *self) {
    return self->frequency;
}

bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t *self) {
    return self->variable_frequency;
}

const mcu_pin_obj_t *common_hal_pwmio_pwmout_get_pin(pwmio_pwmout_obj_t *self) {
    return self->pin;
}
