// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 snkYmkrct
//
// SPDX-License-Identifier: MIT

#include "gpio.h"
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
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

// Never reset pins
    never_reset_pin_number(7, 0); // PH00 OSC32_IN
    never_reset_pin_number(7, 1); // PH01 OSC32_OUT
    never_reset_pin_number(0, 13); // PA13 SWDIO
    never_reset_pin_number(0, 14); // PA14 SWCLK
    // qspi flash pins for the Daisy Seed -- TODO ?
    never_reset_pin_number(5, 6); // PF06 QSPI IO3
    never_reset_pin_number(5, 7); // PF07 QSPI IO2
    never_reset_pin_number(5, 8); // PF08 QSPI IO0
    never_reset_pin_number(5, 9); // PF09 QSPI IO1
    never_reset_pin_number(5, 10); // PF10 QSPI CLK
    never_reset_pin_number(6, 6); // PG06 QSPI NCS

}
