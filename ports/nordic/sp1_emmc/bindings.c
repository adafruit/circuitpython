// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// `sp1emmc` -- Python access to the SP-1's 4 GB eMMC as a block device.
//
// The card is FAT-formatted (7.3.a) and the songs on it are WAV files, so this
// module is a block device and nothing more. It used to carry two pieces of
// the stock firmware's custom album format -- the TE sector decode (plan 7.2
// D8) and the stem loader's COBS packet layer (7.2.w W6) -- both removed on
// 2026-08-17; te_song_decode/ in the repo root keeps them if they are ever
// wanted back.
//
//     import sp1emmc
//     e = sp1emmc.EMMC()          # VCCQ on, reset released, CMD0..16, EXT_CSD
//     e = sp1emmc.EMMC(high_speed=True)   # ... then HS_TIMING=1 + 32 MHz (D11)
//     e.count                     # 7_733_248 blocks of 512 bytes
//     buf = bytearray(512)
//     e.readblocks(0, buf)
//     e.deinit()
//
// Writing takes BOTH gates (7.2.w W2): SP1_EMMC_WRITE in the build, and
//
//     e = sp1emmc.EMMC(write_enabled=True)
//     e.writeblocks(0, buf)       # 512-byte multiples, no address floor
//
// A default EMMC() is exactly the read-only object 7.2 measured -- writeblocks
// raises on it. The two keywords compose:
//
//     e = sp1emmc.EMMC(high_speed=True, write_enabled=True)   # 7.2.w.g
//
// which is the read-write M32 object a FAT mount wants. 7.2.w's W3 refused
// that pairing until the write direction's launch edge had been modelled; it
// has been, and the answer was that the card's input edge never moved.
//
// There is no filesystem on this chip and this module does not invent one:
// read/writeblocks() only match the native block-device protocol's shape so a
// future FAT-on-eMMC could hand the object to storage.VfsFat unchanged.

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

#include "sp1_emmc/automount.h"
#include "sp1_emmc/blockdev.h"
#include "sp1_emmc/sp1_emmc.h"
#include "sp1_emmc/sp1_emmc_hw.h"

static bool s_constructed;

typedef struct {
    mp_obj_base_t base;
    bool deinited;
    bool write_enabled;
} sp1emmc_emmc_obj_t;

#if SP1_EMMC_AUTOMOUNT
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
    #if SP1_EMMC_AUTOMOUNT
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
    if (!g_emmc_diag.cmd0_sent) {
        return "cmd0";
    }
    if (g_emmc_diag.cmd1_retries < 0) {
        return "cmd1 (card never ready)";
    }
    if (!g_emmc_diag.cmd2_resp) {
        return "cmd2 (no CID)";
    }
    if (!g_emmc_diag.cmd3_resp) {
        return "cmd3";
    }
    if (!g_emmc_diag.cmd7_resp) {
        return "cmd7 (select)";
    }
    if (!g_emmc_diag.cmd16_resp) {
        return "cmd16 (blocklen)";
    }
    return "ext_csd";
}


#if SP1_EMMC_HS_TIMING
static const char *hs_failure_stage(void) {.
    switch (g_emmc_diag.hs_stage) {
        case 0:
            return "DEVICE_TYPE (card does not advertise 52 MHz)";
        case 1:
            return "cmd6 (no response)";
        case 2:
            return "cmd6 busy (card never released DAT0)";
        case 3:
            return g_emmc_diag.hs_switch_error
                ? "cmd13 SWITCH_ERROR (card rejected HS_TIMING)"
                : "cmd13 (card never came back to tran)";
        case 4:
            return "readback (EXT_CSD[185] did not take)";
        default:
            return "M32 smoke test (fell back to 16 MHz)";
    }
}
#endif

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
    #if SP1_EMMC_HS_TIMING
    if (high_speed && !emmc_set_high_speed()) {
        const char *stage = hs_failure_stage();
        release_hardware();
        *hs_failed = true;
        return stage;
    }
    #else
    (void)high_speed;
    #endif
    emmc_prof_reset();
    return NULL;
}

#if SP1_EMMC_AUTOMOUNT
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
    #if SP1_EMMC_WRITE
    s_automount_obj.write_enabled = write_enabled;
    #else
    (void)write_enabled;
    s_automount_obj.write_enabled = false;
    #endif
    s_automounted = true;
    return MP_OBJ_FROM_PTR(&s_automount_obj);
}

