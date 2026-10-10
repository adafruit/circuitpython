// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/mii/Ethernet.h"

void mii_user_reset(void);

// Cross-translation-unit API for supervisor/shared/web_workflow.c; the
// declarations (and web_workflow's contract) live in
// shared-bindings/mii/__init__.h. The board calls set_default_instance
// once its own instance is constructed.
void mii_set_default_instance(mii_ethernet_obj_t *self);
mp_obj_t mii_get_default_instance(void);
