// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 snkYmkrct
//
// SPDX-License-Identifier: MIT

#pragma once

// I2C
extern I2C_TypeDef *mcu_i2c_banks[MAX_I2C];

extern const mcu_periph_obj_t mcu_i2c_sda_list[12];
extern const mcu_periph_obj_t mcu_i2c_scl_list[12];

// SPI
extern SPI_TypeDef *mcu_spi_banks[MAX_SPI];

extern const mcu_periph_obj_t mcu_spi_sck_list[18];
extern const mcu_periph_obj_t mcu_spi_mosi_list[18];
extern const mcu_periph_obj_t mcu_spi_miso_list[15];

// UART
extern USART_TypeDef *mcu_uart_banks[MAX_UART];
extern bool mcu_uart_has_usart[MAX_UART];

extern const mcu_periph_obj_t mcu_uart_tx_list[24];
extern const mcu_periph_obj_t mcu_uart_rx_list[25];

// Timers
#define TIM_PIN_ARRAY_LEN 65
extern const mcu_tim_pin_obj_t mcu_tim_pin_list[TIM_PIN_ARRAY_LEN];
#define TIM_BANK_ARRAY_LEN 17
extern TIM_TypeDef *mcu_tim_banks[TIM_BANK_ARRAY_LEN];

// SDIO - H750 has a MMC interface that includes SDIO
extern SDMMC_TypeDef *mcu_sdio_banks[1];

extern const mcu_periph_obj_t mcu_sdio_clock_list[1];
extern const mcu_periph_obj_t mcu_sdio_command_list[1];
extern const mcu_periph_obj_t mcu_sdio_data0_list[1];
extern const mcu_periph_obj_t mcu_sdio_data1_list[1];
extern const mcu_periph_obj_t mcu_sdio_data2_list[1];
extern const mcu_periph_obj_t mcu_sdio_data3_list[1];
// SDRam
extern const mcu_periph_obj_t sdram_pin_list[57];
