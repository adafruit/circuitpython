// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Not made available to the VM but used by other modules to normalize paths.
const char *common_hal_os_path_abspath(const char *path);
