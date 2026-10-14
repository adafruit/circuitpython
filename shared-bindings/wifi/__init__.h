// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/wifi/Radio.h"

extern wifi_radio_obj_t common_hal_wifi_radio_obj;

void common_hal_wifi_init(bool user_initiated);
void common_hal_wifi_gc_collect(void);

void wifi_user_reset(void);

// Raw hostname of the network interface as configured for the interface's
// DHCP request (option 12). Never allocates. The returned pointer points into
// storage owned by the port and stays valid until the hostname is replaced,
// so use it promptly rather than holding onto it. Used by
// supervisor/shared/web_workflow.
const char *wifi_get_hostname_raw(void);
