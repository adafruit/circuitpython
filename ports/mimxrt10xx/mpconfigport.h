// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#define MICROPY_PY_SYS_PLATFORM                     "NXP IMXRT10XX"
#define SPI_FLASH_MAX_BAUDRATE 24000000

extern uint8_t _ld_filesystem_start;
extern uint8_t _ld_filesystem_end;
extern uint8_t _ld_default_stack_size;

#define CIRCUITPY_DEFAULT_STACK_SIZE                ((uint32_t)&_ld_default_stack_size)

#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR ((uint32_t)&_ld_filesystem_start)
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE ((uint32_t)(&_ld_filesystem_end - &_ld_filesystem_start))

// Allocate 32 bytes at a time instead of the default 16 so that allocated buffers
// are aligned to cache lines.
#define MICROPY_BYTES_PER_GC_BLOCK              (32)

#define CIRCUITPY_USB_DEVICE_HIGH_SPEED (1)

#include "py/circuitpy_mpconfig.h"

#if MICROPY_EMIT_THUMB || MICROPY_EMIT_INLINE_THUMB
// The GC heap is in cacheable OCRAM and the M7 runs with both caches enabled, so
// freshly written code has to be pushed out of the data cache before the
// instruction side is allowed to fetch it.
void *port_native_code_commit(void *buf, size_t len, void *reloc);
#define MP_PLAT_COMMIT_EXEC(buf, len, reloc) port_native_code_commit((buf), (len), (reloc))

// Defining MP_PLAT_COMMIT_EXEC otherwise tells the core that the port owns the native
// text allocation, which turns off GC tracking for it. This hook only flushes caches and
// hands back the same GC heap pointer, so the text still has to be tracked or a later
// collection reclaims code that is only referenced from inside the block.
#define MICROPY_PERSISTENT_CODE_TRACK_FUN_DATA (1)
#define MICROPY_PERSISTENT_CODE_TRACK_BSS_RODATA (0)
#endif

// TODO:
//    mp_obj_t playing_audio[AUDIO_DMA_CHANNEL_COUNT] as an MP_REGISTER_ROOT_POINTER.
