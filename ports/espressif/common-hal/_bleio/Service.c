// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "py/gc.h"
#include "py/runtime.h"
#include "common-hal/_bleio/__init__.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/Adapter.h"

#include "host/ble_gatt.h"

// NimBLE's GATT table keeps raw pointers into a registered local service and
// everything under it: the chr_defs array lives inside the service object, each
// characteristic and descriptor is an access-callback argument, and every UUID
// is referenced by address. Collecting any of them would leave those dangling,
// so retain a heap Service here for as long as NimBLE knows about it.
// Retaining the service is enough because it holds its characteristics,
// which hold their descriptors. Services in static storage, like the BLE workflow's,
// cannot be collected and are skipped.
static bleio_service_obj_t *_retained_services;

static void service_retain(bleio_service_obj_t *self) {
    if (!gc_ptr_on_heap((void *)self)) {
        return;
    }
    for (bleio_service_obj_t *it = _retained_services; it != NULL; it = it->next_retained) {
        if (it == self) {
            return;
        }
    }
    self->next_retained = _retained_services;
    _retained_services = self;
}

static void service_release(bleio_service_obj_t *self) {
    bleio_service_obj_t **prev = &_retained_services;
    for (bleio_service_obj_t *it = *prev; it != NULL; it = it->next_retained) {
        if (it == self) {
            *prev = it->next_retained;
            it->next_retained = NULL;
            return;
        }
        prev = &it->next_retained;
    }
}

void bleio_service_forget_retained(void) {
    _retained_services = NULL;
}

// Mark the retained services. One pointer is enough: the GC traces
// next_retained through the rest of the chain from the head.
void bleio_service_gc_collect(void) {
    gc_collect_ptr(_retained_services);
}

uint32_t _common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary, mp_obj_list_t *characteristic_list) {
    self->handle = 0xFFFF;
    self->uuid = uuid;
    self->characteristic_list = characteristic_list;
    self->is_remote = false;
    self->connection = NULL;
    self->is_secondary = is_secondary;

    self->service_def.type = is_secondary? BLE_GATT_SVC_TYPE_SECONDARY : BLE_GATT_SVC_TYPE_PRIMARY;
    self->service_def.uuid = &uuid->nimble_ble_uuid.u;
    self->service_def.includes = NULL;
    self->service_def.characteristics = self->chr_defs;
    self->next_svc_type = 0;
    self->next_retained = NULL;
    self->registered = false;

    // Don't add the service yet because we don't have any characteristics.
    return 0;
}

void common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary) {
    _common_hal_bleio_service_construct(self, uuid, is_secondary,
        mp_obj_new_list(0, NULL));
}

void common_hal_bleio_service_deinit(bleio_service_obj_t *self) {
    // Take the service out of NimBLE's GATT table first, so nothing torn down
    // below is still reachable from it.
    if (self->registered) {
        ble_gatts_delete_svc(&self->uuid->nimble_ble_uuid.u);
        self->registered = false;
    }
    // NimBLE no longer refers to this service, so stop retaining it, and deinit
    // the characteristics: a local one that can notify or indicate holds an
    // event handler that must come off the event list with it.
    service_release(self);
    if (!self->is_remote) {
        for (size_t i = 0; i < self->characteristic_list->len; i++) {
            common_hal_bleio_characteristic_deinit(
                MP_OBJ_TO_PTR(self->characteristic_list->items[i]));
        }
    }
    self->service_def.type = 0;
}

void bleio_service_from_connection(bleio_service_obj_t *self, mp_obj_t connection) {
    self->handle = BLEIO_HANDLE_INVALID;
    self->uuid = NULL;
    self->characteristic_list = mp_obj_new_list(0, NULL);
    self->is_remote = true;
    self->registered = false;
    self->is_secondary = false;
    self->connection = connection;
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

// Register the service, or register it again after its definition changed:
// NimBLE cannot amend a service that is already in its GATT table, so the old
// version is deleted and the whole service added again.
void bleio_service_readd(bleio_service_obj_t *self) {
    if (self->registered) {
        ble_gatts_delete_svc(&self->uuid->nimble_ble_uuid.u);
        self->registered = false;
    }
    CHECK_NIMBLE_ERROR(ble_gatts_add_dynamic_svcs(&self->service_def));
    self->registered = true;
    // NimBLE now refers to this service and everything under it.
    service_retain(self);
}


void common_hal_bleio_service_add_characteristic(bleio_service_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {
    // Don't overflow chr_defs table.
    size_t i = self->characteristic_list->len;
    if (i >= MAX_CHARACTERISTIC_COUNT) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Too many %q"), MP_QSTR_characteristics);
    }

    mp_obj_list_append(self->characteristic_list, MP_OBJ_FROM_PTR(characteristic));

    if (user_description != NULL) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_user_description);
    }

    self->chr_defs[i].uuid = &characteristic->uuid->nimble_ble_uuid.u;
    self->chr_defs[i].access_cb = bleio_characteristic_access_cb;
    self->chr_defs[i].arg = characteristic;
    self->chr_defs[i].descriptors = characteristic->dsc_defs;
    self->chr_defs[i].flags = characteristic->flags;
    self->chr_defs[i].min_key_size = 16;
    self->chr_defs[i].val_handle = &characteristic->handle;
    self->chr_defs[i].cpfd = NULL;
    self->chr_defs[i + 1].uuid = NULL;
    characteristic->chr_def = &self->chr_defs[i];

    bleio_service_readd(self);
}
