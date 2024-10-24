// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Attribute.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"

#include "host/ble_att.h"

void common_hal_bleio_descriptor_construct(bleio_descriptor_obj_t *self, bleio_characteristic_obj_t *characteristic, bleio_uuid_obj_t *uuid, bleio_attribute_security_mode_t read_perm, bleio_attribute_security_mode_t write_perm, mp_int_t max_length, bool fixed_length, mp_buffer_info_t *initial_value_bufinfo) {
    self->characteristic = characteristic;
    self->uuid = uuid;
    self->handle = BLEIO_HANDLE_INVALID;
    self->read_perm = read_perm;
    self->write_perm = write_perm;
    self->initial_value = mp_obj_new_bytes(initial_value_bufinfo->buf, initial_value_bufinfo->len);

    // Map CP's property values to Nimble's flag values.
    self->flags = 0;
    if (read_perm != SECURITY_MODE_NO_ACCESS) {
        self->flags |= BLE_ATT_F_READ;
    }
    if (write_perm != SECURITY_MODE_NO_ACCESS) {
        self->flags |= BLE_ATT_F_WRITE;
    }
    if (read_perm == SECURITY_MODE_ENC_WITH_MITM || write_perm == SECURITY_MODE_ENC_WITH_MITM ||
        read_perm == SECURITY_MODE_SIGNED_WITH_MITM || write_perm == SECURITY_MODE_SIGNED_WITH_MITM) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("MITM security not supported"));
    }
    if (read_perm == SECURITY_MODE_ENC_NO_MITM) {
        self->flags |= BLE_ATT_F_READ_ENC;
    }
    if (read_perm == SECURITY_MODE_SIGNED_NO_MITM) {
        self->flags |= BLE_ATT_F_READ_AUTHEN;
    }
    if (write_perm == SECURITY_MODE_ENC_NO_MITM) {
        self->flags |= BLE_ATT_F_WRITE_ENC;
    }
    if (write_perm == SECURITY_MODE_SIGNED_NO_MITM) {
        self->flags |= BLE_ATT_F_WRITE_AUTHEN;
    }

    const mp_int_t max_length_max = BLE_ATT_ATTR_MAX_LEN;
    if (max_length < 0 || max_length > max_length_max) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("max_length must be 0-%d when fixed_length is %s"),
            max_length_max, fixed_length ? "True" : "False");
    }
    self->max_length = max_length;
    self->fixed_length = fixed_length;
}

bleio_uuid_obj_t *common_hal_bleio_descriptor_get_uuid(bleio_descriptor_obj_t *self) {
    return self->uuid;
}

bleio_characteristic_obj_t *common_hal_bleio_descriptor_get_characteristic(bleio_descriptor_obj_t *self) {
    return self->characteristic;
}

size_t common_hal_bleio_descriptor_get_value(bleio_descriptor_obj_t *self, uint8_t *buf, size_t len) {
    if (self->characteristic == NULL) {
        return 0;
    }
    if (common_hal_bleio_service_get_is_remote(self->characteristic->service)) {
        uint16_t conn_handle = bleio_connection_get_conn_handle(self->characteristic->service->connection);
        return bleio_gattc_read(conn_handle, self->handle, buf, len);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer(self->initial_value, &bufinfo, MP_BUFFER_READ);
        len = MIN(bufinfo.len, len);
        memcpy(buf, bufinfo.buf, len);
        return len;
    }

    return 0;
}

void common_hal_bleio_descriptor_set_value(bleio_descriptor_obj_t *self, mp_buffer_info_t *bufinfo) {
    if (self->characteristic == NULL) {
        return;
    }

    if (common_hal_bleio_service_get_is_remote(self->characteristic->service)) {
        uint16_t conn_handle = bleio_connection_get_conn_handle(self->characteristic->service->connection);
        bleio_gattc_write(conn_handle, self->handle, bufinfo->buf, bufinfo->len);
    } else {
        // Descriptor values should be set via the initial_value when used locally.
        mp_raise_NotImplementedError(NULL);
    }
}

int bleio_descriptor_access_cb(uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg) {
    bleio_descriptor_obj_t *self = arg;
    const ble_uuid_t *uuid;
    int rc;

    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_DSC:
            uuid = ctxt->dsc->uuid;
            if (ble_uuid_cmp(uuid, &self->uuid->nimble_ble_uuid.u) == 0) {
                mp_buffer_info_t bufinfo;
                if (!mp_get_buffer(self->initial_value, &bufinfo, MP_BUFFER_READ)) {
                    return BLE_ATT_ERR_UNLIKELY;
                }
                rc = os_mbuf_append(ctxt->om,
                    bufinfo.buf,
                    bufinfo.len);
                return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
            }
            return BLE_ATT_ERR_UNLIKELY;

        case BLE_GATT_ACCESS_OP_WRITE_DSC:
            return BLE_ATT_ERR_UNLIKELY;

        default:
            return BLE_ATT_ERR_UNLIKELY;
    }

    return BLE_ATT_ERR_UNLIKELY;
}
