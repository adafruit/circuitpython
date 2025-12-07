// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"
#include "shared-bindings/wifi/AuthMode.h"
#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/Radio.h"
#include "shared-bindings/wifi/ScannedNetworks.h"

mp_obj_t wifi_scannednetworks_do_scan(wifi_radio_obj_t *self) {
    uint8_t ssids[AIRLIFT_MAX_NETWORKS][MAX_SSID_LENGTH];
    uint8_t *network_responses[AIRLIFT_MAX_NETWORKS];
    size_t network_response_lengths[AIRLIFT_MAX_NETWORKS];
    // Set the maximum size for each SSID response. They may be shorter.
    for (size_t i = 0; i < AIRLIFT_MAX_NETWORKS; i++) {
        network_responses[i] = ssids[i];
        network_response_lengths[i] = MAX_SSID_LENGTH;
    }

    size_t num_networks = wifi_radio_send_command_get_response(self, SCAN_NETWORKS,
        NULL, NULL, LENGTHS_8, 0,
        network_responses, network_response_lengths, LENGTHS_8, AIRLIFT_MAX_NETWORKS,
//        AIRLIFT_DEFAULT_TIMEOUT_MS);
        10000);

    // Now fetch each network's details and store them in Network objects.
    uint8_t network_idx;
    size_t param_lengths[1] = { 1 };
    const uint8_t *params[1] = { &network_idx };

    mp_obj_t networks[AIRLIFT_MAX_NETWORKS];

    for (network_idx = 0; network_idx < num_networks; network_idx++) {
        wifi_network_obj_t *network = mp_obj_malloc(wifi_network_obj_t, &wifi_network_type);
        networks[network_idx] = network;

        network->ssid = mp_obj_new_str(
            (const char *)network_responses[network_idx],
            network_response_lengths[network_idx]);


        {
            uint8_t bssid[MAC_ADDRESS_LENGTH] = { 0 };
            uint8_t *responses[1] = { bssid };
            size_t response_lengths[1] = { sizeof(bssid) };

            network->bssid = mp_const_none;
            if (wifi_radio_send_command_get_response(self, GET_IDX_BSSID_CMD,
                params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
                responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
                AIRLIFT_DEFAULT_TIMEOUT_MS) > 0) {
                network->bssid = mp_obj_new_bytes(bssid, MAC_ADDRESS_LENGTH);
            }
        }

        {
            int32_t rssi = 0;
            uint8_t *responses[1] = { (uint8_t *)&rssi };
            size_t response_lengths[1] = { sizeof(rssi) };

            network->rssi = mp_const_none;
            if (wifi_radio_send_command_get_response(self, GET_IDX_RSSI_CMD,
                params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
                responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
                AIRLIFT_DEFAULT_TIMEOUT_MS) > 0) {
                network->rssi = mp_obj_new_int(rssi);
            }
        }

        {
            uint8_t channel = 0;
            uint8_t *responses[1] = { &channel };
            size_t response_lengths[1] = { sizeof(channel) };

            network->channel = mp_const_none;
            if (wifi_radio_send_command_get_response(self, GET_IDX_CHAN_CMD,
                params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
                responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
                AIRLIFT_DEFAULT_TIMEOUT_MS) > 0) {
                network->channel = mp_obj_new_int(channel);
            }
        }

        {
            uint8_t esp_authmode = 0;
            uint8_t *responses[1] = { &esp_authmode };
            size_t response_lengths[1] = { sizeof(esp_authmode) };

            if (wifi_radio_send_command_get_response(self, GET_IDX_ENCT_CMD,
                params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
                responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
                AIRLIFT_DEFAULT_TIMEOUT_MS) > 0) {

                // Convert AirLift authmode to tuple of wifi.AuthMode objects.
                network->authmode = esp_authmode_to_wifi_authmode_tuple(esp_authmode);
            }

            // country code not supported on AirLift.
            network->country = MP_OBJ_NEW_QSTR(MP_QSTR_);
        }
    }

    // Create a ScannedNetworks obj and populate it with the results of the scan.
    wifi_scannednetworks_obj_t *scanned_networks =
        mp_obj_malloc(wifi_scannednetworks_obj_t, &wifi_scannednetworks_type);
    scanned_networks->networks = mp_obj_new_tuple(num_networks, networks);

    return MP_OBJ_FROM_PTR(scanned_networks);
}

mp_obj_t common_hal_wifi_scannednetworks_next(wifi_scannednetworks_obj_t *self) {
    if (self->next_network_index >= self->networks->len) {
        return mp_const_none;
    }

    return self->networks->items[self->next_network_index++];
}

void wifi_scannednetworks_deinit(wifi_scannednetworks_obj_t *self) {
}
