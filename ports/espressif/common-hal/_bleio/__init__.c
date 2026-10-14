// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/shared/bluetooth/bluetooth.h"

#include "common-hal/_bleio/__init__.h"
#include "common-hal/_bleio/ble_events.h"

#include "services/gatt/ble_svc_gatt.h"

#include "nvs_flash.h"

// State for the current GATT client read or write. The NimBLE callbacks run on the
// nimble_host task and record the result here, under _completion_mutex.
//
// We stop waiting after 2 seconds, but NimBLE waits 30 seconds for a response, so a
// callback can arrive after its caller has returned. Each request gets a new
// sequence number, passed as the callback argument, and callbacks for older requests
// are ignored. Read data is staged in _read_buf rather than written to the caller's
// buffer, which may no longer exist.
//
// After a timeout, NimBLE still has the request outstanding on that connection. It
// holds later requests on the connection until a response arrives or it disconnects
// at 30 seconds, so those requests time out too. The stale requests also occupy
// NimBLE's GATT procedure pool (CONFIG_BT_NIMBLE_GATT_MAX_PROCS), so new ones can
// fail with BLE_HS_ENOMEM until then.
static portMUX_TYPE _completion_mutex = portMUX_INITIALIZER_UNLOCKED;
static uint32_t _completion_seq;
// A status of 0 means success, so it can't also mean "still waiting".
static volatile int _completion_status;
static volatile bool _completion_done;
static uint8_t _read_buf[BLE_ATT_ATTR_MAX_LEN];
static uint16_t _read_len;

background_callback_t bleio_background_callback;

void bleio_user_reset(void) {
    if (!common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        return;
    }
    // Stop any user scanning or advertising.
    common_hal_bleio_adapter_stop_scan(&common_hal_bleio_adapter_obj);
    common_hal_bleio_adapter_stop_advertising(&common_hal_bleio_adapter_obj);

    // Disconnect the connections that user code initiated or accepted with its own
    // advertising. Keep the BLE workflow connection if present.
    //
    // Remove each connection's pointers into the VM heap before disconnecting:
    // the heap is about to go away, and a disconnect completes asynchronously.
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        connection->connection_obj = mp_const_none;
        connection->remote_service_list = NULL;
        if (connection->conn_handle != BLEIO_HANDLE_INVALID && connection->user_owned) {
            common_hal_bleio_connection_disconnect(connection);
        }
    }

    // Now clear the adapter's remaining heap pointer, since it will be stale
    // when the VM stops.
    common_hal_bleio_adapter_obj.connection_objs = NULL;

    // Also clean up event handlers that are on the heap.
    ble_event_remove_heap_handlers();
    // And stop retaining heap services, whose characteristics' handler entries
    // the call above just removed.
    bleio_service_forget_retained();

    // Maybe start advertising the BLE workflow.
    supervisor_bluetooth_background();
}

// Turn off BLE on a reset or reload.
void bleio_reset(void) {
    // Set this explicitly to save data.
    if (!common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        return;
    }

    // The stop/start cycle below clears user-created services from the GATT
    // table, and drops every connection, BLE workflow included. So run it
    // only when user code created services. All other user BLE state has already
    // been torn down individually by bleio_user_reset().
    //
    // TODO: ESP-IDF NimBLE can delete individual services (ble_gatts_delete_svc(),
    // used in Service.c), so bleio_user_reset() could delete user services one by
    // one and never restart the BLE stack at all. For now this port matches
    // nordic, whose SoftDevice can only clear services with a full cycle.
    if (!bleio_get_user_services_created()) {
        return;
    }
    bleio_clear_user_services_created();

    supervisor_stop_bluetooth();
    ble_event_reset();
    bleio_adapter_reset(&common_hal_bleio_adapter_obj);
    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, false);
    supervisor_start_bluetooth();

    // The stop/start above rebuilt the GATT table, so now any bonded peers' cached
    // tables are stale. Signal Service Changed over the whole handle range.
    // Without Service Changed, a bonded host trusts its cache indefinitely and can
    // look up characteristics at stale handles.
    //
    // supervisor_start_bluetooth() restarts the stack only when the BLE workflow
    // is enabled. If it did not, NimBLE is stopped and its GATT server state is
    // gone, so we can't signal anything, and if we tried, we'd cause a crash.
    if (common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        ble_svc_gatt_changed(0x0001, 0xffff);
    }
}

