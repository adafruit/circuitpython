// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/gc.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objtuple.h"
#include "shared-bindings/usb_cdc/__init__.h"
#include "shared-bindings/usb_cdc/Serial.h"
#include "supervisor/usb.h"

static bool usb_cdc_console_is_enabled;
static bool usb_cdc_data_is_enabled;

void usb_cdc_set_defaults(void) {
    common_hal_usb_cdc_enable(true,
        false);
}

bool usb_cdc_console_enabled(void) {
    return usb_cdc_console_is_enabled;
}

bool usb_cdc_data_enabled(void) {
    return usb_cdc_data_is_enabled;
}

bool common_hal_usb_cdc_disable(void) {
    return common_hal_usb_cdc_enable(false, false);
}

bool common_hal_usb_cdc_enable(bool console, bool data) {
    usb_cdc_console_is_enabled = console;
    usb_cdc_data_is_enabled = data;
    return true;
}
