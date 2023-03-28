/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "supervisor/shared/bluetooth/bluetooth.h"
#include "common-hal/_bleio/__init__.h"
#include "Adapter.h"
#include "sl_status.h"

bleio_adapter_obj_t common_hal_bleio_adapter_obj;

void bleio_user_reset() {
    // Stop any user scanning or advertising.
    common_hal_bleio_adapter_stop_scan(&common_hal_bleio_adapter_obj);
    common_hal_bleio_adapter_stop_advertising(&common_hal_bleio_adapter_obj);

    // Maybe start advertising the BLE workflow.
    supervisor_bluetooth_background();
}

void bleio_reset() {
    reset_dynamic_service();
    reset_packet_buffer_list();
    reset_characteristic_buffer_list();
    bleio_adapter_reset(&common_hal_bleio_adapter_obj);
    // Set this explicitly to save data.
    common_hal_bleio_adapter_obj.base.type = &bleio_adapter_type;
    if (!common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        return;
    }

    supervisor_stop_bluetooth();
    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, false);
    supervisor_start_bluetooth();
}

void bleio_background(void) {

}

void common_hal_bleio_gc_collect(void) {
    bleio_adapter_gc_collect(&common_hal_bleio_adapter_obj);
}

void check_ble_error(int error_code) {
    if (error_code == SL_STATUS_OK) {
        return;
    }
    switch (error_code) {
        case SL_STATUS_TIMEOUT:
            mp_raise_msg(&mp_type_TimeoutError, NULL);
            return;
        default:
            mp_raise_bleio_BluetoothError(
                translate("Unknown BLE error: %d"), error_code);
            break;
    }
}

void common_hal_bleio_check_connected(uint16_t conn_handle) {
    if (conn_handle == BLEIO_HANDLE_INVALID) {
        mp_raise_ConnectionError(translate("Not connected"));
    }
}
