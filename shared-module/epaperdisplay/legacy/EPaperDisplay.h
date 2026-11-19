// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/epaperdisplay/epaper_core.h"

struct epaperdisplay_epaperdisplay_obj_t {
    mp_obj_base_t base;
    epaperdisplay_epaper_core_t epaper;
};
