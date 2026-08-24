// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// The hardware the eMMC driver drives: the five pins, the RTC2 tick source
// and the SPIM3 data engine.


#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nrf_gpio.h"

// CLK and DAT0 must be on port 0: the data path drives them through NRF_P0
// directly. RST and VCCQ are optional
#define EMMC_NO_PIN 0xFFu

typedef struct {
    uint8_t clk;
    uint8_t cmd;
    uint8_t dat0;
    uint8_t rst;         // active low; may be EMMC_NO_PIN
    uint8_t vccq;        // I/O rail gate; may be EMMC_NO_PIN
    uint32_t clk_bit;    // port-0 masks, for the data path's direct register use
    uint32_t dat_bit;
    volatile uint32_t *dat0_cnf;
    uint32_t dat0_cnf_in;    // input, buffer connected, pull-up
    uint32_t dat0_cnf_out;   // output, input disconnected, no pull, H0H1
} emmc_pinout_t;

#define EMMC_CNF_IN  ((GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) | \
    (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | \
    (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) | \
    (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) | \
    (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos))

#define EMMC_CNF_OUT_H0H1  ((GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) | \
    (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) | \
    (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) | \
    (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos) | \
    (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos))

extern emmc_pinout_t emmc_pinout;

static inline void emmc_opt_pin_write(uint8_t pin, bool high) {
    if (pin == EMMC_NO_PIN) {
        return;
    }
    if (high) {
        nrf_gpio_pin_set(pin);
    } else {
        nrf_gpio_pin_clear(pin);
    }
}

static inline void emmc_opt_pin_output(uint8_t pin) {
    if (pin != EMMC_NO_PIN) {
        nrf_gpio_cfg_output(pin);
    }
}

// SPIM3 clock codes.
#define SPIM_FREQ_M16 0x0A000000u
#define SPIM_FREQ_M32 0x14000000u

// SPIM3 CONFIG codes
#define SPIM_CONFIG_MODE0 0u                  // MSB first, CPOL0/CPHA0
#define SPIM_CONFIG_MODE1 (1u << 1)           // MSB first, CPOL0/CPHA1

// ---- pin control ---------------------------------------------------------
// The command/init path uses the HAL macros; the data path uses the direct
// port-0 register accesses below (~3 cycles vs ~130 for the HAL, which is the
// difference between a usable bit-bang clock and a useless one).
#define CLK_HIGH()   nrf_gpio_pin_set(emmc_pinout.clk)
#define CLK_LOW()    nrf_gpio_pin_clear(emmc_pinout.clk)
#define CMD_HIGH()   nrf_gpio_pin_set(emmc_pinout.cmd)
#define CMD_LOW()    nrf_gpio_pin_clear(emmc_pinout.cmd)
#define DAT0_HIGH()  nrf_gpio_pin_set(emmc_pinout.dat0)
#define DAT0_LOW()   nrf_gpio_pin_clear(emmc_pinout.dat0)
#define DAT0_IN()    (*emmc_pinout.dat0_cnf = emmc_pinout.dat0_cnf_in)
// DAT0 as a HIGH-DRIVE output (H0H1) so edges are fast and clean.
#define DAT0_OUT()   (*emmc_pinout.dat0_cnf = emmc_pinout.dat0_cnf_out)
#define CMD_IN()     nrf_gpio_cfg_input(emmc_pinout.cmd, NRF_GPIO_PIN_PULLUP)
#define CMD_OUT()    nrf_gpio_cfg_output(emmc_pinout.cmd)
#define READ_CMD()   nrf_gpio_pin_read(emmc_pinout.cmd)
#define READ_DAT0()  nrf_gpio_pin_read(emmc_pinout.dat0)

#define RCLK_HIGH(bit)  (NRF_P0->OUTSET = (bit))
#define RCLK_LOW(bit)   (NRF_P0->OUTCLR = (bit))
#define RDAT_HIGH(bit)  (NRF_P0->OUTSET = (bit))
#define RDAT_GET(bit)   ((NRF_P0->IN & (bit)) != 0u)
// A few NOPs of settle after a clock edge for the delay-free (hd==0) path:
// covers the card's data-output valid time without throttling to a busy-wait.
#define EDGE_SETTLE() __asm__ volatile ("nop\nnop\nnop")

#define RST_ASSERT()   emmc_opt_pin_write(emmc_pinout.rst, false)
#define RST_RELEASE()  emmc_opt_pin_write(emmc_pinout.rst, true)
#define VCCQ_ON()      emmc_opt_pin_write(emmc_pinout.vccq, true)
#define VCCQ_OFF()     emmc_opt_pin_write(emmc_pinout.vccq, false)

static inline void emmc_pins_init(void) {
    nrf_gpio_cfg(emmc_pinout.clk, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_H0H1, NRF_GPIO_PIN_NOSENSE);  // high-drive CLK
    nrf_gpio_cfg_output(emmc_pinout.cmd);
    DAT0_OUT();                                                          // high-drive DAT0
    emmc_opt_pin_output(emmc_pinout.rst);
    // VCCQ: standard drive. Do NOT "improve" this to H0H1 without evidence --
    // the rail gate does not need the extra drive and the card came up on it.
    emmc_opt_pin_output(emmc_pinout.vccq);
}

static inline void emmc_pins_release(void) {
    nrf_gpio_cfg_default(emmc_pinout.clk);
    nrf_gpio_cfg_default(emmc_pinout.cmd);
    nrf_gpio_cfg_default(emmc_pinout.dat0);
    if (emmc_pinout.rst != EMMC_NO_PIN) {
        nrf_gpio_cfg_default(emmc_pinout.rst);
    }
    // VCCQ stays an output, driven low: the rail must stay off, not float.
}

