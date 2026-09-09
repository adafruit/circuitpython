// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/_bleio/Connection.h"

#include <string.h>
#include <stdio.h>

#include "py/objarray.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"

//| class Connection:
//|     """A BLE connection to another device. Used to discover and interact with services on the other
//|     device.
//|
//|     Usage::
//|
//|        import _bleio
//|
//|        my_entry = None
//|        for entry in _bleio.adapter.scan(2.5):
//|            if entry.name is not None and entry.name == 'InterestingPeripheral':
//|                my_entry = entry
//|                break
//|
//|        if not my_entry:
//|            raise Exception("'InterestingPeripheral' not found")
//|
//|        connection = _bleio.adapter.connect(my_entry.address, timeout=10)"""
//|

void bleio_connection_ensure_connected(bleio_connection_obj_t *self) {
    if (!common_hal_bleio_connection_get_connected(self)) {
        mp_raise_ConnectionError(MP_ERROR_TEXT("Connection has been disconnected and can no longer be used. Create a new connection."));
    }
}

//|     def __init__(self) -> None:
//|         """Connections cannot be made directly. Instead, to initiate a connection use `Adapter.connect`.
//|         Connections may also be made when another device initiates a connection. To use a Connection
//|         created by a peer, read the `Adapter.connections` property."""
//|         ...
//|
//|     def disconnect(self) -> None:
//|         """Disconnects from the remote peripheral. Does nothing if already disconnected."""
//|         ...
//|
static mp_obj_t bleio_connection_disconnect(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // common_hal_bleio_connection_disconnect() does nothing if already disconnected.
    common_hal_bleio_connection_disconnect(self->connection);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_disconnect_obj, bleio_connection_disconnect);


//|     def pair(self, *, bond: bool = True) -> None:
//|         """Pair to the peer to improve security.
//|
//|         **Limitation**: Currently ``bond``must be ``True``: bonding always occurs.
//|         """
//|         ...
//|
static mp_obj_t bleio_connection_pair(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_bond };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bond, MP_ARG_BOOL, {.u_bool = true} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_bond].u_bool == false) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q=%q"), MP_QSTR_bond, MP_QSTR_False);
    }
    bleio_connection_ensure_connected(self);

    common_hal_bleio_connection_pair(self->connection, args[ARG_bond].u_bool);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(bleio_connection_pair_obj, 1, bleio_connection_pair);

//|     def discover_remote_services(
//|         self, service_uuids_whitelist: Optional[Iterable[UUID]] = None
//|     ) -> Tuple[Service, ...]:
//|         """Do BLE discovery for all services or for the given service UUIDS,
//|         to find their handles and characteristics, and return the discovered services.
//|         `Connection.connected` must be True.
//|
//|         :param iterable service_uuids_whitelist:
//|
//|           an iterable of :py:class:`UUID` objects for the services provided by the peripheral
//|           that you want to use.
//|
//|           The peripheral may provide more services, but services not listed are ignored
//|           and will not be returned.
//|
//|           If service_uuids_whitelist is None, then all services will undergo discovery, which can be
//|           slow.
//|
//|           If the service UUID is 128-bit, or its characteristic UUID's are 128-bit, you
//|           you must have already created a :py:class:`UUID` object for that UUID in order for the
//|           service or characteristic to be discovered. Creating the UUID causes the UUID to be
//|           registered for use. (This restriction may be lifted in the future.)
//|
//|         :return: A tuple of `_bleio.Service` objects provided by the remote peripheral."""
//|         ...
//|
static mp_obj_t bleio_connection_discover_remote_services(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_service_uuids_whitelist };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_service_uuids_whitelist, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bleio_connection_ensure_connected(self);

    return MP_OBJ_FROM_PTR(common_hal_bleio_connection_discover_remote_services(
        self,
        args[ARG_service_uuids_whitelist].u_obj));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(bleio_connection_discover_remote_services_obj, 1, bleio_connection_discover_remote_services);

