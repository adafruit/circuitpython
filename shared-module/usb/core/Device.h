// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2025 Sam Blenny
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t device_address;
    uint8_t configuration_index; // not bConfigurationValue
    uint8_t *configuration_descriptor; // Contains the length of the all descriptors.
    uint8_t open_endpoints[8];
    uint16_t first_langid;
} usb_core_device_obj_t;


// Number of ms to wait in a background task loop after a TinyUSB tuh_* api call
// returns a failure result. This delay is part of a kludge to help TinyUSB
// recognize when a device has been unplugged. Without the delay, usb.core.find()
// can get super broken. This mechanism can be removed once somebody finds and
// fixes whatever is causing TinyUSB to get confused about unplugged devices.
#define USB_CORE_TUH_FAIL_WAIT_MS (15)

// These values get used to set USBError.errno and USBTimeoutError.errno.
// It would be possible to define these to more closely mimic desktop PyUSB on
// a given OS (maybe Debian?). But, for USB error handling in CircuitPython,
// there's an argument to be made that it's more useful to set arbitrary codes
// here that map directly to errors coming from TinyUSB. That way, CircuitPython
// code can have more visibility into what went wrong. POSIX errno codes are
// pretty far removed from the details of how TinyUSB can fail, so using them
// here would be uninformative.

// Error due to attempting to open endpoint before setting configuration
#define USB_CORE_NOCONFIG (1)

// Errors from transfer callback result
#define USB_CORE_NULL_PTR  (2)
#define USB_CORE_XFER_FAIL (3)
#define USB_CORE_STALLED   (4)
#define USB_CORE_TIMEOUT   (5)
#define USB_CORE_INVALID   (6)

// Errors from a non-callback TinyUSB function returning false. Seeing one of
// these probably means a TU_VERIFY(...) check failed in the TinyUSB code.
#define USB_CORE_EDPT_XFER     (7)
#define USB_CORE_OPEN_ENDPOINT (8)
#define USB_CORE_CONTROL_XFER  (9)
#define USB_CORE_VID_PID_GET  (10)
