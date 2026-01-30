// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/espsdcard/SDCard.h"

extern const mp_obj_type_t espsdcard_sdcard_type;

// Module-level deinit for convenience (static method support)
void espsdcard_sdcard_deinit_all(void);
