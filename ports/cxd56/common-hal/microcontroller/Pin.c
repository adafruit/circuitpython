// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include <unistd.h>

#include <sys/ioctl.h>
#include <arch/chip/pin.h>
#include <arch/chip/adc.h>
#include <arch/board/board.h>

#include "shared-bindings/microcontroller/Pin.h"

typedef struct {
    const mcu_pin_obj_t *pin;
    bool free;
} pin_status_t;

const mcu_pin_obj_t pin_UART2_RXD = PIN(PIN_UART2_RXD, false);
const mcu_pin_obj_t pin_UART2_TXD = PIN(PIN_UART2_TXD, false);
const mcu_pin_obj_t pin_HIF_IRQ_OUT = PIN(PIN_HIF_IRQ_OUT, false);
const mcu_pin_obj_t pin_PWM3 = PIN(PIN_PWM3, false);
const mcu_pin_obj_t pin_SPI2_MOSI = PIN(PIN_SPI2_MOSI, false);
const mcu_pin_obj_t pin_PWM1 = PIN(PIN_PWM1, false);
const mcu_pin_obj_t pin_PWM0 = PIN(PIN_PWM0, false);
const mcu_pin_obj_t pin_SPI3_CS1_X = PIN(PIN_SPI3_CS1_X, false);
const mcu_pin_obj_t pin_SPI2_MISO = PIN(PIN_SPI2_MISO, false);
const mcu_pin_obj_t pin_PWM2 = PIN(PIN_PWM2, false);
const mcu_pin_obj_t pin_SPI4_CS_X = PIN(PIN_SPI4_CS_X, false);
const mcu_pin_obj_t pin_SPI4_MOSI = PIN(PIN_SPI4_MOSI, false);
const mcu_pin_obj_t pin_SPI4_MISO = PIN(PIN_SPI4_MISO, false);
const mcu_pin_obj_t pin_SPI4_SCK = PIN(PIN_SPI4_SCK, false);
const mcu_pin_obj_t pin_I2C0_BDT = PIN(PIN_I2C0_BDT, false);
const mcu_pin_obj_t pin_I2C0_BCK = PIN(PIN_I2C0_BCK, false);
const mcu_pin_obj_t pin_EMMC_DATA0 = PIN(PIN_EMMC_DATA0, false);
const mcu_pin_obj_t pin_EMMC_DATA1 = PIN(PIN_EMMC_DATA1, false);
const mcu_pin_obj_t pin_I2S0_DATA_OUT = PIN(PIN_I2S0_DATA_OUT, false);
const mcu_pin_obj_t pin_I2S0_DATA_IN = PIN(PIN_I2S0_DATA_IN, false);
const mcu_pin_obj_t pin_EMMC_DATA2 = PIN(PIN_EMMC_DATA2, false);
const mcu_pin_obj_t pin_EMMC_DATA3 = PIN(PIN_EMMC_DATA3, false);
const mcu_pin_obj_t pin_SEN_IRQ_IN = PIN(PIN_SEN_IRQ_IN, false);
const mcu_pin_obj_t pin_EMMC_CLK = PIN(PIN_EMMC_CLK, false);
const mcu_pin_obj_t pin_EMMC_CMD = PIN(PIN_EMMC_CMD, false);
const mcu_pin_obj_t pin_I2S0_LRCK = PIN(PIN_I2S0_LRCK, false);
const mcu_pin_obj_t pin_I2S0_BCK = PIN(PIN_I2S0_BCK, false);
const mcu_pin_obj_t pin_UART2_CTS = PIN(PIN_UART2_CTS, false);
const mcu_pin_obj_t pin_UART2_RTS = PIN(PIN_UART2_RTS, false);
const mcu_pin_obj_t pin_I2S1_BCK = PIN(PIN_I2S1_BCK, false);
const mcu_pin_obj_t pin_I2S1_LRCK = PIN(PIN_I2S1_LRCK, false);
const mcu_pin_obj_t pin_I2S1_DATA_IN = PIN(PIN_I2S1_DATA_IN, false);
const mcu_pin_obj_t pin_I2S1_DATA_OUT = PIN(PIN_I2S1_DATA_OUT, false);
const mcu_pin_obj_t pin_SDIO_CLK = PIN(PIN_SDIO_CLK, false);
const mcu_pin_obj_t pin_SDIO_CMD = PIN(PIN_SDIO_CMD, false);
const mcu_pin_obj_t pin_SDIO_DATA0 = PIN(PIN_SDIO_DATA0, false);
const mcu_pin_obj_t pin_SDIO_DATA1 = PIN(PIN_SDIO_DATA1, false);
const mcu_pin_obj_t pin_SDIO_DATA2 = PIN(PIN_SDIO_DATA2, false);
const mcu_pin_obj_t pin_SDIO_DATA3 = PIN(PIN_SDIO_DATA3, false);
const mcu_pin_obj_t pin_LPADC0 = PIN(0, true);
const mcu_pin_obj_t pin_LPADC1 = PIN(1, true);
const mcu_pin_obj_t pin_LPADC2 = PIN(2, true);
const mcu_pin_obj_t pin_LPADC3 = PIN(3, true);
const mcu_pin_obj_t pin_HPADC0 = PIN(4, true);
const mcu_pin_obj_t pin_HPADC1 = PIN(5, true);

static pin_status_t pins[] = {
    { &pin_UART2_RXD, true },
    { &pin_UART2_TXD, true },
    { &pin_HIF_IRQ_OUT, true },
    { &pin_PWM3, true },
    { &pin_SPI2_MOSI, true },
    { &pin_PWM1, true },
    { &pin_PWM0, true },
    { &pin_SPI3_CS1_X, true },
    { &pin_SPI2_MISO, true },
    { &pin_PWM2, true },
    { &pin_SPI4_CS_X, true },
    { &pin_SPI4_MOSI, true },
    { &pin_SPI4_MISO, true },
    { &pin_SPI4_SCK, true },
    { &pin_I2C0_BDT, true },
    { &pin_I2C0_BCK, true },
    { &pin_EMMC_DATA0, true },
    { &pin_EMMC_DATA1, true },
    { &pin_I2S0_DATA_OUT, true },
    { &pin_I2S0_DATA_IN, true },
    { &pin_EMMC_DATA2, true },
    { &pin_EMMC_DATA3, true },
    { &pin_SEN_IRQ_IN, true },
    { &pin_EMMC_CLK, true },
    { &pin_EMMC_CMD, true },
    { &pin_I2S0_LRCK, true },
    { &pin_I2S0_BCK, true },
    { &pin_UART2_CTS, true },
    { &pin_UART2_RTS, true },
    { &pin_I2S1_BCK, true },
    { &pin_I2S1_LRCK, true },
    { &pin_I2S1_DATA_IN, true },
    { &pin_I2S1_DATA_OUT, true },
};

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    for (int i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (pins[i].pin->number == pin->number) {
            return pins[i].free;
        }
    }

    return true;
}

void reset_pin_number(uint8_t pin_number) {
    for (int i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (pins[i].pin->number == pin_number) {
            pins[i].free = true;
        }
    }
}


void claim_pin(const mcu_pin_obj_t *pin) {
    for (int i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (pins[i].pin->number == pin->number) {
            pins[i].free = false;
        }
    }
}
