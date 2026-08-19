// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// The hardware sp1_emmc.c drives: the five pins, the RTC2 tick source and the
// SPIM3 data engine.


#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nrf_gpio.h"

// Pins (SP-1 wiring).
#define SP1_EMMC_PIN_CLK   6u    // P0.06
#define SP1_EMMC_PIN_DAT0  7u    // P0.07
#define SP1_EMMC_PIN_CMD   8u    // P0.08
#define SP1_EMMC_PIN_RST   40u   // P1.08, active low
#define SP1_EMMC_PIN_VCCQ  14u   // P0.14, I/O rail gate

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
#define CLK_HIGH()   nrf_gpio_pin_set(SP1_EMMC_PIN_CLK)
#define CLK_LOW()    nrf_gpio_pin_clear(SP1_EMMC_PIN_CLK)
#define CMD_HIGH()   nrf_gpio_pin_set(SP1_EMMC_PIN_CMD)
#define CMD_LOW()    nrf_gpio_pin_clear(SP1_EMMC_PIN_CMD)
#define DAT0_HIGH()  nrf_gpio_pin_set(SP1_EMMC_PIN_DAT0)
#define DAT0_LOW()   nrf_gpio_pin_clear(SP1_EMMC_PIN_DAT0)
#define DAT0_IN()    nrf_gpio_cfg_input(SP1_EMMC_PIN_DAT0, NRF_GPIO_PIN_PULLUP)
// DAT0 as a HIGH-DRIVE output (H0H1) so edges are fast and clean.
#define DAT0_OUT()   nrf_gpio_cfg(SP1_EMMC_PIN_DAT0, NRF_GPIO_PIN_DIR_OUTPUT, \
    NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_NOPULL, \
    NRF_GPIO_PIN_H0H1, NRF_GPIO_PIN_NOSENSE)
#define CMD_IN()     nrf_gpio_cfg_input(SP1_EMMC_PIN_CMD, NRF_GPIO_PIN_PULLUP)
#define CMD_OUT()    nrf_gpio_cfg_output(SP1_EMMC_PIN_CMD)
#define READ_CMD()   nrf_gpio_pin_read(SP1_EMMC_PIN_CMD)
#define READ_DAT0()  nrf_gpio_pin_read(SP1_EMMC_PIN_DAT0)

#define P0_CLK_BIT   (1u << SP1_EMMC_PIN_CLK)
#define P0_DAT_BIT   (1u << SP1_EMMC_PIN_DAT0)
#define RCLK_HIGH()  (NRF_P0->OUTSET = P0_CLK_BIT)
#define RCLK_LOW()   (NRF_P0->OUTCLR = P0_CLK_BIT)
#define RDAT_HIGH()  (NRF_P0->OUTSET = P0_DAT_BIT)
#define RDAT_LOW()   (NRF_P0->OUTCLR = P0_DAT_BIT)
#define RDAT_GET()   ((NRF_P0->IN >> SP1_EMMC_PIN_DAT0) & 1u)
// A few NOPs of settle after a clock edge for the delay-free (hd==0) path:
// covers the card's data-output valid time without throttling to a busy-wait.
#define EDGE_SETTLE() __asm__ volatile ("nop\nnop\nnop")

#define RST_ASSERT()   nrf_gpio_pin_clear(SP1_EMMC_PIN_RST)
#define RST_RELEASE()  nrf_gpio_pin_set(SP1_EMMC_PIN_RST)
#define VCCQ_ON()      nrf_gpio_pin_set(SP1_EMMC_PIN_VCCQ)
#define VCCQ_OFF()     nrf_gpio_pin_clear(SP1_EMMC_PIN_VCCQ)

static inline void sp1_emmc_pins_init(void) {
    nrf_gpio_cfg(SP1_EMMC_PIN_CLK, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_H0H1, NRF_GPIO_PIN_NOSENSE);  // high-drive CLK
    nrf_gpio_cfg_output(SP1_EMMC_PIN_CMD);
    DAT0_OUT();                                                          // high-drive DAT0
    nrf_gpio_cfg_output(SP1_EMMC_PIN_RST);
    // VCCQ: standard drive. Do NOT "improve" this to H0H1 without evidence --
    // the rail gate does not need the extra drive and the card came up on it.
    nrf_gpio_cfg_output(SP1_EMMC_PIN_VCCQ);
}

static inline void sp1_emmc_pins_release(void) {
    nrf_gpio_cfg_default(SP1_EMMC_PIN_CLK);
    nrf_gpio_cfg_default(SP1_EMMC_PIN_CMD);
    nrf_gpio_cfg_default(SP1_EMMC_PIN_DAT0);
    nrf_gpio_cfg_default(SP1_EMMC_PIN_RST);
    // VCCQ stays an output, driven low: the rail must stay off, not float.
}

