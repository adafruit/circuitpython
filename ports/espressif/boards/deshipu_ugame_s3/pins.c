#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"


static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_P1), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_P2), MP_ROM_PTR(&pin_GPIO44) },
    { MP_ROM_QSTR(MP_QSTR_P3), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_P4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_P5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_P6), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_P7), MP_ROM_PTR(&pin_GPIO7) },

    { MP_ROM_QSTR(MP_QSTR_BUTTON_LEFT), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_UP), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_RIGHT), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_DOWN), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_X), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_O), MP_ROM_PTR(&pin_GPIO48) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_Z), MP_ROM_PTR(&pin_GPIO47) },

    { MP_ROM_QSTR(MP_QSTR_LIGHT), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_BATTERY), MP_ROM_PTR(&pin_GPIO8) },


    { MP_ROM_QSTR(MP_QSTR_AUDIO_BCLK), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_LRCLK), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_DATA), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_GAIN), MP_ROM_PTR(&pin_GPIO18) },

    { MP_ROM_QSTR(MP_QSTR_TFT_RESET), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_TFT_CS), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_TFT_DC), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_TFT_SCK), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_TFT_MOSI), MP_ROM_PTR(&pin_GPIO11) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)},
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
