// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "common-hal/_bleio/Adapter.h"
#include "common-hal/_bleio/__init__.h"
#include "bindings/zephyr_kernel/__init__.h"
#include "common-hal/_bleio/Connection.h"
#include "common-hal/_bleio/Service.h"
#include "supervisor/shared/bluetooth/bluetooth.h"
#include "supervisor/shared/tick.h"

#include <zephyr/bluetooth/att.h>

// The singleton _bleio.Adapter object
bleio_adapter_obj_t common_hal_bleio_adapter_obj;

// Called once by the BLE workflow at boot and again on every `import _bleio`,
// so it must not disturb a running adapter: the workflow may be advertising or
// connected by the time user code imports the module. Starting the BLE stack happens in
// common_hal_bleio_adapter_set_enabled(), which the importer calls next.
void common_hal_bleio_init(void) {
    common_hal_bleio_adapter_obj.base.type = &bleio_adapter_type;
    bleio_connection_register_auth_callbacks();
}

// Tear down user BLE state at the end of a VM run, leaving the BLE workflow
// connection up. Runs while the VM heap is still valid.
void bleio_user_reset(void) {
    if (common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        // Stop any user scanning or advertising.
        common_hal_bleio_adapter_stop_scan(&common_hal_bleio_adapter_obj);
        common_hal_bleio_adapter_stop_advertising(&common_hal_bleio_adapter_obj);

        // Disconnect the connections that user code initiated or accepted with its
        // own advertising. The BLE workflow connection is not user-owned and stays up.
        //
        // Clear each connection's pointer into the VM heap first: the heap is about
        // to go away, and a disconnect completes asynchronously, possibly after the heap
        // is gone.
        for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
            bleio_connection_internal_t *connection = &bleio_connections[i];
            connection->connection_obj = mp_const_none;
            if (connection->conn != NULL && connection->user_owned) {
                common_hal_bleio_connection_disconnect(connection);
            }
        }

        // Remove references to any VM heap objects.
        common_hal_bleio_adapter_obj.connection_objs = NULL;
        common_hal_bleio_adapter_obj.scan_results = NULL;
    }

    // Remove user-created services from Zephyr's GATT database and free their
    // buffers. Zephyr indicates Service Changed to connected peers itself, and
    // records it for bonded peers to receive when they next connect.
    bleio_service_unregister_retained();

    // Maybe start advertising the BLE workflow.
    supervisor_bluetooth_background();
}

// Called after the VM heap is gone. On nordic and espressif this restarts the
// BLE stack when user code created GATT services, because their stacks can't
// remove services one at a time. Zephyr can, and bleio_user_reset() already did,
// so there is nothing left that requires dropping the workflow connection.
void bleio_reset(void) {
    common_hal_bleio_adapter_obj.base.type = &bleio_adapter_type;
    bleio_clear_user_services_created();
}

void common_hal_bleio_gc_collect(void) {
    bleio_adapter_gc_collect(&common_hal_bleio_adapter_obj);
    bleio_service_gc_collect();
}

// =======================================================================
// Shared synchronous GATT helpers
// =======================================================================

typedef struct {
    uint8_t *buf;
    size_t buf_len;
    size_t read_len;
    volatile bool done;
    volatile int err;
} gattc_read_ctx_t;

typedef struct {
    volatile bool done;
    volatile int err;
} gattc_write_ctx_t;

static gattc_read_ctx_t *active_read_ctx;
static gattc_write_ctx_t *active_write_ctx;
static struct bt_gatt_read_params read_params;
static struct bt_gatt_write_params write_params;

