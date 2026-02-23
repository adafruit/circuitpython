// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "py/gc.h"
#include "py/misc.h"
#include "py/mpstate.h"
#include "py/mpprint.h"
#include "py/objstr.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/settings.h"

#define SETTINGS_PATH "/settings.toml"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#if CIRCUITPY_SETTINGS_TOML
typedef FIL file_arg;
static bool open_file(const char *name, file_arg *file_handle) {
    #if defined(UNIX)
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t file_obj = mp_call_function_2(
            MP_OBJ_FROM_PTR(&mp_builtin_open_obj), mp_obj_new_str(name, strlen(name)), MP_ROM_QSTR(MP_QSTR_rb));
        mp_arg_validate_type(file_obj, &mp_type_vfs_fat_fileio, MP_QSTR_file);
        pyb_file_obj_t *file = MP_OBJ_TO_PTR(file_obj);
        *file_handle = file->fp;
        nlr_pop();
        return true;
    } else {
        return false;
    }
    #else
    fs_user_mount_t *fs_mount = filesystem_circuitpy();
    if (fs_mount == NULL) {
        return false;
    }
    FATFS *fatfs = &fs_mount->fatfs;
    FRESULT result = f_open(fatfs, file_handle, name, FA_READ);
    return result == FR_OK;
    #endif
}

static void close_file(file_arg *file_handle) {
    f_close(file_handle);
}

static bool is_eof(file_arg *file_handle) {
    return f_eof(file_handle) || f_error(file_handle);
}

// Return 0 if there is no next character (EOF).
static uint8_t get_next_byte(FIL *file_handle) {
    uint8_t character = 0;
    UINT quantity_read;
    // If there's an error or quantity_read is 0, character will remain 0.
    f_read(file_handle, &character, 1, &quantity_read);
    return character;
}

static void seek_eof(file_arg *file_handle) {
    f_lseek(file_handle, f_size(file_handle));
}

// For a fixed buffer, record the required size rather than throwing
static void vstr_add_byte_nonstd(vstr_t *vstr, byte b) {
    if (!vstr->fixed_buf || vstr->alloc > vstr->len) {
        vstr_add_byte(vstr, b);
    } else {
        vstr->len++;
    }
}

// For a fixed buffer, record the required size rather than throwing
static void vstr_add_char_nonstd(vstr_t *vstr, unichar c) {
    size_t ulen =
        (c < 0x80) ? 1 :
        (c < 0x800) ? 2 :
        (c < 0x10000) ? 3 : 4;
    if (!vstr->fixed_buf || vstr->alloc > vstr->len + ulen) {
        vstr_add_char(vstr, c);
    } else {
        vstr->len += ulen;
    }
}

static void next_line(file_arg *file_handle) {
    uint8_t character;
    do {
        character = get_next_byte(file_handle);
    } while (character != 0 && character != '\n');
}

// Discard whitespace, except for newlines, returning the next character after the whitespace.
// Return 0 if there is no next character (EOF).
static uint8_t consume_whitespace(file_arg *file_handle) {
    uint8_t character;
    do {
        character = get_next_byte(file_handle);
    } while (character != '\n' && character != 0 && unichar_isspace(character));
    return character;
}

// Starting at the start of a new line, determines if the key matches the given key.
//
// If result is true, the key matches and file pointer is pointing just after the "=".
// If the result is false, the key does NOT match and the file pointer is
// pointing at the start of the next line, if any
static bool key_matches(file_arg *file_handle, const char *key) {
    uint8_t character;
    character = consume_whitespace(file_handle);
    // [section] isn't implemented, so skip to end of file.
    if (character == '[' || character == 0) {
        seek_eof(file_handle);
        return false;
    }
    while (*key) {
        if (character != *key++) {
            // A character didn't match the key, so it's not a match
            // If the non-matching char was not the end of the line,
            // then consume the rest of the line
            if (character != '\n') {
                next_line(file_handle);
            }
            return false;
        }
        character = get_next_byte(file_handle);
    }
    // the next character could be whitespace; consume if necessary
    if (unichar_isspace(character)) {
        character = consume_whitespace(file_handle);
    }
    // If we're not looking at the "=" then the key didn't match
    if (character != '=') {
        // A character didn't match the key, so it's not a match
        // If the non-matching char was not the end of the line,
        // then consume the rest of the line
        if (character != '\n') {
            next_line(file_handle);
        }
        return false;
    }
    return true;
}

