/*
 * @brief Common SystemInit function for LPC17xx/40xx chips
 *
 * @note
 * Copyright 2013-2014, 2019, 2020 NXP
 * All rights reserved.
 *
 * @par
 * NXP Confidential. This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms.
 *
 * By expressly accepting such terms or by downloading, installing, activating
 * and/or otherwise using the software, you are agreeing that you have read, and
 * that you agree to comply with and are bound by, such license terms.
 *
 * If you do not agree to be bound by the applicable license terms, then you may not
 * retain, install, activate or otherwise use the software.
 */

 #if defined(NO_BOARD_LIB)
 #include "chip.h"
 #else
 #include <boards/lpc55s28_evk/board_config.h>
 #endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

#if defined(NO_BOARD_LIB)
const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;
#endif

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Set up and initialize hardware prior to call to main */
void SystemInit(void) {
    unsigned int *pSCB_VTOR = (unsigned int *)0xE000ED08;

    #if defined(__IAR_SYSTEMS_ICC__)
    extern void *__vector_table;

    *pSCB_VTOR = (unsigned int)&__vector_table;
    #elif defined(__CODE_RED)
    extern void *g_pfnVectors;

    *pSCB_VTOR = (unsigned int)&g_pfnVectors;
    #elif defined(__ARMCC_VERSION)
    extern void *__Vectors;

    *pSCB_VTOR = (unsigned int)&__Vectors;
    #endif

    #if defined(__FPU_PRESENT) && __FPU_PRESENT == 1
    fpuInit();
    #endif

    #if defined(NO_BOARD_LIB)
    /* Chip specific SystemInit */
    Chip_SystemInit();
    #else
    /* Setup system clocking and muxing */
    Board_SystemInit();
    #endif
}
