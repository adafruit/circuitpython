// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#if defined(CONFIG_IDF_TARGET_ESP32C3)
#include "components/esp_rom/include/esp32c3/rom/gpio.h"
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#include "components/esp_rom/include/esp32c6/rom/gpio.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#include "components/esp_rom/include/esp32s2/rom/gpio.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "components/esp_rom/include/esp32s3/rom/gpio.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tusb.h"

#if CIRCUITPY_USB_DEVICE
#ifdef CFG_TUSB_DEBUG
  #define USBD_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE)
#else
  #define USBD_STACK_SIZE     (3 * configMINIMAL_STACK_SIZE / 2)
#endif

StackType_t usb_device_stack[USBD_STACK_SIZE];
StaticTask_t usb_device_taskdef;

static usb_phy_handle_t phy_hdl;

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
        .target = USB_PHY_TARGET_INT,

        .otg_mode = USB_OTG_MODE_DEVICE,
        // https://github.com/hathach/tinyusb/issues/2943#issuecomment-2601888322
        // Set speed to undefined (auto-detect) to avoid timing/race issue with S3 with host such as macOS
        .otg_speed = USB_PHY_SPEED_UNDEFINED,
    };
    usb_new_phy(&phy_conf, &phy_hdl);

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
