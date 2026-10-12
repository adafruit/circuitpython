// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#include "samd/sercom.h"
#include "common-hal/busio/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

static bool never_reset_sercoms[SERCOM_INST_NUM];

void never_reset_sercom(Sercom *sercom) {
    // Reset all SERCOMs except the ones being used by on-board devices.
    Sercom *sercom_instances[SERCOM_INST_NUM] = SERCOM_INSTS;
    for (int i = 0; i < SERCOM_INST_NUM; i++) {
        if (sercom_instances[i] == sercom) {
            never_reset_sercoms[i] = true;
            break;
        }
    }
}

void allow_reset_sercom(Sercom *sercom) {
    // Reset all SERCOMs except the ones being used by on-board devices.
    Sercom *sercom_instances[SERCOM_INST_NUM] = SERCOM_INSTS;
    for (int i = 0; i < SERCOM_INST_NUM; i++) {
        if (sercom_instances[i] == sercom) {
            never_reset_sercoms[i] = false;
            break;
        }
    }
}

void reset_sercoms(void) {
    // Reset all SERCOMs except the ones being used by on-board devices.
    Sercom *sercom_instances[SERCOM_INST_NUM] = SERCOM_INSTS;
    for (int i = 0; i < SERCOM_INST_NUM; i++) {
        if (never_reset_sercoms[i]) {
            continue;
        }
        // SWRST is same for all modes of SERCOMs.
        sercom_instances[i]->SPI.CTRLA.bit.SWRST = 1;
    }
}

uint8_t sercom_setup_pin(const mcu_pin_obj_t *pin, uint32_t pinmux, enum gpio_direction direction,
    bool strong_drive) {
    gpio_set_pin_direction(pin->number, direction);
    gpio_set_pin_pull_mode(pin->number, GPIO_PULL_OFF);
    gpio_set_pin_function(pin->number, pinmux);
    if (strong_drive) {
        hri_port_set_PINCFG_DRVSTR_bit(PORT, (enum gpio_port)GPIO_PORT(pin->number), GPIO_PIN(pin->number));
    }
    claim_pin(pin);
    return pin->number;
}
