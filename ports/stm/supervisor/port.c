// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "supervisor/background_callback.h"
#include "supervisor/board.h"
#include "supervisor/port.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"

#if CIRCUITPY_BUSIO
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"
#endif
#if CIRCUITPY_PULSEIO || CIRCUITPY_PWMIO
#include "peripherals/timers.h"
#endif
#if CIRCUITPY_SDIOIO
#include "common-hal/sdioio/SDCard.h"
#endif
#if CIRCUITPY_PULSEIO || CIRCUITPY_ALARM
#include "peripherals/exti.h"
#endif
#if CIRCUITPY_ALARM
#include "common-hal/alarm/__init__.h"
#endif
#if CIRCUITPY_RTC
#include "shared-bindings/rtc/__init__.h"
#endif

#include "peripherals/clocks.h"
#include "peripherals/gpio.h"
#include "peripherals/rtc.h"

#include STM32_HAL_H

void NVIC_SystemReset(void) NORETURN;

#if (CPY_STM32H7) || (CPY_STM32F7)
#if defined(CIRCUITPY_HW_SDRAM_SIZE)
#include "stm.h"
#include "sdram.h"
#include <stdbool.h>
#include <stddef.h>
#include "lib/tlsf/tlsf.h"
// internal SRAM + external SDRAM
#define CIRCUITPY_RAM_DEVICE_COUNT (2)
#endif

// Device memories must be accessed in order.
#define DEVICE 2
// Normal memory can have accesses reorder and prefetched.
#define NORMAL 0
// Prevents instruction access.
#define NO_EXECUTION 1
#define EXECUTION 0
// Shareable if the memory system manages coherency.
#define NOT_SHAREABLE 0
#define SHAREABLE 1
#define NOT_CACHEABLE 0
#define CACHEABLE 1
#define NOT_BUFFERABLE 0
#define BUFFERABLE 1
#define NO_SUBREGIONS 0

extern uint32_t _ld_stack_top;

extern uint32_t _ld_d1_ram_bss_start;
extern uint32_t _ld_d1_ram_bss_size;
extern uint32_t _ld_d1_ram_data_destination;
extern uint32_t _ld_d1_ram_data_size;
extern uint32_t _ld_d1_ram_data_flash_copy;
extern uint32_t _ld_dtcm_bss_start;
extern uint32_t _ld_dtcm_bss_size;
extern uint32_t _ld_dtcm_data_destination;
extern uint32_t _ld_dtcm_data_size;
extern uint32_t _ld_dtcm_data_flash_copy;
extern uint32_t _ld_itcm_destination;
extern uint32_t _ld_itcm_size;
extern uint32_t _ld_itcm_flash_copy;

extern void main(void);
extern void SystemInit(void);

// This replaces the Reset_Handler in gcc/startup_*.s, calls SystemInit from system_*.c
__attribute__((used, naked)) void Reset_Handler(void) {
    __disable_irq();
    __set_MSP((uint32_t)&_ld_stack_top);

    /* Disable MPU */
    ARM_MPU_Disable();

    // Copy all of the itcm code to run from ITCM. Do this while the MPU is disabled because we write
    // protect it.
    for (uint32_t i = 0; i < ((size_t)&_ld_itcm_size) / 4; i++) {
        (&_ld_itcm_destination)[i] = (&_ld_itcm_flash_copy)[i];
    }

    // The first number in RBAR is the region number. When searching for a policy, the region with
    // the highest number wins. If none match, then the default policy set at enable applies.

    // Mark all the flash the same until instructed otherwise.
    MPU->RBAR = ARM_MPU_RBAR(11, 0x08000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, CPY_FLASH_REGION_SIZE);

    // This the ITCM. Set it to read-only because we've loaded everything already and it's easy to
    // accidentally write the wrong value to 0x00000000 (aka NULL).
    MPU->RBAR = ARM_MPU_RBAR(12, 0x00000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_RO, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, CPY_ITCM_REGION_SIZE);

    // This the DTCM.
    MPU->RBAR = ARM_MPU_RBAR(14, 0x20000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, CPY_DTCM_REGION_SIZE);

    // This is AXI SRAM (D1).
    MPU->RBAR = ARM_MPU_RBAR(15, CPY_SRAM_START_ADDR);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, CPY_SRAM_SUBMASK, CPY_SRAM_REGION_SIZE);

    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    // Copy all of the data to run from DTCM.
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_data_size) / 4; i++) {
        (&_ld_dtcm_data_destination)[i] = (&_ld_dtcm_data_flash_copy)[i];
    }

    // Clear DTCM bss.
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_bss_size) / 4; i++) {
        (&_ld_dtcm_bss_start)[i] = 0;
    }

    // Copy all of the data to run from D1 RAM.
    for (uint32_t i = 0; i < ((size_t)&_ld_d1_ram_data_size) / 4; i++) {
        (&_ld_d1_ram_data_destination)[i] = (&_ld_d1_ram_data_flash_copy)[i];
    }

    // Clear D1 RAM bss.
    for (uint32_t i = 0; i < ((size_t)&_ld_d1_ram_bss_size) / 4; i++) {
        (&_ld_d1_ram_bss_start)[i] = 0;
    }
    #ifdef STM32H750xx
    __DMB(); /* ARM says to use a DMB instruction before relocating VTOR */
    SCB->VTOR = 0x90000000u; /* We relocate vector table to the QSPI sector 1 */
    __DSB(); /* ARM says to use a DSB instruction just after relocating VTOR */
    /*
       Since the STM32H750 microcontroller has only 128kB internal flash,
       CircuitPython has to run from an external flash (QSPI or FMC).
       This means a custom bootloader, like tinyUF2, is needed on the internal
       flash to initialize the clocks, PLLs and (QSPI) controller, and to
       start execution of the firmware from the external flash.
       It also makes the SystemInit() call not necessary for this chip.
    */
    #if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    /* Enable I cache. */
    SCB_EnableICache();
    #endif /* __ICACHE_PRESENT */

    #if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    /* Enable D cache. */
    SCB_EnableDCache();
    #endif /* __DCACHE_PRESENT */

    #else
    SystemInit();
    #endif
    __enable_irq();
    main();
}
#endif // end H7 specific code

