// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/hci_vs.h>
#include <zephyr/settings/settings.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "bindings/zephyr_kernel/__init__.h"
#include "shared-bindings/_bleio/__init__.h"
#include "common-hal/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-module/_bleio/Address.h"
#include "shared-module/_bleio/ScanResults.h"
#include "supervisor/background_callback.h"
#include "supervisor/shared/bluetooth/bluetooth.h"
#include "supervisor/shared/tick.h"

bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

// Background pump: drains the BLE file-transfer / serial PacketBuffers by
// calling supervisor_bluetooth_background().  Queued from GATT write callbacks
// and connection events so the VM processes incoming data promptly.
static background_callback_t bluetooth_background_cb = {NULL, NULL};

static void bluetooth_adapter_background(void *data) {
    (void)data;
    supervisor_bluetooth_background();
}

void bleio_request_bluetooth_background(void) {
    if (bluetooth_background_cb.fun != NULL) {
        background_callback_add_core(&bluetooth_background_cb);
    }
}

static bool scan_callbacks_registered = false;
static bleio_scanresults_obj_t *active_scan_results = NULL;
static struct bt_le_scan_cb scan_callbacks;
static bool ble_advertising = false;
// True when advertising was started by the BLE workflow (supervisor) rather
// than user code. Lets the workflow restart its own adverts without disturbing
// user-initiated advertising.
static bool ble_advertising_internal = false;
static bool ble_adapter_enabled = true;

#define BLEIO_ADV_MAX_FIELDS 16
#define BLEIO_ADV_MAX_DATA_LEN 31
static struct bt_data adv_data[BLEIO_ADV_MAX_FIELDS];
static struct bt_data scan_resp_data[BLEIO_ADV_MAX_FIELDS];
static uint8_t adv_data_storage[BLEIO_ADV_MAX_DATA_LEN];
static uint8_t scan_resp_storage[BLEIO_ADV_MAX_DATA_LEN];

static uint8_t bleio_address_type_from_zephyr(const bt_addr_le_t *addr) {
    if (addr == NULL) {
        return BLEIO_ADDRESS_TYPE_PUBLIC;
    }

    switch (addr->type) {
        case BT_ADDR_LE_PUBLIC:
        case BT_ADDR_LE_PUBLIC_ID:
            return BLEIO_ADDRESS_TYPE_PUBLIC;
        case BT_ADDR_LE_RANDOM:
        case BT_ADDR_LE_RANDOM_ID:
        case BT_ADDR_LE_UNRESOLVED:
            if (BT_ADDR_IS_RPA(&addr->a)) {
                return BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_RESOLVABLE;
            }
            if (BT_ADDR_IS_NRPA(&addr->a)) {
                return BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE;
            }
            return BLEIO_ADDRESS_TYPE_RANDOM_STATIC;
        default:
            return BLEIO_ADDRESS_TYPE_PUBLIC;
    }
}

static uint8_t bleio_address_type_to_zephyr(uint8_t type) {
    switch (type) {
        case BLEIO_ADDRESS_TYPE_PUBLIC:
            return BT_ADDR_LE_PUBLIC;
        case BLEIO_ADDRESS_TYPE_RANDOM_STATIC:
        case BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_RESOLVABLE:
        case BLEIO_ADDRESS_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE:
            return BT_ADDR_LE_RANDOM;
        default:
            return BT_ADDR_LE_PUBLIC;
    }
}

static bleio_connection_internal_t *bleio_connection_find_by_conn(const struct bt_conn *conn) {
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn == conn) {
            return connection;
        }
    }

    return NULL;
}

static bleio_connection_internal_t *bleio_connection_track(struct bt_conn *conn) {
    bleio_connection_internal_t *connection = bleio_connection_find_by_conn(conn);
    if (connection == NULL) {
        for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
            bleio_connection_internal_t *candidate = &bleio_connections[i];
            if (candidate->conn == NULL) {
                connection = candidate;
                break;
            }
        }
    }

    if (connection == NULL) {
        return NULL;
    }

    if (connection->conn == NULL) {
        connection->conn = bt_conn_ref(conn);
    }

    return connection;
}

