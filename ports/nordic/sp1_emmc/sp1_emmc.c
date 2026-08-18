// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// ============================================================================
//  SP-1 eMMC flash driver (1-bit MMC protocol over the nRF52840)
//  Read and write paths; writing is gated at runtime by
//  EMMC(write_enabled=True).
// ============================================================================
//    Two layers:
//
//    * COMMAND / control phases (init, CMD17/18 headers, busy polling) are
//      bit-banged on GPIO. They are short and timing-insensitive.
//
//    * The 512-byte DATA payloads ride SPIM3 + EasyDMA at 16 MHz. eMMC DAT0 at
//      default speed is SPI-mode-0 compatible: the host launches data while
//      CLK is low, the card samples (and launches) on the rising edge, MSB
//      first. The start-bit hunt is bit-banged, then the payload + CRC16 is
//      exactly byte-aligned for one RX DMA; on ENABLE=0 the pins fall back to
//      their GPIO latches, so the surrounding bit-bang continues seamlessly.
//
//  INTEGRITY: every block read is verified against the card's CRC16 and the
//  caller retries on a mismatch.
//
// ============================================================================

#include "sp1_emmc.h"
#include "sp1_emmc_hw.h"

#include <string.h>

#define CMD_SAFE_HALF_US 1u   // slow clock for the IDENTIFICATION phase only

// Command-phase half-period: starts safe (eMMC identification requires a slow
// clock), switched to 0 (full-speed bit-bang, ~1-2 MHz) once init completes.
static uint32_t s_cmd_half_us = CMD_SAFE_HALF_US;

volatile uint32_t g_emmc_clk_half_us = CMD_SAFE_HALF_US;

sp1_emmc_state_t g_emmc_state;

static bool s_ready;
static uint32_t s_rca;
static uint32_t s_block_count;        // from EXT_CSD SEC_COUNT; 0 = not read yet
static uint8_t s_device_type;         // from EXT_CSD[196]; 0 = not read yet

// One 512-byte block + its CRC16, byte-aligned, for the RX DMA.
static uint8_t s_dma_rx[EMMC_BLOCK_SIZE + 2];

#define HALF(hd)  do { if (hd) { EMMC_DELAY_US(hd); } } while (0)

// ---- bounded-wait helpers --------------------------------------------------
// The 32768 Hz counter is 24-bit, so every elapsed calculation masks.
#define US_TO_TICKS(us) ((uint32_t)(((uint64_t)(us) * EMMC_TICKS_HZ + 999999u) / 1000000u))

static inline uint32_t ticks_since(uint32_t t0) {
    return (emmc_ticks() - t0) & EMMC_TICK_MASK;
}

static inline void half_delay(uint32_t us) {
    if (us) {
        EMMC_DELAY_US(us);
    }
}

// Safe clock pulse for command/CRC phases.
static inline void clk_pulse(void) {
    CLK_HIGH();
    half_delay(s_cmd_half_us);
    CLK_LOW();
    half_delay(s_cmd_half_us);
}

static void cmd_send_bit(uint8_t bit) {
    // caller (send_command) sets CMD_OUT() once.
    if (bit) {
        CMD_HIGH();
    } else {
        CMD_LOW();
    }
    clk_pulse();
}

// SAMPLE POINT: read the line at the END of the low phase, i.e. before this
// bit's clock pulse, not in the middle of it. That is the one point in the
// cycle where BOTH of the card's timing modes hold valid data, which is what
// makes this path work either side of an HS_TIMING switch:
//
//   * backward-compatible timing: the card launches on the FALLING edge and
//     holds the bit until the next one, so the whole low phase is valid.
//     tOSU(min) = tWL(min) - tODLY, data good from ~8 ns after the edge.
//     We read a full low phase later.
//   * high-speed timing: the card launches on the RISING edge (tODLY, 13.7 ns
//     max, referenced to it) and holds until the next rising edge, so the low
//     phase is again inside the window.
//
static uint8_t cmd_recv_bit(void) {
    // caller sets CMD_IN() once before the response read
    uint8_t b = (uint8_t)READ_CMD();
    clk_pulse();
    return b;
}

