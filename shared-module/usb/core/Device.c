// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2025 Sam Blenny
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/usb/core/Device.h"

#include "tusb_config.h"
#include "supervisor/port.h"
#include "supervisor/port_heap.h"

#include "lib/tinyusb/src/host/hcd.h"
#include "lib/tinyusb/src/host/usbh.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/usb/core/__init__.h"
#include "shared-bindings/usb/util/__init__.h"
#include "shared-module/usb/utf16le.h"
#include "supervisor/shared/tick.h"
#include "supervisor/usb.h"

// Track what device numbers are mounted. We can't use tuh_ready() because it is
// true before enumeration completes and TinyUSB drivers are started.
static size_t _mounted_devices = 0;

void tuh_mount_cb(uint8_t dev_addr) {
    _mounted_devices |= 1 << dev_addr;
}

// Bulk IN buffering needs the completion callback to run from CircuitPython's
// background tasks, where it can't interrupt read() partway through.
#define IN_BUFFER_ENABLED (CFG_TUSB_OS == OPT_OS_NONE || CFG_TUSB_OS == OPT_OS_PICO)

#if IN_BUFFER_ENABLED
static void _in_buffer_release_device(uint8_t daddr);
#endif

void tuh_umount_cb(uint8_t dev_addr) {
    _mounted_devices &= ~(1 << dev_addr);
    #if IN_BUFFER_ENABLED
    _in_buffer_release_device(dev_addr);
    #endif
}

static xfer_result_t _xfer_result;
static size_t _actual_len;

#if !CIRCUITPY_ALL_MEMORY_DMA_CAPABLE
// Helper to ensure buffer is DMA-capable for transfer operations
static uint8_t *_ensure_dma_buffer(usb_core_device_obj_t *self, const uint8_t *buffer, size_t len, bool for_write) {
    if (port_buffer_is_dma_capable(buffer)) {
        return (uint8_t *)buffer;  // Already DMA-capable, use directly
    }

    uint8_t *dma_buffer = port_malloc(len, true);  // true = DMA capable
    if (dma_buffer == NULL) {
        return NULL;  // Allocation failed
    }

    // Copy data to DMA buffer if writing
    if (for_write && buffer != NULL) {
        memcpy(dma_buffer, buffer, len);
    }

    return dma_buffer;
}

#endif

#if IN_BUFFER_ENABLED
// Once Python reads a bulk IN endpoint, keep a transfer queued on it so the device
// is polled while Python is busy. Completed transfers wait here for read(). When
// the queue is full no transfer is queued, and the device holds its data.
#define IN_BUFFER_SLOTS 4
#define IN_BUFFER_DEPTH 8
// How long a released buffer is kept for an aborted transfer to finish writing.
#define IN_BUFFER_STALE_MS 10

typedef struct {
    usb_core_device_obj_t *owner; // NULL when the slot is not in use
    uint8_t *data; // IN_BUFFER_DEPTH transfers of mps bytes each
    uint8_t *stale_data; // released while a transfer was queued, freed by _in_buffer_reap()
    uint32_t stale_time;
    uint16_t len[IN_BUFFER_DEPTH];
    uint16_t mps;
    uint16_t offset; // bytes of the head transfer already read
    uint8_t daddr;
    uint8_t ep_addr;
    uint8_t head;
    uint8_t count;
    uint8_t gen; // changed on release so an old completion is ignored
    bool busy;
    xfer_result_t error;
} in_buffer_t;

static in_buffer_t _in_buffers[IN_BUFFER_SLOTS];

static void _in_buffer_queue(in_buffer_t *buf);

static void _in_buffer_cb(tuh_xfer_t *xfer) {
    in_buffer_t *buf = &_in_buffers[xfer->user_data & 0xff];
    if (buf->owner == NULL || !buf->busy || buf->gen != (uint8_t)(xfer->user_data >> 8)) {
        return;
    }
    buf->busy = false;
    if (xfer->result != XFER_RESULT_SUCCESS) {
        buf->error = xfer->result;
        return;
    }
    buf->len[(buf->head + buf->count) % IN_BUFFER_DEPTH] = xfer->actual_len;
    buf->count++;
    _in_buffer_queue(buf);
}

