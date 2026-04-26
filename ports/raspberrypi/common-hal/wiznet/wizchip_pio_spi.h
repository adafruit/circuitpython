/*
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _WIZNET_SPI_FUNCS_H_
#define _WIZNET_SPI_FUNCS_H_

#include <stdint.h>

typedef struct wiznet_pio_spi_funcs **wiznet_pio_spi_handle_t;
#if CIRCUITPY_WIZNET_W6300
typedef struct wiznet_pio_spi_config
{
    uint16_t clock_div_major;
    uint8_t clock_div_minor;
    uint8_t clock_pin;
    uint8_t data_io0_pin;
    uint8_t data_io1_pin;
    uint8_t data_io2_pin;
    uint8_t data_io3_pin;
    uint8_t cs_pin;
    uint8_t reset_pin;
    uint8_t irq_pin;
} wiznet_pio_spi_config_t;

typedef struct wiznet_pio_spi_funcs {
    void (*close)(wiznet_pio_spi_handle_t funcs);
    void (*set_active)(wiznet_pio_spi_handle_t funcs);
    void (*set_inactive)(void);
    void (*frame_start)(void);
    void (*frame_end)(void);
    bool (*read_byte)(uint8_t op_code, uint16_t AddrSel, uint8_t *rx, uint16_t rx_length);
    bool (*write_byte)(uint8_t op_code, uint16_t AddrSel, const uint8_t *tx, uint16_t tx_length);
    void (*read_buffer)(uint8_t *pBuf, uint16_t len);
    void (*write_buffer)(const uint8_t *pBuf, uint16_t len);
    void (*reset)(wiznet_pio_spi_handle_t funcs);
} wiznet_pio_spi_funcs_t;
#else // W55RP20
typedef struct wiznet_pio_spi_config {
    uint8_t data_in_pin;
    uint8_t data_out_pin;
    uint8_t cs_pin;
    uint8_t clock_pin;
    uint8_t irq_pin;
    uint8_t reset_pin;
    uint16_t clock_div_major;
    uint8_t clock_div_minor;
    uint8_t spi_hw_instance;
} wiznet_pio_spi_config_t;

typedef struct wiznet_pio_spi_funcs {
    void (*close)(wiznet_pio_spi_handle_t funcs);
    void (*set_active)(wiznet_pio_spi_handle_t funcs);
    void (*set_inactive)(void);
    void (*frame_start)(void);
    void (*frame_end)(void);
    void (*read_buffer)(uint8_t *pBuf, uint16_t len);
    void (*write_buffer)(const uint8_t *pBuf, uint16_t len);
    void (*reset)(wiznet_pio_spi_handle_t funcs);
} wiznet_pio_spi_funcs_t;
#endif

#endif

#ifndef _WIZNET_SPI_PIO_H_
#define _WIZNET_SPI_PIO_H_

wiznet_pio_spi_handle_t wiznet_pio_spi_open(const wiznet_pio_spi_config_t *pio_spi_config);
void wiznet_pio_spi_close(wiznet_pio_spi_handle_t handle);
#if CIRCUITPY_WIZNET_W6300
bool wiznet_pio_spi_write_byte(uint8_t op_code, uint16_t AddrSel, const uint8_t *tx, uint16_t tx_length);
bool wiznet_pio_spi_read_byte(uint8_t op_code, uint16_t AddrSel, uint8_t *rx, uint16_t rx_length);
#else // W55RP20
bool wiznet_pio_spi_transfer(const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length);
#endif
void wiznet_pio_spi_read_buffer(uint8_t *pBuf, uint16_t len);
void wiznet_pio_spi_write_buffer(const uint8_t *pBuf, uint16_t len);

#endif
