// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/shared/safe_mode.h"

#include "common-hal/_bleio/Adapter.h"
#include "common-hal/_bleio/Service.h"

static int characteristic_on_ble_gap_evt(struct ble_gap_event *event, void *param);

void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self, bleio_service_obj_t *service,
    uint16_t handle, bleio_uuid_obj_t *uuid, bleio_characteristic_properties_t props,
    bleio_attribute_security_mode_t read_perm, bleio_attribute_security_mode_t write_perm,
    mp_int_t max_length, bool fixed_length, mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {
    self->service = service;
    self->uuid = uuid;
    self->handle = BLEIO_HANDLE_INVALID;
    self->cccd_handle = BLEIO_HANDLE_INVALID;
    self->sccd_handle = BLEIO_HANDLE_INVALID;
    self->props = props;
    self->read_perm = read_perm;
    self->write_perm = write_perm;
    self->observer = mp_const_none;

    // Map CP's property values to Nimble's flag values.
    self->flags = 0;
    if ((props & CHAR_PROP_BROADCAST) != 0) {
        self->flags |= BLE_GATT_CHR_F_BROADCAST;
    }
    if ((props & CHAR_PROP_INDICATE) != 0) {
        self->flags |= BLE_GATT_CHR_F_INDICATE;
    }
    if ((props & CHAR_PROP_NOTIFY) != 0) {
        self->flags |= BLE_GATT_CHR_F_NOTIFY;
    }
    if ((props & CHAR_PROP_READ) != 0) {
        self->flags |= BLE_GATT_CHR_F_READ;
    }
    if ((props & CHAR_PROP_WRITE) != 0) {
        self->flags |= BLE_GATT_CHR_F_WRITE;
    }
    if ((props & CHAR_PROP_WRITE_NO_RESPONSE) != 0) {
        self->flags |= BLE_GATT_CHR_F_WRITE_NO_RSP;
    }
    if (read_perm == SECURITY_MODE_ENC_WITH_MITM || write_perm == SECURITY_MODE_ENC_WITH_MITM ||
        read_perm == SECURITY_MODE_SIGNED_WITH_MITM || write_perm == SECURITY_MODE_SIGNED_WITH_MITM) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("MITM security not supported"));
    }
    if (read_perm == SECURITY_MODE_ENC_NO_MITM) {
        self->flags |= BLE_GATT_CHR_F_READ_ENC;
    }
    if (read_perm == SECURITY_MODE_SIGNED_NO_MITM) {
        self->flags |= BLE_GATT_CHR_F_READ_AUTHEN;
    }
    if (write_perm == SECURITY_MODE_ENC_NO_MITM) {
        self->flags |= BLE_GATT_CHR_F_WRITE_ENC;
    }
    if (write_perm == SECURITY_MODE_SIGNED_NO_MITM) {
        self->flags |= BLE_GATT_CHR_F_WRITE_AUTHEN;
    }

    // If max_length is 0, then no storage is allocated.
    if (max_length > 0) {
        if (gc_alloc_possible()) {
            self->current_value = m_malloc_without_collect(max_length);
        } else {
            self->current_value = port_malloc(max_length, false);
            if (self->current_value == NULL) {
                reset_into_safe_mode(SAFE_MODE_NO_HEAP);
            }
        }
    }
    self->current_value_alloc = max_length;
    self->current_value_len = 0;

    self->max_length = max_length;
    self->fixed_length = fixed_length;

    if (initial_value_bufinfo != NULL) {
        common_hal_bleio_characteristic_set_value(self, initial_value_bufinfo);
    }

    if (gc_alloc_possible()) {
        self->descriptor_list = mp_obj_new_list(0, NULL);
    } else {
        self->descriptor_list = NULL;
    }

    if (service->is_remote) {
        // If the service is remote, we're buffering incoming notifications and indications.
        self->handle = handle;
        ble_event_add_handler_entry(&self->event_handler_entry, characteristic_on_ble_gap_evt, self);
    } else {
        common_hal_bleio_service_add_characteristic(self->service, self, initial_value_bufinfo, user_description);
    }
}

