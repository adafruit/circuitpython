// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
// SPDX-FileContributor: 2025 Nicolai Electronics
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "supervisor/usb.h"
#include "supervisor/port.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/readline/readline.h"

#include "hal/gpio_ll.h"

#include "esp_err.h"
#include "esp_private/usb_phy.h"
#include "soc/usb_periph.h"

#include "driver/gpio.h"
#include "esp_private/periph_ctrl.h"

#include "rom/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tusb.h"

#ifdef CONFIG_IDF_TARGET_ESP32P4
#include "hal/usb_serial_jtag_ll.h"
#endif

#if CIRCUITPY_USB_DEVICE
#ifdef CFG_TUSB_DEBUG
  #define USBD_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE)
#else
  #define USBD_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE / 2)
#endif

StackType_t usb_device_stack[USBD_STACK_SIZE];
StaticTask_t usb_device_taskdef;

static usb_phy_handle_t device_phy_hdl;

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
static void usb_device_task(void *param) {
    (void)param;

    // RTOS forever loop
    while (1) {
        // tinyusb device task
        if (tusb_inited()) {
            tud_task();
            tud_cdc_write_flush();
        }
        vTaskDelay(1);
    }
}
#endif // CIRCUITPY_USB_DEVICE

void init_usb_hardware(void) {
    #if CIRCUITPY_USB_DEVICE
    // Configure USB PHY
    usb_phy_config_t phy_conf = {
        .controller = USB_PHY_CTRL_OTG,
        #if defined(CONFIG_IDF_TARGET_ESP32P4) && CIRCUITPY_USB_DEVICE_INSTANCE == 1
        .target = USB_PHY_TARGET_UTMI,
        #else
        .target = USB_PHY_TARGET_INT,
        #endif
        .otg_mode = USB_OTG_MODE_DEVICE,
        #if defined(CONFIG_IDF_TARGET_ESP32P4) && CIRCUITPY_USB_DEVICE_INSTANCE == 0
        .otg_speed = USB_PHY_SPEED_FULL,
        #else
        // https://github.com/hathach/tinyusb/issues/2943#issuecomment-2601888322
        // Set speed to undefined (auto-detect) to avoid timing/race issue with S3 with host such as macOS
        .otg_speed = USB_PHY_SPEED_UNDEFINED,
        #endif
    };
    usb_new_phy(&phy_conf, &device_phy_hdl);

    #if CIRCUITPY_ESP32P4_SWAP_LSFS == 1
    #ifndef CONFIG_IDF_TARGET_ESP32P4
    #error "LSFS swap is only supported on ESP32P4"
    #endif
    // Switch the USB PHY
    const usb_serial_jtag_pull_override_vals_t override_disable_usb = {
        .dm_pd = true, .dm_pu = false, .dp_pd = true, .dp_pu = false
    };
    const usb_serial_jtag_pull_override_vals_t override_enable_usb = {
        .dm_pd = false, .dm_pu = false, .dp_pd = false, .dp_pu = true
    };

    // Drop off the bus by removing the pull-up on USB DP
    usb_serial_jtag_ll_phy_enable_pull_override(&override_disable_usb);

    // Select USB mode by swapping and un-swapping the two PHYs
    vTaskDelay(pdMS_TO_TICKS(500));  // Wait for disconnect before switching to device
    usb_serial_jtag_ll_phy_select(1);

    // Put the device back onto the bus by re-enabling the pull-up on USB DP
    usb_serial_jtag_ll_phy_enable_pull_override(&override_enable_usb);
    usb_serial_jtag_ll_phy_disable_pull_override();
    #endif

    // Pin the USB task to the same core as CircuitPython. This way we leave
    // the other core for networking.
    (void)xTaskCreateStaticPinnedToCore(usb_device_task,
        "usbd",
        USBD_STACK_SIZE,
        NULL,
        5,
        usb_device_stack,
        &usb_device_taskdef,
        xPortGetCoreID());
    #endif
}
