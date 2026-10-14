/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "py/mphal.h"
#include "em_gpio.h"

#define GPIO_PORT_COUNT (MP_ARRAY_SIZE(ports))

GPIO_Port_TypeDef ports[] = {gpioPortA, gpioPortB, gpioPortC, gpioPortD};
static uint16_t claimed_pins[GPIO_PORT_COUNT];

//  Mark pin as free and return it to a quiescent state.
void reset_pin_number(uint8_t pin_port, uint8_t pin_number) {
    // Clear claimed bit & reset
    claimed_pins[pin_port] &= ~(1 << pin_number);
    GPIO_PinModeSet(pin_port, pin_number, gpioModeInput, 1);
}

// Reset pin
void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->port, pin->number);
}

// Mark pin as in used
void claim_pin(uint8_t pin_port, uint8_t pin_number) {
    // Set bit in claimed_pins bitmask.
    claimed_pins[pin_port] |= 1 << pin_number;
}

// Check pin status free or in used
bool pin_number_is_free(uint8_t pin_port, uint8_t pin_number) {
    return !(claimed_pins[pin_port] & (1 << pin_number));
}

// Check pin status free or in used
bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->port, pin->number);
}

// Calculate pin number for a pin obj
uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->port * 16 + pin->number;
}

// Mark pin is in used
void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claim_pin(pin->port, pin->number);
}

// Reset pin by pin number
void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no / 16, pin_no % 16);
}
