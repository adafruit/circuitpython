#include "shared-bindings/board/__init__.h"
#include "shared-bindings/tca9555/__init__.h"
#include "py/objtuple.h"

STATIC const qstr board_slot_fields[] = {
    MP_QSTR_ID,
    MP_QSTR_FAST1,
    MP_QSTR_FAST2,
    MP_QSTR_FAST3,
    MP_QSTR_FAST4,
    MP_QSTR_SLOW1,
    MP_QSTR_SLOW2,
    MP_QSTR_SLOW3,
    MP_QSTR_ADC1_ADDR,
    MP_QSTR_ADC2_TEMP_ADDR
};

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot1_obj,
    board_slot_fields,
    10,
    MP_ROM_INT(1),
    (mp_obj_t)&pin_GPIO0,
    (mp_obj_t)&pin_GPIO1,
    (mp_obj_t)&pin_GPIO2,
    (mp_obj_t)&pin_GPIO3,
    (mp_obj_t)&pin_TCA0_3,
    (mp_obj_t)&pin_TCA0_4,
    (mp_obj_t)&pin_TCA0_5,
    MP_ROM_INT(0),  // 0b0000
    MP_ROM_INT(3)   // 0b0011
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot2_obj,
    board_slot_fields,
    10,
    MP_ROM_INT(2),
    (mp_obj_t)&pin_GPIO4,
    (mp_obj_t)&pin_GPIO5,
    (mp_obj_t)&pin_GPIO6,
    (mp_obj_t)&pin_GPIO7,
    (mp_obj_t)&pin_TCA0_0,
    (mp_obj_t)&pin_TCA0_1,
    (mp_obj_t)&pin_TCA0_2,
    MP_ROM_INT(1),  // 0b0001
    MP_ROM_INT(6)   // 0b0110
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot3_obj,
    board_slot_fields,
    10,
    MP_ROM_INT(3),
    (mp_obj_t)&pin_GPIO8,
    (mp_obj_t)&pin_GPIO9,
    (mp_obj_t)&pin_GPIO10,
    (mp_obj_t)&pin_GPIO11,
    (mp_obj_t)&pin_TCA0_8,
    (mp_obj_t)&pin_TCA0_9,
    (mp_obj_t)&pin_TCA0_10,
    MP_ROM_INT(4),  // 0b0100
    MP_ROM_INT(2)   // 0b0010
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot4_obj,
    board_slot_fields,
    10,
    MP_ROM_INT(4),
    (mp_obj_t)&pin_GPIO12,
    (mp_obj_t)&pin_GPIO13,
    (mp_obj_t)&pin_GPIO14,
    (mp_obj_t)&pin_GPIO15,
    (mp_obj_t)&pin_TCA1_7,
    (mp_obj_t)&pin_TCA1_5,
    (mp_obj_t)&pin_TCA1_6,
    MP_ROM_INT(5),  // 0b0101
    MP_ROM_INT(7)   // 0b0111
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot5_obj,
    board_slot_fields,
    10,
    MP_ROM_INT(5),
    (mp_obj_t)&pin_GPIO16,
    (mp_obj_t)&pin_GPIO17,
    (mp_obj_t)&pin_GPIO18,
    (mp_obj_t)&pin_GPIO19,
    (mp_obj_t)&pin_TCA1_15,
    (mp_obj_t)&pin_TCA1_14,
    (mp_obj_t)&pin_TCA1_13,
    MP_ROM_INT(8),  // 0b1000
    MP_ROM_INT(11)  // 0b1011
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot6_obj,
    board_slot_fields,
    10,
    MP_ROM_INT(6),
    (mp_obj_t)&pin_GPIO20,
    (mp_obj_t)&pin_GPIO21,
    (mp_obj_t)&pin_GPIO22,
    (mp_obj_t)&pin_GPIO23,
    (mp_obj_t)&pin_TCA1_10,
    (mp_obj_t)&pin_TCA1_12,
    (mp_obj_t)&pin_TCA1_11,
    MP_ROM_INT(9),  // 0b1001
    MP_ROM_INT(10)  // 0b1010
    );

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_SLOT1), MP_ROM_PTR(&board_slot1_obj) },
    { MP_ROM_QSTR(MP_QSTR_SLOT2), MP_ROM_PTR(&board_slot2_obj) },
    { MP_ROM_QSTR(MP_QSTR_SLOT3), MP_ROM_PTR(&board_slot3_obj) },
    { MP_ROM_QSTR(MP_QSTR_SLOT4), MP_ROM_PTR(&board_slot4_obj) },
    { MP_ROM_QSTR(MP_QSTR_SLOT5), MP_ROM_PTR(&board_slot5_obj) },
    { MP_ROM_QSTR(MP_QSTR_SLOT6), MP_ROM_PTR(&board_slot6_obj) },
    { MP_ROM_QSTR(MP_QSTR_NUM_SLOTS), MP_ROM_INT(6) },

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO24) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO25) },

    { MP_ROM_QSTR(MP_QSTR_EX_I2C_SDA), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_EX_SPI_SCK), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_GP26_A0), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_GP26), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO26) },

    { MP_ROM_QSTR(MP_QSTR_EX_I2C_SCL), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_EX_SPI_MOSI), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_GP27_A1), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_GP27), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO27) },

    { MP_ROM_QSTR(MP_QSTR_INT), MP_ROM_PTR(&pin_GPIO28) },

    { MP_ROM_QSTR(MP_QSTR_SHARED_ADC), MP_ROM_PTR(&pin_GPIO29) },

    { MP_ROM_QSTR(MP_QSTR_MAIN_EN), MP_ROM_PTR(&pin_TCA0_6) },
    { MP_ROM_QSTR(MP_QSTR_USER_SW), MP_ROM_PTR(&pin_TCA0_7) },

    { MP_ROM_QSTR(MP_QSTR_ADC_ADDR_1), MP_ROM_PTR(&pin_TCA0_12) },
    { MP_ROM_QSTR(MP_QSTR_ADC_ADDR_2), MP_ROM_PTR(&pin_TCA0_13) },
    { MP_ROM_QSTR(MP_QSTR_ADC_ADDR_3), MP_ROM_PTR(&pin_TCA0_14) },
    { MP_ROM_QSTR(MP_QSTR_ADC_MUX_EN_1), MP_ROM_PTR(&pin_TCA0_15) },
    { MP_ROM_QSTR(MP_QSTR_ADC_MUX_EN_2), MP_ROM_PTR(&pin_TCA0_11) },

    { MP_ROM_QSTR(MP_QSTR_SW_A), MP_ROM_PTR(&pin_TCA1_1) },
    { MP_ROM_QSTR(MP_QSTR_SW_B), MP_ROM_PTR(&pin_TCA1_2) },
    { MP_ROM_QSTR(MP_QSTR_LED_A), MP_ROM_PTR(&pin_TCA1_3) },
    { MP_ROM_QSTR(MP_QSTR_LED_B), MP_ROM_PTR(&pin_TCA1_4) },

    { MP_ROM_QSTR(MP_QSTR_LCD_BL), MP_ROM_PTR(&pin_TCA1_0) },
    { MP_ROM_QSTR(MP_QSTR_LCD_DC), MP_ROM_PTR(&pin_TCA1_8) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CS), MP_ROM_PTR(&pin_TCA1_9) },

    { MP_ROM_QSTR(MP_QSTR_CURRENT_SENSE_ADDR), MP_ROM_INT(12) },        // 0b1100
    { MP_ROM_QSTR(MP_QSTR_TEMP_SENSE_ADDR), MP_ROM_INT(13) },           // 0b1101
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_OUT_SENSE_ADDR), MP_ROM_INT(14) },    // 0b1110
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_IN_SENSE_ADDR), MP_ROM_INT(15) },     // 0b1111

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_STEMMA_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
