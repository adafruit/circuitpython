// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/slist.h>

#include "py/runtime.h"
#include "py/gc.h"
#include "bindings/zephyr_kernel/__init__.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "common-hal/_bleio/__init__.h"
#include "common-hal/_bleio/CharacteristicBuffer.h"
#include "common-hal/_bleio/Connection.h"
#include "common-hal/_bleio/UUID.h"
#include "common-hal/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "supervisor/port_heap.h"

// CCCD write callback — captures the subscribing connection for PacketBuffer.
ssize_t bleio_ccc_write_cb(struct bt_conn *conn,
    const struct bt_gatt_attr *attr, uint16_t value) {
    struct bt_gatt_ccc_managed_user_data *ccc_data =
        (struct bt_gatt_ccc_managed_user_data *)attr->user_data;
    bleio_characteristic_obj_t *characteristic =
        (bleio_characteristic_obj_t *)((char *)ccc_data
            - offsetof(bleio_characteristic_obj_t, zephyr_ccc));

    if (characteristic->observer != mp_const_none &&
        mp_obj_is_type(characteristic->observer, &bleio_packet_buffer_type)) {
        bleio_packet_buffer_set_conn(
            MP_OBJ_TO_PTR(characteristic->observer),
            value != 0 ? conn : NULL);
    }
    return sizeof(value);
}

uint16_t bleio_security_to_zephyr_perm(
    bleio_attribute_security_mode_t read_perm,
    bleio_attribute_security_mode_t write_perm,
    bleio_characteristic_properties_t props) {
    uint16_t perm = 0;

    if (props & CHAR_PROP_READ) {
        switch (read_perm) {
            case SECURITY_MODE_OPEN:
                perm |= BT_GATT_PERM_READ;
                break;
            case SECURITY_MODE_ENC_NO_MITM:
                perm |= BT_GATT_PERM_READ_ENCRYPT;
                break;
            case SECURITY_MODE_ENC_WITH_MITM:
                perm |= BT_GATT_PERM_READ_AUTHEN;
                break;
            case SECURITY_MODE_LESC_ENC_WITH_MITM:
                perm |= BT_GATT_PERM_READ_LESC;
                break;
            default:
                break;
        }
    }

    if (props & (CHAR_PROP_WRITE | CHAR_PROP_WRITE_NO_RESPONSE)) {
        switch (write_perm) {
            case SECURITY_MODE_OPEN:
                perm |= BT_GATT_PERM_WRITE;
                break;
            case SECURITY_MODE_ENC_NO_MITM:
                perm |= BT_GATT_PERM_WRITE_ENCRYPT;
                break;
            case SECURITY_MODE_ENC_WITH_MITM:
                perm |= BT_GATT_PERM_WRITE_AUTHEN;
                break;
            case SECURITY_MODE_LESC_ENC_WITH_MITM:
                perm |= BT_GATT_PERM_WRITE_LESC;
                break;
            default:
                break;
        }
    }

    return perm;
}

ssize_t bleio_char_read_cb(struct bt_conn *conn,
    const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset) {
    bleio_characteristic_obj_t *self = attr->user_data;
    (void)conn;
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
        self->current_value, self->current_value_len);
}

ssize_t bleio_char_write_cb(struct bt_conn *conn,
    const struct bt_gatt_attr *attr, const void *buf, uint16_t len,
    uint16_t offset, uint8_t flags) {
    bleio_characteristic_obj_t *self = attr->user_data;
    (void)flags;
    if (offset + len > self->max_length) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    memcpy(self->current_value + offset, buf, len);
    if (offset + len > self->current_value_len) {
        self->current_value_len = offset + len;
    }

    // Notify any observer (e.g., CharacteristicBuffer, PacketBuffer) of the write.
    if (self->observer != mp_const_none) {
        if (mp_obj_is_type(self->observer, &bleio_characteristic_buffer_type)) {
            bleio_characteristic_buffer_extend(MP_OBJ_TO_PTR(self->observer), buf, len);
        } else if (mp_obj_is_type(self->observer, &bleio_packet_buffer_type)) {
            bleio_packet_buffer_extend(MP_OBJ_TO_PTR(self->observer), conn, buf, len);
        }
    }

    return len;
}

void bleio_ccc_changed_cb(const struct bt_gatt_attr *attr, uint16_t value) {
    // Track subscription state if needed in the future.
    (void)attr;
    (void)value;
}

bleio_characteristic_properties_t common_hal_bleio_characteristic_get_properties(bleio_characteristic_obj_t *self) {
    return self->props;
}

mp_obj_tuple_t *common_hal_bleio_characteristic_get_descriptors(bleio_characteristic_obj_t *self) {
    if (self->descriptor_list == NULL) {
        return mp_const_empty_tuple;
    }
    return mp_obj_new_tuple(self->descriptor_list->len, self->descriptor_list->items);
}

bleio_service_obj_t *common_hal_bleio_characteristic_get_service(bleio_characteristic_obj_t *self) {
    return self->service;
}

