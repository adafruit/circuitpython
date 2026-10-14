// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/mii/__init__.h"
#include "shared-bindings/mii/__init__.h"

void mii_user_reset(void) {
    // Nothing to do on user reset for now.
}

static mii_ethernet_obj_t *_default_instance;

// Called by board code once the board's ethernet interface is constructed.
void mii_set_default_instance(mii_ethernet_obj_t *self) {
    _default_instance = self;
}

mp_obj_t mii_get_default_instance(void) {
    if (_default_instance == NULL) {
        return NULL;
    }
    return MP_OBJ_FROM_PTR(_default_instance);
}
