/**********************************************************************
* $Id$    system_LPC177x_8x.h      2011-06-02
*//**
* @file    system_LPC177x_8x.h
* @brief  CMSIS Cortex-M3 Device Peripheral Access Layer Source File
*      for the NXP LPC177x_8x Device Series
* @version  1.0
* @date    02. June. 2011
* @author  NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
* modified by KEIL:
* 2016-12-07:
* - moved clock macros from system_LPC177x_8x.h to system_LPC177x_8x.c
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#ifndef __SYSTEM_LPC177x_8x_H
#define __SYSTEM_LPC177x_8x_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint32_t SystemCoreClock;      /*!< System Clock Frequency (Core Clock)    */
extern uint32_t PeripheralClock;      /*!< Peripheral Clock Frequency (Pclk)      */
extern uint32_t EMCClock;             /*!< EMC Clock                              */
extern uint32_t USBClock;             /*!< USB Frequency                          */


/**
  \brief  Setup the microcontroller system.

    Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
  \brief  Update SystemCoreClock variable.

    Updates the SystemCoreClock with current core Clock
    retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);


#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_LPC177x_8x_H */