static settings_err_t read_unicode_escape(file_arg *file_handle, int sz, vstr_t *vstr) {
    char hex_buf[sz + 1];
    for (int i = 0; i < sz; i++) {
        hex_buf[i] = get_next_byte(file_handle);
    }
    hex_buf[sz] = 0;
    char *end;
    unsigned long c = strtoul(hex_buf, &end, 16);
    if (end != &hex_buf[sz]) {
        return SETTINGS_ERR_BAD_VALUE;
    }
    if (c >= 0x110000) {
        return SETTINGS_ERR_UNICODE;
    }
    vstr_add_char_nonstd(vstr, c);
    return SETTINGS_OK;
}

// Read a quoted string
static settings_err_t read_string_value(file_arg *file_handle, vstr_t *vstr) {
    while (true) {
        int character = get_next_byte(file_handle);
        switch (character) {
            case 0:
            case '\n':
                return SETTINGS_ERR_BAD_VALUE;

            case '"':
                character = consume_whitespace(file_handle);
                switch (character) {
                    case '#':
                        next_line(file_handle);
                        MP_FALLTHROUGH;
                    case 0:
                    case '\n':
                        return SETTINGS_OK;
                    default:
                        return SETTINGS_ERR_BAD_VALUE;
                }

            case '\\':
                character = get_next_byte(file_handle);
                switch (character) {
                    case 0:
                    case '\n':
                        return SETTINGS_ERR_BAD_VALUE;
                    case 'b':
                        character = '\b';
                        break;
                    case 'r':
                        character = '\r';
                        break;
                    case 'n':
                        character = '\n';
                        break;
                    case 't':
                        character = '\t';
                        break;
                    case 'v':
                        character = '\v';
                        break;
                    case 'f':
                        character = '\f';
                        break;
                    case 'U':
                    case 'u': {
                        int sz = (character == 'u') ? 4 : 8;
                        settings_err_t res;
                        res = read_unicode_escape(file_handle, sz, vstr);
                        if (res != SETTINGS_OK) {
                            return res;
                        }
                        continue;
                    }
                        // default falls through, other escaped characters
                        // represent themselves
                }
                MP_FALLTHROUGH;
            default:
                vstr_add_byte_nonstd(vstr, character);
        }
    }
}

// Read a bare value (non-quoted value) as a string
// Trims leading and trailing spaces/tabs, stops at # comment or newline
static settings_err_t read_bare_value(file_arg *file_handle, vstr_t *vstr, int first_character) {
    int character = first_character;
    size_t trailing_space_count = 0;

    while (true) {
        switch (character) {
            case 0:
            case '\n':
                // Remove trailing spaces/tabs and \r
                vstr->len -= trailing_space_count;
                return SETTINGS_OK;
            case '#':
                // Remove trailing spaces/tabs and \r before comment
                vstr->len -= trailing_space_count;
                next_line(file_handle);
                return SETTINGS_OK;
            case ' ':
            case '\t':
            case '\r':
                // Track potential trailing whitespace
                vstr_add_byte_nonstd(vstr, character);
                trailing_space_count++;
                break;
            default:
                // Non-whitespace character resets trailing space count
                vstr_add_byte_nonstd(vstr, character);
                trailing_space_count = 0;
        }
        character = get_next_byte(file_handle);
    }
}

static mp_int_t read_value(file_arg *file_handle, vstr_t *vstr, bool *quoted) {
    uint8_t character;
    character = consume_whitespace(file_handle);
    *quoted = (character == '"');

    if (*quoted) {
        return read_string_value(file_handle, vstr);
    } else if (character == '\n' || character == 0) {
        // Empty value is an error
        return SETTINGS_ERR_BAD_VALUE;
    } else {
        return read_bare_value(file_handle, vstr, character);
    }
}

static settings_err_t settings_get_vstr(const char *key, vstr_t *vstr, bool *quoted) {
    file_arg file_handle;
    if (!open_file(SETTINGS_PATH, &file_handle)) {
        return SETTINGS_ERR_OPEN;
    }

    settings_err_t result = SETTINGS_ERR_NOT_FOUND;
    while (!is_eof(&file_handle)) {
        if (key_matches(&file_handle, key)) {
            result = read_value(&file_handle, vstr, quoted);
            break;
        }
    }
    close_file(&file_handle);
    return result;
}

static settings_err_t settings_get_buf_terminated(const char *key, char *value, size_t value_len, bool *quoted) {
    vstr_t vstr;
    vstr_init_fixed_buf(&vstr, value_len, value);
    settings_err_t result = settings_get_vstr(key, &vstr, quoted);

    if (result == SETTINGS_OK) {
        vstr_add_byte_nonstd(&vstr, 0);
        memcpy(value, vstr.buf, MIN(vstr.len, value_len));
        if (vstr.len > value_len) { // this length includes trailing NUL
            result = SETTINGS_ERR_LENGTH;
        }
    }
    return result;
}

