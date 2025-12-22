// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/busio/UART.h"

typedef busio_uart_obj_t usb_cdc_serial_obj_t;

// Helper function for Zephyr-specific initialization from device tree
mp_obj_t common_hal_usb_cdc_serial_construct_from_device(usb_cdc_serial_obj_t *self, const struct device *uart_device, uint16_t receiver_buffer_size, byte *receiver_buffer);
