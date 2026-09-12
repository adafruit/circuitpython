// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "supervisor/filesystem.h"

extern bool supervisor_workflow_connecting(void);

// File system helpers for workflow code. All of them take full paths (such as
// "/code.py"), resolve them to a mount and return supervisor_fs_err_t. Paths
// may name either FAT or littlefs mounts.
supervisor_fs_err_t supervisor_workflow_move(const char *old_path, const char *new_path);
supervisor_fs_err_t supervisor_workflow_mkdir(uint64_t mtime_ns, const char *full_path);
supervisor_fs_err_t supervisor_workflow_mkdir_parents(uint64_t mtime_ns, char *path);
supervisor_fs_err_t supervisor_workflow_delete_recursive(const char *full_path);