static void _in_buffer_queue(in_buffer_t *buf) {
    if (buf->busy || buf->error != XFER_RESULT_SUCCESS || buf->count == IN_BUFFER_DEPTH) {
        return;
    }
    tuh_xfer_t xfer = {
        .daddr = buf->daddr,
        .ep_addr = buf->ep_addr,
        .buflen = buf->mps,
        .buffer = buf->data + ((buf->head + buf->count) % IN_BUFFER_DEPTH) * buf->mps,
        .complete_cb = _in_buffer_cb,
        .user_data = (buf - _in_buffers) | (buf->gen << 8),
    };
    buf->busy = tuh_edpt_xfer(&xfer);
}

static void _in_buffer_release(in_buffer_t *buf) {
    if (buf->busy) {
        // Some host controllers can still write into the buffer after an abort, and
        // TinyUSB may have its completion queued, so free it later.
        tuh_edpt_abort_xfer(buf->daddr, buf->ep_addr);
        buf->stale_data = buf->data;
        buf->stale_time = supervisor_ticks_ms32();
    } else {
        port_free(buf->data);
    }
    buf->data = NULL;
    buf->owner = NULL;
    buf->busy = false;
    buf->head = 0;
    buf->count = 0;
    buf->offset = 0;
    buf->gen++;
}

static bool _in_buffer_in_use(uint8_t daddr, uint8_t ep_addr) {
    for (size_t i = 0; i < IN_BUFFER_SLOTS; i++) {
        in_buffer_t *buf = &_in_buffers[i];
        if (buf->owner != NULL && buf->daddr == daddr && buf->ep_addr == ep_addr) {
            return true;
        }
    }
    return false;
}

static void _in_buffer_release_device(uint8_t daddr) {
    for (size_t i = 0; i < IN_BUFFER_SLOTS; i++) {
        in_buffer_t *buf = &_in_buffers[i];
        if (buf->owner != NULL && buf->daddr == daddr) {
            _in_buffer_release(buf);
        }
    }
}

// Runs TinyUSB until released buffers can't be written to or completed into, then
// frees them. Called from Python context, never from a finaliser or callback.
static void _in_buffer_reap(void) {
    for (size_t i = 0; i < IN_BUFFER_SLOTS; i++) {
        in_buffer_t *buf = &_in_buffers[i];
        if (buf->stale_data == NULL) {
            continue;
        }
        while (supervisor_ticks_ms32() - buf->stale_time < IN_BUFFER_STALE_MS) {
            RUN_BACKGROUND_TASKS;
        }
        port_free(buf->stale_data);
        buf->stale_data = NULL;
    }
}
#endif

bool common_hal_usb_core_device_construct(usb_core_device_obj_t *self, uint8_t device_address) {
    if (!tuh_inited()) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No usb host port initialized"));
    }

    if (device_address == 0 || device_address > CFG_TUH_DEVICE_MAX + CFG_TUH_HUB) {
        return false;
    }
    if ((_mounted_devices & (1 << device_address)) == 0) {
        return false;
    }
    self->device_address = device_address;
    self->first_langid = 0;
    _xfer_result = XFER_RESULT_INVALID;
    return true;
}

bool common_hal_usb_core_device_deinited(usb_core_device_obj_t *self) {
    return self->device_address == 0;
}

void common_hal_usb_core_device_deinit(usb_core_device_obj_t *self) {
    if (common_hal_usb_core_device_deinited(self)) {
        return;
    }
    #if IN_BUFFER_ENABLED
    for (size_t i = 0; i < IN_BUFFER_SLOTS; i++) {
        if (_in_buffers[i].owner == self) {
            _in_buffer_release(&_in_buffers[i]);
        }
    }
    #endif
    size_t open_size = sizeof(self->open_endpoints);
    for (size_t i = 0; i < open_size; i++) {
        if (self->open_endpoints[i] != 0) {
            #if IN_BUFFER_ENABLED
            // Another Device object for the same device is still buffering it.
            if (_in_buffer_in_use(self->device_address, self->open_endpoints[i])) {
                self->open_endpoints[i] = 0;
                continue;
            }
            #endif
            tuh_edpt_close(self->device_address, self->open_endpoints[i]);
            self->open_endpoints[i] = 0;
        }
    }
    self->device_address = 0;
}

