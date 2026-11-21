// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/espidf/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/usb_host/Port.h"
#include "supervisor/port.h"
#include "supervisor/usb.h"
#include "esp_private/usb_phy.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
#if !CIRCUITPY_USB_HOST_TASK
#error "The espressif USB host runs TinyUSB in a task of its own"
#endif

// TinyUSB's host stack runs in this task, which waits on TinyUSB's event queue
// and so runs as soon as the controller interrupt posts an event. The VM
// doesn't have to get to its background tasks first: an isochronous stream
// queues its next transfer within the frame while the VM writes to an SD card
// or collects garbage.
//
// It runs above the VM's priority, like the MAX3421E's host task, so that a
// busy VM doesn't hold it up. Like the other USB tasks it is pinned to the
// same core as CircuitPython, which leaves the other core for networking.
#define USB_HOST_TASK_PRIORITY (5)
// Bytes. Enumeration and the class and Python module callbacks run on it.
#define USB_HOST_TASK_STACK_SIZE (4096)

static usb_host_port_obj_t usb_host_instance;
static usb_phy_handle_t host_phy;

static StackType_t _host_task_stack[USB_HOST_TASK_STACK_SIZE];
static StaticTask_t _host_task_tcb;
static TaskHandle_t _host_task = NULL;

static void _usb_host_task(void *param) {
    (void)param;
    while (true) {
        // Returns once the events that came in are handled. It waits only as
        // long as no TinyUSB timer (enumeration delays, control transfer
        // timeouts) is due.
        tuh_task_ext(UINT32_MAX, false);
        // Python code may be waiting for what the events completed.
        port_wake_main_task();
    }
}

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
    // The port is never deinited, so neither is the task. It is the only
    // caller of tuh_task_ext(), which is why it only starts after tuh_init().
    // This runs on the VM, so xPortGetCoreID() is CircuitPython's core.
    if (_host_task == NULL) {
        _host_task = xTaskCreateStaticPinnedToCore(_usb_host_task, "usbh",
            USB_HOST_TASK_STACK_SIZE, NULL, USB_HOST_TASK_PRIORITY,
            _host_task_stack, &_host_task_tcb, xPortGetCoreID());
    }
    self->base.type = &usb_host_port_type;
    self->dp = dp;
    self->dm = dm;
    return self;
}
