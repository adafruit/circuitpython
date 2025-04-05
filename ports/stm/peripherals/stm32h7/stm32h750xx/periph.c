// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 snkYmkrct
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "peripherals/pins.h"
#include "peripherals/periph.h"

// See alternate functions tables in the STM32H750xx datasheet

// I2C
I2C_TypeDef *mcu_i2c_banks[MAX_I2C] = {I2C1, I2C2, I2C3, I2C4};

const mcu_periph_obj_t mcu_i2c_sda_list[12] = {
    PERIPH(1, 4, &pin_PB07),
    PERIPH(4, 6, &pin_PB07),
    PERIPH(1, 4, &pin_PB09),
    PERIPH(4, 6, &pin_PB09),
    PERIPH(2, 4, &pin_PB11),
    PERIPH(3, 4, &pin_PC09),
    PERIPH(4, 4, &pin_PD13),
    PERIPH(2, 4, &pin_PF00),
    PERIPH(4, 4, &pin_PF15),
    PERIPH(2, 4, &pin_PH05),
    PERIPH(3, 4, &pin_PH08),
    PERIPH(4, 4, &pin_PH12)
};

const mcu_periph_obj_t mcu_i2c_scl_list[12] = {
    PERIPH(3, 4, &pin_PA08),
    PERIPH(1, 4, &pin_PB06),
    PERIPH(4, 6, &pin_PB06),
    PERIPH(1, 4, &pin_PB08),
    PERIPH(4, 6, &pin_PB08),
    PERIPH(2, 4, &pin_PB10),
    PERIPH(4, 4, &pin_PD12),
    PERIPH(2, 4, &pin_PF01),
    PERIPH(4, 4, &pin_PF14),
    PERIPH(2, 4, &pin_PH04),
    PERIPH(3, 4, &pin_PH07),
    PERIPH(4, 4, &pin_PH11)
};

// SPI

SPI_TypeDef *mcu_spi_banks[MAX_SPI] = {SPI1, SPI2, SPI3, SPI4, SPI5, SPI6};

const mcu_periph_obj_t mcu_spi_sck_list[18] = {
    PERIPH(1, 5, &pin_PA05),
    PERIPH(6, 8, &pin_PA05),
    PERIPH(2, 5, &pin_PA09),
    PERIPH(2, 5, &pin_PA12),
    PERIPH(1, 5, &pin_PB03),
    PERIPH(3, 6, &pin_PB03),
    PERIPH(6, 8, &pin_PB03),
    PERIPH(2, 5, &pin_PB10),
    PERIPH(2, 5, &pin_PB13),
    PERIPH(3, 6, &pin_PC10),
    PERIPH(2, 5, &pin_PD03),
    PERIPH(4, 5, &pin_PE02),
    PERIPH(4, 5, &pin_PE12),
    PERIPH(5, 5, &pin_PF07),
    PERIPH(1, 5, &pin_PG11),
    PERIPH(6, 5, &pin_PG13),
    PERIPH(5, 5, &pin_PH06),
    PERIPH(2, 5, &pin_PI01),
};

const mcu_periph_obj_t mcu_spi_mosi_list[18] = {
    PERIPH(1, 5, &pin_PA07),
    PERIPH(6, 8, &pin_PA07),
    PERIPH(3, 7, &pin_PB02),
    PERIPH(1, 5, &pin_PB05),
    PERIPH(3, 7, &pin_PB05),
    PERIPH(6, 8, &pin_PB05),
    PERIPH(2, 5, &pin_PB15),
    PERIPH(2, 5, &pin_PC01),
    PERIPH(2, 5, &pin_PC03),
    PERIPH(3, 6, &pin_PC12),
    PERIPH(3, 5, &pin_PD06),
    PERIPH(1, 5, &pin_PD07),
    PERIPH(4, 5, &pin_PE06),
    PERIPH(4, 5, &pin_PE14),
    PERIPH(5, 5, &pin_PF09),
    PERIPH(5, 5, &pin_PF11),
    PERIPH(6, 5, &pin_PG14),
    PERIPH(2, 5, &pin_PI03),
};