uint16_t common_hal_usb_core_device_get_idVendor(usb_core_device_obj_t *self) {
    uint16_t vid;
    uint16_t pid;
    if (!tuh_vid_pid_get(self->device_address, &vid, &pid)) {
        mp_raise_usb_core_USBError(NULL);
    }
    return vid;
}

uint16_t common_hal_usb_core_device_get_idProduct(usb_core_device_obj_t *self) {
    uint16_t vid;
    uint16_t pid;
    if (!tuh_vid_pid_get(self->device_address, &vid, &pid)) {
        mp_raise_usb_core_USBError(NULL);
    }
    return pid;
}

static void _transfer_done_cb(tuh_xfer_t *xfer) {
    // Store the result so we stop waiting for the transfer.
    _xfer_result = xfer->result;
    // The passed in xfer is not the original one we passed in, so we need to
    // copy any info out that we want (like actual_len.)
    _actual_len = xfer->actual_len;
}

static bool _wait_for_callback(void) {
    while (!mp_hal_is_interrupted() &&
           _xfer_result == XFER_RESULT_INVALID) {
        // The background tasks include TinyUSB which will call the function
        // we provided above. In other words, the callback isn't in an interrupt.
        RUN_BACKGROUND_TASKS;
    }
    if (mp_hal_is_interrupted()) {
        // Handle case of VM being interrupted by Ctrl-C or autoreload
        return false;
    }
    // Handle callback result code from TinyUSB
    xfer_result_t result = _xfer_result;
    _xfer_result = XFER_RESULT_INVALID;
    switch (result) {
        case XFER_RESULT_SUCCESS:
            return true;
        case XFER_RESULT_ABORTED:
        case XFER_RESULT_FAILED:
            mp_raise_usb_core_USBError(NULL);
            break;
        case XFER_RESULT_STALLED:
            mp_raise_usb_core_USBError(MP_ERROR_TEXT("Pipe error"));
            break;
        case XFER_RESULT_TIMEOUT:
        case XFER_RESULT_INVALID:
            mp_raise_usb_core_USBTimeoutError();
            break;
    }
    return false;
}

static void _prepare_for_transfer(void) {
    // Prepare for transfer. Unless there is a timeout, these static globals will
    // get modified by the _transfer_done_cb() callback when tinyusb finishes the
    // transfer or encounters an error condition.
    _xfer_result = XFER_RESULT_INVALID;
    _actual_len = 0;
}

static void _abort_transfer(tuh_xfer_t *xfer) {
    bool aborted = tuh_edpt_abort_xfer(xfer->daddr, xfer->ep_addr);
    if (aborted) {
        // If the transfer was aborted, then we can continue.
        return;
    }
    uint32_t start_time = supervisor_ticks_ms32();
    // If not, we need to wait for it to finish, otherwise we may free memory out from under it.
    // Limit the wait time to 10 milliseconds to avoid blocking indefinitely.
    while (_xfer_result == XFER_RESULT_INVALID && (supervisor_ticks_ms32() - start_time < 10)) {
        // The background tasks include TinyUSB which will call the function
        // we provided above. In other words, the callback isn't in an interrupt.
        RUN_BACKGROUND_TASKS;
    }
}

