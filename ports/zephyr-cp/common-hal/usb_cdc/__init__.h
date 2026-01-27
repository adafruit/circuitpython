// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mpconfig.h"
#include "py/objtuple.h"
#include "supervisor/usb.h"

bool usb_cdc_console_enabled(void);
bool usb_cdc_data_enabled(void);

void usb_cdc_set_defaults(void);
