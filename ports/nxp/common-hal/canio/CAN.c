/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"

#include "common-hal/canio/CAN.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"

#if (0)
#include "genhdr/candata.h"
#endif


extern ARM_DRIVER_CAN Driver_CAN1;


STATIC void __SignalUnitEvent_cb(uint32_t event) {
    return;
}

STATIC void __SignalObjectEvent_cb(uint32_t obj_idx, uint32_t event) {
    return;
}

void common_hal_canio_reset(void) {
    return;
}

void common_hal_canio_can_construct(canio_can_obj_t *self, mcu_pin_obj_t *tx, mcu_pin_obj_t *rx, int baudrate, bool loopback, bool silent) {
    ARM_DRIVER_CAN *can_drv = &Driver_CAN1;

    can_drv->Initialize(__SignalUnitEvent_cb, __SignalObjectEvent_cb);

    can_drv->PowerControl(ARM_POWER_FULL);

    can_drv->SetMode(ARM_CAN_MODE_INITIALIZATION);

    uint32_t bitrate = 125000UL;
    can_drv->SetBitrate(ARM_CAN_BITRATE_NOMINAL, bitrate,
        ARM_CAN_BIT_PROP_SEG(4)
        | ARM_CAN_BIT_PHASE_SEG1(7U)
        | ARM_CAN_BIT_PHASE_SEG2(4U)
        | ARM_CAN_BIT_SJW(4U));

    ARM_CAN_CAPABILITIES can_dev_info = can_drv->GetCapabilities();
    const size_t num_of_channels = can_dev_info.num_objects;

    ARM_CAN_OBJ_CAPABILITIES chan_info;
    uint32_t tx_chan_id = UINT32_MAX;
    uint32_t rx_chan_id = UINT32_MAX;

    for (size_t n = 0U; n < num_of_channels; ++n) {
        chan_info = can_drv->ObjectGetCapabilities(n);

        if ((UINT32_MAX == rx_chan_id) && (chan_info.tx)) {
            tx_chan_id = n;
        }

        if ((UINT32_MAX == rx_chan_id) && (chan_info.rx)) {
            rx_chan_id = n;
        }
    }

    can_drv->ObjectConfigure(tx_chan_id, ARM_CAN_OBJ_TX);
    can_drv->ObjectConfigure(rx_chan_id, ARM_CAN_OBJ_RX);

    if (loopback) {
        if (silent) {
            can_drv->SetMode(ARM_CAN_MODE_LOOPBACK_INTERNAL);
        } else {
            can_drv->SetMode(ARM_CAN_MODE_LOOPBACK_EXTERNAL);
        }
    } else {
        can_drv->SetMode(ARM_CAN_MODE_NORMAL);
    }

    return;
}

void common_hal_canio_can_deinit(canio_can_obj_t *self) {
    return;
}

bool common_hal_canio_can_deinited(canio_can_obj_t *self) {
    return false;
}

void common_hal_canio_can_check_for_deinit(canio_can_obj_t *self) {
    return;
}

bool common_hal_canio_can_loopback_get(canio_can_obj_t *self) {
    return false;
}

int common_hal_canio_can_baudrate_get(canio_can_obj_t *self) {
    return 0;
}

int common_hal_canio_can_transmit_error_count_get(canio_can_obj_t *self) {
    return 0;
}

int common_hal_canio_can_receive_error_count_get(canio_can_obj_t *self) {
    return 0;
}

canio_bus_state_t common_hal_canio_can_state_get(canio_can_obj_t *self) {
    return BUS_STATE_OFF;
}

void common_hal_canio_can_restart(canio_can_obj_t *self) {
    return;
}

bool common_hal_canio_can_auto_restart_get(canio_can_obj_t *self) {
    return false;
}

void common_hal_canio_can_auto_restart_set(canio_can_obj_t *self, bool value) {
    return;
}

void common_hal_canio_can_send(canio_can_obj_t *self, mp_obj_t message_in) {
    return;
}

bool common_hal_canio_can_silent_get(canio_can_obj_t *self) {
    return false;
}