static void print_error(const char *key, settings_err_t result) {
    switch (result) {
        case SETTINGS_OK:
        case SETTINGS_ERR_OPEN:
        case SETTINGS_ERR_NOT_FOUND:
            // These errors need not be printed.
            // The code asking for the value is not necessarily expecting to find one.
            return;
        default:
            mp_cprintf(&mp_plat_print, MP_ERROR_TEXT("An error occurred while retrieving '%s':\n"), key);
            break;
    }

    switch (result) {
        case SETTINGS_ERR_UNICODE:
            mp_cprintf(&mp_plat_print, MP_ERROR_TEXT("Invalid unicode escape"));
            break;
        case SETTINGS_ERR_BAD_VALUE:
            mp_cprintf(&mp_plat_print, MP_ERROR_TEXT("Invalid format"));
            break;
        default:
            mp_cprintf(&mp_plat_print, MP_ERROR_TEXT("Internal error"));
            break;
    }
    mp_printf(&mp_plat_print, "\n");
}


static settings_err_t get_str(const char *key, char *value, size_t value_len) {
    bool quoted;
    settings_err_t result = settings_get_buf_terminated(key, value, value_len, &quoted);
    if (result == SETTINGS_OK && !quoted) {
        result = SETTINGS_ERR_BAD_VALUE;
    }
    return result;
}

settings_err_t settings_get_str(const char *key, char *value, size_t value_len) {
    settings_err_t result = get_str(key, value, value_len);
    print_error(key, result);
    return result;
}

static settings_err_t get_int(const char *key, mp_int_t *value) {
    char buf[16];
    bool quoted;
    settings_err_t result = settings_get_buf_terminated(key, buf, sizeof(buf), &quoted);
    if (result != SETTINGS_OK) {
        return result;
    }
    if (quoted) {
        return SETTINGS_ERR_BAD_VALUE;
    }
    char *end;
    long num = strtol(buf, &end, 0);
    while (unichar_isspace(*end)) {
        end++;
    }
    if (end == buf || *end) { // If the whole buffer was not consumed it's an error
        return SETTINGS_ERR_BAD_VALUE;
    }
    *value = (mp_int_t)num;
    return SETTINGS_OK;
}

settings_err_t settings_get_int(const char *key, mp_int_t *value) {
    settings_err_t result = get_int(key, value);
    print_error(key, result);
    return result;
}

static settings_err_t get_bool(const char *key, bool *value) {
    char buf[16];
    bool quoted;
    settings_err_t result = settings_get_buf_terminated(key, buf, sizeof(buf), &quoted);
    if (result != SETTINGS_OK) {
        return result;
    }
    if (quoted) {
        return SETTINGS_ERR_BAD_VALUE;
    }

    // Check for "true" or "false" (case-sensitive)
    if (strcmp(buf, "true") == 0) {
        *value = true;
        return SETTINGS_OK;
    } else if (strcmp(buf, "false") == 0) {
        *value = false;
        return SETTINGS_OK;
    }

    // Not a valid boolean value
    return SETTINGS_ERR_BAD_VALUE;
}

settings_err_t settings_get_bool(const char *key, bool *value) {
    settings_err_t result = get_bool(key, value);
    print_error(key, result);
    return result;
}

// Get the raw value as a vstr, whether quoted or bare. Value may be an invalid TOML value.
settings_err_t settings_get_raw_vstr(const char *key, vstr_t *vstr) {
    bool quoted;
    return settings_get_vstr(key, vstr, &quoted);
}

settings_err_t settings_get_obj(const char *key, mp_obj_t *value) {
    vstr_t vstr;
    vstr_init(&vstr, 32);
    bool quoted;

    settings_err_t result = settings_get_vstr(key, &vstr, &quoted);
    if (result != SETTINGS_OK) {
        return result;
    }

    if (quoted) {
        // Successfully parsed a quoted string
        *value = mp_obj_new_str_from_vstr(&vstr);
        return SETTINGS_OK;
    }

    // Not a quoted string, try boolean
    bool bool_val;
    result = get_bool(key, &bool_val);
    if (result == SETTINGS_OK) {
        *value = mp_obj_new_bool(bool_val);
        return SETTINGS_OK;
    }

    // Not a boolean, try integer
    mp_int_t int_val;
    result = get_int(key, &int_val);
    if (result == SETTINGS_OK) {
        *value = mp_obj_new_int(int_val);
        return SETTINGS_OK;
    }

    return SETTINGS_ERR_BAD_VALUE;
}

#endif