static uint8_t crc7(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        uint8_t v = data[i];
        for (int b = 7; b >= 0; b--) {
            crc <<= 1;
            if (((v >> b) & 1) ^ ((crc >> 7) & 1)) {
                crc ^= 0x09;
            }
            crc &= 0x7F;
        }
    }
    return (crc << 1) | 1;
}

// Table-driven CRC16-CCITT
static uint16_t s_crc16_tab[256];
static void crc16_tab_init(void) {
    for (uint32_t i = 0; i < 256; i++) {
        uint16_t crc = (uint16_t)(i << 8);
        for (int b = 0; b < 8; b++) {
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
        }
        s_crc16_tab[i] = crc;
    }
}

__attribute__((optimize("O2")))
static uint16_t crc16(const uint8_t *data, uint32_t len) {
    uint16_t crc = 0;
    for (uint32_t i = 0; i < len; i++) {
        crc = (uint16_t)((crc << 8) ^ s_crc16_tab[(crc >> 8) ^ data[i]]);
    }
    return crc;
}

static bool send_command(uint8_t cmd_index, uint32_t arg, uint8_t *r1_out) {
    uint8_t frame[6];
    frame[0] = 0x40 | (cmd_index & 0x3F);
    frame[1] = (uint8_t)(arg >> 24);
    frame[2] = (uint8_t)(arg >> 16);
    frame[3] = (uint8_t)(arg >> 8);
    frame[4] = (uint8_t)(arg);
    frame[5] = crc7(frame, 5);

    // PRE-COMMAND GAP on an UNDRIVEN line
    CMD_IN();
    for (int i = 0; i < 24; i++) {
        clk_pulse();
    }
    CMD_OUT();
    cmd_send_bit(0);
    cmd_send_bit(1);
    for (int b = 5; b >= 0; b--) {
        cmd_send_bit((frame[0] >> b) & 1);
    }
    for (int i = 1; i <= 4; i++) {
        for (int b = 7; b >= 0; b--) {
            cmd_send_bit((frame[i] >> b) & 1);
        }
    }
    for (int b = 7; b >= 1; b--) {
        cmd_send_bit((frame[5] >> b) & 1);
    }
    cmd_send_bit(1);

    CMD_IN();
    bool responded = false;
    for (int t = 0; t < 200; t++) {
        clk_pulse();
        if (!READ_CMD()) {
            responded = true;
            break;
        }
    }
    if (!responded) {
        return false;
    }

    if (!r1_out) {
        return true;
    }

    uint8_t resp[6] = {0};
    for (int i = 0; i < 38; i++) {
        uint8_t bit = cmd_recv_bit();
        resp[i / 8] |= (bit << (7 - (i % 8)));
    }
    memcpy(r1_out, resp, 6);

    // Leave CMD as an INPUT (pulled up)
    return true;
}

// Bit-banged MMC commands intermittently miss the response on the first try
// (settling after the previous command); retry until the card answers.
static bool send_command_retry(uint8_t cmd, uint32_t arg, uint8_t *r1_out, int tries) {
    for (int t = 0; t < tries; t++) {
        if (send_command(cmd, arg, r1_out)) {
            return true;
        }
        if (t == 0) {
            // First miss = the card still settling after the previous burst: a
            // handful of idle clocks is all it needs.
            for (int c = 0; c < 16; c++) {
                clk_pulse();
            }
        } else {
            EMMC_SLEEP_MS(2);
        }
    }
    return false;
}

