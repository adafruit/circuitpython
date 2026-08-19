// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// SP-1 eMMC: 1-bit MMC protocol over the nRF52840 (CLK/CMD/DAT0 + RST_n and a
// VCCQ rail gate).
//
// Card: Toshiba THGBMNG5D1LBAIL, e-MMC 5.0, SEC_COUNT 0x00760000 (7,733,248
// blocks = 3.69 GiB), TRAN_SPEED 0x32 -> 26 MHz in backwards-compatible mode.
// It stays there unless the caller explicitly asks for high-speed timing.


#pragma once

#include <stdbool.h>
#include <stdint.h>

#define EMMC_BLOCK_SIZE 512u

// How far bring-up got, so a failure names the step it stopped at instead of
// leaving the caller to guess. Zeroed by emmc_init().
typedef struct {
    // ---- init progress ----
    bool cmd0_sent;
    int32_t cmd1_retries;      // retries until ready; -1 = never ready
    bool cmd2_resp;
    bool cmd3_resp;
    bool cmd7_resp;
    bool cmd16_resp;
    uint8_t cid[16];           // CMD2 R2 payload (CID[127:0])
    // ---- high-speed timing ----
    // These stay at their zero values unless emmc_set_high_speed() is called.
    bool hs_switch_error;      // CMD13 reported SWITCH_ERROR after the CMD6
    bool hs_active;            // EXT_CSD[185] verified AND the host clock is at M32
    // How far the switch got, so the failure message names a step instead of
    // inferring one from timings that are legitimately 0 on a fast card:
    // 0 not attempted, 1 DEVICE_TYPE ok, 2 CMD6 answered, 3 DAT0 released,
    // 4 back in tran, 5 EXT_CSD[185] verified, 6 running at M32.
    uint8_t hs_stage;
} sp1emmc_state_t;

extern sp1emmc_state_t g_emmc_state;

// DATA-transfer clk half-period in microseconds. 0 = fastest (no busy-wait,
// just GPIO register toggles); set to 0 by emmc_init() on success. Commands
// use a fixed safe clock during identification and 0 afterwards.
extern volatile uint32_t g_emmc_clk_half_us;

// Drop VCCQ, hold RST_n asserted and park the signal pins low for long enough
// that the card comes back from a true power-on.
void emmc_power_cycle(void);

// A wall-clock budget spanning a whole sequence of driver calls.
void emmc_deadline_set(uint32_t timeout_us);
void emmc_deadline_clear(void);
bool emmc_deadline_expired(void);

bool emmc_init(void);
uint32_t emmc_block_count(void);              // 0 until EXT_CSD has been read
bool emmc_cmd13(uint8_t *r1_out);             // SEND_STATUS -- card status R1
bool emmc_read_ext_csd(uint8_t *buf);         // CMD8 -> 512-byte EXT_CSD (read-only)
bool emmc_read_blocks(uint32_t block_addr, uint8_t *buf, uint32_t count);
void emmc_power_down(void);                   // reset asserted, pins released, VCCQ off

// Block-device ioctl, taking extmod/vfs.h's MP_BLOCKDEV_IOCTL_* ops.
bool emmc_blockdev_ioctl(uint32_t op, uint32_t arg, uint32_t *out_value);

// CMD24 (count == 1) / CMD25 + CMD12 (count > 1), each block followed by the
// card's CRC-status token and its programming busy. Direct
// writes only: the card's volatile cache is never enabled, so when this
// returns true the data is in NAND and there is nothing to flush.
bool emmc_write_blocks(uint32_t block_addr, const uint8_t *buf, uint32_t count);

uint32_t emmc_bus_hz(void);                   // the SPIM data-phase clock, Hz

// CMD6 SWITCH: EXT_CSD[185] HS_TIMING = 1, then SPIM3 to M32.
//
// Volatile byte, no user data, one hard-coded argument. Refuses to send the
// command at all unless EXT_CSD[196] DEVICE_TYPE says the part supports 52 MHz,
// and refuses to raise the host clock unless the card's own EXT_CSD reads back
// HS_TIMING = 1 at the OLD clock.
//
// Requires emmc_read_ext_csd() to have run.
bool emmc_set_high_speed(void);
