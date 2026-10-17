// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// Object struct and the common_hal_* accessors.
#include "shared-module/hardwarekey/DigitalSignatureKey.h"

// Type object used in Python. Shared between ports.
extern const mp_obj_type_t hardwarekey_digitalsignaturekey_type;

// The module-level loader function (hardwarekey.load_digital_signature_key()).
extern const mp_obj_fun_builtin_fixed_t hardwarekey_load_digital_signature_key_obj;
