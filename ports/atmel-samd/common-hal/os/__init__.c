// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/qstr.h"

#include "shared-bindings/os/__init__.h"

#ifdef SAM_D5X_E5X
#include "hal/include/hal_rand_sync.h"
#endif

bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length) {
    #ifdef SAM_D5X_E5X
    hri_mclk_set_APBCMASK_TRNG_bit(MCLK);
    struct rand_sync_desc random;
    rand_sync_init(&random, TRNG);
    rand_sync_enable(&random);

    rand_sync_read_buf8(&random, buffer, length);

    rand_sync_disable(&random);
    rand_sync_deinit(&random);
    hri_mclk_clear_APBCMASK_TRNG_bit(MCLK);
    return true;
    #else
    return false;
    #endif
}
