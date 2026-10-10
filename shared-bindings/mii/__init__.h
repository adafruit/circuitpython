// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/mii/Ethernet.h"

void mii_user_reset(void);

// Cross-translation-unit API implemented by each port's common-hal/ethernet
// for supervisor/shared/web_workflow.c. They all operate on the board's
// instance, which board code registers via mii_set_default_instance();
// user-constructed interfaces are separate objects.

// The board's ethernet instance as an mp_obj_t for SocketPool, or NULL when
// the board never set one.
mp_obj_t mii_get_default_instance(void);

// Raw uint32_t IPv4 address of the interface, in network byte order, or 0
// when the interface has none.
uint32_t mii_get_ipv4_address(void);

// Raw hostname of the network interface as configured for the interface's
// DHCP request (option 12). Never allocates. The returned pointer points into
// storage owned by the port and stays valid until the hostname is replaced,
// so use it promptly rather than holding onto it.
const char *mii_get_hostname_raw(void);

// Blocks until the interface has an IPv4 address (DHCP lease or static), or
// 20s pass, or a hard interrupt arrives. Returns immediately when the
// interface cannot ever get one (down, disabled or no link).
void mii_wait_for_lease(void);
