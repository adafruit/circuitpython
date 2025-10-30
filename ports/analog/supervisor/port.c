/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. All Rights Reserved.
 * (now owned by Analog Devices, Inc.),
 * Copyright (C) 2023 Analog Devices, Inc. All Rights Reserved. This software
 * is proprietary to Analog Devices, Inc. and its licensors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file port.c
 * @author Brandon Hurst @ Analog Devices, Inc.
 * @brief Functions required for a basic CircuitPython port
 * @date 2024-07-30
 *
 * @copyright Copyright (c) 2024
 */

#include <stdint.h>
#include <stdio.h>
#include "supervisor/background_callback.h"
#include "supervisor/board.h"
#include "supervisor/port.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"

// Sys includes
#include "max32_port.h"

// Timers
#include "mxc_delay.h"
#include "rtc.h"

// msec to RTC subsec ticks (4 kHz)
/* Converts a time in milleseconds to equivalent RSSA register value */
#define MSEC_TO_SS_ALARM(x) (0 - ((x * 4096) / 1000))

// Externs defined by linker .ld file
extern uint32_t _stack, _heap, _estack, _eheap;
extern uint32_t _ebss;

// From boards/$(BOARD)/board.c
extern const mxc_gpio_cfg_t pb_pin[];
extern const int num_pbs;
extern const mxc_gpio_cfg_t led_pin[];
extern const int num_leds;

// For saving rtc data for ticks
static uint32_t subsec, sec = 0;
static uint32_t tick_flag = 0;

// defined by cmsis core files
extern void NVIC_SystemReset(void) NORETURN;

volatile uint32_t system_ticks = 0;

void SysTick_Handler(void) {
    system_ticks++;
}


safe_mode_t port_init(void) {
    int err = E_NO_ERROR;

    // 1ms tick timer
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_EnableIRQ(SysTick_IRQn);

    // Enable GPIO (enables clocks + common init for ports)
    for (int i = 0; i < MXC_CFG_GPIO_INSTANCES; i++) {
        err = MXC_GPIO_Init(0x1 << i);
        if (err) {
            return SAFE_MODE_PROGRAMMATIC;
        }
    }

    // Enable clock to RTC peripheral
    MXC_GCR->clkctrl |= MXC_F_GCR_CLKCTRL_ERTCO_EN;
    while (!(MXC_GCR->clkctrl & MXC_F_GCR_CLKCTRL_ERTCO_RDY)) {
        ;
    }

    NVIC_EnableIRQ(RTC_IRQn);
    NVIC_EnableIRQ(USB_IRQn);

    // Init RTC w/ 0sec, 0subsec
    // Driven by 32.768 kHz ERTCO, with ssec= 1/4096 s
    while (MXC_RTC_Init(0, 0) != E_SUCCESS) {
    }
    ;

    // enable 1 sec RTC SSEC alarm
    MXC_RTC_DisableInt(MXC_F_RTC_CTRL_SSEC_ALARM_IE);
    MXC_RTC_SetSubsecondAlarm(MSEC_TO_SS_ALARM(1000));
    MXC_RTC_EnableInt(MXC_F_RTC_CTRL_SSEC_ALARM_IE);

    // Enable RTC
    while (MXC_RTC_Start() != E_SUCCESS) {
    }
    ;

    return SAFE_MODE_NONE;
}

void RTC_IRQHandler(void) {
    // Read flags to clear
    int flags = MXC_RTC_GetFlags();

    switch (flags) {
        case MXC_F_RTC_CTRL_SSEC_ALARM:
            MXC_RTC_ClearFlags(MXC_F_RTC_CTRL_SSEC_ALARM);
            break;
        case MXC_F_RTC_CTRL_TOD_ALARM:
            MXC_RTC_ClearFlags(MXC_F_RTC_CTRL_TOD_ALARM);
            break;
        case MXC_F_RTC_CTRL_RDY:
            MXC_RTC_ClearFlags(MXC_F_RTC_CTRL_RDY);
            break;
        default:
            break;
    }

    tick_flag = 1;
}

// Reset the MCU completely
void reset_cpu(void) {
    // includes MCU reset request + awaits on memory bus
    NVIC_SystemReset();
}

// Reset MCU state
void reset_port(void) {
    reset_all_pins();
}

// Reset to the bootloader
// note: not implemented since max32 requires external signals to
//       activate bootloaders
void reset_to_bootloader(void) {
    NVIC_SystemReset();
    while (true) {
        __NOP();
    }
}