static void bleio_connection_clear(bleio_connection_internal_t *self) {
    if (self == NULL) {
        return;
    }

    if (self->conn != NULL) {
        bt_conn_unref(self->conn);
        self->conn = NULL;
    }

    self->connection_obj = mp_const_none;
    self->pair_status = PAIR_NOT_PAIRED;
    self->sec_err = 0;
}

static void bleio_connection_release(bleio_connection_internal_t *connection, uint8_t reason) {
    if (connection == NULL) {
        return;
    }

    if (connection->connection_obj != mp_const_none) {
        bleio_connection_obj_t *connection_obj = MP_OBJ_TO_PTR(connection->connection_obj);
        connection_obj->connection = NULL;
        connection_obj->disconnect_reason = reason;
    }

    bleio_connection_clear(connection);
    common_hal_bleio_adapter_obj.connection_objs = NULL;
}

// Per-connection ATT MTU exchange parameters. The params struct must persist
// until the exchange callback fires, so it lives for the lifetime of the slot.
static struct bt_gatt_exchange_params mtu_exchange_params[BLEIO_TOTAL_CONNECTION_COUNT];

static void on_mtu_exchanged(struct bt_conn *conn, uint8_t err,
    struct bt_gatt_exchange_params *params) {
    (void)conn;
    (void)params;
    if (err == 0) {
        // Wake the workflow so outgoing_packet_length is recomputed with the
        // now-larger negotiated MTU.
        bleio_request_bluetooth_background();
    }
}

static void bleio_connected_cb(struct bt_conn *conn, uint8_t err) {
    if (err != 0) {
        return;
    }

    bleio_connection_internal_t *connection = bleio_connection_track(conn);
    if (connection == NULL) {
        bt_conn_disconnect(conn, BT_HCI_ERR_CONN_LIMIT_EXCEEDED);
        return;
    }

    // Initiate an ATT MTU exchange so the negotiated MTU reflects the larger
    // payload our stack supports (CONFIG_BT_L2CAP_TX_MTU). Many centrals do
    // this themselves, but if they don't we'd be stuck at the default 23-byte
    // MTU (20-byte payload). That forces the file-transfer workflow to split
    // protocol messages across notifications in ways peers can't reassemble
    // (e.g. a listdir_entry whose second fragment begins with a 0x00 flags
    // byte is misread as "unknown command 0x00"). bt_gatt_exchange_mtu returns
    // -EALREADY if the peer already initiated, so this is safe either way.
    size_t idx = (size_t)(connection - bleio_connections);
    mtu_exchange_params[idx].func = on_mtu_exchanged;
    int mtu_err = bt_gatt_exchange_mtu(conn, &mtu_exchange_params[idx]);
    (void)mtu_err;

    // When connectable advertising results in a connection, the controller
    // auto-stops advertising.  Clear our flag to match (we cannot call
    // stop_advertising() here because this callback runs in Zephyr's BT
    // thread context).
    ble_advertising = false;
    ble_advertising_internal = false;

    common_hal_bleio_adapter_obj.connection_objs = NULL;

    // Pump the workflow once now, and arm the recurring background callback
    // so future GATT writes / events get drained by the VM.
    bluetooth_background_cb.fun = bluetooth_adapter_background;
    bluetooth_background_cb.data = NULL;
    bluetooth_adapter_background(NULL);
}

static void bleio_disconnected_cb(struct bt_conn *conn, uint8_t reason) {
    bleio_connection_discovery_abort();
    bleio_connection_release(bleio_connection_find_by_conn(conn), reason);
    bleio_request_bluetooth_background();
}

