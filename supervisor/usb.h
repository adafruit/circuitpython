// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Ports must call this as frequently as they can in order to keep the USB
// connection alive and responsive.  Normally this is called from background
// tasks after the USB IRQ handler is executed, but in specific circumstances
// it may be necessary to call it directly.
void usb_background(void);

// Schedule usb background
void usb_background_schedule(void);

// Console (CDC interface 0) input. The default implementations read TinyUSB's
// fifo directly. A port that runs tud_task() in its own task (espressif) overrides
// them to stage input in a ringbuf, so the fifo is only touched on that task:
// usb_cdc_rx_drain() runs there, the other three on the VM task.
//
// usb_cdc_rx_read() copies up to len bytes into *data and returns the count, 0 when
// nothing is pending. usb_cdc_rx_available() returns the pending byte count.
// usb_cdc_rx_clear() discards staged input; callers flush the TinyUSB fifo themselves.
// usb_cdc_rx_drain() moves fifo bytes into the staging buffer; a no-op by default.
void usb_cdc_rx_drain(void);
size_t usb_cdc_rx_read(uint8_t *data, size_t len);
size_t usb_cdc_rx_available(void);
void usb_cdc_rx_clear(void);

// Ports must call this from their particular USB IRQ handler
void usb_irq_handler(int instance);

// Only inits the USB peripheral clocks and pins. The peripheral will be initialized by
// TinyUSB.
void init_usb_hardware(void);

// Temporary hook for code after init. Only used for RP2040.
void post_usb_init(void);

// Indexes and counts updated as descriptors are built.
typedef struct {
    size_t current_interface;
    size_t current_endpoint;
    size_t num_in_endpoints;
    size_t num_out_endpoints;
} descriptor_counts_t;

typedef struct {
    uint16_t vid;
    uint16_t pid;
    char manufacturer_name[128];
    char product_name[128];
} usb_identification_t;

// Shared implementation.
bool usb_enabled(void);
void usb_add_interface_string(uint8_t interface_string_index, const char str[]);
bool usb_build_descriptors(const usb_identification_t *identification);
bool usb_connected(void);
void usb_disconnect(void);
void usb_init(void);
void usb_set_defaults(void);
size_t usb_boot_py_data_size(void);
void usb_get_boot_py_data(uint8_t *temp_storage, size_t temp_storage_size);
void usb_return_boot_py_data(uint8_t *temp_storage, size_t temp_storage_size);

// Further initialization that must be done with a VM present.
void usb_setup_with_vm(void);


// Propagate plug/unplug events to the MSC logic.
#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
size_t usb_msc_descriptor_length(void);
size_t usb_msc_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string);
void usb_msc_mount(void);
void usb_msc_umount(void);

#include "extmod/vfs_fat.h"
void usb_msc_remount(fs_user_mount_t *fs_mount);
#endif

#if CIRCUITPY_USB_KEYBOARD_WORKFLOW
void usb_keyboard_init(void);
uint32_t usb_keyboard_chars_available(void);
char usb_keyboard_read_char(void);

void usb_keyboard_status(void);

bool usb_keyboard_in_use(uint8_t dev_addr, uint8_t interface);
void usb_keyboard_detach(uint8_t dev_addr, uint8_t interface);
void usb_keyboard_attach(uint8_t dev_addr, uint8_t interface);
void usb_keymap_set(const uint8_t *buf, size_t len);
#endif