// Only frees the transfer buffer on error.
static size_t _handle_timed_transfer_callback(tuh_xfer_t *xfer, mp_int_t timeout, bool our_buffer, bool raise_on_timeout) {
    if (xfer == NULL) {
        mp_raise_usb_core_USBError(NULL);
        return 0;
    }
    uint32_t start_time = supervisor_ticks_ms32();
    while ((timeout == 0 || supervisor_ticks_ms32() - start_time < (uint32_t)timeout) &&
           !mp_hal_is_interrupted() &&
           _xfer_result == XFER_RESULT_INVALID) {
        // The background tasks include TinyUSB which will call the function
        // we provided above. In other words, the callback isn't in an interrupt.
        RUN_BACKGROUND_TASKS;
    }
    if (mp_hal_is_interrupted()) {
        // Handle case of VM being interrupted by Ctrl-C or autoreload
        _abort_transfer(xfer);
        return 0;
    }
    // Handle transfer result code from TinyUSB
    xfer_result_t result = _xfer_result;
    _xfer_result = XFER_RESULT_INVALID;
    // Free the bounce buffer only on paths that raise: raising unwinds past
    // the caller's cleanup, so it must be freed here. Paths that return
    // normally leave ownership with the caller, which copies out of and
    // frees the buffer itself (freeing on both sides would be a double free).
    bool will_raise = result == XFER_RESULT_ABORTED || result == XFER_RESULT_FAILED ||
        result == XFER_RESULT_STALLED ||
        (result == XFER_RESULT_TIMEOUT && raise_on_timeout);
    if (our_buffer && will_raise) {
        port_free(xfer->buffer);
    }
    switch (result) {
        case XFER_RESULT_SUCCESS:
            return _actual_len;
        case XFER_RESULT_ABORTED:
        case XFER_RESULT_FAILED:
            mp_raise_usb_core_USBError(NULL);
            break;
        case XFER_RESULT_STALLED:
            mp_raise_usb_core_USBError(MP_ERROR_TEXT("Pipe error"));
            break;
        case XFER_RESULT_TIMEOUT:
            // This timeout comes from TinyUSB, so assume that it has stopped the
            // transfer (note: timeout logic may be unimplemented on TinyUSB side)
            if (!raise_on_timeout) {
                return 0;
            }
            mp_raise_usb_core_USBTimeoutError();
            break;
        case XFER_RESULT_INVALID:
            // This timeout comes from CircuitPython, not TinyUSB, so tell TinyUSB
            // to stop the transfer.
            _abort_transfer(xfer);
            if (!raise_on_timeout) {
                return 0;               // the caller owns (and frees) the buffer
            }
            if (our_buffer) {
                port_free(xfer->buffer);
            }
            mp_raise_usb_core_USBTimeoutError();
            break;
    }
    return 0;
}

static mp_obj_t _get_string(const uint16_t *temp_buf) {
    size_t utf16_len = ((temp_buf[0] & 0xff) - 2) / sizeof(uint16_t);
    if (utf16_len == 0) {
        return mp_const_none;
    }
    return utf16le_to_string(temp_buf + 1, utf16_len);
}

static void _get_langid(usb_core_device_obj_t *self) {
    if (self->first_langid != 0) {
        return;
    }
    // Two control bytes and one uint16_t language code.
    uint16_t temp_buf[2];
    _prepare_for_transfer();
    if (!tuh_descriptor_get_string(self->device_address, 0, 0, temp_buf, sizeof(temp_buf), _transfer_done_cb, 0)) {
        mp_raise_usb_core_USBError(NULL);
    } else if (_wait_for_callback()) {
        self->first_langid = temp_buf[1];
    }
}

mp_obj_t common_hal_usb_core_device_get_serial_number(usb_core_device_obj_t *self) {
    uint16_t temp_buf[127];
    tusb_desc_device_t descriptor;
    // First, be sure not to ask TinyUSB for a non-existent string (avoid error)
    if (!tuh_descriptor_get_device_local(self->device_address, &descriptor)) {
        return mp_const_none;
    }
    if (descriptor.iSerialNumber == 0) {
        return mp_const_none;
    }
    // Device does provide this string, so continue
    _get_langid(self);
    _prepare_for_transfer();
    if (!tuh_descriptor_get_serial_string(self->device_address, self->first_langid, temp_buf, sizeof(temp_buf), _transfer_done_cb, 0)) {
        mp_raise_usb_core_USBError(NULL);
    } else if (_wait_for_callback()) {
        return _get_string(temp_buf);
    }
    return mp_const_none;
}

