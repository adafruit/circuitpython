// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Minimal freestanding C++ ABI support for the vendored SdFat PIO driver.
//
// CircuitPython links the firmware with gcc and without libstdc++/libsupc++, so
// a few C++ ABI symbols that the driver's class hierarchy makes the compiler
// reference must be provided here:
//   * PioSdioCard's vtable contains a deleting destructor slot that names
//     `operator delete`, even though we only ever placement-new the object and
//     call its destructor explicitly (so it is never actually invoked).
//   * The abstract base classes (SdCardInterface / FsBlockDeviceInterface) have
//     pure virtuals, so their transient vtables reference `__cxa_pure_virtual`.
// Both are effectively unreachable at run time; they exist only to satisfy the
// linker.

#include <cstddef>
#include <cstdlib>

extern "C" void __cxa_pure_virtual(void) {
    abort();
}

void operator delete(void *ptr) noexcept {
    free(ptr);
}

void operator delete(void *ptr, size_t) noexcept {
    free(ptr);
}
