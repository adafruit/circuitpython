// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/usb_hid/Device.h"
#include "shared-module/usb_hid/__init__.h"
#include "shared-module/usb_hid/Device.h"
#include "shared-module/usb_hid/report_descriptors.h"
#include "supervisor/shared/tick.h"
#include "tusb.h"

const usb_hid_device_obj_t usb_hid_device_keyboard_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = keyboard_report_descriptor,
    .report_descriptor_length = sizeof(keyboard_report_descriptor),
    .usage_page = 0x01,
    .usage = 0x06,
    .num_report_ids = 1,
    .report_ids = { 0x01, },
    .in_report_lengths = { 8, },
    .out_report_lengths = { 1, },
};

const usb_hid_device_obj_t usb_hid_device_mouse_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = mouse_report_descriptor,
    .report_descriptor_length = sizeof(mouse_report_descriptor),
    .usage_page = 0x01,
    .usage = 0x02,
    .num_report_ids = 1,
    .report_ids = { 0x02, },
    .in_report_lengths = { 4, },
    .out_report_lengths = { 0, },
};

const usb_hid_device_obj_t usb_hid_device_consumer_control_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = consumer_control_report_descriptor,
    .report_descriptor_length = sizeof(consumer_control_report_descriptor),
    .usage_page = 0x0C,
    .usage = 0x01,
    .num_report_ids = 1,
    .report_ids = { 0x03 },
    .in_report_lengths = { 2, },
    .out_report_lengths = { 0, },
};

char *custom_usb_hid_interface_name;

static size_t get_report_id_idx(usb_hid_device_obj_t *self, size_t report_id) {
    for (size_t i = 0; i < self->num_report_ids; i++) {
        if (report_id == self->report_ids[i]) {
            return i;
        }
    }
    return CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR;
}

// See if report_id is used by this device. If it is -1, then return the sole report id used by this device,
// which might be 0 if no report_id was supplied.
uint8_t common_hal_usb_hid_device_validate_report_id(usb_hid_device_obj_t *self, mp_int_t report_id_arg) {
    if (report_id_arg == -1 && self->num_report_ids == 1) {
        return self->report_ids[0];
    }
    if (!(report_id_arg >= 0 &&
          get_report_id_idx(self, (size_t)report_id_arg) < CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR)) {
        mp_arg_error_invalid(MP_QSTR_report_id);
    }
    return (uint8_t)report_id_arg;
}

void common_hal_usb_hid_device_construct(usb_hid_device_obj_t *self, mp_obj_t report_descriptor, uint16_t usage_page, uint16_t usage, size_t num_report_ids, uint8_t *report_ids, uint8_t *in_report_lengths, uint8_t *out_report_lengths) {
    mp_arg_validate_length_max(
        num_report_ids, CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR, MP_QSTR_report_ids);

    // report buffer pointers are NULL at start, and are created when USB is initialized.
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(report_descriptor, &bufinfo, MP_BUFFER_READ);
    self->report_descriptor_length = bufinfo.len;

    // Copy the raw descriptor bytes into a heap obj. We don't keep the Python descriptor object.

    uint8_t *descriptor_bytes = gc_alloc(bufinfo.len, false);
    memcpy(descriptor_bytes, bufinfo.buf, bufinfo.len);
    self->report_descriptor = descriptor_bytes;

    self->usage_page = usage_page;
    self->usage = usage;
    self->num_report_ids = num_report_ids;
    memcpy(self->report_ids, report_ids, num_report_ids);
    memcpy(self->in_report_lengths, in_report_lengths, num_report_ids);
    memcpy(self->out_report_lengths, out_report_lengths, num_report_ids);
}

uint16_t common_hal_usb_hid_device_get_usage_page(usb_hid_device_obj_t *self) {
    return self->usage_page;
}

uint16_t common_hal_usb_hid_device_get_usage(usb_hid_device_obj_t *self) {
    return self->usage;
}

void common_hal_usb_hid_device_send_report(usb_hid_device_obj_t *self, uint8_t *report, uint8_t len, uint8_t report_id) {
    // report_id and len have already been validated for this device.
    size_t id_idx = get_report_id_idx(self, report_id);

    mp_arg_validate_length(len, self->in_report_lengths[id_idx], MP_QSTR_report);

    // Wait until interface is ready, timeout = 2 seconds
    uint64_t end_ticks = supervisor_ticks_ms64() + 2000;
    while ((supervisor_ticks_ms64() < end_ticks) && !tud_hid_ready()) {
        RUN_BACKGROUND_TASKS;
    }

    if (!tud_suspended()) {
        if (!tud_hid_ready()) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("USB busy"));
        }

        const uint8_t boot_device = usb_hid_boot_device();
        if (boot_device != 0 && tud_hid_get_protocol() == HID_PROTOCOL_BOOT) {
            // In boot protocol the host reads the fixed boot report, which carries no report ID,
            // and ignores every device other than the boot device.
            if (self->usage_page == HID_USAGE_PAGE_DESKTOP &&
                ((boot_device == 1 && self->usage == HID_USAGE_DESKTOP_KEYBOARD) ||
                 (boot_device == 2 && self->usage == HID_USAGE_DESKTOP_MOUSE))) {
                report_id = 0;
            } else {
                return;
            }
        }

        if (!tud_hid_report(report_id, report, len)) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("USB error"));
        }
    } else {
        tud_remote_wakeup();
    }
}

