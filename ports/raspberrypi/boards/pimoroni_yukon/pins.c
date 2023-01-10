#include "shared-bindings/board/__init__.h"
#include "py/objtuple.h"

STATIC const qstr board_slot_fields[] = {
    MP_QSTR_FAST1,
    MP_QSTR_FAST2,
    MP_QSTR_FAST3,
    MP_QSTR_FAST4
};

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot1_obj,
    board_slot_fields,
    4,
    (mp_obj_t)&pin_GPIO0,
    (mp_obj_t)&pin_GPIO1,
    (mp_obj_t)&pin_GPIO2,
    (mp_obj_t)&pin_GPIO3
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot2_obj,
    board_slot_fields,
    4,
    (mp_obj_t)&pin_GPIO4,
    (mp_obj_t)&pin_GPIO5,
    (mp_obj_t)&pin_GPIO6,
    (mp_obj_t)&pin_GPIO7
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot3_obj,
    board_slot_fields,
    4,
    (mp_obj_t)&pin_GPIO8,
    (mp_obj_t)&pin_GPIO9,
    (mp_obj_t)&pin_GPIO10,
    (mp_obj_t)&pin_GPIO11
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot4_obj,
    board_slot_fields,
    4,
    (mp_obj_t)&pin_GPIO12,
    (mp_obj_t)&pin_GPIO13,
    (mp_obj_t)&pin_GPIO14,
    (mp_obj_t)&pin_GPIO15
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot5_obj,
    board_slot_fields,
    4,
    (mp_obj_t)&pin_GPIO16,
    (mp_obj_t)&pin_GPIO17,
    (mp_obj_t)&pin_GPIO18,
    (mp_obj_t)&pin_GPIO19
    );

STATIC MP_DEFINE_ATTRTUPLE(
    board_slot6_obj,
    board_slot_fields,
    4,
    (mp_obj_t)&pin_GPIO20,
    (mp_obj_t)&pin_GPIO21,
    (mp_obj_t)&pin_GPIO22,
    (mp_obj_t)&pin_GPIO23
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

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_STEMMA_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
