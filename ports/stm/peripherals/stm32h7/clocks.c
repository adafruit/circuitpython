// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "stm32h7xx_hal.h"
#include "supervisor/shared/safe_mode.h"
#include <stdbool.h>

// H7 Series
#ifdef STM32H743xx
#include "stm32h7/stm32h743xx/clocks.h"
#endif

#ifdef STM32H750xx
#include "stm32h7/stm32h750xx/clocks.h"
#endif

void stm32_peripherals_clocks_init(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    // Set voltage scaling in accordance with system clock speed
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    __HAL_PWR_VOLTAGESCALING_CONFIG(CPY_CLK_VSCALE);
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }

    // Configure LSE Drive
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    // Set up primary PLL and HSE clocks
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    #if (BOARD_HAS_LOW_SPEED_CRYSTAL)
    RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    #else
    RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    #endif
    #if (CPY_CLK_USB_USES_AUDIOPLL) // Not actually audio PLL in this case, swap macro?
    RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    #endif
    RCC_OscInitStruct.HSEState = BOARD_HSE_SOURCE;
    RCC_OscInitStruct.PLL.PLLState = BOARD_PLL_STATE;
    RCC_OscInitStruct.PLL.PLLSource = BOARD_PLL_SOURCE;
    RCC_OscInitStruct.PLL.PLLM = CPY_CLK_PLLM;
    RCC_OscInitStruct.PLL.PLLN = CPY_CLK_PLLN;
    RCC_OscInitStruct.PLL.PLLP = CPY_CLK_PLLP;
    RCC_OscInitStruct.PLL.PLLQ = CPY_CLK_PLLQ;
    RCC_OscInitStruct.PLL.PLLR = CPY_CLK_PLLR;
    RCC_OscInitStruct.PLL.PLLRGE = CPY_CLK_PLLRGE;
    RCC_OscInitStruct.PLL.PLLVCOSEL = CPY_CLK_PLLVCOSEL;
    RCC_OscInitStruct.PLL.PLLFRACN = CPY_CLK_PLLFRACN;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        // Clock issues are too problematic to even attempt recovery.
        // If you end up here, check whether your LSE settings match your board.
        while (1) {
            ;
        }
    }

    // Configure bus clock sources and divisors
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
        | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = CPY_CLK_AHBDIV;
    RCC_ClkInitStruct.APB1CLKDivider = CPY_CLK_APB1DIV;
    RCC_ClkInitStruct.APB2CLKDivider = CPY_CLK_APB2DIV;
    RCC_ClkInitStruct.APB3CLKDivider = CPY_CLK_APB3DIV;
    RCC_ClkInitStruct.APB4CLKDivider = CPY_CLK_APB4DIV;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, CPY_CLK_FLASH_LATENCY);

    // Set up non-bus peripherals
    // TODO: I2S settings go here
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_USART3
        | RCC_PERIPHCLK_USB;
    #if (BOARD_HAS_LOW_SPEED_CRYSTAL)
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    #else
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    #endif
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    #if (CPY_CLK_USB_USES_AUDIOPLL) // Not actually audio PLL in this case, swap macro?
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    #else
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
    #endif
    #ifdef STM32H750xx
    // USB
    PeriphClkInitStruct.PLL3.PLL3M = 1;
    PeriphClkInitStruct.PLL3.PLL3N = 12;
    PeriphClkInitStruct.PLL3.PLL3P = 2;
    PeriphClkInitStruct.PLL3.PLL3Q = 4;
    PeriphClkInitStruct.PLL3.PLL3R = 2;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_3;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
    // RNG
    PeriphClkInitStruct.PeriphClockSelection = PeriphClkInitStruct.PeriphClockSelection | RCC_PERIPHCLK_RNG;
    PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_HSI48;
    // RTC
    PeriphClkInitStruct.PeriphClockSelection = PeriphClkInitStruct.PeriphClockSelection | RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    #endif
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        while (1) {
            ;
        }
    }
    // Enable USB Voltage detector
    HAL_PWREx_EnableUSBVoltageDetector();
}
