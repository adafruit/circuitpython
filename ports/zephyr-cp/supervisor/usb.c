#include "supervisor/usb.h"

#include "shared-bindings/usb_cdc/__init__.h"
#include "shared-bindings/usb_cdc/Serial.h"

#include "supervisor/zephyr-cp.h"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/diskio.h"
#include "lib/oofatfs/ff.h"

#include <zephyr/devicetree.h>
#include <zephyr/drivers/disk.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/kernel.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/class/usbd_msc.h>

#include "shared-module/storage/__init__.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/reload.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(usb, LOG_LEVEL_INF);

#define USB_DEVICE DT_NODELABEL(zephyr_udc0)

USBD_DEVICE_DEFINE(main_usbd,
    DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
    USB_VID, USB_PID);

USBD_DESC_LANG_DEFINE(main_lang);
USBD_DESC_MANUFACTURER_DEFINE(main_mfr, USB_MANUFACTURER);
USBD_DESC_PRODUCT_DEFINE(main_product, USB_PRODUCT);

USBD_DESC_CONFIG_DEFINE(fs_cfg_desc, "FS Configuration");
USBD_DESC_CONFIG_DEFINE(hs_cfg_desc, "HS Configuration");

/* doc configuration instantiation start */
static const uint8_t attributes = 0;

USBD_CONFIGURATION_DEFINE(main_fs_config,
    attributes,
    100, &fs_cfg_desc);

USBD_CONFIGURATION_DEFINE(main_hs_config,
    attributes,
    100, &hs_cfg_desc);

static usb_cdc_serial_obj_t usb_cdc_console_obj;
static usb_cdc_serial_obj_t usb_cdc_data_obj;

#ifndef USBD_DEFINE_MSC_LUN
#error "MSC not enabled"
#endif

#define LUN_COUNT 1
#define MSC_FLASH_BLOCK_SIZE    512

// The ellipsis range in the designated initializer of `ejected` is not standard C,
// but it works in both gcc and clang.
static bool locked[LUN_COUNT] = { [0 ... (LUN_COUNT - 1)] = false};

// Set to true if a write was in a file data or metadata area,
// as opposed to in the filesystem metadata area (e.g., dirty bit).
// Used to determine if an auto-reload is warranted.
static bool content_write[LUN_COUNT] = { [0 ... (LUN_COUNT - 1)] = false};

int _zephyr_disk_init(struct disk_info *disk);
int _zephyr_disk_status(struct disk_info *disk);
int _zephyr_disk_read(struct disk_info *disk, uint8_t *data_buf, uint32_t start_sector, uint32_t num_sector);
int _zephyr_disk_write(struct disk_info *disk, const uint8_t *data_buf, uint32_t start_sector, uint32_t num_sector);
int _zephyr_disk_ioctl(struct disk_info *disk, uint8_t cmd, void *buff);

static const struct disk_operations disk_ops = {
    .init = _zephyr_disk_init,
    .status = _zephyr_disk_status,
    .read = _zephyr_disk_read,
    .write = _zephyr_disk_write,
    .ioctl = _zephyr_disk_ioctl,
};

static struct disk_info circuitpy_disk = {
    .name = "CIRCUITPY",
    .ops = &disk_ops,
    .dev = NULL
};

USBD_DEFINE_MSC_LUN(circuitpy_lun, "CIRCUITPY", "Zephyr", "FlashDisk", "0.00");


int _zephyr_disk_init(struct disk_info *disk) {
    printk("Initializing disk\n");
    return 0;
}

int _zephyr_disk_status(struct disk_info *disk) {
    fs_user_mount_t *root = filesystem_circuitpy();
    int lun = 0;
    if (root == NULL) {
        printk("Status: No media\n");
        return DISK_STATUS_NOMEDIA;
    }
    if (!filesystem_is_writable_by_usb(root)) {
        printk("Status: Read-only\n");
        return DISK_STATUS_WR_PROTECT;
    }
    // Lock the blockdev once we say we're writable.
    if (!locked[lun] && !blockdev_lock(root)) {
        printk("Status: Locked\n");
        return DISK_STATUS_WR_PROTECT;
    }
    locked[lun] = true;
    return DISK_STATUS_OK;
}