// DATA read: per-bit CLK toggle uses the configurable (possibly 0) half-period.
__attribute__((optimize("O2")))   // read path only: -O2 safe for reads, NOT writes
static bool read_data_block(uint8_t *buf) {
    const uint32_t hd = g_emmc_clk_half_us;

    DAT0_IN();
    // START-BIT HUNT
    {
        uint32_t t0 = emmc_ticks();
        const uint32_t lim = US_TO_TICKS(80000u);      // 80 ms bound
        const uint32_t yield_at = US_TO_TICKS(500u);
        bool got_start = false;
        for (;;) {
            // This hunt samples in the HIGH phase and stays there in both
            // timing modes
            for (int burst = 0; burst < 64 && !got_start; burst++) {
                RCLK_HIGH();
                HALF(hd);
                EDGE_SETTLE();
                if (!RDAT_GET()) {
                    got_start = true;    // leave with RCLK HIGH (as before)
                    break;
                }
                RCLK_LOW();
                HALF(hd);
            }
            uint32_t el = ticks_since(t0);
            if (got_start) {
                break;
            }
            if (el >= lim) {
                return false;
            }
            if (el >= yield_at) {
                emmc_yield();
            }
        }
    }
    RCLK_LOW();
    HALF(hd);

    // The start bit was just consumed by the bit-bang hunt above, so the
    // remaining 512 data bytes + CRC16 are exactly byte-aligned.
    sp1_emmc_spim_xfer(NULL, 0, s_dma_rx, sizeof(s_dma_rx));
    memcpy(buf, s_dma_rx, EMMC_BLOCK_SIZE);
    uint16_t card_crc = (uint16_t)(((uint16_t)s_dma_rx[EMMC_BLOCK_SIZE] << 8) |
        s_dma_rx[EMMC_BLOCK_SIZE + 1]);
    RCLK_HIGH();
    HALF(hd);
    RCLK_LOW();
    HALF(hd);                                          // end bit
    bool crc_ok = crc16(buf, EMMC_BLOCK_SIZE) == card_crc;
    DAT0_OUT();
    DAT0_HIGH();
    return crc_ok;                                     // a mismatch: caller retries
}

bool emmc_cmd13(uint8_t *r1_out) {
    return send_command_retry(13, s_rca, r1_out, 8);
}

// Clock out an R2 response and reassemble the CID. R2 framing: start(0) +
// transmission(0) + 6 reserved ones + CID[127:1] + end(1) = 136 bits.
static void drain_r2_cid(uint8_t *cid_out) {
    uint8_t bits[136];
    for (int i = 0; i < 136; i++) {
        bits[i] = cmd_recv_bit();
    }
    memset(cid_out, 0, 16);
    for (int i = 0; i < 128; i++) {
        // bits[0] start, bits[1] transmission, bits[2..7] six reserved ones,
        // bits[8..134] CID[127:1], bits[135] end bit
        cid_out[i / 8] |= (uint8_t)(bits[8 + i] << (7 - (i % 8)));
    }
}

