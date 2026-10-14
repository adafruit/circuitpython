// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "peripherals/pins.h"

// GPIO26-28 and GPIO30-32 are the SPI flash pads and are not exposed.
// GPIO29 and GPIO41 do not exist.
const mcu_pin_obj_t pin_GPIO0 = PIN(0, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO1 = PIN(1, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO2 = PIN(2, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO3 = PIN(3, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO4 = PIN(4, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO5 = PIN(5, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO6 = PIN(6, NO_ADC, NO_ADC_CHANNEL, 0);
const mcu_pin_obj_t pin_GPIO7 = PIN(7, NO_ADC, NO_ADC_CHANNEL, 1);
const mcu_pin_obj_t pin_GPIO8 = PIN(8, NO_ADC, NO_ADC_CHANNEL, 2);
const mcu_pin_obj_t pin_GPIO9 = PIN(9, NO_ADC, NO_ADC_CHANNEL, 3);
const mcu_pin_obj_t pin_GPIO10 = PIN(10, NO_ADC, NO_ADC_CHANNEL, 4);
const mcu_pin_obj_t pin_GPIO11 = PIN(11, NO_ADC, NO_ADC_CHANNEL, 5);
const mcu_pin_obj_t pin_GPIO12 = PIN(12, NO_ADC, NO_ADC_CHANNEL, 6);
const mcu_pin_obj_t pin_GPIO13 = PIN(13, NO_ADC, NO_ADC_CHANNEL, 7);
const mcu_pin_obj_t pin_GPIO14 = PIN(14, NO_ADC, NO_ADC_CHANNEL, 8);
const mcu_pin_obj_t pin_GPIO15 = PIN(15, NO_ADC, NO_ADC_CHANNEL, 9);
const mcu_pin_obj_t pin_GPIO16 = PIN(16, NO_ADC, NO_ADC_CHANNEL, 10);
const mcu_pin_obj_t pin_GPIO17 = PIN(17, NO_ADC, NO_ADC_CHANNEL, 11);
const mcu_pin_obj_t pin_GPIO18 = PIN(18, NO_ADC, NO_ADC_CHANNEL, 12);
const mcu_pin_obj_t pin_GPIO19 = PIN(19, NO_ADC, NO_ADC_CHANNEL, 13);
const mcu_pin_obj_t pin_GPIO20 = PIN(20, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO21 = PIN(21, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO22 = PIN(22, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO23 = PIN(23, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO24 = PIN(24, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO25 = PIN(25, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO33 = PIN(33, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO34 = PIN(34, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO35 = PIN(35, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO36 = PIN(36, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO37 = PIN(37, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO38 = PIN(38, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO39 = PIN(39, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO40 = PIN(40, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO42 = PIN(42, ADC_UNIT_1, ADC_CHANNEL_0, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO43 = PIN(43, ADC_UNIT_1, ADC_CHANNEL_1, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO44 = PIN(44, ADC_UNIT_1, ADC_CHANNEL_2, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO45 = PIN(45, ADC_UNIT_1, ADC_CHANNEL_3, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO46 = PIN(46, ADC_UNIT_1, ADC_CHANNEL_4, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO47 = PIN(47, ADC_UNIT_1, ADC_CHANNEL_5, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO48 = PIN(48, ADC_UNIT_1, ADC_CHANNEL_6, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO49 = PIN(49, ADC_UNIT_1, ADC_CHANNEL_7, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO50 = PIN(50, ADC_UNIT_2, ADC_CHANNEL_0, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO51 = PIN(51, ADC_UNIT_2, ADC_CHANNEL_1, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO52 = PIN(52, ADC_UNIT_2, ADC_CHANNEL_2, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO53 = PIN(53, ADC_UNIT_2, ADC_CHANNEL_3, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO54 = PIN(54, ADC_UNIT_2, ADC_CHANNEL_4, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO55 = PIN(55, ADC_UNIT_2, ADC_CHANNEL_5, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO56 = PIN(56, ADC_UNIT_2, ADC_CHANNEL_6, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO57 = PIN(57, ADC_UNIT_2, ADC_CHANNEL_7, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO58 = PIN(58, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO59 = PIN(59, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO60 = PIN(60, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO61 = PIN(61, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);

// The USB high-speed PHY pads cannot be used as GPIOs.
const mcu_pin_obj_t pin_USB_HS_DP = PIN(NO_PIN, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_USB_HS_DM = PIN(NO_PIN, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
