// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// GATT server service implementation for Zephyr.
//
// Each bleio Service owns a dynamically-allocated array of bt_gatt_attr
// (the Zephyr GATT attribute table).  When a Characteristic is added, we
// append 2-4 attrs (declaration, value, optional CCC, optional CUD) and
// (re-)register the service with bt_gatt_service_register().
//
// IMPORTANT: Zephyr's BT_UUID_GATT_PRIMARY / BT_UUID_GATT_CHRC / … macros
// expand to compound-literal pointers.  Inside a function those have automatic
// storage duration and become dangling once the function returns.  We therefore
// declare file-scope static const UUIDs (_uuid_primary, _uuid_chrc, …) and
// reference those from every bt_gatt_attr we build.

#include <string.h>
#include <stdlib.h>

#include <zephyr/bluetooth/gatt.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "supervisor/port_heap.h"
#include "bindings/zephyr_kernel/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"
#include "common-hal/_bleio/__init__.h"
#include "common-hal/_bleio/Characteristic.h"

#define INITIAL_ATTR_CAPACITY 16

// Standard GATT UUIDs with static storage duration.
// BT_UUID_GATT_PRIMARY etc. expand to compound literals which have automatic
// storage when used inside functions - the resulting pointers dangle after the
// function returns. These static constants persist for the lifetime of the
// program.
static const struct bt_uuid_16 _uuid_primary = BT_UUID_INIT_16(BT_UUID_GATT_PRIMARY_VAL);
static const struct bt_uuid_16 _uuid_secondary = BT_UUID_INIT_16(BT_UUID_GATT_SECONDARY_VAL);
static const struct bt_uuid_16 _uuid_chrc = BT_UUID_INIT_16(BT_UUID_GATT_CHRC_VAL);
static const struct bt_uuid_16 _uuid_ccc = BT_UUID_INIT_16(BT_UUID_GATT_CCC_VAL);
static const struct bt_uuid_16 _uuid_cud = BT_UUID_INIT_16(BT_UUID_GATT_CUD_VAL);

static void service_ensure_capacity(bleio_service_obj_t *self, size_t needed) {
    if (self->attr_count + needed <= self->attr_capacity) {
        return;
    }
    size_t new_capacity = self->attr_capacity;
    while (new_capacity < self->attr_count + needed) {
        new_capacity *= 2;
    }
    struct bt_gatt_attr *new_attrs = port_realloc(self->attrs,
        new_capacity * sizeof(struct bt_gatt_attr), false);
    self->attrs = new_attrs;
    self->attr_capacity = new_capacity;
}

uint32_t _common_hal_bleio_service_construct(bleio_service_obj_t *self,
    bleio_uuid_obj_t *uuid, bool is_secondary,
    mp_obj_list_t *characteristic_list) {
    self->uuid = uuid;
    self->is_secondary = is_secondary;
    self->is_remote = false;
    self->connection = mp_const_none;
    self->characteristic_list = characteristic_list;
    self->start_handle = 0;
    self->end_handle = 0;
    self->registered = false;

    // Convert UUID to Zephyr format
    bleio_uuid_to_zephyr(uuid, &self->zephyr_uuid);

    // Allocate attrs array
    self->attr_capacity = INITIAL_ATTR_CAPACITY;
    self->attrs = port_malloc(self->attr_capacity * sizeof(struct bt_gatt_attr), false);
    memset(self->attrs, 0, self->attr_capacity * sizeof(struct bt_gatt_attr));
    self->attr_count = 0;

    // Add primary/secondary service declaration at index 0
    const struct bt_uuid *svc_type_uuid = is_secondary
        ? (const struct bt_uuid *)&_uuid_secondary
        : (const struct bt_uuid *)&_uuid_primary;
    self->attrs[0] = (struct bt_gatt_attr) {
        .uuid = svc_type_uuid,
        .perm = BT_GATT_PERM_READ,
        .read = bt_gatt_attr_read_service,
        .user_data = (void *)&self->zephyr_uuid,
    };
    self->attr_count = 1;

    return 0;
}

void common_hal_bleio_service_construct(bleio_service_obj_t *self,
    bleio_uuid_obj_t *uuid, bool is_secondary) {
    mp_obj_list_t *char_list = mp_obj_new_list(0, NULL);
    _common_hal_bleio_service_construct(self, uuid, is_secondary, char_list);
}

void common_hal_bleio_service_deinit(bleio_service_obj_t *self) {
    if (self->registered) {
        bt_gatt_service_unregister(&self->zephyr_service);
        self->registered = false;
    }
    if (self->attrs != NULL) {
        port_free(self->attrs);
        self->attrs = NULL;
        self->attr_capacity = 0;
        self->attr_count = 0;
    }
}

void common_hal_bleio_service_from_remote_service(bleio_service_obj_t *self,
    bleio_connection_obj_t *connection, bleio_uuid_obj_t *uuid, bool is_secondary) {
    self->uuid = uuid;
    self->is_secondary = is_secondary;
    self->is_remote = true;
    self->connection = MP_OBJ_FROM_PTR(connection);
    self->characteristic_list = mp_obj_new_list(0, NULL);
    self->start_handle = 0;
    self->end_handle = 0;
    self->registered = false;
    self->attrs = NULL;
    self->attr_count = 0;
    self->attr_capacity = 0;
    bleio_uuid_to_zephyr(uuid, &self->zephyr_uuid);
}

