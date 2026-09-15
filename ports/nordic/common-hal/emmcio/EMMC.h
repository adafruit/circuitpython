// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// The nRF52840 eMMC back end: the 1-bit MMC protocol (CLK/CMD/DAT0 plus RST_n
// and a VCCQ rail gate) bit-banged on GPIO, with the 512-byte data payloads
// carried by SPIM3 + EasyDMA.
//
// Only one EMMC object can exist at a time, it owns SPIM3.

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/emmcio/EMMC.h"

typedef struct {
    mp_obj_base_t base;
    bool deinited;
    bool write_enabled;


    // How far bring-up got, so a failure names the step it stopped at
    bool cmd0_sent;
    int32_t cmd1_retries;      // retries until ready; -1 = never ready
    bool cmd2_resp;
    bool cmd3_resp;
    bool cmd7_resp;
    bool cmd16_resp;
    uint8_t cid[16];           // CMD2 R2 payload (CID[127:0])

    // These stay at their zero values unless high_speed was asked for.
    bool hs_switch_error;      // CMD13 reported SWITCH_ERROR after the CMD6
    bool hs_active;            // EXT_CSD[185] verified AND the host clock is at M32
    // How far the switch got, so the failure message names a step
    // 0 not attempted, 1 DEVICE_TYPE ok, 2 CMD6 answered, 3 DAT0 released,
    // 4 back in tran, 5 EXT_CSD[185] verified, 6 running at M32.
    uint8_t hs_stage;
} emmcio_emmc_obj_t;

// EMMCIO_OK on success. On failure *detail carries the code's extra number,
// which the binding turns into the exception's argument.
emmcio_construct_result_t common_hal_emmcio_emmc_construct(emmcio_emmc_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command, const mcu_pin_obj_t *data,
    const mcu_pin_obj_t *reset, const mcu_pin_obj_t *vccq,
    bool high_speed, bool write_enabled, int *detail);
void common_hal_emmcio_emmc_deinit(emmcio_emmc_obj_t *self);
bool common_hal_emmcio_emmc_deinited(emmcio_emmc_obj_t *self);

bool common_hal_emmcio_emmc_readblocks(uint32_t block_addr, uint8_t *buf, uint32_t count);

// CMD24 (count == 1) / CMD25 + CMD12 (count > 1), each block followed by the
// card's CRC-status token and its programming busy. Direct writes only: the
// card's volatile cache is never enabled, so when this returns true the data
// is in NAND and there is nothing to flush.
bool common_hal_emmcio_emmc_writeblocks(uint32_t block_addr, const uint8_t *buf, uint32_t count);

// Block-device ioctl, taking extmod/vfs.h's MP_BLOCKDEV_IOCTL_* ops. False
// means the op is not implemented.
bool common_hal_emmcio_emmc_ioctl(uint32_t op, uint32_t arg, uint32_t *out_value);

// CMD8 -> the 512-byte extended CSD. buf must be >= EMMC_BLOCK_SIZE.
bool common_hal_emmcio_emmc_read_ext_csd(uint8_t *buf);

// CMD13 SEND_STATUS -- the 6-byte R1 response.
bool common_hal_emmcio_emmc_read_status(uint8_t *r1_out);

uint32_t common_hal_emmcio_emmc_get_block_count(emmcio_emmc_obj_t *self);   // 0 until EXT_CSD has been read
uint32_t common_hal_emmcio_emmc_get_frequency(emmcio_emmc_obj_t *self);     // the SPIM data-phase clock, Hz
bool common_hal_emmcio_emmc_get_high_speed(emmcio_emmc_obj_t *self);
const uint8_t *common_hal_emmcio_emmc_get_cid(emmcio_emmc_obj_t *self);     // 16 bytes, CID[127:0]

// A wall-clock budget spanning a whole sequence of driver calls, so a card
// that never answers cannot stall a caller.
void common_hal_emmcio_emmc_set_deadline(uint32_t timeout_us);
void common_hal_emmcio_emmc_clear_deadline(void);



// True while a live EMMC object owns SPIM3
bool emmcio_spim3_in_use(void);

// Power the card down and give up its pins
void emmcio_emmc_release_hardware(void);

#if CIRCUITPY_EMMC_USB
// Bring up the supervisor's statically allocated EMMC object.
// Returns MP_OBJ_NULL if the card cannot be brought up.
mp_obj_t emmcio_automount_construct(const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *reset, const mcu_pin_obj_t *vccq,
    bool high_speed, bool write_enabled);

// The automount's undo, safe to call from any of its failure paths.
void emmcio_automount_abandon(void);
#endif
