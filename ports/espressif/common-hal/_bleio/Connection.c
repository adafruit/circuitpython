// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/_bleio/Connection.h"

#include <string.h>
#include <stdio.h>

#include "py/gc.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/qstr.h"
#include "py/runtime.h"

#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Attribute.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-bindings/time/__init__.h"

#include "supervisor/shared/tick.h"

#include "common-hal/_bleio/ble_events.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "host/ble_att.h"

// Defined in Adapter.c - we need these to trigger MTU exchange after
// encryption completes and to unblock the connect() wait loop.
extern volatile int _connection_status;
extern int _mtu_reply(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    uint16_t mtu, void *arg);

// Uncomment to turn on debug logging just in this file.
// #undef CIRCUITPY_VERBOSE_BLE
// #define CIRCUITPY_VERBOSE_BLE (1)

int bleio_connection_event_cb(struct ble_gap_event *event, void *connection_in) {
    bleio_connection_internal_t *connection = (bleio_connection_internal_t *)connection_in;

    switch (event->type) {
        case BLE_GAP_EVENT_DISCONNECT: {
            connection->conn_handle = BLEIO_HANDLE_INVALID;
            connection->pair_status = PAIR_NOT_PAIRED;

            #if CIRCUITPY_VERBOSE_BLE
            mp_printf(&mp_plat_print, "event->disconnect.reason: 0x%x\n", event->disconnect.reason);
            #endif

            if (connection->connection_obj != mp_const_none) {
                bleio_connection_obj_t *obj = connection->connection_obj;
                obj->connection = NULL;
                obj->disconnect_reason = event->disconnect.reason;
            }

            break;
        }

        case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE: {
            // Nothing to do here. CircuitPython doesn't tell the user what PHY
            // we're on.
            break;
        }

        case BLE_GAP_EVENT_CONN_UPDATE: {
            struct ble_gap_conn_desc desc;
            int rc __attribute__((unused)) = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
            assert(rc == 0);
            connection->conn_params_updating = false;
            break;
        }
        case BLE_GAP_EVENT_ENC_CHANGE: {
            struct ble_gap_conn_desc desc;
            ble_gap_conn_find(event->enc_change.conn_handle, &desc);
            if (desc.sec_state.encrypted) {
                connection->pair_status = PAIR_PAIRED;
            }
            // For central-role connections the MTU exchange was deferred
            // from _new_connection() so no unencrypted GATT traffic is sent
            // before pairing. Do it once encryption is settled.
            if (connection->is_central) {
                ble_gattc_exchange_mtu(event->enc_change.conn_handle,
                    _mtu_reply, connection);
            }
            break;
        }
        case BLE_GAP_EVENT_MTU: {
            if (event->mtu.conn_handle != connection->conn_handle) {
                return 0;
            }
            connection->mtu = event->mtu.value;
            break;
        }

        // These events are actually att specific so forward to all registered
        // handlers for them. The handlers themselves decide whether an event
        // is interesting to them.
        case BLE_GAP_EVENT_NOTIFY_RX:
            MP_FALLTHROUGH;
        case BLE_GAP_EVENT_NOTIFY_TX:
            MP_FALLTHROUGH;
        case BLE_GAP_EVENT_SUBSCRIBE:
            int status = ble_event_run_handlers(event);
            background_callback_add_core(&bleio_background_callback);
            return status;

        default:
            #if CIRCUITPY_VERBOSE_BLE
            mp_printf(&mp_plat_print, "Unhandled connection event: %d\n", event->type);
            #endif
            break;
    }

    background_callback_add_core(&bleio_background_callback);
    return 0;
}

bool common_hal_bleio_connection_get_paired(bleio_connection_obj_t *self) {
    if (self->connection == NULL) {
        return false;
    }
    return self->connection->pair_status == PAIR_PAIRED;
}

