// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-bindings/wifi/ScannedNetworks.h"
#include "shared-bindings/wifi/Network.h"

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi.h>

// Event bits for the Radio event group.
#define WIFI_SCAN_DONE_BIT BIT0
#define WIFI_CONNECTED_BIT BIT1
#define WIFI_DISCONNECTED_BIT BIT2

typedef struct {
    mp_obj_base_t base;
    wifi_scannednetworks_obj_t *current_scan;
    // StaticEventGroup_t event_group;
    // EventGroupHandle_t event_group_handle;
    // wifi_config_t sta_config;
    // wifi_network_obj_t ap_info;
    // esp_netif_ip_info_t ip_info;
    // esp_netif_dns_info_t dns_info;
    struct net_if *sta_netif;
    // uint32_t ping_elapsed_time;
    // wifi_config_t ap_config;
    // esp_netif_ip_info_t ap_ip_info;
    struct net_if *ap_netif;
    bool started;
    bool ap_mode;
    bool sta_mode;
    uint8_t retries_left;
    uint8_t starting_retries;
    uint8_t last_disconnect_reason;
    // Signalled from the net_mgmt event handler when a connect attempt
    // finishes, so common_hal_wifi_radio_connect() can wait on the result.
    struct k_sem connect_sem;
    // Latest wifi_conn_status from NET_EVENT_WIFI_CONNECT_RESULT.
    int last_connect_status;
    bool connected;
    // SSID of the association that `connected` refers to, so that a connect()
    // for the network we are already on can return without touching the link.
    uint8_t current_ssid[WIFI_SSID_MAX_LEN];
    size_t current_ssid_len;
} wifi_radio_obj_t;

extern void common_hal_wifi_radio_gc_collect(wifi_radio_obj_t *self);