static void bleio_security_changed_cb(struct bt_conn *conn, bt_security_t level,
    enum bt_security_err err) {
    bleio_connection_internal_t *connection = bleio_connection_find_by_conn(conn);
    if (connection == NULL) {
        return;
    }

    if (err == BT_SECURITY_ERR_SUCCESS && level > BT_SECURITY_L1) {
        // Security was established (encryption enabled).
        // This happens both on first-time pairing and when reconnecting
        // with stored bond keys.
        connection->pair_status = PAIR_PAIRED;
    } else if (err != BT_SECURITY_ERR_SUCCESS) {
        connection->pair_status = PAIR_NOT_PAIRED;
        connection->sec_err = (uint8_t)err;
    }
}

BT_CONN_CB_DEFINE(bleio_connection_callbacks) = {
    .connected = bleio_connected_cb,
    .disconnected = bleio_disconnected_cb,
    .security_changed = bleio_security_changed_cb,
};

static void scan_recv_cb(const struct bt_le_scan_recv_info *info, struct net_buf_simple *buf) {
    if (active_scan_results == NULL || info == NULL || buf == NULL) {
        return;
    }

    const bool connectable = (info->adv_props & BT_GAP_ADV_PROP_CONNECTABLE) != 0;
    const bool scan_response = (info->adv_props & BT_GAP_ADV_PROP_SCAN_RESPONSE) != 0;
    const bt_addr_le_t *addr = info->addr;

    uint8_t addr_bytes[NUM_BLEIO_ADDRESS_BYTES] = {0};
    if (addr != NULL) {
        memcpy(addr_bytes, addr->a.val, sizeof(addr_bytes));
    }

    shared_module_bleio_scanresults_append(active_scan_results,
        supervisor_ticks_ms64(),
        connectable,
        scan_response,
        info->rssi,
        addr_bytes,
        bleio_address_type_from_zephyr(addr),
        buf->data,
        buf->len);
}

static void scan_timeout_cb(void) {
    if (active_scan_results == NULL) {
        return;
    }
    shared_module_bleio_scanresults_set_done(active_scan_results, true);
    active_scan_results = NULL;
}

// We need to disassemble the full advertisement packet because the Zephyr takes
// in each ADT in an array.
static size_t bleio_parse_adv_data(const uint8_t *raw, size_t raw_len, struct bt_data *out,
    size_t out_len, uint8_t *storage, size_t storage_len) {
    size_t count = 0;
    size_t offset = 0;
    size_t storage_offset = 0;

    while (offset < raw_len) {
        uint8_t field_len = raw[offset];
        if (field_len == 0) {
            offset++;
            continue;
        }
        uint8_t data_len = field_len - 1;
        if (offset + field_len + 1 > raw_len ||
            count >= out_len ||
            storage_offset + data_len > storage_len) {
            return 0;
        }
        uint8_t type = raw[offset + 1];
        memcpy(storage + storage_offset, raw + offset + 2, data_len);
        out[count].type = type;
        out[count].data_len = data_len;
        out[count].data = storage + storage_offset;
        storage_offset += data_len;
        count++;
        offset += field_len + 1;
    }

    return count;
}

static uint16_t bleio_validate_and_convert_timeout(mp_float_t timeout) {
    mp_arg_validate_float_range(timeout, 0, UINT16_MAX, MP_QSTR_timeout);

    if (timeout <= 0.0f) {
        return 0;
    }

    const mp_int_t timeout_units =
        mp_arg_validate_int_range((mp_int_t)(timeout * 100.0f + 0.5f), 1, UINT16_MAX, MP_QSTR_timeout);

    return (uint16_t)timeout_units;
}