bool common_hal_bleio_connection_get_connected(bleio_connection_obj_t *self) {
    if (self->connection == NULL) {
        return false;
    }
    return self->connection->conn_handle != BLEIO_HANDLE_INVALID;
}

void common_hal_bleio_connection_disconnect(bleio_connection_internal_t *self) {
    // Second argument is an HCI reason, not an HS error code.
    ble_gap_terminate(self->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

void common_hal_bleio_connection_pair(bleio_connection_internal_t *self, bool bond) {
    // We may already be trying to pair if we just reconnected to a peer we're
    // bonded with.
    while (self->pair_status == PAIR_WAITING && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    if (self->pair_status == PAIR_PAIRED) {
        return;
    }
    self->pair_status = PAIR_WAITING;
    CHECK_NIMBLE_ERROR(ble_gap_security_initiate(self->conn_handle));
    while (self->pair_status == PAIR_WAITING && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    if (mp_hal_is_interrupted()) {
        return;
    }
}

mp_float_t common_hal_bleio_connection_get_connection_interval(bleio_connection_internal_t *self) {
    while (self->conn_params_updating && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    if (mp_hal_is_interrupted()) {
        return 0;
    }
    struct ble_gap_conn_desc desc;
    CHECK_NIMBLE_ERROR(ble_gap_conn_find(self->conn_handle, &desc));
    return 1.25f * desc.conn_itvl;
}

// Return the current negotiated MTU length, minus overhead.
mp_int_t common_hal_bleio_connection_get_max_packet_length(bleio_connection_internal_t *self) {
    return (self->mtu == 0 ? BLE_ATT_MTU_DFLT : self->mtu) - 3;
}

void common_hal_bleio_connection_set_connection_interval(bleio_connection_internal_t *self, mp_float_t new_interval) {
    self->conn_params_updating = true;
    struct ble_gap_conn_desc desc;
    CHECK_NIMBLE_ERROR(ble_gap_conn_find(self->conn_handle, &desc));
    uint16_t interval = new_interval / 1.25f;
    struct ble_gap_upd_params updated = {
        .itvl_min = interval,
        .itvl_max = interval,
        .latency = desc.conn_latency,
        .supervision_timeout = desc.supervision_timeout
    };
    CHECK_NIMBLE_ERROR(ble_gap_update_params(self->conn_handle, &updated));
}

// Zero when discovery is in process. BLE_HS_EDONE or a BLE_HS_ error code when done.
static volatile int _last_discovery_status;

// Give 20 seconds for each step of discovery: services, characteristics, attributes.
#define DISCOVERY_TIMEOUT_MS 20000

// Record result of last discovery step: services, characteristics, descriptors.
static void _set_discovery_step_status(int status) {
    _last_discovery_status = status;
}

static void _check_discovery_status(int status) {
    #if CIRCUITPY_VERBOSE_BLE
    mp_printf(&mp_plat_print, "discovery step status: %d\n", status);
    #endif

    if (status == BLE_HS_EDONE) {
        return;
    }
    if (status < BLE_HS_ERR_ATT_BASE) {
        CHECK_NIMBLE_ERROR(status);
        return;
    }
    CHECK_BLE_ERROR(status);
}

// Raw discovery results are staged here by the NimBLE host-task callbacks.
// Those callbacks run on the "nimble_host" task, NOT the VM task, so they must
// not allocate from the MicroPython heap: an allocation there can trigger a
// gc_collect() that scans the wrong task's stack and frees the VM's live
// objects. Instead the callbacks copy the plain NimBLE result struct into this
// FreeRTOS queue, and the VM task drains it and builds the Python objects.
//
// Discovery is serialized (one discover_remote_services() at a time), so a
// single file-static queue suffices. It must hold one ATT-PDU burst of records:
// within a response PDU the host task invokes the callback repeatedly without
// yielding, so the VM task cannot drain until the next PDU's round-trip. The
// largest burst is a find-information response at the maximum ATT MTU we
// advertise (CONFIG_BT_NIMBLE_ATT_PREFERRED_MTU, 256): about 64 descriptor
// records. The depth is twice that.
//
// The queue is created on first use from the FreeRTOS (IDF) heap, not the GC
// heap, so it needs no GC root and survives soft reloads: a stale procedure's
// sends always land in live memory. It is never deleted.

// One queue item, sized to the largest record type, so one queue can serve every
// discovery step.
typedef union {
    struct ble_gatt_svc svc;
    struct ble_gatt_chr chr;
    struct ble_gatt_dsc dsc;
} discovery_record_t;

#define DISCOVERY_QUEUE_DEPTH (128)
static QueueHandle_t _discovery_queue;

// Create the queue on first use, and empty it before a discovery step. Runs on
// the VM task. xQueueReset() is safe against a stale procedure from a previous
// errored, timed-out, or interrupted discovery that may still be sending
// records on the nimble_host task.
static void _reset_discovery_queue(void) {
    if (_discovery_queue == NULL) {
        _discovery_queue = xQueueCreate(DISCOVERY_QUEUE_DEPTH, sizeof(discovery_record_t));
        if (_discovery_queue == NULL) {
            m_malloc_fail(DISCOVERY_QUEUE_DEPTH * sizeof(discovery_record_t));
        }
    }
    xQueueReset(_discovery_queue);
}

static int _discovered_service_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    const struct ble_gatt_svc *svc,
    void *arg) {
    if (error->status != 0) {
        #if CIRCUITPY_VERBOSE_BLE
        mp_printf(&mp_plat_print, "_discovered_service_cb error->status: %d, handle: %d\n",
            error->status, error->att_handle);
        #endif

        // BLE_HS_EDONE or some error has occurred.
        _set_discovery_step_status(error->status);
        return 0;
    }

    // Runs on the nimble_host task: stage the raw result only, never allocate.
    discovery_record_t record = { .svc = *svc };
    if (xQueueSend(_discovery_queue, &record, 0) != pdTRUE) {
        _set_discovery_step_status(BLE_HS_ENOMEM);
    }
    return 0;
}

static int _discovered_characteristic_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    const struct ble_gatt_chr *chr,
    void *arg) {
    if (error->status != 0) {
        #if CIRCUITPY_VERBOSE_BLE
        mp_printf(&mp_plat_print, "_discovered_characteristic_cb error->status: %d, handle: %d\n",
            error->status, error->att_handle);
        #endif

        // BLE_HS_EDONE or some error has occurred.
        _set_discovery_step_status(error->status);
        return 0;
    }

    // Runs on the nimble_host task: stage the raw result only, never allocate.
    discovery_record_t record = { .chr = *chr };
    if (xQueueSend(_discovery_queue, &record, 0) != pdTRUE) {
        _set_discovery_step_status(BLE_HS_ENOMEM);
    }
    return 0;
}

static int _discovered_descriptor_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    uint16_t chr_val_handle,
    const struct ble_gatt_dsc *dsc,
    void *arg) {
    if (error->status != 0) {
        #if CIRCUITPY_VERBOSE_BLE
        mp_printf(&mp_plat_print, "_discovered_descriptor_cb error->status: %d, handle: %d\n",
            error->status, error->att_handle);
        #endif

        // BLE_HS_EDONE or some error has occurred.
        _set_discovery_step_status(error->status);
        return 0;
    }

    // Runs on the nimble_host task: stage the raw result only, never allocate.
    discovery_record_t record = { .dsc = *dsc };
    if (xQueueSend(_discovery_queue, &record, 0) != pdTRUE) {
        _set_discovery_step_status(BLE_HS_ENOMEM);
    }
    return 0;
}

// Build a Service object from a staged raw record. Runs on the VM task.
static void _build_service(void *ctx, const void *record) {
    bleio_connection_internal_t *self = ctx;
    const struct ble_gatt_svc *svc = record;

    bleio_service_obj_t *service = mp_obj_malloc(bleio_service_obj_t, &bleio_service_type);

    // Initialize several fields at once.
    bleio_service_from_connection(service, bleio_connection_new_from_internal(self));

    service->is_remote = true;
    service->start_handle = svc->start_handle;
    service->end_handle = svc->end_handle;
    service->handle = svc->start_handle;

    bleio_uuid_obj_t *uuid = mp_obj_malloc(bleio_uuid_obj_t, &bleio_uuid_type);

    uuid->nimble_ble_uuid = svc->uuid;
    service->uuid = uuid;

    mp_obj_list_append(MP_OBJ_FROM_PTR(self->remote_service_list),
        MP_OBJ_FROM_PTR(service));
}

// Drain and build all staged records on the VM task, until the step completes
// and the queue is empty. build() is invoked for each raw record with ctx
// passed through. Returns the discovery step status.
static int _drain_records(void *ctx,
    void (*build)(void *ctx, const void *record)) {
    const uint64_t timeout_time_ms = common_hal_time_monotonic_ms() + DISCOVERY_TIMEOUT_MS;
    discovery_record_t record;
    while (true) {
        if (xQueueReceive(_discovery_queue, &record, 0) == pdTRUE) {
            build(ctx, &record);
            continue;
        }
        // Queue is empty.
        if (_last_discovery_status != 0) {
            // On EDONE or a NimBLE error, the terminal callback has already
            // run, so no more records will arrive: drain any that raced in,
            // then stop. On ENOMEM (our own send failure) the procedure may
            // still be running, but we are raising anyway; any later
            // discovery resets the queue before reuse.
            while (xQueueReceive(_discovery_queue, &record, 0) == pdTRUE) {
                build(ctx, &record);
            }
            return _last_discovery_status;
        }
        if (common_hal_time_monotonic_ms() >= timeout_time_ms) {
            return BLE_HS_ETIMEOUT;
        }
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            // Return prematurely. Then the interrupt will be raised.
            _set_discovery_step_status(BLE_HS_EDONE);
        }
    }
}

// Build a Characteristic object from a staged raw record. Runs on the VM task.
static void _build_characteristic(void *ctx, const void *record) {
    bleio_service_obj_t *service = ctx;
    const struct ble_gatt_chr *chr = record;

    bleio_characteristic_obj_t *characteristic =
        mp_obj_malloc(bleio_characteristic_obj_t, &bleio_characteristic_type);

    // Known characteristic UUID.
    bleio_uuid_obj_t *uuid = mp_obj_malloc(bleio_uuid_obj_t, &bleio_uuid_type);
    uuid->nimble_ble_uuid = chr->uuid;

    bleio_characteristic_properties_t props =
        ((chr->properties & BLE_GATT_CHR_PROP_BROADCAST) != 0 ? CHAR_PROP_BROADCAST : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_INDICATE) != 0 ? CHAR_PROP_INDICATE : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_NOTIFY) != 0 ? CHAR_PROP_NOTIFY : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_READ) != 0 ? CHAR_PROP_READ : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_WRITE) != 0 ? CHAR_PROP_WRITE : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_WRITE_NO_RSP) != 0 ? CHAR_PROP_WRITE_NO_RESPONSE : 0);

    // Call common_hal_bleio_characteristic_construct() to initialize some fields and set up evt handler.
    mp_buffer_info_t mp_const_empty_bytes_bufinfo;
    mp_get_buffer_raise(mp_const_empty_bytes, &mp_const_empty_bytes_bufinfo, MP_BUFFER_READ);

    common_hal_bleio_characteristic_construct(
        characteristic, service, chr->val_handle, uuid,
        props, SECURITY_MODE_OPEN, SECURITY_MODE_OPEN,
        GATT_MAX_DATA_LENGTH, false,   // max_length, fixed_length: values don't matter for gattc, but don't use 0
        &mp_const_empty_bytes_bufinfo,
        NULL);
    // Set def_handle directly since it is only used in discovery.
    characteristic->def_handle = chr->def_handle;

    #if CIRCUITPY_VERBOSE_BLE
    mp_printf(&mp_plat_print, "_build_characteristic: char handle: %d\n", characteristic->handle);
    #endif

    mp_obj_list_append(MP_OBJ_FROM_PTR(service->characteristic_list),
        MP_OBJ_FROM_PTR(characteristic));
}

