// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/AuthMode.h"

// From esp_wifi_types_generic.h. Needs to match the enum used to build nina-fw.
typedef enum {
    WIFI_AUTH_OPEN = 0,         /**< Authenticate mode : open */
    WIFI_AUTH_WEP,              /**< Authenticate mode : WEP */
    WIFI_AUTH_WPA_PSK,          /**< Authenticate mode : WPA_PSK */
    WIFI_AUTH_WPA2_PSK,         /**< Authenticate mode : WPA2_PSK */
    WIFI_AUTH_WPA_WPA2_PSK,     /**< Authenticate mode : WPA_WPA2_PSK */
    WIFI_AUTH_ENTERPRISE,       /**< Authenticate mode : Wi-Fi EAP security, treated the same as WIFI_AUTH_WPA2_ENTERPRISE */
    WIFI_AUTH_WPA2_ENTERPRISE = WIFI_AUTH_ENTERPRISE,  /**< Authenticate mode : WPA2-Enterprise security */
    WIFI_AUTH_WPA3_PSK,         /**< Authenticate mode : WPA3_PSK */
    WIFI_AUTH_WPA2_WPA3_PSK,    /**< Authenticate mode : WPA2_WPA3_PSK */
    WIFI_AUTH_WAPI_PSK,         /**< Authenticate mode : WAPI_PSK */
    WIFI_AUTH_OWE,              /**< Authenticate mode : OWE */
    WIFI_AUTH_WPA3_ENT_192,     /**< Authenticate mode : WPA3_ENT_SUITE_B_192_BIT */
    WIFI_AUTH_WPA3_EXT_PSK,     /**< This authentication mode will yield same result as WIFI_AUTH_WPA3_PSK and not recommended to be used. It will be deprecated in future, please use WIFI_AUTH_WPA3_PSK instead. */
    WIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE, /**< This authentication mode will yield same result as WIFI_AUTH_WPA3_PSK and not recommended to be used. It will be deprecated in future, please use WIFI_AUTH_WPA3_PSK instead.*/
    WIFI_AUTH_DPP,              /**< Authenticate mode : DPP */
    WIFI_AUTH_WPA3_ENTERPRISE,  /**< Authenticate mode : WPA3-Enterprise Only Mode */
    WIFI_AUTH_WPA2_WPA3_ENTERPRISE, /**< Authenticate mode : WPA3-Enterprise Transition Mode */
    WIFI_AUTH_WPA_ENTERPRISE,   /**< Authenticate mode : WPA-Enterprise security */
    WIFI_AUTH_MAX
} wifi_auth_mode_t;

// Lookup table to convert authmodes.

typedef struct {
    uint8_t esp_authmode;
    wifi_authmode_t wifi_authmode;
} esp_authmode_to_wifi_authmode_t;

esp_authmode_to_wifi_authmode_t esp_authmode_to_wifi_authmode[] = {
    { WIFI_AUTH_OPEN, AUTHMODE_OPEN },
    { WIFI_AUTH_WEP, AUTHMODE_WEP },
    { WIFI_AUTH_WPA_PSK, AUTHMODE_WPA | AUTHMODE_PSK },
    { WIFI_AUTH_WPA_WPA2_PSK, AUTHMODE_WPA | AUTHMODE_WPA2 | AUTHMODE_PSK },
    { WIFI_AUTH_WPA_ENTERPRISE, AUTHMODE_WPA | AUTHMODE_ENTERPRISE },
    { WIFI_AUTH_WPA2_PSK, AUTHMODE_WPA2 | AUTHMODE_PSK },
    {WIFI_AUTH_WPA2_WPA3_PSK, AUTHMODE_WPA2 | AUTHMODE_WPA3 | AUTHMODE_PSK },
    // Same as WIFI_AUTH_ENTERPRISE.
    { WIFI_AUTH_WPA2_ENTERPRISE, AUTHMODE_WPA2 | AUTHMODE_ENTERPRISE },
    { WIFI_AUTH_WPA2_WPA3_ENTERPRISE, AUTHMODE_WPA2 | AUTHMODE_WPA3 | AUTHMODE_ENTERPRISE },
    { WIFI_AUTH_WPA3_PSK, AUTHMODE_WPA3 | AUTHMODE_PSK },
    { WIFI_AUTH_WPA3_ENTERPRISE, AUTHMODE_WPA3 | AUTHMODE_ENTERPRISE },
};

typedef struct {
    wifi_authmode_t wifi_authmode;
    const cp_enum_obj_t *authmode_obj;
} wifi_authmode_to_authmode_obj_t;

wifi_authmode_to_authmode_obj_t wifi_authmode_to_authmode_obj[] = {
    { AUTHMODE_OPEN, &authmode_OPEN_obj },
    { AUTHMODE_WEP, &authmode_WEP_obj },
    { AUTHMODE_PSK, &authmode_PSK_obj },
    { AUTHMODE_WPA, &authmode_WPA_obj },
    { AUTHMODE_WPA2, &authmode_WPA2_obj },
    { AUTHMODE_WPA3, &authmode_WPA3_obj },
    { AUTHMODE_ENTERPRISE, &authmode_ENTERPRISE_obj },
};

mp_obj_t esp_authmode_to_wifi_authmode_tuple(uint8_t esp_authmode) {
    wifi_authmode_t wifi_authmode_mask = 0;
    // First convert the esp_authmode from AirLift to a bitmask wifi_authmode_t.
    for (size_t i = 0; i < MP_ARRAY_SIZE(esp_authmode_to_wifi_authmode); i++) {
        if (esp_authmode_to_wifi_authmode[i].esp_authmode == esp_authmode) {
            wifi_authmode_mask = esp_authmode_to_wifi_authmode[i].wifi_authmode;
            break;
        }
    }

    // Then, make a tuple of all the AuthMode objects
    // corresponding to the bits set in the authmode bitmask.
    mp_obj_t authmode_objs[MP_ARRAY_SIZE(wifi_authmode_to_authmode_obj)];
    size_t len = 0;
    for (size_t i = 0; i < MP_ARRAY_SIZE(wifi_authmode_to_authmode_obj); i++) {
        if (wifi_authmode_to_authmode_obj[i].wifi_authmode & wifi_authmode_mask) {
            // Bit is set in bitmask.
            authmode_objs[len++] = MP_OBJ_FROM_PTR(wifi_authmode_to_authmode_obj[i].authmode_obj);
        }
    }

    // Save the objs as a tuple.
    return mp_obj_new_tuple(len, authmode_objs);
}

mp_obj_t common_hal_wifi_network_get_ssid(wifi_network_obj_t *self) {
    return self->ssid;
}

mp_obj_t common_hal_wifi_network_get_bssid(wifi_network_obj_t *self) {
    return self->bssid;
}

mp_obj_t common_hal_wifi_network_get_rssi(wifi_network_obj_t *self) {
    return self->rssi;
}

mp_obj_t common_hal_wifi_network_get_channel(wifi_network_obj_t *self) {
    return self->channel;
}

mp_obj_t common_hal_wifi_network_get_country(wifi_network_obj_t *self) {
    return self->country;
}

mp_obj_t common_hal_wifi_network_get_authmode(wifi_network_obj_t *self) {
    return self->authmode;
}
