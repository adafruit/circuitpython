// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc
//
// SPDX-License-Identifier: MIT

#include "supervisor/usb.h"
#include "common-hal/microcontroller/Pin.h"

#include "py/mpconfig.h"

#include "lib/tinyusb/src/device/usbd.h"

// max32 includes
#include "mxc_sys.h"
#include "gcr_regs.h"
#include "mcr_regs.h"

void init_usb_hardware(void) {
    // USB GPIOs are non-configurable on MAX32 devices
    // No need to add them to the never_reset list for mcu/Pin API.

    // 1 ms SysTick initialized in board.c

    // Enable requisite clocks & power for USB
    MXC_SYS_ClockSourceEnable(MXC_SYS_CLOCK_IPO);
    MXC_MCR->ldoctrl |= MXC_F_MCR_LDOCTRL_0P9EN;
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_USB);
    MXC_SYS_Reset_Periph(MXC_SYS_RESET0_USB);

    // Supervisor calls TinyUSB's dcd_init,
    // which initializes the USB PHY.
    // Depending on CIRCUITPY_TINYUSB and CIRCUITPY_USB_DEVICE

    // Interrupt enables are left to TUSB depending on the device class
}

void USB_IRQHandler(void) {
    // Schedules USB background callback
    // appropriate to a given device class via TinyUSB lib
    usb_irq_handler(0);
}
