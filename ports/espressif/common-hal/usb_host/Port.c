// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/espidf/__init__.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/usb_host/Port.h"
#include "supervisor/port.h"
#include "supervisor/port_heap.h"
#include "supervisor/usb.h"
#include "supervisor/shared/tick.h"
#include "esp_private/usb_phy.h"
#include "tusb.h"

#if defined(CONFIG_IDF_TARGET_ESP32P4)
#if CIRCUITPY_USB_HOST_INSTANCE != 1
#error "USB host requires the ESP32-P4 high-speed controller"
#endif
#elif defined(CONFIG_IDF_TARGET_ESP32S31)
// The S31 has only the high-speed controller, as TinyUSB rhport 0.
#if CIRCUITPY_USB_HOST_INSTANCE != 0
#error "USB host requires the ESP32-S31 high-speed controller"
#endif
#else
#error "USB host is not supported on this chip"
#endif
#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_DEVICE_INSTANCE == CIRCUITPY_USB_HOST_INSTANCE
#error "USB host and device must use different controllers"
#endif

static usb_host_port_obj_t usb_host_instance;
static usb_phy_handle_t host_phy;

#if CIRCUITPY_USB_HOST_ISO_RECORDS
// The ring that TinyUSB's DWC2 host driver keeps isochronous IN packets in
// between transfers, and the buffer each packet is received into first.
static uint8_t *_iso_ring = NULL;
static uint8_t *_iso_packet = NULL;
// The largest isochronous packet of a full or high speed endpoint, without
// high-bandwidth transactions.
#define ISO_PACKET_SIZE (1024)
// The largest ring.
#define ISO_RING_MAX (1024 * 1024)
// Once its channel is told to stop, the stream's last packet can still be
// received into the packet buffer until the end of the frame.
#define ISO_STOP_SETTLE_US (2000)
#endif

usb_host_port_obj_t *common_hal_usb_host_port_construct(const mcu_pin_obj_t *dp, const mcu_pin_obj_t *dm) {
    // The high-speed PHY has dedicated pads, not GPIOs.
    if (dp != &pin_USB_HS_DP || dm != &pin_USB_HS_DM) {
        raise_ValueError_invalid_pins();
    }
    usb_host_port_obj_t *self = &usb_host_instance;
    if (self->dp != NULL) {
        return self;
    }

    const usb_phy_config_t config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = USB_PHY_TARGET_UTMI,
        .otg_mode = USB_OTG_MODE_HOST,
        .otg_speed = USB_PHY_SPEED_HIGH,
    };
    CHECK_ESP_RESULT(usb_new_phy(&config, &host_phy));
    if (!tuh_init(TUH_OPT_RHPORT)) {
        usb_del_phy(host_phy);
        host_phy = NULL;
        CHECK_ESP_RESULT(ESP_FAIL);
    }
    self->base.type = &usb_host_port_type;
    self->dp = dp;
    self->dm = dm;
    // Keep polling enumeration timers even without an active display.
    supervisor_enable_tick();
    return self;
}

// TinyUSB installs the controller IRQ. Process its events on the VM task,
// where the Python USB device and keyboard callbacks are safe to run.
void tuh_event_hook_cb(uint8_t rhport, uint32_t eventid, bool in_isr) {
    // Don't wait for the background tick: an isochronous stream must queue its
    // next transfer within the frame.
    usb_background_schedule();
}

bool common_hal_usb_host_port_claim_iso_buffer(size_t size) {
    #if CIRCUITPY_USB_HOST_ISO_RECORDS
    // There is one ring, for one stream at a time. Ending an older stream
    // here would take its packets from another user, so refuse instead.
    if (_iso_ring != NULL) {
        return false;
    }
    uint32_t rounded = 2048;
    while (rounded < size && rounded < ISO_RING_MAX) {
        rounded <<= 1;
    }
    // Only the CPU touches the ring, in the interrupt handler, so it can be in
    // PSRAM. The controller writes the packet buffer by DMA.
    uint8_t *ring = port_malloc(rounded, false);
    uint8_t *packet = port_malloc(ISO_PACKET_SIZE, true);
    if (ring == NULL || packet == NULL) {
        port_free(ring);
        port_free(packet);
        m_malloc_fail(rounded + ISO_PACKET_SIZE);
    }
    const tuh_configure_dwc2_iso_ring_t config = {
        .buffer = ring,
        .size = rounded,
        .packet_buffer = packet,
        .packet_size = ISO_PACKET_SIZE,
    };
    if (!tuh_configure(TUH_OPT_RHPORT, TUH_CFGID_DWC2_ISO_RING, &config)) {
        port_free(ring);
        port_free(packet);
        return false;
    }
    _iso_ring = ring;
    _iso_packet = packet;
    return true;
    #else
    (void)size;
    return false;
    #endif
}

void common_hal_usb_host_port_release_iso_buffer(void) {
    #if CIRCUITPY_USB_HOST_ISO_RECORDS
    if (_iso_ring == NULL) {
        return;
    }
    // This also ends a stream into the ring that is still running.
    const tuh_configure_dwc2_iso_ring_t config = { 0 };
    tuh_configure(TUH_OPT_RHPORT, TUH_CFGID_DWC2_ISO_RING, &config);
    common_hal_mcu_delay_us(ISO_STOP_SETTLE_US);
    port_free(_iso_ring);
    port_free(_iso_packet);
    _iso_ring = NULL;
    _iso_packet = NULL;
    #endif
}
