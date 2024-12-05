// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#ifndef MAX32_PORT_H
#define MAX32_PORT_H

#include <stdint.h>

#include "mxc_assert.h"
#include "mxc_delay.h"
#include "mxc_device.h"
#include "mxc_pins.h"
#include "mxc_sys.h"
#include "mcr_regs.h"

#include "gpio.h"

#ifdef MAX32690
#include "system_max32690.h"
#include "max32690.h"

/** START: GPIO4 Handling specific to MAX32690 */
#define GPIO4_PIN_MASK 0x00000003
#define GPIO4_RESET_MASK 0xFFFFFF77
#define GPIO4_OUTEN_MASK(mask)                                \
    (((mask & (1 << 0)) << MXC_F_MCR_GPIO4_CTRL_P40_OE_POS) | \
    ((mask & (1 << 1)) << (MXC_F_MCR_GPIO4_CTRL_P41_OE_POS - 1)))
#define GPIO4_PULLDIS_MASK(mask)                              \
    (((mask & (1 << 0)) << MXC_F_MCR_GPIO4_CTRL_P40_PE_POS) | \
    ((mask & (1 << 1)) << (MXC_F_MCR_GPIO4_CTRL_P41_PE_POS - 1)))
#define GPIO4_DATAOUT_MASK(mask)                              \
    (((mask & (1 << 0)) << MXC_F_MCR_GPIO4_CTRL_P40_DO_POS) | \
    ((mask & (1 << 1)) << (MXC_F_MCR_GPIO4_CTRL_P41_DO_POS - 1)))
#define GPIO4_DATAOUT_GET_MASK(mask)                                                             \
    ((((MXC_MCR->gpio4_ctrl & MXC_F_MCR_GPIO4_CTRL_P40_DO) >> MXC_F_MCR_GPIO4_CTRL_P40_DO_POS) | \
    ((MXC_MCR->gpio4_ctrl & MXC_F_MCR_GPIO4_CTRL_P41_DO) >>                                    \
    (MXC_F_MCR_GPIO4_CTRL_P41_DO_POS - 1))) &                                                 \
    mask)
#define GPIO4_DATAIN_MASK(mask)                                                                  \
    ((((MXC_MCR->gpio4_ctrl & MXC_F_MCR_GPIO4_CTRL_P40_IN) >> MXC_F_MCR_GPIO4_CTRL_P40_IN_POS) | \
    ((MXC_MCR->gpio4_ctrl & MXC_F_MCR_GPIO4_CTRL_P41_IN) >>                                    \
    (MXC_F_MCR_GPIO4_CTRL_P41_IN_POS - 1))) &                                                 \
    mask)
#define GPIO4_AFEN_MASK(mask)                                  \
    (((mask & (1 << 0)) << MXC_F_MCR_OUTEN_PDOWN_OUT_EN_POS) | \
    ((mask & (1 << 1)) >> (MXC_F_MCR_OUTEN_SQWOUT_EN_POS + 1)))
/** END: GPIO4 Handling specific to MAX32690 */

#endif

/** Linker variables defined....
 *  _estack:    end of the stack
 * _ebss:       end of BSS section
 * _ezero:      same as ebss (acc. to main.c)
 */
extern uint32_t _ezero;
extern uint32_t _estack;
extern uint32_t _ebss; // Stored at the end of the bss section (which includes the heap).
extern uint32_t __stack, __heap;

extern uint32_t SystemCoreClock;

// Tick timer should be 1/1024 s. RTC Oscillator is usually 32.768 kHz ERTCO.
#define TICKS_PER_SEC   1024

#ifdef MAX32690
// 12-bit ssec register, ticks @ 4096 Hz
#define SUBSEC_PER_TICK 4
#endif

#endif // MAX32_PORT_H
