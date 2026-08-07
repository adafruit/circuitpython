// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/objstr.h"
#include "py/objtuple.h"

// The CXD5602 has no true hardware random number generator wired up in this
// port. This function used to fill the buffer from the C library rand(), a
// deterministic PRNG, and return true -- meaning os.urandom() handed back
// predictable bytes while presenting itself as a cryptographically secure
// source. That is the same class of defect behind the 2026 Coldcard wallet
// key-recovery incident, where a silent PRNG fallback stood in for hardware
// entropy.
//
// Fail closed instead: report that no secure entropy is available, so that
// os.urandom() raises NotImplementedError ("No hardware random available")
// rather than silently returning non-random data. This matches the broadcom,
// silabs and litex ports, which also lack a hardware RNG.
//
// The correct long-term fix is to wire a real hardware entropy source for the
// CXD5602 if the silicon exposes one; that requires the chip and datasheet and
// is left as a follow-up.
bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length) {
    (void)buffer;
    (void)length;
    return false;
}
