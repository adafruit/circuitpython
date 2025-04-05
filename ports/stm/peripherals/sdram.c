// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 snkYmkrct
//
// based on implementation from https://github.com/micropython/micropython/blob/master/ports/stm32/sdram.c
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include STM32_HAL_H
#include "common-hal/microcontroller/Pin.h"
#include "peripherals/periph.h"
#if defined(STM32H7)
#include "stm32h7xx_ll_fmc.h"
#include "stm32h7xx_hal_sdram.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_cortex.h"
#endif
#ifdef STM32H750xx
#include "stm32h7/stm32h750xx/periph.h"
#endif


#include "sdram.h"


#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#define DEBUG_OP_printf(...) DEBUG_printf(__VA_ARGS__)
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#define DEBUG_OP_printf(...) (void)0
#endif

#define SDRAM_TIMEOUT          ((uint32_t)0xFFFF)
#define CIRCUITPY_HW_SDRAM_STARTUP_TEST       (0)

static uint8_t FMC_Initialized = 0;
static SDRAM_HandleTypeDef hsdram = {0};
static uint32_t sdram_start_address = 0;


static void sdram_init_seq(const struct stm32_sdram_config *config);

void sdram_init(const struct stm32_sdram_config *config) {
    FMC_SDRAM_TimingTypeDef SDRAM_Timing = {0};

    if (!FMC_Initialized) {

        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FMC;
        PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
            DEBUG_printf("sdram: %s", "periph init clock error");
        }
        /* Peripheral clock enable */
        __HAL_RCC_FMC_CLK_ENABLE();
        FMC_Initialized = 1;
        for (uint i = 0; i < MP_ARRAY_SIZE(sdram_pin_list); i++) {
            GPIO_InitTypeDef GPIO_InitStruct = {0};
            GPIO_InitStruct.Pin = pin_mask(sdram_pin_list[i].pin->number);
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
            HAL_GPIO_Init(pin_port(sdram_pin_list[i].pin->port), &GPIO_InitStruct);
            never_reset_pin_number(sdram_pin_list[i].pin->port, sdram_pin_list[i].pin->number);
        }
    }

    /* SDRAM device configuration */
    hsdram.Instance = config->sdram;

    for (size_t i = 0U; i < config->banks_len; i++) {
        hsdram.State = HAL_SDRAM_STATE_RESET;

        memcpy(&hsdram.Init, &config->banks[i].init, sizeof(hsdram.Init));

        memcpy(&SDRAM_Timing, &config->banks[i].timing, sizeof(SDRAM_Timing));

        /* Initialize the SDRAM controller */
        if (HAL_SDRAM_Init(&hsdram, &SDRAM_Timing) != HAL_OK) {
            DEBUG_printf("sdram bank[%d]: %s", i, "init error");
        }
    }

    sdram_init_seq(config);

}
void sdram_deinit(void) {
    FMC_SDRAM_CommandTypeDef command = {0};
    if (FMC_Initialized) {
        /* Send the module into powerdown mode */
        command.CommandMode = FMC_SDRAM_CMD_POWERDOWN_MODE;
        command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
        command.AutoRefreshNumber = 1;
        command.ModeRegisterDefinition = 0;

        /* Send the command */
        if (HAL_SDRAM_SendCommand(&hsdram, &command, HAL_MAX_DELAY) != HAL_OK) {
            DEBUG_printf("sdram power off error:%s:%d", __func__, __LINE__);
        }
    }
}

void *sdram_start(void) {
    return (void *)sdram_start_address;
}

void *sdram_end(void) {
    return (void *)(sdram_start_address + CIRCUITPY_HW_SDRAM_SIZE);
}

uint32_t sdram_size(void) {
    return CIRCUITPY_HW_SDRAM_SIZE;
}

static void sdram_init_seq(const struct stm32_sdram_config *config) {
    FMC_SDRAM_CommandTypeDef command = {0};
    /* Program the SDRAM external device */

    command.AutoRefreshNumber = config->num_auto_refresh;
    command.ModeRegisterDefinition = config->mode_register;
    if (config->banks_len == 2U) {
        command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1_2;
        sdram_start_address = 0xC0000000;
    } else if (config->banks[0].init.SDBank == FMC_SDRAM_BANK1) {
        command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
        sdram_start_address = 0xC0000000;
    } else {
        command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
        sdram_start_address = 0xD0000000;

    }

    /* Configure a clock configuration enable command */
    command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    HAL_SDRAM_SendCommand(&hsdram, &command, HAL_MAX_DELAY);

    HAL_Delay(100);

    /* Configure a PALL (precharge all) command */
    command.CommandMode = FMC_SDRAM_CMD_PALL;
    HAL_SDRAM_SendCommand(&hsdram, &command, HAL_MAX_DELAY);

    /* Configure a Auto-Refresh command */
    command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    HAL_SDRAM_SendCommand(&hsdram, &command, HAL_MAX_DELAY);

    command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    /* load mode */
    HAL_SDRAM_SendCommand(&hsdram, &command, HAL_MAX_DELAY);

    /* program refresh count */
    HAL_SDRAM_ProgramRefreshRate(&hsdram, config->refresh_rate);

    #if defined(STM32F7) || defined(STM32H7)
    __disable_irq();
    MPU_Region_InitTypeDef MPU_InitStruct = {0};
    /** Enable caching for SDRAM to support non-alligned access.
    */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = CPY_SDRAM_REGION;
    MPU_InitStruct.BaseAddress = sdram_start_address;
    MPU_InitStruct.Size = CPY_SDRAM_REGION_SIZE;
    MPU_InitStruct.SubRegionDisable = 0x0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    __ISB();
    __DSB();
    __DMB();
    __enable_irq();

    #endif

}

