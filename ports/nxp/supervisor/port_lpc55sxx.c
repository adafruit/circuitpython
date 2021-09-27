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


#include <stdint.h>
#include "mcux-sdk/devices/LPC55S28/LPC55S28.h"
#include "mcux-sdk/drivers/utick/fsl_utick.h"


static volatile uint32_t counter;

static void cb(void) {
    counter++;

    return;
}


void RIT_Init(void) {
    counter = 0UL;

    uint32_t CLOCK_CTRL = SYSCON->CLOCK_CTRL;
    CLOCK_CTRL |= SYSCON_CLOCK_CTRL_FRO1MHZ_CLK_ENA(CLOCK_CTRL);
    CLOCK_CTRL |= SYSCON_CLOCK_CTRL_FRO1MHZ_UTICK_ENA(CLOCK_CTRL);
    SYSCON->CLOCK_CTRL = CLOCK_CTRL;

    UTICK_Init(UTICK0);
    return;
}


void RIT_SetTimerIntervalHz(uint32_t freq) {

    /* UTICK is fed by fro_1m, 1 MHz internal oscillator */
    uint32_t count = 1000000UL / freq;

    UTICK_SetTick(UTICK0, kUTICK_Repeat, count, cb);
    return;
}


uint32_t RIT_GetIntStatus(void) {
    return 0;
}


void RIT_ClearInt(void) {
    return;
}


void RIT_Disable(void) {
    return;
}


void RIT_Enable(void) {
    return;
}


uint32_t RIT_GetCounter(void) {
    return counter;
}