void common_hal_bleio_adapter_set_enabled(bleio_adapter_obj_t *self, bool enabled) {
    if (enabled == ble_adapter_enabled) {
        return;
    }
    if (enabled) {
        for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
            bleio_connection_clear(&bleio_connections[i]);
        }
        if (!bt_is_ready()) {
            int err = bt_enable(NULL);
            if (err != 0 && err != -EALREADY) {
                raise_zephyr_error(err);
            }

            // bt_init() returns early without setting BT_DEV_READY when
            // CONFIG_BT_SETTINGS=y and no identity is loaded yet.
            // Load settings so the BT settings handler fires and calls
            // bt_finalize_init() which sets BT_DEV_READY.
            settings_load();
        }
        ble_adapter_enabled = true;
        return;
    }

    // On Zephyr bsim + HCI IPC, disabling and immediately re-enabling BLE can
    // race endpoint rebinding during soft reload. Keep the controller running,
    // but present adapter.enabled=False to CircuitPython code.
    common_hal_bleio_adapter_stop_scan(self);
    common_hal_bleio_adapter_stop_advertising(self);
    ble_adapter_enabled = false;
}

bool common_hal_bleio_adapter_get_enabled(bleio_adapter_obj_t *self) {
    return ble_adapter_enabled;
}

mp_int_t common_hal_bleio_adapter_get_tx_power(bleio_adapter_obj_t *self) {
    struct bt_hci_cp_vs_read_tx_power_level *cp;
    struct bt_hci_rp_vs_read_tx_power_level *rp;
    struct net_buf *buf, *rsp = NULL;

    buf = bt_hci_cmd_alloc(K_MSEC(1000));
    if (!buf) {
        mp_raise_msg(&mp_type_MemoryError, NULL);
    }
    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle_type = BT_HCI_VS_LL_HANDLE_TYPE_ADV;
    cp->handle = 0;

    int err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_READ_TX_POWER_LEVEL, buf, &rsp);
    if (err) {
        raise_zephyr_error(err);
    }

    rp = (void *)rsp->data;
    int8_t power = rp->tx_power_level;
    net_buf_unref(rsp);
    return power;
}

// Non-raising variant of common_hal_bleio_adapter_set_tx_power for use from the
// BLE workflow, which runs outside the VM. Returns 0 on success.
static int bleio_adapter_set_tx_power_noraise(mp_int_t tx_power) {
    struct bt_hci_cp_vs_write_tx_power_level *cp;
    struct net_buf *buf, *rsp = NULL;

    buf = bt_hci_cmd_alloc(K_MSEC(3000));
    if (!buf) {
        return -ENOMEM;
    }
    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle_type = BT_HCI_VS_LL_HANDLE_TYPE_ADV;
    cp->handle = 0;
    cp->tx_power_level = (int8_t)tx_power;

    int err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL, buf, &rsp);
    if (err) {
        return err;
    }

    net_buf_unref(rsp);
    return 0;
}

void common_hal_bleio_adapter_set_tx_power(bleio_adapter_obj_t *self, mp_int_t tx_power) {
    int err = bleio_adapter_set_tx_power_noraise(tx_power);
    if (err) {
        raise_zephyr_error(err);
    }
}

