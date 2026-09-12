// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mpconfig.h"
#include "py/mpstate.h"
#include "py/stackctrl.h"
#include "supervisor/background_callback.h"
#include "supervisor/filesystem.h"
#include "supervisor/workflow.h"
#include "supervisor/shared/serial.h"
#include "supervisor/shared/workflow.h"

#if CIRCUITPY_BLEIO
#include "shared-bindings/_bleio/__init__.h"
#include "supervisor/shared/bluetooth/bluetooth.h"
#if CIRCUITPY_BLE_SERIAL_SERVICE
#include "supervisor/shared/bluetooth/serial.h"
#endif
#endif

#if CIRCUITPY_USB_DEVICE || CIRCUITPY_USB_KEYBOARD_WORKFLOW
#include "supervisor/usb.h"
#endif
#if CIRCUITPY_TINYUSB
#include "tusb.h"
#endif

#if CIRCUITPY_WEB_WORKFLOW
#include "supervisor/shared/web_workflow/web_workflow.h"
#include "supervisor/shared/web_workflow/websocket.h"
static background_callback_t workflow_background_cb = {NULL, NULL};
#endif


// Called during a VM reset. Doesn't actually reset things.
void supervisor_workflow_reset(void) {
    #if CIRCUITPY_BLEIO
    supervisor_start_bluetooth();
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    bool result = supervisor_start_web_workflow();
    if (result) {
        if (!workflow_background_cb.fun) {
            // Enable background callbacks if web_workflow startup successful.
            workflow_background_cb.fun = supervisor_web_workflow_background;
        }
        supervisor_workflow_request_background();
    }
    #endif
}

void supervisor_workflow_request_background(void) {
    #if CIRCUITPY_WEB_WORKFLOW
    if (workflow_background_cb.fun) {
        workflow_background_cb.data = NULL;
        background_callback_add_core(&workflow_background_cb);
    } else {
        // Unblock polling thread if necessary
        socketpool_socket_poll_resume();
    }
    #endif
}

// Return true if host has completed connection to us (such as USB enumeration).
// This is used to determine when to pretend to deep sleep.
bool supervisor_workflow_active(void) {
    #if CIRCUITPY_USB_DEVICE
    // Eventually there might be other non-USB workflows, such as BLE.
    // usb_connected() checks for usb mounted and not suspended.
    if (usb_connected()) {
        return true;
    }
    #endif
    #if CIRCUITPY_WEB_WORKFLOW
    if (websocket_connected()) {
        return true;
    }
    #endif
    #if CIRCUITPY_BLE_SERIAL_SERVICE
    if (ble_serial_connected()) {
        return true;
    }
    #endif

    return false;
}

void supervisor_workflow_start(void) {
    // Start USB after giving boot.py a chance to tweak behavior.
    #if CIRCUITPY_USB_DEVICE
    // Setup USB connection after heap is available.
    // It needs the heap to build descriptors.
    usb_init();
    #endif

    // Set up any other serial connection.
    serial_init();

    #if CIRCUITPY_BLEIO
    bleio_reset();
    supervisor_start_bluetooth();
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    if (supervisor_start_web_workflow()) {
        // Enable background callbacks if web_workflow startup successful.
        workflow_background_cb.fun = supervisor_web_workflow_background;
        // Kick the first background run now that the callback is installed.
        supervisor_workflow_request_background();
    }
    #endif

    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    usb_keyboard_init();
    #endif
}

supervisor_fs_err_t supervisor_workflow_move(const char *old_path, const char *new_path) {
    const char *old_mount_path;
    const char *new_mount_path;
    supervisor_vfs_t *active_mount = filesystem_for_path(old_path, &old_mount_path);
    supervisor_vfs_t *new_mount = filesystem_for_path(new_path, &new_mount_path);
    if (active_mount == NULL || new_mount == NULL || active_mount != new_mount || !supervisor_vfs_supported(active_mount)) {
        return SUPERVISOR_FS_NO_PATH;
    }
    if (!filesystem_lock(active_mount)) {
        return SUPERVISOR_FS_WRITE_PROTECTED;
    }

    supervisor_fs_err_t result = supervisor_vfs_rename(active_mount, old_mount_path, new_mount_path);
    filesystem_unlock(active_mount);
    return result;
}

