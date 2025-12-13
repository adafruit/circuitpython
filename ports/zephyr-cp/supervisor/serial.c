// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/serial.h"

#if CIRCUITPY_USB_DEVICE == 0
#include "shared-bindings/busio/UART.h"

static busio_uart_obj_t zephyr_console;
static uint8_t buffer[64];
#endif

void port_serial_early_init(void) {
    #if CIRCUITPY_USB_DEVICE == 0
    zephyr_console.base.type = &busio_uart_type;
    common_hal_busio_uart_construct_from_device(&zephyr_console, DEVICE_DT_GET(DT_CHOSEN(zephyr_console)), sizeof(buffer), buffer);
    #endif
}

void port_serial_init(void) {
}

bool port_serial_connected(void) {
    #if CIRCUITPY_USB_DEVICE == 1
    return false;
    #else
    return true;
    #endif
}

char port_serial_read(void) {
    #if CIRCUITPY_USB_DEVICE == 0
    char buf[1];
    size_t count = common_hal_busio_uart_read(&zephyr_console, buf, 1, NULL);
    if (count == 0) {
        return -1;
    }
    return buf[0];
    #else
    return -1;
    #endif
}

uint32_t port_serial_bytes_available(void) {
    #if CIRCUITPY_USB_DEVICE == 0
    return common_hal_busio_uart_rx_characters_available(&zephyr_console);
    #else
    return 0;
    #endif
}

void port_serial_write_substring(const char *text, uint32_t length) {
    #if CIRCUITPY_USB_DEVICE == 0
    common_hal_busio_uart_write(&zephyr_console, text, length, NULL);
    #endif
}
