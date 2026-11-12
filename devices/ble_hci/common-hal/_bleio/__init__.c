// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "supervisor/shared/bluetooth/bluetooth.h"

// UUID shared by all cccd's.
bleio_uuid_obj_t cccd_uuid;

// void check_sec_status(uint8_t sec_status) {
//     if (sec_status == BLE_GAP_SEC_STATUS_SUCCESS) {
//         return;
//     }

//     switch (sec_status) {
//         case BLE_GAP_SEC_STATUS_UNSPECIFIED:
//             mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Unspecified issue. The pairing request on the other device may have been declined or ignored."));
//             return;
//         default:
//             mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Unknown security error: 0x%04x"), sec_status);
//     }
// }

void common_hal_bleio_init(void) {
    // Called on every import of _bleio. This is the only initialization HCI gets:
    // unlike other ports, nothing runs at board startup, because HCI is unusable
    // until user code supplies an adapter (UART, etc.) anyway.
    // Create a UUID object for all CCCD's.
    cccd_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&cccd_uuid, BLE_UUID_CCCD, NULL);
    // Keep an enabled adapter's state: every module that uses _bleio imports it again.
    // bleio_reset() resets at the end of each run.
    if (!common_hal_bleio_adapter_obj.enabled) {
        bleio_hci_reset();
    }
}

void bleio_user_reset(void) {
    // HCI doesn't support the BLE workflow so just do a full reset.
    bleio_reset();
}

// Turn off BLE on a reset or reload.
void bleio_reset(void) {
    bleio_hci_reset();

    bleio_adapter_obj_t *adapter = &common_hal_bleio_adapter_obj;
    if (!adapter->allocated) {
        return;
    }
    // The adapter belongs to the program and its UART is on the VM heap: always free it.
    if (common_hal_bleio_adapter_get_enabled(adapter)) {
        bleio_adapter_reset(adapter);
        common_hal_bleio_adapter_set_enabled(adapter, false);
    }
    adapter->allocated = false;

    bleio_set_adapter(mp_const_none);

    // FIX bonding_reset();
    supervisor_start_bluetooth();
}

// The singleton _bleio.Adapter object, bound to _bleio.adapter
bleio_adapter_obj_t common_hal_bleio_adapter_obj = {
    .base = {
        .type = &bleio_adapter_type,
    },
};

bleio_adapter_obj_t *common_hal_bleio_allocate_adapter_or_raise(void) {
    if (common_hal_bleio_adapter_obj.allocated) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Too many Adapters"));
    }
    return &common_hal_bleio_adapter_obj;
}

void bleio_check_connected(uint16_t conn_handle) {
    if (conn_handle == BLE_CONN_HANDLE_INVALID) {
        mp_raise_ConnectionError(MP_ERROR_TEXT("Not connected"));
    }
}

void common_hal_bleio_gc_collect(void) {
    bleio_adapter_gc_collect(&common_hal_bleio_adapter_obj);
}


void bleio_hci_background(void) {
    bleio_adapter_background(&common_hal_bleio_adapter_obj);
}
