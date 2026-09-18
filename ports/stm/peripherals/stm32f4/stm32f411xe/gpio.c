// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "peripherals/gpio.h"
#include "stm32f4xx_hal.h"
#include "common-hal/microcontroller/Pin.h"

void stm32_peripherals_gpio_init(void) {
    // * GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // Never reset pins
    // TODO: Move this out of peripherals. These helpers shouldn't reference anything CircuitPython
    // specific.

    #if !(BOARD_OVERWRITE_SWD)
    #endif

    // Port H is not included in GPIO port array
}

// LEDs are inverted on F411 DISCO
void stm32f4_peripherals_status_led(uint8_t led, uint8_t state) {
}