const mcu_periph_obj_t mcu_spi_miso_list[15] = {
    PERIPH(1, 5, &pin_PA06),
    PERIPH(6, 8, &pin_PA06),
    PERIPH(1, 5, &pin_PB04),
    PERIPH(3, 6, &pin_PB04),
    PERIPH(6, 8, &pin_PB04),
    PERIPH(2, 5, &pin_PB14),
    PERIPH(2, 5, &pin_PC02),
    PERIPH(3, 6, &pin_PC11),
    PERIPH(4, 5, &pin_PE05),
    PERIPH(4, 5, &pin_PE13),
    PERIPH(5, 5, &pin_PF08),
    PERIPH(1, 5, &pin_PG09),
    PERIPH(6, 5, &pin_PG12),
    PERIPH(5, 5, &pin_PH07),
    PERIPH(2, 5, &pin_PI02),
};

// UART

USART_TypeDef *mcu_uart_banks[MAX_UART] = {USART1, USART2, USART3, UART4, UART5, USART6, UART7, UART8};
// circuitpython doesn't implement USART
// bool mcu_uart_has_usart[MAX_UART] = {true, true, true, false, false, true, false, false, false};

const mcu_periph_obj_t mcu_uart_tx_list[24] = {
    PERIPH(4, 8, &pin_PA00),
    PERIPH(2, 7, &pin_PA02),
    PERIPH(1, 7, &pin_PA09),
    PERIPH(4, 6, &pin_PA12),
    PERIPH(7, 11, &pin_PA15),
    PERIPH(7, 11, &pin_PB04),
    PERIPH(1, 7, &pin_PB06),
    PERIPH(5, 14, &pin_PB06),
    PERIPH(4, 8, &pin_PB09),
    PERIPH(3, 7, &pin_PB10),
    PERIPH(5, 14, &pin_PB13),
    PERIPH(1, 4, &pin_PB14),
    PERIPH(6, 7, &pin_PC06),
    PERIPH(3, 7, &pin_PC10),
    PERIPH(4, 8, &pin_PC10),
    PERIPH(5, 8, &pin_PC12),
    PERIPH(4, 8, &pin_PD01),
    PERIPH(2, 7, &pin_PD05),
    PERIPH(3, 7, &pin_PD08),
    PERIPH(8, 8, &pin_PE01),
    PERIPH(7, 7, &pin_PE08),
    PERIPH(7, 7, &pin_PF07),
    PERIPH(6, 7, &pin_PG14),
    PERIPH(4, 8, &pin_PH13),
};

const mcu_periph_obj_t mcu_uart_rx_list[25] = {
    PERIPH(4, 8, &pin_PA01),
    PERIPH(2, 7, &pin_PA03),
    PERIPH(7, 11, &pin_PA08),
    PERIPH(1, 7, &pin_PA10),
    PERIPH(4, 6, &pin_PA11),
    PERIPH(7, 11, &pin_PB03),
    PERIPH(5, 14, &pin_PB05),
    PERIPH(1, 7, &pin_PB07),
    PERIPH(4, 8, &pin_PB08),
    PERIPH(3, 7, &pin_PB11),
    PERIPH(5, 14, &pin_PB12),
    PERIPH(1, 4, &pin_PB15),
    PERIPH(6, 7, &pin_PC07),
    PERIPH(3, 7, &pin_PC11),
    PERIPH(4, 8, &pin_PC11),
    PERIPH(4, 8, &pin_PD00),
    PERIPH(5, 8, &pin_PD02),
    PERIPH(2, 7, &pin_PD06),
    PERIPH(3, 7, &pin_PD09),
    PERIPH(8, 8, &pin_PE00),
    PERIPH(7, 7, &pin_PE07),
    PERIPH(7, 7, &pin_PF06),
    PERIPH(6, 7, &pin_PG09),
    PERIPH(4, 8, &pin_PH14),
    PERIPH(4, 8, &pin_PI09),
};

// Timers
// TIM6 and TIM7 are basic timers that are only used by DAC, and don't have pins
TIM_TypeDef *mcu_tim_banks[TIM_BANK_ARRAY_LEN] = {TIM1, TIM2, TIM3, TIM4, TIM5, NULL, NULL, TIM8,
                                                  NULL, NULL, NULL, TIM12, TIM13, TIM14, TIM15, TIM16, TIM17};
