// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: 2025 rianadon
//
// SPDX-License-Identifier: MIT

#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/usb/cdc_host/Serial.h"
#include "shared-module/usb/cdc_host/Serial.h"
#include "supervisor/shared/tick.h"

#include "tusb.h"
#include "class/cdc/cdc_host.h"
#include "py/stream.h"

size_t common_hal_usb_host_cdc_serial_read(usb_cdc_host_serial_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (!tuh_cdc_mounted(self->idx)) {
        *errcode = ENODEV;
        return 0;
    }

    size_t total_read = tuh_cdc_read(self->idx, data, len);
    *errcode = 0;
    return total_read;
}

size_t common_hal_usb_host_cdc_serial_write(usb_cdc_host_serial_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (!tuh_cdc_mounted(self->idx)) {
        *errcode = ENODEV;
        return 0;
    }

    size_t total_written = tuh_cdc_write(self->idx, data, len);
    *errcode = 0; // Success
    return total_written;
}


uint32_t common_hal_usb_host_cdc_serial_get_in_waiting(usb_cdc_host_serial_obj_t *self) {
    if (!tuh_cdc_mounted(self->idx)) {
        return 0;
    }
    return tuh_cdc_read_available(self->idx);
}

uint32_t common_hal_usb_host_cdc_serial_get_out_waiting(usb_cdc_host_serial_obj_t *self) {
    if (!tuh_cdc_mounted(self->idx)) {
        return 0;
    }
    uint32_t available_space = tuh_cdc_write_available(self->idx);
    if (available_space > CFG_TUH_CDC_TX_BUFSIZE) {
        return 0;
    }
    return CFG_TUH_CDC_TX_BUFSIZE - available_space;
}

void common_hal_usb_host_cdc_serial_reset_input_buffer(usb_cdc_host_serial_obj_t *self) {
    if (tuh_cdc_mounted(self->idx)) {
        tuh_cdc_read_clear(self->idx);
    }
}

uint32_t common_hal_usb_host_cdc_serial_reset_output_buffer(usb_cdc_host_serial_obj_t *self) {
    uint32_t bytes_cleared = 0;
    if (tuh_cdc_mounted(self->idx)) {
        bytes_cleared = common_hal_usb_host_cdc_serial_get_out_waiting(self);
        tuh_cdc_write_clear(self->idx);
    }
    return bytes_cleared;
}

uint32_t common_hal_usb_host_cdc_serial_flush(usb_cdc_host_serial_obj_t *self) {
    if (!tuh_cdc_mounted(self->idx)) {
        return 0;
    }

    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t timeout_ticks = (self->write_timeout < 0) ? 0 : float_to_uint64(self->write_timeout * 1000);

    uint32_t initial_waiting = common_hal_usb_host_cdc_serial_get_out_waiting(self);

    while (common_hal_usb_host_cdc_serial_get_out_waiting(self) > 0) {
        tuh_cdc_write_flush(self->idx);

        if (!(self->write_timeout < 0 || self->write_timeout > 0)) {
            return initial_waiting - common_hal_usb_host_cdc_serial_get_out_waiting(self);
        }

        if (self->write_timeout > 0) {
            if (supervisor_ticks_ms64() - start_ticks >= timeout_ticks) {
                return initial_waiting - common_hal_usb_host_cdc_serial_get_out_waiting(self);
            }
        }

        RUN_BACKGROUND_TASKS;
    }

    return initial_waiting;
}

bool common_hal_usb_host_cdc_serial_get_connected(usb_cdc_host_serial_obj_t *self) {
    return tuh_cdc_mounted(self->idx) && tuh_cdc_connected(self->idx);
}

mp_float_t common_hal_usb_host_cdc_serial_get_timeout(usb_cdc_host_serial_obj_t *self) {
    return self->timeout;
}

void common_hal_usb_host_cdc_serial_set_timeout(usb_cdc_host_serial_obj_t *self, mp_float_t timeout) {
    self->timeout = timeout;
}

mp_float_t common_hal_usb_host_cdc_serial_get_write_timeout(usb_cdc_host_serial_obj_t *self) {
    return self->write_timeout;
}

void common_hal_usb_host_cdc_serial_set_write_timeout(usb_cdc_host_serial_obj_t *self, mp_float_t write_timeout) {
    self->write_timeout = write_timeout;
}
