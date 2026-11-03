// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-bindings/digitalio/Pull.h"
#include "shared-bindings/microcontroller/Pin.h"

#if CIRCUITPY_PIN_CONFIGURATION

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    // A digitalio.Pull, or None to let the object receiving this configuration choose.
    mp_obj_t pull;
} mcu_pin_configuration_obj_t;

extern const mp_obj_type_t mcu_pin_configuration_type;

// Return the pin that obj names: the pin of a PinConfiguration, or obj itself otherwise.
// obj is not validated, so pass the result to a validate_obj_is_*pin*() function.
// If obj is a PinConfiguration whose pull is not None, and pull is not NULL, *pull is set
// to that pull. Otherwise *pull is not touched, so preload it with the caller's default.
mp_obj_t mcu_pin_configuration_unpack(mp_obj_t obj, digitalio_pull_t *pull);

#else

static inline mp_obj_t mcu_pin_configuration_unpack(mp_obj_t obj, digitalio_pull_t *pull) {
    (void)pull;
    return obj;
}

#endif
