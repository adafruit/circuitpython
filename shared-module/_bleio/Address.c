// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/obj.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-module/_bleio/Address.h"

void common_hal_bleio_address_construct(bleio_address_obj_t *self, const uint8_t *bytes, uint8_t address_type) {
    memcpy(self->bytes, bytes, NUM_BLEIO_ADDRESS_BYTES);
    self->type = address_type;
}

mp_obj_t common_hal_bleio_address_get_address_bytes(bleio_address_obj_t *self) {
    // Build a bytes object on demand. Address stores its bytes inline, so this
    // is only reached from Python (e.g. ``address.address_bytes``) where the
    // heap is available.
    return mp_obj_new_bytes(self->bytes, NUM_BLEIO_ADDRESS_BYTES);
}

void common_hal_bleio_address_get_bytes(bleio_address_obj_t *self, uint8_t bytes[NUM_BLEIO_ADDRESS_BYTES]) {
    memcpy(bytes, self->bytes, NUM_BLEIO_ADDRESS_BYTES);
}

uint8_t common_hal_bleio_address_get_type(bleio_address_obj_t *self) {
    return self->type;
}
