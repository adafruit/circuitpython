// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016-2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include <stddef.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/objstr.h"

#include "shared/runtime/interrupt_char.h"
#include "supervisor/port.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/reload.h"
#include "supervisor/shared/traceback.h"
#include "supervisor/shared/workflow.h"

#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_IDENTIFICATION
#include "supervisor/usb.h"
#endif

#if CIRCUITPY_SETTINGS_TOML
#include "supervisor/shared/settings.h"
#endif

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/supervisor/Runtime.h"
#include "shared-bindings/supervisor/StatusBar.h"
#include "shared-bindings/util.h"

//| """Supervisor settings"""
//|

//| runtime: Runtime
//| """Runtime information, such as ``runtime.serial_connected``
//| (USB serial connection status).
//| This object is the sole instance of `supervisor.Runtime`."""

//| status_bar: StatusBar
//| """The status bar, shown on an attached display, and also sent to
//| an attached terminal via OSC escape codes over the REPL serial connection.
//| The status bar reports the current IP or BLE connection, what file is running,
//| the last exception name and location, and firmware version information.
//| This object is the sole instance of `supervisor.StatusBar`."""
//|
//|

//| def reload() -> None:
//|     """Reload the main Python code and run it (equivalent to hitting Ctrl-D at the REPL)."""
//|     ...
//|
//|
static mp_obj_t supervisor_reload(void) {
    reload_initiate(RUN_REASON_SUPERVISOR_RELOAD);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_reload_obj, supervisor_reload);