//|     connected: bool
//|     """True if connected to the remote peer."""
static mp_obj_t bleio_connection_get_connected(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_connection_get_connected(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_connected_obj, bleio_connection_get_connected);

MP_PROPERTY_GETTER(bleio_connection_connected_obj,
    (mp_obj_t)&bleio_connection_get_connected_obj);


//|     paired: bool
//|     """True if paired to the remote peer."""
static mp_obj_t bleio_connection_get_paired(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_connection_get_paired(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_paired_obj, bleio_connection_get_paired);

MP_PROPERTY_GETTER(bleio_connection_paired_obj,
    (mp_obj_t)&bleio_connection_get_paired_obj);


//|     authenticated: bool
//|     """True if the link is encrypted *and* authenticated against
//|     man-in-the-middle attacks - i.e. LE Secure Connections numeric comparison (or
//|     passkey entry) completed. A plain encrypted-but-unauthenticated ("Just Works")
//|     link reads ``False``.
//|
//|     Ports that don't implement authenticated pairing always report ``False``,
//|     even for a bonded link."""
static mp_obj_t bleio_connection_get_authenticated(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_connection_get_authenticated(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_authenticated_obj, bleio_connection_get_authenticated);

MP_PROPERTY_GETTER(bleio_connection_authenticated_obj,
    (mp_obj_t)&bleio_connection_get_authenticated_obj);


//|     pairing_numeric_comparison: Optional[int]
//|     """When the peer has started LE Secure Connections pairing and is waiting for the
//|     user to confirm a 6-digit numeric-comparison value, this is that value (an ``int``,
//|     0-999999); otherwise ``None``. Poll it, show the value to the user, then answer
//|     with `confirm_pairing`. This is the peripheral side of pairing: the peer drives
//|     the procedure, so unlike `pair` this never blocks.
//|
//|     Numeric comparison happens automatically when a central pairs to reach a
//|     characteristic whose ``read_perm`` / ``write_perm`` is
//|     `Attribute.LESC_ENCRYPT_WITH_MITM`. A peripheral with a display and a button
//|     services it roughly like this::
//|
//|        while connection.connected:
//|            code = connection.pairing_numeric_comparison
//|            if code is not None:
//|                display.show(f"Pair? {code:06d}")
//|                connection.confirm_pairing(accept=button_a.value)
//|            if connection.authenticated:
//|                break
//|            time.sleep(0.1)
//|
//|     Only implemented on the espressif port."""
static mp_obj_t bleio_connection_get_pairing_numeric_comparison(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_bleio_connection_get_pairing_numeric_comparison(self);
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_pairing_numeric_comparison_obj, bleio_connection_get_pairing_numeric_comparison);

MP_PROPERTY_GETTER(bleio_connection_pairing_numeric_comparison_obj,
    (mp_obj_t)&bleio_connection_get_pairing_numeric_comparison_obj);


//|     def confirm_pairing(self, accept: bool) -> None:
//|         """Answer a pending numeric-comparison pairing request (see
//|         `pairing_numeric_comparison`). Pass ``accept=True`` if the value shown on the
//|         peer matches the one shown here, ``accept=False`` to reject and abort pairing.
//|
//|         Only implemented on the espressif port.
//|
//|         :raises ConnectionError: if the connection has dropped.
//|         :raises _bleio.BluetoothError: if no numeric-comparison request is pending -
//|             nothing was offered on `pairing_numeric_comparison`, or it already expired
//|             because the peer disconnected or the pairing procedure timed out."""
//|         ...
//|
static mp_obj_t bleio_connection_confirm_pairing(mp_obj_t self_in, mp_obj_t accept_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_connection_confirm_pairing(self, mp_obj_is_true(accept_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(bleio_connection_confirm_pairing_obj, bleio_connection_confirm_pairing);


//|     connection_interval: float
//|     """Time between transmissions in milliseconds. Will be multiple of 1.25ms. Lower numbers
//|     increase speed and decrease latency but increase power consumption.
//|
//|     When setting connection_interval, the peer may reject the new interval and
//|     `connection_interval` will then remain the same.
//|
//|     Apple has additional guidelines that dictate should be a multiple of 15ms except if HID is
//|     available. When HID is available Apple devices may accept 11.25ms intervals."""
static mp_obj_t bleio_connection_get_connection_interval(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bleio_connection_ensure_connected(self);
    return mp_obj_new_float(common_hal_bleio_connection_get_connection_interval(self->connection));
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_connection_interval_obj, bleio_connection_get_connection_interval);

//|     max_packet_length: int
//|     """The maximum number of data bytes that can be sent in a single transmission,
//|     not including overhead bytes.
//|
//|     This is the maximum number of bytes that can be sent in a notification,
//|     which must be sent in a single packet.
//|     But for a regular characteristic read or write, may be sent in multiple packets,
//|     so this limit does not apply."""
//|
//|
static mp_obj_t bleio_connection_get_max_packet_length(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bleio_connection_ensure_connected(self);
    return mp_obj_new_int(common_hal_bleio_connection_get_max_packet_length(self->connection));
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_max_packet_length_obj, bleio_connection_get_max_packet_length);


static mp_obj_t bleio_connection_set_connection_interval(mp_obj_t self_in, mp_obj_t interval_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_float_t interval = mp_obj_get_float(interval_in);

    bleio_connection_ensure_connected(self);
    common_hal_bleio_connection_set_connection_interval(self->connection, interval);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(bleio_connection_set_connection_interval_obj, bleio_connection_set_connection_interval);

MP_PROPERTY_GETSET(bleio_connection_connection_interval_obj,
    (mp_obj_t)&bleio_connection_get_connection_interval_obj,
    (mp_obj_t)&bleio_connection_set_connection_interval_obj);

MP_PROPERTY_GETTER(bleio_connection_max_packet_length_obj,
    (mp_obj_t)&bleio_connection_get_max_packet_length_obj);

static const mp_rom_map_elem_t bleio_connection_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_pair),                     MP_ROM_PTR(&bleio_connection_pair_obj) },
    { MP_ROM_QSTR(MP_QSTR_confirm_pairing),          MP_ROM_PTR(&bleio_connection_confirm_pairing_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),               MP_ROM_PTR(&bleio_connection_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_discover_remote_services), MP_ROM_PTR(&bleio_connection_discover_remote_services_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_connected),           MP_ROM_PTR(&bleio_connection_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_paired),              MP_ROM_PTR(&bleio_connection_paired_obj) },
    { MP_ROM_QSTR(MP_QSTR_authenticated),       MP_ROM_PTR(&bleio_connection_authenticated_obj) },
    { MP_ROM_QSTR(MP_QSTR_pairing_numeric_comparison), MP_ROM_PTR(&bleio_connection_pairing_numeric_comparison_obj) },
    { MP_ROM_QSTR(MP_QSTR_connection_interval), MP_ROM_PTR(&bleio_connection_connection_interval_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_packet_length),   MP_ROM_PTR(&bleio_connection_max_packet_length_obj) },
};

static MP_DEFINE_CONST_DICT(bleio_connection_locals_dict, bleio_connection_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    bleio_connection_type,
    MP_QSTR_Connection,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &bleio_connection_locals_dict
    );