const mcu_tim_pin_obj_t mcu_tim_pin_list[TIM_PIN_ARRAY_LEN] = {
    TIM(2, 1, 1, &pin_PA00),
    TIM(5, 2, 1, &pin_PA00),
    TIM(2, 1, 2, &pin_PA01),
    TIM(5, 2, 2, &pin_PA01),
    TIM(2, 1, 3, &pin_PA02),
    TIM(5, 2, 3, &pin_PA02),
    TIM(15, 4, 1, &pin_PA02),
    TIM(2, 1, 4, &pin_PA03),
    TIM(5, 2, 4, &pin_PA03),
    TIM(15, 4, 2, &pin_PA03),
    TIM(2, 1, 1, &pin_PA05),
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
    TIM(4, 2, 4, &pin_PB09),
    TIM(2, 1, 3, &pin_PB10),
    TIM(2, 1, 4, &pin_PB11),
    TIM(12, 2, 1, &pin_PB14),
    TIM(12, 2, 2, &pin_PB15),
    TIM(3, 2, 1, &pin_PC06),
    TIM(8, 3, 1, &pin_PC06),
    TIM(3, 2, 2, &pin_PC07),
    TIM(8, 3, 2, &pin_PC07),
    TIM(3, 2, 3, &pin_PC08),
    TIM(8, 3, 3, &pin_PC08),
    TIM(3, 2, 4, &pin_PC09),
    TIM(8, 3, 4, &pin_PC09),
    TIM(4, 2, 1, &pin_PD12),
    TIM(4, 2, 2, &pin_PD13),
    TIM(4, 2, 3, &pin_PD14),
    TIM(4, 2, 4, &pin_PD15),
    TIM(15, 4, 1, &pin_PE05),
    TIM(15, 4, 2, &pin_PE06),
    TIM(1, 1, 1, &pin_PE09),
    TIM(1, 1, 2, &pin_PE11),
    TIM(1, 1, 3, &pin_PE13),
    TIM(1, 1, 4, &pin_PE14),
    TIM(16, 1, 1, &pin_PF06),
    TIM(17, 1, 1, &pin_PF07),
    TIM(13, 9, 1, &pin_PF08),
    TIM(14, 9, 1, &pin_PF09),
    TIM(12, 2, 1, &pin_PH06),
    TIM(12, 2, 2, &pin_PH09),
    TIM(5, 2, 1, &pin_PH10),
    TIM(5, 2, 2, &pin_PH11),
    TIM(5, 2, 3, &pin_PH12),
    TIM(5, 2, 4, &pin_PI00),
    TIM(8, 3, 4, &pin_PI02),
    TIM(8, 3, 1, &pin_PI05),
    TIM(8, 3, 2, &pin_PI06),
    TIM(8, 3, 3, &pin_PI07),
};

// SDIO - H750 has a MMC interface that includes SDIO
SDMMC_TypeDef *mcu_sdio_banks[1] = {SDMMC1};

const mcu_periph_obj_t mcu_sdio_clock_list[1] = {
    PERIPH(1, 12, &pin_PC12),
};
const mcu_periph_obj_t mcu_sdio_command_list[1] = {
    PERIPH(1, 12, &pin_PD02),
};
const mcu_periph_obj_t mcu_sdio_data0_list[1] = {
    PERIPH(1, 12, &pin_PC08),
};
const mcu_periph_obj_t mcu_sdio_data1_list[1] = {
    PERIPH(1, 12, &pin_PC09),
};
const mcu_periph_obj_t mcu_sdio_data2_list[1] = {
    PERIPH(1, 12, &pin_PC10),
};
const mcu_periph_obj_t mcu_sdio_data3_list[1] = {
    PERIPH(1, 12, &pin_PC11),
};


