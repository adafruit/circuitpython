// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <errno.h>
#include <string.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/sys/slist.h>

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "bindings/zephyr_kernel/__init__.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "common-hal/_bleio/__init__.h"
#include "common-hal/_bleio/Characteristic.h"
#include "supervisor/port_heap.h"

// Discovery context passed through Zephyr callbacks.
typedef struct {
    volatile bool done;
    volatile int err;
} discovery_context_t;

// Convert a Zephyr bt_uuid to a CircuitPython bleio UUID object.
// Must be called OUTSIDE Zephyr callback context.
static bleio_uuid_obj_t *bleio_uuid_from_zephyr(const struct bt_uuid *zuuid) {
    bleio_uuid_obj_t *uuid = mp_obj_malloc(bleio_uuid_obj_t, &bleio_uuid_type);
    if (zuuid->type == BT_UUID_TYPE_16) {
        common_hal_bleio_uuid_construct(uuid, BT_UUID_16(zuuid)->val, NULL);
    } else {
        // Both Zephyr and CP store UUID bytes in little-endian order.
        const struct bt_uuid_128 *uuid128 = BT_UUID_128(zuuid);
        common_hal_bleio_uuid_construct(uuid,
            (uuid128->val[13] << 8) | uuid128->val[12], uuid128->val);
    }
    return uuid;
}

// Temporary storage for discovered services before creating CP objects.
// Each node is port_malloc'd in the callback and appended to a sys_slist.
typedef struct {
    sys_snode_t node;
    struct bt_uuid_128 uuid;
    uint16_t start_handle;
    uint16_t end_handle;
} discovered_service_t;

// Temporary storage for discovered characteristics before creating CP objects.
typedef struct {
    sys_snode_t node;
    struct bt_uuid_128 uuid;
    uint16_t value_handle;
    uint16_t decl_handle;
    uint8_t properties;
} discovered_char_t;

// Temporary storage for discovered descriptors.
typedef struct {
    sys_snode_t node;
    struct bt_uuid_128 uuid;
    uint16_t handle;
} discovered_desc_t;

// File-scope discovery state for synchronous blocking.
static discovery_context_t *active_discovery_ctx;
static sys_slist_t discovered_list;
static struct bt_gatt_discover_params discovery_params;

// Called from the disconnect callback (BT workqueue context) to abort any
// in-flight GATT discovery. Without this the main thread's spin loop would
// hang waiting for a callback that will never come, and the caller would then
// NULL-dereference the now-cleared connection in bt_gatt_discover().
void bleio_connection_discovery_abort(void) {
    discovery_context_t *ctx = active_discovery_ctx;
    if (ctx != NULL) {
        ctx->err = -ENOTCONN;
        ctx->done = true;
    }
}

