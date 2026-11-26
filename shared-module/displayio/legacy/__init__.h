// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/lifetime.h"

#if CIRCUITPY_BUSDISPLAY
#include "shared-bindings/busdisplay/BusDisplay.h"
#endif
#if CIRCUITPY_AURORA_EPAPER
#include "shared-module/aurora_epaper/aurora_framebuffer.h"
#endif
#include "shared-bindings/displayio/Group.h"
#if CIRCUITPY_EPAPERDISPLAY
#include "shared-bindings/epaperdisplay/EPaperDisplay.h"
#endif
#if CIRCUITPY_FRAMEBUFFERIO
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#endif
#if CIRCUITPY_FOURWIRE
#include "shared-bindings/fourwire/FourWire.h"
#endif
#if CIRCUITPY_I2CDISPLAYBUS
#include "shared-bindings/i2cdisplaybus/I2CDisplayBus.h"
#endif
#if CIRCUITPY_PARALLELDISPLAYBUS
#include "shared-bindings/paralleldisplaybus/ParallelBus.h"
#endif
#if CIRCUITPY_RGBMATRIX
#include "shared-bindings/rgbmatrix/RGBMatrix.h"
#endif
#if CIRCUITPY_IS31FL3741
#include "shared-bindings/is31fl3741/FrameBuffer.h"
#endif
#if CIRCUITPY_SHARPDISPLAY
#include "shared-module/sharpdisplay/SharpMemoryFramebuffer.h"
#endif
#if CIRCUITPY_DOTCLOCKFRAMEBUFFER
#include "common-hal/dotclockframebuffer/DotClockFramebuffer.h"
#endif
#if CIRCUITPY_MIPIDSI
#include "shared-bindings/mipidsi/Display.h"
#endif
#if CIRCUITPY_QSPIBUS
#include "shared-bindings/qspibus/QSPIBus.h"
#endif
#if CIRCUITPY_ZEPHYR_DISPLAY
#include "bindings/zephyr_display/Display.h"
#endif
#if CIRCUITPY_PICODVI
#include "bindings/picodvi/Framebuffer.h"
#endif

extern displayio_group_t circuitpython_splash;
