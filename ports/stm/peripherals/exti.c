// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include "py/mpconfig.h"
#include "py/misc.h"

#include "peripherals/exti.h"

#if !(CPY_STM32H7)

static bool stm_exti_reserved[STM32_GPIO_PORT_SIZE];
static void (*stm_exti_callback[STM32_GPIO_PORT_SIZE])(uint8_t num);
// Bitmask of lines whose interrupt is enabled.
static uint16_t stm_exti_enabled;

// Lines 5-9 and 10-15 each share one NVIC IRQ.
static uint16_t irq_lines_mask(uint8_t num) {
    if (num >= 5 && num <= 9) {
        return 0x03e0;
    } else if (num >= 10 && num <= 15) {
        return 0xfc00;
    }
    return 1 << num;
}

void stm_peripherals_exti_reset_exti(uint8_t num) {
    stm_peripherals_exti_disable(num);
    stm_exti_reserved[num] = false;
    stm_exti_callback[num] = NULL;
}

bool stm_peripherals_exti_is_free(uint8_t num) {
    return !stm_exti_reserved[num];
}

bool stm_peripherals_exti_reserve(uint8_t num) {
    if (!stm_peripherals_exti_is_free(num)) {
        return false;
    }
    stm_exti_reserved[num] = true;
    return true;
}

void stm_peripherals_exti_enable(uint8_t num) {
    stm_exti_enabled |= 1 << num;
    HAL_NVIC_EnableIRQ(stm_peripherals_exti_get_irq(num));
}

void stm_peripherals_exti_disable(uint8_t num) {
    stm_exti_enabled &= ~(1 << num);
    if ((stm_exti_enabled & irq_lines_mask(num)) == 0) {
        HAL_NVIC_DisableIRQ(stm_peripherals_exti_get_irq(num));
    }
}

void stm_peripherals_exti_set_callback(void (*callback)(uint8_t num), uint8_t number) {
    stm_exti_callback[number] = callback;
}

IRQn_Type stm_peripherals_exti_get_irq(uint8_t num) {
    if (num == 0) {
        return EXTI0_IRQn;
    } else if (num == 1) {
        return EXTI1_IRQn;
    } else if (num == 2) {
        return EXTI2_IRQn;
    } else if (num == 3) {
        return EXTI3_IRQn;
    } else if (num == 4) {
        return EXTI4_IRQn;
    } else if (num >= 5 && num <= 9) {
        return EXTI9_5_IRQn;
    } else if (num >= 10 && num <= 15) {
        return EXTI15_10_IRQn;
    } else {
        return 0;
    }
}

#ifdef STM32L4
#define PR PR1
#endif
static void exti_irq_handler(uint8_t first, uint8_t last) {
    uint32_t pending = EXTI->PR;
    for (uint8_t i = first; i <= last; i++) {
        if (!(pending & (1 << i))) {
            continue;
        }
        if ((stm_exti_enabled & (1 << i)) && stm_exti_callback[i] != NULL) {
            stm_exti_callback[i](i);
        } else {
            // No one is handling this line: clear it so the IRQ does not fire again.
            EXTI->PR = 1 << i;
        }
    }
}
#ifdef STM32L4
#undef PR
#endif

void EXTI0_IRQHandler(void) {
    exti_irq_handler(0, 0);
}
void EXTI1_IRQHandler(void) {
    exti_irq_handler(1, 1);
}
void EXTI2_IRQHandler(void) {
    exti_irq_handler(2, 2);
}
void EXTI3_IRQHandler(void) {
    exti_irq_handler(3, 3);
}
void EXTI4_IRQHandler(void) {
    exti_irq_handler(4, 4);
}
void EXTI9_5_IRQHandler(void) {
    exti_irq_handler(5, 9);
}
void EXTI15_10_IRQHandler(void) {
    exti_irq_handler(10, 15);
}


#endif
