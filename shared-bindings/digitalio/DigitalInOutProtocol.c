// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DigitalInOutProtocol.h"

#include "py/obj.h"
#include "py/objtype.h"
#include "py/proto.h"
#include "py/runtime.h"
#include "py/nlr.h"
#include "py/gc.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"
#include "shared-bindings/util.h"
#include "supervisor/port_heap.h"

//| from typing import Protocol, Optional
//|
//| class DigitalInOutProtocol(Protocol):
//|     """Protocol for digital input/output pin control.
//|
//|     Any object that implements this protocol can be used as a digital pin,
//|     providing compatibility with code expecting a `digitalio.DigitalInOut`.
//|     """
//|
//|     def deinit(self) -> None:
//|         """Deinitialize the pin and release hardware resources."""
//|         ...
//|
//|     def deinited(self) -> bool:
//|         """Check whether the pin has been deinitialized.
//|
//|         :return: True if deinitialized, False otherwise
//|         """
//|         ...
//|
//|     def switch_to_input(self, pull: Optional[digitalio.Pull] = None) -> None:
//|         """Configure the pin as a digital input.
//|
//|         :param pull: Pull resistor configuration (UP, DOWN, or None)
//|         """
//|         ...
//|
//|     def switch_to_output(
//|         self,
//|         value: bool = False,
//|         drive_mode: digitalio.DriveMode = digitalio.DriveMode.PUSH_PULL
//|     ) -> None:
//|         """Configure the pin as a digital output.
//|
//|         :param value: Initial output value (default False)
//|         :param drive_mode: Output drive mode (PUSH_PULL or OPEN_DRAIN)
//|         """
//|         ...
//|
//|     @property
//|     def direction(self) -> digitalio.Direction:
//|         """The pin direction (INPUT or OUTPUT)."""
//|         ...
//|
//|     @direction.setter
//|     def direction(self, value: digitalio.Direction) -> None:
//|         ...
//|
//|     @property
//|     def value(self) -> bool:
//|         """The digital logic level of the pin."""
//|         ...
//|
//|     @value.setter
//|     def value(self, val: bool) -> None:
//|         ...
//|
//|     @property
//|     def pull(self) -> Optional[digitalio.Pull]:
//|         """The pull resistor configuration for inputs (UP, DOWN, or None)."""
//|         ...
//|
//|     @pull.setter
//|     def pull(self, pul: Optional[digitalio.Pull]) -> None:
//|         ...
//|
//|     @property
//|     def drive_mode(self) -> digitalio.DriveMode:
//|         """The drive mode for outputs (PUSH_PULL or OPEN_DRAIN)."""
//|         ...
//|
//|     @drive_mode.setter
//|     def drive_mode(self, mode: digitalio.DriveMode) -> None:
//|         ...
//|
// C Implementation Notes:
// -----------------------
// For C implementations, define a digitalinout_p_t protocol structure and assign it
// to your type's protocol field in MP_DEFINE_CONST_OBJ_TYPE.
//
// Example:
//   static const digitalinout_p_t my_type_proto = {
//       MP_PROTO_IMPLEMENT(MP_QSTR_DigitalInOut)
//       .construct = my_construct_func,
//       .deinit = my_deinit_func,
//       .deinited = my_deinited_func,
//       .switch_to_input = my_switch_to_input_func,
//       .switch_to_output = my_switch_to_output_func,
//       .get_direction = my_get_direction_func,
//       .get_value = my_get_value_func,
//       .set_value = my_set_value_func,
//       .get_drive_mode = my_get_drive_mode_func,
//       .set_drive_mode = my_set_drive_mode_func,
//       .get_pull = my_get_pull_func,
//       .set_pull = my_set_pull_func,
//   };
//
//   MP_DEFINE_CONST_OBJ_TYPE(
//       my_type,
//       MP_QSTR_MyType,
//       MP_TYPE_FLAG_NONE,
//       make_new, my_make_new,
//       protocol, &my_type_proto
//   );
//
// See shared-bindings/digitalio/DigitalInOut.c for a complete example.
//

#if CIRCUITPY_DIGITALINOUT_PROTOCOL
static void check_object_has_method(mp_obj_t obj, qstr method_name) {
    mp_obj_t dest[2];
    mp_load_method_protected(obj, method_name, dest, true);
    if (dest[0] == MP_OBJ_NULL) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q object missing '%q' method"), MP_OBJ_TO_PTR(obj), method_name);
    }
}

static void check_object_has_attr(mp_obj_t obj, qstr attr_name) {
    mp_obj_t dest[2];
    mp_load_method_protected(obj, attr_name, dest, true);
    if (dest[0] == MP_OBJ_NULL) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q object missing '%q' attribute"), MP_OBJ_TO_PTR(obj), attr_name);
    }
}
#endif