bool emmc_init(void) {
    s_ready = false;
    s_block_count = 0;
    s_device_type = 0;
    g_emmc_clk_half_us = CMD_SAFE_HALF_US;
    s_cmd_half_us = CMD_SAFE_HALF_US;
    memset(&g_emmc_state, 0, sizeof(g_emmc_state));
    g_emmc_state.cmd1_retries = -1;

    sp1_emmc_pins_init();
    sp1_emmc_spim_init();                // hardware-clocked data path, at M16
    crc16_tab_init();

    CLK_LOW();
    CMD_HIGH();
    DAT0_HIGH();

    VCCQ_ON();
    EMMC_SLEEP_MS(10);

    RST_ASSERT();
    EMMC_SLEEP_MS(1);
    RST_RELEASE();
    EMMC_SLEEP_MS(2);

    CMD_HIGH();
    for (int i = 0; i < 80; i++) {       // 74+ clocks before the first command
        clk_pulse();
    }

    send_command(0, 0x00000000, NULL);   // CMD0 GO_IDLE (no response expected)
    g_emmc_state.cmd0_sent = true;
    EMMC_SLEEP_MS(1);

    // CMD1 SEND_OP_COND, arg 0x40FF8000: HCS=1
    uint8_t r3[6] = {0};
    for (int retry = 0; retry < 1000; retry++) {
        bool ok = send_command(1, 0x40FF8000, r3);
        emmc_feed();
        EMMC_SLEEP_MS(1);
        if (ok && (r3[1] & 0x80)) {      // response seen AND busy bit set = ready
            g_emmc_state.cmd1_retries = retry;
            break;
        }
    }
    if (g_emmc_state.cmd1_retries < 0) {  // card never responded ready -> stop
        return false;
    }

    for (int t = 0; t < 8; t++) {
        g_emmc_state.cmd2_resp = send_command(2, 0, NULL);
        if (g_emmc_state.cmd2_resp) {
            drain_r2_cid(g_emmc_state.cid);
            break;
        }
        EMMC_SLEEP_MS(2);
    }
    EMMC_SLEEP_MS(1);

    uint8_t r6[6] = {0};
    s_rca = 0x0001u << 16;
    g_emmc_state.cmd3_resp = send_command_retry(3, s_rca, r6, 8);   // SET_RELATIVE_ADDR
    EMMC_SLEEP_MS(1);

    uint8_t r1[6] = {0};
    g_emmc_state.cmd7_resp = send_command_retry(7, s_rca, r1, 8);   // SELECT_CARD
    EMMC_SLEEP_MS(1);
    g_emmc_state.cmd16_resp = send_command_retry(16, EMMC_BLOCK_SIZE, r1, 8); // SET_BLOCKLEN
    EMMC_SLEEP_MS(1);

    // strict: ready only if the card actually selected AND accepted block length
    s_ready = g_emmc_state.cmd7_resp && g_emmc_state.cmd16_resp;
    if (s_ready) {
        s_cmd_half_us = 0u;              // identification done: full-speed commands
        g_emmc_clk_half_us = 0u;
    }
    return s_ready;
}

bool emmc_is_ready(void) {
    return s_ready;
}

uint32_t emmc_block_count(void) {
    return s_block_count;
}

// The block-device ioctl
bool emmc_blockdev_ioctl(uint32_t op, uint32_t arg, uint32_t *out_value) {
    (void)arg;
    *out_value = 0;
    switch (op) {
        case EMMC_IOCTL_INIT:
            // The constructor already did the whole CMD0..CMD16 + EXT_CSD
            // walk, or raised. 0 means "initialised"; a card that has since
            // been deinited answers with the error the callers check for
            // (s_ready), so a mount over a dead object fails at INIT rather
            // than at the first read.
            *out_value = s_ready ? 0u : 1u;
            break;
        case EMMC_IOCTL_DEINIT:
        case EMMC_IOCTL_SYNC:
        case EMMC_IOCTL_BLOCK_ERASE:
            break;
        case EMMC_IOCTL_BLOCK_COUNT:
            *out_value = s_block_count;
            break;
        case EMMC_IOCTL_BLOCK_SIZE:
            *out_value = EMMC_BLOCK_SIZE;
            break;
        default:
            return false;
    }
    return true;
}

// Power-off: release the bus pins and cut the VCCQ I/O rail. With no write path
// and the card's volatile cache never enabled there is nothing to flush first,
// so board_power_off_prepare() needs no eMMC step of its own. The card is gone
// until the next emmc_init().
void emmc_power_down(void) {
    s_ready = false;
    s_block_count = 0;
    sp1_emmc_spim_deinit();
    RST_ASSERT();
    sp1_emmc_pins_release();
    VCCQ_OFF();                          // rail off (pin stays an output)
}

// CMD8 SEND_EXT_CSD: an ADTC (read) command -- the card responds R1, then
// sends a single 512-byte EXT_CSD data block on DAT0 exactly like CMD17.
// Read-only and safe. buf must be >= EMMC_BLOCK_SIZE.
bool emmc_read_ext_csd(uint8_t *buf) {
    if (!s_ready) {
        return false;
    }
    uint8_t r1[6];
    if (!send_command_retry(8, 0, r1, 8)) {
        return false;
    }
    if (!read_data_block(buf)) {
        return false;
    }
    // SEC_COUNT[215:212], little-endian. 0x00760000 on this part = 7,733,248
    // blocks; the value is the software LBA bound for every later read.
    s_block_count = (uint32_t)buf[212] | ((uint32_t)buf[213] << 8) |
        ((uint32_t)buf[214] << 16) | ((uint32_t)buf[215] << 24);
    // DEVICE_TYPE[196] gates the HS_TIMING switch (bit 1 = 52 MHz supported;
    // this part reads 0x57).
    s_device_type = buf[196];
    return true;
}

