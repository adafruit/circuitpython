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
 * @file picoapid.h
 *
 * Pico api definitions commonly used by picoapi and picoapiext
 *
 * This header file must be part of the runtime-only pico system and therefore
 * must not include picoapiext.h!
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#ifndef PICOAPID_H_
#define PICOAPID_H_

#include "picodefs.h"
#include "picoapi.h"
#include "picoos.h"
#include "picorsrc.h"
#include "picoctrl.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* Pico system descriptor */
typedef struct pico_system {
    picoos_uint32 magic;        /* magic number used to validate handles */
    picoos_Common common;
    picorsrc_ResourceManager rm;
    picoctrl_Engine engine;
} pico_system_t;


/* declared in picoapi.c */
extern int is_valid_system_handle(pico_System system);
extern picoos_Common pico_sysGetCommon(pico_System this);


#if 0
{
#endif
#ifdef __cplusplus
}
#endif

#endif /* PICOAPID_H_ */