// Low power clock variables
static volatile uint32_t systick_ms;

#if  defined(CIRCUITPY_HW_SDRAM_SIZE)
static tlsf_t _heap = NULL;
static pool_t pools[CIRCUITPY_RAM_DEVICE_COUNT] = {NULL};


void port_heap_init(void) {
    // heap init in _port_heap_init called from port_init
}

void stm_add_sdram_to_heap(void) {
    size_t sdram_memory_size = sdram_size();
    pools[1] = tlsf_add_pool(_heap, sdram_start(), sdram_memory_size);
}

static void _port_heap_init(void) {
    uint32_t *heap_bottom = port_heap_get_bottom();
    uint32_t *heap_top = port_heap_get_top();
    size_t size = (heap_top - heap_bottom) * sizeof(uint32_t);
    size_t sdram_memory_size = sdram_size();

    _heap = tlsf_create_with_pool(heap_bottom, size, size + sdram_memory_size);
    pools[0] = tlsf_get_pool(_heap);
}

static bool max_size_walker(void *ptr, size_t size, int used, void *user) {
    size_t *max_size = (size_t *)user;
    if (!used && *max_size < size) {
        *max_size = size;
    }
    return true;
}

size_t port_heap_get_largest_free_size(void) {
    size_t max_size = 0;
    for (size_t i = 0; i < CIRCUITPY_RAM_DEVICE_COUNT; i++) {
        if (pools[i]) {
            tlsf_walk_pool(pools[i], max_size_walker, &max_size);
        }
    }
    return tlsf_fit_size(_heap, max_size);
}

void *port_malloc(size_t size, bool dma_capable) {
    void *block = tlsf_malloc(_heap, size);
    return block;
}

void port_free(void *ptr) {
    tlsf_free(_heap, ptr);
}

void *port_realloc(void *ptr, size_t size, bool dma_capable) {
    return tlsf_realloc(_heap, ptr, size);
}
#endif

safe_mode_t port_init(void) {
    HAL_Init(); // Turns on SysTick
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    #if CPY_STM32F4 || CPY_STM32L4
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    #if CIRCUITPY_ALARM
    // TODO: don't reset RTC entirely and move this back to alarm
    if (STM_ALARM_FLAG & 0x01) {
        // We've woken from deep sleep. Was it the WKUP pin or the RTC?
        if (RTC->ISR & RTC_FLAG_ALRBF) {
            // Alarm B is the deep sleep alarm
            alarm_set_wakeup_reason(STM_WAKEUP_RTC);
        } else {
            alarm_set_wakeup_reason(STM_WAKEUP_GPIO);
        }
    }
    #endif

    __HAL_RCC_BACKUPRESET_FORCE();
    __HAL_RCC_BACKUPRESET_RELEASE();

    #endif

    stm32_peripherals_clocks_init();
    stm32_peripherals_gpio_init();
    stm32_peripherals_rtc_init();

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    stm32_peripherals_rtc_reset_alarms();

    #if  defined(CIRCUITPY_HW_SDRAM_SIZE)
    _port_heap_init();
    #endif
    // Turn off SysTick
    SysTick->CTRL = 0;

    return SAFE_MODE_NONE;
}

void HAL_Delay(uint32_t delay_ms) {
    if (SysTick->CTRL != 0) {
        // SysTick is on, so use it
        uint32_t tickstart = systick_ms;
        while (systick_ms - tickstart < delay_ms) {
        }
    } else {
        mp_hal_delay_ms(delay_ms);
    }
}

