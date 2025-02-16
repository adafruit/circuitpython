// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/zephyr_kernel/__init__.h"
#include "py/runtime.h"

#include <errno.h>


void raise_zephyr_error(int err) {
    switch (-err) {
        case EALREADY:
            printk("EALREADY\n");
            break;
        case EDESTADDRREQ:
            printk("EDESTADDRREQ\n");
            break;
        case EMSGSIZE:
            printk("EMSGSIZE\n");
            break;
        case EPROTONOSUPPORT:
            printk("EPROTONOSUPPORT\n");
            break;
        case EADDRNOTAVAIL:
            printk("EADDRNOTAVAIL\n");
            break;
        case ENETRESET:
            printk("ENETRESET\n");
            break;
        case EISCONN:
            printk("EISCONN\n");
            break;
        case ENOTCONN:
            printk("ENOTCONN\n");
            break;
        case ENOTSUP:
            printk("ENOTSUP\n");
            break;
        default:
            printk("Zephyr error %d\n", err);
    }
}
