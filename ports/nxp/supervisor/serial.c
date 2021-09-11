/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mphal.h"
#include <string.h>
#include "supervisor/serial.h"

#include "Driver_USART.h"

extern ARM_DRIVER_USART Driver_USART1;

static char rx_buf[256u];
static size_t rd;
static volatile size_t wr;
static bool is_init;

// ARM_USART_SignalEvent_t
static void cb_event(uint32_t event) {
    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
        // FIXME: Replace the code below circular buffer
        size_t _wr = wr;
        _wr++;
        if (_wr >= (sizeof(rx_buf) / sizeof(rx_buf[0]))) {
            _wr = 0u;
        }

        wr = _wr;
    }

    return;
}

void serial_early_init(void) {
    return;
}

void serial_init(void) {
    #if (1)
    rd = 0u;
    wr = 0u;
    is_init = true;
    memset(&rx_buf[0u], '\0', sizeof(rx_buf));

    int32_t status = Driver_USART1.Initialize(cb_event);
    assert((ARM_DRIVER_OK == status));

    status = Driver_USART1.PowerControl(ARM_POWER_FULL);
    assert((ARM_DRIVER_OK == status));

    uint32_t control = ARM_USART_MODE_ASYNCHRONOUS
        | ARM_USART_DATA_BITS_8
        | ARM_USART_PARITY_NONE
        | ARM_USART_STOP_BITS_1
        | ARM_USART_FLOW_CONTROL_NONE;
    uint32_t baudrate = 115200u;
    status = Driver_USART1.Control(control, baudrate);
    assert((ARM_DRIVER_OK == status));

    status = Driver_USART1.Control(ARM_USART_CONTROL_TX, 1); // enable TX output
    assert((ARM_DRIVER_OK == status));

    status = Driver_USART1.Control(ARM_USART_CONTROL_RX, 1); // enable RX input
    assert((ARM_DRIVER_OK == status));

    #else
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) == HAL_OK) {
        stm32f4_peripherals_status_led(1,1);
    }
    #endif
    return;
}

bool serial_connected(void) {
    return true;
}

char serial_read(void) {
    #if (1)
    char data = '\0';

    if (rd != wr) {
        data = rx_buf[rd];
        rd++;
        if (rd >= (sizeof(rx_buf) / sizeof(rx_buf[0]))) {
            rd = 0u;
        }
    }

    return data;
    #else
    uint8_t data;
    HAL_UART_Receive(&huart2, &data, 1,500);
    return data;
    #endif
}

bool serial_bytes_available(void) {
    #if (1)

    size_t RxCount = Driver_USART1.GetRxCount();
    size_t _wr = wr;
    bool is_empty = (_wr == rd);

    if ((RxCount) || (is_init)) {
        #if (0)
        if ((_wr != rd) || is_init) {
        #else
        {
            #endif
            is_init = false;

            Driver_USART1.Receive(&rx_buf[_wr], 1u);
        }
    }

    return !is_empty;
    #else
    return __HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE);
    #endif
}

void serial_write(const char *text) {
    #if (1)
    const size_t len = strlen(text);
    Driver_USART1.Send(text, len);

    ARM_USART_STATUS status;
    do {
        status = Driver_USART1.GetStatus();
    } while (status.tx_busy);

    return;
    #else
    serial_write_substring(text, strlen(text));
    #endif
}

void serial_write_substring(const char *text, uint32_t len) {
    if (len > 0U) {
        #if (1)
        const size_t num = strlen(text);
        if (num < len) {
            len = num;
        }
        Driver_USART1.Send(text, len);

        ARM_USART_STATUS status;
        do {
            status = Driver_USART1.GetStatus();
        } while (status.tx_busy);

        #else
        HAL_UART_Transmit(&huart2, (uint8_t *)text, len, 5000);
        #endif
    }
    return;
}

#if defined(CIRCUITPY_USB_CDC) && !(CIRCUITPY_USB_CDC)
void supervisor_workflow_reset(void) {
    return;
}
#endif
