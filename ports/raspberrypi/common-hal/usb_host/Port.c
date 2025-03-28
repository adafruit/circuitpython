// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/rp2pio/StateMachine.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/usb_host/Port.h"
#include "supervisor/shared/serial.h"
#include "supervisor/usb.h"

#include "pico/time.h"
#include "hardware/structs/mpu.h"
#ifdef PICO_RP2040
#include "RP2040.h" // (cmsis)
#endif
#ifdef PICO_RP2350
#include "RP2350.h" // (cmsis)
#endif
#include "hardware/dma.h"
#include "pico/multicore.h"

#include "py/runtime.h"

#include "tusb.h"

#include "lib/Pico-PIO-USB/src/pio_usb.h"
#include "lib/Pico-PIO-USB/src/pio_usb_configuration.h"


usb_host_port_obj_t usb_host_instance;

volatile bool _core1_ready = false;

static void __not_in_flash_func(core1_main)(void) {
    // The MPU is reset before this starts.
    SysTick->LOAD = (uint32_t)((common_hal_mcu_processor_get_frequency() / 1000) - 1UL);
    SysTick->VAL = 0UL;   /* Load the SysTick Counter Value */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |  // Processor clock.
        SysTick_CTRL_ENABLE_Msk;

    // Turn off flash access. After this, it will hard fault. Better than messing
    // up CIRCUITPY.
    #if __CORTEX_M == 0
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
    MPU->RNR = 6; // 7 is used by pico-sdk stack protection.
    MPU->RBAR = XIP_BASE | MPU_RBAR_VALID_Msk;
    MPU->RASR = MPU_RASR_XN_Msk | // Set execute never and everything else is restricted.
        MPU_RASR_ENABLE_Msk |
        (0x1b << MPU_RASR_SIZE_Pos);         // Size is 0x10000000 which masks up to SRAM region.
    MPU->RNR = 7;
    #endif
    #if __CORTEX_M == 33
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
    MPU->RNR = 6; // 7 is used by pico-sdk stack protection.
    MPU->RBAR = XIP_BASE | MPU_RBAR_XN_Msk;
    MPU->RLAR = XIP_SRAM_BASE | MPU_RLAR_EN_Msk;
    #endif

    _core1_ready = true;

    while (true) {
        pio_usb_host_frame();
        if (tuh_task_event_ready()) {
            // Queue the tinyusb background task.
            usb_background_schedule();
        }
        // Wait for systick to reload.
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) {
        }
    }
}

static uint8_t _sm_free_count(uint8_t pio_index) {
    PIO pio = pio_get_instance(pio_index);
    uint8_t free_count = 0;
    for (size_t j = 0; j < NUM_PIO_STATE_MACHINES; j++) {
        if (!pio_sm_is_claimed(pio, j)) {
            free_count++;
        }
    }
    return free_count;
}

static bool _has_program_room(uint8_t pio_index, uint8_t program_size) {
    PIO pio = pio_get_instance(pio_index);
    pio_program_t program_struct = {
        .instructions = NULL,
        .length = program_size,
        .origin = -1
    };
    return pio_can_add_program(pio, &program_struct);
}

// As of 0.6.1, the PIO resource requirement is 1 PIO with 3 state machines &
// 32 instructions. Since there are only 32 instructions in a state machine, it should
// be impossible to have an allocated state machine but 32 instruction slots available;
// go ahead and check for it anyway.
//
// Since we check that ALL state machines are available, it's not possible for the GPIO
// ranges to mismatch on rp2350b
static size_t get_usb_pio(void) {
    for (size_t i = 0; i < NUM_PIOS; i++) {
        if (_has_program_room(i, 32) && _sm_free_count(i) == NUM_PIO_STATE_MACHINES) {
            return i;
        }
    }
    mp_raise_RuntimeError(MP_ERROR_TEXT("All state machines in use"));
}


usb_host_port_obj_t *common_hal_usb_host_port_construct(const mcu_pin_obj_t *dp, const mcu_pin_obj_t *dm) {
    if ((dp->number + 1 != dm->number)
        && (dp->number - 1 != dm->number)) {
        raise_ValueError_invalid_pins();
    }
    usb_host_port_obj_t *self = &usb_host_instance;

    // Return the singleton if given the same pins.
    if (self->dp != NULL) {
        if (self->dp != dp || self->dm != dm) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("%q in use"), MP_QSTR_usb_host);
        }
        return self;
    }

    assert_pin_free(dp);
    assert_pin_free(dm);

    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.skip_alarm_pool = true;
    pio_cfg.pin_dp = dp->number;
    if (dp->number - 1 == dm->number) {
        pio_cfg.pinout = PIO_USB_PINOUT_DMDP;
    }
    pio_cfg.pio_tx_num = get_usb_pio();
    pio_cfg.pio_rx_num = pio_cfg.pio_tx_num;
    int dma_ch = dma_claim_unused_channel(false);
    if (dma_ch < 0) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All dma channels in use"));
    }
    pio_cfg.tx_ch = dma_ch;

    self->base.type = &usb_host_port_type;
    self->dp = dp;
    self->dm = dm;

    PIO pio = pio_get_instance(pio_cfg.pio_tx_num);

    // Unclaim everything so that the library can.
    dma_channel_unclaim(pio_cfg.tx_ch);

    // Set all of the state machines to never reset.
    rp2pio_statemachine_never_reset(pio, pio_cfg.sm_tx);
    rp2pio_statemachine_never_reset(pio, pio_cfg.sm_rx);
    rp2pio_statemachine_never_reset(pio, pio_cfg.sm_eop);

    common_hal_never_reset_pin(dp);
    common_hal_never_reset_pin(dm);

    // Core 1 will run the SOF interrupt directly.
    _core1_ready = false;
    multicore_launch_core1(core1_main);
    while (!_core1_ready) {
    }

    tuh_configure(TUH_OPT_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);
    tuh_init(TUH_OPT_RHPORT);

    return self;
}

// Not used, but we must define to put this hook into SRAM
void __not_in_flash_func(tuh_event_hook_cb)(uint8_t rhport, uint32_t eventid, bool in_isr) {
    (void)rhport;
    (void)eventid;
    (void)in_isr;
}