bool sp1emmc_is_automounted(void) {
    return s_automounted;
}
#endif

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
    #if !SP1_EMMC_HS_TIMING
    if (high_speed) {
        mp_raise_ValueError(MP_ERROR_TEXT("high_speed not compiled in"));
    }
    #endif
    #if !SP1_EMMC_WRITE
    if (write_enabled) {
        mp_raise_ValueError(MP_ERROR_TEXT("write_enabled not compiled in"));
    }
    #endif

    #if SP1_EMMC_AUTOMOUNT
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

static mp_obj_t sp1emmc_emmc_deinit(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->deinited) {
        release_hardware();
        self->deinited = true;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_deinit_obj, sp1emmc_emmc_deinit);

static mp_obj_t sp1emmc_emmc_obj___exit__(size_t n_args, const mp_obj_t *args) {
    return sp1emmc_emmc_deinit(args[0]);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sp1emmc_emmc___exit___obj, 4, 4, sp1emmc_emmc_obj___exit__);

// readblocks(start_block, buf) -- len(buf) must be a multiple of 512.
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

// writeblocks(start_block, buf) -- len(buf) must be a multiple of 512.
#if SP1_EMMC_WRITE

static int emmc_write_chunked(const uint8_t *src, mp_uint_t start, mp_uint_t count, bool from_vm) {
    mp_uint_t total = emmc_block_count();
    if (count == 0 || start >= total || count > total - start) {
        return -MP_EINVAL;
    }
    for (mp_uint_t done = 0; done < count; ) {
        mp_uint_t run = MIN(CHUNK_BLOCKS, count - done);
        bool ok = false;
        for (int attempt = 0; attempt < 3 && !ok; attempt++) {
            if (attempt) {
                g_emmc_diag.wr_retries++;
            }
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
#endif

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

// The op numbers are restated in sp1_emmc.h because the driver also compiles
// for the host harness, which has no extmod headers. If upstream ever
// renumbers them, fail here rather than on the card.
MP_STATIC_ASSERT(EMMC_IOCTL_INIT == MP_BLOCKDEV_IOCTL_INIT);
MP_STATIC_ASSERT(EMMC_IOCTL_DEINIT == MP_BLOCKDEV_IOCTL_DEINIT);
MP_STATIC_ASSERT(EMMC_IOCTL_SYNC == MP_BLOCKDEV_IOCTL_SYNC);
MP_STATIC_ASSERT(EMMC_IOCTL_BLOCK_COUNT == MP_BLOCKDEV_IOCTL_BLOCK_COUNT);
MP_STATIC_ASSERT(EMMC_IOCTL_BLOCK_SIZE == MP_BLOCKDEV_IOCTL_BLOCK_SIZE);
MP_STATIC_ASSERT(EMMC_IOCTL_BLOCK_ERASE == MP_BLOCKDEV_IOCTL_BLOCK_ERASE);

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
    #if SP1_EMMC_WRITE
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->deinited) {
        return -MP_ENODEV;
    }

    if (!self->write_enabled) {
        return -MP_EROFS;
    }
    return emmc_write_chunked(buf, start_block, nblocks, false);
    #else
    (void)self_in;
    (void)buf;
    (void)start_block;
    (void)nblocks;
    return -MP_EROFS;
    #endif
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
    #if SP1_EMMC_WRITE
    return !self->deinited && self->write_enabled;
    #else
    (void)self;
    return false;
    #endif
}

// read_ext_csd() -> bytes, the card's 512-byte EXT_CSD (CMD8). Read-only;
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

// status() -> int, the card status register (CMD13 SEND_STATUS), R1 bits [39:8] of the response.
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

// reset_profile() zeros the per-block cycle accumulators.
static mp_obj_t sp1emmc_emmc_reset_profile(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    emmc_prof_reset();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_reset_profile_obj, sp1emmc_emmc_reset_profile);

// ---- properties -----------------------------------------------------------

static void diag_store(mp_obj_t dict, qstr key, mp_obj_t value) {
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(key), value);
}

static mp_obj_t sp1emmc_emmc_get_count(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(emmc_block_count());
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_count_obj, sp1emmc_emmc_get_count);
MP_PROPERTY_GETTER(sp1emmc_emmc_count_obj, (mp_obj_t)&sp1emmc_emmc_get_count_obj);

