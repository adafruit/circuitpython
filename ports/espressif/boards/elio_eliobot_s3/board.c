// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Romain Boutrois
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "py/mpstate.h"
#include "supervisor/filesystem.h"

void board_init(void) {
    mp_import_stat_t stat_b = mp_import_stat("boot.py");
    if (stat_b != MP_IMPORT_STAT_FILE) {
        supervisor_vfs_t *fs_mount = filesystem_circuitpy();
        supervisor_vfs_file_t fs;
        // Default boot.py: CIRCUITPY is read-only over USB so children can't
        // break the robot's files from the drive, while code sent over the
        // serial REPL or BLE (Eliobot's web editor) can still write them.
        const byte buffer[] =
            "import storage\n"
            "\n"
            "# Eliobot default: the USB drive is read-only, and code sent by\n"
            "# the Elioblocs editor can write files. To edit files from the drive\n"
            "# instead, comment out the next line.\n"
            "storage.remount(\"/\", False)\n";
        if (supervisor_vfs_open_file(fs_mount, "/boot.py",
            SUPERVISOR_FS_OPEN_WRITE | SUPERVISOR_FS_OPEN_CREATE |
            SUPERVISOR_FS_OPEN_TRUNCATE, 0, &fs) == SUPERVISOR_FS_OK) {
            size_t char_written = 0;
            supervisor_vfs_write_file(&fs, buffer, sizeof(buffer) - 1, &char_written);
            supervisor_vfs_close_file(&fs);
        }
    }
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
