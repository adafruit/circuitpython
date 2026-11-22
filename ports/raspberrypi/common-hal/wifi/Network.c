// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/objlist.h"

#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/AuthMode.h"

mp_obj_t common_hal_wifi_network_get_ssid(wifi_network_obj_t *self) {
    const char *cstr = (const char *)self->record.ssid;
    return mp_obj_new_str(cstr, self->record.ssid_len);
}

#define MAC_ADDRESS_LENGTH 6

mp_obj_t common_hal_wifi_network_get_bssid(wifi_network_obj_t *self) {
    return mp_obj_new_bytes(self->record.bssid, MAC_ADDRESS_LENGTH);
}

mp_obj_t common_hal_wifi_network_get_rssi(wifi_network_obj_t *self) {
    return mp_obj_new_int(self->record.rssi);
}

mp_obj_t common_hal_wifi_network_get_channel(wifi_network_obj_t *self) {
    return mp_obj_new_int(self->record.channel);
}

mp_obj_t common_hal_wifi_network_get_country(wifi_network_obj_t *self) {
    return MP_OBJ_NEW_QSTR(MP_QSTR_);
}

mp_obj_t common_hal_wifi_network_get_authmode(wifi_network_obj_t *self) {
    uint32_t auth = self->record.auth_mode;
    uint32_t authmode_mask = 0;
    if (auth == 0) {
        authmode_mask = AUTHMODE_OPEN;
    }
    if (auth & CYW43_AUTH_FLAG_WPA_SECURITY) {
        authmode_mask |= AUTHMODE_WPA;
    }
    if (auth & CYW43_AUTH_FLAG_WPA2_SECURITY) {
        authmode_mask |= AUTHMODE_WPA2;
    }
    if (auth & CYW43_AUTH_FLAG_WPA3_SECURITY) {
        authmode_mask |= AUTHMODE_WPA3;
    }
    if (auth & CYW43_AUTH_FLAG_ENTERPRISE_ENABLED) {
        authmode_mask |= AUTHMODE_ENTERPRISE;
    } else if (authmode_mask & (AUTHMODE_WPA | AUTHMODE_WPA2 | AUTHMODE_WPA3)) {
        authmode_mask |= AUTHMODE_PSK;
    } else if (auth & CYW43_AUTH_FLAG_WEP_ENABLED) {
        authmode_mask |= AUTHMODE_WEP;
    }
    mp_obj_t authmode_list = mp_obj_new_list(0, NULL);
    if (authmode_mask != 0) {
        for (uint32_t i = 0; i < 32; i++) {
            if ((authmode_mask >> i) & 1) {
                mp_obj_list_append(authmode_list, cp_enum_find(&wifi_authmode_type, 1 << i));
            }
        }
    }
    return authmode_list;
}