static uint8_t on_gattc_read(struct bt_conn *conn, uint8_t err,
    struct bt_gatt_read_params *params,
    const void *data, uint16_t length) {
    gattc_read_ctx_t *ctx = active_read_ctx;
    if (ctx == NULL) {
        return BT_GATT_ITER_STOP;
    }

    if (err) {
        ctx->err = err;
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    if (data == NULL || length == 0) {
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    size_t copy_len = length;
    if (ctx->read_len + copy_len > ctx->buf_len) {
        copy_len = ctx->buf_len - ctx->read_len;
    }
    if (copy_len > 0) {
        memcpy(ctx->buf + ctx->read_len, data, copy_len);
        ctx->read_len += copy_len;
    }

    ctx->done = true;
    return BT_GATT_ITER_STOP;
}

static void on_gattc_write(struct bt_conn *conn, uint8_t err,
    struct bt_gatt_write_params *params) {
    gattc_write_ctx_t *ctx = active_write_ctx;
    if (ctx == NULL) {
        return;
    }
    ctx->err = err;
    ctx->done = true;
}

// Raise an exception for a nonzero ATT error code from a GATT client read or write.
// connected is whether the connection was still up when the request finished.
static void check_gattc_status(bool connected, uint8_t err) {
    if (err == 0) {
        return;
    }
    // Zephyr reports local failures, including a disconnect while the request was
    // outstanding, as BT_ATT_ERR_UNLIKELY. Check the connection before blaming the peer.
    if (!connected) {
        mp_raise_ConnectionError(MP_ERROR_TEXT("Not connected"));
    }
    switch (err) {
        case BT_ATT_ERR_READ_NOT_PERMITTED:
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("%q not permitted"), MP_QSTR_read);
            return;
        case BT_ATT_ERR_WRITE_NOT_PERMITTED:
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("%q not permitted"), MP_QSTR_write);
            return;
        case BT_ATT_ERR_AUTHENTICATION:
            mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Insufficient authentication"));
            return;
        case BT_ATT_ERR_AUTHORIZATION:
            mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Insufficient authorization"));
            return;
        case BT_ATT_ERR_INSUFFICIENT_ENCRYPTION:
        case BT_ATT_ERR_ENCRYPTION_KEY_SIZE:
            mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Insufficient encryption"));
            return;
        case BT_ATT_ERR_INVALID_ATTRIBUTE_LEN:
            mp_arg_error_invalid(MP_QSTR_length);
            return;
        default:
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Unknown gatt error: 0x%04x"), err);
    }
}

static bool conn_is_connected(struct bt_conn *conn) {
    struct bt_conn_info info;
    return bt_conn_get_info(conn, &info) == 0 && info.state == BT_CONN_STATE_CONNECTED;
}

size_t bleio_gattc_read_sync(struct bt_conn *conn, uint16_t handle,
    uint8_t *buf, size_t len) {
    gattc_read_ctx_t ctx = {
        .buf = buf,
        .buf_len = len,
        .read_len = 0,
        .done = false,
        .err = 0,
    };
    active_read_ctx = &ctx;

    memset(&read_params, 0, sizeof(read_params));
    read_params.func = on_gattc_read;
    read_params.handle_count = 1;
    read_params.single.handle = handle;
    read_params.single.offset = 0;

    // `bt_conn_ref()` holds a reference to the connection, so conn stays valid until
    // its state is checked below, even if a disconnect drops the connection's own
    // reference meanwhile. It returns NULL if the peer has already disconnected.
    if (bt_conn_ref(conn) == NULL) {
        active_read_ctx = NULL;
        mp_raise_ConnectionError(MP_ERROR_TEXT("Not connected"));
    }
    int err = bt_gatt_read(conn, &read_params);
    if (err != 0) {
        active_read_ctx = NULL;
        bt_conn_unref(conn);
        raise_zephyr_error(err);
    }

    while (!ctx.done) {
        RUN_BACKGROUND_TASKS;
    }
    active_read_ctx = NULL;

    bool connected = conn_is_connected(conn);
    bt_conn_unref(conn);
    check_gattc_status(connected, ctx.err);

    return ctx.read_len;
}

void bleio_gattc_write_sync(struct bt_conn *conn, uint16_t handle,
    const uint8_t *data, size_t len) {
    gattc_write_ctx_t ctx = {
        .done = false,
        .err = 0,
    };
    active_write_ctx = &ctx;

    memset(&write_params, 0, sizeof(write_params));
    write_params.func = on_gattc_write;
    write_params.handle = handle;
    write_params.offset = 0;
    write_params.data = data;
    write_params.length = len;

    // `bt_conn_ref()` holds a reference to the connection, so conn stays valid until
    // its state is checked below, even if a disconnect drops the connection's own
    // reference meanwhile. It returns NULL if the peer has already disconnected.
    if (bt_conn_ref(conn) == NULL) {
        active_write_ctx = NULL;
        mp_raise_ConnectionError(MP_ERROR_TEXT("Not connected"));
    }
    int err = bt_gatt_write(conn, &write_params);
    if (err != 0) {
        active_write_ctx = NULL;
        bt_conn_unref(conn);
        raise_zephyr_error(err);
    }

    while (!ctx.done) {
        RUN_BACKGROUND_TASKS;
    }
    active_write_ctx = NULL;

    bool connected = conn_is_connected(conn);
    bt_conn_unref(conn);
    check_gattc_status(connected, ctx.err);
}
