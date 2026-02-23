// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/usb.h"

#if CIRCUITPY_STORAGE
#include "shared-module/storage/__init__.h"
#endif

#if CIRCUITPY_USB_DEVICE
#include "shared-bindings/supervisor/__init__.h"

#if CIRCUITPY_USB_CDC
#include "shared-module/usb_cdc/__init__.h"
#endif

#if CIRCUITPY_USB_HID
#include "shared-module/usb_hid/__init__.h"
#endif

#if CIRCUITPY_USB_MIDI
#include "shared-module/usb_midi/__init__.h"
#endif

#if CIRCUITPY_USB_VIDEO
#include "shared-module/usb_video/__init__.h"
#endif
#endif

// Set up USB defaults before any USB changes are made in boot.py
void usb_set_defaults(void) {
    #if CIRCUITPY_USB_DEVICE
    #if CIRCUITPY_STORAGE && CIRCUITPY_USB_MSC
    storage_usb_set_defaults();
    #endif

    #if CIRCUITPY_USB_CDC
    usb_cdc_set_defaults();
    #endif

    #if CIRCUITPY_USB_HID
    usb_hid_set_defaults();
    #endif

    #if CIRCUITPY_USB_MIDI
    usb_midi_set_defaults();
    #endif
    #endif
};

// Call this when ready to run code.py or a REPL, and a VM has been started.
void usb_setup_with_vm(void) {
    #if CIRCUITPY_USB_DEVICE
    #if CIRCUITPY_USB_HID
    usb_hid_setup_devices();
    #endif

    #if CIRCUITPY_USB_MIDI
    usb_midi_setup_ports();
    #endif
    #endif
}
