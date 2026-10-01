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
 * @file picokfst.c
 *
 * FST knowledge loading and access
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */
#include "picoos.h"
#include "picodbg.h"
#include "picoknow.h"
#include "picokfst.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


#define FileHdrSize 4       /* size of FST file header */



/* ************************************************************/
/* function to create specialized kb, */
/* to be used by picorsrc only */
/* ************************************************************/

/** object       : FSTKnowledgeBase
 *  shortcut     : kfst
 *  derived from : picoknow_KnowledgeBase
 */

typedef struct kfst_subobj * kfst_SubObj;

typedef struct kfst_subobj{
    picoos_uint8 * fstStream;         /* the byte stream base address */
    picoos_int32 hdrLen;              /* length of file header */
    picoos_int32 transductionMode;    /* transduction mode to be used for FST */
    picoos_int32 nrClasses;           /* nr of pair/transition classes in FST; class is in [1..nrClasses] */
    picoos_int32 nrStates;            /* nr of states in FST; state is in [1..nrState] */
    picoos_int32 termClass;           /* pair class of terminator symbol pair; probably obsolete */
    picoos_int32 alphaHashTabSize;    /* size of pair alphabet hash table */
    picoos_int32 alphaHashTabPos;     /* absolute address of the start of the pair alphabet */
    picoos_int32 transTabEntrySize;   /* size in bytes of each transition table entry */
    picoos_int32 transTabPos;         /* absolute address of the start of the transition table */
    picoos_int32 inEpsStateTabPos;    /* absolute address of the start of the input epsilon transition table */
    picoos_int32 accStateTabPos;      /* absolute address of the table of accepting states */
} kfst_subobj_t;



/* ************************************************************/
/* primitives for reading from byte stream */
/* ************************************************************/

/* Converts 'nrBytes' bytes starting at position '*pos' in byte stream 'stream' into unsigned number 'num'.
   '*pos' is modified to the position right after the number */
static void FixedBytesToUnsignedNum (picoos_uint8 * stream, picoos_uint8 nrBytes, picoos_uint32 * pos, picoos_uint32 * num)
{
    picoos_int32 i;

    (*num) = 0;
    for (i = 0; i < nrBytes; i++) {
        (*num) = ((*num) << 8) + (picoos_uint32)stream[*pos];
        (*pos)++;
    }
}


/* Converts 'nrBytes' bytes starting at position '*pos' in byte stream 'stream' into signed number 'num'.
   '*pos' is modified to the position right after the number */
static void FixedBytesToSignedNum (picoos_uint8 * stream, picoos_uint8 nrBytes, picoos_uint32 * pos, picoos_int32 * num)
{
    picoos_int32 i;
    picoos_uint32 val;

    val = 0;
    for (i = 0; i < nrBytes; i++) {
        val = (val << 8) + (picoos_uint32)stream[*pos];
        (*pos)++;
    }
    if (val % 2 == 1) {
        /* negative number */
        (*num) = -((picoos_int32)((val - 1) / 2)) - 1;
    } else {
        /* positive number */
        (*num) = val / 2;
    }
}


/* Converts varying-sized sequence of bytes starting at position '*pos' in byte stream 'stream'
   into (signed) number 'num'. '*pos' is modified to the position right after the number. */
static void BytesToNum (picoos_uint8 * stream, picoos_uint32 * pos, picoos_int32 * num)
{
    picoos_uint32 val;
    picoos_uint32 b;

    val = 0;
    b = (picoos_uint32)stream[*pos];
    (*pos)++;
    while (b < 128) {
        val = (val << 7) + b;
        b = (picoos_uint32)stream[*pos];
        (*pos)++;
    }
    val = (val << 7) + (b - 128);
    if (val % 2 == 1) {
        /* negative number */
        (*num) = -((picoos_int32)((val - 1) / 2)) - 1;
    } else {
        /* positive number */
        (*num) = val / 2;
    }
}


/* ************************************************************/
/* setting up FST from byte stream */
/* ************************************************************/

