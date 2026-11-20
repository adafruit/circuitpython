// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#if PICO_RP2040
#include "Framebuffer_RP2040.h"
#else
#include "Framebuffer_RP2350.h"
#endif

mp_obj_t picodvi_framebuffer_start_vm(picodvi_framebuffer_obj_t *self);
#if !PICO_RP2040
void picodvi_framebuffer_set_audioout(picodvi_framebuffer_obj_t *self, mp_obj_t audioout);
void picodvi_framebuffer_set_audioout_sample(picodvi_framebuffer_obj_t *self, mp_obj_t sample);
#endif
