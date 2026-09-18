// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"

#include "samd/pins.h"

#ifdef SPEAKER_ENABLE_PIN
bool speaker_enable_in_use;
#endif

#ifdef SAM_D5X_E5X
#define SWD_MUX GPIO_PIN_FUNCTION_H
#endif
#ifdef SAMD21
#define SWD_MUX GPIO_PIN_FUNCTION_G
#endif


void reset_pin_number(uint8_t pin_number) {
    if (pin_number >= PORT_BITS) {
        return;
    }

    if (pin_number == PIN_PA30
        #ifdef SAM_D5X_E5X
        ) {
        #endif
        #ifdef SAMD21
        || pin_number == PIN_PA31) {
        #endif
        gpio_set_pin_function(pin_number, SWD_MUX);
    } else {
        gpio_set_pin_direction(pin_number, GPIO_DIRECTION_OFF);
        gpio_set_pin_function(pin_number, GPIO_PIN_FUNCTION_OFF);
    }

    #ifdef SPEAKER_ENABLE_PIN
    if (pin_number == SPEAKER_ENABLE_PIN->number) {
        speaker_enable_in_use = false;
        gpio_set_pin_function(pin_number, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(SPEAKER_ENABLE_PIN->number, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(SPEAKER_ENABLE_PIN->number, false);
    }
    #endif
}


void common_hal_reset_pin(const mcu_pin_obj_t* pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->number);
}

void claim_pin(const mcu_pin_obj_t* pin) {
    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        speaker_enable_in_use = true;
    }
    #endif
}

bool pin_number_is_free(uint8_t pin_number) {
    PortGroup *const port = &PORT->Group[(enum gpio_port)GPIO_PORT(pin_number)];
    uint8_t pin_index = GPIO_PIN(pin_number);
    volatile PORT_PINCFG_Type *state = &port->PINCFG[pin_index];
    volatile PORT_PMUX_Type *pmux = &port->PMUX[pin_index / 2];

    if (pin_number == PIN_PA30 || pin_number == PIN_PA31) {
        if (DSU->STATUSB.bit.DBGPRES == 1) {
            return false;
        }
        if (pin_number == PIN_PA30
            #ifdef SAMD21
            || pin_number == PIN_PA31
            #endif
            )
            {
            return state->bit.PMUXEN == 1 && ((pmux->reg >> (4 * pin_index % 2)) & 0xf) == SWD_MUX;
        }
    }

    return state->bit.PMUXEN == 0 && state->bit.INEN == 0 &&
           state->bit.PULLEN == 0 && (port->DIR.reg & (1 << pin_index)) == 0;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t* pin) {
    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        return !speaker_enable_in_use;
    }
    #endif

    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t* pin) {
    return pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t* pin) {
    return claim_pin(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no);
}

mcu_pin_function_t *mcu_find_pin_function(mcu_pin_function_t *table, const mcu_pin_obj_t *pin, int instance, uint16_t name) {
    if (!pin) {
        return NULL;
    }

    for(; table->obj; table++) {
        if (instance != -1 && instance != table->instance) {
            continue;
        }
        if (pin == table->obj) {
            return table;
        }
    }
    mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q pin"), name);
}