// ---- R1b / program busy on DAT0 --------------------------------------------
// Shared by the CMD6 switch (below) and the write path (further down): the
// card pulls DAT0 low while it programs and releases it high when done, and it
// only advances on OUR clock, so the host must keep clocking for the card to
// get anywhere.

#define EMMC_BUSY_LEADIN_CLOCKS 16

// run_bg picks the service call for a long stall
//   true  -- emmc_yield(): feed the dog AND run background tasks. That is
//            where the power-off gesture lives.
//   false -- emmc_feed(): feed the dog ONLY. Used by every wait inside a
//            write, so a gesture can never drop the rail around a card that
//            is mid-program. Detection is deferred by at most one bounded
//            wait (<=500 ms) against a 3 s hold; between blocks and between
//            calls the gesture is live as usual.
static bool dat0_busy_wait(uint32_t timeout_us, bool run_bg) {
    DAT0_IN();                                   // never drive against a busy card
    for (int i = 0; i < EMMC_BUSY_LEADIN_CLOCKS; i++) {
        clk_pulse();
    }
    uint32_t t0 = emmc_ticks();
    const uint32_t lim = US_TO_TICKS(timeout_us);
    for (;;) {
        bool released = false;
        for (int i = 0; i < 64 && !released; i++) {
            CLK_HIGH();
            half_delay(s_cmd_half_us);
            released = READ_DAT0() != 0;
            CLK_LOW();
            half_delay(s_cmd_half_us);
        }
        uint32_t el = ticks_since(t0);
        if (released) {
            DAT0_OUT();                          // back to the read path's resting state
            DAT0_HIGH();
            return true;
        }
        if (el >= lim) {
            // DAT0 STAYS AN INPUT on a timeout
            return false;
        }
        if (run_bg) {
            emmc_yield();
        } else {
            emmc_feed();
        }
    }
}

// CMD6 SWITCH argument: access 0b11 (WRITE_BYTE) | index 185 | value 1 |
// cmd_set 0  ->  0x03 B9 01 00.
#define EMMC_SWITCH_HS_TIMING_ARG  0x03B90100u
#define EMMC_EXT_CSD_HS_TIMING     185u
#define EMMC_EXT_CSD_DEVICE_TYPE   196u
#define EMMC_DEVICE_TYPE_HS52      0x02u

// GENERIC_CMD6_TIME on this part is 0x05 = 50 ms. Ten times that is the bound.
#define EMMC_CMD6_BUSY_US  500000u

// Poll CMD13 until the card is back in tran and ready for data. This is the
// authoritative "the switch finished" test, and it is also where SWITCH_ERROR
// (status bit 7) shows up if the card rejected the write.
static bool wait_tran_after_switch(uint32_t timeout_us) {
    uint32_t t0 = emmc_ticks();
    const uint32_t lim = US_TO_TICKS(timeout_us);
    for (;;) {
        uint8_t r1[6];
        if (emmc_cmd13(r1)) {
            uint32_t status = ((uint32_t)r1[1] << 24) | ((uint32_t)r1[2] << 16) |
                ((uint32_t)r1[3] << 8) | (uint32_t)r1[4];
            if (status & (1u << 7)) {            // SWITCH_ERROR: the card said no
                g_emmc_state.hs_switch_error = true;
                return false;
            }
            if (((status >> 9) & 0xFu) == 4u && ((status >> 8) & 1u)) {
                return true;                     // tran + ready_for_data
            }
        }
        if (ticks_since(t0) >= lim) {
            return false;
        }
        emmc_yield();
        EMMC_SLEEP_MS(1);
    }
}

