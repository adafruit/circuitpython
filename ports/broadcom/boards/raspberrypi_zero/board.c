// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"

#include "bindings/videocore/Framebuffer.h"
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#include "bindings/videocore/Hvs.h"

void board_init(void) {
  hvs_channel_t *display = &allocate_display()->hvs_display;
  display->channel = 1;
  display->base.type = &hvs_channel_type;
  return;
  /*
    videocore_framebuffer_obj_t *fb = &allocate_display_bus()->videocore;
    fb->base.type = &videocore_framebuffer_type;
    common_hal_videocore_framebuffer_construct(fb, 640, 480);

    framebufferio_framebufferdisplay_obj_t *display = &allocate_display()->framebuffer_display;
    display->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(
        display,
        MP_OBJ_FROM_PTR(fb),
        0,
        true);
  */
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