// ---- the write path's DMA buffer (anomaly 198) ----------------------------
// SPIM3 on the nRF52840 corrupts TX bytes when EasyDMA reads them out of the
// upper RAM regions while the CPU is busy elsewhere (errata 198). The port
// already reserves 8 KiB of low RAM for exactly this (mpconfigport.h:36,
// SPIM3_BUFFER_RAM_START_ADDR), and busio's SPI uses it for the same reason --
// which is safe to share because an sp1emmc.EMMC object owns SPIM3 outright
// while it lives: busio's allocator asks sp1emmc_spim3_in_use() and falls back
// to SPIM0/1/2, so the two can never have a transfer in flight at once.
//
// The write path relies on this buffer rather than on retrying a bad CRC
// status; the status token is still enforced as the backstop.
#define SP1_EMMC_TX_FRAME  ((uint8_t *)SPIM3_BUFFER_RAM_START_ADDR)

// ---- time ----------------------------------------------------------------
// Free-running 32768 Hz counter (RTC2, the supervisor's tick source). 24-bit,
// so differences must be masked; it wraps every 512 s.
#define EMMC_TICKS_HZ      32768u
#define EMMC_TICK_MASK     0x00FFFFFFu
#define EMMC_TICKS()       (NRF_RTC2->COUNTER)

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
// peripheral register IS the state -- no shadow copy to drift, and it survives
// ENABLE=0 between transfers. sp1_emmc_spim_init() puts both back to M16 /
// mode 0 on every init, so a fresh object always starts at compat speed even
// if the previous one ran high.

static inline void sp1_emmc_spim_init(void) {
    NRF_SPIM3->ENABLE = 0;
    NRF_SPIM3->PSEL.SCK = SP1_EMMC_PIN_CLK;
    NRF_SPIM3->PSEL.MOSI = 0xFFFFFFFFu;   // attached per-transfer (write path only)
    NRF_SPIM3->PSEL.MISO = 0xFFFFFFFFu;
    NRF_SPIM3->PSEL.CSN = 0xFFFFFFFFu;
    NRF_SPIM3->FREQUENCY = SPIM_FREQ_M16;
    NRF_SPIM3->CONFIG = SPIM_CONFIG_MODE0;   // the card comes up in compat timing
    NRF_SPIM3->ORC = 0xFF;                // idle-high filler
}

static inline void sp1_emmc_spim_deinit(void) {
    NRF_SPIM3->ENABLE = 0;
    NRF_SPIM3->PSEL.SCK = 0xFFFFFFFFu;
    NRF_SPIM3->PSEL.MOSI = 0xFFFFFFFFu;
    NRF_SPIM3->PSEL.MISO = 0xFFFFFFFFu;
}

// One blocking DMA transfer with the wires temporarily owned by SPIM. While
// ENABLED the peripheral drives SCK (+MOSI for TX) / samples MISO; on disable
// the pins fall back to their GPIO latches (CLK low, DAT0 as configured), so
// the surrounding bit-bang phases continue seamlessly. That handoff is the
// whole trick and it is proven on this hardware.
//
// A read is rx-only (MOSI unselected), so SPIM3 anomaly 198 (TX corruption)
// cannot bite there at all. Writes make TX real, which is why their frame is
// built in sp1_emmc_tx_frame() above.
static inline void sp1_emmc_spim_xfer(const uint8_t *tx, uint32_t txlen, uint8_t *rx, uint32_t rxlen) {
    NRF_SPIM3->PSEL.MOSI = tx ? SP1_EMMC_PIN_DAT0 : 0xFFFFFFFFu;
    NRF_SPIM3->PSEL.MISO = rx ? SP1_EMMC_PIN_DAT0 : 0xFFFFFFFFu;
    NRF_SPIM3->ENABLE = 7;
    NRF_SPIM3->TXD.PTR = (uint32_t)tx;
    NRF_SPIM3->TXD.MAXCNT = tx ? txlen : 0;
    NRF_SPIM3->RXD.PTR = (uint32_t)rx;
    NRF_SPIM3->RXD.MAXCNT = rx ? rxlen : 0;
    NRF_SPIM3->EVENTS_END = 0;
    NRF_SPIM3->TASKS_START = 1;
    while (!NRF_SPIM3->EVENTS_END) {
        // ~260 us for a full block at 16 MHz -- far too short to be worth a
        // background-task round trip, and any yield here would risk the
        // caller's framing.
    }
    NRF_SPIM3->ENABLE = 0;
}
