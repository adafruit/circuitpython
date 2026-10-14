// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Adaboot identity for the SMP "bootloader info" command. Zephyr's
// application-side os_mgmt handler reports the hardcoded "MCUboot" and
// derives the boot mode from the application's own Kconfig. This hook
// answers with the fork's name and fills mode/running slot from the BLINFO
// TLVs the bootloader left in the zephyr,bootloader-info shared-data
// retention area (written by boot_write_shared_data() on every boot).

#include <string.h>

#include <zephyr/init.h>

#if defined(CONFIG_MCUMGR_GRP_OS_BOOTLOADER_INFO_HOOK) && \
    defined(CONFIG_RETENTION_BOOTLOADER_INFO_TYPE_MCUBOOT)

/* The bootloader-info hook API is part of the MCUmgr/SMP stack: the event's
 * data struct embeds a live zcbor response encoder and a zcbor_string query,
 * and the hook appends its answers with zcbor put calls. The headers (and
 * Zephyr's zcbor include path, which only exists when CONFIG_MCUMGR selects
 * ZCBOR) are therefore guarded along with the hook: SMP boards compile it
 * in, no-Bluetooth boards like frdm_mcxn947 compile the whole hook out and
 * never need the headers. */
#include <zephyr/kernel.h>
#include <zephyr/mgmt/mcumgr/mgmt/callbacks.h>
#include <zephyr/mgmt/mcumgr/grp/os_mgmt/os_mgmt.h>
#include <zephyr/mgmt/mcumgr/grp/os_mgmt/os_mgmt_callbacks.h>
#include <zephyr/retention/blinfo.h>

#include <bootutil/boot_status.h>
#include <zcbor_encode.h>

// Only answer the queries we know how to fill from the shared data; leave
// everything else for Zephyr's handler (which reports the query error).
#define ADABOOT_INFO_NAME "Adaboot"

static enum mgmt_cb_return adaboot_info_event(uint32_t event,
    enum mgmt_cb_return prev_status, int32_t *rc,
    uint16_t *group, bool *abort_more, void *data,
    size_t data_size) {
    struct os_mgmt_bootloader_info_data *info = data;
    struct zcbor_string query = *info->query;
    int32_t value;

    if (*info->has_output) {
        // Another hook already answered; leave the response alone.
        return MGMT_CB_OK;
    }

    if (*info->decoded == 0) {
        // Empty query: identify the bootloader.
        if (zcbor_tstr_put_lit(info->zse, "bootloader") &&
            zcbor_tstr_put_lit(info->zse, ADABOOT_INFO_NAME)) {
            *info->has_output = true;
        }
        return MGMT_CB_OK;
    }

    if (query.len == 4 && memcmp(query.value, "mode", 4) == 0) {
        int rc = blinfo_lookup(BLINFO_MODE, (char *)&value, sizeof(value));
        if (rc == sizeof(value) &&
            zcbor_tstr_put_lit(info->zse, "mode") &&
            zcbor_int32_put(info->zse, value)) {
            *info->has_output = true;
        }
    } else if (query.len == 4 && memcmp(query.value, "slot", 4) == 0) {
        // Single-application layouts never write a running-slot TLV; leave
        // the query unanswered so Zephyr reports its usual error.
        int rc = blinfo_lookup(BLINFO_RUNNING_SLOT, (char *)&value, sizeof(value));
        if (rc == sizeof(value) &&
            zcbor_tstr_put_lit(info->zse, "slot") &&
            zcbor_int32_put(info->zse, value)) {
            *info->has_output = true;
        }
    }

    return MGMT_CB_OK;
}

static struct mgmt_callback adaboot_info_callback = {
    .callback = adaboot_info_event,
    .event_id = MGMT_EVT_OP_OS_MGMT_BOOTLOADER_INFO,
};

void adaboot_info_init(void) {
    mgmt_callback_register(&adaboot_info_callback);
}

#endif
