// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/zephyr_kernel/__init__.h"
#include "py/runtime.h"
#include <zephyr/kernel.h>

#include <errno.h>
#include <zephyr/kernel.h>

void raise_zephyr_error(int err) {
    if (err == 0) {
        return;
    }
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
        case EADDRINUSE:
            printk("EADDRINUSE\n");
            break;
        case EIO:
            printk("EIO\n");
            break;
        case ENOSYS:
            printk("ENOSYS\n");
            break;
        case EINVAL:
            printk("EINVAL\n");
            break;
        default:
            printk("Zephyr error %d\n", err);
    }
    mp_raise_OSError(-err);
}
