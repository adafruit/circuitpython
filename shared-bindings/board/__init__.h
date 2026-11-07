// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objstr.h"

#include "shared-bindings/microcontroller/Pin.h"  // for the pin definitions

// A port can inject board.EFUSE_KEY* (or other hardware key) entries into
// every board's globals table by defining CIRCUITPY_BOARD_HARDWARE_KEYS (a
// comma-terminated list of { MP_ROM_QSTR(...), MP_ROM_PTR(...) } pairs).
#if CIRCUITPY_HARDWAREKEY
#include "common-hal/hardwarekey/board.h"
#endif
#ifndef CIRCUITPY_BOARD_HARDWARE_KEYS
#define CIRCUITPY_BOARD_HARDWARE_KEYS
#endif

#if CIRCUITPY_MUTABLE_BOARD
extern mp_obj_dict_t board_module_globals;
#else
extern const mp_obj_dict_t board_module_globals;
#endif
static const MP_DEFINE_STR_OBJ(board_module_id_obj, CIRCUITPY_BOARD_ID);

mp_obj_t common_hal_board_get_i2c(const mp_int_t instance);
mp_obj_t common_hal_board_create_i2c(const mp_int_t instance);
mp_obj_t board_i2c(size_t n_args, const mp_obj_t *args);
MP_DECLARE_CONST_FUN_OBJ_0(board_i2c_obj);

mp_obj_t common_hal_board_get_spi(const mp_int_t instance);
mp_obj_t common_hal_board_create_spi(const mp_int_t instance);
mp_obj_t board_spi(size_t n_args, const mp_obj_t *args);
MP_DECLARE_CONST_FUN_OBJ_0(board_spi_obj);

mp_obj_t common_hal_board_get_uart(const mp_int_t instance);
mp_obj_t common_hal_board_create_uart(const mp_int_t instance);
mp_obj_t board_uart(size_t n_args, const mp_obj_t *args);
MP_DECLARE_CONST_FUN_OBJ_0(board_uart_obj);

#define CIRCUITPY_BOARD_BUS_SINGLETON(name, bus, instance) \
    static mp_obj_t board_##name(void) { \
        return common_hal_board_create_##bus(instance); \
    } \
    MP_DEFINE_CONST_FUN_OBJ_0(board_##name##_obj, board_##name);

#define CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS \
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_board) }, \
    { MP_ROM_QSTR(MP_QSTR_board_id), MP_ROM_PTR(&board_module_id_obj) }, \
    CIRCUITPY_BOARD_HARDWARE_KEYS

#define CIRCUITPYTHON_MUTABLE_BOARD_DICT_STANDARD_ITEMS \
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_board) }, \
    { MP_ROM_QSTR(MP_QSTR_board_id), MP_OBJ_FROM_PTR(&board_module_id_obj) }, \
    CIRCUITPY_BOARD_HARDWARE_KEYS