//| def set_next_code_file(
//|     filename: Optional[str],
//|     *,
//|     working_directory: Optional[str] = None,
//|     reload_on_success: bool = False,
//|     reload_on_error: bool = False,
//|     sticky_on_success: bool = False,
//|     sticky_on_error: bool = False,
//|     sticky_on_reload: bool = False,
//| ) -> None:
//|     """Set what file to run on the next vm run.
//|
//|     When not ``None``, the given ``filename`` is inserted at the front of the usual ['code.py',
//|     'main.py'] search sequence.
//|
//|     The optional keyword arguments specify what happens after the specified file has run:
//|
//|     ``sticky_on_…`` determine whether the newly set filename and options stay in effect: If
//|     True, further runs will continue to run that file (unless it says otherwise by calling
//|     ``set_next_code_filename()`` itself). If False, the settings will only affect one run and
//|     revert to the standard code.py/main.py afterwards.
//|
//|     ``reload_on_…`` determine how to continue: If False, wait in the usual "Code done running.
//|     Waiting for reload. / Press any key to enter the REPL. Use CTRL-D to reload." state. If
//|     True, reload immediately as if CTRL-D was pressed.
//|
//|     ``…_on_success`` take effect when the program runs to completion or calls ``sys.exit()``.
//|
//|     ``…_on_error`` take effect when the program exits with an exception, including the
//|     KeyboardInterrupt caused by CTRL-C.
//|
//|     ``…_on_reload`` take effect when the program is interrupted by files being written to the USB
//|     drive (auto-reload) or when it calls ``supervisor.reload()``.
//|
//|     These settings are stored in RAM, not in persistent memory, and will therefore only affect
//|     soft reloads. Powering off or resetting the device will always revert to standard settings.
//|
//|     When called multiple times in the same run, only the last call takes effect, replacing any
//|     settings made by previous ones. This is the main use of passing ``None`` as a filename: to
//|     reset to the standard search sequence."""
//|     ...
//|
//|
static mp_obj_t supervisor_set_next_code_file(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_filename, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_working_directory, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_reload_on_success, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_reload_on_error, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_sticky_on_success, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_sticky_on_error, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_sticky_on_reload, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    struct {
        mp_arg_val_t filename;
        mp_arg_val_t working_directory;
        mp_arg_val_t reload_on_success;
        mp_arg_val_t reload_on_error;
        mp_arg_val_t sticky_on_success;
        mp_arg_val_t sticky_on_error;
        mp_arg_val_t sticky_on_reload;
    } args;
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t *)&args);
    mp_obj_t filename_obj = args.filename.u_obj;
    if (!mp_obj_is_str_or_bytes(filename_obj) && filename_obj != mp_const_none) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), MP_QSTR_filename, MP_QSTR_str, MP_QSTR_None, mp_obj_get_type(filename_obj)->name);
    }

    mp_obj_t working_directory_obj = args.working_directory.u_obj;
    if (!mp_obj_is_str_or_bytes(working_directory_obj) && working_directory_obj != mp_const_none) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), MP_QSTR_working_directory, MP_QSTR_str, MP_QSTR_None, mp_obj_get_type(working_directory_obj)->name);
    }
    if (filename_obj == mp_const_none) {
        filename_obj = mp_const_empty_bytes;
    }
    uint8_t options = 0;
    if (args.reload_on_success.u_bool) {
        options |= SUPERVISOR_NEXT_CODE_OPT_RELOAD_ON_SUCCESS;
    }
    if (args.reload_on_error.u_bool) {
        options |= SUPERVISOR_NEXT_CODE_OPT_RELOAD_ON_ERROR;
    }
    if (args.sticky_on_success.u_bool) {
        options |= SUPERVISOR_NEXT_CODE_OPT_STICKY_ON_SUCCESS;
    }
    if (args.sticky_on_error.u_bool) {
        options |= SUPERVISOR_NEXT_CODE_OPT_STICKY_ON_ERROR;
    }
    if (args.sticky_on_reload.u_bool) {
        options |= SUPERVISOR_NEXT_CODE_OPT_STICKY_ON_RELOAD;
    }
    size_t len;
    const char *filename = mp_obj_str_get_data(filename_obj, &len);
    if (!path_exists(filename)) {
        mp_raise_ValueError(MP_ERROR_TEXT("File not found"));
    }

    size_t working_directory_len = 0;
    const char *working_directory = NULL;
    if (working_directory_obj != mp_const_none) {
        working_directory = mp_obj_str_get_data(working_directory_obj, &working_directory_len);
        if (!path_exists(working_directory)) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_working_directory);
        }
    }
    if (next_code_configuration != NULL) {
        port_free(next_code_configuration);
        next_code_configuration = NULL;
    }
    if (options != 0 || len != 0) {

        size_t next_code_size = sizeof(supervisor_next_code_info_t) + len + 1;
        if (working_directory_len > 0) {
            next_code_size += working_directory_len + 1;
        }
        next_code_configuration = port_malloc(next_code_size, false);
        if (next_code_configuration == NULL) {
            m_malloc_fail(next_code_size);
        }
        char *filename_ptr = (char *)next_code_configuration + sizeof(supervisor_next_code_info_t);

        // Copy filename
        memcpy(filename_ptr, filename, len);
        filename_ptr[len] = '\0';

        char *working_directory_ptr = NULL;
        // Copy working directory after filename if present
        if (working_directory_len > 0) {
            working_directory_ptr = filename_ptr + len + 1;
            memcpy(working_directory_ptr, working_directory, working_directory_len);
            working_directory_ptr[working_directory_len] = '\0';
        }
        // Set everything up last. We may have raised an exception early and we
        // don't want to free the memory if we failed.
        next_code_configuration->filename = filename_ptr;
        next_code_configuration->working_directory = working_directory_ptr;
        next_code_configuration->options = options | SUPERVISOR_NEXT_CODE_OPT_NEWLY_SET;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(supervisor_set_next_code_file_obj, 0, supervisor_set_next_code_file);

//| def ticks_ms() -> int:
//|     """Return the time in milliseconds since an unspecified reference point, wrapping after 2**29ms.
//|
//|     The value is initialized so that the first overflow occurs about 65
//|     seconds after power-on, making it feasible to check that your program
//|     works properly around an overflow.
//|
//|     The wrap value was chosen so that it is always possible to add
//|     or subtract two `ticks_ms` values without overflow on a board without
//|     long ints (or without allocating any long integer objects, on boards with
//|     long ints).
//|
//|     This ticks value comes from a low-accuracy clock internal to the
//|     microcontroller, just like `time.monotonic`.  Due to its low accuracy
//|     and the fact that it "wraps around" every few days, it is intended
//|     for working with short term events like advancing an LED animation,
//|     not for long term events like counting down the time until a holiday.
//|
//|     Addition, subtraction, and comparison of ticks values can be done
//|     with routines like the following::
//|
//|         _TICKS_PERIOD = const(1<<29)
//|         _TICKS_MAX = const(_TICKS_PERIOD-1)
//|         _TICKS_HALFPERIOD = const(_TICKS_PERIOD//2)
//|
//|         def ticks_add(ticks, delta):
//|             "Add a delta to a base number of ticks, performing wraparound at 2**29ms."
//|             return (ticks + delta) % _TICKS_PERIOD
//|
//|         def ticks_diff(ticks1, ticks2):
//|             "Compute the signed difference between two ticks values, assuming that they are within 2**28 ticks"
//|             diff = (ticks1 - ticks2) & _TICKS_MAX
//|             diff = ((diff + _TICKS_HALFPERIOD) & _TICKS_MAX) - _TICKS_HALFPERIOD
//|             return diff
//|
//|         def ticks_less(ticks1, ticks2):
//|             "Return true iff ticks1 is less than ticks2, assuming that they are within 2**28 ticks"
//|             return ticks_diff(ticks1, ticks2) < 0
//|
//|     """
//|     ...
//|
//|
mp_obj_t supervisor_ticks_ms(void) {
    uint64_t ticks_ms = common_hal_time_monotonic_ms();
    return mp_obj_new_int((ticks_ms + 0x1fff0000) % (1 << 29));
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_ticks_ms_obj, supervisor_ticks_ms);

//| def get_previous_traceback() -> Optional[str]:
//|     """If the last vm run ended with an exception (including the KeyboardInterrupt caused by
//|     CTRL-C), returns the traceback as a string.
//|     Otherwise, returns ``None``.
//|
//|     An exception traceback is only preserved over a soft reload, a hard reset clears it.
//|
//|     Only code (main or boot) runs are considered, not REPL runs."""
//|     ...
//|
//|
static mp_obj_t supervisor_get_previous_traceback(void) {
    if (prev_traceback_string) {
        size_t len = strlen(prev_traceback_string);
        if (len > 0) {
            mp_obj_str_t *o = mp_obj_malloc(mp_obj_str_t, &mp_type_str);
            o->len = len;
            // callers probably aren't going to compare this string, so skip computing the hash
            o->hash = 0;
            o->data = (const byte *)prev_traceback_string;
            return MP_OBJ_FROM_PTR(o);
        }
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_get_previous_traceback_obj, supervisor_get_previous_traceback);

//| def reset_terminal(x_pixels: int, y_pixels: int) -> None:
//|     """Reset the CircuitPython serial terminal with new dimensions."""
//|     ...
//|
//|
static mp_obj_t supervisor_reset_terminal(mp_obj_t x_pixels, mp_obj_t y_pixels) {
    #if CIRCUITPY_DISPLAYIO
    supervisor_stop_terminal();
    supervisor_start_terminal(mp_obj_get_int(x_pixels), mp_obj_get_int(y_pixels));
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(supervisor_reset_terminal_obj, supervisor_reset_terminal);

//| def set_usb_identification(
//|     manufacturer: Optional[str] = None,
//|     product: Optional[str] = None,
//|     vid: int = -1,
//|     pid: int = -1,
//| ) -> None:
//|     """Override identification constants in the USB Device Descriptor.
//|
//|     If passed, `manufacturer` and `product` must be ASCII strings (or buffers) of at most 126
//|     characters. Any omitted arguments will be left at their default values.
//|
//|     This method must be called in boot.py to have any effect.
//|
//|     Not available on boards without native USB support.
//|     """
//|     ...
//|
//|
static mp_obj_t supervisor_set_usb_identification(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_IDENTIFICATION
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_manufacturer, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_product, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_vid, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_pid, MP_ARG_INT, {.u_int = -1} },
    };
    struct {
        mp_arg_val_t manufacturer;
        mp_arg_val_t product;
        mp_arg_val_t vid;
        mp_arg_val_t pid;
    } args;
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t *)&args);

    if (custom_usb_identification == NULL) {
        custom_usb_identification = port_malloc(sizeof(usb_identification_t), false);
    }

    mp_arg_validate_int_range(args.vid.u_int, -1, (1 << 16) - 1, MP_QSTR_vid);
    mp_arg_validate_int_range(args.pid.u_int, -1, (1 << 16) - 1, MP_QSTR_pid);

    custom_usb_identification->vid = args.vid.u_int > -1 ? args.vid.u_int : USB_VID;
    custom_usb_identification->pid = args.pid.u_int > -1 ? args.pid.u_int : USB_PID;

    mp_buffer_info_t info;
    if (args.manufacturer.u_obj != mp_const_none) {
        mp_get_buffer_raise(args.manufacturer.u_obj, &info, MP_BUFFER_READ);
        mp_arg_validate_length_range(info.len, 0, 126, MP_QSTR_manufacturer);
        memcpy(custom_usb_identification->manufacturer_name, info.buf, info.len);
        custom_usb_identification->manufacturer_name[info.len] = 0;
    } else {
        strcpy(custom_usb_identification->manufacturer_name, USB_MANUFACTURER);
    }

    if (args.product.u_obj != mp_const_none) {
        mp_get_buffer_raise(args.product.u_obj, &info, MP_BUFFER_READ);
        mp_arg_validate_length_range(info.len, 0, 126, MP_QSTR_product);
        memcpy(custom_usb_identification->product_name, info.buf, info.len);
        custom_usb_identification->product_name[info.len] = 0;
    } else {
        strcpy(custom_usb_identification->product_name, USB_PRODUCT);
    }

    return mp_const_none;
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_KW(supervisor_set_usb_identification_obj, 0, supervisor_set_usb_identification);

