// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "peripherals/pins.h"
#include "peripherals/periph.h"

// I2C

I2C_TypeDef *mcu_i2c_banks[3] = {I2C1, I2C2, I2C3};

const mcu_periph_obj_t mcu_i2c_sda_list[7] = {
    PERIPH(1, 4, &pin_PB07),
    PERIPH(1, 4, &pin_PB09),
    PERIPH(2, 9, &pin_PB09),
    PERIPH(2, 9, &pin_PB03),
    PERIPH(3, 4, &pin_PC09),
    PERIPH(3, 9, &pin_PB04),
    PERIPH(3, 9, &pin_PB08)
};

const mcu_periph_obj_t mcu_i2c_scl_list[4] = {
    PERIPH(1, 4, &pin_PB06),
    PERIPH(1, 4, &pin_PB08),
    PERIPH(2, 4, &pin_PB10),
    PERIPH(3, 4, &pin_PA08)
};

// SPI

SPI_TypeDef *mcu_spi_banks[5] = {SPI1, SPI2, SPI3, SPI4, SPI5};

const mcu_periph_obj_t mcu_spi_sck_list[10] = {
    PERIPH(1, 5, &pin_PA05),
    PERIPH(1, 5, &pin_PB03),
    PERIPH(2, 5, &pin_PB10),
    PERIPH(2, 5, &pin_PB13),
    PERIPH(2, 5, &pin_PC07),
    PERIPH(3, 6, &pin_PB03),
    PERIPH(3, 7, &pin_PB12),
    PERIPH(3, 6, &pin_PC10),
    PERIPH(4, 6, &pin_PB13),
    PERIPH(5, 6, &pin_PB00)
};

const mcu_periph_obj_t mcu_spi_mosi_list[9] = {
    PERIPH(1, 5, &pin_PA07),
    PERIPH(1, 5, &pin_PB05),
    PERIPH(2, 5, &pin_PB15),
    PERIPH(2, 5, &pin_PC03),
    PERIPH(3, 6, &pin_PB05),
    PERIPH(3, 6, &pin_PC12),
    PERIPH(4, 5, &pin_PA01),
    PERIPH(5, 6, &pin_PA10),
    PERIPH(5, 6, &pin_PB08)
};

const mcu_periph_obj_t mcu_spi_miso_list[8] = {
    PERIPH(1, 5, &pin_PA06),
    PERIPH(1, 5, &pin_PB04),
    PERIPH(2, 5, &pin_PB14),
    PERIPH(2, 5, &pin_PC02),
    PERIPH(3, 6, &pin_PB04),
    PERIPH(3, 6, &pin_PC11),
    PERIPH(4, 6, &pin_PA11),
    PERIPH(5, 6, &pin_PA12)
};

const mcu_periph_obj_t mcu_spi_nss_list[8] = {
    PERIPH(1, 5, &pin_PA04),
    PERIPH(1, 5, &pin_PA15),
    PERIPH(2, 5, &pin_PB09),
    PERIPH(2, 5, &pin_PB12),
    PERIPH(3, 6, &pin_PA04),
    PERIPH(3, 6, &pin_PA15),
    PERIPH(4, 6, &pin_PB12),
    PERIPH(5, 6, &pin_PB01)
};

// UART

USART_TypeDef *mcu_uart_banks[MAX_UART] = {USART1, USART2, USART3, NULL, NULL, USART6};
bool mcu_uart_has_usart[MAX_UART] = {true, true, true, false, false, true};

const mcu_periph_obj_t mcu_uart_tx_list[8] = {
    PERIPH(2, 7, &pin_PA02),
    PERIPH(1, 7, &pin_PA09),
    PERIPH(1, 7, &pin_PA15),
    PERIPH(6, 8, &pin_PA11),
    PERIPH(1, 7, &pin_PB06),
    PERIPH(3, 7, &pin_PB10),
    PERIPH(6, 8, &pin_PC06),
    PERIPH(3, 7, &pin_PC10)
};