// ---- the write path's DMA buffer (anomaly 198) ----------------------------
// SPIM3 on the nRF52840 corrupts TX bytes when EasyDMA reads them out of the
// upper RAM regions while the CPU is busy elsewhere (errata 198). The port
// already reserves 8 KiB of low RAM for exactly this (mpconfigport.h:36,
// SPIM3_BUFFER_RAM_START_ADDR), and busio's SPI uses it for the same reason --
// which is safe to share because an emmcio.EMMC object owns SPIM3 outright
// while it lives: busio's allocator asks emmcio_spim3_in_use() and falls back
// to SPIM0/1/2, so the two can never have a transfer in flight at once.
//
// The write path relies on this buffer rather than on retrying a bad CRC
// status; the status token is still enforced as the backstop.
#define EMMC_TX_FRAME  ((uint8_t *)SPIM3_BUFFER_RAM_START_ADDR)

// ---- time ----------------------------------------------------------------
// Free-running 32768 Hz counter (RTC2, the supervisor's tick source). 24-bit,
// so differences must be masked; it wraps every 512 s.
#define EMMC_TICKS_HZ      32768u
#define EMMC_TICK_MASK     0x00FFFFFFu
#define EMMC_TICKS()       (NRF_RTC2->COUNTER)

static inline uint32_t ticks_since_raw(uint32_t t0) {
    return (EMMC_TICKS() - t0) & EMMC_TICK_MASK;
}

// 20 ms, ~75x the 260 us a full block takes at M16.
#define EMMC_SPIM_TIMEOUT_TICKS  ((EMMC_TICKS_HZ * 20u) / 1000u)

// ---- SPIM3 data engine ---------------------------------------------------
// SPIM3 is the only instance that runs above 8 MHz. M16 = 16 MHz, the fastest
// in-spec step for this card at power-on timing (TRAN_SPEED 0x32 -> 26 MHz cap
// in backwards-compatible mode). The bus is fixed there; the ONE way it moves
// is emmc_set_high_speed(), which first gets the card's own EXT_CSD to read
// back HS_TIMING = 1 (52 MHz limit) and only then steps to M32.
// There is still no free-floating "speed knob": the two codes at the top of
// this file are the only values ever written.
//
// NRF_SPIM3->FREQUENCY and ->CONFIG are read and written directly: the
// peripheral register is the state, and it survives
// ENABLE=0 between transfers. emmc_spim_init() puts both back to M16 /
// mode 0 on every init, so a fresh object always starts at compat speed even
// if the previous one ran high.

static inline void emmc_spim_init(void) {
    NRF_SPIM3->ENABLE = 0;
    NRF_SPIM3->PSEL.SCK = emmc_pinout.clk;
    NRF_SPIM3->PSEL.MOSI = 0xFFFFFFFFu;   // attached per-transfer (write path only)
    NRF_SPIM3->PSEL.MISO = 0xFFFFFFFFu;
    NRF_SPIM3->PSEL.CSN = 0xFFFFFFFFu;
    NRF_SPIM3->FREQUENCY = SPIM_FREQ_M16;
    NRF_SPIM3->CONFIG = SPIM_CONFIG_MODE0;   // the card comes up in compat timing
    NRF_SPIM3->ORC = 0xFF;                // idle-high filler
}

static inline void emmc_spim_deinit(void) {
    NRF_SPIM3->ENABLE = 0;
    NRF_SPIM3->PSEL.SCK = 0xFFFFFFFFu;
    NRF_SPIM3->PSEL.MOSI = 0xFFFFFFFFu;
    NRF_SPIM3->PSEL.MISO = 0xFFFFFFFFu;
}

// One blocking DMA transfer with the wires temporarily owned by SPIM. While
// ENABLED the peripheral drives SCK (+MOSI for TX) / samples MISO; on disable
// the pins fall back to their GPIO latches (CLK low, DAT0 as configured), so
// the surrounding bit-bang phases continue seamlessly.
//
// A read is rx-only (MOSI unselected), so SPIM3 anomaly 198 (TX corruption)
// cannot bite there at all. Writes make TX real, which is why their frame is
// built in EMMC_TX_FRAME above.
static inline void emmc_spim_xfer(const uint8_t *tx, uint32_t txlen, uint8_t *rx, uint32_t rxlen) {
    NRF_SPIM3->PSEL.MOSI = tx ? emmc_pinout.dat0 : 0xFFFFFFFFu;
    NRF_SPIM3->PSEL.MISO = rx ? emmc_pinout.dat0 : 0xFFFFFFFFu;
    NRF_SPIM3->ENABLE = 7;
    NRF_SPIM3->TXD.PTR = (uint32_t)tx;
    NRF_SPIM3->TXD.MAXCNT = tx ? txlen : 0;
    NRF_SPIM3->RXD.PTR = (uint32_t)rx;
    NRF_SPIM3->RXD.MAXCNT = rx ? rxlen : 0;
    NRF_SPIM3->EVENTS_END = 0;
    NRF_SPIM3->TASKS_START = 1;
    {
        uint32_t t0 = EMMC_TICKS();
        while (!NRF_SPIM3->EVENTS_END) {
            if (ticks_since_raw(t0) >= EMMC_SPIM_TIMEOUT_TICKS) {
                NRF_SPIM3->EVENTS_STOPPED = 0;
                NRF_SPIM3->TASKS_STOP = 1;
                // Let EasyDMA stop writing before the buffer is handed back.
                for (uint32_t i = 0; i < 10000u && !NRF_SPIM3->EVENTS_STOPPED; i++) {
                }
                break;
            }
        }
    }
    NRF_SPIM3->ENABLE = 0;
}