/** FMC GPIO Configuration
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PG15   ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PI7   ------> FMC_D29
  PI6   ------> FMC_D28
  PI5   ------> FMC_NBL3
  PD1   ------> FMC_D3
  PI3   ------> FMC_D27
  PI2   ------> FMC_D26
  PI9   ------> FMC_D30
  PI4   ------> FMC_NBL2
  PH15   ------> FMC_D23
  PI1   ------> FMC_D25
  PF0   ------> FMC_A0
  PI10   ------> FMC_D31
  PH13   ------> FMC_D21
  PH14   ------> FMC_D22
  PI0   ------> FMC_D24
  PH2   ------> FMC_SDCKE0
  PH3   ------> FMC_SDNE0
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PG8   ------> FMC_SDCLK
  PF3   ------> FMC_A3
  PF4   ------> FMC_A4
  PH5   ------> FMC_SDNWE
  PF5   ------> FMC_A5
  PH12   ------> FMC_D20
  PG5   ------> FMC_BA1
  PG4   ------> FMC_BA0
  PH11   ------> FMC_D19
  PH10   ------> FMC_D18
  PD15   ------> FMC_D1
  PG2   ------> FMC_A12
  PG1   ------> FMC_A11
  PH8   ------> FMC_D16
  PH9   ------> FMC_D17
  PD14   ------> FMC_D0
  PF13   ------> FMC_A7
  PG0   ------> FMC_A10
  PE13   ------> FMC_D10
  PD10   ------> FMC_D15
  PF12   ------> FMC_A6
  PF15   ------> FMC_A9
  PE8   ------> FMC_D5
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PE14   ------> FMC_D11
  PD9   ------> FMC_D14
  PD8   ------> FMC_D13
  PF11   ------> FMC_SDNRAS
  PF14   ------> FMC_A8
  PE7   ------> FMC_D4
  PE10   ------> FMC_D7
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  */

const mcu_periph_obj_t sdram_pin_list[57] = {
    PERIPH(4, 12, &pin_PE01),
    PERIPH(4, 12, &pin_PE00),
    PERIPH(6, 12, &pin_PG15),
    PERIPH(3, 12, &pin_PD00),
    PERIPH(8, 12, &pin_PI07),
    PERIPH(8, 12, &pin_PI06),
    PERIPH(8, 12, &pin_PI05),
    PERIPH(3, 12, &pin_PD01),
    PERIPH(8, 12, &pin_PI03),
    PERIPH(8, 12, &pin_PI02),
    PERIPH(8, 12, &pin_PI09),
    PERIPH(8, 12, &pin_PI04),
    PERIPH(7, 12, &pin_PH15),
    PERIPH(8, 12, &pin_PI01),
    PERIPH(5, 12, &pin_PF00),
    PERIPH(8, 12, &pin_PI10),
    PERIPH(7, 12, &pin_PH13),
    PERIPH(7, 12, &pin_PH14),
    PERIPH(8, 12, &pin_PI00),
    PERIPH(7, 12, &pin_PH02),
    PERIPH(7, 12, &pin_PH03),
    PERIPH(5, 12, &pin_PF02),
    PERIPH(5, 12, &pin_PF01),
    PERIPH(6, 12, &pin_PG08),
    PERIPH(5, 12, &pin_PF03),
    PERIPH(5, 12, &pin_PF04),
    PERIPH(7, 12, &pin_PH05),
    PERIPH(5, 12, &pin_PF05),
    PERIPH(7, 12, &pin_PH12),
    PERIPH(6, 12, &pin_PG05),
    PERIPH(6, 12, &pin_PG04),
    PERIPH(7, 12, &pin_PH11),
    PERIPH(7, 12, &pin_PH10),
    PERIPH(3, 12, &pin_PD15),
    PERIPH(6, 12, &pin_PG02),
    PERIPH(6, 12, &pin_PG01),
    PERIPH(7, 12, &pin_PH08),
    PERIPH(7, 12, &pin_PH09),
    PERIPH(3, 12, &pin_PD14),
    PERIPH(5, 12, &pin_PF13),
    PERIPH(6, 12, &pin_PG00),
    PERIPH(4, 12, &pin_PE13),
    PERIPH(3, 12, &pin_PD10),
    PERIPH(5, 12, &pin_PF12),
    PERIPH(5, 12, &pin_PF15),
    PERIPH(4, 12, &pin_PE08),
    PERIPH(4, 12, &pin_PE09),
    PERIPH(4, 12, &pin_PE11),
    PERIPH(4, 12, &pin_PE14),
    PERIPH(3, 12, &pin_PD09),
    PERIPH(3, 12, &pin_PD08),
    PERIPH(5, 12, &pin_PF11),
    PERIPH(5, 12, &pin_PF14),
    PERIPH(4, 12, &pin_PE07),
    PERIPH(4, 12, &pin_PE10),
    PERIPH(4, 12, &pin_PE12),
    PERIPH(4, 12, &pin_PE15),
};
