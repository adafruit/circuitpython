// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Romain Boutrois
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"
#include "py/mpstate.h"
#include "supervisor/filesystem.h"

void board_init(void) {
    mp_import_stat_t stat_b = mp_import_stat("boot.py");
    if (stat_b != MP_IMPORT_STAT_FILE) {
        fs_user_mount_t *fs_mount = filesystem_circuitpy();
        FATFS *fatfs = &fs_mount->fatfs;
        FIL fs;
        UINT char_written = 0;
        const byte buffer[] = "import board\nimport storage\n\n# Write options : True = Mass Storage, False = REPL\nstorage.remount(\"/\", False)\n";
        // Create or modify existing boot.py file
        f_open(fatfs, &fs, "/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
        f_write(&fs, buffer, sizeof(buffer) - 1, &char_written);
        f_close(&fs);
        // Delete code.py, use main.py
        mp_import_stat_t stat_c = mp_import_stat("code.py");
        if (stat_c == MP_IMPORT_STAT_FILE) {
            f_unlink(fatfs, "/code.py");
        }
        // Create main.py file
        const byte buffer2[] = "print(\"Hello World!\")\n";
        f_open(fatfs, &fs, "/main.py", FA_WRITE | FA_CREATE_ALWAYS);
        f_write(&fs, buffer2, sizeof(buffer2) - 1, &char_written);
        f_close(&fs);
    }
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