uint32_t HAL_GetTick(void) {
    if (SysTick->CTRL != 0) {
        return systick_ms;
    } else {
        uint8_t subticks;
        uint32_t result = (uint32_t)port_get_raw_ticks(&subticks);
        return result;
    }
}

void SysTick_Handler(void) {
    systick_ms += 1;
    // Read the CTRL register to clear the SysTick interrupt.
    SysTick->CTRL;
}

void reset_port(void) {
    reset_all_pins();

    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if CIRCUITPY_BUSIO
    spi_reset();
    uart_reset();
    #endif
    #if CIRCUITPY_SDIOIO
    sdioio_reset();
    #endif
    #if CIRCUITPY_ALARM
    exti_reset();
    #endif
}

void reset_to_bootloader(void) {

/*
From STM AN2606:
Before jumping to bootloader user must:
• Disable all peripheral clocks
• Disable used PLL
• Disable interrupts
• Clear pending interrupts
System memory boot mode can be exited by getting out from bootloader activation
condition and generating hardware reset or using Go command to execute user code
*/
    HAL_RCC_DeInit();
    HAL_DeInit();

    // Disable all pending interrupts using NVIC
    for (uint8_t i = 0; i < MP_ARRAY_SIZE(NVIC->ICER); ++i) {
        NVIC->ICER[i] = 0xFFFFFFFF;
    }

    // if it is necessary to ensure an interrupt will not be triggered after disabling it in the NVIC,
    // add a DSB instruction and then an ISB instruction. (ARM Cortex™-M Programming Guide to
    // Memory Barrier Instructions, 4.6 Disabling Interrupts using NVIC)
    __DSB();
    __ISB();

    // Clear all pending interrupts using NVIC
    for (uint8_t i = 0; i < MP_ARRAY_SIZE(NVIC->ICPR); ++i) {
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    // information about jump addresses has been taken from STM AN2606.
    #if defined(STM32F4)
    __set_MSP(*((uint32_t *)0x1FFF0000));
    ((void (*)(void)) * ((uint32_t *)0x1FFF0004))();
    #else
    // DFU mode for STM32 variant note implemented.
    NVIC_SystemReset();
    #endif

    while (true) {
        asm ("nop;");
    }
}

void reset_cpu(void) {
    NVIC_SystemReset();
}

extern uint32_t _ld_heap_start, _ld_heap_end, _ld_stack_top, _ld_stack_bottom;

uint32_t *port_heap_get_bottom(void) {
    return &_ld_heap_start;
}

// heap memory can be set in SRAM and stack can be set in DTCM
uint32_t *port_heap_get_top(void) {
    return &_ld_heap_end;
}

uint32_t *port_stack_get_limit(void) {
    #pragma GCC diagnostic push

    #pragma GCC diagnostic ignored "-Warray-bounds"
    return port_stack_get_top() - (CIRCUITPY_DEFAULT_STACK_SIZE + CIRCUITPY_EXCEPTION_STACK_SIZE) / sizeof(uint32_t);
    #pragma GCC diagnostic pop
}

uint32_t *port_stack_get_top(void) {
    return &_ld_stack_top;
}

extern uint32_t _ebss;

// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    _ebss = value;
}

uint32_t port_get_saved_word(void) {
    return _ebss;
}

__attribute__((used)) void MemManage_Handler(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

__attribute__((used)) void BusFault_Handler(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

__attribute__((used)) void UsageFault_Handler(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

__attribute__((used)) void HardFault_Handler(void) {
    reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    while (true) {
        asm ("nop;");
    }
}

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    return stm32_peripherals_rtc_monotonic_ticks(subticks);
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    stm32_peripherals_rtc_set_wakeup_mode_tick();
    stm32_peripherals_rtc_assign_wkup_callback(supervisor_tick);
    stm32_peripherals_rtc_enable_wakeup_timer();
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    stm32_peripherals_rtc_disable_wakeup_timer();
}

void port_interrupt_after_ticks(uint32_t ticks) {
    stm32_peripherals_rtc_set_alarm(PERIPHERALS_ALARM_A, ticks);
}

void port_idle_until_interrupt(void) {
    // Clear the FPU interrupt because it can prevent us from sleeping.
    if (__get_FPSCR() & ~(0x9f)) {
        __set_FPSCR(__get_FPSCR() & ~(0x9f));
        (void)__get_FPSCR();
    }
    // The alarm might have triggered before we even reach the WFI
    if (stm32_peripherals_rtc_alarm_triggered(PERIPHERALS_ALARM_A)) {
        return;
    }
    common_hal_mcu_disable_interrupts();
    if (!background_callback_pending()) {
        __WFI();
    }
    common_hal_mcu_enable_interrupts();
}

// Required by __libc_init_array in startup code if we are compiling using
// -nostdlib/-nostartfiles.
void _init(void) {

}

#if CIRCUITPY_ALARM
// in case boards/xxx/board.c does not provide board_deinit()
MP_WEAK void board_deinit(void) {
}
#endif
