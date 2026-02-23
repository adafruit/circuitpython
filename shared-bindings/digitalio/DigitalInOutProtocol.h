// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"
#include "py/mperrno.h"
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"

// Protocol structure for DigitalInOut implementations
// Note: mcu_pin_obj_t and digitalinout_result_t are defined by files that include this header
typedef struct _digitalinout_p_t {
    MP_PROTOCOL_HEAD // MP_QSTR_DigitalInOut
    void (*deinit)(mp_obj_t self);
    bool (*deinited)(mp_obj_t self);
    digitalinout_result_t (*switch_to_input)(mp_obj_t self, digitalio_pull_t pull);
    digitalinout_result_t (*switch_to_output)(mp_obj_t self, bool value, digitalio_drive_mode_t drive_mode);
    digitalio_direction_t (*get_direction)(mp_obj_t self);
    mp_negative_errno_t (*set_value)(mp_obj_t self, bool value); // Return 0 if ok
    mp_negative_errno_t (*get_value)(mp_obj_t self, bool *value); // Return 0 if ok
    digitalinout_result_t (*set_drive_mode)(mp_obj_t self, digitalio_drive_mode_t drive_mode);
    digitalio_drive_mode_t (*get_drive_mode)(mp_obj_t self);
    digitalinout_result_t (*set_pull)(mp_obj_t self, digitalio_pull_t pull);
    digitalio_pull_t (*get_pull)(mp_obj_t self);
} digitalinout_p_t;

// Protocol helper functions
// These functions work with any object that implements the DigitalInOut protocol,
// either through native C protocol or Python attributes/methods.

// Converts a Pin or DigitalInOutProtocol to a DigitalInOutProtocol object.
// If pin_or_dio is a Pin, allocates and initializes a DigitalInOut object.
// If pin_or_dio is already a DigitalInOutProtocol, returns it directly.
// If allow_none is true and pin_or_dio is None, returns None.
// If force_port_allocation is true, uses port_malloc instead of GC allocation.
// Sets *out_owns_pin to true if a new DigitalInOut was allocated (caller must deinit and free).
// Returns the DigitalInOutProtocol object to use.
// Raises an exception on error.
// Note: To free allocated objects, deinit first, then use gc_ptr_on_heap() to determine
// if port_free() should be called (if not on heap) or let GC handle it (if on heap).
mp_obj_t digitalinout_protocol_from_pin(
    mp_obj_t pin_or_dio,
    qstr arg_name,
    bool allow_none,
    bool force_port_allocation,
    bool *out_owns_pin);

#if CIRCUITPY_DIGITALINOUT_PROTOCOL
// Protocol helper functions that do protocol lookup or Python fallback
void digitalinout_protocol_deinit(mp_obj_t self);
bool digitalinout_protocol_deinited(mp_obj_t self);
digitalinout_result_t digitalinout_protocol_switch_to_input(mp_obj_t self, digitalio_pull_t pull);
digitalinout_result_t digitalinout_protocol_switch_to_output(mp_obj_t self, bool value, digitalio_drive_mode_t drive_mode);
digitalio_direction_t digitalinout_protocol_get_direction(mp_obj_t self);
mp_negative_errno_t digitalinout_protocol_set_value(mp_obj_t self, bool value);
mp_negative_errno_t digitalinout_protocol_get_value(mp_obj_t self, bool *value);
digitalinout_result_t digitalinout_protocol_set_drive_mode(mp_obj_t self, digitalio_drive_mode_t drive_mode);
digitalio_drive_mode_t digitalinout_protocol_get_drive_mode(mp_obj_t self);
digitalinout_result_t digitalinout_protocol_set_pull(mp_obj_t self, digitalio_pull_t pull);
digitalio_pull_t digitalinout_protocol_get_pull(mp_obj_t self);
#else
// When protocol is disabled, map directly to native DigitalInOut functions
#define digitalinout_protocol_deinit digitalinout_deinit
#define digitalinout_protocol_deinited digitalinout_deinited
#define digitalinout_protocol_switch_to_input digitalinout_switch_to_input
#define digitalinout_protocol_switch_to_output digitalinout_switch_to_output
#define digitalinout_protocol_get_direction digitalinout_get_direction
#define digitalinout_protocol_set_value digitalinout_set_value
#define digitalinout_protocol_get_value digitalinout_get_value
#define digitalinout_protocol_set_drive_mode digitalinout_set_drive_mode
#define digitalinout_protocol_get_drive_mode digitalinout_get_drive_mode
#define digitalinout_protocol_set_pull digitalinout_set_pull
#define digitalinout_protocol_get_pull digitalinout_get_pull
#endif
