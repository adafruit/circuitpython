// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"

#include "shared-bindings/displayio/__init__.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/qspibus/QSPIBus.h"

extern const mp_obj_type_t qspibus_qspibus_type;

void common_hal_qspibus_qspibus_construct(
    qspibus_qspibus_obj_t *self,
    const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *data0,
    const mcu_pin_obj_t *data1,
    const mcu_pin_obj_t *data2,
    const mcu_pin_obj_t *data3,
    const mcu_pin_obj_t *cs,
    const mcu_pin_obj_t *dcx,
    const mcu_pin_obj_t *reset,
    uint32_t frequency);

void common_hal_qspibus_qspibus_deinit(qspibus_qspibus_obj_t *self);
bool common_hal_qspibus_qspibus_deinited(qspibus_qspibus_obj_t *self);

void common_hal_qspibus_qspibus_send_command(
    qspibus_qspibus_obj_t *self,
    uint8_t command,
    const uint8_t *data,
    size_t len);
void common_hal_qspibus_qspibus_write_command(
    qspibus_qspibus_obj_t *self,
    uint8_t command);
void common_hal_qspibus_qspibus_write_data(
    qspibus_qspibus_obj_t *self,
    const uint8_t *data,
    size_t len);

bool common_hal_qspibus_qspibus_reset(mp_obj_t obj);
bool common_hal_qspibus_qspibus_bus_free(mp_obj_t obj);
bool common_hal_qspibus_qspibus_begin_transaction(mp_obj_t obj);
void common_hal_qspibus_qspibus_send(
    mp_obj_t obj,
    display_byte_type_t data_type,
    display_chip_select_behavior_t chip_select,
    const uint8_t *data,
    uint32_t data_length);
void common_hal_qspibus_qspibus_end_transaction(mp_obj_t obj);
void common_hal_qspibus_qspibus_collect_ptrs(mp_obj_t obj);
