// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "Teenage Engineering SP-1"
#define MICROPY_HW_MCU_NAME         "nRF52840"

// Flash map. The bootloader owns 0x00000-0x20000 and jumps to 0x20000, so
// the vector table goes there.
//
// There is no bootloader in *high* flash either, so both bootloader sizes are
// zero and BOOTLOADER_START_ADDR collapses onto the bootloader settings page
// at 0xFF000. That page belongs to the bootloader and must never be
// written.
//
//   0x00000-0x20000  bootloader (never touched)
//   0x20000-0x21000  ISR/vector table
//   0x21000-0xBD000  firmware (624 KiB)
//   0xBD000-0xBF000  microcontroller.nvm (8 KiB)
//   0xBF000-0xFF000  CIRCUITPY internal-flash FAT (256 KiB)
//   0xFF000-0x100000 bootloader settings page (RESERVED)
#define ISR_START_ADDR              (0x20000)
#define BOOTLOADER_SIZE             (0)
#define BOOTLOADER_MBR_SIZE         (0)

// No BLE on this board, so nothing to store for bonding.
#define CIRCUITPY_BLE_CONFIG_SIZE   (0)

// No 32.768 kHz crystal: P0.00 and P0.01 are playback LEDs. Use the RC
// oscillator for LFCLK.
#define BOARD_HAS_32KHZ_XTAL        (0)

// Power off. There is no reset pin, no power switch and no removable battery,
// so SYSTEM_OFF is the only "off" this device has. Waking from off is one
// of only two routes back to the bootloader. P0.27 (Function) is the only
// GPIO button and the only wake source; it is a plain switch to ground.
#define BOARD_POWER_OFF_BUTTON_PIN  NRF_GPIO_PIN_MAP(0, 27)

// The bootloader's DFU magic. The gate at 0x6b2 is 16 bits wide and
// split across both retention registers,
//
//     GPREGRET | (GPREGRET2 << 8) == 0x7EB3
//
//
// This bootloader has no separate UF2/OTA and serial-DFU requests. There is
// one gate, so both magics are the same pair and, reset_to_bootloader() and
// microcontroller.on_next_reset(RunMode.BOOTLOADER) both land in boot mode.
#define BOOTLOADER_DFU_MAGIC        (0xB3)
#define BOOTLOADER_DFU_MAGIC2       (0x7E)
#define BOOTLOADER_UF2_MAGIC        (0xB3)
#define BOOTLOADER_UF2_MAGIC2       (0x7E)

// No SD card LUN. There is no card slot on this device.
#define CIRCUITPY_SDCARD_USB        (0)

// The 4 GB eMMC
#define DEFAULT_EMMC_CLOCK          (&pin_P0_06)
#define DEFAULT_EMMC_COMMAND        (&pin_P0_08)
#define DEFAULT_EMMC_DATA           (&pin_P0_07)
#define DEFAULT_EMMC_RESET          (&pin_P1_08)
#define DEFAULT_EMMC_VCCQ           (&pin_P0_14)

// TWIM to the CS42L42 (0x48) and TAS2505 (0x18).
#define DEFAULT_I2C_BUS_SCL         (&pin_P1_11)
#define DEFAULT_I2C_BUS_SDA         (&pin_P1_07)

// The CDC REPL is normally the only place status is visible. Borrow the first
// track LED for the supervisor status LED so that safe mode is legible on the
// device itself. It is claimed only while the supervisor is showing status and
// is released before user code runs, so board.LED_TRACK1 stays usable.
#define MICROPY_HW_LED_STATUS       (&pin_P0_29)

// Blink that same LED once, ~200 ms, the moment the power-off hold completes.
// The gesture is otherwise silent.
#define BOARD_POWER_OFF_CONFIRM_LED_PIN NRF_GPIO_PIN_MAP(0, 29)
