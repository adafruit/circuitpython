// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2015 Josef Gajdusek
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "extmod/vfs_fat.h"
#include "py/obj.h"
#include "py/objnamedtuple.h"
#include "py/runtime.h"
#include "shared-bindings/storage/__init__.h"
#include "supervisor/flash.h"

//| """Storage management
//|
//| The `storage` provides storage management functionality such as mounting and
//| unmounting which is typically handled by the operating system hosting Python.
//| CircuitPython does not have an OS, so this module provides this functionality
//| directly.
//|
//| For more information regarding using the `storage` module, refer to the `CircuitPython
//| Essentials Learn guide
//| <https://learn.adafruit.com/circuitpython-essentials/circuitpython-storage>`_.
//| """
//|
//|
//| def mount(filesystem: VfsFat, mount_path: str, *, readonly: bool = False) -> None:
//|     """Mounts the given filesystem object at the given path.
//|
//|     This is the CircuitPython analog to the UNIX ``mount`` command.
//|
//|     :param VfsFat filesystem: The filesystem to mount.
//|     :param str mount_path: Where to mount the filesystem.
//|     :param bool readonly: True when the filesystem should be readonly to CircuitPython.
//|     """
//|     ...
//|
//|
static mp_obj_t storage_mount(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_filesystem, ARG_mount_path, ARG_readonly };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_filesystem, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_mount_path, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_readonly, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the mount point
    const char *mnt_str = mp_obj_str_get_str(args[ARG_mount_path].u_obj);


    mp_obj_t vfs_obj = args[ARG_filesystem].u_obj;

    // Currently, the only supported filesystem is VfsFat.
    mp_arg_validate_type(vfs_obj, &mp_fat_vfs_type, MP_QSTR_filesystem);

    common_hal_storage_mount(vfs_obj, mnt_str, args[ARG_readonly].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(storage_mount_obj, 0, storage_mount);

//| def umount(mount: Union[str, VfsFat]) -> None:
//|     """Unmounts the given filesystem object or if *mount* is a path, then unmount
//|     the filesystem mounted at that location.
//|
//|     This is the CircuitPython analog to the UNIX ``umount`` command."""
//|     ...
//|
//|
static mp_obj_t storage_umount(mp_obj_t mnt_in) {
    if (mp_obj_is_str(mnt_in)) {
        common_hal_storage_umount_path(mp_obj_str_get_str(mnt_in));
    } else {
        common_hal_storage_umount_object(mnt_in);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(storage_umount_obj, storage_umount);

//| def remount(
//|     mount_path: str,
//|     readonly: bool = False,
//|     *,
//|     disable_concurrent_write_protection: bool = False,
//| ) -> None:
//|     """Remounts the given path with new parameters.
//|
//|     This can always be done from ``boot.py``. After boot, it can only be done when the host computer
//|     doesn't have write access and CircuitPython isn't currently writing to the filesystem. An
//|     exception will be raised if this is the case. Some host OSes allow you to eject a drive which
//|     will allow for remounting.
//|
//|     Remounting after USB is active may take a little time because it "ejects" the drive for one
//|     query from the host. These queries happen every second or so.
//|
//|     :param str mount_path: The path to remount.
//|     :param bool readonly: True when the filesystem should be readonly to CircuitPython.
//|     :param bool disable_concurrent_write_protection: When True, the check that makes sure the
//|       underlying filesystem data is written by one computer is disabled. Disabling the protection
//|       allows CircuitPython and a host to write to the same filesystem with the risk that the
//|       filesystem will be corrupted."""
//|     ...
//|
//|
static mp_obj_t storage_remount(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mount_path, ARG_readonly, ARG_disable_concurrent_write_protection };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mount_path, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_readonly, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_disable_concurrent_write_protection, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *mnt_str = mp_obj_str_get_str(args[ARG_mount_path].u_obj);

    common_hal_storage_remount(mnt_str, args[ARG_readonly].u_bool, args[ARG_disable_concurrent_write_protection].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(storage_remount_obj, 0, storage_remount);

//| def getmount(mount_path: str) -> VfsFat:
//|     """Retrieves the mount object associated with the mount path"""
//|     ...
//|
//|
static mp_obj_t storage_getmount(const mp_obj_t mnt_in) {
    return common_hal_storage_getmount(mp_obj_str_get_str(mnt_in));
}
MP_DEFINE_CONST_FUN_OBJ_1(storage_getmount_obj, storage_getmount);

//| def erase_filesystem(extended: Optional[bool] = None) -> None:
//|     """Erase and re-create the **CIRCUITPY** filesystem.
//|
//|     On boards that present USB-visible **CIRCUITPY** drive (e.g., SAMD21 and SAMD51),
//|     then call `microcontroller.reset()` to restart CircuitPython and have the
//|     host computer remount **CIRCUITPY**.
//|
//|     This function can be called from the REPL when **CIRCUITPY**
//|     has become corrupted.
//|
//|     :param bool extended: On boards that support ``dualbank`` module
//|         and the ``extended`` parameter, the **CIRCUITPY** storage can be
//|         extended by setting this to `True`. If this isn't provided or
//|         set to `None` (default), the existing configuration will be used.
//|
//|     .. note:: New firmware starts with storage extended. In case of an existing
//|          filesystem (e.g. uf2 load), the existing extension setting is preserved.
//|
//|     .. warning:: All the data on **CIRCUITPY** will be lost, and
//|         CircuitPython will restart on certain boards."""
//|     ...
//|
//|

static mp_obj_t storage_erase_filesystem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_extended };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_extended, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if CIRCUITPY_STORAGE_EXTEND
    bool extended = (args[ARG_extended].u_obj == mp_const_none) ? supervisor_flash_get_extended() : mp_obj_is_true(args[ARG_extended].u_obj);
    common_hal_storage_erase_filesystem(extended);
    #else
    if (mp_obj_is_true(args[ARG_extended].u_obj)) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q=%q"), MP_QSTR_extended, MP_QSTR_True);
    }
    common_hal_storage_erase_filesystem(false);
    #endif

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(storage_erase_filesystem_obj, 0, storage_erase_filesystem);

//| def disable_usb_drive() -> None:
//|     """Disable presenting **CIRCUITPY** as a USB mass storage device.
//|     By default, the device is enabled and **CIRCUITPY** is visible, if USB is available.
//|     Must called in ``boot.py``, before USB is connected.
//      If you want to disable the USB drive after `boot.py` has run, see `unsafe_disable_usb_drive()`.
//|     """
//|     ...
//|
//|
static mp_obj_t storage_disable_usb_drive(void) {
    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
    if (!common_hal_storage_disable_usb_drive()) {
    #else
    if (true) {
        #endif
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(storage_disable_usb_drive_obj, storage_disable_usb_drive);

//| def unsafe_disable_usb_drive() -> None:
//|     """Disable presenting **CIRCUITPY** as a USB mass storage device even if in use.
//|     By default, the device is enabled and **CIRCUITPY** is visible.
//|     After the call, **CIRCUITPY** will be read/write to your code and from the REPL but not appear over USB.
//|
//|     Unlike `disable_usb_drive()`, `unsafe_disable_usb_drive()` can be called
//|     after ``code.py`` starts or from the REPL, after USB has started.
//|
//|     .. warning:: If ``unsafe_disable_usb_drive()`` is called when the host is in the middle of writing **CIRCUITPY**,
//|       filesystem corruption can occur.
//|       It is similar to the sudden physical removal of a USB drive.
//|       Before calling ``unsafe_disable_usb_drive()``,
//|       make sure the host has finished any writes to **CIRCUITPY**.
//|
//|       * On Windows, do one of these:
//|
//|         * Eject ("Safely Remove") the **CIRCUITPY** drive.
//|         * Use a "sync" program, such as `Sysinternals Sync <https://learn.microsoft.com/en-us/sysinternals/downloads/sync>`__.
//|         * Programmatically call ``_commit()`` or ``_flushall()`` or similar.
//|
//|       * On Linux or macOS, do one of these:
//|
//|         * Eject (unmount) the **CIRCUITPY** drive.
//|         * Type ``sync`` in a terminal.
//|         * Programmatically call ``sync()`` or ``fsync()``.
//|
//|       * If none of the above are possible or convenient, wait several seconds to allow any writes to complete.
//|         This can be unreliable, as the interval to wait depends on the host operating system
//|         and how the drive is mounted.
//|         In some operating systems, you can specify that the drive be mounted as "sync on write",
//|         so that all writes happen immediately.
//|
//|     When `unsafe_disable_usb_drive()` is called after USB has started,
//|     the **CIRCUITPY** USB drive logical unit (LUN) will report as "not ready",
//|     causing the host to unmount it.
//|     The drive can be made ready and available again by calling `enable_usb_drive()`.
//|     When `disable_usb_drive` is called after ``code.py`` starts or in the REPL,
//|     the call will delay 2.5 seconds before returning,
//|     so that host has time to detect that the drive is not ready.
//|     The host polls the device approximately every one or two seconds.
//|
//|     When the USB drive is disabled, **CIRCUITPY** becomes read/write, and can be written
//|     from user code or the REPL. This is easier than arranging for a `remount()` in ``boot.py``.
//|     Code editors and file uploaders can use this feature to write files via the REPL.
//|
//|     If `unsafe_disable_usb_drive()` is called in ``boot.py``, it is identical to calling
//|     `disable_usb_drive()`.
//|     """
//|     ...
//|
//|
static mp_obj_t storage_unsafe_disable_usb_drive(void) {
    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
    if (!common_hal_storage_unsafe_disable_usb_drive()) {
    #else
    if (true) {
        #endif
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(storage_unsafe_disable_usb_drive_obj, storage_unsafe_disable_usb_drive);

//| def enable_usb_drive() -> None:
//|     """Enable presenting **CIRCUITPY** as a USB mass storage device.
//|     By default, the device is enabled and **CIRCUITPY** is visible,
//|     so you do not normally need to call this function in ``boot.py``.
//|
//|     If you call `enable_usb_drive()` after ``code.py`` starts or in the REPL,
//|     you can reverse the effect of a previous `unsafe_disable_usb_drive()`.
//|     The **CIRCUITPY** drive will reappear to the host, and become read-only again
//|     if it was previously read-only.
//|
//|     If you enable too many USB devices at once, you will run out of USB endpoints.
//|     The number of available endpoints varies by microcontroller.
//|     CircuitPython will go into safe mode after running ``boot.py`` to inform you if
//|     not enough endpoints are available.
//|     """
//|     ...
//|
//|
static mp_obj_t storage_enable_usb_drive(void) {
    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
    if (!common_hal_storage_enable_usb_drive()) {
    #else
    if (true) {
        #endif
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(storage_enable_usb_drive_obj, storage_enable_usb_drive);


//| def map_file(file: typing.BinaryIO) -> Tuple[memoryview, ...]:
//|     """Map an open file on the internal CIRCUITPY drive straight from flash, without copying it
//|     into RAM. Returns one read-only `memoryview` per contiguous run of the file's clusters, in
//|     file order: a contiguous file gives a 1-tuple, an empty file an empty tuple. Run boundaries
//|     fall on cluster boundaries. The views stay valid after the file is closed, and anything that
//|     takes a buffer can use them: a `synthio.MidiTrack`, an `audiocore.RawSample`, a wavetable.
//|
//|     Treat the file as read-only while a view is in use: rewriting it shows the new bytes, and
//|     mid-write it shows a torn file.
//|
//|     :param typing.BinaryIO file: A file on the CIRCUITPY drive open for binary reading (``"rb"``)
//|     :raises OSError: ``EINVAL`` if the file is closed or not open for reading only,
//|       ``EOPNOTSUPP`` if it is on another mount or this build cannot map the drive,
//|       ``EIO`` if its cluster chain is corrupt
//|     :raises ~builtins.MemoryError: if ``open()`` could not allocate the file's cluster map
//|     :raises NotImplementedError: on a port whose drive is not memory-mapped"""
//|     ...
static mp_obj_t storage_map_file(mp_obj_t file_in) {
    return common_hal_storage_map_file(file_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(storage_map_file_obj, storage_map_file);

static const mp_rom_map_elem_t storage_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_storage) },

    { MP_ROM_QSTR(MP_QSTR_mount),                    MP_ROM_PTR(&storage_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount),                   MP_ROM_PTR(&storage_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_remount),                  MP_ROM_PTR(&storage_remount_obj) },
    { MP_ROM_QSTR(MP_QSTR_getmount),                 MP_ROM_PTR(&storage_getmount_obj) },
    { MP_ROM_QSTR(MP_QSTR_map_file),                 MP_ROM_PTR(&storage_map_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_erase_filesystem),         MP_ROM_PTR(&storage_erase_filesystem_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_usb_drive),        MP_ROM_PTR(&storage_disable_usb_drive_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_usb_drive),         MP_ROM_PTR(&storage_enable_usb_drive_obj) },
    { MP_ROM_QSTR(MP_QSTR_unsafe_disable_usb_drive), MP_ROM_PTR(&storage_unsafe_disable_usb_drive_obj) },

//| class VfsFat:
//|     def __init__(self, block_device: BlockDevice) -> None:
//|         """Create a new VfsFat filesystem around the given block device.
//|
//|         :param block_device: Block device the the filesystem lives on"""
//|
//|     label: str
//|     """The filesystem label, up to 11 case-insensitive bytes.  Note that
//|     this property can only be set when the device is writable by the
//|     microcontroller."""
//|     ...
//|     readonly: bool
//|     """``True`` when the device is mounted as readonly by the microcontroller.
//|     This property cannot be changed, use `storage.remount` instead."""
//|     ...
//|
//|     @staticmethod
//|     def mkfs(block_device: BlockDevice) -> None:
//|         """Format the block device, deleting any data that may have been there.
//|
//|         **Limitations**: On SAMD21 builds, `mkfs()` will raise ``OSError(22)`` when
//|         attempting to format filesystems larger than 4GB. The extra code to format larger
//|         filesystems will not fit on these builds. You can still access
//|         larger filesystems, but you will need to format the filesystem on another device.
//|         """
//|         ...
//|
//|     def open(self, path: str, mode: str) -> None:
//|         """Like builtin ``open()``"""
//|         ...
//|
//|     def ilistdir(
//|         self, path: str
//|     ) -> Iterator[Union[Tuple[AnyStr, int, int, int], Tuple[AnyStr, int, int]]]:
//|         """Return an iterator whose values describe files and folders within
//|         ``path``"""
//|         ...
//|
//|     def mkdir(self, path: str) -> None:
//|         """Like `os.mkdir`"""
//|         ...
//|
//|     def rmdir(self, path: str) -> None:
//|         """Like `os.rmdir`"""
//|         ...
//|
//|     def stat(self, path: str) -> Tuple[int, int, int, int, int, int, int, int, int, int]:
//|         """Like `os.stat`"""
//|         ...
//|
//|     def statvfs(self, path: int) -> Tuple[int, int, int, int, int, int, int, int, int, int]:
//|         """Like `os.statvfs`"""
//|         ...
//|
//|     def mount(self, readonly: bool, mkfs: VfsFat) -> None:
//|         """Don't call this directly, call `storage.mount`."""
//|         ...
//|
//|     def umount(self) -> None:
//|         """Don't call this directly, call `storage.umount`."""
//|         ...
//|
//|
    { MP_ROM_QSTR(MP_QSTR_VfsFat), MP_ROM_PTR(&mp_fat_vfs_type) },
};

static MP_DEFINE_CONST_DICT(storage_module_globals, storage_module_globals_table);

const mp_obj_module_t storage_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&storage_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_storage, storage_module);
