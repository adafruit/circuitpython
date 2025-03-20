// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"
#include <string.h>
#include "supervisor/shared/serial.h"

#include "uart.h"
#include "uart_regs.h"

#ifndef MAX32_SERIAL
#define MAX32_SERIAL 0
#endif

#if MAX32_SERIAL
#ifdef MAX32690
#define CONSOLE_UART MXC_UART0
#endif
#endif

void port_serial_early_init(void) {

}

void port_serial_init(void) {
    #if MAX32_SERIAL
    MXC_GCR->clkctrl |= MXC_F_GCR_CLKCTRL_IBRO_EN;
    while (!(MXC_GCR->clkctrl & MXC_F_GCR_CLKCTRL_IBRO_RDY)) {
        ;
    }
    MXC_UART_Init(CONSOLE_UART, 115200, MXC_UART_IBRO_CLK);
    #endif
}

bool port_serial_connected(void) {
    return true;
}

char port_serial_read(void) {
    #if MAX32_SERIAL
    // uint8_t data;
    // HAL_UART_Receive(&huart2, &data, 1, 500);
    // return data;
    uint8_t rData;

    mxc_uart_req_t uart_req = {
        .uart = CONSOLE_UART,
        .rxCnt = 0,
        .txCnt = 0,
        .txData = NULL,
        .rxData = &rData,
        .txLen = 0,
        .rxLen = 1
    };
    MXC_UART_Transaction(&uart_req);
    return rData;
    #else
    return -1;
    #endif
}

uint32_t port_serial_bytes_available(void) {
    #if MAX32_SERIAL
    return MXC_UART_GetRXFIFOAvailable(CONSOLE_UART);
    #else
    return 0;
    #endif
}

void port_serial_write_substring(const char *text, uint32_t len) {
    #if MAX32_SERIAL
    mxc_uart_req_t uart_req = {
        .uart = CONSOLE_UART,
        .rxCnt = 0,
        .txCnt = 0,
        .txData = (const unsigned char *)text,
        .rxData = NULL,
        .txLen = len,
        .rxLen = 0
    };
    MXC_UART_Transaction(&uart_req);
    #endif
}