mp_obj_t common_hal_usb_core_device_get_product(usb_core_device_obj_t *self) {
    uint16_t temp_buf[127];
    tusb_desc_device_t descriptor;
    // First, be sure not to ask TinyUSB for a non-existent string (avoid error)
    if (!tuh_descriptor_get_device_local(self->device_address, &descriptor)) {
        return mp_const_none;
    }
    if (descriptor.iProduct == 0) {
        return mp_const_none;
    }
    // Device does provide this string, so continue
    _get_langid(self);
    _prepare_for_transfer();
    if (!tuh_descriptor_get_product_string(self->device_address, self->first_langid, temp_buf, sizeof(temp_buf), _transfer_done_cb, 0)) {
        mp_raise_usb_core_USBError(NULL);
    } else if (_wait_for_callback()) {
        return _get_string(temp_buf);
    }
    return mp_const_none;
}

mp_obj_t common_hal_usb_core_device_get_manufacturer(usb_core_device_obj_t *self) {
    uint16_t temp_buf[127];
    tusb_desc_device_t descriptor;
    // First, be sure not to ask TinyUSB for a non-existent string (avoid error)
    if (!tuh_descriptor_get_device_local(self->device_address, &descriptor)) {
        return mp_const_none;
    }
    if (descriptor.iManufacturer == 0) {
        return mp_const_none;
    }
    // Device does provide this string, so continue
    _get_langid(self);
    _prepare_for_transfer();
    if (!tuh_descriptor_get_manufacturer_string(self->device_address, self->first_langid, temp_buf, sizeof(temp_buf), _transfer_done_cb, 0)) {
        mp_raise_usb_core_USBError(NULL);
    } else if (_wait_for_callback()) {
        return _get_string(temp_buf);
    }
    return mp_const_none;
}


mp_int_t common_hal_usb_core_device_get_bus(usb_core_device_obj_t *self) {
    tuh_bus_info_t bus_info;
    if (!tuh_bus_info_get(self->device_address, &bus_info)) {
        return 0;
    }
    return bus_info.rhport;
}

mp_obj_t common_hal_usb_core_device_get_port_numbers(usb_core_device_obj_t *self) {
    tuh_bus_info_t bus_info;
    if (!tuh_bus_info_get(self->device_address, &bus_info)) {
        return mp_const_none;
    }
    if (bus_info.hub_addr == 0) {
        return mp_const_none;
    }
    // USB allows for 5 hubs deep chaining. So we're at most 5 ports deep.
    mp_obj_t ports[5];
    size_t port_count = 0;
    tuh_bus_info_t current_bus_info = bus_info;
    while (current_bus_info.hub_addr != 0 && port_count < MP_ARRAY_SIZE(ports)) {
        // Reverse the order of the ports so most downstream comes last.
        ports[MP_ARRAY_SIZE(ports) - 1 - port_count] = MP_OBJ_NEW_SMALL_INT(current_bus_info.hub_port);
        port_count++;
        if (!tuh_bus_info_get(current_bus_info.hub_addr, &current_bus_info)) {
            break;
        }
    }
    return mp_obj_new_tuple(port_count, ports + (MP_ARRAY_SIZE(ports) - port_count));
}

mp_int_t common_hal_usb_core_device_get_speed(usb_core_device_obj_t *self) {
    tuh_bus_info_t bus_info;
    if (!tuh_bus_info_get(self->device_address, &bus_info)) {
        return 0;
    }
    switch (bus_info.speed) {
        case TUSB_SPEED_HIGH:
            return PYUSB_SPEED_HIGH;
        case TUSB_SPEED_FULL:
            return PYUSB_SPEED_FULL;
        case TUSB_SPEED_LOW:
            return PYUSB_SPEED_LOW;
        default:
            return 0;
    }
}

void common_hal_usb_core_device_set_configuration(usb_core_device_obj_t *self, mp_int_t configuration) {
    // We assume that the config index is one less than the value.
    uint8_t config_index = configuration - 1;
    // Get the configuration descriptor and cache it. We'll use it later to open
    // endpoints.

    // Get only the config descriptor first.
    tusb_desc_configuration_t desc;
    if (!tuh_descriptor_get_configuration(self->device_address, config_index, &desc, sizeof(desc), _transfer_done_cb, 0) ||
        !_wait_for_callback()) {
        return;
    }

    // Get the config descriptor plus interfaces and endpoints.
    self->configuration_descriptor = m_realloc(self->configuration_descriptor, desc.wTotalLength);
    if (!tuh_descriptor_get_configuration(self->device_address, config_index, self->configuration_descriptor, desc.wTotalLength, _transfer_done_cb, 0) ||
        !_wait_for_callback()) {
        return;
    }
    tuh_configuration_set(self->device_address, configuration, _transfer_done_cb, 0);
    _wait_for_callback();
}