bleio_uuid_obj_t *common_hal_bleio_service_get_uuid(bleio_service_obj_t *self) {
    return self->uuid;
}

mp_obj_tuple_t *common_hal_bleio_service_get_characteristics(bleio_service_obj_t *self) {
    return mp_obj_new_tuple(self->characteristic_list->len, self->characteristic_list->items);
}

bool common_hal_bleio_service_get_is_remote(bleio_service_obj_t *self) {
    return self->is_remote;
}

bool common_hal_bleio_service_get_is_secondary(bleio_service_obj_t *self) {
    return self->is_secondary;
}

void common_hal_bleio_service_add_characteristic(bleio_service_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {

    if (self->registered) {
        bt_gatt_service_unregister(&self->zephyr_service);
        self->registered = false;
    }

    // Calculate how many attrs we need:
    // 1 = characteristic declaration, 1 = value attr
    // +1 if NOTIFY|INDICATE (CCC descriptor)
    // +1 if user_description
    size_t attrs_needed = 2;
    bool needs_ccc = (characteristic->props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE)) != 0;
    bool needs_user_desc = (user_description != NULL && user_description[0] != '\0');
    if (needs_ccc) {
        attrs_needed++;
    }
    if (needs_user_desc) {
        attrs_needed++;
    }

    service_ensure_capacity(self, attrs_needed);

    // Map CP properties to Zephyr BT spec properties
    uint8_t zephyr_props = 0;
    if (characteristic->props & CHAR_PROP_BROADCAST) {
        zephyr_props |= BT_GATT_CHRC_BROADCAST;
    }
    if (characteristic->props & CHAR_PROP_READ) {
        zephyr_props |= BT_GATT_CHRC_READ;
    }
    if (characteristic->props & CHAR_PROP_WRITE_NO_RESPONSE) {
        zephyr_props |= BT_GATT_CHRC_WRITE_WITHOUT_RESP;
    }
    if (characteristic->props & CHAR_PROP_WRITE) {
        zephyr_props |= BT_GATT_CHRC_WRITE;
    }
    if (characteristic->props & CHAR_PROP_NOTIFY) {
        zephyr_props |= BT_GATT_CHRC_NOTIFY;
    }
    if (characteristic->props & CHAR_PROP_INDICATE) {
        zephyr_props |= BT_GATT_CHRC_INDICATE;
    }

    // Set up the Zephyr chrc struct in the characteristic
    characteristic->zephyr_chrc.uuid = &characteristic->zephyr_uuid.uuid;
    characteristic->zephyr_chrc.value_handle = 0;
    characteristic->zephyr_chrc.properties = zephyr_props;

    // Map permissions
    uint16_t perm = bleio_security_to_zephyr_perm(
        characteristic->read_perm, characteristic->write_perm, characteristic->props);

    // Attr: characteristic declaration
    size_t idx = self->attr_count;
    self->attrs[idx] = (struct bt_gatt_attr) {
        .uuid = (const struct bt_uuid *)&_uuid_chrc,
        .perm = BT_GATT_PERM_READ,
        .read = bt_gatt_attr_read_chrc,
        .user_data = &characteristic->zephyr_chrc,
    };
    idx++;

    // Attr: characteristic value (UUID points into characteristic, which persists)
    characteristic->value_attr_index = idx;
    self->attrs[idx] = (struct bt_gatt_attr) {
        .uuid = &characteristic->zephyr_uuid.uuid,
        .perm = perm,
        .read = (characteristic->props & CHAR_PROP_READ) ? bleio_char_read_cb : NULL,
        .write = (characteristic->props & (CHAR_PROP_WRITE | CHAR_PROP_WRITE_NO_RESPONSE)) ? bleio_char_write_cb : NULL,
        .user_data = characteristic,
    };
    idx++;

    // Attr: CCC descriptor (for NOTIFY/INDICATE)
    if (needs_ccc) {
        characteristic->zephyr_ccc = (struct bt_gatt_ccc_managed_user_data)
            BT_GATT_CCC_MANAGED_USER_DATA_INIT(bleio_ccc_changed_cb, bleio_ccc_write_cb, NULL);
        self->attrs[idx] = (struct bt_gatt_attr) {
            .uuid = (const struct bt_uuid *)&_uuid_ccc,
            .perm = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
            .read = bt_gatt_attr_read_ccc,
            .write = bt_gatt_attr_write_ccc,
            .user_data = &characteristic->zephyr_ccc,
        };
        idx++;
    }

    // Attr: user description descriptor
    if (needs_user_desc) {
        self->attrs[idx] = (struct bt_gatt_attr) {
            .uuid = (const struct bt_uuid *)&_uuid_cud,
            .perm = BT_GATT_PERM_READ,
            .read = bt_gatt_attr_read_cud,
            .user_data = (void *)user_description,
        };
        idx++;
    }

    self->attr_count = idx;

    // Add characteristic to list
    mp_obj_list_append(MP_OBJ_FROM_PTR(self->characteristic_list),
        MP_OBJ_FROM_PTR(characteristic));

    // Register the service
    self->zephyr_service.attrs = self->attrs;
    self->zephyr_service.attr_count = self->attr_count;
    int err = bt_gatt_service_register(&self->zephyr_service);
    if (err != 0) {
        raise_zephyr_error(err);
    }
    self->registered = true;
}