bleio_address_obj_t *common_hal_bleio_adapter_get_address(bleio_adapter_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_bleio_adapter_set_address(bleio_adapter_obj_t *self, bleio_address_obj_t *address) {
    mp_raise_NotImplementedError(NULL);
}

mp_obj_str_t *common_hal_bleio_adapter_get_name(bleio_adapter_obj_t *self) {
    (void)self;
    const char *name = bt_get_name();
    return mp_obj_new_str(name, strlen(name));
}

void common_hal_bleio_adapter_set_name(bleio_adapter_obj_t *self, const char *name) {
    (void)self;
    size_t len = strlen(name);
    int err = 0;
    if (len > CONFIG_BT_DEVICE_NAME_MAX) {
        char truncated[CONFIG_BT_DEVICE_NAME_MAX + 1];
        memcpy(truncated, name, CONFIG_BT_DEVICE_NAME_MAX);
        truncated[CONFIG_BT_DEVICE_NAME_MAX] = '\0';
        err = bt_set_name(truncated);
    } else {
        err = bt_set_name(name);
    }
    if (err != 0) {
        raise_zephyr_error(err);
    }
}

// Internal start_advertising used by the BLE workflow (file transfer + serial
// services). Runs outside the VM, so it must not raise. Returns 0 on success or
// a positive errno on failure. A timeout of 0 means advertise indefinitely.
// This is the core implementation; common_hal_bleio_adapter_start_advertising()
// delegates here and translates errors into exceptions.
uint32_t _common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self,
    bool connectable, bool anonymous, uint32_t timeout, float interval,
    const uint8_t *advertising_data, uint16_t advertising_data_len,
    const uint8_t *scan_response_data, uint16_t scan_response_data_len,
    mp_int_t tx_power, const bleio_address_obj_t *directed_to) {
    (void)directed_to;
    (void)interval;
    (void)anonymous;
    (void)timeout;

    if (advertising_data_len > BLEIO_ADV_MAX_DATA_LEN ||
        scan_response_data_len > BLEIO_ADV_MAX_DATA_LEN) {
        return (uint32_t)EINVAL;
    }

    // Don't disturb advertising that is already active (either user code or the
    // workflow's own previous advert). The caller is responsible for stopping
    // first if a restart is desired.
    if (ble_advertising) {
        return (uint32_t)EBUSY;
    }

    bt_addr_le_t id_addrs[CONFIG_BT_ID_MAX];
    size_t id_count = CONFIG_BT_ID_MAX;
    bt_id_get(id_addrs, &id_count);
    if (id_count == 0 || bt_addr_le_eq(&id_addrs[BT_ID_DEFAULT], BT_ADDR_LE_ANY)) {
        int id = bt_id_create(NULL, NULL);
        if (id < 0) {
            return (uint32_t)(-id);
        }
    }

    size_t adv_count = bleio_parse_adv_data(advertising_data,
        advertising_data_len,
        adv_data,
        BLEIO_ADV_MAX_FIELDS,
        adv_data_storage,
        sizeof(adv_data_storage));
    if (adv_count == 0) {
        return (uint32_t)EINVAL;
    }

    size_t scan_resp_count = 0;
    if (scan_response_data_len > 0) {
        scan_resp_count = bleio_parse_adv_data(scan_response_data,
            scan_response_data_len,
            scan_resp_data,
            BLEIO_ADV_MAX_FIELDS,
            scan_resp_storage,
            sizeof(scan_resp_storage));
        if (scan_resp_count == 0) {
            return (uint32_t)EINVAL;
        }
    }

    struct bt_le_adv_param adv_params;
    if (connectable) {
        adv_params = (struct bt_le_adv_param)BT_LE_ADV_PARAM_INIT(
            BT_LE_ADV_OPT_CONN,
            BT_GAP_ADV_FAST_INT_MIN_1,
            BT_GAP_ADV_FAST_INT_MAX_1,
            NULL);
    } else if (scan_resp_count > 0) {
        adv_params = (struct bt_le_adv_param)BT_LE_ADV_PARAM_INIT(
            BT_LE_ADV_OPT_SCANNABLE,
            BT_GAP_ADV_FAST_INT_MIN_2,
            BT_GAP_ADV_FAST_INT_MAX_2,
            NULL);
    } else {
        adv_params = (struct bt_le_adv_param)BT_LE_ADV_PARAM_INIT(
            0,
            BT_GAP_ADV_FAST_INT_MIN_2,
            BT_GAP_ADV_FAST_INT_MAX_2,
            NULL);
    }

    // Best-effort TX power: the vendor HCI command may not exist on all
    // controllers, so ignore failures here.
    (void)bleio_adapter_set_tx_power_noraise(tx_power);

    int err = bt_le_adv_start(&adv_params,
        adv_data,
        adv_count,
        scan_resp_count > 0 ? scan_resp_data : NULL,
        scan_resp_count);
    if (err) {
        return (uint32_t)(-err);
    }

    ble_advertising = true;
    // Default to workflow-owned; the public wrapper overrides this for user code.
    ble_advertising_internal = true;
    return 0;
}

