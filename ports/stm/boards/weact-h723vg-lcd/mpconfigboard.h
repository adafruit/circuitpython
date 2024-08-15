// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "WeAct H723VG LCD"
#define MICROPY_HW_MCU_NAME         "STM32H723"

#define FLASH_PAGE_SIZE             (0x4000)

// H7 and F7 MPU definitions
#define CPY_FLASH_REGION_SIZE   ARM_MPU_REGION_SIZE_1MB
#define CPY_ITCM_REGION_SIZE    ARM_MPU_REGION_SIZE_64KB
#define CPY_DTCM_REGION_SIZE    ARM_MPU_REGION_SIZE_128KB
#define CPY_SRAM_REGION_SIZE    ARM_MPU_REGION_SIZE_320KB
#define CPY_SRAM_SUBMASK        0x00
#define CPY_SRAM_START_ADDR     0x24000000
#define CFG_TUSB_OS OPT_OS_NONE

#define HSE_VALUE ((uint32_t)25000000)
#define LSE_VALUE ((uint32_t)32768)

#define BOARD_HSE_SOURCE (RCC_HSE_ON)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)
#define BOARD_NO_VBUS_SENSE 1
#define BOARD_NO_USB_OTG_ID_SENSE 1

#define MICROPY_HW_LED_STATUS (&pin_PE03)

// on-board SPI flash
#define SPI_FLASH_MOSI_PIN (&pin_PD07)
#define SPI_FLASH_MISO_PIN (&pin_PB04)
#define SPI_FLASH_SCK_PIN  (&pin_PB03)
#define SPI_FLASH_CS_PIN   (&pin_PD06)

#if QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0  (&pin_PD11)
#define MICROPY_QSPI_DATA1  (&pin_PD12)
#define MICROPY_QSPI_DATA2  (&pin_PE02)
#define MICROPY_QSPI_DATA3  (&pin_PD13)
#define MICROPY_QSPI_SCK    (&pin_PB02)
#define MICROPY_QSPI_CS     (&pin_PB06)
#endif

// usb?
#define IGNORE_PIN_PA11 1
#define IGNORE_PIN_PA12 1

#define DEFAULT_I2C_BUS_SCL (&pin_PB08)
#define DEFAULT_I2C_BUS_SDA (&pin_PB09)

#define DEFAULT_SPI_BUS_SCK (&pin_PB13)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB15)
#define DEFAULT_SPI_BUS_MISO (&pin_PB14)

#define DEFAULT_UART_BUS_RX (&pin_PA10)
#define DEFAULT_UART_BUS_TX (&pin_PA09)
