// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "stm32h7xx_hal.h"

// Chip:                STM32H743
// Line Type:           Single-Core
// Speed:               480MHz (MAX)

// Defaults:
#ifndef CPY_CLK_VSCALE
#define CPY_CLK_VSCALE (PWR_REGULATOR_VOLTAGE_SCALE0)
#endif
#ifndef CPY_CLK_PLLM
#define CPY_CLK_PLLM (HSE_VALUE / 2000000)
#endif
#ifndef CPY_CLK_PLLN
#define CPY_CLK_PLLN (480)
#endif
#ifndef CPY_CLK_PLLP
#define CPY_CLK_PLLP (2)
#endif
#ifndef CPY_CLK_PLLQ
#define CPY_CLK_PLLQ (20)
#endif
#ifndef CPY_CLK_PLLR
#define CPY_CLK_PLLR (2)
#endif
#ifndef CPY_CLK_PLLRGE
#define CPY_CLK_PLLRGE (RCC_PLL1VCIRANGE_1)
#endif
#ifndef CPY_CLK_PLLVCOSEL
#define CPY_CLK_PLLVCOSEL (RCC_PLL1VCOWIDE)
#endif
#ifndef CPY_CLK_PLLFRACN
#define CPY_CLK_PLLFRACN (0)
#endif
#ifndef CPY_CLK_AHBDIV
#define CPY_CLK_AHBDIV (RCC_HCLK_DIV2)
#endif
#ifndef CPY_CLK_APB1DIV
#define CPY_CLK_APB1DIV (RCC_APB1_DIV2)
#endif
#ifndef CPY_CLK_APB2DIV
#define CPY_CLK_APB2DIV (RCC_APB2_DIV2)
#endif
#ifndef CPY_CLK_APB3DIV
#define CPY_CLK_APB3DIV (RCC_APB3_DIV2)
#endif
#ifndef CPY_CLK_APB4DIV
#define CPY_CLK_APB4DIV (RCC_APB4_DIV2)
#endif
#ifndef CPY_CLK_FLASH_LATENCY
#define CPY_CLK_FLASH_LATENCY (FLASH_LATENCY_4)
#endif
#ifndef CPY_CLK_USB_USES_AUDIOPLL
#define CPY_CLK_USB_USES_AUDIOPLL (0)
#endif
#ifndef BOARD_HSE_SOURCE
#define BOARD_HSE_SOURCE (RCC_HSE_ON)
#endif
#ifndef BOARD_PLL_STATE
#define BOARD_PLL_STATE (RCC_PLL_ON)
#endif
#ifndef BOARD_PLL_SOURCE
#define BOARD_PLL_SOURCE (RCC_PLLSOURCE_HSE)
#endif
