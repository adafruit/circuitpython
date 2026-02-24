// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"

uint common_hal_busio_dma_i2c_read(busio_i2c_obj_t *i2c, uint8_t address, uint8_t *data, size_t len, bool nostop);
uint common_hal_busio_dma_i2c_write(busio_i2c_obj_t *i2c, uint8_t address, const uint8_t *data, size_t len, bool nostop);
bool common_hal_busio_dma_i2c_is_busy(uint dma_channel);

uint common_hal_busio_dma_spi_write(busio_spi_obj_t *spi, const uint8_t *data, size_t len);
uint common_hal_busio_dma_spi_read(busio_spi_obj_t *spi, uint8_t write_value, uint8_t *data, size_t len);
uint common_hal_busio_dma_spi_transfer(busio_spi_obj_t *spi, const uint8_t *out_data, uint8_t *in_data, size_t len);
bool common_hal_busio_dma_spi_is_busy(uint dma_channel);

uint common_hal_busio_dma_uart_read(busio_uart_obj_t *uart, uint8_t *data, size_t len);
uint common_hal_busio_dma_uart_write(busio_uart_obj_t *uart, const uint8_t *data, size_t len);
bool common_hal_busio_dma_uart_is_busy(uint dma_channel);
