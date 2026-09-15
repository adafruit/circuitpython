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

#include "sdkconfig.h"

#ifndef BLEIO_TOTAL_CONNECTION_COUNT
#define BLEIO_TOTAL_CONNECTION_COUNT CONFIG_BT_NIMBLE_MAX_CONNECTIONS
#endif
// Do not allow more connections than NimBLE was configured for.
MP_STATIC_ASSERT(BLEIO_TOTAL_CONNECTION_COUNT <= CONFIG_BT_NIMBLE_MAX_CONNECTIONS);

#define BLEIO_HANDLE_INVALID     0xffff

extern bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

typedef struct {
    mp_obj_base_t base;
    bleio_scanresults_obj_t *scan_results;
    mp_obj_t name;
    mp_obj_tuple_t *connection_objs;
    background_callback_t background_callback;
    bool user_advertising;
    // Cached local address returned by common_hal_bleio_adapter_get_address().
    // Stored inline so it never needs to allocate, even when read before the
    // heap is available (e.g. from bleio_adapter_reset_name).
    bleio_address_obj_t address;
} bleio_adapter_obj_t;

void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter);
void bleio_adapter_reset(bleio_adapter_obj_t *adapter);
