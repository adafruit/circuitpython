/*
 * This file is part of Adafruit for EFR32 project
 */
#ifndef MICROPY_INCLUDED_EFR32_COMMON_HAL_DIGITALIO_DIGITALINOUT_H
#define MICROPY_INCLUDED_EFR32_COMMON_HAL_DIGITALIO_DIGITALINOUT_H

#include "common-hal/microcontroller/Pin.h"
#include "em_gpio.h"

typedef struct
{
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
} digitalio_digitalinout_obj_t;

#endif // MICROPY_INCLUDED_STM32_COMMON_HAL_DIGITALIO_DIGITALINOUT_H
