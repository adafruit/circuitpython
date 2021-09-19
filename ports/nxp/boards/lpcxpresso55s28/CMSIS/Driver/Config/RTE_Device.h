/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RTE_DEVICE_H
#define _RTE_DEVICE_H

extern void USART0_InitPins();
extern void USART0_DeinitPins();
extern void BOARD_InitSW2(void); /* Function assigned for the Cortex-M33 (Core #0) */
extern void BOARD_InitLEDBlue(void); /* Function assigned for the Cortex-M33 (Core #0) */

/* Driver name mapping. */
/* User needs to provide the implementation of USARTX_GetFreq/USARTX_InitPins/USARTX_DeinitPins for the enabled USART
 * instance. */

// -------- <<< Use Configuration Wizard in Context Menu >>> --------------------
// <e> USART0 (Universal synchronous/asynchronous receiver transmitter)
#define RTE_USART0        1

// <h> DMA
//   <e> Tx
//     <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//   </e>
#define RTE_USART0_DMA_EN 0
#define RTE_USART0_DMA_TX_CH       5
#define RTE_USART0_DMA_RX_CH       4
// </h>
// </e> USART0 (Universal synchronous/asynchronous receiver transmitter)

#define RTE_USART0_DMA_TX_DMA_BASE DMA0
#define RTE_USART0_DMA_RX_DMA_BASE DMA0
// -------- <<< end of configuration section >>> --------------------

/* USART configuration. */
#define USART_RX_BUFFER_LEN     64
#define USART0_RX_BUFFER_ENABLE 1
#define USART1_RX_BUFFER_ENABLE 0
#define USART2_RX_BUFFER_ENABLE 0
#define USART3_RX_BUFFER_ENABLE 0
#define USART4_RX_BUFFER_ENABLE 0
#define USART5_RX_BUFFER_ENABLE 0
#define USART6_RX_BUFFER_ENABLE 0
#define USART7_RX_BUFFER_ENABLE 0

#define RTE_USART0_PIN_INIT        USART0_InitPins
#define RTE_USART0_PIN_DEINIT      USART0_DeinitPins

#if (0)
#include "board.h"

#else
#ifndef BOARD_LED_RED_GPIO
#define BOARD_LED_RED_GPIO GPIO
#endif
#define BOARD_LED_RED_GPIO_PORT 1U
#ifndef BOARD_LED_RED_GPIO_PIN
#define BOARD_LED_RED_GPIO_PIN 6U
#endif

#ifndef BOARD_LED_BLUE_GPIO
#define BOARD_LED_BLUE_GPIO GPIO
#endif
#define BOARD_LED_BLUE_GPIO_PORT 1U
#ifndef BOARD_LED_BLUE_GPIO_PIN
#define BOARD_LED_BLUE_GPIO_PIN 4U
#endif

#ifndef BOARD_LED_GREEN_GPIO
#define BOARD_LED_GREEN_GPIO GPIO
#endif
#define BOARD_LED_GREEN_GPIO_PORT 1U
#ifndef BOARD_LED_GREEN_GPIO_PIN
#define BOARD_LED_GREEN_GPIO_PIN 7U
#endif

#ifndef BOARD_SW1_GPIO
#define BOARD_SW1_GPIO GPIO
#endif
#define BOARD_SW1_GPIO_PORT 0U
#ifndef BOARD_SW1_GPIO_PIN
#define BOARD_SW1_GPIO_PIN 5U
#endif
#define BOARD_SW1_NAME        "SW1"
#define BOARD_SW1_IRQ         PIN_INT0_IRQn
#define BOARD_SW1_IRQ_HANDLER PIN_INT0_IRQHandler

#ifndef BOARD_SW2_GPIO
#define BOARD_SW2_GPIO GPIO
#endif
#define BOARD_SW2_GPIO_PORT 1U
#ifndef BOARD_SW2_GPIO_PIN
#define BOARD_SW2_GPIO_PIN 18U
#endif
#define BOARD_SW2_NAME              "SW2"
#define BOARD_SW2_IRQ               PIN_INT1_IRQn
#define BOARD_SW2_IRQ_HANDLER       PIN_INT1_IRQHandler
#define BOARD_SW2_GPIO_PININT_INDEX 1

#endif


/*----- PORT 1 Configuration */
#define RTE_GPIO_PORT1             1
#define RTE_GPIO_PORT1_SIZE_OF_MAP 2
#define RTE_GPIO_PORT1_MAPS                                                \
    {                                                                      \
        {BOARD_LED_BLUE_GPIO_PIN, kPINT_PinInt0, BOARD_InitLEDBlue, NULL}, \
        {BOARD_SW2_GPIO_PIN, kPINT_PinInt1, BOARD_InitSW2, NULL},      \
    }
#define RTE_GPIO_PORT1_MAX_INTERRUPT_CONTEXTS 1


#endif /* _RTE_DEVICE_H */
