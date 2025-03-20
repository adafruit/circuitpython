// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "py/obj.h"

extern const mp_obj_module_t usb_util_module;

#define PYUSB_SPEED_LOW 1
#define PYUSB_SPEED_FULL 2
#define PYUSB_SPEED_HIGH 3
