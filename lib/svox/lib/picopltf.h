/*
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file picopltf.h
 *
 * Header file (only) to define platform symbols.
 *
 * Refer to http://predef.sourceforge.net/ for a comprehensive list
 * of pre-defined C/C++ compiler macros.
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 */

#if !defined(__PICOPLTF_H__)
#define __PICOPLTF_H__

#define ENDIANNESS_BIG 1
#define ENDIANNESS_LITTLE 2

/* * platform identifiers ***/
#define PICO_Windows    1   /* Windows */
#define PICO_MacOSX     5   /* Macintosh OS X */
#define PICO_Linux      7   /* Linux */
#define PICO_ESP        9   /* ESP */
#define PICO_CIRCUITPY  11  /* CircuitPython core module (picotts) */

/* * definition of current platform ***/
#if !defined(PICO_PLATFORM)
#if defined(_WIN32)
#define PICO_PLATFORM    PICO_Windows
#elif defined(__APPLE__) && defined(__MACH__)
#define PICO_PLATFORM    PICO_MacOSX
#elif defined(linux) || defined(__linux__) || defined(__linux)
#define PICO_PLATFORM    PICO_Linux
#elif defined(CIRCUITPY)  /* before ESP_PLATFORM: CircuitPython espressif builds define both */
#define PICO_PLATFORM    PICO_CIRCUITPY
#elif defined(ESP_PLATFORM)
#define PICO_PLATFORM    PICO_ESP
#else
#error PICO_PLATFORM not defined
#endif
#endif /* !defined(PICO_PLATFORM) */


/* * symbol PICO_PLATFORM_STRING to define platform as string ***/
/* The pico-sdk (CircuitPython raspberrypi port) defines its own; keep it. */
#if !defined(PICO_PLATFORM_STRING)
#if (PICO_PLATFORM == PICO_Windows)
#define PICO_PLATFORM_STRING "Windows"
#elif (PICO_PLATFORM == PICO_MacOSX)
#define PICO_PLATFORM_STRING "MacOSX"
#elif (PICO_PLATFORM == PICO_Linux)
#define PICO_PLATFORM_STRING "Linux"
#elif (PICO_PLATFORM == PICO_ESP)
#define PICO_PLATFORM_STRING "ESP"
#elif (PICO_PLATFORM == PICO_CIRCUITPY)
#define PICO_PLATFORM_STRING "CircuitPython"
#elif (PICO_PLATFORM == PICO_GENERIC)
#define PICO_PLATFORM_STRING "UnknownPlatform"
#endif
#endif /* !defined(PICO_PLATFORM_STRING) */

#if (PICO_PLATFORM == PICO_MacOSX)
#define PICO_ENDIANNESS ENDIANNESS_BIG
#else
#define PICO_ENDIANNESS ENDIANNESS_LITTLE
#endif

#endif /* !defined(__PICOPLTF_H__) */