bool emmc_set_high_speed(void) {
    if (!s_ready) {
        return false;
    }
    // Gate on the card's own capability byte.
    if (!(s_device_type & EMMC_DEVICE_TYPE_HS52)) {
        return false;
    }
    g_emmc_state.hs_stage = 1;

    uint8_t r1[6];
    if (!send_command_retry(6, EMMC_SWITCH_HS_TIMING_ARG, r1, 8)) {
        return false;
    }
    g_emmc_state.hs_stage = 2;
    // run_bg = true: a CMD6 on a volatile byte has no in-flight card state a
    // power-off gesture could damage, so this wait services them as the read
    // path does.
    if (!dat0_busy_wait(EMMC_CMD6_BUSY_US, true)) {
        return false;
    }
    g_emmc_state.hs_stage = 3;
    if (!wait_tran_after_switch(EMMC_CMD6_BUSY_US)) {
        return false;
    }
    g_emmc_state.hs_stage = 4;

    // THE DATA PATH'S HALF OF THE SWITCH. HS_TIMING moves the edge the card
    // launches DAT0 on, from falling to rising, so SPIM has to move its sample
    // edge with it (CPHA=1) or every block after this point comes back shifted
    // by a bit and fails its CRC16. The command path needs no such flag,
    // cmd_recv_bit() reads at a point that is valid in both timings. But,
    // SPIM samples on an edge, and an edge has to pick one.
    //
    // This happens BEFORE the readback, because the readback is itself a block
    // read off a card that has already switched.
    sp1_emmc_spim_set_config(SPIM_CONFIG_MODE1);

    // Read the byte back AT THE OLD CLOCK. A card that ACKed the switch but did
    // not take it would otherwise be met with a 32 MHz bus it never agreed to,
    // and the only symptom would be CRC noise that looks like a wiring fault.
    uint8_t ext_csd[EMMC_BLOCK_SIZE];
    if (!emmc_read_ext_csd(ext_csd) ||
        ext_csd[EMMC_EXT_CSD_HS_TIMING] != 1u) {
        sp1_emmc_spim_set_config(SPIM_CONFIG_MODE0);
        return false;                            // still at M16, card still readable
    }
    g_emmc_state.hs_stage = 5;

    // Only now does the host clock move. The re-read is a smoke test of the
    // faster bus with the integrity layer watching: if the first fast transfer
    // cannot even fetch a block the card just served correctly, fall straight
    // back.
    sp1_emmc_spim_set_freq(SPIM_FREQ_M32);
    if (!emmc_read_ext_csd(ext_csd) || ext_csd[EMMC_EXT_CSD_HS_TIMING] != 1u) {
        // Back to the old CLOCK but NOT to the old phase: the card is in
        // high-speed timing and stays there until the rail drops, and
        // high-speed timing is specified from 0 Hz up. Mode 1 is how we talk
        // to it at M16 now.
        sp1_emmc_spim_set_freq(SPIM_FREQ_M16);
        return false;
    }
    g_emmc_state.hs_active = true;
    g_emmc_state.hs_stage = 6;
    return true;
}

bool emmc_read_blocks(uint32_t block_addr, uint8_t *buf, uint32_t count) {
    if (!s_ready || count == 0) {
        return false;
    }
    // Reject an out-of-range LBA before any command reaches the card
    if (s_block_count != 0 &&
        (block_addr >= s_block_count || count > s_block_count - block_addr)) {
        return false;
    }
    uint8_t r1[6];
    if (count == 1) {
        if (!send_command_retry(17, block_addr, r1, 8)) {
            return false;
        }
        return read_data_block(buf);
    }
    // RETRY like CMD17 above: at high bus duty the card intermittently misses
    // the first command after the previous burst's CMD12
    if (!send_command_retry(18, block_addr, r1, 4)) {
        return false;
    }

    uint32_t bt0 = emmc_ticks();
    const uint32_t blim = US_TO_TICKS(150000u);
    for (uint32_t i = 0; i < count; i++) {
        if (i && ticks_since(bt0) >= blim) {
            (void)send_command_retry(12, 0, r1, 3);
            return false;
        }
        if (!read_data_block(buf + i * EMMC_BLOCK_SIZE)) {
            (void)send_command_retry(12, 0, r1, 3);
            return false;
        }
    }
    (void)send_command_retry(12, 0, r1, 3);
    return true;
}