// Raises an exception on failure. Returns the number of bytes transferred (maybe zero) on success.
static size_t _xfer(tuh_xfer_t *xfer, mp_int_t timeout, bool our_buffer, bool raise_on_timeout) {
    _prepare_for_transfer();
    xfer->complete_cb = _transfer_done_cb;
    if (!tuh_edpt_xfer(xfer)) {
        if (our_buffer) {
            port_free(xfer->buffer);
        }
        mp_raise_usb_core_USBError(NULL);
        return 0;
    }
    return _handle_timed_transfer_callback(xfer, timeout, our_buffer, raise_on_timeout);
}

static tusb_desc_endpoint_t const *_find_endpoint_descriptor(usb_core_device_obj_t *self, mp_int_t endpoint) {
    tusb_desc_configuration_t *desc_cfg = (tusb_desc_configuration_t *)self->configuration_descriptor;

    uint32_t total_length = tu_le16toh(desc_cfg->wTotalLength);
    uint8_t const *desc_end = ((uint8_t const *)desc_cfg) + total_length;
    uint8_t const *p_desc = tu_desc_next(desc_cfg);

    // parse each interfaces
    while (p_desc < desc_end) {
        if (TUSB_DESC_ENDPOINT == tu_desc_type(p_desc)) {
            tusb_desc_endpoint_t const *desc_ep = (tusb_desc_endpoint_t const *)p_desc;
            if (desc_ep->bEndpointAddress == endpoint) {
                return desc_ep;
            }
        }

        p_desc = tu_desc_next(p_desc);
    }
    return NULL;
}

static bool _open_endpoint(usb_core_device_obj_t *self, mp_int_t endpoint) {
    bool endpoint_open = false;
    size_t open_size = sizeof(self->open_endpoints);
    size_t first_free = open_size;
    for (size_t i = 0; i < open_size; i++) {
        if (self->open_endpoints[i] == endpoint) {
            endpoint_open = true;
        } else if (first_free == open_size && self->open_endpoints[i] == 0) {
            first_free = i;
        }
    }
    if (endpoint_open) {
        return true;
    }

    if (self->configuration_descriptor == NULL) {
        mp_raise_usb_core_USBError(MP_ERROR_TEXT("No configuration set"));
        return false;
    }

    tusb_desc_endpoint_t const *desc_ep = _find_endpoint_descriptor(self, endpoint);
    if (desc_ep == NULL) {
        return false;
    }

    bool open = tuh_edpt_open(self->device_address, desc_ep);
    if (open) {
        self->open_endpoints[first_free] = endpoint;
    }
    return open;
}

#if IN_BUFFER_ENABLED
// Returns the buffer for a bulk IN endpoint, claiming a slot on first use. Returns
// NULL for other endpoints, or when no slot or memory is free, and read() then
// reads directly as before.
static in_buffer_t *_in_buffer_get(usb_core_device_obj_t *self, mp_int_t endpoint) {
    in_buffer_t *free_buf = NULL;
    for (size_t i = 0; i < IN_BUFFER_SLOTS; i++) {
        in_buffer_t *buf = &_in_buffers[i];
        if (buf->owner != NULL && buf->daddr == self->device_address && buf->ep_addr == endpoint) {
            return buf;
        }
        if (free_buf == NULL && buf->owner == NULL && buf->stale_data == NULL) {
            free_buf = buf;
        }
    }
    if (free_buf == NULL || tu_edpt_dir(endpoint) != TUSB_DIR_IN ||
        (_mounted_devices & (1 << self->device_address)) == 0) {
        return NULL;
    }
    tusb_desc_endpoint_t const *desc_ep = _find_endpoint_descriptor(self, endpoint);
    if (desc_ep == NULL || desc_ep->bmAttributes.xfer != TUSB_XFER_BULK) {
        return NULL;
    }
    uint16_t mps = tu_edpt_packet_size(desc_ep);
    if (mps == 0) {
        return NULL;
    }
    free_buf->data = port_malloc(IN_BUFFER_DEPTH * mps, true);
    if (free_buf->data == NULL) {
        return NULL;
    }
    free_buf->owner = self;
    free_buf->mps = mps;
    free_buf->daddr = self->device_address;
    free_buf->ep_addr = endpoint;
    free_buf->error = XFER_RESULT_SUCCESS;
    return free_buf;
}