bool common_hal_bleio_characteristic_deinited(bleio_characteristic_obj_t *self) {
    return self->handle == BLEIO_HANDLE_INVALID;
}

void common_hal_bleio_characteristic_deinit(bleio_characteristic_obj_t *self) {
    if (common_hal_bleio_characteristic_deinited(self)) {
        return;
    }
    if (self->current_value != NULL) {
        if (gc_nbytes(self->current_value) > 0) {
            m_free(self->current_value);
        } else {
            port_free(self->current_value);
        }

        self->current_value = NULL;
    }

    // Used to indicate deinit.
    self->handle = BLEIO_HANDLE_INVALID;
}

mp_obj_tuple_t *common_hal_bleio_characteristic_get_descriptors(bleio_characteristic_obj_t *self) {
    if (self->descriptor_list == NULL) {
        return mp_const_empty_tuple;
    }
    return mp_obj_new_tuple(self->descriptor_list->len, self->descriptor_list->items);
}

bleio_service_obj_t *common_hal_bleio_characteristic_get_service(bleio_characteristic_obj_t *self) {
    return self->service;
}



size_t common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self, uint8_t *buf, size_t len) {
    // Do GATT operations only if this characteristic has been added to a registered service.
    if (self->handle == BLEIO_HANDLE_INVALID) {
        return 0;
    }
    uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
    if (common_hal_bleio_service_get_is_remote(self->service)) {
        return bleio_gattc_read(conn_handle, self->handle, buf, len);
    } else {
        len = MIN(self->current_value_len, len);
        memcpy(buf, self->current_value, len);
        return len;
    }

    return 0;
}

size_t common_hal_bleio_characteristic_get_max_length(bleio_characteristic_obj_t *self) {
    return self->max_length;
}

void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    if (common_hal_bleio_service_get_is_remote(self->service)) {
        uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
        if ((self->props & CHAR_PROP_WRITE_NO_RESPONSE) != 0) {
            CHECK_NIMBLE_ERROR(ble_gattc_write_no_rsp_flat(conn_handle, self->handle, bufinfo->buf, bufinfo->len));
        } else {
            bleio_gattc_write(conn_handle, self->handle, bufinfo->buf, bufinfo->len);
        }
    } else {
        // Validate data length for local characteristics only.
        if (self->fixed_length && bufinfo->len != self->max_length) {
            mp_raise_ValueError(MP_ERROR_TEXT("Value length != required fixed length"));
        }
        if (bufinfo->len > self->max_length) {
            mp_raise_ValueError(MP_ERROR_TEXT("Value length > max_length"));
        }

        if (bufinfo == NULL) {
            self->current_value_len = 0;
            ble_gatts_chr_updated(self->handle);
            return;
        }

        self->current_value_len = bufinfo->len;
        memcpy(self->current_value, bufinfo->buf, self->current_value_len);

        ble_gatts_chr_updated(self->handle);
    }
}

// Used when we're the client.
static int characteristic_on_ble_gap_evt(struct ble_gap_event *event, void *param) {
    bleio_characteristic_obj_t *self = (bleio_characteristic_obj_t *)param;
    switch (event->type) {
        case BLE_GAP_EVENT_NOTIFY_RX: {
            // A remote service wrote to this characteristic.

            // Must be a notification, and event handle must match the handle for my characteristic.
            if (event->notify_rx.indication == 0 &&
                event->notify_rx.attr_handle == self->handle) {
                if (self->observer == mp_const_none) {
                    return 0;
                }
                const struct os_mbuf *m = event->notify_rx.om;
                uint16_t packet_len = OS_MBUF_PKTLEN(m);
                uint8_t temp_full_packet[packet_len];
                int rc = ble_hs_mbuf_to_flat(m, temp_full_packet, packet_len, NULL);
                if (rc != 0) {
                    return rc;
                }
                if (mp_obj_is_type(self->observer, &bleio_characteristic_buffer_type)) {
                    bleio_characteristic_buffer_extend(MP_OBJ_FROM_PTR(self->observer), temp_full_packet, packet_len);
                } else if (mp_obj_is_type(self->observer, &bleio_packet_buffer_type)) {
                    bleio_packet_buffer_extend(MP_OBJ_FROM_PTR(self->observer), event->notify_rx.conn_handle, temp_full_packet, packet_len);
                }
            }
            break;
        }
        case BLE_GAP_EVENT_SUBSCRIBE:
            break;
        default:
            return 0;
            break;
    }
    return 0;
}

