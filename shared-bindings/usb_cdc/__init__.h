// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#if defined(__ZEPHYR__) && __ZEPHYR__ == 1
#include "common-hal/usb_cdc/__init__.h"
#else
#include "shared-module/usb_cdc/__init__.h"
#endif

#if CIRCUITPY_FULL_BUILD
extern char *custom_usb_cdc_console_control_interface_name;
extern char *custom_usb_cdc_console_data_interface_name;
extern char *custom_usb_cdc_data_control_interface_name;
extern char *custom_usb_cdc_data_data_interface_name;
#endif

//  Set the module dict entries.
void usb_cdc_set_console(mp_obj_t serial_obj);
void usb_cdc_set_data(mp_obj_t serial_obj);

extern bool common_hal_usb_cdc_disable(void);
extern bool common_hal_usb_cdc_enable(bool console, bool data);
