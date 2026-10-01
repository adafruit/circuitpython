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
 * @file picokdbg.h
 *
 * debug support knowledge base
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *-  0.1, 08.05.2008, MRi - initial version
 *
 */

#ifndef PICOKDBG_H_
#define PICOKDBG_H_


#include "picoos.h"
#include "picoknow.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ************************************************************/
/* Dbg type and functions */
/* ************************************************************/

/**
 * to be used by picorsrc only
 */
pico_status_t picokdbg_specializeDbgKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common);

typedef struct picokdbg_dbg *picokdbg_Dbg;

/**
 * return kb Phones for usage in PU
 */
picokdbg_Dbg picokdbg_getDbg(picoknow_KnowledgeBase this);


/* phone ID - phone symbol conversion functions */

/**
 * return phone ID for phone symbol 'phsym' which must be 0 terminated
 */
picoos_uint8 picokdbg_getPhoneId(const picokdbg_Dbg dbg,
                                 const picoos_char *phsym);

/**
 * return pointer to phone symbol for phone ID phid
 */
picoos_char *picokdbg_getPhoneSym(const picokdbg_Dbg dbg,
                                  const picoos_uint8 phid);

#ifdef __cplusplus
}
#endif


#endif /*PICOKDBG_H_*/
