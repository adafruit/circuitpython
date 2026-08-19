// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "extmod/vfs.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared/runtime/context_manager_helpers.h"
#include "common-hal/microcontroller/Pin.h"
#include "peripherals/nrf/nrf52840/pins.h"

#include "bindings/sp1emmc/EMMC.h"
#include "sp1emmc/automount.h"
#include "sp1emmc/sp1emmc.h"

#include "nrf.h"

static bool s_constructed;

typedef struct {
    mp_obj_base_t base;
    bool deinited;
    bool write_enabled;
} sp1emmc_emmc_obj_t;

#if SP1EMMC_AUTOMOUNT
static sp1emmc_emmc_obj_t s_automount_obj;
static bool s_automounted;
#endif

static const mcu_pin_obj_t *const emmc_pins[] = {
    &pin_P0_06,   // EMMC_CLK
    &pin_P0_07,   // EMMC_DAT0
    &pin_P0_08,   // EMMC_CMD
    &pin_P1_08,   // EMMC_RESET
    &pin_P0_14,   // EMMC_VCCQ
};

bool sp1emmc_spim3_in_use(void) {
    return s_constructed;
}

static void release_hardware(void) {
    emmc_power_down();
    for (size_t i = 0; i < MP_ARRAY_SIZE(emmc_pins); i++) {
        reset_pin_number(emmc_pins[i]->number);
    }
    s_constructed = false;
}

void sp1emmc_reset(void) {
    #if SP1EMMC_AUTOMOUNT
    if (s_automounted) {
        return;
    }
    #endif
    if (s_constructed) {
        release_hardware();
    }
}

static void check_for_deinit(sp1emmc_emmc_obj_t *self) {
    if (self->deinited) {
        raise_deinited_error();
    }
}

static const char *init_failure_stage(void) {
    if (!g_emmc_state.cmd0_sent) {
        return "cmd0";
    }
    if (g_emmc_state.cmd1_retries < 0) {
        return "cmd1 (card never ready)";
    }
    if (!g_emmc_state.cmd2_resp) {
        return "cmd2 (no CID)";
    }
    if (!g_emmc_state.cmd3_resp) {
        return "cmd3";
    }
    if (!g_emmc_state.cmd7_resp) {
        return "cmd7 (select)";
    }
    if (!g_emmc_state.cmd16_resp) {
        return "cmd16 (blocklen)";
    }
    return "ext_csd";
}

static const char *hs_failure_stage(void) {
    switch (g_emmc_state.hs_stage) {
        case 0:
            return "DEVICE_TYPE (card does not advertise 52 MHz)";
        case 1:
            return "cmd6 (no response)";
        case 2:
            return "cmd6 busy (card never released DAT0)";
        case 3:
            return g_emmc_state.hs_switch_error
                ? "cmd13 SWITCH_ERROR (card rejected HS_TIMING)"
                : "cmd13 (card never came back to tran)";
        case 4:
            return "readback (EXT_CSD[185] did not take)";
        default:
            return "32 MHz smoke test (fell back to 16 MHz)";
    }
}

// Power the card up and (optionally) switch it to high speed. Shared by
// make_new() and the automount, deliberately never raises. Returns NULL
// on success, or a static stage description, *hs_failed tells the
// caller which of the two messages to use. On any failure the hardware has
// already been released, pin claims included.
static const char *emmc_power_up(bool high_speed, bool *hs_failed) {
    *hs_failed = false;
    s_constructed = true;

    if (!emmc_init()) {
        const char *stage = init_failure_stage();
        release_hardware();
        return stage;
    }

    uint8_t ext_csd[EMMC_BLOCK_SIZE];
    if (!emmc_read_ext_csd(ext_csd)) {
        release_hardware();
        return "ext_csd";
    }
    if (high_speed && !emmc_set_high_speed()) {
        const char *stage = hs_failure_stage();
        release_hardware();
        *hs_failed = true;
        return stage;
    }
    return NULL;
}