mp_obj_t digitalinout_protocol_from_pin(
    mp_obj_t pin_or_dio,
    qstr arg_name,
    bool allow_none,
    bool use_port_allocation,
    bool *out_owns_pin) {

    *out_owns_pin = false;

    // Handle None case
    if (allow_none && pin_or_dio == mp_const_none) {
        return mp_const_none;
    }

    // Check if it's a Pin
    if (mp_obj_is_type(pin_or_dio, &mcu_pin_type)) {
        // Validate the pin is free
        const mcu_pin_obj_t *pin;
        if (allow_none) {
            pin = validate_obj_is_free_pin_or_none(pin_or_dio, arg_name);
            if (pin == NULL) {
                return mp_const_none;
            }
        } else {
            pin = validate_obj_is_free_pin(pin_or_dio, arg_name);
        }

        // Allocate and construct a DigitalInOut object
        // Use port_malloc if GC is not available or if forced
        digitalio_digitalinout_obj_t *dio;
        if (use_port_allocation) {
            dio = port_malloc(sizeof(digitalio_digitalinout_obj_t), false);
        } else {
            dio = m_malloc(sizeof(digitalio_digitalinout_obj_t));
        }
        dio->base.type = &digitalio_digitalinout_type;
        mp_obj_t dio_obj = MP_OBJ_FROM_PTR(dio);
        *out_owns_pin = true;

        digitalinout_result_t result = common_hal_digitalio_digitalinout_construct(dio_obj, pin);
        if (result != DIGITALINOUT_OK) {
            // Free the allocation on error
            if (use_port_allocation) {
                port_free(dio);
            }
            mp_raise_ValueError_varg(MP_ERROR_TEXT("%q init failed"), arg_name);
        }
        return dio_obj;
    }

    #if CIRCUITPY_DIGITALINOUT_PROTOCOL
    // Check if it natively implements the DigitalInOutProtocol
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, pin_or_dio);
    if (proto != NULL) {
        // Native protocol support - use it directly
        return pin_or_dio;
    }

    // Verify the object has the required methods/attributes
    check_object_has_method(pin_or_dio, MP_QSTR_deinit);
    check_object_has_method(pin_or_dio, MP_QSTR_switch_to_input);
    check_object_has_method(pin_or_dio, MP_QSTR_switch_to_output);
    check_object_has_attr(pin_or_dio, MP_QSTR_deinited);
    check_object_has_attr(pin_or_dio, MP_QSTR_direction);
    check_object_has_attr(pin_or_dio, MP_QSTR_value);
    check_object_has_attr(pin_or_dio, MP_QSTR_drive_mode);
    check_object_has_attr(pin_or_dio, MP_QSTR_pull);

    // Object has all required attributes - use it as DigitalInOutProtocol
    return pin_or_dio;
    #else
    mp_raise_TypeError_varg(MP_ERROR_TEXT("'%q' object does not support '%q'"),
        mp_obj_get_type_qstr(pin_or_dio), MP_QSTR_DigitalInOut);
    #endif
}

// These functions are only used when CIRCUITPY_DIGITALINOUT_PROTOCOL is enabled.
// Otherwise, the digitalinout_* functions are called directly.
#if CIRCUITPY_DIGITALINOUT_PROTOCOL
void digitalinout_protocol_deinit(mp_obj_t self) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_protocol_digitalinout, self);
    if (proto && proto->deinit) {
        proto->deinit(self);
        return;
    }

    // Fallback to Python method call
    mp_obj_t dest[2];
    mp_load_method_maybe(self, MP_QSTR_deinit, dest);
    if (dest[0] != MP_OBJ_NULL) {
        mp_call_method_n_kw(0, 0, dest);
        return;
    }
}

bool digitalinout_protocol_deinited(mp_obj_t self) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->deinited) {
        return proto->deinited(self);
    }

    // Try as attribute
    mp_obj_t attr = mp_load_attr(self, MP_QSTR_deinited);
    return mp_obj_is_true(attr);
}

digitalinout_result_t digitalinout_protocol_switch_to_input(mp_obj_t self, digitalio_pull_t pull) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->switch_to_input) {
        return proto->switch_to_input(self, pull);
    }

    // Fallback to Python method call
    mp_obj_t dest[3];
    mp_load_method_maybe(self, MP_QSTR_switch_to_input, dest);
    if (dest[0] != MP_OBJ_NULL) {
        mp_obj_t pull_obj = mp_const_none;
        if (pull == PULL_UP) {
            pull_obj = MP_OBJ_FROM_PTR(&digitalio_pull_up_obj);
        } else if (pull == PULL_DOWN) {
            pull_obj = MP_OBJ_FROM_PTR(&digitalio_pull_down_obj);
        }
        dest[2] = pull_obj;
        mp_call_method_n_kw(1, 0, dest);
        return DIGITALINOUT_OK;
    }

    return DIGITALINOUT_PIN_BUSY;
}

