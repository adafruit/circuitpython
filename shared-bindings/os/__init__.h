// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "py/objtuple.h"
#include "shared-module/os/__init__.h"

void common_hal_os_chdir(const char *path);
mp_obj_t common_hal_os_getcwd(void);
mp_obj_t common_hal_os_getenv(const char *key, mp_obj_t default_);
mp_obj_t common_hal_os_getenv_path(const char *path, const char *key, mp_obj_t default_);

#if CIRCUITPY_WIFI || CIRCUITPY_MII
// Gets the value of os.uname().nodename. That is the network hostname, or the
// hostname the network interface will be given once it is set up, or
// MICROPY_HW_MCU_NAME if there is no hostname. Writes the value to buf as a
// NUL-terminated string, truncated to fit in buf_size bytes. If buf_size is 0,
// writes nothing. Never allocates or raises.
void common_hal_os_get_nodename(char *buf, size_t buf_size);
#endif

mp_obj_t common_hal_os_listdir(const char *path);
void common_hal_os_mkdir(const char *path);
void common_hal_os_remove(const char *path);
void common_hal_os_rename(const char *old_path, const char *new_path);
void common_hal_os_rmdir(const char *path);
mp_obj_t common_hal_os_stat(const char *path);
mp_obj_t common_hal_os_statvfs(const char *path);
void common_hal_os_utime(const char *path, mp_obj_t times);

// Returns true if data was correctly sourced from a true random number generator.
bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length);
