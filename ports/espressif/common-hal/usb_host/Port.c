// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/espidf/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "py/runtime.h"
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
#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
// The S2 and S3 have one full-speed OTG controller (TinyUSB rhport 0) on the
// internal PHY, on GPIO19 (D-) and GPIO20 (D+). USB device uses the same
// controller, so with both built in the role is picked at startup
// (CIRCUITPY_USB_ROLE_SWITCH, see supervisor/usb.h).
#if CIRCUITPY_USB_HOST_INSTANCE != 0
#error "USB host requires the full-speed controller (CIRCUITPY_USB_HOST_INSTANCE 0)"
#endif
#if CIRCUITPY_ESP_USB_SERIAL_JTAG
#error "USB host cannot share the USB PHY with USB-Serial/JTAG"
#endif
#else
#error "USB host is not supported on this chip"
#endif
#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_DEVICE_INSTANCE == CIRCUITPY_USB_HOST_INSTANCE && !CIRCUITPY_USB_ROLE_SWITCH
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

#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#define USB_HOST_DP (&pin_GPIO20)
#define USB_HOST_DM (&pin_GPIO19)
#define USB_HOST_PHY_TARGET USB_PHY_TARGET_INT
#define USB_HOST_PHY_SPEED USB_PHY_SPEED_FULL
#else
// The high-speed PHY has dedicated pads, not GPIOs.
#define USB_HOST_DP (&pin_USB_HS_DP)
#define USB_HOST_DM (&pin_USB_HS_DM)
#define USB_HOST_PHY_TARGET USB_PHY_TARGET_UTMI
#define USB_HOST_PHY_SPEED USB_PHY_SPEED_HIGH
#endif

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
    if (dp != USB_HOST_DP || dm != USB_HOST_DM) {
        raise_ValueError_invalid_pins();
    }
    #if CIRCUITPY_USB_ROLE_SWITCH
    // The one controller is either a device or a host. After boot.py the device
    // stack runs (CIRCUITPY drive, REPL), so only boot.py can make it a host.
    if (!usb_host_mode() && tud_inited()) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("USB is a device. Create usb_host.Port in boot.py to make it a host."));
    }
    #endif
    usb_host_port_obj_t *self = &usb_host_instance;
    // On the S2/S3 the pads are real GPIOs, but Pin.c never resets them and marks
    // them in use at every reset (as for USB device), so Python cannot take them.
    if (self->dp != NULL) {
        return self;
    }

    // usb_new_phy() in host mode also connects the D+/D- pull-downs and
    // forces the A-side / VBUS-valid signals, so the controller runs as host.
    const usb_phy_config_t config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = USB_HOST_PHY_TARGET,
        .otg_mode = USB_OTG_MODE_HOST,
        .otg_speed = USB_HOST_PHY_SPEED,
    };
    CHECK_ESP_RESULT(usb_new_phy(&config, &host_phy));
    if (!tuh_init(TUH_OPT_RHPORT)) {
        usb_del_phy(host_phy);
        host_phy = NULL;
        CHECK_ESP_RESULT(ESP_FAIL);
    }
    #if CIRCUITPY_USB_ROLE_SWITCH
    // From now on usb_init() leaves the controller to the host.
    usb_set_host_mode();
    #endif
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
