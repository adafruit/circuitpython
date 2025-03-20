// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/serial.h"

#include "bindings/zephyr_serial/UART.h"

static zephyr_serial_uart_obj_t zephyr_console;
static uint8_t buffer[64];

void port_serial_early_init(void) {
    #if CIRCUITPY_USB_DEVICE == 0
    zephyr_console.base.type = &zephyr_serial_uart_type;
    zephyr_serial_uart_construct(&zephyr_console, DEVICE_DT_GET(DT_CHOSEN(zephyr_console)), sizeof(buffer), buffer);
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
    size_t count = zephyr_serial_uart_read(&zephyr_console, buf, 1, NULL);
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
    return zephyr_serial_uart_rx_characters_available(&zephyr_console);
    #else
    return 0;
    #endif
}

void port_serial_write_substring(const char *text, uint32_t length) {
    #if CIRCUITPY_USB_DEVICE == 0
    zephyr_serial_uart_write(&zephyr_console, text, length, NULL);
    #endif
}