// The singleton _bleio.Adapter object, bound to _bleio.adapter
// It currently only has properties and no state. Inited by bleio_reset
bleio_adapter_obj_t common_hal_bleio_adapter_obj;

void bleio_background(void *data) {
    (void)data;
    supervisor_bluetooth_background();
}

void common_hal_bleio_init(void) {
    common_hal_bleio_adapter_obj.base.type = &bleio_adapter_type;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);


    bleio_background_callback.fun = bleio_background;
    bleio_background_callback.data = NULL;
}

void common_hal_bleio_gc_collect(void) {
    bleio_adapter_gc_collect(&common_hal_bleio_adapter_obj);
    bleio_service_gc_collect();
}

void check_nimble_error(int rc, const char *file, size_t line) {
    if (rc == NIMBLE_OK) {
        return;
    }
    switch (rc) {
        case BLE_HS_ENOMEM:
            mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Nimble out of memory"));
            return;
        case BLE_HS_ETIMEOUT:
            mp_raise_msg(&mp_type_TimeoutError, NULL);
            return;
        case BLE_HS_EINVAL:
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid BLE parameter"));
            return;
        case BLE_HS_ENOTCONN:
            mp_raise_ConnectionError(MP_ERROR_TEXT("Not connected"));
            return;
        case BLE_HS_EALREADY:
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Already in progress"));
            return;
        default:
            #if CIRCUITPY_VERBOSE_BLE || CIRCUITPY_DEBUG
            if (file) {
                mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Unknown BLE error at %s:%d: 0x%04x"), file, line, rc);
            }
            #else
            (void)file;
            (void)line;
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Unknown BLE error: 0x%04x"), rc);
            #endif

            break;
    }
}

void check_ble_error(int error_code, const char *file, size_t line) {
    // 0 means success. For BLE_HS_* codes, there is no defined "SUCCESS" value.
    if (error_code == 0) {
        return;
    }
    switch (error_code) {
        case BLE_HS_ATT_ERR(BLE_ATT_ERR_READ_NOT_PERMITTED):
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("%q not permitted"), MP_QSTR_read);
            return;
        case BLE_HS_ATT_ERR(BLE_ATT_ERR_WRITE_NOT_PERMITTED):
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("%q not permitted"), MP_QSTR_write);
            return;
        case BLE_HS_ATT_ERR(BLE_ATT_ERR_INSUFFICIENT_AUTHEN):
            mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Insufficient authentication"));
            return;
        case BLE_HS_ATT_ERR(BLE_ATT_ERR_INSUFFICIENT_AUTHOR):
            mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Insufficient authorization"));
            return;
        case BLE_HS_ATT_ERR(BLE_ATT_ERR_INSUFFICIENT_ENC):
        case BLE_HS_ATT_ERR(BLE_ATT_ERR_INSUFFICIENT_KEY_SZ):
            mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Insufficient encryption"));
            return;
        case BLE_HS_ATT_ERR(BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN):
            mp_arg_error_invalid(MP_QSTR_length);
            return;
        default:
            if (error_code > BLE_HS_ERR_ATT_BASE && error_code < BLE_HS_ERR_HCI_BASE) {
                // Some other ATT error from the peer.
                mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Unknown gatt error: 0x%04x"), error_code);
            }
            #if CIRCUITPY_VERBOSE_BLE || CIRCUITPY_DEBUG
            if (file) {
                mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Unknown BLE error at %s:%d: 0x%04x"), file, line, error_code);
            }
            #else
            (void)file;
            (void)line;
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Unknown BLE error: 0x%04x"), error_code);
            #endif

            break;
    }
}

void check_notify(BaseType_t result) {
    if (result == pdTRUE) {
        return;
    }
    mp_raise_msg(&mp_type_TimeoutError, NULL);
}