// Used when we're the server.
int bleio_characteristic_access_cb(uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg) {
    bleio_characteristic_obj_t *self = (bleio_characteristic_obj_t *)arg;
    int rc;

    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            if (attr_handle == self->handle) {
                rc = os_mbuf_append(ctxt->om,
                    self->current_value,
                    self->current_value_len);
                return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
            }
            return BLE_ATT_ERR_UNLIKELY;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            if (attr_handle == self->handle) {
                uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);
                if (om_len > self->max_length || (self->fixed_length && om_len != self->max_length)) {
                    return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
                }
                self->current_value_len = om_len;

                rc = ble_hs_mbuf_to_flat(ctxt->om, self->current_value, om_len, NULL);
                if (rc != 0) {
                    return rc;
                }
                if (self->observer != mp_const_none) {
                    if (mp_obj_is_type(self->observer, &bleio_characteristic_buffer_type)) {
                        bleio_characteristic_buffer_extend(MP_OBJ_FROM_PTR(self->observer), self->current_value, self->current_value_len);
                    } else if (mp_obj_is_type(self->observer, &bleio_packet_buffer_type)) {
                        bleio_packet_buffer_extend(MP_OBJ_FROM_PTR(self->observer), conn_handle, self->current_value, self->current_value_len);
                    }
                }
                background_callback_add_core(&bleio_background_callback);
                return rc;
            }
            return BLE_ATT_ERR_UNLIKELY;

        default:
            return BLE_ATT_ERR_UNLIKELY;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

bleio_uuid_obj_t *common_hal_bleio_characteristic_get_uuid(bleio_characteristic_obj_t *self) {
    return self->uuid;
}

bleio_characteristic_properties_t common_hal_bleio_characteristic_get_properties(bleio_characteristic_obj_t *self) {
    return self->props;
}

void common_hal_bleio_characteristic_add_descriptor(bleio_characteristic_obj_t *self,
    bleio_descriptor_obj_t *descriptor) {
    size_t i = self->descriptor_list->len;
    if (i >= MAX_DESCRIPTORS) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Too many descriptors"));
    }

    mp_obj_list_append(MP_OBJ_FROM_PTR(self->descriptor_list),
        MP_OBJ_FROM_PTR(descriptor));

    descriptor->dsc_def = &self->dsc_defs[i];
    struct ble_gatt_dsc_def *dsc_def = descriptor->dsc_def;
    dsc_def->uuid = &descriptor->uuid->nimble_ble_uuid.u;
    dsc_def->att_flags = descriptor->flags;
    dsc_def->min_key_size = 16;
    dsc_def->access_cb = bleio_descriptor_access_cb;
    dsc_def->arg = descriptor;

    bleio_service_readd(self->service);
}

void common_hal_bleio_characteristic_set_cccd(bleio_characteristic_obj_t *self, bool notify, bool indicate) {
    if (self->cccd_handle == BLEIO_HANDLE_INVALID) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("No CCCD for this Characteristic"));
    }

    if (!common_hal_bleio_service_get_is_remote(self->service)) {
        mp_raise_bleio_RoleError(MP_ERROR_TEXT("Can't set CCCD on local Characteristic"));
    }

    const uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
    common_hal_bleio_check_connected(conn_handle);

    uint16_t cccd_value =
        (notify ? 1 << 0 : 0) |
        (indicate ? 1 << 1: 0);

    bleio_gattc_write(conn_handle, self->cccd_handle, (uint8_t *)&cccd_value, 2);
}

void bleio_characteristic_set_observer(bleio_characteristic_obj_t *self, mp_obj_t observer) {
    self->observer = observer;
}

void bleio_characteristic_clear_observer(bleio_characteristic_obj_t *self) {
    self->observer = mp_const_none;
}
