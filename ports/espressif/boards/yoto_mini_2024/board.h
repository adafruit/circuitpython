#include "py/obj.h"
#include "shared-module/busdisplay/BusDisplay.h"

extern void board_set(qstr key, mp_obj_t value);
extern busdisplay_busdisplay_obj_t board_display;