static mp_obj_t sp1emmc_emmc_get_block_size(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(EMMC_BLOCK_SIZE);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_block_size_obj, sp1emmc_emmc_get_block_size);
MP_PROPERTY_GETTER(sp1emmc_emmc_block_size_obj, (mp_obj_t)&sp1emmc_emmc_get_block_size_obj);

// cid: the 16-byte CID from CMD2. MID 0x11 (Toshiba)
static mp_obj_t sp1emmc_emmc_get_cid(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bytes(g_emmc_diag.cid, sizeof(g_emmc_diag.cid));
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_cid_obj, sp1emmc_emmc_get_cid);
MP_PROPERTY_GETTER(sp1emmc_emmc_cid_obj, (mp_obj_t)&sp1emmc_emmc_get_cid_obj);

static mp_obj_t sp1emmc_emmc_get_crc_errors(mp_obj_t self_in) {
    return mp_obj_new_int_from_uint(g_emmc_diag.crc_rd_errs);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_crc_errors_obj, sp1emmc_emmc_get_crc_errors);
MP_PROPERTY_GETTER(sp1emmc_emmc_crc_errors_obj, (mp_obj_t)&sp1emmc_emmc_get_crc_errors_obj);

static mp_obj_t sp1emmc_emmc_get_retries(mp_obj_t self_in) {
    return mp_obj_new_int_from_uint(g_emmc_diag.cmd_retries);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_retries_obj, sp1emmc_emmc_get_retries);
MP_PROPERTY_GETTER(sp1emmc_emmc_retries_obj, (mp_obj_t)&sp1emmc_emmc_get_retries_obj);

static mp_obj_t sp1emmc_emmc_get_busy_timeouts(mp_obj_t self_in) {
    return mp_obj_new_int_from_uint(g_emmc_diag.busy_timeouts);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_busy_timeouts_obj, sp1emmc_emmc_get_busy_timeouts);
MP_PROPERTY_GETTER(sp1emmc_emmc_busy_timeouts_obj, (mp_obj_t)&sp1emmc_emmc_get_busy_timeouts_obj);

static mp_obj_t sp1emmc_emmc_get_rd_wait_us_max(mp_obj_t self_in) {
    return mp_obj_new_int_from_uint(g_emmc_diag.rd_wait_us_max);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_rd_wait_us_max_obj, sp1emmc_emmc_get_rd_wait_us_max);
MP_PROPERTY_GETTER(sp1emmc_emmc_rd_wait_us_max_obj, (mp_obj_t)&sp1emmc_emmc_get_rd_wait_us_max_obj);

// write_enabled: whether this object may write at all. False by default.
static mp_obj_t sp1emmc_emmc_get_write_enabled(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(self->write_enabled);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_write_enabled_obj, sp1emmc_emmc_get_write_enabled);
MP_PROPERTY_GETTER(sp1emmc_emmc_write_enabled_obj, (mp_obj_t)&sp1emmc_emmc_get_write_enabled_obj);

static mp_obj_t sp1emmc_emmc_get_wr_busy_us_max(mp_obj_t self_in) {
    return mp_obj_new_int_from_uint(g_emmc_diag.wr_busy_us_max);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_wr_busy_us_max_obj, sp1emmc_emmc_get_wr_busy_us_max);
MP_PROPERTY_GETTER(sp1emmc_emmc_wr_busy_us_max_obj, (mp_obj_t)&sp1emmc_emmc_get_wr_busy_us_max_obj);