#if defined(CIRCUITPY_HW_SDRAM_STARTUP_TEST) && (CIRCUITPY_HW_SDRAM_STARTUP_TEST == 1)

bool __attribute__((optimize("Os"))) sdram_test(bool exhaustive) {
    uint8_t const pattern = 0xaa;
    uint8_t const antipattern = 0x55;
    volatile uint8_t *const mem_base = (uint8_t *)sdram_start();

    char error_buffer[1024];

    DEBUG_printf("sdram: %s\n", "sdram test started");

    #if (__DCACHE_PRESENT == 1)
    bool i_cache_disabled = false;
    bool d_cache_disabled = false;

    // Disable caches for testing.
    if (SCB->CCR & (uint32_t)SCB_CCR_IC_Msk) {
        SCB_DisableICache();
        i_cache_disabled = true;
    }

    if (SCB->CCR & (uint32_t)SCB_CCR_DC_Msk) {
        SCB_DisableDCache();
        d_cache_disabled = true;
    }
    #endif

    // Test data bus
    for (uint32_t i = 0; i < hsdram.Init.MemoryDataWidth; i++) {
        *((volatile uint32_t *)mem_base) = (1u << i);
        __DSB();
        if (*((volatile uint32_t *)mem_base) != (1u << i)) {
            snprintf(error_buffer, sizeof(error_buffer),
                "Data bus test failed at 0x%p expected 0x%x found 0x%lx",
                &mem_base[0], (1 << i), ((volatile uint32_t *)mem_base)[0]);
            DEBUG_printf("error: %s\n", error_buffer);
            return false;
        }
    }

    // Test address bus
    for (uint32_t i = 1; i < CIRCUITPY_HW_SDRAM_SIZE; i <<= 1) {
        mem_base[i] = pattern;
        __DSB();
        if (mem_base[i] != pattern) {
            snprintf(error_buffer, sizeof(error_buffer),
                "Address bus test failed at 0x%p expected 0x%x found 0x%x",
                &mem_base[i], pattern, mem_base[i]);
            DEBUG_printf("error: %s\n", error_buffer);
            return false;
        }
    }

    // Check for aliasing (overlapping addresses)
    mem_base[0] = antipattern;
    __DSB();
    for (uint32_t i = 1; i < CIRCUITPY_HW_SDRAM_SIZE; i <<= 1) {
        if (mem_base[i] != pattern) {
            snprintf(error_buffer, sizeof(error_buffer),
                "Address bus overlap at 0x%p expected 0x%x found 0x%x",
                &mem_base[i], pattern, mem_base[i]);
            DEBUG_printf("error: %s\n", error_buffer);
            return false;
        }
    }

    // Test all RAM cells
    if (exhaustive) {
        // Write all memory first then compare, so even if the cache
        // is enabled, it's not just writing and reading from cache.
        // Note: This test should also detect refresh rate issues.
        for (uint32_t i = 0; i < CIRCUITPY_HW_SDRAM_SIZE; i++) {
            mem_base[i] = ((i % 2) ? pattern : antipattern);
        }

        for (uint32_t i = 0; i < CIRCUITPY_HW_SDRAM_SIZE; i++) {
            if (mem_base[i] != ((i % 2) ? pattern : antipattern)) {
                snprintf(error_buffer, sizeof(error_buffer),
                    "Address bus slow test failed at 0x%p expected 0x%x found 0x%x",
                    &mem_base[i], ((i % 2) ? pattern : antipattern), mem_base[i]);
                DEBUG_printf("error: %s\n", error_buffer);
                return false;
            }
        }
    }

    #if (__DCACHE_PRESENT == 1)
    // Re-enable caches if they were enabled before the test started.
    if (i_cache_disabled) {
        SCB_EnableICache();
    }

    if (d_cache_disabled) {
        SCB_EnableDCache();
    }
    #endif

    DEBUG_printf("sdram: %s\n", "sdram test successfully!");

    return true;
}

#endif  // sdram_test
