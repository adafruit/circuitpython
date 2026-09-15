// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "extmod/vfs.h"

#include "shared-bindings/emmcio/EMMC.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared/runtime/context_manager_helpers.h"
#include "common-hal/emmcio/EMMC.h"

static void check_for_deinit(emmcio_emmc_obj_t *self) {
    if (common_hal_emmcio_emmc_deinited(self)) {
        raise_deinited_error();
    }
}

//| class EMMC:
//|     """eMMC as a block device"""
//|
//|     def __init__(
//|         self,
//|         *,
//|         clock: microcontroller.Pin,
//|         command: microcontroller.Pin,
//|         data: microcontroller.Pin,
//|         reset: Optional[microcontroller.Pin] = None,
//|         vccq: Optional[microcontroller.Pin] = None,
//|         high_speed: bool = False,
//|         write_enabled: bool = False,
//|     ) -> None:
//|         """Power up the card and make it ready for block access.
//|
//|         Only one `EMMC` object may exist at a time. Call `deinit()`, or use
//|         the object as a context manager, to release the card and its pins.
//|
//|         :param ~microcontroller.Pin clock: the card's CLK pin
//|         :param ~microcontroller.Pin command: the card's CMD pin
//|         :param ~microcontroller.Pin data: the card's DAT0 pin. The bus is
//|           1-bit, so this is a single pin.
//|         :param ~microcontroller.Pin reset: the card's RST_n pin, if the board
//|           wires one
//|         :param ~microcontroller.Pin vccq: a pin gating the card's I/O rail,
//|           if the board has one
//|         :param bool high_speed: Run the bus at its faster clock rate. Raises
//|           an `OSError` if the card will not make the switch.
//|         :param bool write_enabled: Allow `writeblocks()`. When `False`, the
//|           object is read-only and every write path refuses.
//|
//|         :raises ValueError: if the pins are unusable or already in use, or
//|           if the card is owned by the USB drive.
//|         :raises OSError: if the card does not come up. ``emmcio failure: n``
//|           names the MMC command that did not answer (``8`` being the
//|           extended CSD), and ``high_speed failure: n`` how far the switch
//|           got: ``0`` the card does not advertise 52 MHz, ``1`` no CMD6
//|           response, ``2`` the card never released DAT0, ``3`` it never came
//|           back to the transfer state, ``4`` the EXT_CSD readback did not
//|           take, ``5`` the 32 MHz smoke test failed, ``7`` the card rejected
//|           HS_TIMING.
//|
//|         Mount the card's filesystem::
//|
//|           import board
//|           import emmcio
//|           import storage
//|
//|           emmc = emmcio.EMMC(
//|               clock=board.EMMC_CLK,
//|               command=board.EMMC_CMD,
//|               data=board.EMMC_DAT0,
//|               reset=board.EMMC_RESET,
//|               vccq=board.EMMC_VCCQ,
//|               high_speed=True,
//|               write_enabled=True,
//|           )
//|           storage.mount(storage.VfsFat(emmc), "/sd")
//|         """
//|         ...
//|
static mp_obj_t emmcio_emmc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_clock, ARG_command, ARG_data, ARG_reset, ARG_vccq, ARG_high_speed, ARG_write_enabled };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ },
        { MP_QSTR_command, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ },
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ },
        { MP_QSTR_reset, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_vccq, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_high_speed, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_write_enabled, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *command = validate_obj_is_pin(args[ARG_command].u_obj, MP_QSTR_command);
    const mcu_pin_obj_t *data = validate_obj_is_pin(args[ARG_data].u_obj, MP_QSTR_data);
    const mcu_pin_obj_t *reset = validate_obj_is_pin_or_none(args[ARG_reset].u_obj, MP_QSTR_reset);
    const mcu_pin_obj_t *vccq = validate_obj_is_pin_or_none(args[ARG_vccq].u_obj, MP_QSTR_vccq);

    // Every line is a separate net, so two of them being the same pin is a
    // wiring mistake, not a configuration.
    const mcu_pin_obj_t *pins[] = { clock, command, data, reset, vccq };
    const qstr names[] = { MP_QSTR_clock, MP_QSTR_command, MP_QSTR_data, MP_QSTR_reset, MP_QSTR_vccq };
    for (size_t i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        for (size_t j = i + 1; j < MP_ARRAY_SIZE(pins); j++) {
            if (pins[i] != NULL && pins[i] == pins[j]) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("%q and %q must be different"),
                    names[i], names[j]);
            }
        }
    }

    emmcio_emmc_obj_t *self = mp_obj_malloc(emmcio_emmc_obj_t, &emmcio_emmc_type);
    // An OSError means the card itself did not come up; a ValueError means the
    // wiring or the hardware is unusable.
    int detail = 0;
    switch (common_hal_emmcio_emmc_construct(self, clock, command, data, reset, vccq,
        args[ARG_high_speed].u_bool, args[ARG_write_enabled].u_bool, &detail)) {
        case EMMCIO_OK:
            break;
        case EMMCIO_ERR_PIN_PORT:
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q pin"), (qstr)detail);
            break;
        case EMMCIO_ERR_SPI_IN_USE:
            mp_raise_ValueError(MP_ERROR_TEXT("SPI peripheral in use"));
            break;
        case EMMCIO_ERR_PIN_IN_USE:
            mp_raise_ValueError(MP_ERROR_TEXT("Hardware in use, try alternative pins"));
            break;
        case EMMCIO_ERR_USB_OWNED:
            mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_emmcio);
            break;
        case EMMCIO_ERR_IN_USE:
            mp_raise_ValueError(MP_ERROR_TEXT("Peripheral in use"));
            break;
        case EMMCIO_ERR_INIT:
            mp_raise_OSError_msg_varg(MP_ERROR_TEXT("%q failure: %d"), MP_QSTR_emmcio, detail);
            break;
        case EMMCIO_ERR_HIGH_SPEED:
            mp_raise_OSError_msg_varg(MP_ERROR_TEXT("%q failure: %d"), MP_QSTR_high_speed, detail);
            break;
    }
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Release the card and the pins it uses. Any further use of this
//|         object raises a `ValueError`."""
//|         ...
//|
static mp_obj_t emmcio_emmc_deinit(mp_obj_t self_in) {
    common_hal_emmcio_emmc_deinit(MP_OBJ_TO_PTR(self_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(emmcio_emmc_deinit_obj, emmcio_emmc_deinit);

//|     def __enter__(self) -> EMMC:
//|         """No-op used by Context Managers."""
//|         ...
//|
//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
static mp_obj_t emmcio_emmc_obj___exit__(size_t n_args, const mp_obj_t *args) {
    return emmcio_emmc_deinit(args[0]);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(emmcio_emmc___exit___obj, 4, 4, emmcio_emmc_obj___exit__);

#define CHUNK_BLOCKS 64u

static int emmc_read_chunked(emmcio_emmc_obj_t *self, uint8_t *out, mp_uint_t start, mp_uint_t count, bool from_vm) {
    mp_uint_t total = common_hal_emmcio_emmc_get_block_count(self);
    if (count == 0 || start >= total || count > total - start) {
        return -MP_EINVAL;
    }
    for (mp_uint_t done = 0; done < count;) {
        mp_uint_t run = MIN(CHUNK_BLOCKS, count - done);
        bool ok = false;
        for (int attempt = 0; attempt < 3 && !ok; attempt++) {
            ok = common_hal_emmcio_emmc_readblocks(start + done, out + done * EMMC_BLOCK_SIZE, run);
        }
        if (!ok) {
            return -MP_EIO;
        }
        done += run;
        RUN_BACKGROUND_TASKS;
        if (from_vm) {
            mp_handle_pending(true);
        }
    }
    return 0;
}

//|     def readblocks(self, start_block: int, buf: WriteableBuffer) -> None:
//|         """Read into ``buf`` starting at ``start_block``.
//|
//|         :param int start_block: the first block to read
//|         :param WriteableBuffer buf: a buffer whose length is a non-zero
//|           multiple of `block_size`
//|
//|         :raises ValueError: if ``buf`` is the wrong length, or the requested
//|           blocks run past the end of the card.
//|         :raises OSError: if the card fails to deliver the data."""
//|         ...
//|
static mp_obj_t emmcio_emmc_readblocks(mp_obj_t self_in, mp_obj_t start_in, mp_obj_t buf_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.len == 0 || (bufinfo.len % EMMC_BLOCK_SIZE) != 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Buffer must be a multiple of %d bytes"), 512);
    }
    mp_uint_t start = mp_obj_get_int_truncated(start_in);
    mp_uint_t count = bufinfo.len / EMMC_BLOCK_SIZE;
    mp_uint_t total = common_hal_emmcio_emmc_get_block_count(self);
    if (start >= total || count > total - start) {
        mp_raise_ValueError(MP_ERROR_TEXT("address out of range"));
    }

    int err = emmc_read_chunked(self, bufinfo.buf, start, count, true);
    if (err != 0) {
        mp_raise_OSError(-err);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(emmcio_emmc_readblocks_obj, emmcio_emmc_readblocks);

static int emmc_write_chunked(emmcio_emmc_obj_t *self, const uint8_t *src, mp_uint_t start, mp_uint_t count, bool from_vm) {
    mp_uint_t total = common_hal_emmcio_emmc_get_block_count(self);
    if (count == 0 || start >= total || count > total - start) {
        return -MP_EINVAL;
    }
    for (mp_uint_t done = 0; done < count;) {
        mp_uint_t run = MIN(CHUNK_BLOCKS, count - done);
        bool ok = false;
        for (int attempt = 0; attempt < 3 && !ok; attempt++) {
            ok = common_hal_emmcio_emmc_writeblocks(start + done, src + done * EMMC_BLOCK_SIZE, run);
        }
        if (!ok) {
            return -MP_EIO;
        }
        done += run;
        RUN_BACKGROUND_TASKS;
        if (from_vm) {
            mp_handle_pending(true);
        }
    }
    return 0;
}

//|     def writeblocks(self, start_block: int, buf: ReadableBuffer) -> None:
//|         """Write ``buf`` to the card starting at ``start_block``.
//|
//|         :param int start_block: the first block to write
//|         :param ReadableBuffer buf: a buffer whose length is a non-zero
//|           multiple of `block_size`
//|
//|         :raises RuntimeError: if this object was not constructed with
//|           ``write_enabled=True``.
//|         :raises ValueError: if ``buf`` is the wrong length, or the requested
//|           blocks run past the end of the card.
//|         :raises OSError: if the write fails."""
//|         ...
//|
static mp_obj_t emmcio_emmc_writeblocks(mp_obj_t self_in, mp_obj_t start_in, mp_obj_t buf_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (!self->write_enabled) {
        mp_raise_msg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Read-only"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len == 0 || (bufinfo.len % EMMC_BLOCK_SIZE) != 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Buffer must be a multiple of %d bytes"), 512);
    }
    mp_uint_t start = mp_obj_get_int_truncated(start_in);
    mp_uint_t count = bufinfo.len / EMMC_BLOCK_SIZE;
    mp_uint_t total = common_hal_emmcio_emmc_get_block_count(self);
    if (start >= total || count > total - start) {
        mp_raise_ValueError(MP_ERROR_TEXT("address out of range"));
    }

    int err = emmc_write_chunked(self, bufinfo.buf, start, count, true);
    if (err != 0) {
        mp_raise_OSError(-err);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(emmcio_emmc_writeblocks_obj, emmcio_emmc_writeblocks);

//|     def ioctl(self, op: int, arg: int) -> Optional[int]:
//|         """Perform a block-device control operation, as required by the
//|         block-device protocol. Returns `None` for operations this device does
//|         not implement."""
//|         ...
//|
static mp_obj_t emmcio_emmc_ioctl(mp_obj_t self_in, mp_obj_t op_in, mp_obj_t arg_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    uint32_t out = 0;
    if (!common_hal_emmcio_emmc_ioctl(mp_obj_get_int_truncated(op_in),
        mp_obj_get_int_truncated(arg_in), &out)) {
        return mp_const_none;
    }
    return mp_obj_new_int_from_uint(out);
}
static MP_DEFINE_CONST_FUN_OBJ_3(emmcio_emmc_ioctl_obj, emmcio_emmc_ioctl);

mp_uint_t emmcio_emmc_readblocks_native(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t nblocks) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_emmcio_emmc_deinited(self)) {
        return -MP_ENODEV;
    }
    return emmc_read_chunked(self, buf, start_block, nblocks, false);
}

mp_uint_t emmcio_emmc_writeblocks_native(mp_obj_t self_in, const uint8_t *buf,
    uint32_t start_block, uint32_t nblocks) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_emmcio_emmc_deinited(self)) {
        return -MP_ENODEV;
    }

    if (!self->write_enabled) {
        return -MP_EROFS;
    }
    return emmc_write_chunked(self, buf, start_block, nblocks, false);
}

bool emmcio_emmc_ioctl_native(mp_obj_t self_in, uint32_t cmd, uint32_t arg,
    size_t *out_value) {

    (void)self_in;
    uint32_t out = 0;
    bool ok = common_hal_emmcio_emmc_ioctl(cmd, arg, &out);
    *out_value = out;
    return ok;
}

bool emmcio_emmc_is_write_enabled(mp_obj_t self_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return !common_hal_emmcio_emmc_deinited(self) && self->write_enabled;
}

//|     def read_ext_csd(self) -> bytes:
//|         """Read the card's 512-byte extended CSD register."""
//|         ...
//|
static mp_obj_t emmcio_emmc_read_ext_csd(mp_obj_t self_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    uint8_t ext_csd[EMMC_BLOCK_SIZE];
    if (!common_hal_emmcio_emmc_read_ext_csd(ext_csd)) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_obj_new_bytes(ext_csd, sizeof(ext_csd));
}
static MP_DEFINE_CONST_FUN_OBJ_1(emmcio_emmc_read_ext_csd_obj, emmcio_emmc_read_ext_csd);

//|     def status(self) -> int:
//|         """Read the card's 32-bit status register."""
//|         ...
//|
static mp_obj_t emmcio_emmc_status(mp_obj_t self_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    uint8_t r1[6];
    if (!common_hal_emmcio_emmc_read_status(r1)) {
        mp_raise_OSError(MP_EIO);
    }
    uint32_t status = ((uint32_t)r1[1] << 24) | ((uint32_t)r1[2] << 16) |
        ((uint32_t)r1[3] << 8) | (uint32_t)r1[4];
    return mp_obj_new_int_from_uint(status);
}
static MP_DEFINE_CONST_FUN_OBJ_1(emmcio_emmc_status_obj, emmcio_emmc_status);

//|     count: int
//|     """The number of blocks on the card."""
//|
static mp_obj_t emmcio_emmc_get_count(mp_obj_t self_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(common_hal_emmcio_emmc_get_block_count(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(emmcio_emmc_get_count_obj, emmcio_emmc_get_count);
MP_PROPERTY_GETTER(emmcio_emmc_count_obj, (mp_obj_t)&emmcio_emmc_get_count_obj);

//|     block_size: int
//|     """The size of one block, in bytes."""
//|
static mp_obj_t emmcio_emmc_get_block_size(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(EMMC_BLOCK_SIZE);
}
static MP_DEFINE_CONST_FUN_OBJ_1(emmcio_emmc_get_block_size_obj, emmcio_emmc_get_block_size);
MP_PROPERTY_GETTER(emmcio_emmc_block_size_obj, (mp_obj_t)&emmcio_emmc_get_block_size_obj);

//|     cid: bytes
//|     """The card's 16-byte identification register."""
//|
static mp_obj_t emmcio_emmc_get_cid(mp_obj_t self_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bytes(common_hal_emmcio_emmc_get_cid(self), 16);
}
static MP_DEFINE_CONST_FUN_OBJ_1(emmcio_emmc_get_cid_obj, emmcio_emmc_get_cid);
MP_PROPERTY_GETTER(emmcio_emmc_cid_obj, (mp_obj_t)&emmcio_emmc_get_cid_obj);

//|     write_enabled: bool
//|     """Whether `writeblocks()` is permitted on this object."""
//|
static mp_obj_t emmcio_emmc_get_write_enabled(mp_obj_t self_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(self->write_enabled);
}
static MP_DEFINE_CONST_FUN_OBJ_1(emmcio_emmc_get_write_enabled_obj, emmcio_emmc_get_write_enabled);
MP_PROPERTY_GETTER(emmcio_emmc_write_enabled_obj, (mp_obj_t)&emmcio_emmc_get_write_enabled_obj);

//|     high_speed: bool
//|     """Whether the card is running at its faster clock rate."""
//|
static mp_obj_t emmcio_emmc_get_high_speed(mp_obj_t self_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_emmcio_emmc_get_high_speed(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(emmcio_emmc_get_high_speed_obj, emmcio_emmc_get_high_speed);
MP_PROPERTY_GETTER(emmcio_emmc_high_speed_obj, (mp_obj_t)&emmcio_emmc_get_high_speed_obj);

//|     frequency: int
//|     """The bus clock rate in Hz."""
//|
static mp_obj_t emmcio_emmc_get_frequency(mp_obj_t self_in) {
    emmcio_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(common_hal_emmcio_emmc_get_frequency(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(emmcio_emmc_get_frequency_obj, emmcio_emmc_get_frequency);
MP_PROPERTY_GETTER(emmcio_emmc_frequency_obj, (mp_obj_t)&emmcio_emmc_get_frequency_obj);

static const mp_rom_map_elem_t emmcio_emmc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&emmcio_emmc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&emmcio_emmc___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&emmcio_emmc_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&emmcio_emmc_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&emmcio_emmc_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_ext_csd), MP_ROM_PTR(&emmcio_emmc_read_ext_csd_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&emmcio_emmc_status_obj) },

    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&emmcio_emmc_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_high_speed), MP_ROM_PTR(&emmcio_emmc_high_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&emmcio_emmc_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_block_size), MP_ROM_PTR(&emmcio_emmc_block_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_cid), MP_ROM_PTR(&emmcio_emmc_cid_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_enabled), MP_ROM_PTR(&emmcio_emmc_write_enabled_obj) },
};
static MP_DEFINE_CONST_DICT(emmcio_emmc_locals_dict, emmcio_emmc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    emmcio_emmc_type,
    MP_QSTR_EMMC,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &emmcio_emmc_locals_dict,
    make_new, emmcio_emmc_make_new
    );
