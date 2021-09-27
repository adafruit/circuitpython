/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#undef RTC_EV_SUPPORT
#define RTC_EV_SUPPORT 0

#include <stdint.h>

#include "nxp_lpcopen/lpc175x_6x/lpc_chip_175x_6x/inc/chip.h"
#include "nxp_lpcopen/lpc175x_6x/lpc_chip_175x_6x/inc/sysctl_17xx_40xx.h"
#include "nxp_lpcopen/lpc175x_6x/lpc_chip_175x_6x/inc/clock_17xx_40xx.h"
#include "nxp_lpcopen/lpc175x_6x/lpc_chip_175x_6x/inc/ritimer_17xx_40xx.h"
#include "nxp_lpcopen/lpc175x_6x/lpc_chip_175x_6x/inc/rtc_17xx_40xx.h"


void RIT_Init(void) {
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_RIT);
    LPC_RITIMER->COMPVAL = 0xFFFFFFFFUL;
    LPC_RITIMER->MASK = 0x00000000UL;
    LPC_RITIMER->CTRL = 0x0CUL;
    LPC_RITIMER->COUNTER = 0x00000000UL;

    return;
}


void RIT_SetTimerIntervalHz(uint32_t freq) {
    uint64_t cmp_value;

    /* Determine approximate compare value based on clock rate and passed interval */
    cmp_value = (uint64_t)Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_RIT);
    cmp_value = cmp_value / (uint64_t)freq;

    /* Set timer compare value and periodic mode */
    Chip_RIT_SetCOMPVAL(LPC_RITIMER, cmp_value);

    return;
}


uint32_t RIT_GetIntStatus(void) {
    return Chip_RIT_GetIntStatus(LPC_RITIMER);
}


void RIT_ClearInt(void) {
    Chip_RIT_ClearInt(LPC_RITIMER);
    return;
}


void RIT_Disable(void) {
    Chip_RIT_Disable(LPC_RITIMER);
    return;
}


void RIT_Enable(void) {
    Chip_RIT_Enable(LPC_RITIMER);
    return;
}


uint32_t RIT_GetCounter(void) {
    uint32_t Counter = Chip_RIT_GetCounter(LPC_RITIMER);
    #if (1)
    Counter /= (100000000ULL / 32768ULL);
    #else
    uint64_t clk = (uint64_t)Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_RIT);
    Counter /= (clk / 32768ULL);
    #endif

    return Counter;
}
