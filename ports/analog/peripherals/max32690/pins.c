// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "peripherals/pins.h"
#include "max32690.h"

const mcu_pin_obj_t pin_P0_00 = PIN(0, 0);
const mcu_pin_obj_t pin_P0_01 = PIN(0, 1);
const mcu_pin_obj_t pin_P0_02 = PIN(0, 2);
const mcu_pin_obj_t pin_P0_03 = PIN(0, 3);
const mcu_pin_obj_t pin_P0_04 = PIN(0, 4);
const mcu_pin_obj_t pin_P0_05 = PIN(0, 5);
const mcu_pin_obj_t pin_P0_06 = PIN(0, 6);
const mcu_pin_obj_t pin_P0_07 = PIN(0, 7);
const mcu_pin_obj_t pin_P0_08 = PIN(0, 8);
const mcu_pin_obj_t pin_P0_09 = PIN(0, 9);
const mcu_pin_obj_t pin_P0_10 = PIN(0, 10);
const mcu_pin_obj_t pin_P0_11 = PIN(0, 11);
const mcu_pin_obj_t pin_P0_12 = PIN(0, 12);
const mcu_pin_obj_t pin_P0_13 = PIN(0, 13);
const mcu_pin_obj_t pin_P0_14 = PIN(0, 14);
const mcu_pin_obj_t pin_P0_15 = PIN(0, 15);
const mcu_pin_obj_t pin_P0_16 = PIN(0, 16);
const mcu_pin_obj_t pin_P0_17 = PIN(0, 17);
const mcu_pin_obj_t pin_P0_18 = PIN(0, 18);
const mcu_pin_obj_t pin_P0_19 = PIN(0, 19);
const mcu_pin_obj_t pin_P0_20 = PIN(0, 20);
const mcu_pin_obj_t pin_P0_21 = PIN(0, 21);
const mcu_pin_obj_t pin_P0_22 = PIN(0, 22);
const mcu_pin_obj_t pin_P0_23 = PIN(0, 23);
const mcu_pin_obj_t pin_P0_24 = PIN(0, 24);
const mcu_pin_obj_t pin_P0_25 = PIN(0, 25);
const mcu_pin_obj_t pin_P0_26 = PIN(0, 26);
const mcu_pin_obj_t pin_P0_27 = PIN(0, 27);
const mcu_pin_obj_t pin_P0_28 = PIN(0, 28);
const mcu_pin_obj_t pin_P0_29 = PIN(0, 29);
const mcu_pin_obj_t pin_P0_30 = PIN(0, 30);
const mcu_pin_obj_t pin_P0_31 = PIN(0, 31);

const mcu_pin_obj_t pin_P1_00 = PIN(1, 0);
const mcu_pin_obj_t pin_P1_01 = PIN(1, 1);
const mcu_pin_obj_t pin_P1_02 = PIN(1, 2);
const mcu_pin_obj_t pin_P1_03 = PIN(1, 3);
const mcu_pin_obj_t pin_P1_04 = PIN(1, 4);
const mcu_pin_obj_t pin_P1_05 = PIN(1, 5);
const mcu_pin_obj_t pin_P1_06 = PIN(1, 6);
const mcu_pin_obj_t pin_P1_07 = PIN(1, 7);
const mcu_pin_obj_t pin_P1_08 = PIN(1, 8);
const mcu_pin_obj_t pin_P1_09 = PIN(1, 9);
const mcu_pin_obj_t pin_P1_10 = PIN(1, 10);
const mcu_pin_obj_t pin_P1_11 = PIN(1, 11);
const mcu_pin_obj_t pin_P1_12 = PIN(1, 12);
const mcu_pin_obj_t pin_P1_13 = PIN(1, 13);
const mcu_pin_obj_t pin_P1_14 = PIN(1, 14);
const mcu_pin_obj_t pin_P1_15 = PIN(1, 15);
const mcu_pin_obj_t pin_P1_16 = PIN(1, 16);
const mcu_pin_obj_t pin_P1_17 = PIN(1, 17);
const mcu_pin_obj_t pin_P1_18 = PIN(1, 18);
const mcu_pin_obj_t pin_P1_19 = PIN(1, 19);
const mcu_pin_obj_t pin_P1_20 = PIN(1, 20);
const mcu_pin_obj_t pin_P1_21 = PIN(1, 21);
const mcu_pin_obj_t pin_P1_22 = PIN(1, 22);
const mcu_pin_obj_t pin_P1_23 = PIN(1, 23);
const mcu_pin_obj_t pin_P1_24 = PIN(1, 24);
const mcu_pin_obj_t pin_P1_25 = PIN(1, 25);
const mcu_pin_obj_t pin_P1_26 = PIN(1, 26);
const mcu_pin_obj_t pin_P1_27 = PIN(1, 27);
const mcu_pin_obj_t pin_P1_28 = PIN(1, 28);
const mcu_pin_obj_t pin_P1_29 = PIN(1, 29);
const mcu_pin_obj_t pin_P1_30 = PIN(1, 30);
const mcu_pin_obj_t pin_P1_31 = PIN(1, 31);