// Build a Descriptor object from a staged raw record. Runs on the VM task.
static void _build_descriptor(void *ctx, const void *record) {
    bleio_characteristic_obj_t *characteristic = ctx;
    const struct ble_gatt_dsc *dsc = record;

    // Remember handles for certain well-known descriptors.
    switch (dsc->uuid.u16.value) {
        case 0x2902:
            characteristic->cccd_handle = dsc->handle;
            break;

        case 0x2903:
            characteristic->sccd_handle = dsc->handle;
            break;

        case 0x2901:
            characteristic->user_desc_handle = dsc->handle;
            break;

        default:
            break;
    }

    bleio_descriptor_obj_t *descriptor = mp_obj_malloc(bleio_descriptor_obj_t, &bleio_descriptor_type);

    bleio_uuid_obj_t *uuid = mp_obj_malloc(bleio_uuid_obj_t, &bleio_uuid_type);
    uuid->nimble_ble_uuid = dsc->uuid;
    common_hal_bleio_descriptor_construct(
        descriptor, characteristic, uuid,
        SECURITY_MODE_OPEN, SECURITY_MODE_OPEN,
        GATT_MAX_DATA_LENGTH, false, mp_const_empty_bytes);
    descriptor->handle = dsc->handle;

    #if CIRCUITPY_VERBOSE_BLE
    mp_printf(&mp_plat_print, "_build_descriptor: char handle: %d, desc handle: %d, uuid type: %d, u16 value: 0x%x\n",
        characteristic->handle, descriptor->handle, dsc->uuid.u.type, dsc->uuid.u16.value);
    #endif

    mp_obj_list_append(MP_OBJ_FROM_PTR(characteristic->descriptor_list),
        MP_OBJ_FROM_PTR(descriptor));
}