#if SP1EMMC_AUTOMOUNT
// Construct the supervisor's EMMC object. Returns MP_OBJ_NULL if the
// card cannot be brought up, and raises nothing on any path.
//
// The pins are marked never-reset as well as claimed. Claiming keeps Python
// from taking them; never-reset keeps reset_all_pins() from reconfiguring them
// out from under a live volume between VM runs.
mp_obj_t sp1emmc_automount_construct(bool high_speed, bool write_enabled) {
    if (s_constructed) {
        return MP_OBJ_NULL;
    }
    if ((NRF_SPIM3->ENABLE & SPIM_ENABLE_ENABLE_Msk) != 0) {
        return MP_OBJ_NULL;
    }
    for (size_t i = 0; i < MP_ARRAY_SIZE(emmc_pins); i++) {
        if (!pin_number_is_free(emmc_pins[i]->number)) {
            return MP_OBJ_NULL;
        }
    }
    for (size_t i = 0; i < MP_ARRAY_SIZE(emmc_pins); i++) {
        claim_pin(emmc_pins[i]);
        never_reset_pin_number(emmc_pins[i]->number);
    }
    bool hs_failed = false;
    if (emmc_power_up(high_speed, &hs_failed) != NULL) {
        return MP_OBJ_NULL;
    }
    s_automount_obj.base.type = &sp1emmc_emmc_type;
    s_automount_obj.deinited = false;
    s_automount_obj.write_enabled = write_enabled;
    s_automounted = true;
    return MP_OBJ_FROM_PTR(&s_automount_obj);
}

bool sp1emmc_is_automounted(void) {
    return s_automounted;
}

// The automount's undo. Safe to call from any of its failure paths
void sp1emmc_automount_abandon(void) {
    s_automounted = false;
    s_automount_obj.deinited = true;
    if (s_constructed) {
        release_hardware();
    }
}
#endif

//| class EMMC:
//|     """The on-board eMMC as a block device"""
//|
//|     def __init__(self, *, high_speed: bool = False, write_enabled: bool = False) -> None:
//|         """Power up the card and make it ready for block access.
//|
//|         Only one `EMMC` object may exist at a time. Call `deinit()`, or use
//|         the object as a context manager, to release the card and its pins.
//|
//|         :param bool high_speed: Run the bus at its faster clock rate. Raises
//|           an `OSError` if the card will not make the switch.
//|         :param bool write_enabled: Allow `writeblocks()`. When `False`, the
//|           object is read-only and every write path refuses.
//|
//|         :raises ValueError: if the card is already in use, or is owned by
//|           the USB drive.
//|         :raises OSError: if the card does not come up.
//|
//|         Mount the card's filesystem::
//|
//|           import sp1emmc
//|           import storage
//|
//|           emmc = sp1emmc.EMMC(high_speed=True, write_enabled=True)
//|           storage.mount(storage.VfsFat(emmc), "/sd")
//|         """
//|         ...
//|
static mp_obj_t sp1emmc_emmc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_high_speed, ARG_write_enabled };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_high_speed, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_write_enabled, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    bool high_speed = args[ARG_high_speed].u_bool;
    bool write_enabled = args[ARG_write_enabled].u_bool;

    #if SP1EMMC_AUTOMOUNT
    if (s_automounted) {
        mp_raise_ValueError(MP_ERROR_TEXT("eMMC owned by the USB drive; set CIRCUITPY_EMMC_USB = false in settings.toml"));
    }
    #endif
    if (s_constructed) {
        mp_raise_ValueError(MP_ERROR_TEXT("eMMC already in use"));
    }

    if ((NRF_SPIM3->ENABLE & SPIM_ENABLE_ENABLE_Msk) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("SPI peripheral in use"));
    }
    for (size_t i = 0; i < MP_ARRAY_SIZE(emmc_pins); i++) {
        assert_pin_free(emmc_pins[i]);
    }
    for (size_t i = 0; i < MP_ARRAY_SIZE(emmc_pins); i++) {
        claim_pin(emmc_pins[i]);
    }
    bool hs_failed = false;
    const char *stage = emmc_power_up(high_speed, &hs_failed);
    if (stage != NULL) {
        mp_raise_msg_varg(&mp_type_OSError,
            hs_failed ? MP_ERROR_TEXT("eMMC high-speed switch failed at %s")
                      : MP_ERROR_TEXT("eMMC init failed at %s"), stage);
    }

    sp1emmc_emmc_obj_t *self = mp_obj_malloc(sp1emmc_emmc_obj_t, &sp1emmc_emmc_type);
    self->deinited = false;
    self->write_enabled = write_enabled;
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Release the card and the pins it uses. Any further use of this
//|         object raises a `ValueError`."""
//|         ...
//|
static mp_obj_t sp1emmc_emmc_deinit(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->deinited) {
        release_hardware();
        self->deinited = true;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_deinit_obj, sp1emmc_emmc_deinit);

//|     def __enter__(self) -> EMMC:
//|         """No-op used by Context Managers."""
//|         ...
//|
//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
static mp_obj_t sp1emmc_emmc_obj___exit__(size_t n_args, const mp_obj_t *args) {
    return sp1emmc_emmc_deinit(args[0]);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sp1emmc_emmc___exit___obj, 4, 4, sp1emmc_emmc_obj___exit__);

#define CHUNK_BLOCKS 64u