//| def get_setting(key: str, default: object=None) -> int | str | bool:
//|     """
//|     Get and parse the value for the given ``key`` from the ``/settings.toml`` file.
//|     If ``key`` is not found or ``settings.toml`` is not present, return the ``default`` value.
//|
//|     :param str key: The setting key to retrieve
//|     :return: The setting value as an ``int``, ``str``, or ``bool`` depending on the value in the file
//|
//|     :raises ValueError: If the value cannot be parsed as a valid TOML value.
//|
//|     The value must be parseable as one of these types:
//|
//|     - ``str``: Double-quoted string.
//|       The string may include Unicode characters, and ``\\u`` Unicode escapes. Backslash-escaped characters
//|       ``\\b``, ``\\r``, ``\\n``, ``\\t``, ``\\v``, ``\\v`` are also allowed.
//|     - ``int``: signed or unsigned integer
//|     - lower-case boolean words ``true`` and ``false``.
//|       The values are returned as Python ``True`` or ``False`` values.
//|
//|     Example::
//|
//|         # settings.toml:
//|         WIDTH = 42
//|         color = "red"
//|         DEBUG = true
//|
//|         import supervisor
//|         print(supervisor.get_setting("WIDTH")) # prints 42
//|         print(supervisor.get_setting("color")) # prints 'red'
//|         print(supervisor.get_setting("DEBUG")) # prints True
//|     """
//|     ...
//|
static mp_obj_t supervisor_get_setting(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    #if CIRCUITPY_SETTINGS_TOML
    enum { ARG_key, ARG_default };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_key, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_default, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t *)&args);

    const char *key = mp_obj_str_get_str(args[ARG_key].u_obj);
    mp_obj_t value;
    settings_err_t result = settings_get_obj(key, &value);

    switch (result) {
        case SETTINGS_OK:
            return value;
        case SETTINGS_ERR_NOT_FOUND:
        case SETTINGS_ERR_OPEN:
            return args[ARG_default].u_obj;
        default:
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_value);
    }
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_KW(supervisor_get_setting_obj, 1, supervisor_get_setting);