static pico_status_t kfstInitialize(register picoknow_KnowledgeBase this,
        picoos_Common common)
{
    picoos_uint32 curpos;
    picoos_int32 offs;
    kfst_subobj_t * kfst;

    PICODBG_DEBUG(("kfstInitialize -- start\n"));

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING, NULL,
                NULL);
    }
    kfst = (kfst_subobj_t *) this->subObj;

    /* +CT+ */
    kfst->fstStream = this->base;
    PICODBG_TRACE(("base: %d\n",this->base));
    kfst->hdrLen = FileHdrSize;
    curpos = kfst->hdrLen;
    BytesToNum(kfst->fstStream,& curpos,& kfst->transductionMode);
    BytesToNum(kfst->fstStream,& curpos,& kfst->nrClasses);
    BytesToNum(kfst->fstStream,& curpos,& kfst->nrStates);
    BytesToNum(kfst->fstStream,& curpos,& kfst->termClass);
    BytesToNum(kfst->fstStream,& curpos,& kfst->alphaHashTabSize);
    BytesToNum(kfst->fstStream,& curpos,& offs);
    kfst->alphaHashTabPos = kfst->hdrLen + offs;
    BytesToNum(kfst->fstStream,& curpos,& kfst->transTabEntrySize);
    BytesToNum(kfst->fstStream,& curpos,& offs);
    kfst->transTabPos = kfst->hdrLen + offs;
    BytesToNum(kfst->fstStream,& curpos,& offs);
    kfst->inEpsStateTabPos = kfst->hdrLen + offs;
    BytesToNum(kfst->fstStream,& curpos,& offs);
    kfst->accStateTabPos = kfst->hdrLen + offs;
    /* -CT- */

    return PICO_OK;
}


static pico_status_t kfstSubObjDeallocate(register picoknow_KnowledgeBase this,
        picoos_MemoryManager mm)
{
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}


/* calculates a small number of data (e.g. addresses) from kb for fast access.
 * This data is encapsulated in a picokfst_FST that can later be retrieved
 * with picokfst_getFST. */
pico_status_t picokfst_specializeFSTKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common)
{
    pico_status_t status;

    if (NULL == this) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING, NULL, NULL);
    }
    if (0 < this->size) {
        /* not a dummy kb */
        this->subDeallocate = kfstSubObjDeallocate;

        this->subObj = picoos_allocate(common->mm, sizeof(kfst_subobj_t));

        if (NULL == this->subObj) {
            return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM, NULL, NULL);
        }
        status = kfstInitialize(this, common);
        if (PICO_OK != status) {
            picoos_deallocate(common->mm,(void **)&this->subObj);
        }
    }
    return PICO_OK;
}


/* ************************************************************/
/* FST type and getFST function */
/* ************************************************************/



/* return kb FST for usage in PU */
picokfst_FST picokfst_getFST(picoknow_KnowledgeBase this)
{
    if (NULL == this) {
        return NULL;
    } else {
        return (picokfst_FST) this->subObj;
    }
}



/* ************************************************************/
/* FST access methods */
/* ************************************************************/


/* see description in header file */
extern picoos_uint8 picokfst_kfstGetTransductionMode(picokfst_FST this)
{
    kfst_SubObj fst = (kfst_SubObj) this;
    if (fst != NULL) {
        return fst->transductionMode;
    } else {
        return 0;
    }
}


/* see description in header file */
extern void picokfst_kfstGetFSTSizes (picokfst_FST this, picoos_int32 *nrStates, picoos_int32 *nrClasses)
{
    kfst_SubObj fst = (kfst_SubObj) this;
    if (fst != NULL) {
        *nrStates = fst->nrStates;
        *nrClasses = fst->nrClasses;
    } else {
        *nrStates = 0;
        *nrClasses = 0;
    }
}

/* see description in header file */
extern void picokfst_kfstStartPairSearch (picokfst_FST this, picokfst_symid_t inSym,
                                          picoos_bool * inSymFound, picoos_int32 * searchState)
{
    picoos_uint32 pos;
    picoos_int32 offs;
    picoos_int32 h;
    picoos_int32 inSymCellPos;
    picoos_int32 inSymX;
    picoos_int32 nextSameHashInSymOffs;

    kfst_SubObj fst = (kfst_SubObj) this;
    (*searchState) =  -1;
    (*inSymFound) = 0;
    h = inSym % fst->alphaHashTabSize;
    pos = fst->alphaHashTabPos + (h * 4);
    FixedBytesToSignedNum(fst->fstStream,4,& pos,& offs);
    if (offs > 0) {
        inSymCellPos = fst->alphaHashTabPos + offs;
        pos = inSymCellPos;
        BytesToNum(fst->fstStream,& pos,& inSymX);
        BytesToNum(fst->fstStream,& pos,& nextSameHashInSymOffs);
        while ((inSymX != inSym) && (nextSameHashInSymOffs > 0)) {
            inSymCellPos = inSymCellPos + nextSameHashInSymOffs;
            pos = inSymCellPos;
            BytesToNum(fst->fstStream,& pos,& inSymX);
            BytesToNum(fst->fstStream,& pos,& nextSameHashInSymOffs);
        }
        if (inSymX == inSym) {
            /* input symbol found; state is set to position after symbol cell */
            (*searchState) = pos;
            (*inSymFound) = 1;
        }
    }
}


