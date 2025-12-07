// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/ssl/SSLContext.h"
#include "shared-bindings/ssl/SSLSocket.h"

#include "py/runtime.h"
#include "py/stream.h"

void common_hal_ssl_sslcontext_construct(ssl_sslcontext_obj_t *self) {
    common_hal_ssl_sslcontext_set_default_verify_paths(self);
}

void common_hal_ssl_sslcontext_load_verify_locations(ssl_sslcontext_obj_t *self,
    const char *cadata) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_ssl_sslcontext_set_default_verify_paths(ssl_sslcontext_obj_t *self) {
    // The default paths are what's built in to NINA-FW, so nothing need be done.
}

bool common_hal_ssl_sslcontext_get_check_hostname(ssl_sslcontext_obj_t *self) {
    return true;
}

void common_hal_ssl_sslcontext_set_check_hostname(ssl_sslcontext_obj_t *self, bool value) {
    if (!value) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be %q"), MP_QSTR_check_hostname, MP_QSTR_true);
    }
}

void common_hal_ssl_sslcontext_load_cert_chain(ssl_sslcontext_obj_t *self, mp_buffer_info_t *cert_buf, mp_buffer_info_t *key_buf) {
    mp_raise_NotImplementedError(NULL);
}
