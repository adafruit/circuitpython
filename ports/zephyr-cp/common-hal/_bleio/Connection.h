// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "py/obj.h"

struct bt_conn;

typedef enum {
    PAIR_NOT_PAIRED,
    PAIR_WAITING,
    PAIR_PAIRED,
} pair_status_t;

typedef struct {
    struct bt_conn *conn;
    mp_obj_t connection_obj;
    volatile pair_status_t pair_status;
    uint8_t sec_err; // Security error code from pairing attempt
    // True if user code initiated this connection or accepted it with its own
    // advertising. bleio_user_reset() disconnects only these; the BLE workflow
    // connection is not user-owned and survives VM restarts.
    bool user_owned;
} bleio_connection_internal_t;

typedef struct {
    mp_obj_base_t base;
    bleio_connection_internal_t *connection;
    uint8_t disconnect_reason;
} bleio_connection_obj_t;

mp_obj_t bleio_connection_new_from_internal(bleio_connection_internal_t *connection);
void bleio_connection_register_auth_callbacks(void);
