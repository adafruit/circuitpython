// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <zephyr/bluetooth/gatt.h>

#include "py/obj.h"
#include "py/objlist.h"
#include "common-hal/_bleio/UUID.h"

typedef struct bleio_service_obj {
    mp_obj_base_t base;
    bleio_uuid_obj_t *uuid;
    mp_obj_t connection;
    mp_obj_list_t *characteristic_list;
    uint16_t start_handle;
    uint16_t end_handle;
    bool is_remote;
    bool is_secondary;
    // Zephyr GATT server fields:
    struct bt_gatt_service zephyr_service;
    struct bt_gatt_attr *attrs;
    size_t attr_count;
    size_t attr_capacity;
    struct bt_uuid_128 zephyr_uuid;
    bool registered;
    // Link in the list of heap services retained while Zephyr's GATT database
    // refers to them. See bleio_service_unregister_retained().
    struct bleio_service_obj *next_retained;
} bleio_service_obj_t;

// Unregister every user-created service still in Zephyr's GATT database and
// free their port-heap buffers. Call from bleio_user_reset(), while the GC heap
// the service objects live in is still valid.
void bleio_service_unregister_retained(void);

// Mark the retained services as reachable during a GC pass.
void bleio_service_gc_collect(void);
