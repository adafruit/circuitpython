// This file is part of the CircuitPython project: https://circuitpython.org
// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/qspibus/QSPIBus.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"

#include "py/binary.h"
#include "py/obj.h"
#include "py/runtime.h"

static void check_for_deinit(qspibus_qspibus_obj_t *self) {
    if (common_hal_qspibus_qspibus_deinited(self)) {
        raise_deinited_error();
    }
}

//| class QSPIBus:
//|     """QSPI bus for quad-SPI displays."""
//|
//|     def __init__(
//|         self,
//|         *,
//|         clock: microcontroller.Pin,
//|         data0: microcontroller.Pin,
//|         data1: microcontroller.Pin,
//|         data2: microcontroller.Pin,
//|         data3: microcontroller.Pin,
//|         cs: microcontroller.Pin,
//|         dcx: Optional[microcontroller.Pin] = None,
//|         reset: Optional[microcontroller.Pin] = None,
//|         frequency: int = 80_000_000,
//|     ) -> None:
//|         """Create a QSPIBus object for quad-SPI display communication.
//|
//|         :param ~microcontroller.Pin clock: QSPI clock pin
//|         :param ~microcontroller.Pin data0: QSPI data line 0
//|         :param ~microcontroller.Pin data1: QSPI data line 1
//|         :param ~microcontroller.Pin data2: QSPI data line 2
//|         :param ~microcontroller.Pin data3: QSPI data line 3
//|         :param ~microcontroller.Pin cs: Chip select pin
//|         :param ~microcontroller.Pin dcx: Optional data/command select pin.
//|             Reserved for future hardware paths. Current ESP32-S3 implementation
//|             uses encoded QSPI command words and does not require explicit DCX.
//|         :param ~microcontroller.Pin reset: Optional reset pin
//|         :param int frequency: Bus frequency in Hz (1-80MHz)
//|         """
//|         ...
//|
static mp_obj_t qspibus_qspibus_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_clock, ARG_data0, ARG_data1, ARG_data2, ARG_data3, ARG_cs, ARG_dcx, ARG_reset, ARG_frequency };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_data0, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_data1, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_data2, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_data3, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_cs, MP_ARG_KW_ONLY | MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_dcx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_reset, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_frequency, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 80000000} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *data0 = validate_obj_is_free_pin(args[ARG_data0].u_obj, MP_QSTR_data0);
    const mcu_pin_obj_t *data1 = validate_obj_is_free_pin(args[ARG_data1].u_obj, MP_QSTR_data1);
    const mcu_pin_obj_t *data2 = validate_obj_is_free_pin(args[ARG_data2].u_obj, MP_QSTR_data2);
    const mcu_pin_obj_t *data3 = validate_obj_is_free_pin(args[ARG_data3].u_obj, MP_QSTR_data3);
    const mcu_pin_obj_t *cs = validate_obj_is_free_pin(args[ARG_cs].u_obj, MP_QSTR_cs);
    const mcu_pin_obj_t *dcx = validate_obj_is_free_pin_or_none(args[ARG_dcx].u_obj, MP_QSTR_dcx);
    const mcu_pin_obj_t *reset = validate_obj_is_free_pin_or_none(args[ARG_reset].u_obj, MP_QSTR_reset);

    uint32_t frequency = (uint32_t)mp_arg_validate_int_range(args[ARG_frequency].u_int, 1, 80000000, MP_QSTR_frequency);

    qspibus_qspibus_obj_t *self = &allocate_display_bus_or_raise()->qspi_bus;
    self->base.type = &qspibus_qspibus_type;
    common_hal_qspibus_qspibus_construct(self, clock, data0, data1, data2, data3, cs, dcx, reset, frequency);

    return MP_OBJ_FROM_PTR(self);
}

//|     def reset(self) -> None:
//|         """Perform a hardware reset using the reset pin.
//|
//|         :raises RuntimeError: if no reset pin was provided at construction.
//|         """
//|         ...
//|
static mp_obj_t qspibus_qspibus_obj_reset(mp_obj_t self_in) {
    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (!common_hal_qspibus_qspibus_reset(MP_OBJ_FROM_PTR(self))) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_reset);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(qspibus_qspibus_reset_obj, qspibus_qspibus_obj_reset);

//|     def send(
//|         self, command: int, data: ReadableBuffer, *, toggle_every_byte: bool = False
//|     ) -> None:
//|         """Sends the given command value followed by the full set of data. Display state, such as
//|         vertical scroll, set via ``send`` may or may not be reset once the code is done."""
//|         ...
//|
static mp_obj_t qspibus_qspibus_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_command, ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_command, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_data, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);

    uint8_t command = (uint8_t)mp_arg_validate_int_range(args[ARG_command].u_int, 0, 255, MP_QSTR_command);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

    // Flush any pending command from a prior write_command() call.
    // begin_transaction() returns false while has_pending_command is set,
    // so entering the wait loop without flushing would spin forever.
    if (self->has_pending_command) {
        common_hal_qspibus_qspibus_write_data(self, NULL, 0);
    }

    // Wait for display bus to be available, then acquire transaction.
    while (!common_hal_qspibus_qspibus_begin_transaction(MP_OBJ_FROM_PTR(self))) {
        RUN_BACKGROUND_TASKS;
    }
    common_hal_qspibus_qspibus_send(MP_OBJ_FROM_PTR(self), DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, &command, 1);
    common_hal_qspibus_qspibus_send(MP_OBJ_FROM_PTR(self), DISPLAY_DATA, CHIP_SELECT_UNTOUCHED, ((uint8_t *)bufinfo.buf), bufinfo.len);
    common_hal_qspibus_qspibus_end_transaction(MP_OBJ_FROM_PTR(self));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(qspibus_qspibus_send_obj, 1, qspibus_qspibus_send);

//|     def write_command(self, command: int) -> None:
//|         """Stage a command byte for subsequent :py:meth:`write_data`.
//|
//|         If a previously staged command had no data, it is sent as
//|         a command-only transaction before staging the new one.
//|         """
//|         ...
//|
static mp_obj_t qspibus_qspibus_write_command(mp_obj_t self_in, mp_obj_t command_obj) {
    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    uint8_t command = (uint8_t)mp_arg_validate_int_range(mp_obj_get_int(command_obj), 0, 255, MP_QSTR_command);
    common_hal_qspibus_qspibus_write_command(self, command);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(qspibus_qspibus_write_command_obj, qspibus_qspibus_write_command);

//|     def write_data(self, data: ReadableBuffer) -> None:
//|         """Send payload bytes for the most recently staged command."""
//|         ...
//|
static mp_obj_t qspibus_qspibus_write_data(mp_obj_t self_in, mp_obj_t data_obj) {
    qspibus_qspibus_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);
    common_hal_qspibus_qspibus_write_data(self, (const uint8_t *)bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(qspibus_qspibus_write_data_obj, qspibus_qspibus_write_data);

static const mp_rom_map_elem_t qspibus_qspibus_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&qspibus_qspibus_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&qspibus_qspibus_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_command), MP_ROM_PTR(&qspibus_qspibus_write_command_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_data), MP_ROM_PTR(&qspibus_qspibus_write_data_obj) },
};
static MP_DEFINE_CONST_DICT(qspibus_qspibus_locals_dict, qspibus_qspibus_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    qspibus_qspibus_type,
    MP_QSTR_QSPIBus,
    MP_TYPE_FLAG_NONE,
    make_new, qspibus_qspibus_make_new,
    locals_dict, &qspibus_qspibus_locals_dict
    );