// The card declares MIN_PERF_W_* = 0x00: no minimum write performance
#define EMMC_WR_BUSY_US    500000u
// Same shape as the read side
#define EMMC_WR_BURST_US   250000u

__attribute__((optimize("Os")))
static bool write_data_block(const uint8_t *buf) {
    const uint32_t hd = g_emmc_clk_half_us;

    // Write convention: change DAT0 while CLK is LOW, then a full half-period
    // of setup before the rising edge where the card latches it. DAT0 is a
    // HIGH-DRIVE (H0H1) output.
    //
    // The frame opens with DAT0 idle-HIGH for a whole byte (the Nwr gap) so
    // the card cannot mistake a stray low for an early start bit and misframe
    // the token.
    DAT0_OUT();
    RDAT_HIGH();

    uint8_t *tx = sp1_emmc_tx_frame();   // the reserved low-RAM SPIM3 buffer
    uint16_t crc = crc16(buf, EMMC_BLOCK_SIZE);
    tx[0] = 0xFF;                                  // Nwr idle gap
    tx[1] = 0xFE;                                  // 7 idle bits + START 0
    memcpy(&tx[2], buf, EMMC_BLOCK_SIZE);
    tx[2 + EMMC_BLOCK_SIZE] = (uint8_t)(crc >> 8);
    tx[2 + EMMC_BLOCK_SIZE + 1] = (uint8_t)crc;
    RCLK_LOW();
    // Launch edge is mode 0's, always
    //
    // HS_TIMING moved the card's OUTPUT edge, and only that. Its input timing
    // is unchanged: both of the datasheet's tables (p.18 high-speed, p.19
    // backward-compatible) give tISU = tIH = 3 ns for CMD/DAT "referenced to
    // CLK", i.e. the card latches the host on the rising edge in either mode.
    // So the read path has to follow the card to CPHA=1 and the write path
    // must NOT: in mode 1 SPIM shifts MOSI on the leading edge, which is the
    // very edge the card samples -- zero setup against a 3 ns requirement,
    // and the card takes the previous bit. Mode 0 shifts on the trailing
    // edge and hands the card a whole half period of setup: 31 ns at M16,
    // 15.6 ns at M32, both an order of magnitude over tISU.
    //
    // Saving and restoring rather than assuming keeps "the peripheral
    // register IS the state" true for the read path (sp1_emmc_hw.h): this
    // function borrows the phase for one DMA and gives it back. Two register
    // writes against a ~130 us transfer.
    const uint32_t saved_cfg = sp1_emmc_spim_config();
    if (saved_cfg != SPIM_CONFIG_MODE0) {
        sp1_emmc_spim_set_config(SPIM_CONFIG_MODE0);
    }
    // The TX frame ends exactly at the crc's last bit, no trailing idle
    // byte. The card emits its CRC-status token a couple of clocks after the
    // end bit.
    sp1_emmc_spim_xfer(tx, 2u + EMMC_BLOCK_SIZE + 2u, NULL, 0);
    if (saved_cfg != SPIM_CONFIG_MODE0) {
        sp1_emmc_spim_set_config(saved_cfg);
    }
    // END bit: DAT0 is back at its GPIO latch (output HIGH) -- clock it.
    HALF(hd);
    EDGE_SETTLE();
    RCLK_HIGH();
    HALF(hd);
    RCLK_LOW();

    // CRC-status token: the card drives DAT0 low (start bit), then 3 status
    // bits -- 010 accepted, 101 CRC error, 110 write error -- then releases.
    DAT0_IN();
    int wr_status = -1;
    for (int i = 0; i < 16; i++) {
        RCLK_HIGH();
        HALF(hd);
        EDGE_SETTLE();
        int start = (int)RDAT_GET();
        RCLK_LOW();
        HALF(hd);
        if (!start) {
            int st = 0;
            for (int k = 0; k < 3; k++) {
                RCLK_HIGH();
                HALF(hd);
                EDGE_SETTLE();
                st = (st << 1) | (int)RDAT_GET();
                RCLK_LOW();
                HALF(hd);
            }
            wr_status = st;
            break;
        }
    }

    // Programming busy on DAT0
    if (!dat0_busy_wait(EMMC_WR_BUSY_US, false)) {
        return false;                    // DAT0 left an INPUT -- see the wait
    }

    // ENFORCE the token: 0b010 = accepted. Anything else -- including "never
    // saw one" -- means the card did not take the block, and returning false
    // makes the caller retry instead of believing a glitch was stored.
    if (wr_status != 0x2) {
        return false;
    }
    return true;
}

