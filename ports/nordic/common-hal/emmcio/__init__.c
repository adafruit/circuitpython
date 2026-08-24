// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/emmcio/EMMC.h"
#include "shared-bindings/emmcio/__init__.h"
#include "shared-module/emmcio/__init__.h"

void emmcio_reset(void) {
    // The supervisor's mount outlives the VM, so its card stays up.
    if (emmcio_is_automounted()) {
        return;
    }
    if (emmcio_spim3_in_use()) {
        emmcio_emmc_release_hardware();
    }
}
