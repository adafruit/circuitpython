// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Alif Aiman for Cytron Technologies
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME "Cytron Maker Sumo RP2350"
#define MICROPY_HW_MCU_NAME "rp2350a"

// User LED on GP3 doubles as the CircuitPython status LED
#define MICROPY_HW_LED_STATUS (&pin_GPIO3)
