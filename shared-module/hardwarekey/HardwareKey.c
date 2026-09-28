// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "shared-module/hardwarekey/HardwareKey.h"

// These accessors are port-independent. The one port-specific step -- turning a
// hardware key slot into self->key_id -- happens when the port fills in its
// per-slot HardwareKey objects at startup.

mp_int_t common_hal_hardwarekey_hardwarekey_get_key_slot(hardwarekey_hardwarekey_obj_t *self) {
    return self->key_slot;
}

hardwarekey_purpose_t common_hal_hardwarekey_hardwarekey_get_purpose(hardwarekey_hardwarekey_obj_t *self) {
    return self->purpose;
}

bool common_hal_hardwarekey_hardwarekey_get_exportable(hardwarekey_hardwarekey_obj_t *self) {
    return self->exportable;
}

psa_key_id_t common_hal_hardwarekey_hardwarekey_get_key_id(hardwarekey_hardwarekey_obj_t *self) {
    return self->key_id;
}