supervisor_fs_err_t supervisor_workflow_mkdir(uint64_t mtime_ns, const char *full_path) {
    const char *mount_path;
    supervisor_vfs_t *active_mount = filesystem_for_path(full_path, &mount_path);
    if (active_mount == NULL || !supervisor_vfs_supported(active_mount)) {
        return SUPERVISOR_FS_NO_PATH;
    }

    // If there is a mount on the directory, then the mount_path will be empty.
    if (strlen(mount_path) == 0) {
        return SUPERVISOR_FS_EXIST;
    }

    // Check to see if the directory exists already. We don't care about writing
    // it if it already exists.
    supervisor_fs_err_t result = supervisor_vfs_stat(active_mount, mount_path, NULL, NULL, NULL);
    if (result == SUPERVISOR_FS_OK) {
        return SUPERVISOR_FS_EXIST;
    }

    if (!filesystem_lock(active_mount)) {
        return SUPERVISOR_FS_WRITE_PROTECTED;
    }

    result = supervisor_vfs_mkdir(active_mount, mount_path, mtime_ns);
    filesystem_unlock(active_mount);
    return result;
}

supervisor_fs_err_t supervisor_workflow_mkdir_parents(uint64_t mtime_ns, char *path) {
    supervisor_fs_err_t result = SUPERVISOR_FS_OK;
    // Make parent directories.
    for (size_t j = 1; j < strlen(path); j++) {
        if (path[j] == '/') {
            path[j] = '\0';
            result = supervisor_workflow_mkdir(mtime_ns, path);
            path[j] = '/';
            if (result != SUPERVISOR_FS_OK && result != SUPERVISOR_FS_EXIST) {
                break;
            }
        }
    }
    // Make the target directory.
    if (result == SUPERVISOR_FS_OK || result == SUPERVISOR_FS_EXIST) {
        result = supervisor_workflow_mkdir(mtime_ns, path);
        // This may return SUPERVISOR_FS_EXIST when a file with the same name already exists.
        // FATFS does the same thing.
    }
    return result;
}

static supervisor_fs_err_t supervisor_workflow_delete_directory_contents(supervisor_vfs_t *active_mount, const char *path) {
    // Check the stack since we're putting paths on it.
    if (mp_stack_usage() >= MP_STATE_THREAD(stack_limit)) {
        return SUPERVISOR_FS_IO;
    }
    supervisor_vfs_dir_t dir;
    memset(&dir, 0, sizeof(dir));
    char name[FF_MAX_LFN + 1];
    supervisor_fs_err_t res = SUPERVISOR_FS_OK;
    while (res == SUPERVISOR_FS_OK) {
        res = supervisor_vfs_opendir(active_mount, path, &dir);
        if (res != SUPERVISOR_FS_OK) {
            break;
        }
        res = supervisor_vfs_readdir(&dir, name, sizeof(name), NULL, NULL, NULL);
        // We close and reopen the directory every time since we're deleting
        // entries and it may invalidate the directory handle.
        supervisor_vfs_closedir(&dir);
        if (res != SUPERVISOR_FS_OK || name[0] == '\0') {
            break;
        }
        size_t pathlen = strlen(path);
        size_t fnlen = strlen(name);
        char full_path[pathlen + 1 + fnlen + 1];
        memcpy(full_path, path, pathlen);
        full_path[pathlen] = '/';
        memcpy(full_path + pathlen + 1, name, fnlen + 1);
        if (path[pathlen - 1] == '/') {
            // Trim the extra slash we put in. This happens when path is "/".
            memmove(full_path + pathlen, name, fnlen + 1);
        }
        bool is_dir = false;
        res = supervisor_vfs_stat(active_mount, full_path, &is_dir, NULL, NULL);
        if (res != SUPERVISOR_FS_OK) {
            break;
        }
        if (is_dir) {
            res = supervisor_workflow_delete_directory_contents(active_mount, full_path);
            if (res != SUPERVISOR_FS_OK) {
                break;
            }
        }
        res = supervisor_vfs_unlink(active_mount, full_path);
    }
    supervisor_vfs_closedir(&dir);
    return res;
}

supervisor_fs_err_t supervisor_workflow_delete_recursive(const char *full_path) {
    const char *mount_path;
    supervisor_vfs_t *active_mount = filesystem_for_path(full_path, &mount_path);
    if (active_mount == NULL || !supervisor_vfs_supported(active_mount)) {
        return SUPERVISOR_FS_NO_PATH;
    }
    if (!filesystem_lock(active_mount)) {
        return SUPERVISOR_FS_WRITE_PROTECTED;
    }
    supervisor_fs_err_t result = SUPERVISOR_FS_OK;
    bool is_dir = false;
    result = supervisor_vfs_stat(active_mount, mount_path, &is_dir, NULL, NULL);
    if (result == SUPERVISOR_FS_OK) {
        if (is_dir) {
            result = supervisor_workflow_delete_directory_contents(active_mount, mount_path);
        }
        if (result == SUPERVISOR_FS_OK) {
            result = supervisor_vfs_unlink(active_mount, mount_path);
        }
    }
    filesystem_unlock(active_mount);
    return result;
}
