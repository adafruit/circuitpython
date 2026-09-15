// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objtuple.h"

#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/ScanResults.h"

#include "supervisor/background_callback.h"

#ifndef BLEIO_TOTAL_CONNECTION_COUNT
#define BLEIO_TOTAL_CONNECTION_COUNT 5
#endif

#define BLEIO_HANDLE_INVALID BLE_CONN_HANDLE_INVALID

extern bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

typedef struct {
    mp_obj_base_t base;
    // We create buffers and copy the advertising data so it will live for as long as we need.
    uint8_t *advertising_data;
    uint8_t *scan_response_data;
    // Pointer to current data.
    const uint8_t *current_advertising_data;
    bleio_scanresults_obj_t *scan_results;
    mp_obj_t name;
    mp_obj_tuple_t *connection_objs;
    ble_drv_evt_handler_entry_t connection_handler_entry;
    ble_drv_evt_handler_entry_t advertising_handler_entry;
    background_callback_t background_callback;
    bool user_advertising;
    // Whether the current (or most recent) advertising was started by user code
    // rather than the supervisor. Unlike user_advertising, this is not cleared when
    // advertising stops, so the connected-event handler can still read it: the
    // advertising event handler runs first and has already stopped the advertising.
    bool advertising_started_by_user;
    // Cached local address returned by common_hal_bleio_adapter_get_address().
    // Stored inline so it never needs to allocate, even when read before the
    // heap is available (e.g. from bleio_adapter_reset_name).
    bleio_address_obj_t address;
} bleio_adapter_obj_t;

void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter);
void bleio_adapter_reset(bleio_adapter_obj_t *adapter);