/** Acquire values of stack & heap starts & limits
 *  Return variables defined by linkerscript.
 */
uint32_t *port_stack_get_limit(void) {
    // ignore array bounds GCC warnings
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"

    // NOTE: Only return how much stack we have allotted for CircuitPython
    return port_stack_get_top() - (CIRCUITPY_DEFAULT_STACK_SIZE + CIRCUITPY_EXCEPTION_STACK_SIZE) / sizeof(uint32_t);
    #pragma GCC diagnostic pop
}
uint32_t *port_stack_get_top(void) {
    return &_stack;
}
uint32_t *port_heap_get_bottom(void) {
    return &_heap;
}
uint32_t *port_heap_get_top(void) {
    return port_stack_get_limit();
}

/** Save & retrieve a word from memory over a reset cycle.
 *  Used for safe mode
 */
void port_set_saved_word(uint32_t value) {
    _ebss = value;
}
uint32_t port_get_saved_word(void) {
    return _ebss;
}

// Raw monotonic tick count since startup.
// NOTE (rollover):
// seconds reg is 32 bits, can hold up to 2^32-1
// theref. rolls over after ~136 years
uint64_t port_get_raw_ticks(uint8_t *subticks) {
    // Ensure we can read from ssec register as soon as we can
    // MXC function does cross-tick / busy checking of RTC controller
    if (MXC_RTC->ctrl & MXC_F_RTC_CTRL_EN) {
        // NOTE: RTC_GetTime always returns BUSY if RTC is not running
        while ((MXC_RTC_GetTime(&sec, &subsec)) != E_NO_ERROR) {
            ;
        }
    } else {
        sec = MXC_RTC->sec;
        subsec = MXC_RTC->ssec;
    }

    // Return ticks given total subseconds
    // ticks = TICKS/s * s + subsec/ subs/tick
    uint64_t raw_ticks = ((uint64_t)TICKS_PER_SEC) * sec + (subsec / SUBSEC_PER_TICK);

    if (subticks) {
        // subticks may only be filled to a resn of 1/4096 in some cases
        // e.g. multiply by 32 / 8 = 4 to get true 1/32768 subticks
        *subticks = (32 / (SUBSEC_PER_TICK)) * (subsec - (subsec / SUBSEC_PER_TICK));
    }

    return raw_ticks;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    while (MXC_RTC_Start() == E_BUSY) {
        ;
    }
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    while (MXC_RTC_Stop() == E_BUSY) {
        ;
    }
}

// Wake the CPU after a given # of ticks or sooner
void port_interrupt_after_ticks(uint32_t ticks) {
    uint32_t ticks_msec = 0;

    ticks_msec = (ticks / TICKS_PER_SEC) * 1000;

    // Disable RTC interrupts
    MXC_RTC_DisableInt(MXC_F_RTC_CTRL_SSEC_ALARM_IE |
        MXC_F_RTC_CTRL_TOD_ALARM_IE | MXC_F_RTC_CTRL_RDY_IE);

    // Stop RTC & store current time & ticks
    port_get_raw_ticks(NULL);

    // Clear the flag to be set by the RTC Handler
    tick_flag = 0;

    // Subsec alarm is the starting/reload value of the SSEC counter.
    // ISR triggered when SSEC rolls over from 0xFFFF_FFFF to 0x0
    while (MXC_RTC_SetSubsecondAlarm(MSEC_TO_SS_ALARM(ticks_msec)) != E_SUCCESS) {
    }

    MXC_RTC_EnableInt(MXC_F_RTC_CTRL_SSEC_ALARM_IE);

}

void port_idle_until_interrupt(void) {
    #if CIRCUITPY_RTC
    // Check if alarm triggers before we even got here
    if (MXC_RTC_GetFlags() == (MXC_F_RTC_CTRL_TOD_ALARM | MXC_F_RTC_CTRL_SSEC_ALARM)) {
        return;
    }
    #endif

    // Interrupts should be disabled to ensure the ISR queue is flushed
    // WFI still returns as long as the interrupt flag toggles;
    // only when we re-enable interrupts will the ISR function trigger
    common_hal_mcu_disable_interrupts();
    if (!background_callback_pending()) {
        __DSB();
        /** DEBUG: may comment out WFI for debugging port functions */
        // __WFI();
    }
    common_hal_mcu_enable_interrupts();
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

// Required by libc _init_array loops in startup code
// if we are compiling using "-nostdlib/-nostartfiles"
void _init(void) {
}