static const mp_rom_map_elem_t supervisor_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_supervisor) },
    { MP_ROM_QSTR(MP_QSTR_runtime),  MP_ROM_PTR(&common_hal_supervisor_runtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_reload),  MP_ROM_PTR(&supervisor_reload_obj) },
    { MP_ROM_QSTR(MP_QSTR_RunReason),  MP_ROM_PTR(&supervisor_run_reason_type) },
    #if CIRCUITPY_SAFEMODE_PY
    { MP_ROM_QSTR(MP_QSTR_SafeModeReason),  MP_ROM_PTR(&supervisor_safe_mode_reason_type) },
    #else
    { MP_ROM_QSTR(MP_QSTR_SafeModeReason),  MP_ROM_NONE },
    #endif
    { MP_ROM_QSTR(MP_QSTR_set_next_code_file),  MP_ROM_PTR(&supervisor_set_next_code_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_ms),  MP_ROM_PTR(&supervisor_ticks_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_previous_traceback),  MP_ROM_PTR(&supervisor_get_previous_traceback_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_setting),  MP_ROM_PTR(&supervisor_get_setting_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_terminal),  MP_ROM_PTR(&supervisor_reset_terminal_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_usb_identification),  MP_ROM_PTR(&supervisor_set_usb_identification_obj) },
    { MP_ROM_QSTR(MP_QSTR_status_bar),  MP_ROM_PTR(&shared_module_supervisor_status_bar_obj) },
};

static MP_DEFINE_CONST_DICT(supervisor_module_globals, supervisor_module_globals_table);

const mp_obj_module_t supervisor_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&supervisor_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_supervisor, supervisor_module);
