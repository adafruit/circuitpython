// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/espidf/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/usb_host/Port.h"
#include "supervisor/usb.h"
#include "supervisor/shared/tick.h"
#include "esp_private/usb_phy.h"
#include "tusb.h"

#if !defined(CONFIG_IDF_TARGET_ESP32P4) || CIRCUITPY_USB_HOST_INSTANCE != 1
#error "USB host requires the ESP32-P4 high-speed controller"
#endif
#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_DEVICE_INSTANCE == CIRCUITPY_USB_HOST_INSTANCE
#error "USB host and device must use different controllers"
#endif

static usb_host_port_obj_t usb_host_instance;
static usb_phy_handle_t host_phy;

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
    usb_background_schedule();
}
