// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Lucian Copeland
//
// SPDX-License-Identifier: MIT

#pragma once
#include "common-hal/rclcpy/__init__.h"

void common_hal_rclcpy_init(const char *agent_ip, const char *agent_port, int16_t domain_id);
rclcpy_context_t *common_hal_rclcpy_get_default_context(void);
bool common_hal_rclcpy_default_context_is_initialized(void);