static uint8_t on_service_discovered(struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    struct bt_gatt_discover_params *params) {
    discovery_context_t *ctx = active_discovery_ctx;

    if (attr == NULL) {
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    discovered_service_t *ds = port_malloc(sizeof(discovered_service_t), false);
    if (ds == NULL) {
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    struct bt_gatt_service_val *svc_val = (struct bt_gatt_service_val *)attr->user_data;
    ds->start_handle = attr->handle;
    ds->end_handle = svc_val->end_handle;

    // Copy UUID into our storage
    if (svc_val->uuid->type == BT_UUID_TYPE_16) {
        ds->uuid.uuid.type = BT_UUID_TYPE_16;
        ((struct bt_uuid_16 *)&ds->uuid)->val = BT_UUID_16(svc_val->uuid)->val;
    } else {
        memcpy(&ds->uuid, svc_val->uuid, sizeof(struct bt_uuid_128));
    }

    sys_slist_append(&discovered_list, &ds->node);

    return BT_GATT_ITER_CONTINUE;
}

static uint8_t on_characteristic_discovered(struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    struct bt_gatt_discover_params *params) {
    discovery_context_t *ctx = active_discovery_ctx;

    if (attr == NULL) {
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    discovered_char_t *dc = port_malloc(sizeof(discovered_char_t), false);
    if (dc == NULL) {
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    struct bt_gatt_chrc *chrc = (struct bt_gatt_chrc *)attr->user_data;
    dc->value_handle = chrc->value_handle;
    dc->decl_handle = attr->handle;
    dc->properties = chrc->properties;

    // Copy UUID into our storage
    if (chrc->uuid->type == BT_UUID_TYPE_16) {
        dc->uuid.uuid.type = BT_UUID_TYPE_16;
        ((struct bt_uuid_16 *)&dc->uuid)->val = BT_UUID_16(chrc->uuid)->val;
    } else {
        memcpy(&dc->uuid, chrc->uuid, sizeof(struct bt_uuid_128));
    }

    sys_slist_append(&discovered_list, &dc->node);

    return BT_GATT_ITER_CONTINUE;
}

// Pairing: characteristic pointer with its declaration handle for descriptor discovery.
typedef struct {
    bleio_characteristic_obj_t *characteristic;
    uint16_t decl_handle;
} char_with_decl_t;

// Forward declaration for use by descriptor discovery.
static void free_discovered_list(void);
static void bleio_discovery_check_connected(struct bt_conn *conn,
    discovery_context_t *ctx);

// Callback for descriptor discovery.
static uint8_t on_descriptor_discovered(struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    struct bt_gatt_discover_params *params) {
    discovery_context_t *ctx = active_discovery_ctx;

    if (attr == NULL) {
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    discovered_desc_t *dd = port_malloc(sizeof(discovered_desc_t), false);
    if (dd == NULL) {
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    dd->handle = attr->handle;

    // Copy UUID into our storage
    if (attr->uuid->type == BT_UUID_TYPE_16) {
        dd->uuid.uuid.type = BT_UUID_TYPE_16;
        ((struct bt_uuid_16 *)&dd->uuid)->val = BT_UUID_16(attr->uuid)->val;
    } else {
        memcpy(&dd->uuid, attr->uuid, sizeof(struct bt_uuid_128));
    }

    sys_slist_append(&discovered_list, &dd->node);

    return BT_GATT_ITER_CONTINUE;
}

// Create descriptors from discovered_list and add them to the characteristic.
// Called OUTSIDE the Zephyr callback context. Drains and frees all nodes.
static void create_descriptors_from_discovered(bleio_characteristic_obj_t *characteristic) {
    sys_snode_t *node;
    while ((node = sys_slist_get(&discovered_list)) != NULL) {
        discovered_desc_t *dd = CONTAINER_OF(node, discovered_desc_t, node);

        bleio_uuid_obj_t *uuid = bleio_uuid_from_zephyr(&dd->uuid.uuid);

        // Remember handles for certain well-known descriptors.
        if (dd->uuid.uuid.type == BT_UUID_TYPE_16) {
            switch (((struct bt_uuid_16 *)&dd->uuid)->val) {
                case 0x2902:
                    characteristic->cccd_handle = dd->handle;
                    break;
                case 0x2903:
                    characteristic->sccd_handle = dd->handle;
                    break;
                case 0x2901:
                    characteristic->user_desc_handle = dd->handle;
                    break;
                default:
                    break;
            }
        }

        bleio_descriptor_obj_t *descriptor =
            mp_obj_malloc(bleio_descriptor_obj_t, &bleio_descriptor_type);

        // Remote descriptors: set characteristic and UUID only.
        // Reads/writes go over GATT via the handle.
        descriptor->characteristic = characteristic;
        descriptor->uuid = uuid;
        descriptor->handle = dd->handle;
        descriptor->read_perm = SECURITY_MODE_OPEN;
        descriptor->write_perm = SECURITY_MODE_OPEN;
        descriptor->max_length = 20;
        descriptor->fixed_length = false;
        descriptor->value = m_malloc(20);
        memset(descriptor->value, 0, 20);
        descriptor->value_length = 0;

        common_hal_bleio_characteristic_add_descriptor(characteristic, descriptor);

        port_free(dd);
    }
}

// Discover descriptors for a single characteristic.
static void discover_descriptors_for_characteristic(struct bt_conn *conn,
    discovery_context_t *ctx, bleio_characteristic_obj_t *characteristic,
    uint16_t end_handle) {
    // Bail cleanly if the link dropped before this phase began.
    bleio_discovery_check_connected(conn, ctx);
    uint16_t start = characteristic->handle + 1;
    if (start > end_handle) {
        return;
    }

    sys_slist_init(&discovered_list);
    ctx->done = false;
    ctx->err = 0;

    memset(&discovery_params, 0, sizeof(discovery_params));
    discovery_params.uuid = NULL;
    discovery_params.start_handle = start;
    discovery_params.end_handle = end_handle;
    discovery_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    discovery_params.func = on_descriptor_discovered;

    int err = bt_gatt_discover(conn, &discovery_params);
    if (err != 0) {
        free_discovered_list();
        if (err == -ENOENT) {
            return;
        }
        raise_zephyr_error(err);
    }

    while (!ctx->done) {
        RUN_BACKGROUND_TASKS;
    }

    // The link may have dropped while we were waiting.
    bleio_discovery_check_connected(conn, ctx);

    create_descriptors_from_discovered(characteristic);
}

// Create CircuitPython characteristic objects from the discovered_list.
// Called OUTSIDE the Zephyr callback context so MP allocations are safe.
// Drains and frees all nodes from the list.
// Returns the number of characteristics created and fills the chars_out array
// (which must be large enough).
static size_t create_characteristics_from_discovered(bleio_service_obj_t *service,
    char_with_decl_t *chars_out, size_t max_chars) {
    size_t count = 0;
    sys_snode_t *node;
    while ((node = sys_slist_get(&discovered_list)) != NULL) {
        discovered_char_t *dc = CONTAINER_OF(node, discovered_char_t, node);

        bleio_uuid_obj_t *uuid = bleio_uuid_from_zephyr(&dc->uuid.uuid);

        bleio_characteristic_properties_t props = 0;
        if (dc->properties & BT_GATT_CHRC_BROADCAST) {
            props |= CHAR_PROP_BROADCAST;
        }
        if (dc->properties & BT_GATT_CHRC_READ) {
            props |= CHAR_PROP_READ;
        }
        if (dc->properties & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
            props |= CHAR_PROP_WRITE_NO_RESPONSE;
        }
        if (dc->properties & BT_GATT_CHRC_WRITE) {
            props |= CHAR_PROP_WRITE;
        }
        if (dc->properties & BT_GATT_CHRC_NOTIFY) {
            props |= CHAR_PROP_NOTIFY;
        }
        if (dc->properties & BT_GATT_CHRC_INDICATE) {
            props |= CHAR_PROP_INDICATE;
        }

        bleio_characteristic_obj_t *characteristic =
            mp_obj_malloc(bleio_characteristic_obj_t, &bleio_characteristic_type);

        // Use max_length=20 for remote chars - reads go over the wire.
        common_hal_bleio_characteristic_construct(
            characteristic, service,
            dc->value_handle, uuid, props,
            SECURITY_MODE_OPEN, SECURITY_MODE_OPEN,
            20, false, NULL, NULL);

        if (count < max_chars) {
            chars_out[count].characteristic = characteristic;
            chars_out[count].decl_handle = dc->decl_handle;
        }
        count++;

        port_free(dc);
    }
    return count;
}

// Helper to drain and free all nodes from the discovered_list.
static void free_discovered_list(void) {
    sys_snode_t *node;
    while ((node = sys_slist_get(&discovered_list)) != NULL) {
        port_free(node);
    }
}

// The link dropped during discovery: the disconnect callback cleared
// connection->conn (so a subsequent bt_gatt_discover() would NULL-deref, since
// CONFIG_ASSERT is off) and/or aborted the active discovery (ctx->err set).
// Stop discovery cleanly with a "Not connected" exception instead of
// crashing or hanging on the spin loop.
static void bleio_discovery_check_connected(struct bt_conn *conn,
    discovery_context_t *ctx) {
    if (conn == NULL || ctx->err != 0) {
        free_discovered_list();
        active_discovery_ctx = NULL;
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Not connected"));
    }
}

// Discover characteristics for a single remote service.
static void discover_characteristics_for_service(struct bt_conn *conn,
    discovery_context_t *ctx, bleio_service_obj_t *service) {
    // Bail cleanly if the link dropped before this phase began.
    bleio_discovery_check_connected(conn, ctx);
    // Need at least 2 handles: one for the service declaration, one for a characteristic
    if (service->end_handle <= service->start_handle) {
        return;
    }

    sys_slist_init(&discovered_list);
    ctx->done = false;
    ctx->err = 0;

    memset(&discovery_params, 0, sizeof(discovery_params));
    discovery_params.uuid = NULL;
    discovery_params.start_handle = service->start_handle + 1;
    discovery_params.end_handle = service->end_handle;
    discovery_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    discovery_params.func = on_characteristic_discovered;

    int err = bt_gatt_discover(conn, &discovery_params);
    if (err != 0) {
        free_discovered_list();
        // -ENOENT means no characteristics found in the range, which is fine.
        if (err == -ENOENT) {
            return;
        }
        raise_zephyr_error(err);
    }

    while (!ctx->done) {
        RUN_BACKGROUND_TASKS;
    }

    // The link may have dropped while we were waiting.
    bleio_discovery_check_connected(conn, ctx);

    // Create CP objects outside of callback context where MP allocations are safe.
    // This drains and frees the list nodes.
    char_with_decl_t chars[16];
    size_t num_chars = create_characteristics_from_discovered(service, chars, 16);

    // Discover descriptors for each characteristic.
    // The descriptor range for char[i] is from char[i].handle+1 to
    // char[i+1].decl_handle-1 (or service.end_handle for the last).
    for (size_t i = 0; i < num_chars && i < 16; i++) {
        uint16_t desc_end;
        if (i + 1 < num_chars) {
            desc_end = chars[i + 1].decl_handle - 1;
        } else {
            desc_end = service->end_handle;
        }
        discover_descriptors_for_characteristic(conn, ctx,
            chars[i].characteristic, desc_end);
    }
}

// ===== PAIRING / BONDING =====

// Auth info callbacks to track pairing completion/failure
static struct bt_conn_auth_info_cb auth_info_cb;
static bool auth_info_cb_registered = false;

static void on_pairing_complete(struct bt_conn *conn, bool bonded) {
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn == conn) {
            bleio_connections[i].pair_status = PAIR_PAIRED;
            bleio_connections[i].sec_err = 0;
            break;
        }
    }
}

static void on_pairing_failed(struct bt_conn *conn, enum bt_security_err reason) {
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn == conn) {
            bleio_connections[i].pair_status = PAIR_NOT_PAIRED;
            bleio_connections[i].sec_err = (uint8_t)reason;
            break;
        }
    }
}

void bleio_connection_register_auth_callbacks(void) {
    if (!auth_info_cb_registered) {
        auth_info_cb.pairing_complete = on_pairing_complete;
        auth_info_cb.pairing_failed = on_pairing_failed;
        bt_conn_auth_info_cb_register(&auth_info_cb);
        auth_info_cb_registered = true;
    }
}

void common_hal_bleio_connection_pair(bleio_connection_internal_t *self, bool bond) {
    (void)bond; // Bonding is enabled globally via CONFIG_BT_BONDABLE=y

    if (self == NULL || self->conn == NULL) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Not connected"));
    }

    // Already paired?
    if (self->pair_status == PAIR_PAIRED) {
        return;
    }

    // Ensure auth callbacks are registered
    bleio_connection_register_auth_callbacks();

    self->pair_status = PAIR_WAITING;
    self->sec_err = 0;

    int err = bt_conn_set_security(self->conn, BT_SECURITY_L2);
    if (err != 0) {
        self->pair_status = PAIR_NOT_PAIRED;
        raise_zephyr_error(err);
    }

    // bt_conn_set_security may return 0 immediately if already encrypted
    // (e.g., reconnection with stored bond keys). Check current security level.
    if (bt_conn_get_security(self->conn) > BT_SECURITY_L1) {
        self->pair_status = PAIR_PAIRED;
        return;
    }

    // Wait for pairing to complete. Zephyr's SMP reports each attempt via
    // pairing_failed before auto-restarting security (smp.c), so a stale-bond
    // retry shows up as an intermediate BT_SECURITY_ERR_PIN_OR_KEY_MISSING
    // followed by a fresh attempt that can succeed. Ride through those and
    // rely on Zephyr to signal the end: success (PAIR_PAIRED), SMP_TIMEOUT
    // dropping the link (observed as conn == NULL), or a user interrupt.
    while (self->pair_status != PAIR_PAIRED
           && self->conn != NULL
           && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }

    if (mp_hal_is_interrupted()) {
        if (self->conn != NULL) {
            bt_conn_auth_cancel(self->conn);
        }
        self->pair_status = PAIR_NOT_PAIRED;
        return;
    }

    if (self->pair_status == PAIR_PAIRED) {
        return;
    }

    // Reuse the messages shared with the nordic _bleio implementation
    // (check_sec_status). A dropped link clears sec_err, so it falls through
    // to the "unspecified issue" message alongside the explicit UNSPECIFIED
    // error code rather than introducing a zephyr-specific "Pairing failed"
    // string.
    uint8_t sec_err_code = self->sec_err;
    if (sec_err_code == BT_SECURITY_ERR_UNSPECIFIED || sec_err_code == 0) {
        mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Unspecified issue. The pairing request on the other device may have been declined or ignored."));
    } else {
        mp_raise_bleio_SecurityError(MP_ERROR_TEXT("Unknown security error: 0x%04x"), sec_err_code);
    }
}

void common_hal_bleio_connection_disconnect(bleio_connection_internal_t *self) {
    if (self == NULL || self->conn == NULL) {
        return;
    }

    int err = bt_conn_disconnect(self->conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (err != 0 && err != -ENOTCONN) {
        raise_zephyr_error(err);
    }

    // The connection may now be disconnecting; force connections tuple rebuild.
    common_hal_bleio_adapter_obj.connection_objs = NULL;
}

bool common_hal_bleio_connection_get_connected(bleio_connection_obj_t *self) {
    if (self == NULL || self->connection == NULL) {
        return false;
    }

    bleio_connection_internal_t *connection = self->connection;
    if (connection->conn == NULL) {
        return false;
    }

    struct bt_conn_info info;
    if (bt_conn_get_info(connection->conn, &info) != 0) {
        return false;
    }

    return info.state == BT_CONN_STATE_CONNECTED || info.state == BT_CONN_STATE_DISCONNECTING;
}

mp_int_t common_hal_bleio_connection_get_max_packet_length(bleio_connection_internal_t *self) {
    if (self == NULL || self->conn == NULL) {
        return 20;
    }

    uint16_t mtu = bt_gatt_get_mtu(self->conn);
    if (mtu < 3) {
        return 20;
    }
    return mtu - 3;
}

bool common_hal_bleio_connection_get_paired(bleio_connection_obj_t *self) {
    if (self == NULL || self->connection == NULL) {
        return false;
    }
    return self->connection->pair_status == PAIR_PAIRED;
}

mp_obj_tuple_t *common_hal_bleio_connection_discover_remote_services(bleio_connection_obj_t *self, mp_obj_t service_uuids_whitelist) {
    bleio_connection_internal_t *connection = self->connection;
    if (connection == NULL || connection->conn == NULL) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Not connected"));
    }

    discovery_context_t ctx;
    ctx.done = false;
    ctx.err = 0;

    active_discovery_ctx = &ctx;
    sys_slist_init(&discovered_list);

    // Create the result list for service objects.
    mp_obj_list_t *service_list = mp_obj_new_list(0, NULL);

    // Discover primary services (callback appends nodes to discovered_list).
    // When a whitelist is given, pass each UUID to Zephyr so filtering happens
    // on the remote device — only matching services are returned.
    if (service_uuids_whitelist == mp_const_none) {
        memset(&discovery_params, 0, sizeof(discovery_params));
        discovery_params.uuid = NULL;
        discovery_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
        discovery_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
        discovery_params.type = BT_GATT_DISCOVER_PRIMARY;
        discovery_params.func = on_service_discovered;

        int err = bt_gatt_discover(connection->conn, &discovery_params);
        if (err != 0) {
            free_discovered_list();
            active_discovery_ctx = NULL;
            raise_zephyr_error(err);
        }

        while (!ctx.done) {
            RUN_BACKGROUND_TASKS;
        }

        // The link may have dropped during primary discovery.
        bleio_discovery_check_connected(connection->conn, &ctx);
    } else {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(service_uuids_whitelist, &iter_buf);
        mp_obj_t uuid_obj;
        while ((uuid_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            bleio_uuid_obj_t *cp_uuid = mp_arg_validate_type(uuid_obj, &bleio_uuid_type, MP_QSTR_uuid);

            memset(&discovery_params, 0, sizeof(discovery_params));
            discovery_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
            discovery_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
            discovery_params.type = BT_GATT_DISCOVER_PRIMARY;
            discovery_params.func = on_service_discovered;

            // Set the Zephyr UUID filter. Stack variables are safe because
            // bt_gatt_discover is synchronous (we spin until ctx.done).
            union {
                struct bt_uuid_16 u16;
                struct bt_uuid_128 u128;
            } z_uuid;
            if (cp_uuid->size == BT_UUID_SIZE_16) {
                z_uuid.u16.uuid.type = BT_UUID_TYPE_16;
                z_uuid.u16.val = cp_uuid->uuid16;
                discovery_params.uuid = &z_uuid.u16.uuid;
            } else {
                z_uuid.u128.uuid.type = BT_UUID_TYPE_128;
                memcpy(z_uuid.u128.val, cp_uuid->uuid128, 16);
                discovery_params.uuid = &z_uuid.u128.uuid;
            }

            ctx.done = false;
            ctx.err = 0;

            int err = bt_gatt_discover(connection->conn, &discovery_params);
            if (err != 0) {
                free_discovered_list();
                active_discovery_ctx = NULL;
                raise_zephyr_error(err);
            }

            while (!ctx.done) {
                RUN_BACKGROUND_TASKS;
            }

            // The link may have dropped during this UUID's discovery; stop
            // before the next iteration clears ctx.err and crashes on a NULL conn.
            bleio_discovery_check_connected(connection->conn, &ctx);
        }
    }

    // Create CP service objects outside callback context where MP allocations are safe.
    // Drain and free the discovered service nodes.
    mp_obj_list_t *result_list = service_list;
    sys_snode_t *snode;
    while ((snode = sys_slist_get(&discovered_list)) != NULL) {
        discovered_service_t *ds = CONTAINER_OF(snode, discovered_service_t, node);
        bleio_uuid_obj_t *uuid = bleio_uuid_from_zephyr(&ds->uuid.uuid);
        bleio_service_obj_t *service = mp_obj_malloc(bleio_service_obj_t, &bleio_service_type);
        common_hal_bleio_service_from_remote_service(service, self, uuid, false);
        service->start_handle = ds->start_handle;
        service->end_handle = ds->end_handle;
        mp_obj_list_append(MP_OBJ_FROM_PTR(service_list), MP_OBJ_FROM_PTR(service));
        port_free(ds);
    }

    // Discover characteristics for each service
    for (size_t i = 0; i < result_list->len; i++) {
        bleio_service_obj_t *svc = MP_OBJ_TO_PTR(result_list->items[i]);
        if (svc->start_handle < svc->end_handle) {
            // discover_characteristics_for_service re-checks, but guard here
            // too so we don't enter the phase after a mid-loop disconnect.
            bleio_discovery_check_connected(connection->conn, &ctx);
            discover_characteristics_for_service(connection->conn, &ctx, svc);
        }
    }

    active_discovery_ctx = NULL;

    return mp_obj_new_tuple(result_list->len, result_list->items);
}

mp_float_t common_hal_bleio_connection_get_connection_interval(bleio_connection_internal_t *self) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_bleio_connection_set_connection_interval(bleio_connection_internal_t *self, mp_float_t new_interval) {
    mp_raise_NotImplementedError(NULL);
}

mp_obj_t bleio_connection_new_from_internal(bleio_connection_internal_t *connection) {
    if (connection == NULL) {
        return mp_const_none;
    }

    if (connection->connection_obj != mp_const_none) {
        return connection->connection_obj;
    }

    bleio_connection_obj_t *connection_obj = mp_obj_malloc(bleio_connection_obj_t, &bleio_connection_type);
    connection_obj->connection = connection;
    connection_obj->disconnect_reason = 0;
    connection->connection_obj = MP_OBJ_FROM_PTR(connection_obj);

    return connection->connection_obj;
}
