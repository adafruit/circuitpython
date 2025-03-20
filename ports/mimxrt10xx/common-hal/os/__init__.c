// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/qstr.h"

#include "shared-bindings/os/__init__.h"

#if CIRCUITPY_RANDOM
#include "sdk/drivers/trng/fsl_trng.h"
#endif

bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length) {
    #if CIRCUITPY_RANDOM
    trng_config_t trngConfig;

    TRNG_GetDefaultConfig(&trngConfig);
    trngConfig.sampleMode = kTRNG_SampleModeVonNeumann;

    TRNG_Init(TRNG, &trngConfig);
    TRNG_GetRandomData(TRNG, buffer, length);
    TRNG_Deinit(TRNG);

    return true;
    #else
    return false;
    #endif
}
