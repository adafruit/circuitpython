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

#ifndef SP1_EMMC_WRITE
#define SP1_EMMC_WRITE (0)
#endif

// The HS_TIMING switch. Off unless the build asks for it.
#ifndef SP1_EMMC_HS_TIMING
#define SP1_EMMC_HS_TIMING (0)
#endif

#define EMMC_BLOCK_SIZE 512u

// Where init got to / how the bus is behaving.
typedef struct {
    // ---- init progress ----
    bool cmd0_sent;
    int32_t cmd1_retries;      // retries until ready; -1 = never ready
    bool cmd2_resp;
    bool cmd3_resp;
    bool cmd7_resp;
    bool cmd16_resp;
    uint8_t ocr[6];            // CMD1 R3 response bytes
    uint8_t r1[6];             // CMD7 R1 response bytes
    uint8_t cid[16];           // CMD2 R2 payload (CID[127:0])
    uint32_t init_us;          // wall-clock cost of the last emmc_init()
    // ---- last command ----
    bool last_cmd_resp;
    int32_t resp_clocks;       // clocks until last response start bit (-1 = none)
    int32_t cmd2_clocks;       // same, captured for CMD2
    int32_t cmd2_tries;        // CMD2 attempts before a response
    uint16_t rd_crc;           // CRC16 the card appended to the last read block
    // ---- integrity / stalls ----
    uint32_t crc_rd_errs;      // verified-read CRC catches (acceptance: stays 0)
    uint32_t cmd_retries;      // first-try response misses recovered
    uint32_t busy_timeouts;    // busy/hunt expiries (SEPARATE from CRC errors)
    uint32_t rd_wait_us_max;   // worst read start-bit access wait, us
    uint32_t burst_aborts;     // CMD18 bursts cut short by the burst deadline
    uint32_t blocks_read;      // blocks successfully read this session
    // ---- writing ----
    uint32_t werr;             // CRC-status tokens that were not 010 (accepted)
    uint32_t wr_retries;       // failed write attempts the caller retried
    uint32_t wr_busy_us_max;   // worst post-write program busy, us (E3/W-R9)
    uint32_t blocks_written;   // blocks the card accepted this session
    int32_t wr_status;         // last CRC-status token: 2 ok, 5 CRC, 6 write, -1 none
    // ---- high-speed timing ----
    // All of these stay at their zero values unless emmc_set_high_speed() is
    // called, which is what makes "did anything issue a CMD6?" answerable from
    // Python rather than by reading the build flags.
    bool hs_requested;         // a caller asked for the switch
    bool cmd6_sent;            // CMD6 SWITCH actually went out on the wire
    bool cmd6_resp;            // ... and the card answered R1b
    bool hs_switch_error;      // CMD13 reported SWITCH_ERROR after the CMD6
    bool hs_verified;          // EXT_CSD[185] read back as 1 at the OLD clock
    bool hs_active;            // verified AND the host clock is now at M32
    bool hs_dat_phase;         // SPIM is sampling DAT0 on the TRAILING edge,
                               // i.e. following the card into high-speed
                               // timing, where it launches on the rising edge
    uint8_t hs_timing_at_init; // EXT_CSD[185] as found, before any CMD6
    uint8_t hs_timing_readback;// EXT_CSD[185] after the CMD6
    uint8_t device_type;       // EXT_CSD[196]; bit 1 = 52 MHz supported
    uint32_t cmd6_busy_us;     // how long the card held DAT0 low after CMD6
    uint32_t cmd6_tran_us;     // ... plus the CMD13 poll back to tran
    // How far the switch got, so the failure message names a step instead of
    // inferring one from timings that are legitimately 0 on a fast card:
    // 0 not attempted, 1 DEVICE_TYPE ok, 2 CMD6 answered, 3 DAT0 released,
    // 4 back in tran, 5 EXT_CSD[185] verified, 6 running at M32.
    uint8_t hs_stage;
    // ---- per-block profile ----
    // Cycle counts from the CPU's DWT counter (64 MHz). prof_ok says whether
    // that counter is actually running; if it is not, every accumulator below
    // stays 0 rather than lying. Reset with emmc_prof_reset().
    bool prof_ok;
    uint32_t prof_blocks;      // blocks measured
    uint32_t prof_calls;       // emmc_read_blocks() calls measured
    uint64_t prof_hunt_cyc;    // bit-banged start-bit hunt
    uint64_t prof_dma_cyc;     // SPIM3 RX DMA of 512 B + CRC16
    uint64_t prof_verify_cyc;  // memcpy to the caller + CRC16 check
    uint64_t prof_block_cyc;   // whole read_data_block(), the three above + edges
    uint64_t prof_call_cyc;    // whole emmc_read_blocks(): blocks + CMD18/CMD12
} sp1_emmc_diag_t;