const mcu_pin_obj_t pin_P2_00 = PIN(2, 0);
const mcu_pin_obj_t pin_P2_01 = PIN(2, 1);
const mcu_pin_obj_t pin_P2_02 = PIN(2, 2);
const mcu_pin_obj_t pin_P2_03 = PIN(2, 3);
const mcu_pin_obj_t pin_P2_04 = PIN(2, 4);
const mcu_pin_obj_t pin_P2_05 = PIN(2, 5);
const mcu_pin_obj_t pin_P2_06 = PIN(2, 6);
const mcu_pin_obj_t pin_P2_07 = PIN(2, 7);
const mcu_pin_obj_t pin_P2_08 = PIN(2, 8);
const mcu_pin_obj_t pin_P2_09 = PIN(2, 9);
const mcu_pin_obj_t pin_P2_10 = PIN(2, 10);
const mcu_pin_obj_t pin_P2_11 = PIN(2, 11);
const mcu_pin_obj_t pin_P2_12 = PIN(2, 12);
const mcu_pin_obj_t pin_P2_13 = PIN(2, 13);
const mcu_pin_obj_t pin_P2_14 = PIN(2, 14);
const mcu_pin_obj_t pin_P2_15 = PIN(2, 15);
const mcu_pin_obj_t pin_P2_16 = PIN(2, 16);
const mcu_pin_obj_t pin_P2_17 = PIN(2, 17);
const mcu_pin_obj_t pin_P2_18 = PIN(2, 18);
const mcu_pin_obj_t pin_P2_19 = PIN(2, 19);
const mcu_pin_obj_t pin_P2_20 = PIN(2, 20);
const mcu_pin_obj_t pin_P2_21 = PIN(2, 21);
const mcu_pin_obj_t pin_P2_22 = PIN(2, 22);
const mcu_pin_obj_t pin_P2_23 = PIN(2, 23);
const mcu_pin_obj_t pin_P2_24 = PIN(2, 24);
const mcu_pin_obj_t pin_P2_25 = PIN(2, 25);
const mcu_pin_obj_t pin_P2_26 = PIN(2, 26);
const mcu_pin_obj_t pin_P2_27 = PIN(2, 27);
const mcu_pin_obj_t pin_P2_28 = PIN(2, 28);
const mcu_pin_obj_t pin_P2_29 = PIN(2, 29);
const mcu_pin_obj_t pin_P2_30 = PIN(2, 30);
const mcu_pin_obj_t pin_P2_31 = PIN(2, 31);

const mcu_pin_obj_t pin_P3_00 = PIN(3, 0);
const mcu_pin_obj_t pin_P3_01 = PIN(3, 1);
const mcu_pin_obj_t pin_P3_02 = PIN(3, 2);
const mcu_pin_obj_t pin_P3_03 = PIN(3, 3);
const mcu_pin_obj_t pin_P3_04 = PIN(3, 4);
const mcu_pin_obj_t pin_P3_05 = PIN(3, 5);
const mcu_pin_obj_t pin_P3_06 = PIN(3, 6);
const mcu_pin_obj_t pin_P3_07 = PIN(3, 7);
const mcu_pin_obj_t pin_P3_08 = PIN(3, 8);
const mcu_pin_obj_t pin_P3_09 = PIN(3, 9);

const mcu_pin_obj_t pin_P4_00 = PIN(4, 0);
const mcu_pin_obj_t pin_P4_01 = PIN(4, 1);