// write_errors: CRC-status tokens that were not 010.
static mp_obj_t sp1emmc_emmc_get_write_errors(mp_obj_t self_in) {
    return mp_obj_new_int_from_uint(g_emmc_diag.werr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_write_errors_obj, sp1emmc_emmc_get_write_errors);
MP_PROPERTY_GETTER(sp1emmc_emmc_write_errors_obj, (mp_obj_t)&sp1emmc_emmc_get_write_errors_obj);

// high_speed: True once HS_TIMING = 1 has been read back off the card AND the
// host clock is at M32. False means the bus is at 16 MHz
static mp_obj_t sp1emmc_emmc_get_high_speed(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(g_emmc_diag.hs_active);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_high_speed_obj, sp1emmc_emmc_get_high_speed);
MP_PROPERTY_GETTER(sp1emmc_emmc_high_speed_obj, (mp_obj_t)&sp1emmc_emmc_get_high_speed_obj);

// frequency: the SPIM data-phase clock in Hz
static mp_obj_t sp1emmc_emmc_get_frequency(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(emmc_bus_hz());
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_frequency_obj, sp1emmc_emmc_get_frequency);
MP_PROPERTY_GETTER(sp1emmc_emmc_frequency_obj, (mp_obj_t)&sp1emmc_emmc_get_frequency_obj);

// profile: where a block's time actually goes, in microseconds, averaged over
// every block read since the last reset_profile(). 7.2.c could only infer the
// split between the DMA and everything else; this measures it, which is what
// decides whether M32 can help (the DMA halves; the rest may not).
static mp_obj_t sp1emmc_emmc_get_profile(mp_obj_t self_in) {
    sp1emmc_emmc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    mp_obj_t d = mp_obj_new_dict(9);
    uint32_t blocks = g_emmc_diag.prof_blocks;
    uint32_t calls = g_emmc_diag.prof_calls;
    diag_store(d, MP_QSTR_ok, mp_obj_new_bool(g_emmc_diag.prof_ok));
    diag_store(d, MP_QSTR_blocks, mp_obj_new_int_from_uint(blocks));
    diag_store(d, MP_QSTR_calls, mp_obj_new_int_from_uint(calls));
    // Per-block averages in microseconds. Integer division is fine at these
    // magnitudes (hundreds of us) and keeps floats out of the hot path.
    #define PROF_US(field, n) mp_obj_new_int_from_uint( \
        (n) ? (uint32_t)((field) / (n) / (EMMC_CYCLES_HZ / 1000000u)) : 0u)
    diag_store(d, MP_QSTR_hunt_us, PROF_US(g_emmc_diag.prof_hunt_cyc, blocks));
    diag_store(d, MP_QSTR_dma_us, PROF_US(g_emmc_diag.prof_dma_cyc, blocks));
    diag_store(d, MP_QSTR_verify_us, PROF_US(g_emmc_diag.prof_verify_cyc, blocks));
    diag_store(d, MP_QSTR_block_us, PROF_US(g_emmc_diag.prof_block_cyc, blocks));
    diag_store(d, MP_QSTR_call_us, PROF_US(g_emmc_diag.prof_call_cyc, calls));
    // The command half: whole-call time minus the blocks inside it, per block.
    // This is the CMD18/CMD12 handshake plus the bounds check -- the part M32
    // does nothing for.
    uint64_t cmd_cyc = g_emmc_diag.prof_call_cyc > g_emmc_diag.prof_block_cyc
        ? g_emmc_diag.prof_call_cyc - g_emmc_diag.prof_block_cyc : 0;
    diag_store(d, MP_QSTR_cmd_us, PROF_US(cmd_cyc, blocks));
    #undef PROF_US
    return d;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_profile_obj, sp1emmc_emmc_get_profile);
MP_PROPERTY_GETTER(sp1emmc_emmc_profile_obj, (mp_obj_t)&sp1emmc_emmc_get_profile_obj);