bool emmc_write_blocks(uint32_t block_addr, const uint8_t *buf, uint32_t count) {
    if (!s_ready || count == 0) {
        return false;
    }
    if (s_block_count != 0 &&
        (block_addr >= s_block_count || count > s_block_count - block_addr)) {
        return false;
    }
    uint8_t r1[6];
    if (count == 1) {
        if (!send_command_retry(24, block_addr, r1, 8)) {
            return false;
        }
        return write_data_block(buf);
    }
    // Settle-miss retry, exactly as CMD18: at high bus duty the card
    // intermittently misses the first command after the previous burst.
    if (!send_command_retry(25, block_addr, r1, 4)) {
        return false;
    }
    uint32_t bt0 = emmc_ticks();
    const uint32_t blim = US_TO_TICKS(EMMC_WR_BURST_US);
    for (uint32_t i = 0; i < count; i++) {
        if (i && ticks_since(bt0) >= blim) {
            (void)send_command_retry(12, 0, r1, 3);
            return false;
        }
        if (!write_data_block(buf + i * EMMC_BLOCK_SIZE)) {
            (void)send_command_retry(12, 0, r1, 3);
            return false;
        }
    }
    (void)send_command_retry(12, 0, r1, 3);

    (void)dat0_busy_wait(EMMC_WR_BUSY_US, false);
    return true;
}

uint32_t emmc_bus_hz(void) {
    // SPIM3's M16/M32 codes are special values, NOT points on the linear scale
    // the K125..M8 codes sit on (0x0A000000 would decode to 156 MHz there), so
    // this is a lookup and not arithmetic. Only two values are ever written.
    return sp1_emmc_spim_freq() == SPIM_FREQ_M32 ? 32000000u : 16000000u;
}

// ============================================================================
//  Deliberately not implemented
// ============================================================================
//  CMD6's dangerous clients (CACHE_CTRL, FLUSH_CACHE, HPI_MGMT, BKOPS AUTO_EN,
//  POWER_OFF_NOTIFICATION), CMD35/36/38 TRIM and the abortable HPI machinery
//  are absent rather than merely unexposed. Nothing in the read or write path
//  needs them, and each one can put the card into a state a block device has no
//  way to recover from.
//
//  CMD6 itself is compiled in for exactly one volatile byte -- see the
//  HS_TIMING block above. That is a hard-coded argument with no caller input,
//  not a general SWITCH, so none of the clients listed above become reachable.
//
//  Three invariants here look like cleanup targets and are not: the TX frame
//  must end at the CRC's last bit, DAT0 must stay an input after a busy
//  timeout, and write_data_block() must keep its -Os attribute (only a build
//  flag can undo that one, which is why it is stated per-function).
//
//  Open question: HPI_FEATURES bit 1 = 0 on this part means JEDEC wants HPI
//  signalled via CMD13 rather than CMD12. Nothing here uses HPI.
// ============================================================================