static int emmc_read_chunked(uint8_t *out, mp_uint_t start, mp_uint_t count, bool from_vm) {
    mp_uint_t total = emmc_block_count();
    if (count == 0 || start >= total || count > total - start) {
        return -MP_EINVAL;
    }
    for (mp_uint_t done = 0; done < count; ) {
        mp_uint_t run = MIN(CHUNK_BLOCKS, count - done);
        bool ok = false;
        for (int attempt = 0; attempt < 3 && !ok; attempt++) {
            ok = emmc_read_blocks(start + done, out + done * EMMC_BLOCK_SIZE, run);
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
static mp_obj_t sp1emmc_emmc_readblocks(mp_obj_t self_in, mp_obj_t start_in, mp_obj_t buf_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.len == 0 || (bufinfo.len % EMMC_BLOCK_SIZE) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer length must be a multiple of 512"));
    }
    mp_uint_t start = mp_obj_get_int_truncated(start_in);
    mp_uint_t count = bufinfo.len / EMMC_BLOCK_SIZE;
    mp_uint_t total = emmc_block_count();
    if (start >= total || count > total - start) {
        mp_raise_ValueError(MP_ERROR_TEXT("Block address out of range"));
    }

    int err = emmc_read_chunked(bufinfo.buf, start, count, true);
    if (err != 0) {
        mp_raise_OSError(-err);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(sp1emmc_emmc_readblocks_obj, sp1emmc_emmc_readblocks);

static int emmc_write_chunked(const uint8_t *src, mp_uint_t start, mp_uint_t count, bool from_vm) {
    mp_uint_t total = emmc_block_count();
    if (count == 0 || start >= total || count > total - start) {
        return -MP_EINVAL;
    }
    for (mp_uint_t done = 0; done < count; ) {
        mp_uint_t run = MIN(CHUNK_BLOCKS, count - done);
        bool ok = false;
        for (int attempt = 0; attempt < 3 && !ok; attempt++) {
            ok = emmc_write_blocks(start + done, src + done * EMMC_BLOCK_SIZE, run);
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
static mp_obj_t sp1emmc_emmc_writeblocks(mp_obj_t self_in, mp_obj_t start_in, mp_obj_t buf_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (!self->write_enabled) {
        mp_raise_msg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Read-only: construct EMMC(write_enabled=True) to write"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len == 0 || (bufinfo.len % EMMC_BLOCK_SIZE) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer length must be a multiple of 512"));
    }
    mp_uint_t start = mp_obj_get_int_truncated(start_in);
    mp_uint_t count = bufinfo.len / EMMC_BLOCK_SIZE;
    mp_uint_t total = emmc_block_count();
    if (start >= total || count > total - start) {
        mp_raise_ValueError(MP_ERROR_TEXT("Block address out of range"));
    }

    int err = emmc_write_chunked(bufinfo.buf, start, count, true);
    if (err != 0) {
        mp_raise_OSError(-err);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(sp1emmc_emmc_writeblocks_obj, sp1emmc_emmc_writeblocks);

//|     def ioctl(self, op: int, arg: int) -> Optional[int]:
//|         """Perform a block-device control operation, as required by the
//|         block-device protocol. Returns `None` for operations this device does
//|         not implement."""
//|         ...
//|
static mp_obj_t sp1emmc_emmc_ioctl(mp_obj_t self_in, mp_obj_t op_in, mp_obj_t arg_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    uint32_t out = 0;
    if (!emmc_blockdev_ioctl(mp_obj_get_int_truncated(op_in),
        mp_obj_get_int_truncated(arg_in), &out)) {
        return mp_const_none;
    }
    return mp_obj_new_int_from_uint(out);
}
static MP_DEFINE_CONST_FUN_OBJ_3(sp1emmc_emmc_ioctl_obj, sp1emmc_emmc_ioctl);

mp_uint_t sp1emmc_emmc_readblocks_native(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t nblocks) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->deinited) {
        return -MP_ENODEV;
    }
    return emmc_read_chunked(buf, start_block, nblocks, false);
}

mp_uint_t sp1emmc_emmc_writeblocks_native(mp_obj_t self_in, const uint8_t *buf,
    uint32_t start_block, uint32_t nblocks) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->deinited) {
        return -MP_ENODEV;
    }

    if (!self->write_enabled) {
        return -MP_EROFS;
    }
    return emmc_write_chunked(buf, start_block, nblocks, false);
}

bool sp1emmc_emmc_ioctl_native(mp_obj_t self_in, uint32_t cmd, uint32_t arg,
    size_t *out_value) {

    (void)self_in;
    uint32_t out = 0;
    bool ok = emmc_blockdev_ioctl(cmd, arg, &out);
    *out_value = out;
    return ok;
}

bool sp1emmc_emmc_is_write_enabled(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return !self->deinited && self->write_enabled;
}

//|     def read_ext_csd(self) -> bytes:
//|         """Read the card's 512-byte extended CSD register."""
//|         ...
//|
static mp_obj_t sp1emmc_emmc_read_ext_csd(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    uint8_t ext_csd[EMMC_BLOCK_SIZE];
    if (!emmc_read_ext_csd(ext_csd)) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_obj_new_bytes(ext_csd, sizeof(ext_csd));
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_read_ext_csd_obj, sp1emmc_emmc_read_ext_csd);

//|     def status(self) -> int:
//|         """Read the card's 32-bit status register."""
//|         ...
//|
static mp_obj_t sp1emmc_emmc_status(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    uint8_t r1[6];
    if (!emmc_cmd13(r1)) {
        mp_raise_OSError(MP_EIO);
    }
    uint32_t status = ((uint32_t)r1[1] << 24) | ((uint32_t)r1[2] << 16) |
        ((uint32_t)r1[3] << 8) | (uint32_t)r1[4];
    return mp_obj_new_int_from_uint(status);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_status_obj, sp1emmc_emmc_status);

// ---- properties -----------------------------------------------------------

//|     count: int
//|     """The number of blocks on the card."""
//|
static mp_obj_t sp1emmc_emmc_get_count(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(emmc_block_count());
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_count_obj, sp1emmc_emmc_get_count);
MP_PROPERTY_GETTER(sp1emmc_emmc_count_obj, (mp_obj_t)&sp1emmc_emmc_get_count_obj);

//|     block_size: int
//|     """The size of one block, in bytes."""
//|
static mp_obj_t sp1emmc_emmc_get_block_size(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(EMMC_BLOCK_SIZE);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_block_size_obj, sp1emmc_emmc_get_block_size);
MP_PROPERTY_GETTER(sp1emmc_emmc_block_size_obj, (mp_obj_t)&sp1emmc_emmc_get_block_size_obj);

//|     cid: bytes
//|     """The card's 16-byte identification register."""
//|
static mp_obj_t sp1emmc_emmc_get_cid(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bytes(g_emmc_state.cid, sizeof(g_emmc_state.cid));
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_cid_obj, sp1emmc_emmc_get_cid);
MP_PROPERTY_GETTER(sp1emmc_emmc_cid_obj, (mp_obj_t)&sp1emmc_emmc_get_cid_obj);

//|     write_enabled: bool
//|     """Whether `writeblocks()` is permitted on this object."""
//|
static mp_obj_t sp1emmc_emmc_get_write_enabled(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(self->write_enabled);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_write_enabled_obj, sp1emmc_emmc_get_write_enabled);
MP_PROPERTY_GETTER(sp1emmc_emmc_write_enabled_obj, (mp_obj_t)&sp1emmc_emmc_get_write_enabled_obj);

//|     high_speed: bool
//|     """Whether the card is running at its faster clock rate."""
//|
static mp_obj_t sp1emmc_emmc_get_high_speed(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(g_emmc_state.hs_active);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_high_speed_obj, sp1emmc_emmc_get_high_speed);
MP_PROPERTY_GETTER(sp1emmc_emmc_high_speed_obj, (mp_obj_t)&sp1emmc_emmc_get_high_speed_obj);

//|     frequency: int
//|     """The bus clock rate in Hz."""
//|
static mp_obj_t sp1emmc_emmc_get_frequency(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(emmc_bus_hz());
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_frequency_obj, sp1emmc_emmc_get_frequency);
MP_PROPERTY_GETTER(sp1emmc_emmc_frequency_obj, (mp_obj_t)&sp1emmc_emmc_get_frequency_obj);

static const mp_rom_map_elem_t sp1emmc_emmc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&sp1emmc_emmc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&sp1emmc_emmc___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&sp1emmc_emmc_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&sp1emmc_emmc_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&sp1emmc_emmc_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_ext_csd), MP_ROM_PTR(&sp1emmc_emmc_read_ext_csd_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&sp1emmc_emmc_status_obj) },

    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&sp1emmc_emmc_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_high_speed), MP_ROM_PTR(&sp1emmc_emmc_high_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&sp1emmc_emmc_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_block_size), MP_ROM_PTR(&sp1emmc_emmc_block_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_cid), MP_ROM_PTR(&sp1emmc_emmc_cid_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_enabled), MP_ROM_PTR(&sp1emmc_emmc_write_enabled_obj) },
};
static MP_DEFINE_CONST_DICT(sp1emmc_emmc_locals_dict, sp1emmc_emmc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    sp1emmc_emmc_type,
    MP_QSTR_EMMC,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &sp1emmc_emmc_locals_dict,
    make_new, sp1emmc_emmc_make_new
    );
