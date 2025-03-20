// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2017 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"
#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/Radio.h"
#include "shared-bindings/wifi/ScannedNetworks.h"

#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>


void wifi_scannednetworks_scan_result(wifi_scannednetworks_obj_t *self, struct wifi_scan_result *result) {
    if (k_msgq_put(&self->msgq, result, K_NO_WAIT) != 0) {
        printk("Dropping scan result!\n");
    }
}

static void wifi_scannednetworks_done(wifi_scannednetworks_obj_t *self) {
    self->done = true;
}

static bool wifi_scannednetworks_wait_for_scan(wifi_scannednetworks_obj_t *self) {

    return !mp_hal_is_interrupted();
}

mp_obj_t common_hal_wifi_scannednetworks_next(wifi_scannednetworks_obj_t *self) {
    if (self->done) {
        return mp_const_none;
    }
    // If we don't have any results queued, then wait until we do.
    while (k_fifo_is_empty(&self->fifo) && k_msgq_num_used_get(&self->msgq) == 0) {
        k_poll(self->events, ARRAY_SIZE(self->events), K_FOREVER);
        if (mp_hal_is_interrupted()) {
            wifi_scannednetworks_done(self);
        }
        if (k_msgq_num_used_get(&self->msgq) > 0) {
            // We found something.
            break;
        }
        int signaled;
        int result;
        k_poll_signal_check(&self->channel_done, &signaled, &result);
        if (signaled) {
            wifi_scannednetworks_scan_next_channel(self);
        }
        if (self->done) {
            return mp_const_none;
        }
    }
    // Copy everything out of the message queue into the FIFO because it's a
    // fixed size.
    while (k_msgq_num_used_get(&self->msgq) > 0) {
        wifi_network_obj_t *entry = mp_obj_malloc(wifi_network_obj_t, &wifi_network_type);
        k_msgq_get(&self->msgq, &entry->scan_result, K_NO_WAIT);
        // This will use the base python object space for the linked list. We
        // need to reset it before returning this memory as a Python object.
        k_fifo_put(&self->fifo, entry);
    }

    wifi_network_obj_t *entry = k_fifo_get(&self->fifo, K_NO_WAIT);
    entry->base.type = &wifi_network_type;
    return MP_OBJ_FROM_PTR(entry);
}

// We don't do a linear scan so that we look at a variety of spectrum up front.
static uint8_t scan_pattern[] = {6, 1, 11, 3, 9, 13, 2, 4, 8, 12, 5, 7, 10, 14, 0};

void wifi_scannednetworks_scan_next_channel(wifi_scannednetworks_obj_t *self) {
    // There is no channel 0, so use that as a flag to indicate we've run out of channels to scan.
    uint8_t next_channel = 0;
    while (self->current_channel_index < sizeof(scan_pattern)) {
        next_channel = scan_pattern[self->current_channel_index];
        self->current_channel_index++;
        // Scan only channels that are in the specified range.
        if (self->start_channel <= next_channel && next_channel <= self->end_channel) {
            break;
        }
    }
    k_poll_signal_init(&self->channel_done);
    k_poll_event_init(&self->events[2],
        K_POLL_TYPE_SIGNAL,
        K_POLL_MODE_NOTIFY_ONLY,
        &self->channel_done);

    struct wifi_scan_params params = { 0 };
    params.band_chan[0].band = WIFI_FREQ_BAND_2_4_GHZ;
    params.band_chan[0].channel = next_channel;
    if (next_channel == 0) {
        wifi_scannednetworks_done(self);
    } else {
        int res = net_mgmt(NET_REQUEST_WIFI_SCAN, self->netif, &params, sizeof(params));
        if (res != 0) {
            printk("Failed to start wifi scan %d\n", res);
            raise_zephyr_error(res);
            wifi_scannednetworks_done(self);
        } else {
            self->channel_scan_in_progress = true;
        }
    }
}

void wifi_scannednetworks_deinit(wifi_scannednetworks_obj_t *self) {
    // Free any results we don't need.
    while (!k_fifo_is_empty(&self->fifo)) {
        wifi_network_obj_t *entry = k_fifo_get(&self->fifo, K_NO_WAIT);

        #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
        m_free(entry, sizeof(wifi_network_obj_t));
        #else
        m_free(entry);
        #endif
    }
    wifi_scannednetworks_done(self);
}
