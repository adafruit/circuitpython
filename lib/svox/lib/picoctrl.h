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
 * @file picoctrl.h
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#ifndef PICOCTRL_H_
#define PICOCTRL_H_

#include "picodefs.h"
#include "picoos.h"
#include "picorsrc.h"
#include "picodata.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#define PICOCTRL_MAX_PROC_UNITS 25

/* temporarily increased for preprocessing
#define PICOCTRL_DEFAULT_ENGINE_SIZE 200000
*/
#define PICOCTRL_DEFAULT_ENGINE_SIZE 1000000

typedef struct picoctrl_engine * picoctrl_Engine;

picoos_int16 picoctrl_isValidEngineHandle(picoctrl_Engine this);

picoctrl_Engine picoctrl_newEngine (
        picoos_MemoryManager mm,
        picorsrc_ResourceManager rm,
        const picoos_char * voiceName
        );

void picoctrl_disposeEngine(
        picoos_MemoryManager mm,
        picorsrc_ResourceManager rm,
        picoctrl_Engine * this
        );

pico_status_t picoctrl_engFeedText(
        picoctrl_Engine engine,
        picoos_char * text,
        picoos_int16  textSize,
        picoos_int16 * bytesPut);

pico_status_t picoctrl_engReset(
        picoctrl_Engine engine,
        picoos_int32 resetMode);

picoos_Common picoctrl_engGetCommon(picoctrl_Engine this);

picodata_step_result_t picoctrl_engFetchOutputItemBytes(
        picoctrl_Engine engine,
        picoos_char * buffer,
        picoos_int16 bufferSize,
        picoos_int16  * bytesReceived
);

void picoctrl_engResetExceptionManager(
        picoctrl_Engine this
        );


picodata_step_result_t picoctrl_getLastScheduledPU(
        picoctrl_Engine engine
        );

picodata_step_result_t picoctrl_getLastProducedItemType(
        picoctrl_Engine engine
        );

#ifdef __cplusplus
}
#endif

#endif /*PICOCTRL_H_*/
