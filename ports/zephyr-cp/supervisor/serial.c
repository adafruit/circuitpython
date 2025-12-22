// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/serial.h"

#include "supervisor/zephyr-cp.h"

#if CIRCUITPY_USB_DEVICE == 1
#include "shared-bindings/usb_cdc/Serial.h"
usb_cdc_serial_obj_t *usb_console;
#else
#include "shared-bindings/busio/UART.h"
static busio_uart_obj_t uart_console;
static uint8_t buffer[64];
#endif

void port_serial_early_init(void) {
    #if CIRCUITPY_USB_DEVICE == 0
    uart_console.base.type = &busio_uart_type;
    common_hal_busio_uart_construct_from_device(&uart_console, DEVICE_DT_GET(DT_CHOSEN(zephyr_console)), sizeof(buffer), buffer);
    #endif
}

void port_serial_init(void) {
    #if CIRCUITPY_USB_DEVICE == 1
    usb_console = usb_cdc_serial_get_console();
    #endif
}

bool port_serial_connected(void) {
    #if CIRCUITPY_USB_DEVICE == 1
    if (usb_console == NULL) {
        return false;
    }
    return common_hal_usb_cdc_serial_get_connected(usb_console);
    #else
    return true;
    #endif
}

char port_serial_read(void) {
    #if CIRCUITPY_USB_DEVICE == 1
    if (usb_console == NULL) {
        return -1;
    }
    char buf[1];
    size_t count = common_hal_usb_cdc_serial_read(usb_console, buf, 1, NULL);
    if (count == 0) {
        return -1;
    }
    return buf[0];
    #else
    char buf[1];
    size_t count = common_hal_busio_uart_read(&uart_console, buf, 1, NULL);
    if (count == 0) {
        return -1;
    }
    return buf[0];
    #endif
}

uint32_t port_serial_bytes_available(void) {
    #if CIRCUITPY_USB_DEVICE == 1
    if (usb_console == NULL) {
        return 0;
    }
    return common_hal_usb_cdc_serial_get_in_waiting(usb_console);
    #else
    return common_hal_busio_uart_rx_characters_available(&uart_console);
    #endif
}

void port_serial_write_substring(const char *text, uint32_t length) {
    #if CIRCUITPY_USB_DEVICE == 1
    if (usb_console == NULL) {
        return;
    }
    common_hal_usb_cdc_serial_write(usb_console, text, length, NULL);
    #else
    common_hal_busio_uart_write(&uart_console, text, length, NULL);
    #endif
}