mp_obj_t common_hal_usb_hid_device_get_last_received_report(usb_hid_device_obj_t *self, uint8_t report_id) {
    // report_id has already been validated for this device.
    size_t id_idx = get_report_id_idx(self, report_id);
    if (!self->out_report_buffers_updated[id_idx]) {
        return mp_const_none;
    }
    self->out_report_buffers_updated[id_idx] = false;
    return mp_obj_new_bytes(self->out_report_buffers[id_idx], self->out_report_lengths[id_idx]);
}

void usb_hid_device_create_report_buffers(usb_hid_device_obj_t *self) {
    for (size_t i = 0; i < self->num_report_ids; i++) {
        // The IN buffers are used only for tud_hid_get_report_cb(),
        // which is an unusual case. Normally we can just pass the data directly with tud_hid_report().
        self->in_report_buffers[i] =
            self->in_report_lengths[i] > 0
            ? gc_alloc(self->in_report_lengths[i], false)
            : NULL;

        self->out_report_buffers[i] =
            self->out_report_lengths[i] > 0
            ? gc_alloc(self->out_report_lengths[i], false)
            : NULL;
    }
    memset(self->out_report_buffers_updated, 0, sizeof(self->out_report_buffers_updated));
}


// Callback invoked when we receive Get_Report request through control endpoint
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    (void)itf;
    // Support Input Report and Feature Report
    if (report_type != HID_REPORT_TYPE_INPUT && report_type != HID_REPORT_TYPE_FEATURE) {
        return 0;
    }

    // fill buffer with current report

    usb_hid_device_obj_t *hid_device;
    size_t id_idx;
    // Find device with this report id, and get the report id index.
    if (usb_hid_get_device_with_report_id(report_id, &hid_device, &id_idx)) {
        // Make sure buffer exists before trying to copy into it.
        if (hid_device->in_report_buffers[id_idx]) {
            memcpy(buffer, hid_device->in_report_buffers[id_idx], reqlen);
            return reqlen;
        }
    }
    return 0;
}

// Callback invoked when we receive Set_Report request through control endpoint
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    (void)itf;

    usb_hid_device_obj_t *hid_device = NULL;
    size_t id_idx;

    // As of https://github.com/hathach/tinyusb/pull/2253, HID_REPORT_TYPE_INVALID reports are not
    // sent to this callback, but are instead sent to tud_hid_report_fail_cb(), which we don't bother
    // to implement.
    // So this callback is only going to see HID_REPORT_TYPE_OUTPUT.
    // HID_REPORT_TYPE_FEATURE is not used yet.
    if (report_id == 0) {
        // This could be a report with a non-zero report ID in the first byte, or
        // it could be for report ID 0.
        // Heuristic: see if there's a device with report ID 0, and if its report length matches
        // the size of the incoming buffer. In that case, assume the first byte is not the report ID,
        // but is data. Otherwise use the first byte as the report id.
        if (usb_hid_get_device_with_report_id(0, &hid_device, &id_idx) &&
            hid_device &&
            hid_device->out_report_buffers[id_idx] &&
            hid_device->out_report_lengths[id_idx] == bufsize) {
            // Use as is, with report_id 0.
        } else {
            // No matching report ID 0, so use the first byte as the report ID.
            report_id = buffer[0];
            buffer++;
            bufsize--;
        }
    }

    // report_id might be changed due to parsing above, so test again.
    if ((report_id == 0) ||
        // Fetch the matching device if we don't already have the report_id 0 device.
        (usb_hid_get_device_with_report_id(report_id, &hid_device, &id_idx) &&
         hid_device &&
         hid_device->out_report_buffers[id_idx] &&
         hid_device->out_report_lengths[id_idx] == bufsize)) {
        // If a report of the correct size has been read, save it in the proper OUT report buffer.
        memcpy(hid_device->out_report_buffers[id_idx], buffer, bufsize);
        hid_device->out_report_buffers_updated[id_idx] = true;
    }
}