// Copies queued packets into buffer until it is full or a short packet ends the
// transfer, like a direct read.
static size_t _in_buffer_read(in_buffer_t *buf, uint8_t *buffer, size_t len, mp_int_t timeout, bool raise_on_timeout) {
    _in_buffer_queue(buf);
    uint32_t start_time = supervisor_ticks_ms32();
    while ((timeout == 0 || supervisor_ticks_ms32() - start_time < (uint32_t)timeout) &&
           !mp_hal_is_interrupted() &&
           buf->count == 0 && buf->busy) {
        RUN_BACKGROUND_TASKS;
    }
    if (mp_hal_is_interrupted()) {
        return 0;
    }
    if (buf->count == 0) {
        xfer_result_t error = buf->error;
        buf->error = XFER_RESULT_SUCCESS;
        if (error == XFER_RESULT_STALLED) {
            mp_raise_usb_core_USBError(MP_ERROR_TEXT("Pipe error"));
        }
        if ((error == XFER_RESULT_SUCCESS && !buf->busy) ||
            (error != XFER_RESULT_SUCCESS && error != XFER_RESULT_TIMEOUT)) {
            mp_raise_usb_core_USBError(NULL);
        }
        if (raise_on_timeout) {
            mp_raise_usb_core_USBTimeoutError();
        }
        return 0;
    }
    size_t total = 0;
    while (buf->count > 0 && total < len) {
        uint16_t packet_len = buf->len[buf->head];
        size_t n = MIN(len - total, (size_t)(packet_len - buf->offset));
        memcpy(buffer + total, buf->data + buf->head * buf->mps + buf->offset, n);
        total += n;
        buf->offset += n;
        if (buf->offset < packet_len) {
            break;
        }
        buf->offset = 0;
        buf->head = (buf->head + 1) % IN_BUFFER_DEPTH;
        buf->count--;
        if (packet_len < buf->mps) {
            break;
        }
    }
    _in_buffer_queue(buf);
    return total;
}
#endif

mp_int_t common_hal_usb_core_device_write(usb_core_device_obj_t *self, mp_int_t endpoint, const uint8_t *buffer, mp_int_t len, mp_int_t timeout) {
    if (!_open_endpoint(self, endpoint)) {
        mp_raise_usb_core_USBError(NULL);
        return 0;
    }

    #if !CIRCUITPY_ALL_MEMORY_DMA_CAPABLE
    // Ensure buffer is in DMA-capable memory
    uint8_t *dma_buffer = _ensure_dma_buffer(self, buffer, len, true);  // true = for write
    if (dma_buffer == NULL) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Could not allocate DMA capable buffer"));
        return 0;
    }
    #else
    uint8_t *dma_buffer = (uint8_t *)buffer;  // All memory is DMA-capable
    #endif

    tuh_xfer_t xfer;
    xfer.daddr = self->device_address;
    xfer.ep_addr = endpoint;
    xfer.buffer = dma_buffer;
    xfer.buflen = len;
    size_t result = _xfer(&xfer, timeout, dma_buffer != buffer, true);
    #if !CIRCUITPY_ALL_MEMORY_DMA_CAPABLE
    if (dma_buffer != buffer) {
        port_free(dma_buffer);
    }
    #endif
    return result;
}

