// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "peripherals/gpio.h"
#include "common-hal/microcontroller/Pin.h"

void stm32_peripherals_gpio_init(void) {
    // Enable all GPIO for now
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    // Pins in use by the system; mark them claimed so user code can't use them.
    claim_pin(2, 13); // PC13 anti tamp
    claim_pin(2, 14); // PC14 OSC32_IN
    claim_pin(2, 15); // PC15 OSC32_OUT
    claim_pin(0, 13); // PA13 SWDIO
    claim_pin(0, 14); // PA14 SWCLK

    // Port H is not included in GPIO port array
}

void stm32f4_peripherals_status_led(uint8_t led, uint8_t state) {

}
