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
 * @file picokdbg.c
 *
 * debug support knowledge base
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#include "picoos.h"
#include "picoknow.h"
#include "picodbg.h"
#include "picokdbg.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#if defined(PICO_DEBUG)

/**
 * @addtogroup picokdbg

 * <b> Pico Debug Support for knowledge base </b>\n
 *

 * @b Phones

 * overview of binary file format for dbg kb:

    dbg-kb = phonesyms

    phonesyms = {PHONESYM8}=256

    PHONESYM6: 8 bytes, symbol name (must be 0 terminated), the
               corresponding ID corresponds to the offset in the
               phonesyms array
*/

/* maximum length of phonesym string including terminating 0 */
#define KDBG_PHONESYMLEN_MAX 8


typedef struct kdbg_subobj *kdbg_SubObj;

typedef struct kdbg_subobj {
    picoos_uint8 *phonesyms;
} kdbg_subobj_t;


static pico_status_t kdbgInitialize(register picoknow_KnowledgeBase this,
                                    picoos_Common common) {
    kdbg_subobj_t *kdbg;

    PICODBG_DEBUG(("start"));

    if (NULL == this || NULL == this->subObj) {
        PICODBG_DEBUG(("2nd check failed"));
        return picoos_emRaiseException(common->em, PICO_ERR_OTHER, NULL, NULL);
    }
    kdbg = (kdbg_subobj_t *)this->subObj;
    kdbg->phonesyms = this->base;
    return PICO_OK;
}


static pico_status_t kdbgSubObjDeallocate(register picoknow_KnowledgeBase this,
                                          picoos_MemoryManager mm) {
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}


pico_status_t picokdbg_specializeDbgKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common) {
    if (NULL == this) {
        PICODBG_INFO(("no debug symbols loaded"));
        return PICO_OK;
    }
    this->subDeallocate = kdbgSubObjDeallocate;
    this->subObj = picoos_allocate(common->mm, sizeof(kdbg_subobj_t));
    if (NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                       NULL, NULL);
    }
    return kdbgInitialize(this, common);
}


picokdbg_Dbg picokdbg_getDbg(picoknow_KnowledgeBase this) {
    if (NULL == this) {
        return NULL;
    } else {
        return (picokdbg_Dbg)this->subObj;
    }
}


/* Dbg methods */

picoos_uint8 picokdbg_getPhoneId(const picokdbg_Dbg this,
                                 const picoos_char *phsym) {
    kdbg_subobj_t *kdbg;
    picoos_uint16 i;

    if (this == NULL)
        return 0;

    kdbg = (kdbg_subobj_t *)this;
    /* sequential search */
    for (i = 0; i < 256; i++) {
        if (!picoos_strcmp(phsym,
             (picoos_char *)&(kdbg->phonesyms[i * KDBG_PHONESYMLEN_MAX])))
            return (picoos_uint8)i;
    }
    return 0;
}


picoos_char *picokdbg_getPhoneSym(const picokdbg_Dbg this,
                                  const picoos_uint8 phid) {
    kdbg_subobj_t *kdbg;

    if (this == NULL)
        return NULL;

    kdbg = (kdbg_subobj_t *)this;
    return (picoos_char *)&(kdbg->phonesyms[phid * KDBG_PHONESYMLEN_MAX]);
}



#else

/* To prevent warning about "translation unit is empty" when
   diagnostic output is disabled. */
static void picokdbg_dummy(void) {
    picokdbg_dummy();
}


#endif /* defined(PICO_DEBUG) */

#ifdef __cplusplus
}
#endif


/* end */
