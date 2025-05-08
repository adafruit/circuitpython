// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// In ESP-IDF v5.4, Espressif changed the format of the in-flash cert bundle, which
// made lib/mbedtls_config/crt_bundle.c no longer work. Rather than update that,
// just wrap those functions and use the ESP-IDF versions.

#include "py/mperrno.h"
#include "mbedtls/x509_crt.h"
#include "lib/mbedtls_config/crt_bundle.h"
#include "esp_crt_bundle.h"

static int convert_esp_err(esp_err_t ret) {
    switch (ret) {
        case ESP_OK:
            return 0;
        default:
        // Right now esp_crt_bundle.c doesn't return very specific errors.
        case ESP_ERR_INVALID_ARG:
            return -MP_EINVAL;
    }
}

int crt_bundle_attach(mbedtls_ssl_config *ssl_conf) {
    return convert_esp_err(esp_crt_bundle_attach(ssl_conf));
}

void crt_bundle_detach(mbedtls_ssl_config *conf) {
    esp_crt_bundle_detach(conf);
}

int crt_bundle_set(const uint8_t *x509_bundle, size_t bundle_size) {
    return convert_esp_err(esp_crt_bundle_set(x509_bundle, bundle_size));
}
