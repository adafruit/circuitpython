#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // External pins are in silkscreen order, from top to bottom, left side, then right side
    {MP_ROM_QSTR(MP_QSTR_VP), MP_ROM_PTR(&pin_GPIO36)},

    {MP_ROM_QSTR(MP_QSTR_VN), MP_ROM_PTR(&pin_GPIO39)},

    {MP_ROM_QSTR(MP_QSTR_G34), MP_ROM_PTR(&pin_GPIO34)},
    {MP_ROM_QSTR(MP_QSTR_A34), MP_ROM_PTR(&pin_GPIO34)},

    {MP_ROM_QSTR(MP_QSTR_G35), MP_ROM_PTR(&pin_GPIO35)},
    {MP_ROM_QSTR(MP_QSTR_A35), MP_ROM_PTR(&pin_GPIO35)},

    {MP_ROM_QSTR(MP_QSTR_G32), MP_ROM_PTR(&pin_GPIO32)},
    {MP_ROM_QSTR(MP_QSTR_A32), MP_ROM_PTR(&pin_GPIO32)},

    {MP_ROM_QSTR(MP_QSTR_G33), MP_ROM_PTR(&pin_GPIO33)},
    {MP_ROM_QSTR(MP_QSTR_A33), MP_ROM_PTR(&pin_GPIO33)},

    {MP_ROM_QSTR(MP_QSTR_G25), MP_ROM_PTR(&pin_GPIO25)},
    {MP_ROM_QSTR(MP_QSTR_DAC1), MP_ROM_PTR(&pin_GPIO25)},

    {MP_ROM_QSTR(MP_QSTR_G26), MP_ROM_PTR(&pin_GPIO26)},
    {MP_ROM_QSTR(MP_QSTR_DAC2), MP_ROM_PTR(&pin_GPIO26)},

    {MP_ROM_QSTR(MP_QSTR_G27), MP_ROM_PTR(&pin_GPIO27)},

    {MP_ROM_QSTR(MP_QSTR_G14), MP_ROM_PTR(&pin_GPIO14)},

    {MP_ROM_QSTR(MP_QSTR_G12), MP_ROM_PTR(&pin_GPIO12)},


    {MP_ROM_QSTR(MP_QSTR_G22), MP_ROM_PTR(&pin_GPIO22)},
    {MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO22)},

    {MP_ROM_QSTR(MP_QSTR_G19), MP_ROM_PTR(&pin_GPIO19)},

    {MP_ROM_QSTR(MP_QSTR_G23), MP_ROM_PTR(&pin_GPIO23)},

    {MP_ROM_QSTR(MP_QSTR_G18), MP_ROM_PTR(&pin_GPIO18)},

    {MP_ROM_QSTR(MP_QSTR_G5), MP_ROM_PTR(&pin_GPIO5)},

    {MP_ROM_QSTR(MP_QSTR_G17), MP_ROM_PTR(&pin_GPIO17)},

    {MP_ROM_QSTR(MP_QSTR_G16), MP_ROM_PTR(&pin_GPIO16)},

    {MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_GPIO4)},

    {MP_ROM_QSTR(MP_QSTR_G0), MP_ROM_PTR(&pin_GPIO0)},

    {MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&pin_GPIO2)},

    {MP_ROM_QSTR(MP_QSTR_G15), MP_ROM_PTR(&pin_GPIO15)},

    {MP_ROM_QSTR(MP_QSTR_G13), MP_ROM_PTR(&pin_GPIO13)}
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