const mcu_periph_obj_t mcu_uart_rx_list[8] = {
    PERIPH(2, 7, &pin_PA03),
    PERIPH(1, 7, &pin_PA10),
    PERIPH(6, 8, &pin_PA12),
    PERIPH(1, 7, &pin_PB03),
    PERIPH(1, 7, &pin_PB07),
    PERIPH(3, 7, &pin_PC05),
    PERIPH(6, 8, &pin_PC07),
    PERIPH(3, 7, &pin_PC11)
};

// Timers
// TIM6 and TIM7 are basic timers that are only used by DAC, and don't have pins
TIM_TypeDef *mcu_tim_banks[14] = {TIM1, TIM2, TIM3, TIM4, TIM5, NULL, NULL, TIM8, TIM9, TIM10,
                                  TIM11, TIM12, TIM13, TIM14};

const mcu_tim_pin_obj_t mcu_tim_pin_list[41] = {
    TIM(2, 1, 1, &pin_PA00),
    TIM(5, 2, 1, &pin_PA00),
    TIM(2, 1, 2, &pin_PA01),
    TIM(5, 2, 2, &pin_PA01),
    TIM(2, 1, 3, &pin_PA02),
    TIM(5, 2, 3, &pin_PA02),
    TIM(2, 1, 4, &pin_PA03),
    TIM(5, 2, 4, &pin_PA03),
    TIM(9, 3, 1, &pin_PA02),
    TIM(9, 3, 2, &pin_PA03),
    TIM(3, 2, 1, &pin_PA06),
    TIM(13, 9, 1, &pin_PA06),
    TIM(3, 2, 2, &pin_PA07),
    TIM(14, 9, 1, &pin_PA07),
    TIM(1, 1, 1, &pin_PA08),
    TIM(1, 1, 2, &pin_PA09),
    TIM(1, 1, 3, &pin_PA10),
    TIM(1, 1, 4, &pin_PA11),
    TIM(2, 1, 1, &pin_PA15),
    TIM(3, 2, 3, &pin_PB00),
    TIM(3, 2, 4, &pin_PB01),
    TIM(2, 1, 2, &pin_PB03),
    TIM(3, 2, 1, &pin_PB04),
    TIM(3, 2, 2, &pin_PB05),
    TIM(4, 2, 1, &pin_PB06),
    TIM(4, 2, 2, &pin_PB07),
    TIM(4, 2, 3, &pin_PB08),
    TIM(10, 3, 1, &pin_PB08),
    TIM(4, 2, 4, &pin_PB09),
    TIM(11, 3, 1, &pin_PB09),
    TIM(2, 1, 3, &pin_PB10),
    TIM(12, 9, 1, &pin_PB14),
    TIM(12, 9, 2, &pin_PB15),
    TIM(3, 2, 1, &pin_PC06),
    TIM(3, 2, 2, &pin_PC07),
    TIM(3, 2, 3, &pin_PC08),
    TIM(3, 2, 4, &pin_PC09),
    TIM(8, 3, 1, &pin_PC06),
    TIM(8, 3, 2, &pin_PC07),
    TIM(8, 3, 3, &pin_PC08),
    TIM(8, 3, 4, &pin_PC09)
};

// FSMC, used by paralleldisplaybus. periph_index is the data bit (D0-D7).
// Pin and alternate function mapping from ST's STM32_open_pin_data.

const mcu_periph_obj_t mcu_fsmc_data_list[8] = {
    PERIPH(0, 10, &pin_PB14),
    PERIPH(1, 10, &pin_PC06),
    PERIPH(2, 10, &pin_PC11),
    PERIPH(3, 10, &pin_PC12),
    PERIPH(4, 12, &pin_PA02),
    PERIPH(5, 12, &pin_PA03),
    PERIPH(6, 12, &pin_PA04),
    PERIPH(7, 12, &pin_PA05)
};

const mcu_periph_obj_t mcu_fsmc_nwe_list[2] = {
    PERIPH(1, 12, &pin_PC02),
    PERIPH(1, 10, &pin_PD02)
};

const mcu_periph_obj_t mcu_fsmc_noe_list[1] = {
    PERIPH(1, 12, &pin_PC05)
};
