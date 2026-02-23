// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef enum {
    SETTINGS_OK = 0,
    SETTINGS_ERR_OPEN,
    SETTINGS_ERR_UNICODE,
    SETTINGS_ERR_LENGTH,
    SETTINGS_ERR_NOT_FOUND,
    SETTINGS_ERR_BAD_VALUE,
} settings_err_t;

// Read a string value from the settings file.
// If it fits, the return value is 0-terminated. The passed-in buffer
// may be modified even if an error is returned. Allocation free.
// An error that is not 'open' or 'not found' is printed on the repl.
// Returns an error if the value is not a quoted string.
settings_err_t settings_get_str(const char *key, char *value, size_t value_len);

// Read an integer value from the settings file.
// Returns SETTINGS_OK and sets value to the read value. Returns
// SETTINGS_ERR_... if the value was not numeric. allocation-free.
// If any error code is returned, value is guaranteed not modified
// An error that is not 'open' or 'not found' is printed on the repl.
settings_err_t settings_get_int(const char *key, mp_int_t *value);

// Read a boolean value from the settings file.
// Returns SETTINGS_OK and sets value to the read value. Returns
// SETTINGS_ERR_... if the value was not a boolean (true or false). allocation-free.
// If any error code is returned, value is guaranteed not modified
// An error that is not 'open' or 'not found' is printed on the repl.
settings_err_t settings_get_bool(const char *key, bool *value);

// Read a value from the settings file and return as parsed Python object.
// Returns SETTINGS_OK and sets value to a parsed Python object from the RHS value:
// - Quoted strings return as str
// - Bare "true" or "false" return as bool
// - Valid integers return as int
// Returns SETTINGS_ERR_... if the value is not parseable as one of these types.
// An error that is not 'open' or 'not found' is printed on the repl.
settings_err_t settings_get_obj(const char *key, mp_obj_t *value);

// Read the raw value as a string, whether quoted or bare.
// This is used by os.getenv() to always return strings.
// Does not print errors.
settings_err_t settings_get_raw_vstr(const char *key, vstr_t *vstr);
