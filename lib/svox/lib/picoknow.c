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
 * @file picoknow.c
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#include "picodefs.h"
#include "picoos.h"
#include "picodbg.h"
#include "picoknow.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/**  class   : KnowledgeBase
 *   shortcut : kb
 *
 */
extern picoknow_KnowledgeBase picoknow_newKnowledgeBase(picoos_MemoryManager mm)
{
    picoknow_KnowledgeBase this;
    PICODBG_TRACE(("start"));

    this = picoos_allocate(mm,sizeof(*this));
    if (NULL != this) {
      PICODBG_TRACE(("allocated KnowledgeBase at address %i with size %i",(picoos_uint32)this,sizeof(*this)));
        /* initialize */
        this->next = NULL;
        this->id = PICOKNOW_KBID_NULL;
        this->base = NULL;
        this->size = 0;
        this->subObj = NULL;
        this->subDeallocate = NULL;
    }
    return this;
}

extern void picoknow_disposeKnowledgeBase(picoos_MemoryManager mm, picoknow_KnowledgeBase * this)
{
    picoos_uint8 id;
    if (NULL != (*this)) {
        id = (*this)->id;
        PICODBG_TRACE(("disposing KnowledgeBase id=%i",id));
        /* terminate */
        if ((*this)->subObj != NULL) {
            (*this)->subDeallocate((*this),mm);
        }
        picoos_deallocate(mm,(void**)this);
    }
}

#ifdef __cplusplus
}
#endif

/* End picoknow.c */