digitalinout_result_t digitalinout_protocol_switch_to_output(mp_obj_t self, bool value, digitalio_drive_mode_t drive_mode) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->switch_to_output) {
        return proto->switch_to_output(self, value, drive_mode);
    }

    // Fallback to Python method call
    mp_obj_t dest[4];
    mp_load_method_maybe(self, MP_QSTR_switch_to_output, dest);
    if (dest[0] != MP_OBJ_NULL) {
        dest[2] = mp_obj_new_bool(value);
        dest[3] = (drive_mode == DRIVE_MODE_PUSH_PULL) ?
            MP_OBJ_FROM_PTR(&digitalio_drive_mode_push_pull_obj) :
            MP_OBJ_FROM_PTR(&digitalio_drive_mode_open_drain_obj);
        mp_call_method_n_kw(2, 0, dest);
        return DIGITALINOUT_OK;
    }

    mp_raise_TypeError(MP_ERROR_TEXT("object does not support DigitalInOut protocol"));
}

digitalio_direction_t digitalinout_protocol_get_direction(mp_obj_t self) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->get_direction) {
        return proto->get_direction(self);
    }

    // Fallback to Python attribute access
    mp_obj_t direction = mp_load_attr(self, MP_QSTR_direction);
    if (direction == MP_ROM_PTR(&digitalio_direction_input_obj)) {
        return DIRECTION_INPUT;
    }
    return DIRECTION_OUTPUT;
}

mp_negative_errno_t digitalinout_protocol_set_value(mp_obj_t self, bool value) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->set_value) {
        return proto->set_value(self, value);
    }

    // Fallback to Python attribute assignment
    mp_store_attr(self, MP_QSTR_value, mp_obj_new_bool(value));
    return 0;
}

mp_negative_errno_t digitalinout_protocol_get_value(mp_obj_t self, bool *value) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->get_value) {
        return proto->get_value(self, value);
    }

    // Fallback to Python attribute access
    *value = mp_obj_is_true(mp_load_attr(self, MP_QSTR_value));
    return 0;
}

digitalinout_result_t digitalinout_protocol_set_drive_mode(mp_obj_t self, digitalio_drive_mode_t drive_mode) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->set_drive_mode) {
        return proto->set_drive_mode(self, drive_mode);
    }

    // Fallback to Python attribute assignment
    mp_obj_t drive_mode_obj = (drive_mode == DRIVE_MODE_PUSH_PULL) ?
        MP_OBJ_FROM_PTR(&digitalio_drive_mode_push_pull_obj) :
        MP_OBJ_FROM_PTR(&digitalio_drive_mode_open_drain_obj);
    mp_store_attr(self, MP_QSTR_drive_mode, drive_mode_obj);
    return DIGITALINOUT_OK;
}

digitalio_drive_mode_t digitalinout_protocol_get_drive_mode(mp_obj_t self) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->get_drive_mode) {
        return proto->get_drive_mode(self);
    }

    // Fallback to Python attribute access
    mp_obj_t drive_mode = mp_load_attr(self, MP_QSTR_drive_mode);
    if (drive_mode == MP_ROM_PTR(&digitalio_drive_mode_open_drain_obj)) {
        return DRIVE_MODE_OPEN_DRAIN;
    }
    return DRIVE_MODE_PUSH_PULL;
}

digitalinout_result_t digitalinout_protocol_set_pull(mp_obj_t self, digitalio_pull_t pull) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->set_pull) {
        return proto->set_pull(self, pull);
    }

    // Fallback to Python attribute assignment
    mp_obj_t pull_obj = mp_const_none;
    if (pull == PULL_UP) {
        pull_obj = MP_OBJ_FROM_PTR(&digitalio_pull_up_obj);
    } else if (pull == PULL_DOWN) {
        pull_obj = MP_OBJ_FROM_PTR(&digitalio_pull_down_obj);
    }
    mp_store_attr(self, MP_QSTR_pull, pull_obj);
    return DIGITALINOUT_OK;
}

digitalio_pull_t digitalinout_protocol_get_pull(mp_obj_t self) {
    const digitalinout_p_t *proto = mp_proto_get(MP_QSTR_DigitalInOut, self);
    if (proto && proto->get_pull) {
        return proto->get_pull(self);
    }

    // Fallback to Python attribute access
    mp_obj_t pull = mp_load_attr(self, MP_QSTR_pull);
    if (pull == MP_OBJ_FROM_PTR(&digitalio_pull_up_obj)) {
        return PULL_UP;
    } else if (pull == MP_OBJ_FROM_PTR(&digitalio_pull_down_obj)) {
        return PULL_DOWN;
    }
    return PULL_NONE;
}

#endif // CIRCUITPY_DIGITALINOUT_PROTOCOL
