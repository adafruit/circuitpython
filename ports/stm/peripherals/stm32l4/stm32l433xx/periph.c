// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Blues Wireless Contributors
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "peripherals/pins.h"
#include "peripherals/periph.h"

I2C_TypeDef *mcu_i2c_banks[I2C_BANK_ARRAY_LEN] = {I2C1, I2C2};

const mcu_periph_obj_t mcu_i2c_sda_list[I2C_SDA_ARRAY_LEN] = {
    PERIPH(1, 4, &pin_PA10),
    PERIPH(2, 4, &pin_PB14),
};

const mcu_periph_obj_t mcu_i2c_scl_list[I2C_SCL_ARRAY_LEN] = {
    PERIPH(1, 4, &pin_PA09),
    PERIPH(2, 4, &pin_PB13),
};

SPI_TypeDef *mcu_spi_banks[SPI_BANK_ARRAY_LEN] = {SPI1};

const mcu_periph_obj_t mcu_spi_sck_list[SPI_SCK_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PB03),
};
const mcu_periph_obj_t mcu_spi_mosi_list[SPI_MOSI_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA12),
};
const mcu_periph_obj_t mcu_spi_miso_list[SPI_MISO_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA11),
};
const mcu_periph_obj_t mcu_spi_nss_list[SPI_NSS_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA15),
};

USART_TypeDef *mcu_uart_banks[MAX_UART] = {USART1, USART2, USART3};
bool mcu_uart_has_usart[MAX_UART] = {true, true, true, false, false};

const mcu_periph_obj_t mcu_uart_tx_list[UART_TX_ARRAY_LEN] = {
    PERIPH(2, 7, &pin_PA02),
    PERIPH(1, 7, &pin_PA09),
    PERIPH(1, 7, &pin_PB06),
    PERIPH(3, 7, &pin_PB10),
};
const mcu_periph_obj_t mcu_uart_rx_list[UART_RX_ARRAY_LEN] = {
    PERIPH(2, 7, &pin_PA03),
    PERIPH(1, 7, &pin_PA10),
    PERIPH(2, 3, &pin_PA15),
    PERIPH(1, 7, &pin_PB07),
    PERIPH(3, 7, &pin_PB11),
};

// Timers
TIM_TypeDef *mcu_tim_banks[TIM_BANK_ARRAY_LEN] = {TIM1, TIM2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, TIM15, TIM16, NULL};

const mcu_tim_pin_obj_t mcu_tim_pin_list[TIM_PIN_ARRAY_LEN] = {
    TIM(2, 1, 1, &pin_PA00),
    TIM(2, 1, 2, &pin_PA01),
    TIM(2, 1, 3, &pin_PA02),
    TIM(15, 14, 1, &pin_PA02),
    TIM(2, 1, 4, &pin_PA03),
    TIM(15, 14, 2, &pin_PA03),
    TIM(2, 1, 1, &pin_PA05),
    TIM(16, 14, 1, &pin_PA06),
    TIM(1, 1, 1, &pin_PA08),
    TIM(1, 1, 2, &pin_PA09),
    TIM(1, 1, 3, &pin_PA10),
    TIM(1, 1, 4, &pin_PA11),
    TIM(2, 1, 1, &pin_PA15),
    TIM(2, 1, 2, &pin_PB03),
    TIM(16, 14, 1, &pin_PB08),
    TIM(2, 1, 3, &pin_PB10),
    TIM(2, 1, 4, &pin_PB11),
    TIM(15, 14, 1, &pin_PB14),
    TIM(15, 14, 2, &pin_PB15),
};

// CAN
CAN_TypeDef *mcu_can_banks[] = {CAN1};

const mcu_periph_obj_t mcu_can_tx_list[2] = {
    PERIPH(1, 10, &pin_PA12),
    PERIPH(1, 10, &pin_PB09),
};
const mcu_periph_obj_t mcu_can_rx_list[2] = {
    PERIPH(1, 10, &pin_PA11),
    PERIPH(1, 10, &pin_PB08),
};
