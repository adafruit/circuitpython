// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "gpio.h"
#include "common-hal/microcontroller/Pin.h"

void stm32_peripherals_gpio_init(void) {
    // Enable all GPIO for now
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // Never reset pins
    never_reset_pin_number(2, 14); // PC14 OSC32_IN
    never_reset_pin_number(2, 15); // PC15 OSC32_OUT
    never_reset_pin_number(0, 13); // PA13 SWDIO
    never_reset_pin_number(0, 14); // PA14 SWCLK

    never_reset_pin_number(1,  6); // PB6  OSPI NCS
    never_reset_pin_number(1,  2); // PB2  OSPI CLK
    never_reset_pin_number(3, 11); // PD11 OSPI IO0
    never_reset_pin_number(3, 12); // PD12 OSPI IO1
    never_reset_pin_number(4,  2); // PE2  OSPI IO2
    never_reset_pin_number(3, 13); // PD13 OSPI IO3

}