int _zephyr_disk_read(struct disk_info *disk, uint8_t *data_buf, uint32_t start_sector, uint32_t num_sector) {
    fs_user_mount_t *root = filesystem_circuitpy();

    uint32_t disk_block_count;
    disk_ioctl(root, GET_SECTOR_COUNT, &disk_block_count);

    if (start_sector + num_sector > disk_block_count) {
        return -EIO;
    }
    disk_read(root, data_buf, start_sector, num_sector);
    return 0;
}

int _zephyr_disk_write(struct disk_info *disk, const uint8_t *data_buf, uint32_t start_sector, uint32_t num_sector) {
    fs_user_mount_t *root = filesystem_circuitpy();
    int lun = 0;
    autoreload_suspend(AUTORELOAD_SUSPEND_USB);
    disk_write(root, data_buf, start_sector, num_sector);
    // Since by getting here we assume the mount is read-only to
    // CircuitPython let's update the cached FatFs sector if it's the one
    // we just wrote.
    if
    #if FF_MAX_SS != FF_MIN_SS
    (root->fatfs.ssize == MSC_FLASH_BLOCK_SIZE)
    #else
    // The compiler can optimize this away.
    (FF_MAX_SS == FILESYSTEM_BLOCK_SIZE)
    #endif
    {
        if (start_sector == root->fatfs.winsect && start_sector > 0) {
            memcpy(root->fatfs.win,
                data_buf + MSC_FLASH_BLOCK_SIZE * (root->fatfs.winsect - start_sector),
                MSC_FLASH_BLOCK_SIZE);
        }
    }

    // A write to an lba below fatbase is in the filesystem metadata (BPB) area or the "Reserved Region",
    // and is probably setting or clearing the dirty bit. This should not trigger auto-reload.
    // All other writes will trigger auto-reload.
    if (start_sector >= root->fatfs.fatbase) {
        content_write[lun] = true;
    }
    return 0;
}

int _zephyr_disk_ioctl(struct disk_info *disk, uint8_t cmd, void *buff) {

    fs_user_mount_t *root = filesystem_circuitpy();
    int lun = 0;
    switch (cmd) {
        case DISK_IOCTL_GET_SECTOR_COUNT:
            disk_ioctl(root, GET_SECTOR_COUNT, buff);
            return 0;
        case DISK_IOCTL_GET_SECTOR_SIZE:
            disk_ioctl(root, GET_SECTOR_SIZE, buff);
            return 0;
        case DISK_IOCTL_CTRL_SYNC:
            disk_ioctl(root, CTRL_SYNC, buff);
            autoreload_resume(AUTORELOAD_SUSPEND_USB);

            // This write is complete; initiate an autoreload if this was a file data or metadata write,
            // not just a dirty-bit write.
            if (content_write[lun] && lun == 0) {
                autoreload_trigger();
                content_write[lun] = false;
            }
            return 0;
        default:
            printk("Unsupported disk ioctl %02x\n", cmd);
            return -ENOTSUP;
    }
    return 0;
}

static void _msg_cb(struct usbd_context *const ctx, const struct usbd_msg *msg) {
    LOG_INF("USBD message: %s", usbd_msg_type_string(msg->type));
}