void common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self,
    bool connectable, bool anonymous, uint32_t timeout, mp_float_t interval,
    mp_buffer_info_t *advertising_data_bufinfo,
    mp_buffer_info_t *scan_response_data_bufinfo,
    mp_int_t tx_power, const bleio_address_obj_t *directed_to) {
    (void)directed_to;
    (void)interval;

    if (advertising_data_bufinfo->len > BLEIO_ADV_MAX_DATA_LEN ||
        scan_response_data_bufinfo->len > BLEIO_ADV_MAX_DATA_LEN) {
        mp_raise_NotImplementedError(NULL);
    }

    if (timeout != 0) {
        mp_raise_NotImplementedError(NULL);
    }

    if (anonymous) {
        mp_raise_NotImplementedError(NULL);
    }

    if (ble_advertising) {
        if (!ble_advertising_internal) {
            // User code is already advertising.
            raise_zephyr_error(-EALREADY);
        }
        // The workflow is advertising. Stop it so user code can take over.
        common_hal_bleio_adapter_stop_advertising(self);
    }

    uint32_t status = _common_hal_bleio_adapter_start_advertising(self,
        connectable,
        anonymous,
        timeout,
        interval,
        advertising_data_bufinfo->buf,
        advertising_data_bufinfo->len,
        scan_response_data_bufinfo->buf,
        scan_response_data_bufinfo->len,
        tx_power,
        directed_to);
    if (status == (uint32_t)EINVAL) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid advertising data"));
    }
    if (status != 0) {
        raise_zephyr_error(-(int)status);
    }

    // Mark as user-owned so the workflow won't clobber it.
    ble_advertising_internal = false;
}

void common_hal_bleio_adapter_stop_advertising(bleio_adapter_obj_t *self) {
    (void)self;
    if (!ble_advertising) {
        return;
    }
    bt_le_adv_stop();
    ble_advertising = false;
    ble_advertising_internal = false;
}

bool common_hal_bleio_adapter_get_advertising(bleio_adapter_obj_t *self) {
    (void)self;
    return ble_advertising;
}

mp_obj_t common_hal_bleio_adapter_start_scan(bleio_adapter_obj_t *self, uint8_t *prefixes, size_t prefix_length, bool extended, mp_int_t buffer_size, mp_float_t timeout, mp_float_t interval, mp_float_t window, mp_int_t minimum_rssi, bool active) {
    (void)extended;

    if (self->scan_results != NULL) {
        if (!shared_module_bleio_scanresults_get_done(self->scan_results)) {
            common_hal_bleio_adapter_stop_scan(self);
        } else {
            self->scan_results = NULL;
        }
    }

    int err = 0;

    self->scan_results = shared_module_bleio_new_scanresults(buffer_size, prefixes, prefix_length, minimum_rssi);
    active_scan_results = self->scan_results;

    if (!scan_callbacks_registered) {
        scan_callbacks.recv = scan_recv_cb;
        scan_callbacks.timeout = scan_timeout_cb;
        err = bt_le_scan_cb_register(&scan_callbacks);
        if (err != 0) {
            self->scan_results = NULL;
            active_scan_results = NULL;
            raise_zephyr_error(err);
        }
        scan_callbacks_registered = true;
    }

    uint16_t interval_units = (uint16_t)((interval / 0.000625f) + 0.5f);
    uint16_t window_units = (uint16_t)((window / 0.000625f) + 0.5f);
    uint16_t timeout_units = bleio_validate_and_convert_timeout(timeout);

    struct bt_le_scan_param scan_params = {
        .type = active ? BT_LE_SCAN_TYPE_ACTIVE : BT_LE_SCAN_TYPE_PASSIVE,
        .options = BT_LE_SCAN_OPT_FILTER_DUPLICATE,
        .interval = interval_units,
        .window = window_units,
        .timeout = (uint16_t)timeout_units,
        .interval_coded = 0,
        .window_coded = 0,
    };

    err = bt_le_scan_start(&scan_params, NULL);
    if (err != 0) {
        self->scan_results = NULL;
        active_scan_results = NULL;
        raise_zephyr_error(err);
    }

    return MP_OBJ_FROM_PTR(self->scan_results);
}

