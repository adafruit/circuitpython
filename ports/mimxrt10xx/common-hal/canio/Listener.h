// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 qutefox
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/canio/CAN.h"
#include "shared-module/canio/Match.h"

typedef struct canio_listener_obj {
    mp_obj_base_t base;
    canio_can_obj_t *can;
    uint32_t timeout_ms;
} canio_listener_obj_t;
