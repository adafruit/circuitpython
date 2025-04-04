// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2015 Josef Gajdusek
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "extmod/vfs.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/os/__init__.h"
#include "shared-bindings/storage/__init__.h"
#include "supervisor/filesystem.h"
#include "supervisor/flash.h"

#if CIRCUITPY_USB_DEVICE
#include "supervisor/usb.h"
#endif

#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
#include "tusb.h"

// Is the MSC device enabled?
bool storage_usb_is_enabled;

void storage_usb_set_defaults(void) {
    storage_usb_is_enabled = CIRCUITPY_USB_MSC_ENABLED_DEFAULT;
}

bool storage_usb_enabled(void) {
    return storage_usb_is_enabled;
}

static bool usb_drive_set_enabled(bool enabled) {
    // We can't change the descriptors once we're connected.
    if (tud_connected()) {
        return false;
    }
    filesystem_set_internal_writable_by_usb(enabled);
    storage_usb_is_enabled = enabled;
    return true;
}

bool common_hal_storage_disable_usb_drive(void) {
    return usb_drive_set_enabled(false);
}

bool common_hal_storage_enable_usb_drive(void) {
    return usb_drive_set_enabled(true);
}
#else
bool common_hal_storage_disable_usb_drive(void) {
    return false;
}

bool common_hal_storage_enable_usb_drive(void) {
    return false;
}
#endif // CIRCUITPY_USB_MSC

static mp_obj_t mp_vfs_proxy_call(mp_vfs_mount_t *vfs, qstr meth_name, size_t n_args, const mp_obj_t *args) {
    if (vfs == MP_VFS_NONE) {
        // mount point not found
        mp_raise_OSError(MP_ENODEV);
    }
    if (vfs == MP_VFS_ROOT) {
        // can't do operation on root dir
        mp_raise_OSError(MP_EPERM);
    }
    mp_obj_t meth[n_args + 2];
    mp_load_method(vfs->obj, meth_name, meth);
    if (args != NULL) {
        memcpy(meth + 2, args, n_args * sizeof(*args));
    }
    return mp_call_method_n_kw(n_args, 0, meth);
}

void common_hal_storage_mount(mp_obj_t vfs_obj, const char *mount_path, bool readonly) {
    const char *abs_mount_path = common_hal_os_path_abspath(mount_path);
    // create new object
    mp_vfs_mount_t *vfs = m_new_obj(mp_vfs_mount_t);
    vfs->str = abs_mount_path;
    vfs->len = strlen(abs_mount_path);
    vfs->obj = vfs_obj;
    vfs->next = NULL;

    mp_obj_t args[2];
    args[0] = readonly ? mp_const_true : mp_const_false;
    args[1] = mp_const_false; // Don't make the file system automatically when mounting.

    // Check that there is a directory with the same name as the mount point.
    // But it's ok to mount '/' in any case.
    if (strcmp(vfs->str, "/") != 0) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t mount_point_stat = common_hal_os_stat(abs_mount_path);
            nlr_pop();
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mount_point_stat);
            if ((MP_OBJ_SMALL_INT_VALUE(t->items[0]) & MP_S_IFDIR) == 0) {
                mp_raise_RuntimeError(MP_ERROR_TEXT("Mount point directory missing"));
            }
        } else {
            // Something with the same name doesn't exist.
            mp_raise_RuntimeError(MP_ERROR_TEXT("Mount point directory missing"));
        }
    }

    // check that the destination mount point is unused
    const char *path_out;
    mp_vfs_mount_t *existing_mount = mp_vfs_lookup_path(abs_mount_path, &path_out);
    if (existing_mount != MP_VFS_NONE && existing_mount != MP_VFS_ROOT) {
        if (vfs->len != 1 && existing_mount->len == 1) {
            // if root dir is mounted, still allow to mount something within a subdir of root
        } else {
            // mount point in use
            mp_raise_OSError(MP_EPERM);
        }
    }

    // call the underlying object to do any mounting operation
    mp_vfs_proxy_call(vfs, MP_QSTR_mount, 2, (mp_obj_t *)&args);

    // Insert the vfs into the mount table by pushing it onto the front of the
    // mount table.
    mp_vfs_mount_t **vfsp = &MP_STATE_VM(vfs_mount_table);
    vfs->next = *vfsp;
    *vfsp = vfs;
}

void common_hal_storage_umount_object(mp_obj_t vfs_obj) {
    // remove vfs from the mount table
    mp_vfs_mount_t *vfs = NULL;
    for (mp_vfs_mount_t **vfsp = &MP_STATE_VM(vfs_mount_table); *vfsp != NULL; vfsp = &(*vfsp)->next) {
        if ((*vfsp)->obj == vfs_obj) {
            vfs = *vfsp;
            *vfsp = (*vfsp)->next;
            break;
        }
    }

    if (vfs == NULL) {
        mp_raise_OSError(MP_EINVAL);
    }

    // if we unmounted the current device then set current to root
    if (MP_STATE_VM(vfs_cur) == vfs) {
        MP_STATE_VM(vfs_cur) = MP_VFS_ROOT;
    }

    // call the underlying object to do any unmounting operation
    mp_vfs_proxy_call(vfs, MP_QSTR_umount, 0, NULL);
}

static mp_obj_t storage_object_from_path(const char *mount_path) {
    const char *abs_mount_path = common_hal_os_path_abspath(mount_path);
    for (mp_vfs_mount_t **vfsp = &MP_STATE_VM(vfs_mount_table); *vfsp != NULL; vfsp = &(*vfsp)->next) {
        if (strcmp(abs_mount_path, (*vfsp)->str) == 0) {
            return (*vfsp)->obj;
        }
    }
    mp_raise_OSError(MP_EINVAL);
}

void common_hal_storage_umount_path(const char *mount_path) {
    common_hal_storage_umount_object(storage_object_from_path(mount_path));
}

mp_obj_t common_hal_storage_getmount(const char *mount_path) {
    return storage_object_from_path(mount_path);
}

void common_hal_storage_remount(const char *mount_path, bool readonly, bool disable_concurrent_write_protection) {
    const char *path_under_mount;
    const char *abs_mount_path = common_hal_os_path_abspath(mount_path);
    fs_user_mount_t *fs_usermount = filesystem_for_path(abs_mount_path, &path_under_mount);
    if (path_under_mount[0] != 0 && strcmp(abs_mount_path, "/") != 0) {
        mp_raise_OSError(MP_EINVAL);
    }

    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
    if (!blockdev_lock(fs_usermount)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot remount path when visible via USB."));
    }
    #endif

    filesystem_set_writable_by_usb(fs_usermount, readonly);
    filesystem_set_concurrent_write_protection(fs_usermount, !disable_concurrent_write_protection);
    blockdev_unlock(fs_usermount);

    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
    usb_msc_remount(fs_usermount);
    #endif
}

void common_hal_storage_erase_filesystem(bool extended) {
    #if CIRCUITPY_USB_DEVICE
    usb_disconnect();
    #endif
    mp_hal_delay_ms(1000);
    #if CIRCUITPY_STORAGE_EXTEND
    supervisor_flash_set_extended(extended);
    #endif
    (void)filesystem_init(false, true);  // Force a re-format. Ignore failure.
    common_hal_mcu_on_next_reset(RUNMODE_NORMAL);
    common_hal_mcu_reset();
    // We won't actually get here, since we're resetting.
}