/* see description in header file */
extern void picokfst_kfstGetNextPair (picokfst_FST this, picoos_int32 * searchState,
                                      picoos_bool * pairFound,
                                      picokfst_symid_t * outSym, picokfst_class_t * pairClass)
{
    picoos_uint32 pos;
    picoos_int32 val;

    kfst_SubObj fst = (kfst_SubObj) this;
    if ((*searchState) < 0) {
        (*pairFound) = 0;
        (*outSym) = PICOKFST_SYMID_ILLEG;
        (*pairClass) =  -1;
    } else {
        pos = (*searchState);
        BytesToNum(fst->fstStream,& pos,& val);
        *outSym = (picokfst_symid_t)val;
        if ((*outSym) != PICOKFST_SYMID_ILLEG) {
            BytesToNum(fst->fstStream,& pos,& val);
            *pairClass = (picokfst_class_t)val;
            (*pairFound) = 1;
            (*searchState) = pos;
        } else {
            (*pairFound) = 0;
            (*outSym) = PICOKFST_SYMID_ILLEG;
            (*pairClass) =  -1;
            (*searchState) =  -1;
        }
    }
}



/* see description in header file */
extern void picokfst_kfstGetTrans (picokfst_FST this, picokfst_state_t startState, picokfst_class_t transClass,
                                   picokfst_state_t * endState)
{

    picoos_uint32 pos;
    picoos_int32 index;
    picoos_uint32 endStateX;

    kfst_SubObj fst = (kfst_SubObj) this;
    if ((startState < 1) || (startState > fst->nrStates) || (transClass < 1) || (transClass > fst->nrClasses)) {
        (*endState) = 0;
    } else {
        index = (startState - 1) * fst->nrClasses + transClass - 1;
        pos = fst->transTabPos + (index * fst->transTabEntrySize);
        FixedBytesToUnsignedNum(fst->fstStream,fst->transTabEntrySize,& pos,& endStateX);
        (*endState) = endStateX;
    }
}


/* see description in header file */
extern void picokfst_kfstStartInEpsTransSearch (picokfst_FST this, picokfst_state_t startState,
                                                picoos_bool * inEpsTransFound, picoos_int32 * searchState)
{

    picoos_int32 offs;
    picoos_uint32 pos;

    kfst_SubObj fst = (kfst_SubObj) this;
    (*searchState) =  -1;
    (*inEpsTransFound) = 0;
    if ((startState > 0) && (startState <= fst->nrStates)) {
        pos = fst->inEpsStateTabPos + (startState - 1) * 4;
        FixedBytesToSignedNum(fst->fstStream,4,& pos,& offs);
        if (offs > 0) {
            (*searchState) = fst->inEpsStateTabPos + offs;
            (*inEpsTransFound) = 1;
        }
    }
}



/* see description in header file */
extern void picokfst_kfstGetNextInEpsTrans (picokfst_FST this, picoos_int32 * searchState,
                                            picoos_bool * inEpsTransFound,
                                            picokfst_symid_t * outSym, picokfst_state_t * endState)
{
    picoos_uint32 pos;
    picoos_int32 val;

    kfst_SubObj fst = (kfst_SubObj) this;
    if ((*searchState) < 0) {
        (*inEpsTransFound) = 0;
        (*outSym) = PICOKFST_SYMID_ILLEG;
        (*endState) = 0;
    } else {
        pos = (*searchState);
        BytesToNum(fst->fstStream,& pos,& val);
        *outSym = (picokfst_symid_t)val;
        if ((*outSym) != PICOKFST_SYMID_ILLEG) {
            BytesToNum(fst->fstStream,& pos,& val);
            *endState = (picokfst_state_t)val;
            (*inEpsTransFound) = 1;
            (*searchState) = pos;
        } else {
            (*inEpsTransFound) = 0;
            (*outSym) = PICOKFST_SYMID_ILLEG;
            (*endState) = 0;
            (*searchState) =  -1;
        }
    }
}


/* see description in header file */
extern picoos_bool picokfst_kfstIsAcceptingState (picokfst_FST this, picokfst_state_t state)
{

    picoos_uint32 pos;
    picoos_uint32 val;

    kfst_SubObj fst = (kfst_SubObj) this;
    if ((state > 0) && (state <= fst->nrStates)) {
        pos = fst->accStateTabPos + (state - 1);
        FixedBytesToUnsignedNum(fst->fstStream,1,& pos,& val);
        return (val == 1);
    } else {
        return 0;
    }
}

#ifdef __cplusplus
}
#endif

/* End picofst.c */