bleio_uuid_obj_t *common_hal_bleio_characteristic_get_uuid(bleio_characteristic_obj_t *self) {
    return self->uuid;
}

size_t common_hal_bleio_characteristic_get_max_length(bleio_characteristic_obj_t *self) {
    return self->max_length;
}

size_t common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self, uint8_t *buf, size_t len) {
    if (self->service != NULL && self->service->is_remote) {
        // Remote characteristic: read via GATT client
        bleio_connection_obj_t *connection = MP_OBJ_TO_PTR(self->service->connection);
        if (connection == NULL || connection->connection == NULL ||
            connection->connection->conn == NULL) {
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Not connected"));
        }

        return bleio_gattc_read_sync(connection->connection->conn,
            self->handle, buf, len);
    }

    // Local characteristic
    size_t copy_len = self->current_value_len;
    if (copy_len > len) {
        copy_len = len;
    }
    memcpy(buf, self->current_value, copy_len);
    return copy_len;
}

void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self,
    bleio_service_obj_t *service, uint16_t handle, bleio_uuid_obj_t *uuid,
    bleio_characteristic_properties_t props,
    bleio_attribute_security_mode_t read_perm,
    bleio_attribute_security_mode_t write_perm,
    mp_int_t max_length, bool fixed_length,
    mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {

    self->service = service;
    self->uuid = uuid;
    self->handle = handle;
    self->props = props;
    self->read_perm = read_perm;
    self->write_perm = write_perm;
    self->max_length = max_length;
    self->fixed_length = fixed_length;
    self->observer = mp_const_none;
    // The descriptor list is an mp_obj (GC object). When constructed before
    // gc_init() (e.g. the BLE workflow at boot), the GC heap isn't available,
    // so leave it NULL and lazily create it on first use. Matches the nordic
    // port's handling.
    if (gc_alloc_possible()) {
        self->descriptor_list = mp_obj_new_list(0, NULL);
    } else {
        self->descriptor_list = NULL;
    }

    // Allocate value buffer
    self->current_value = port_malloc(max_length, false);
    memset(self->current_value, 0, max_length);
    self->current_value_alloc = max_length;
    self->current_value_len = 0;

    // Copy initial value if provided
    if (initial_value_bufinfo != NULL && initial_value_bufinfo->len > 0) {
        size_t len = initial_value_bufinfo->len;
        if (len > (size_t)max_length) {
            len = max_length;
        }
        memcpy(self->current_value, initial_value_bufinfo->buf, len);
        self->current_value_len = len;
    }

    // Convert UUID to Zephyr format
    bleio_uuid_to_zephyr(uuid, &self->zephyr_uuid);

    if (service->is_remote) {
        // Remote characteristic: just add to the service's list
        mp_obj_list_append(MP_OBJ_FROM_PTR(service->characteristic_list),
            MP_OBJ_FROM_PTR(self));
    } else {
        common_hal_bleio_service_add_characteristic(service, self,
            initial_value_bufinfo, user_description);

        // Create a Descriptor object for user_description (CUD 0x2901)
        if (user_description != NULL && user_description[0] != '\0') {
            bleio_uuid_obj_t *desc_uuid = mp_obj_malloc(bleio_uuid_obj_t, &bleio_uuid_type);
            common_hal_bleio_uuid_construct(desc_uuid, 0x2901, NULL);

            bleio_descriptor_obj_t *descriptor = mp_obj_malloc(bleio_descriptor_obj_t, &bleio_descriptor_type);

            size_t desc_len = strlen(user_description);
            mp_buffer_info_t desc_bufinfo = {
                .buf = (void *)user_description,
                .len = desc_len,
            };

            common_hal_bleio_descriptor_construct(
                descriptor, self, desc_uuid,
                SECURITY_MODE_OPEN, SECURITY_MODE_OPEN,
                desc_len, false, &desc_bufinfo);

            common_hal_bleio_characteristic_add_descriptor(self, descriptor);
        }
    }
}

bool common_hal_bleio_characteristic_deinited(bleio_characteristic_obj_t *self) {
    return self->service == NULL;
}

void common_hal_bleio_characteristic_deinit(bleio_characteristic_obj_t *self) {
    // Nothing to do - service handles unregistration
    if (self->current_value != NULL) {
        port_free(self->current_value);
        self->current_value = NULL;
        self->current_value_alloc = 0;
        self->current_value_len = 0;
    }
}

// Struct for tracking GATT notification subscriptions on remote characteristics.
typedef struct {
    struct bt_gatt_subscribe_params params;
    uint16_t value_handle;
    uint16_t ccc_handle;
    bleio_characteristic_obj_t *characteristic;
    volatile bool subscribed;
} zephyr_subscription_t;