mp_int_t common_hal_usb_core_device_read(usb_core_device_obj_t *self, mp_int_t endpoint, uint8_t *buffer, mp_int_t len, mp_int_t timeout, bool raise_on_timeout) {
    if (!_open_endpoint(self, endpoint)) {
        mp_raise_usb_core_USBError(NULL);
        return 0;
    }

    #if IN_BUFFER_ENABLED
    _in_buffer_reap();
    in_buffer_t *in_buf = _in_buffer_get(self, endpoint);
    if (in_buf != NULL) {
        return _in_buffer_read(in_buf, buffer, len, timeout, raise_on_timeout);
    }
    #endif

    #if !CIRCUITPY_ALL_MEMORY_DMA_CAPABLE
    // Ensure buffer is in DMA-capable memory
    uint8_t *dma_buffer = _ensure_dma_buffer(self, buffer, len, false);  // false = for read
    if (dma_buffer == NULL) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Could not allocate DMA capable buffer"));
        return 0;
    }
    #else
    uint8_t *dma_buffer = buffer;  // All memory is DMA-capable
    #endif

    tuh_xfer_t xfer;
    xfer.daddr = self->device_address;
    xfer.ep_addr = endpoint;
    xfer.buffer = dma_buffer;
    xfer.buflen = len;
    mp_int_t result = _xfer(&xfer, timeout, dma_buffer != buffer, raise_on_timeout);

    #if !CIRCUITPY_ALL_MEMORY_DMA_CAPABLE
    // Copy data back to original buffer if needed
    if (dma_buffer != buffer) {
        memcpy(buffer, dma_buffer, result);
        port_free(dma_buffer);
    }
    #endif

    return result;
}

mp_int_t common_hal_usb_core_device_ctrl_transfer(usb_core_device_obj_t *self,
    mp_int_t bmRequestType, mp_int_t bRequest,
    mp_int_t wValue, mp_int_t wIndex,
    uint8_t *buffer, mp_int_t len, mp_int_t timeout) {
    // Timeout is in ms.

    #if !CIRCUITPY_ALL_MEMORY_DMA_CAPABLE
    // Determine if this is a write (host-to-device) or read (device-to-host) transfer
    bool is_write = (bmRequestType & 0x80) == 0;  // Bit 7: 0=host-to-device, 1=device-to-host

    // Ensure buffer is in DMA-capable memory
    uint8_t *dma_buffer = NULL;
    if (len > 0 && buffer != NULL) {
        dma_buffer = _ensure_dma_buffer(self, buffer, len, is_write);
        if (dma_buffer == NULL) {
            mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Could not allocate DMA capable buffer"));
            return 0;
        }
    }
    #else
    uint8_t *dma_buffer = buffer;  // All memory is DMA-capable
    #endif

    tusb_control_request_t request = {
        .bmRequestType = bmRequestType,
        .bRequest = bRequest,
        .wValue = wValue,
        .wIndex = wIndex,
        .wLength = len
    };
    tuh_xfer_t xfer = {
        .daddr = self->device_address,
        .ep_addr = 0,
        .setup = &request,
        .buffer = dma_buffer,
        .complete_cb = _transfer_done_cb,
    };

    _prepare_for_transfer();
    if (!tuh_control_xfer(&xfer)) {
        mp_raise_usb_core_USBError(NULL);
        return 0;
    }
    mp_int_t result = (mp_int_t)_handle_timed_transfer_callback(&xfer, timeout, dma_buffer != buffer, true);

    #if !CIRCUITPY_ALL_MEMORY_DMA_CAPABLE
    if (dma_buffer != buffer) {
        // Copy data back to original buffer if this was a read transfer
        if (buffer != NULL && !is_write) {
            memcpy(buffer, dma_buffer, result);
        }
        port_free(dma_buffer);
    }
    #endif

    return result;
}

bool common_hal_usb_core_device_is_kernel_driver_active(usb_core_device_obj_t *self, mp_int_t interface) {
    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    if (usb_keyboard_in_use(self->device_address, interface)) {
        return true;
    }
    #endif
    return false;
}

void common_hal_usb_core_device_detach_kernel_driver(usb_core_device_obj_t *self, mp_int_t interface) {
    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    usb_keyboard_detach(self->device_address, interface);
    #endif
}

void common_hal_usb_core_device_attach_kernel_driver(usb_core_device_obj_t *self, mp_int_t interface) {
    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    usb_keyboard_attach(self->device_address, interface);
    #endif
}