extern sp1_emmc_diag_t g_emmc_diag;

// DATA-transfer clk half-period in microseconds. 0 = fastest (no busy-wait,
// just GPIO register toggles); set to 0 by emmc_init() on success. Commands
// use a fixed safe clock during identification and 0 afterwards.
extern volatile uint32_t g_emmc_clk_half_us;

bool emmc_init(void);
bool emmc_is_ready(void);
uint32_t emmc_block_count(void);              // 0 until EXT_CSD has been read
bool emmc_cmd13(uint8_t *r1_out);             // SEND_STATUS -- card status R1
bool emmc_read_ext_csd(uint8_t *buf);         // CMD8 -> 512-byte EXT_CSD (read-only)
bool emmc_read_blocks(uint32_t block_addr, uint8_t *buf, uint32_t count);
void emmc_power_down(void);                   // reset asserted, pins released, VCCQ off

// The block-protocol ioctl ops (extmod/vfs.h's MP_BLOCKDEV_IOCTL_*), restated
// here because this driver also compiles for the host harness, which has no
// MicroPython headers. bindings.c static-asserts the two sets agree, so a
// future upstream renumbering stops the build rather than the card.
#define EMMC_IOCTL_INIT        1u
#define EMMC_IOCTL_DEINIT      2u
#define EMMC_IOCTL_SYNC        3u
#define EMMC_IOCTL_BLOCK_COUNT 4u
#define EMMC_IOCTL_BLOCK_SIZE  5u
#define EMMC_IOCTL_BLOCK_ERASE 6u


bool emmc_blockdev_ioctl(uint32_t op, uint32_t arg, uint32_t *out_value);

#if SP1_EMMC_WRITE
// CMD24 (count == 1) / CMD25 + CMD12 (count > 1), each block followed by the
// card's CRC-status token and its programming busy. Direct
// writes only: the card's volatile cache is never enabled, so when this
// returns true the data is in NAND and there is nothing to flush.
bool emmc_write_blocks(uint32_t block_addr, const uint8_t *buf, uint32_t count);
#endif

uint32_t emmc_bus_hz(void);                   // the SPIM data-phase clock, Hz
void emmc_prof_reset(void);                   // zero the per-block profile

#if SP1_EMMC_HS_TIMING
// CMD6 SWITCH: EXT_CSD[185] HS_TIMING = 1, then SPIM3 to M32.
//
// Volatile byte, no user data, one hard-coded argument. Refuses to send the
// command at all unless EXT_CSD[196] DEVICE_TYPE says the part supports 52 MHz,
// and refuses to raise the host clock unless the card's own EXT_CSD reads back
// HS_TIMING = 1 at the OLD clock.
//
// Requires emmc_read_ext_csd() to have run.
bool emmc_set_high_speed(void);
#endif

// ---- bindings-side hooks (bindings.c) --------------------------------------

// True while a live sp1emmc.EMMC object owns SPIM3.
bool sp1emmc_spim3_in_use(void);

// Clear module state on every VM reset. board_reset_pin_defaults() has already
// yanked the card's rail by then, so the state must not pretend to survive.
void sp1emmc_reset(void);