void common_hal_bleio_adapter_stop_scan(bleio_adapter_obj_t *self) {
    if (self->scan_results == NULL) {
        return;
    }
    bt_le_scan_stop();
    shared_module_bleio_scanresults_set_done(self->scan_results, true);
    active_scan_results = NULL;
    self->scan_results = NULL;
}

bool common_hal_bleio_adapter_get_connected(bleio_adapter_obj_t *self) {
    if (!ble_adapter_enabled) {
        return false;
    }

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        if (bleio_connections[i].conn != NULL) {
            return true;
        }
    }

    return false;
}

mp_obj_t common_hal_bleio_adapter_get_connections(bleio_adapter_obj_t *self) {
    if (!ble_adapter_enabled) {
        self->connection_objs = NULL;
        return mp_const_empty_tuple;
    }

    if (self->connection_objs != NULL) {
        return self->connection_objs;
    }

    size_t total_connected = 0;
    mp_obj_t items[BLEIO_TOTAL_CONNECTION_COUNT];
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn == NULL) {
            continue;
        }

        if (connection->connection_obj == mp_const_none) {
            connection->connection_obj = bleio_connection_new_from_internal(connection);
        }

        items[total_connected] = connection->connection_obj;
        total_connected++;
    }

    self->connection_objs = mp_obj_new_tuple(total_connected, items);
    return self->connection_objs;
}

mp_obj_t common_hal_bleio_adapter_connect(bleio_adapter_obj_t *self, bleio_address_obj_t *address, mp_float_t timeout) {
    common_hal_bleio_adapter_stop_scan(self);

    const uint16_t timeout_units = bleio_validate_and_convert_timeout(timeout);

    mp_buffer_info_t address_bufinfo;
    mp_get_buffer_raise(address->bytes, &address_bufinfo, MP_BUFFER_READ);

    bt_addr_le_t peer = {
        .type = bleio_address_type_to_zephyr(address->type),
    };
    memcpy(peer.a.val, address_bufinfo.buf, NUM_BLEIO_ADDRESS_BYTES);

    struct bt_conn_le_create_param create_params = BT_CONN_LE_CREATE_PARAM_INIT(
        BT_CONN_LE_OPT_NONE,
        BT_GAP_SCAN_FAST_INTERVAL,
        BT_GAP_SCAN_FAST_INTERVAL);
    create_params.timeout = timeout_units;

    struct bt_conn *conn = NULL;
    int err = bt_conn_le_create(&peer, &create_params, BT_LE_CONN_PARAM_DEFAULT, &conn);
    if (err != 0) {
        raise_zephyr_error(err);
    }

    while (true) {
        struct bt_conn_info info;
        err = bt_conn_get_info(conn, &info);
        if (err == 0) {
            if (info.state == BT_CONN_STATE_CONNECTED) {
                break;
            }

            if (info.state == BT_CONN_STATE_DISCONNECTED) {
                bt_conn_unref(conn);
                mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Failed to connect: timeout"));
            }
        } else if (err != -ENOTCONN) {
            bt_conn_unref(conn);
            raise_zephyr_error(err);
        }

        RUN_BACKGROUND_TASKS;
    }

    bleio_connection_internal_t *connection = bleio_connection_find_by_conn(conn);
    if (connection == NULL) {
        connection = bleio_connection_track(conn);
    }

    if (connection == NULL) {
        bt_conn_unref(conn);
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Failed to connect: internal error"));
    }

    // bt_conn_le_create() gave us a ref in `conn`; `connection` keeps its own
    // ref via bleio_connection_track(). Drop the create ref now.
    bt_conn_unref(conn);

    self->connection_objs = NULL;
    return bleio_connection_new_from_internal(connection);
}