static void discover_remote_services(bleio_connection_internal_t *self, mp_obj_t service_uuids_whitelist) {
    // Start over with an empty list.
    self->remote_service_list = mp_obj_new_list(0, NULL);

    if (service_uuids_whitelist == mp_const_none) {
        // Reset discovery status and staging queue before starting callbacks.
        _set_discovery_step_status(0);
        _reset_discovery_queue();

        CHECK_NIMBLE_ERROR(ble_gattc_disc_all_svcs(self->conn_handle, _discovered_service_cb, self));

        // Drain staged services and build them on the VM task until done.
        int status = _drain_records(self, _build_service);
        _check_discovery_status(status);
    } else {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(service_uuids_whitelist, &iter_buf);
        mp_obj_t uuid_obj;
        while ((uuid_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if (!mp_obj_is_type(uuid_obj, &bleio_uuid_type)) {
                mp_raise_TypeError(MP_ERROR_TEXT("non-UUID found in service_uuids_whitelist"));
            }
            bleio_uuid_obj_t *uuid = MP_OBJ_TO_PTR(uuid_obj);

            // Reset discovery status and staging queue before starting callbacks.
            _set_discovery_step_status(0);
            _reset_discovery_queue();

            CHECK_NIMBLE_ERROR(ble_gattc_disc_svc_by_uuid(self->conn_handle, &uuid->nimble_ble_uuid.u,
                _discovered_service_cb, self));

            // Drain staged services and build them on the VM task until done.
            int status = _drain_records(self, _build_service);
            _check_discovery_status(status);
        }
    }

    // Now discover characteristics for each discovered service.
    for (size_t i = 0; i < self->remote_service_list->len; i++) {
        bleio_service_obj_t *service = MP_OBJ_TO_PTR(self->remote_service_list->items[i]);

        // Reset discovery status and staging queue before starting callbacks.
        _set_discovery_step_status(0);
        _reset_discovery_queue();

        CHECK_NIMBLE_ERROR(ble_gattc_disc_all_chrs(self->conn_handle,
            service->start_handle,
            service->end_handle,
            _discovered_characteristic_cb,
            service));

        // Drain staged characteristics and build them on the VM task until done.
        int status = _drain_records(service, _build_characteristic);
        _check_discovery_status(status);

        // Got characteristics for this service. Now discover descriptors for each characteristic.
        size_t char_list_len = service->characteristic_list->len;
        for (size_t char_idx = 0; char_idx < char_list_len; ++char_idx) {
            bleio_characteristic_obj_t *characteristic =
                MP_OBJ_TO_PTR(service->characteristic_list->items[char_idx]);
            // Determine the handle range for the given characteristic's descriptors.
            // The end of the range is dictated by the next characteristic or the end
            // handle of the service.
            const bool last_characteristic = char_idx == char_list_len - 1;
            bleio_characteristic_obj_t *next_characteristic = last_characteristic
                ? NULL
                : MP_OBJ_TO_PTR(service->characteristic_list->items[char_idx + 1]);

            uint16_t end_handle = next_characteristic == NULL
                ? service->end_handle
                : next_characteristic->def_handle - 1;

            // Pre-check if there are no descriptors to discover so descriptor discovery doesn't fail
            if (end_handle <= characteristic->handle) {
                continue;
            }

            // Reset discovery status and staging queue before starting callbacks.
            _set_discovery_step_status(0);
            _reset_discovery_queue();

            // The descriptor handle inclusive range is [characteristic->handle + 1, end_handle],
            // but ble_gattc_disc_all_dscs() requires starting with characteristic->handle.
            CHECK_NIMBLE_ERROR(ble_gattc_disc_all_dscs(self->conn_handle, characteristic->handle,
                end_handle,
                _discovered_descriptor_cb, characteristic));

            // Drain staged descriptors and build them on the VM task until done.
            status = _drain_records(characteristic, _build_descriptor);
            _check_discovery_status(status);
        }
    }
}

mp_obj_tuple_t *common_hal_bleio_connection_discover_remote_services(bleio_connection_obj_t *self, mp_obj_t service_uuids_whitelist) {
    discover_remote_services(self->connection, service_uuids_whitelist);
    bleio_connection_ensure_connected(self);
    // Convert to a tuple and then clear the list so the callee will take ownership.
    mp_obj_tuple_t *services_tuple =
        mp_obj_new_tuple(self->connection->remote_service_list->len,
            self->connection->remote_service_list->items);
    mp_obj_list_clear(MP_OBJ_FROM_PTR(self->connection->remote_service_list));

    return services_tuple;
}

uint16_t bleio_connection_get_conn_handle(bleio_connection_obj_t *self) {
    if (self == NULL || self->connection == NULL) {
        return BLEIO_HANDLE_INVALID;
    }
    return self->connection->conn_handle;
}

mp_obj_t bleio_connection_new_from_internal(bleio_connection_internal_t *internal) {
    if (internal->connection_obj != mp_const_none) {
        return internal->connection_obj;
    }
    bleio_connection_obj_t *connection = mp_obj_malloc(bleio_connection_obj_t, &bleio_connection_type);

    connection->connection = internal;
    internal->connection_obj = connection;

    return MP_OBJ_FROM_PTR(connection);
}

// Find the connection that uses the given conn_handle. Return NULL if not found.
bleio_connection_internal_t *bleio_conn_handle_to_connection(uint16_t conn_handle) {
    bleio_connection_internal_t *connection;
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        connection = &bleio_connections[i];
        if (connection->conn_handle == conn_handle) {
            return connection;
        }
    }

    return NULL;
}
