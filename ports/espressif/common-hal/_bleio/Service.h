// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objlist.h"
#include "common-hal/_bleio/UUID.h"

#define MAX_CHARACTERISTIC_COUNT 10

#include "host/ble_gatt.h"

typedef struct bleio_service_obj {
    mp_obj_base_t base;
    // Handle for the local service.
    uint16_t handle;
    // True if created during discovery.
    bool is_remote;
    bool is_secondary;
    // True while NimBLE's GATT table holds this service: it must be deleted
    // before the definition is changed or the service is torn down.
    bool registered;
    bleio_uuid_obj_t *uuid;
    // The connection object is set only when this is a remote service.
    // A local service doesn't know the connection.
    mp_obj_t connection;
    mp_obj_list_t *characteristic_list;
    // Range of attribute handles of this remote service.
    uint16_t start_handle;
    uint16_t end_handle;
    struct ble_gatt_svc_def service_def;
    // Include a spot for terminating the service def array.
    uint8_t next_svc_type;
    struct ble_gatt_chr_def chr_defs[MAX_CHARACTERISTIC_COUNT + 1];
    // Link in the list of services retained while NimBLE's GATT table refers
    // to them; see Service.c. NULL when not on that list.
    struct bleio_service_obj *next_retained;
} bleio_service_obj_t;

void bleio_service_from_connection(bleio_service_obj_t *self, mp_obj_t connection);
void bleio_service_readd(bleio_service_obj_t *self);

// Stop retaining every service. Call when the heap they live on is about to go
// away.
void bleio_service_forget_retained(void);

// Mark the retained services as reachable during a GC pass.
void bleio_service_gc_collect(void);
