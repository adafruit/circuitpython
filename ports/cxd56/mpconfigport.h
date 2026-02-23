// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_PY_SYS_PLATFORM                 "CXD56"

// SD card socket on board is configured for sdioio, which is not supported for automatic USB presentation.
#define CIRCUITPY_SDCARD_USB (0)

// 64kiB stack
#define CIRCUITPY_DEFAULT_STACK_SIZE            (0x10000)

// CXD56 architecture uses fixed endpoint numbers.
// Override default definitions in circuitpy_mpconfig.h,
// so define these before #include'ing that file.
#define USB_CDC_EP_NUM_NOTIFICATION (3)
#define USB_CDC_EP_NUM_DATA_OUT (2)
#define USB_CDC_EP_NUM_DATA_IN (1)
#define USB_MSC_EP_NUM_OUT (5)
#define USB_MSC_EP_NUM_IN (4)

#define CIRCUITPY_USB_DEVICE_HIGH_SPEED (1)

#include "py/circuitpy_mpconfig.h"

#define MICROPY_BYTES_PER_GC_BLOCK              (32)