// The whole diagnostics struct, for the bring-up harness.
static mp_obj_t sp1emmc_emmc_get_diagnostics(mp_obj_t self_in) {
    mp_obj_t d = mp_obj_new_dict(20);
    diag_store(d, MP_QSTR_cmd0_sent, mp_obj_new_bool(g_emmc_diag.cmd0_sent));
    diag_store(d, MP_QSTR_cmd1_retries, mp_obj_new_int(g_emmc_diag.cmd1_retries));
    diag_store(d, MP_QSTR_cmd2_resp, mp_obj_new_bool(g_emmc_diag.cmd2_resp));
    diag_store(d, MP_QSTR_cmd2_tries, mp_obj_new_int(g_emmc_diag.cmd2_tries));
    diag_store(d, MP_QSTR_cmd2_clocks, mp_obj_new_int(g_emmc_diag.cmd2_clocks));
    diag_store(d, MP_QSTR_cmd3_resp, mp_obj_new_bool(g_emmc_diag.cmd3_resp));
    diag_store(d, MP_QSTR_cmd7_resp, mp_obj_new_bool(g_emmc_diag.cmd7_resp));
    diag_store(d, MP_QSTR_cmd16_resp, mp_obj_new_bool(g_emmc_diag.cmd16_resp));
    diag_store(d, MP_QSTR_ocr, mp_obj_new_bytes(g_emmc_diag.ocr, sizeof(g_emmc_diag.ocr)));
    diag_store(d, MP_QSTR_r1, mp_obj_new_bytes(g_emmc_diag.r1, sizeof(g_emmc_diag.r1)));
    diag_store(d, MP_QSTR_cid, mp_obj_new_bytes(g_emmc_diag.cid, sizeof(g_emmc_diag.cid)));
    diag_store(d, MP_QSTR_init_us, mp_obj_new_int_from_uint(g_emmc_diag.init_us));
    diag_store(d, MP_QSTR_last_cmd_resp, mp_obj_new_bool(g_emmc_diag.last_cmd_resp));
    diag_store(d, MP_QSTR_resp_clocks, mp_obj_new_int(g_emmc_diag.resp_clocks));
    diag_store(d, MP_QSTR_rd_crc, mp_obj_new_int_from_uint(g_emmc_diag.rd_crc));
    diag_store(d, MP_QSTR_crc_errors, mp_obj_new_int_from_uint(g_emmc_diag.crc_rd_errs));
    diag_store(d, MP_QSTR_retries, mp_obj_new_int_from_uint(g_emmc_diag.cmd_retries));
    diag_store(d, MP_QSTR_busy_timeouts, mp_obj_new_int_from_uint(g_emmc_diag.busy_timeouts));
    diag_store(d, MP_QSTR_burst_aborts, mp_obj_new_int_from_uint(g_emmc_diag.burst_aborts));
    diag_store(d, MP_QSTR_rd_wait_us_max, mp_obj_new_int_from_uint(g_emmc_diag.rd_wait_us_max));
    diag_store(d, MP_QSTR_blocks_read, mp_obj_new_int_from_uint(g_emmc_diag.blocks_read));
    // The write path (7.2.w W2). These read 0/-1 on a build or a run that
    // never wrote, which is how the bring-up harness proves a read-only image
    // stayed read-only without inspecting the build flags.
    diag_store(d, MP_QSTR_werr, mp_obj_new_int_from_uint(g_emmc_diag.werr));
    diag_store(d, MP_QSTR_wr_retries, mp_obj_new_int_from_uint(g_emmc_diag.wr_retries));
    diag_store(d, MP_QSTR_wr_busy_us_max, mp_obj_new_int_from_uint(g_emmc_diag.wr_busy_us_max));
    diag_store(d, MP_QSTR_blocks_written, mp_obj_new_int_from_uint(g_emmc_diag.blocks_written));
    diag_store(d, MP_QSTR_wr_status, mp_obj_new_int(g_emmc_diag.wr_status));
    // The HS_TIMING switch (plan D11). These read 0/False on a build or a run
    // that never asked for it, which is how the harness proves no CMD6 went out.
    diag_store(d, MP_QSTR_hs_requested, mp_obj_new_bool(g_emmc_diag.hs_requested));
    diag_store(d, MP_QSTR_cmd6_sent, mp_obj_new_bool(g_emmc_diag.cmd6_sent));
    diag_store(d, MP_QSTR_cmd6_resp, mp_obj_new_bool(g_emmc_diag.cmd6_resp));
    diag_store(d, MP_QSTR_hs_switch_error, mp_obj_new_bool(g_emmc_diag.hs_switch_error));
    diag_store(d, MP_QSTR_hs_verified, mp_obj_new_bool(g_emmc_diag.hs_verified));
    diag_store(d, MP_QSTR_hs_active, mp_obj_new_bool(g_emmc_diag.hs_active));
    diag_store(d, MP_QSTR_hs_dat_phase, mp_obj_new_bool(g_emmc_diag.hs_dat_phase));
    diag_store(d, MP_QSTR_hs_timing_at_init, mp_obj_new_int_from_uint(g_emmc_diag.hs_timing_at_init));
    diag_store(d, MP_QSTR_hs_timing_readback, mp_obj_new_int_from_uint(g_emmc_diag.hs_timing_readback));
    diag_store(d, MP_QSTR_device_type, mp_obj_new_int_from_uint(g_emmc_diag.device_type));
    diag_store(d, MP_QSTR_cmd6_busy_us, mp_obj_new_int_from_uint(g_emmc_diag.cmd6_busy_us));
    diag_store(d, MP_QSTR_cmd6_tran_us, mp_obj_new_int_from_uint(g_emmc_diag.cmd6_tran_us));
    diag_store(d, MP_QSTR_hs_stage, mp_obj_new_int_from_uint(g_emmc_diag.hs_stage));
    diag_store(d, MP_QSTR_frequency, mp_obj_new_int_from_uint(emmc_bus_hz()));
    return d;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sp1emmc_emmc_get_diagnostics_obj, sp1emmc_emmc_get_diagnostics);