void usb_init(void) {
    printk("Initializing USB\n");
    int err;

    printk("Adding language descriptor\n");
    err = usbd_add_descriptor(&main_usbd, &main_lang);
    if (err) {
        LOG_ERR("Failed to initialize language descriptor (%d)", err);
        return;
    }

    err = usbd_add_descriptor(&main_usbd, &main_mfr);
    if (err) {
        LOG_ERR("Failed to initialize manufacturer descriptor (%d)", err);
        return;
    }

    err = usbd_add_descriptor(&main_usbd, &main_product);
    if (err) {
        LOG_ERR("Failed to initialize product descriptor (%d)", err);
        return;
    }

    bool console = usb_cdc_console_enabled();
    if (console) {
        uint8_t *receiver_buffer = port_malloc(128, true);
        if (receiver_buffer != NULL) {
            common_hal_usb_cdc_serial_construct_from_device(&usb_cdc_console_obj, DEVICE_DT_GET(DT_NODELABEL(cdc_acm_console)), 128, receiver_buffer);
        } else {
            console = false;
        }
    }
    usb_cdc_set_console(console ? MP_OBJ_FROM_PTR(&usb_cdc_console_obj) : mp_const_none);

    bool data = usb_cdc_data_enabled();
    if (data) {
        uint8_t *receiver_buffer = port_malloc(128, true);
        if (receiver_buffer != NULL) {
            common_hal_usb_cdc_serial_construct_from_device(&usb_cdc_data_obj, DEVICE_DT_GET(DT_NODELABEL(cdc_acm_data)), 128, receiver_buffer);
        } else {
            data = false;
        }
    }
    usb_cdc_set_data(data ? MP_OBJ_FROM_PTR(&usb_cdc_data_obj) : mp_const_none);

    err = disk_access_register(&circuitpy_disk);
    if (err) {
        printk("Failed to register disk access %d\n", err);
        return;
    }

    if (USBD_SUPPORTS_HIGH_SPEED &&
        usbd_caps_speed(&main_usbd) == USBD_SPEED_HS) {
        printk("Adding High-Speed configuration\n");
        err = usbd_add_configuration(&main_usbd, USBD_SPEED_HS,
            &main_hs_config);
        if (err) {
            LOG_ERR("Failed to add High-Speed configuration");
            return;
        }

        printk("Registering High-Speed cdc_acm class\n");
        if (usb_cdc_console_enabled()) {
            err = usbd_register_class(&main_usbd, "cdc_acm_0", USBD_SPEED_HS, 1);
            if (err) {
                printk("Failed to add register classes %d\n", err);
                return;
            }
        }

        if (usb_cdc_data_enabled()) {
            err = usbd_register_class(&main_usbd, "cdc_acm_1", USBD_SPEED_HS, 1);
            if (err) {
                printk("Failed to add register classes %d\n", err);
                return;
            }
        }

        err = usbd_register_class(&main_usbd, "msc_0", USBD_SPEED_HS, 1);
        if (err) {
            printk("Failed to add register MSC class %d\n", err);
            return;
        } else {
            printk("Registered MSC class for high speed\n");
        }

        usbd_device_set_code_triple(&main_usbd, USBD_SPEED_HS,
            USB_BCC_MISCELLANEOUS, 0x02, 0x01);
    }

    /* doc configuration register start */
    printk("Adding Full-Speed configuration\n");
    err = usbd_add_configuration(&main_usbd, USBD_SPEED_FS,
        &main_fs_config);
    if (err) {
        LOG_ERR("Failed to add Full-Speed configuration");
        return;
    }
    /* doc configuration register end */

    /* doc functions register start */

    if (usb_cdc_console_enabled()) {
        printk("Registering Full-Speed cdc_acm class\n");
        err = usbd_register_class(&main_usbd, "cdc_acm_0", USBD_SPEED_FS, 1);
        if (err) {
            printk("Failed to add register classes\n");
            return;
        }
    }

    if (usb_cdc_data_enabled()) {
        printk("Registering Full-Speed cdc_acm class\n");
        err = usbd_register_class(&main_usbd, "cdc_acm_1", USBD_SPEED_FS, 1);
        if (err) {
            printk("Failed to add register classes\n");
            return;
        }
    }

    err = usbd_register_class(&main_usbd, "msc_0", USBD_SPEED_FS, 1);
    if (err) {
        printk("Failed to add register MSC class %d\n", err);
        return;
    }
    /* doc functions register end */

    usbd_device_set_code_triple(&main_usbd, USBD_SPEED_FS,
        USB_BCC_MISCELLANEOUS, 0x02, 0x01);
    printk("Setting self powered\n");
    usbd_self_powered(&main_usbd, attributes & USB_SCD_SELF_POWERED);

    printk("Registering callback\n");
    err = usbd_msg_register_cb(&main_usbd, _msg_cb);
    if (err) {
        LOG_ERR("Failed to register message callback");
        return;
    }

    printk("usbd_init\n");
    err = usbd_init(&main_usbd);
    if (err) {
        LOG_ERR("Failed to initialize device support");
        return;
    }

    printk("USB initialized\n");

    err = usbd_enable(&main_usbd);
    if (err) {
        LOG_ERR("Failed to enable device support");
        return;
    }
    printk("usbd enabled\n");
}

bool usb_connected(void) {
    return false;
}

void usb_disconnect(void) {
}

usb_cdc_serial_obj_t *usb_cdc_serial_get_console(void) {
    if (usb_cdc_console_enabled()) {
        return &usb_cdc_console_obj;
    }
    return NULL;
}
