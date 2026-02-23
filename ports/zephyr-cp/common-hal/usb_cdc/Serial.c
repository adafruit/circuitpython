// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/usb_cdc/Serial.h"
#include "shared-bindings/busio/UART.h"
#include "supervisor/shared/tick.h"

mp_obj_t common_hal_usb_cdc_serial_construct_from_device(usb_cdc_serial_obj_t *self, const struct device *uart_device, uint16_t receiver_buffer_size, byte *receiver_buffer) {
    common_hal_busio_uart_construct_from_device(self, uart_device, receiver_buffer_size, receiver_buffer);
    self->base.type = &usb_cdc_serial_type;
    return MP_OBJ_FROM_PTR(self);
}

size_t common_hal_usb_cdc_serial_read(usb_cdc_serial_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    return common_hal_busio_uart_read(self, data, len, errcode);
}

size_t common_hal_usb_cdc_serial_write(usb_cdc_serial_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    return common_hal_busio_uart_write(self, data, len, errcode);
}

uint32_t common_hal_usb_cdc_serial_get_in_waiting(usb_cdc_serial_obj_t *self) {
    return common_hal_busio_uart_rx_characters_available(self);
}

uint32_t common_hal_usb_cdc_serial_get_out_waiting(usb_cdc_serial_obj_t *self) {
    // Return number of FIFO bytes currently occupied.
    // return CFG_TUD_CDC_TX_BUFSIZE - tud_cdc_n_write_available(self->idx);
    return 0;
}

void common_hal_usb_cdc_serial_reset_input_buffer(usb_cdc_serial_obj_t *self) {
    common_hal_busio_uart_clear_rx_buffer(self);
}

uint32_t common_hal_usb_cdc_serial_reset_output_buffer(usb_cdc_serial_obj_t *self) {
    // return tud_cdc_n_write_clear(self->idx);
    return 0;
}

uint32_t common_hal_usb_cdc_serial_flush(usb_cdc_serial_obj_t *self) {
    // return tud_cdc_n_write_flush(self->idx);
    return 0;
}

bool common_hal_usb_cdc_serial_get_connected(usb_cdc_serial_obj_t *self) {
    return !common_hal_busio_uart_deinited(self);
}

mp_float_t common_hal_usb_cdc_serial_get_timeout(usb_cdc_serial_obj_t *self) {
    return common_hal_busio_uart_get_timeout(self);
}

void common_hal_usb_cdc_serial_set_timeout(usb_cdc_serial_obj_t *self, mp_float_t timeout) {
    common_hal_busio_uart_set_timeout(self, timeout);
}

mp_float_t common_hal_usb_cdc_serial_get_write_timeout(usb_cdc_serial_obj_t *self) {
    return common_hal_busio_uart_get_write_timeout(self);
}

void common_hal_usb_cdc_serial_set_write_timeout(usb_cdc_serial_obj_t *self, mp_float_t write_timeout) {
    common_hal_busio_uart_set_write_timeout(self, write_timeout);
}