MP_PROPERTY_GETTER(sp1emmc_emmc_diagnostics_obj, (mp_obj_t)&sp1emmc_emmc_get_diagnostics_obj);

static const mp_rom_map_elem_t sp1emmc_emmc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&sp1emmc_emmc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&sp1emmc_emmc___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&sp1emmc_emmc_readblocks_obj) },
    #if SP1_EMMC_WRITE
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&sp1emmc_emmc_writeblocks_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&sp1emmc_emmc_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_ext_csd), MP_ROM_PTR(&sp1emmc_emmc_read_ext_csd_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&sp1emmc_emmc_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_profile), MP_ROM_PTR(&sp1emmc_emmc_reset_profile_obj) },

    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&sp1emmc_emmc_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_high_speed), MP_ROM_PTR(&sp1emmc_emmc_high_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&sp1emmc_emmc_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_profile), MP_ROM_PTR(&sp1emmc_emmc_profile_obj) },
    { MP_ROM_QSTR(MP_QSTR_block_size), MP_ROM_PTR(&sp1emmc_emmc_block_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_cid), MP_ROM_PTR(&sp1emmc_emmc_cid_obj) },
    { MP_ROM_QSTR(MP_QSTR_crc_errors), MP_ROM_PTR(&sp1emmc_emmc_crc_errors_obj) },
    { MP_ROM_QSTR(MP_QSTR_retries), MP_ROM_PTR(&sp1emmc_emmc_retries_obj) },
    { MP_ROM_QSTR(MP_QSTR_busy_timeouts), MP_ROM_PTR(&sp1emmc_emmc_busy_timeouts_obj) },
    { MP_ROM_QSTR(MP_QSTR_rd_wait_us_max), MP_ROM_PTR(&sp1emmc_emmc_rd_wait_us_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_enabled), MP_ROM_PTR(&sp1emmc_emmc_write_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_errors), MP_ROM_PTR(&sp1emmc_emmc_write_errors_obj) },
    { MP_ROM_QSTR(MP_QSTR_wr_busy_us_max), MP_ROM_PTR(&sp1emmc_emmc_wr_busy_us_max_obj) },
    { MP_ROM_QSTR(MP_QSTR_diagnostics), MP_ROM_PTR(&sp1emmc_emmc_diagnostics_obj) },
};
static MP_DEFINE_CONST_DICT(sp1emmc_emmc_locals_dict, sp1emmc_emmc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    sp1emmc_emmc_type,
    MP_QSTR_EMMC,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &sp1emmc_emmc_locals_dict,
    make_new, sp1emmc_emmc_make_new
    );

// ---- module ---------------------------------------------------------------

// Module-level diagnostics and the automount flag (7.3.f). With the supervisor
// holding the card there is no EMMC object to ask, and the integrity counters
// are exactly what a USB transfer has to be graded on -- `crc_errors` after a
// desktop copies a file is the whole acceptance test. Cheap to expose: the
// getter never looked at `self` in the first place, because every counter it
// reports lives in g_emmc_diag.
static mp_obj_t sp1emmc_diagnostics(void) {
    return sp1emmc_emmc_get_diagnostics(mp_const_none);
}
static MP_DEFINE_CONST_FUN_OBJ_0(sp1emmc_diagnostics_obj, sp1emmc_diagnostics);

// True when the card belongs to the supervisor's /sd mount, false otherwise
static mp_obj_t sp1emmc_automounted(void) {
    return mp_obj_new_bool(sp1emmc_is_automounted());
}
static MP_DEFINE_CONST_FUN_OBJ_0(sp1emmc_automounted_obj, sp1emmc_automounted);

static const mp_rom_map_elem_t sp1emmc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sp1emmc) },
    { MP_ROM_QSTR(MP_QSTR_EMMC), MP_ROM_PTR(&sp1emmc_emmc_type) },
    { MP_ROM_QSTR(MP_QSTR_diagnostics), MP_ROM_PTR(&sp1emmc_diagnostics_obj) },
    { MP_ROM_QSTR(MP_QSTR_automounted), MP_ROM_PTR(&sp1emmc_automounted_obj) },
};
static MP_DEFINE_CONST_DICT(sp1emmc_module_globals, sp1emmc_module_globals_table);

const mp_obj_module_t sp1emmc_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sp1emmc_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_sp1emmc, sp1emmc_module);
