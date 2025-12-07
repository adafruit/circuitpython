// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"
#include "py/objtuple.h"
#include "shared-bindings/wifi/Radio.h"


typedef struct wifi_scannednetworks_obj_t {
    mp_obj_base_t base;
    mp_obj_tuple_t *networks;
    size_t next_network_index;
} wifi_scannednetworks_obj_t;


// Forward declaration due to mutually referenced typedef struct definitions.
typedef struct wifi_radio_obj_t wifi_radio_obj_t;

mp_obj_t wifi_scannednetworks_do_scan(wifi_radio_obj_t *self);
void wifi_scannednetworks_deinit(wifi_scannednetworks_obj_t *self);
