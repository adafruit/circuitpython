// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objtuple.h"

#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/ScanResults.h"

#define BLEIO_TOTAL_CONNECTION_COUNT CONFIG_BT_MAX_CONN

extern bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

typedef struct {
    mp_obj_base_t base;
    bleio_scanresults_obj_t *scan_results;
    mp_obj_t name;
    mp_obj_tuple_t *connection_objs;
    bool user_advertising;
    // Cached local address returned by common_hal_bleio_adapter_get_address().
    // Stored inline so it never needs to allocate, even when read before the
    // heap is available (e.g. from bleio_adapter_reset_name).
    bleio_address_obj_t address;
} bleio_adapter_obj_t;

void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter);

// Queue a background run of supervisor_bluetooth_background() so the VM drains
// incoming BLE PacketBuffer data. Safe to call from Zephyr BT/workqueue context.
void bleio_request_bluetooth_background(void);