static uint8_t on_gattc_notify(struct bt_conn *conn,
    struct bt_gatt_subscribe_params *params,
    const void *data, uint16_t length) {
    zephyr_subscription_t *sub = CONTAINER_OF(params, zephyr_subscription_t, params);
    if (sub->characteristic != NULL &&
        sub->characteristic->observer != mp_const_none) {
        if (mp_obj_is_type(sub->characteristic->observer, &bleio_characteristic_buffer_type)) {
            bleio_characteristic_buffer_extend(
                MP_OBJ_TO_PTR(sub->characteristic->observer), data, length);
        } else if (mp_obj_is_type(sub->characteristic->observer, &bleio_packet_buffer_type)) {
            bleio_packet_buffer_extend(
                MP_OBJ_TO_PTR(sub->characteristic->observer), conn, data, length);
        }
    }
    return BT_GATT_ITER_CONTINUE;
}

void common_hal_bleio_characteristic_set_cccd(bleio_characteristic_obj_t *self, bool notify, bool indicate) {
    // Only valid for remote characteristics (client-side).
    if (self->service == NULL || !self->service->is_remote) {
        return;
    }

    bleio_connection_obj_t *connection_obj = MP_OBJ_TO_PTR(self->service->connection);
    if (connection_obj == NULL || connection_obj->connection == NULL ||
        connection_obj->connection->conn == NULL) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Not connected"));
    }

    struct bt_conn *conn = connection_obj->connection->conn;

    if (!notify && !indicate) {
        // Unsubscribe from any existing subscription.
        // We don't track subscriptions per-characteristic yet,
        // so just return for now.
        return;
    }

    if (self->cccd_handle == 0) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("No CCCD for this Characteristic"));
    }

    // Allocate subscription tracking from port heap (won't move on GC).
    // Note: Simplified - we allocate a new subscription each time.
    // A production implementation would track and reuse subscriptions.
    zephyr_subscription_t *sub = port_malloc(sizeof(zephyr_subscription_t), false);
    if (sub == NULL) {
        mp_raise_msg(&mp_type_MemoryError, NULL);
    }

    // Zero-initialize the entire structure so that unset fields
    // (subscribe, flags, discover_params, work_q, node) are NULL/0
    // rather than garbage from the heap. bt_gatt_subscribe checks
    // subscribe==NULL to decide whether to do a synchronous write.
    memset(sub, 0, sizeof(zephyr_subscription_t));

    sub->characteristic = self;
    sub->value_handle = self->handle;
    sub->ccc_handle = self->cccd_handle;
    sub->subscribed = false;

    sub->params.notify = on_gattc_notify;
    sub->params.value_handle = self->handle;
    sub->params.ccc_handle = self->cccd_handle;
    sub->params.value = notify ? BT_GATT_CCC_NOTIFY : BT_GATT_CCC_INDICATE;

    int err = bt_gatt_subscribe(conn, &sub->params);
    if (err != 0) {
        port_free(sub);
        raise_zephyr_error(err);
    }
    sub->subscribed = true;
}

void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    if (self->service != NULL && self->service->is_remote) {
        // Remote characteristic: write via GATT client
        bleio_connection_obj_t *connection = MP_OBJ_TO_PTR(self->service->connection);
        if (connection == NULL || connection->connection == NULL ||
            connection->connection->conn == NULL) {
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Not connected"));
        }

        if (self->props & CHAR_PROP_WRITE_NO_RESPONSE) {
            int err = bt_gatt_write_without_response(
                connection->connection->conn,
                self->handle,
                bufinfo->buf, bufinfo->len, false);
            if (err != 0) {
                raise_zephyr_error(err);
            }
        } else {
            bleio_gattc_write_sync(connection->connection->conn,
                self->handle, bufinfo->buf, bufinfo->len);
        }
        return;
    }

    // Local characteristic
    size_t len = bufinfo->len;
    if (len > self->max_length) {
        len = self->max_length;
    }
    memcpy(self->current_value, bufinfo->buf, len);
    self->current_value_len = len;

    // If NOTIFY and service is registered, send notification
    if ((self->props & CHAR_PROP_NOTIFY) && self->service != NULL &&
        self->service->registered) {
        bt_gatt_notify(NULL, &self->service->attrs[self->value_attr_index],
            self->current_value, self->current_value_len);
    }
}

void common_hal_bleio_characteristic_add_descriptor(bleio_characteristic_obj_t *self,
    bleio_descriptor_obj_t *descriptor) {
    if (self->descriptor_list == NULL) {
        self->descriptor_list = mp_obj_new_list(0, NULL);
    }
    mp_obj_list_append(MP_OBJ_FROM_PTR(self->descriptor_list),
        MP_OBJ_FROM_PTR(descriptor));
    // Descriptors added after characteristic construction would need
    // service re-registration; for now the common case is handled by
    // Service.add_characteristic which adds descriptors at registration time.
}

void bleio_characteristic_set_observer(bleio_characteristic_obj_t *self, mp_obj_t observer) {
    self->observer = observer;
}

void bleio_characteristic_clear_observer(bleio_characteristic_obj_t *self) {
    self->observer = mp_const_none;
}