struct bond_collect_ctx {
    bt_addr_le_t *addrs;
    size_t *count;
    size_t max;
};

static void bond_iterator_collect(const struct bt_bond_info *info, void *user_data) {
    struct bond_collect_ctx *ctx = (struct bond_collect_ctx *)user_data;
    if (*ctx->count < ctx->max) {
        bt_addr_le_copy(&ctx->addrs[*ctx->count], &info->addr);
        (*ctx->count)++;
    }
}

static void bond_iterator_check(const struct bt_bond_info *info, void *user_data) {
    (void)info;
    bool *has_bonds = (bool *)user_data;
    *has_bonds = true;
}

void common_hal_bleio_adapter_erase_bonding(bleio_adapter_obj_t *self) {
    // Unpair all bonded devices for all local identities.
    for (uint8_t id = 0; id < CONFIG_BT_ID_MAX; id++) {
        // bt_unpair takes an addr; use bt_foreach_bond to iterate and unpair.
        // We need to collect addresses first since we can't unpair during iteration.
        bt_addr_le_t addrs[CONFIG_BT_MAX_PAIRED];
        size_t addr_count = 0;

        bt_foreach_bond(id, bond_iterator_collect, &(struct bond_collect_ctx) {
            .addrs = addrs,
            .count = &addr_count,
            .max = CONFIG_BT_MAX_PAIRED
        });

        for (size_t i = 0; i < addr_count; i++) {
            bt_unpair(id, &addrs[i]);
        }
    }

    // Reset pairing state on all connections
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connections[i].pair_status = PAIR_NOT_PAIRED;
        bleio_connections[i].sec_err = 0;
    }
}

bool common_hal_bleio_adapter_is_bonded_to_central(bleio_adapter_obj_t *self) {
    // Check if any bond exists for identity 0
    for (uint8_t id = 0; id < CONFIG_BT_ID_MAX; id++) {
        bool has_bonds = false;
        bt_foreach_bond(id, bond_iterator_check, &has_bonds);
        if (has_bonds) {
            return true;
        }
    }
    return false;
}

void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter) {
    gc_collect_root((void **)adapter, sizeof(bleio_adapter_obj_t) / sizeof(size_t));
    gc_collect_root((void **)bleio_connections, sizeof(bleio_connections) / sizeof(size_t));
}

void bleio_adapter_reset(bleio_adapter_obj_t *adapter) {
    if (adapter == NULL) {
        return;
    }

    common_hal_bleio_adapter_stop_scan(adapter);
    common_hal_bleio_adapter_stop_advertising(adapter);

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn != NULL) {
            bt_conn_disconnect(connection->conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        }
        if (connection->connection_obj != MP_OBJ_NULL &&
            connection->connection_obj != mp_const_none) {
            bleio_connection_obj_t *connection_obj = MP_OBJ_TO_PTR(connection->connection_obj);
            connection_obj->connection = NULL;
            connection_obj->disconnect_reason = BT_HCI_ERR_REMOTE_USER_TERM_CONN;
        }
        bleio_connection_clear(connection);
    }

    adapter->scan_results = NULL;
    adapter->connection_objs = NULL;
    active_scan_results = NULL;
    ble_advertising = false;
    ble_advertising_internal = false;
    ble_adapter_enabled = bt_is_ready();
}

bleio_adapter_obj_t *common_hal_bleio_allocate_adapter_or_raise(void) {
    return &common_hal_bleio_adapter_obj;
}

uint16_t bleio_adapter_get_name(char *buf, uint16_t len) {
    const char *name = bt_get_name();
    uint16_t full_len = strlen(name);
    if (len > 0) {
        uint16_t copy_len = len < full_len ? len : full_len;
        memcpy(buf, name, copy_len);
    }
    return full_len;
}
