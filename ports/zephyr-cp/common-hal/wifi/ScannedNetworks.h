// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"

#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>

#define MAX_BUFFERED_SCAN_RESULTS 10

typedef struct {
    mp_obj_base_t base;
    uint8_t current_channel_index;
    struct k_poll_signal channel_done;
    struct k_poll_event events[3];

    // Hold results as they move from the callback to the CP thread.
    char msgq_buffer[MAX_BUFFERED_SCAN_RESULTS * sizeof(struct wifi_scan_result)];
    struct k_msgq msgq;
    // Buffer the scan results before we return them. They are stored on the CP heap.
    struct k_fifo fifo;

    // Limits on what channels to scan.
    uint8_t start_channel;
    uint8_t end_channel; // Inclusive

    struct net_if *netif;

    bool done;
    bool channel_scan_in_progress;
} wifi_scannednetworks_obj_t;

void wifi_scannednetworks_scan_result(wifi_scannednetworks_obj_t *self, struct wifi_scan_result *result);
void wifi_scannednetworks_scan_next_channel(wifi_scannednetworks_obj_t *self);
void wifi_scannednetworks_deinit(wifi_scannednetworks_obj_t *self);
