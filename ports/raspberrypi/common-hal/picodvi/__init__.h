// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern void picodvi_autoconstruct(void);

// The buffers of the display built at boot. On boards with PSRAM they are
// allocated at boot even when that display is not built, see
// picodvi_autoconstruct. A Framebuffer whose framebuffer and DMA command list
// fit in them uses them instead of allocating, and leaves them allocated.
uint32_t *picodvi_boot_ram_take(size_t bytes);
bool picodvi_is_boot_ram(const void *ptr);