void bleio_check_connected(uint16_t conn_handle) {
    if (conn_handle == BLEIO_HANDLE_INVALID) {
        mp_raise_ConnectionError(MP_ERROR_TEXT("Not connected"));
    }
}

// Start a new request, which makes callbacks for any earlier request stale.
// Returns the sequence number to pass as the NimBLE callback argument.
static void *_start_request(void) {
    portENTER_CRITICAL(&_completion_mutex);
    _completion_seq++;
    _completion_status = 0;
    _completion_done = false;
    _read_len = 0;
    const uint32_t seq = _completion_seq;
    portEXIT_CRITICAL(&_completion_mutex);
    return (void *)(uintptr_t)seq;
}

// Record a callback's status if it is for the current request. Returns true if so.
// Call with _completion_mutex held.
static bool _complete_request(void *arg, int status) {
    if ((uint32_t)(uintptr_t)arg != _completion_seq) {
        return false;
    }
    _completion_status = status;
    _completion_done = true;
    return true;
}

// Wait for a callback to record a completion status, and return that status.
// Return BLE_HS_ETIMEOUT if no callback arrives in time. If interrupted, return 0
// with _completion_done still false, so the pending KeyboardInterrupt is raised
// instead of an error.
static int _wait_for_completion(uint32_t timeout_msecs) {
    const uint64_t timeout_time_ms = common_hal_time_monotonic_ms() + timeout_msecs;
    while (!_completion_done) {
        if (mp_hal_is_interrupted()) {
            return 0;
        }
        if (common_hal_time_monotonic_ms() >= timeout_time_ms) {
            return BLE_HS_ETIMEOUT;
        }
        RUN_BACKGROUND_TASKS;
    }
    return _completion_status;
}

// Raise an exception for a nonzero completion status. Statuses from
// BLE_HS_ERR_ATT_BASE up are ATT errors from the peer, such as insufficient
// encryption, which check_ble_error() maps to SecurityError.
static void _check_completion_status(int status) {
    if (status < BLE_HS_ERR_ATT_BASE) {
        CHECK_NIMBLE_ERROR(status);
        return;
    }
    CHECK_BLE_ERROR(status);
}

static int _read_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    struct ble_gatt_attr *attr,
    void *arg) {
    #if CIRCUITPY_VERBOSE_BLE
    // For debugging.
    mp_printf(&mp_plat_print, "Read status: %d\n", error->status);
    #endif

    portENTER_CRITICAL(&_completion_mutex);
    // A single read gets exactly one callback, so the staged data can't be
    // overwritten once the request is complete.
    if (_complete_request(arg, error->status) && error->status == 0) {
        _read_len = MIN(sizeof(_read_buf), OS_MBUF_PKTLEN(attr->om));
        os_mbuf_copydata(attr->om, attr->offset, _read_len, _read_buf);
    }
    portEXIT_CRITICAL(&_completion_mutex);

    return 0;
}

int bleio_gattc_read(uint16_t conn_handle, uint16_t value_handle, uint8_t *buf, size_t len) {
    void *seq = _start_request();
    CHECK_NIMBLE_ERROR(ble_gattc_read(conn_handle, value_handle, _read_cb, seq));
    _check_completion_status(_wait_for_completion(2000));
    if (!_completion_done) {
        // Interrupted: nothing was read.
        return 0;
    }
    len = MIN(len, _read_len);
    memcpy(buf, _read_buf, len);
    return len;
}


static int _write_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    struct ble_gatt_attr *attr,
    void *arg) {
    portENTER_CRITICAL(&_completion_mutex);
    _complete_request(arg, error->status);
    portEXIT_CRITICAL(&_completion_mutex);

    return 0;
}

void bleio_gattc_write(uint16_t conn_handle, uint16_t value_handle, uint8_t *buf, size_t len) {
    void *seq = _start_request();
    CHECK_NIMBLE_ERROR(ble_gattc_write_flat(conn_handle, value_handle, buf, len, _write_cb, seq));
    _check_completion_status(_wait_for_completion(2000));
}
