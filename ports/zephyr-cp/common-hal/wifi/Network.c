// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/AuthMode.h"

mp_obj_t common_hal_wifi_network_get_ssid(wifi_network_obj_t *self) {
    const char *cstr = (const char *)self->scan_result.ssid;
    return mp_obj_new_str(cstr, self->scan_result.ssid_length);
}

mp_obj_t common_hal_wifi_network_get_bssid(wifi_network_obj_t *self) {
    return mp_obj_new_bytes(self->scan_result.mac, self->scan_result.mac_length);
}

mp_obj_t common_hal_wifi_network_get_rssi(wifi_network_obj_t *self) {
    return mp_obj_new_int(self->scan_result.rssi);
}

mp_obj_t common_hal_wifi_network_get_channel(wifi_network_obj_t *self) {
    return mp_obj_new_int(self->scan_result.channel);
}

mp_obj_t common_hal_wifi_network_get_country(wifi_network_obj_t *self) {
    // const char *cstr = (const char *)self->record.country.cc;
    // 2 instead of strlen(cstr) as this gives us only the country-code
    // return mp_obj_new_str(cstr, 2);
    return mp_const_none;
}

mp_obj_t common_hal_wifi_network_get_authmode(wifi_network_obj_t *self) {
    // Translate Zephyr's wifi_security_type. An empty list would read as an
    // open network to any caller checking for AUTHMODE_OPEN.
    uint32_t authmode_mask = 0;
    switch (self->scan_result.security) {
        case WIFI_SECURITY_TYPE_NONE:
            authmode_mask = AUTHMODE_OPEN;
            break;
        case WIFI_SECURITY_TYPE_WEP:
            authmode_mask = AUTHMODE_WEP;
            break;
        case WIFI_SECURITY_TYPE_WPA_PSK:
            authmode_mask = AUTHMODE_WPA | AUTHMODE_PSK;
            break;
        case WIFI_SECURITY_TYPE_PSK:
        case WIFI_SECURITY_TYPE_PSK_SHA256:
            authmode_mask = AUTHMODE_WPA2 | AUTHMODE_PSK;
            break;
        case WIFI_SECURITY_TYPE_SAE:      // == WIFI_SECURITY_TYPE_SAE_HNP (alias)
        case WIFI_SECURITY_TYPE_SAE_H2E:
        case WIFI_SECURITY_TYPE_SAE_AUTO:
        case WIFI_SECURITY_TYPE_SAE_EXT_KEY:
        case WIFI_SECURITY_TYPE_FT_SAE:
            authmode_mask = AUTHMODE_WPA3 | AUTHMODE_PSK;
            break;
        case WIFI_SECURITY_TYPE_WPA_AUTO_PERSONAL:
            authmode_mask = AUTHMODE_WPA | AUTHMODE_WPA2 | AUTHMODE_WPA3 | AUTHMODE_PSK;
            break;
        case WIFI_SECURITY_TYPE_EAP:      // == WIFI_SECURITY_TYPE_EAP_TLS (alias)
        case WIFI_SECURITY_TYPE_EAP_PEAP_MSCHAPV2:
        case WIFI_SECURITY_TYPE_EAP_PEAP_GTC:
        case WIFI_SECURITY_TYPE_EAP_TTLS_MSCHAPV2:
        case WIFI_SECURITY_TYPE_EAP_PEAP_TLS:
        case WIFI_SECURITY_TYPE_FT_EAP:
            authmode_mask = AUTHMODE_WPA2 | AUTHMODE_ENTERPRISE;
            break;
        default:
            break;
    }
    mp_obj_t authmode_list = mp_obj_new_list(0, NULL);
    if (authmode_mask != 0) {
        for (uint8_t i = 0; i < 32; i++) {
            if ((authmode_mask >> i) & 1) {
                mp_obj_list_append(authmode_list, cp_enum_find(&wifi_authmode_type, 1 << i));
            }
        }
    }
    return authmode_list;
}
