// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// Padding/hash-algorithm markers, mirroring
// cryptography.hazmat.primitives.asymmetric.padding.PKCS1v15/OAEP and
// cryptography.hazmat.primitives.hashes.SHA256.
//
// PKCS1v15 and SHA256 are plain constants: no parameters, nothing to
// construct, so *_obj is the only thing exposed under that name (the type
// itself isn't reachable, so it can't be called). OAEP takes a real
// parameter, so it stays a constructible type; *_singleton is what its
// make_new() returns after validating arguments.
extern const mp_obj_type_t crypto_primitives_pkcs1v15_type;
extern const mp_obj_base_t crypto_primitives_pkcs1v15_obj;
extern const mp_obj_type_t crypto_primitives_sha256_type;
extern const mp_obj_base_t crypto_primitives_sha256_obj;
extern const mp_obj_type_t crypto_primitives_oaep_type;
extern const mp_obj_base_t crypto_primitives_oaep_singleton;
