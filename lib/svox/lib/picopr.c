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
 * @file picopr.c
 *
 * text preprocessor
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
#include "picobase.h"
#include "picodbg.h"
#include "picodata.h"
#include "picokpr.h"
#include "picopr.h"
#include "picoktab.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* *****************************************************************************/
/* constants */
/* *****************************************************************************/

#define PR_TRACE_MEM      FALSE
#define PR_TRACE_MAX_MEM  FALSE
#define PR_TRACE_PATHCOST TRUE

#define PR_WORK_MEM_SIZE  10000
#define PR_DYN_MEM_SIZE   7000

#define PR_ENABLED TRUE

#define PR_MAX_NR_ITERATIONS 1000;

#define SPEC_CHAR           "\\/"

#define PICO_ERR_CONTEXT_NOT_FOUND            PICO_ERR_OTHER
#define PICO_ERR_MAX_PREPROC_PATH_LEN_REACHED PICO_ERR_OTHER

#define IN_BUF_SIZE   255
#define OUT_BUF_SIZE  IN_BUF_SIZE + 3 * PICODATA_ITEM_HEADSIZE + 3

#define PR_MAX_NR_PREPROC   (1 + PICOKNOW_MAX_NUM_UTPP)

#define PR_MAX_PATH_LEN     130
#define PR_MAX_DATA_LEN     IN_BUF_SIZE
#define PR_MAX_DATA_LEN_Z   PR_MAX_DATA_LEN + 1      /* all strings in picopr should use this constant
                                                        to ensure zero termination */
#define PR_COST_INIT        100000
#define PR_COST             10
#define PR_EOL              '\n'

/* Bit mask constants for token sets with parameters */
#define PR_TSE_MASK_OUT      (1<<PR_TSEOut)
#define PR_TSE_MASK_MIN      (1<<PR_TSEMin)
#define PR_TSE_MASK_MAX      (1<<PR_TSEMax)
#define PR_TSE_MASK_LEN      (1<<PR_TSELen)
#define PR_TSE_MASK_VAL      (1<<PR_TSEVal)
#define PR_TSE_MASK_STR      (1<<PR_TSEStr)
#define PR_TSE_MASK_HEAD     (1<<PR_TSEHead)
#define PR_TSE_MASK_MID      (1<<PR_TSEMid)
#define PR_TSE_MASK_TAIL     (1<<PR_TSETail)
#define PR_TSE_MASK_PROD     (1<<PR_TSEProd)
#define PR_TSE_MASK_PRODEXT  (1<<PR_TSEProdExt)
#define PR_TSE_MASK_VAR      (1<<PR_TSEVar)
#define PR_TSE_MASK_LEX      (1<<PR_TSELex)
#define PR_TSE_MASK_COST     (1<<PR_TSECost)
#define PR_TSE_MASK_ID       (1<<PR_TSEID)
#define PR_TSE_MASK_DUMMY1   (1<<PR_TSEDummy1)
#define PR_TSE_MASK_DUMMY2   (1<<PR_TSEDummy2)
#define PR_TSE_MASK_DUMMY3   (1<<PR_TSEDummy3)

/* Bit mask constants for token sets without parameters */
#define PR_TSE_MASK_BEGIN      (1<<PR_TSEBegin)
#define PR_TSE_MASK_END        (1<<PR_TSEEnd)
#define PR_TSE_MASK_SPACE      (1<<PR_TSESpace)
#define PR_TSE_MASK_DIGIT      (1<<PR_TSEDigit)
#define PR_TSE_MASK_LETTER     (1<<PR_TSELetter)
#define PR_TSE_MASK_CHAR       (1<<PR_TSEChar)
#define PR_TSE_MASK_SEQ        (1<<PR_TSESeq)
#define PR_TSE_MASK_CMPR       (1<<PR_TSECmpr)
#define PR_TSE_MASK_NLZ        (1<<PR_TSENLZ)
#define PR_TSE_MASK_ROMAN      (1<<PR_TSERoman)
#define PR_TSE_MASK_CI         (1<<PR_TSECI)
#define PR_TSE_MASK_CIS        (1<<PR_TSECIS)
#define PR_TSE_MASK_AUC        (1<<PR_TSEAUC)
#define PR_TSE_MASK_ALC        (1<<PR_TSEALC)
#define PR_TSE_MASK_SUC        (1<<PR_TSESUC)
#define PR_TSE_MASK_ACCEPT     (1<<PR_TSEAccept)
#define PR_TSE_MASK_NEXT       (1<<PR_TSENext)
#define PR_TSE_MASK_ALTL       (1<<PR_TSEAltL)
#define PR_TSE_MASK_ALTR       (1<<PR_TSEAltR)

#define PR_FIRST_TSE_WP PR_TSEOut

#define PR_SMALLER 1
#define PR_EQUAL   0
#define PR_LARGER  2

#define PR_SPELL_WITH_SENTENCE_BREAK  -2
#define PR_SPELL_WITH_PHRASE_BREAK    -1
#define PR_SPELL                       0

#define PICO_SPEED_MIN           20
#define PICO_SPEED_MAX          500
#define PICO_SPEED_DEFAULT      100
#define PICO_SPEED_FACTOR_MIN   500
#define PICO_SPEED_FACTOR_MAX  2000

#define PICO_PITCH_MIN           50
#define PICO_PITCH_MAX          200
#define PICO_PITCH_DEFAULT      100
#define PICO_PITCH_FACTOR_MIN   500
#define PICO_PITCH_FACTOR_MAX  2000
#define PICO_PITCH_ADD_MIN     -100
#define PICO_PITCH_ADD_MAX      100
#define PICO_PITCH_ADD_DEFAULT    0

#define PICO_VOLUME_MIN           0
#define PICO_VOLUME_MAX         500
#define PICO_VOLUME_DEFAULT     100
#define PICO_VOLUME_FACTOR_MIN  500
#define PICO_VOLUME_FACTOR_MAX 2000

#define PICO_CONTEXT_DEFAULT   "DEFAULT"

#define PICO_PARAGRAPH_PAUSE_DUR 500


/* *****************************************************************************/
/* types */
/* *****************************************************************************/

typedef enum {PR_OStr, PR_OVar, PR_OItem, PR_OSpell, PR_ORomanToCard, PR_OVal,
              PR_OLeft, PR_ORight, PR_ORLZ, PR_OIgnore, PR_OPitch, PR_OSpeed,
              PR_OVolume, PR_OVoice, PR_OContext, PR_OPhonSVOXPA, PR_OPhonSAMPA,
              PR_OPlay, PR_OUseSig, PR_OGenFile, PR_OAudioEdit, PR_OPara,
              PR_OSent, PR_OBreak, PR_OMark, PR_OConcat, PR_OLast}  pr_OutType;

typedef enum {PR_TSEBegin, PR_TSEEnd, PR_TSESpace, PR_TSEDigit, PR_TSELetter, PR_TSEChar, PR_TSESeq,
              PR_TSECmpr, PR_TSENLZ, PR_TSERoman, PR_TSECI, PR_TSECIS, PR_TSEAUC, PR_TSEALC, PR_TSESUC,
              PR_TSEAccept, PR_TSENext, PR_TSEAltL, PR_TSEAltR}  pr_TokSetEleNP;

typedef enum {PR_TSEOut, PR_TSEMin, PR_TSEMax, PR_TSELen, PR_TSEVal, PR_TSEStr, PR_TSEHead, PR_TSEMid,
              PR_TSETail, PR_TSEProd, PR_TSEProdExt, PR_TSEVar, PR_TSELex, PR_TSECost, PR_TSEID,
              PR_TSEDummy1, PR_TSEDummy2, PR_TSEDummy3}  pr_TokSetEleWP;

typedef enum {PR_GSNoPreproc, PR_GS_START, PR_GSContinue, PR_GSNeedToken, PR_GSNotFound, PR_GSFound}  pr_GlobalState;

typedef enum {PR_LSError, PR_LSInit, PR_LSGetToken, PR_LSGetToken2, PR_LSMatch, PR_LSGoBack,
              PR_LSGetProdToken, PR_LSInProd, PR_LSGetProdContToken, PR_LSInProdCont, PR_LSGetNextToken,
              PR_LSGetAltToken}  pr_LocalState;

typedef enum {PR_MSNotMatched, PR_MSMatched, PR_MSMatchedContinue, PR_MSMatchedMulti}  pr_MatchState;

typedef struct pr_Prod * pr_ProdList;
typedef struct pr_Prod {
    picokpr_Preproc rNetwork;
    picokpr_ProdArrOffset rProdOfs;
    pr_ProdList rNext;
} pr_Prod;

typedef struct pr_Context * pr_ContextList;
typedef struct pr_Context {
    picoos_uchar * rContextName;
    pr_ProdList rProdList;
    pr_ContextList rNext;
} pr_Context;

/* *****************************************************************************/
/* used, but to be checked */

#define MaxNrShortStrParams 2
#define MaxPhoneLen 14
#define ShortStrParamLen (2 * MaxPhoneLen)
typedef picoos_uchar ShortStrParam[ShortStrParamLen];


typedef struct pr_ioItem * pr_ioItemPtr;
typedef struct pr_ioItem {
    pr_ioItemPtr next;
    picoos_int32 val;
    struct picodata_itemhead head;
    picoos_uchar * strci;
    picoos_uchar * strcis;
    picoos_bool alc;
    picoos_bool auc;
    picoos_bool suc;
    picoos_uchar data[PR_MAX_DATA_LEN_Z];
} pr_ioItem;

typedef struct pr_ioItem2 {
    pr_ioItemPtr next;
    picoos_int32 val;
    struct picodata_itemhead head;
    picoos_uchar * strci;
    picoos_uchar * strcis;
    picoos_bool alc;
    picoos_bool auc;
    picoos_bool suc;
} pr_ioItem2;

#define PR_IOITEM_MIN_SIZE sizeof(pr_ioItem2)

typedef picoos_uint32 pr_MemState;
typedef enum {pr_DynMem, pr_WorkMem} pr_MemTypes;

/* *****************************************************************************/

typedef struct pr_OutItemVar * pr_OutItemVarPtr;
struct pr_OutItemVar {
    pr_ioItemPtr first;
    pr_ioItemPtr last;
    picoos_int32 id;
    pr_OutItemVarPtr next;
};


struct pr_WorkItem {
    pr_ioItemPtr rit;
};
typedef pr_ioItemPtr pr_WorkItems[PR_MAX_PATH_LEN+1];

struct pr_PathEle {
    picokpr_Preproc rnetwork;
    picoos_int16 ritemid;
    picoos_int16 rcompare;
    picoos_int16 rdepth;
    picokpr_TokArrOffset rtok;
    picokpr_StrArrOffset rprodname;
    picoos_int32 rprodprefcost;
    pr_LocalState rlState;
};

typedef struct pr_Path {
    picoos_int32 rcost;
    picoos_int32 rlen;
    struct pr_PathEle rele[PR_MAX_PATH_LEN];
} pr_Path;

/* *****************************************************************************/

/** subobject : PreprocUnit
 *  shortcut  : pr
 */
typedef struct pr_subobj
{
    pr_ioItemPtr rinItemList;
    pr_ioItemPtr rlastInItem;
    pr_ioItemPtr routItemList;
    pr_ioItemPtr rlastOutItem;
    pr_GlobalState rgState;
    pr_Path ractpath;
    pr_Path rbestpath;
    picoos_int32 rnritems;
    pr_WorkItems ritems;
    picoos_int32 rignore;
    picoos_int32 spellMode;
    picoos_int32 maxPathLen;
    picoos_bool insidePhoneme;

    picoos_uint8 inBuf[IN_BUF_SIZE+PICODATA_ITEM_HEADSIZE]; /* internal input buffer */
    picoos_uint16 inBufLen;

    picoos_uint8 outBuf[OUT_BUF_SIZE]; /* internal output buffer */
    picoos_uint16 outReadPos; /* next pos to read from outBuf */
    picoos_uint16 outWritePos; /* next pos to write to outBuf */

    picokpr_Preproc preproc[PR_MAX_NR_PREPROC];
    pr_ContextList ctxList;
    pr_ProdList prodList;

    pr_ContextList actCtx;
    picoos_bool actCtxChanged;

    picoos_uchar tmpStr1[PR_MAX_DATA_LEN_Z];
    picoos_uchar tmpStr2[PR_MAX_DATA_LEN_Z];

    picoos_uint8 pr_WorkMem[PR_WORK_MEM_SIZE];
    picoos_uint32 workMemTop;
    picoos_uint32 maxWorkMemTop;
    picoos_uint8 pr_DynMem[PR_DYN_MEM_SIZE];
    picoos_MemoryManager dynMemMM;
    picoos_int32 dynMemSize;
    picoos_int32 maxDynMemSize;

    picoos_bool outOfMemory;

    picoos_bool forceOutput;
    picoos_int16 nrIterations;

    picoos_uchar lspaces[128];
    picoos_uchar saveFile[IN_BUF_SIZE];

    pr_ioItem tmpItem;

    picotrns_SimpleTransducer transducer;

    /* kbs */

    picoktab_Graphs graphs;
    picokfst_FST xsampa_parser;
    picokfst_FST svoxpa_parser;
    picokfst_FST xsampa2svoxpa_mapper;

} pr_subobj_t;

/* *****************************************************************************/
/* prototypes */

static void pr_getOutputItemList (picodata_ProcessingUnit this, pr_subobj_t * pr,
                                  picokpr_Preproc network,
                                  picokpr_OutItemArrOffset outitem,
                                  pr_OutItemVarPtr vars,
                                  pr_ioItemPtr * first, pr_ioItemPtr * last);

/* *****************************************************************************/

#define pr_iABS(X) (((X) < 0) ? (-(X)) : (X))

/* *****************************************************************************/
/* module internal memory managment for dynamic and working memory using memory
   partitions allocated with pr_subobj_t.
   Dynamic memory is allocated in pr_subobj_t->pr_DynMem. Dynamic memory has
   to be deallocated again with pr_DEALLOCATE.
   Working memory is allocated in pr_subobj_t->pr_WorkMem. Working memory is stack
   based and may not to be deallocated with pr_DEALLOCATE, but with pr_resetMemState
   to a state previously saved with pr_getMemState.
*/

static void pr_ALLOCATE (picodata_ProcessingUnit this, pr_MemTypes mType, void * * adr, unsigned int byteSize)
  /* allocates 'byteSize' bytes in the memery partition given by 'mType' */
{
    pr_subobj_t * pr = (pr_subobj_t *) this->subObj;
    picoos_int32 incrUsedBytes, prevmaxDynMemSize;

    if (mType == pr_WorkMem) {
        if ((pr->workMemTop + byteSize) < PR_WORK_MEM_SIZE) {
            (*adr) = (void *)(&(pr->pr_WorkMem[pr->workMemTop]));
            byteSize = ((byteSize + PICOOS_ALIGN_SIZE - 1) / PICOOS_ALIGN_SIZE) * PICOOS_ALIGN_SIZE;
            pr->workMemTop += byteSize;
#if PR_TRACE_MEM
            PICODBG_INFO(("pr_WorkMem: +%u, tot:%i of %i", byteSize, pr->workMemTop, PR_WORK_MEM_SIZE));
#endif

            if (pr->workMemTop > pr->maxWorkMemTop) {
                pr->maxWorkMemTop = pr->workMemTop;
#if PR_TRACE_MAX_MEM
                PICODBG_INFO(("new max pr_WorkMem: %i of %i", pr->workMemTop, PR_WORK_MEM_SIZE));
#endif
            }
        }
        else {
            (*adr) = NULL;
            PICODBG_ERROR(("pr out of working memory"));
            picoos_emRaiseException(this->common->em, PICO_EXC_OUT_OF_MEM, (picoos_char *)"pr out of dynamic memory", (picoos_char *)"");
            pr->outOfMemory = TRUE;
        }
    }
    else if (mType == pr_DynMem) {
        (*adr) = picoos_allocate(pr->dynMemMM, byteSize);
        if ((*adr) != NULL) {
            prevmaxDynMemSize = pr->maxDynMemSize;
            picoos_getMemUsage(pr->dynMemMM, 1, &pr->dynMemSize, &incrUsedBytes, &pr->maxDynMemSize);
#if PR_TRACE_MEM
            PICODBG_INFO(("pr_DynMem : +%i, tot:%i of %i", incrUsedBytes, pr->dynMemSize, PR_DYN_MEM_SIZE));
#endif

#if PR_TRACE_MAX_MEM
            if (pr->maxDynMemSize > prevmaxDynMemSize) {
                PICODBG_INFO(("new max pr_DynMem : %i of %i", pr->maxDynMemSize, PR_DYN_MEM_SIZE));
            }
#endif
        }
        else {
            PICODBG_ERROR(("pr out of dynamic memory"));
            picoos_emRaiseException(this->common->em, PICO_EXC_OUT_OF_MEM, (picoos_char *)"pr out of dynamic memory", (picoos_char *)"");
            pr->outOfMemory = TRUE;
        }
    }
    else {
        (*adr) = NULL;
    }
}


static void pr_DEALLOCATE (picodata_ProcessingUnit this, pr_MemTypes mType, void * * adr)
{
    pr_subobj_t * pr = (pr_subobj_t *) this->subObj;
    picoos_int32 incrUsedBytes;
    if (mType == pr_WorkMem) {
        PICODBG_INFO(("not possible; use pr_resetMemState instead"));
    }
    else if (mType == pr_DynMem) {
        picoos_deallocate(pr->dynMemMM, &(*adr));
        picoos_getMemUsage(pr->dynMemMM, 1, &pr->dynMemSize, &incrUsedBytes, &pr->maxDynMemSize);
#if PR_TRACE_MEM
        PICODBG_INFO(("pr_DynMem : %i, tot:%i of %i: adr: %u", incrUsedBytes, pr->dynMemSize, PR_DYN_MEM_SIZE, *adr));
#endif
    }
    else {
        (*adr) = NULL;
    }
}


static void pr_getMemState(picodata_ProcessingUnit this, pr_MemTypes mType, picoos_uint32 *lmemState)
{
    pr_subobj_t * pr = (pr_subobj_t *) this->subObj;
    mType = mType;        /* avoid warning "var not used in this function"*/
    *lmemState = pr->workMemTop;
}


static void pr_resetMemState(picodata_ProcessingUnit this, pr_MemTypes mType, picoos_uint32 lmemState)
{
    pr_subobj_t * pr = (pr_subobj_t *) this->subObj;

#if PR_TRACE_MEM
    PICODBG_INFO(("pr_WorkMem: -%i, tot:%i of %i", pr->workMemTop-lmemState, lmemState, PR_WORK_MEM_SIZE));
#endif
    mType = mType;        /* avoid warning "var not used in this function"*/
    pr->workMemTop = lmemState;
}


/* *****************************************************************************/
/* string operations */

static picoos_int32 pr_strlen(const picoos_uchar * str)
{
    picoos_int32 i;

    i=0;
    while ((i<PR_MAX_DATA_LEN) && (str[i] != 0)) {
        i++;
    }
    return i;
}


static picoos_uint32 pr_strcpy(picoos_uint8 * dest, const picoos_uint8 * src)
{
    picoos_int32 i;

    i = 0;
    while ((i<PR_MAX_DATA_LEN) && (src[i] != 0)) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
    return i;
}


static picoos_uint32 pr_strcat(picoos_uint8 * dest, const picoos_uint8 * src)
{
    picoos_int32 i, j;

    i = 0;
    while ((i<PR_MAX_DATA_LEN) && (dest[i] != 0)) {
        i++;
    }
    j = 0;
    while ((i<PR_MAX_DATA_LEN) && (j<PR_MAX_DATA_LEN) && (src[j] != 0)) {
        dest[i] = src[j];
        i++;
        j++;
    }
    dest[i] = 0;
    return i;
}


static void pr_getTermPartStr (picoos_uchar string[], picoos_int32 * ind, picoos_uchar termCh, picoos_uchar str[], picoos_bool * done)
{
    int j;
    picoos_bool done1;

    done1 = TRUE;
    j = 0;
    while ((*ind < PR_MAX_DATA_LEN) && (string[*ind] != termCh) && (string[*ind] != 0)) {
        if (j < PR_MAX_DATA_LEN) {
            str[j] = string[*ind];
            j++;
        } else {
            done1 = FALSE;
        }
        (*ind)++;
    }
    if (j < PR_MAX_DATA_LEN) {
        str[j] = 0;
    }
    *done = ((*ind < PR_MAX_DATA_LEN) && (string[*ind] == termCh));
    if (*done) {
        (*ind)++;
    }
    *done = *done && done1;
}


static picoos_int32 pr_removeSubstring (int pos, int len, unsigned char str[])
{
    int i;
    int length;

    length = pr_strlen(str);
    if (pos >= length) {
        return length;
    } else {
        i = pos + len;
        while (i < length) {
            str[pos] = str[i];
            i++;
            pos++;
        }
        str[pos] = 0;
        return pos;
    }
}


static picoos_bool pr_strEqual(picoos_uchar * str1, picoos_uchar * str2)
{
   return (picoos_strcmp((picoos_char *)str1, (picoos_char *)str2) == 0);
}


static void pr_int_to_string(picoos_int32 n, picoos_uchar * str, picoos_int32 maxstrlen)
{
    picoos_int32 i, len;
    picoos_bool negative=FALSE;

    len = 0;
    str[0] = 0;
    if (n<0) {
        negative = TRUE;
        n = -n;
        len++;
    }
    i = n;

    while (i>0) {
        i = i / 10;
        len++;
    }

    if (len<maxstrlen) {
        str[len] = 0;
        i = n;
        while ((i>0) && (len>0)) {
            len--;
            str[len] = i % 10 + '0';
            i = i / 10;
        }
        if (negative) {
            len--;
            str[len] = '-';
        }
    }
}
/* *****************************************************************************/

static void pr_firstLetterToLowerCase (const picoos_uchar src[], picoos_uchar dest[])
{

    picoos_int32 i;
    picoos_int32 j;
    picoos_int32 l;
    picoos_bool done;

    i = 0;
    j = 0;
    l = picobase_det_utf8_length(src[0]);
    while ((i < l) && (j < PR_MAX_DATA_LEN)) {
        dest[j] = src[i];
        i++;
        j++;
    }
    if (j < PR_MAX_DATA_LEN) {
        dest[j] = 0;
    }
    picobase_lowercase_utf8_str(dest, (picoos_char*)dest, PR_MAX_DATA_LEN, &done);
    j = picobase_det_utf8_length(dest[0]);
    l = pr_strlen(src);
    while ((i < l) && (j < PR_MAX_DATA_LEN)) {
        dest[j] = src[i];
        i++;
        j++;
    }
    dest[j] = 0;
}


static picoos_int32 tok_tokenDigitStrToInt (picodata_ProcessingUnit this, pr_subobj_t * pr, picoos_uchar stokenStr[])
{
    picoos_uint32 i;
    picoos_uint32 l;
    picoos_int32 id;
    picoos_int32 val;
    picoos_uint32 n;
    picobase_utf8char utf8char;

    val = 0;
    i = 0;
    l = pr_strlen(stokenStr);
    while (i < l) {
        picobase_get_next_utf8char(stokenStr, PR_MAX_DATA_LEN, & i, utf8char);
        id = picoktab_graphOffset(pr->graphs, utf8char);
        if (id > 0) {
          if (picoktab_getIntPropValue(pr->graphs, id, &n)) {
                val = (10 * val) + n;
            } else {
                val = ((10 * val) + (int)((int)utf8char[0] - (int)'0'));
            }
        } else if ((utf8char[0] >= '0') && (utf8char[0] <= '9')) {
            val = 10 * val + ((int)utf8char[0] - (int)'0');
        }
    }
    return val;
}


static picoos_bool pr_isLatinNumber (picoos_uchar str[], picoos_int32 * val)
{

    picoos_uint32 li;
    picoos_uint32 llen;
    picoos_uchar lact;
    picoos_uchar lnext;
    picoos_uchar lprev;
    picoos_uchar llatinI;
    picoos_uchar llatinV;
    picoos_uchar llatinX;
    picoos_uchar llatinL;
    picoos_uchar llatinC;
    picoos_uchar llatinD;
    picoos_uchar llatinM;
    picoos_int32 lseq;
    picobase_utf8char utf8;

    *val = 0;
    llen = picobase_utf8_length(str, PR_MAX_DATA_LEN);
    if (llen > 0) {
        li = 0;
        picobase_get_next_utf8char(str, PR_MAX_DATA_LEN, & li,utf8);
        if (picobase_is_utf8_uppercase(utf8, PICOBASE_UTF8_MAXLEN)) {
            llatinI = 'I';
            llatinV = 'V';
            llatinX = 'X';
            llatinL = 'L';
            llatinC = 'C';
            llatinD = 'D';
            llatinM = 'M';
        } else {
            llatinI = 'i';
            llatinV = 'v';
            llatinX = 'x';
            llatinL = 'l';
            llatinC = 'c';
            llatinD = 'd';
            llatinM = 'm';
        }
        lseq = 1000;
        li = 0;
        while (li < llen) {
            if (li > 0) {
                lprev = str[li - 1];
            } else {
                lprev = 0;
            }
            lact = str[li];
            if (li < (llen - 1)) {
                lnext = str[li + 1];
            } else {
                lnext = 0;
            }
            if ((lseq > 1) && (lact == llatinI)) {
                if ((lprev != lact) && (lseq >= 4)) {
                    if (lnext == llatinV) {
                        *val = *val + 4;
                        li++;
                        lseq = 1;
                    } else if (lnext == llatinX) {
                        *val = *val + 9;
                        li++;
                        lseq = 1;
                    } else {
                        *val = *val + 1;
                        lseq = 3;
                    }
                } else {
                    *val = *val + 1;
                    lseq = lseq - 1;
                }
            } else if ((lseq > 5) && (lact == llatinV)) {
                *val = *val + 5;
                lseq = 5;
            } else if ((lseq > 10) && (lact == llatinX)) {
                if ((lprev != lact) && (lseq >= 40)) {
                    if (lnext == llatinL) {
                        *val = *val + 40;
                        li++;
                        lseq = 10;
                    } else if (lnext == llatinC) {
                        *val = *val + 90;
                        li++;
                        lseq = 10;
                    } else {
                        *val = *val + 10;
                        lseq = 30;
                    }
                } else {
                    *val = *val + 10;
                    lseq = lseq - 10;
                }
            } else if ((lseq > 50) && (lact == llatinL)) {
                *val = *val + 50;
                lseq = 50;
            } else if ((lseq > 100) && (lact == llatinC)) {
                if ((lprev != lact) && (lseq >= 400)) {
                    if (lnext == llatinD) {
                        *val = *val + 400;
                        li++;
                        lseq = 100;
                    } else if (lnext == llatinM) {
                        *val = *val + 900;
                        li++;
                        lseq = 100;
                    } else {
                        *val = *val + 100;
                        lseq = 300;
                    }
                } else {
                    *val = *val + 100;
                    lseq = lseq - 100;
                }
            } else if ((lseq > 500) && (lact == llatinD)) {
                *val = *val + 500;
                lseq = 500;
            } else if ((lseq >= 1000) && (lact == llatinM)) {
                *val = *val + 1000;
            } else {
                return FALSE;
            }
            li++;
        }
    }
    return TRUE;
}


static picoos_bool pr_isSUC (picoos_uchar str[])
{

    picoos_int32 li;
    picoos_bool lis;
    picobase_utf8char lutf;
    picoos_int32 lj;
    picoos_int32 ll;
    picoos_bool luc;

    li = 0;
    lis = TRUE;
    luc = TRUE;
    while (lis && (li < PR_MAX_DATA_LEN) && (str[li] != 0)) {
        lj = 0;
        ll = picobase_det_utf8_length(str[li]);
        while (lj < ll) {
            lutf[lj] = str[li];
            lj++;
            li++;
        }
        lutf[lj] = 0;
        if (luc) {
            lis = lis && picobase_is_utf8_uppercase(lutf,PICOBASE_UTF8_MAXLEN+1);
        } else {
            lis = lis && picobase_is_utf8_lowercase(lutf,PICOBASE_UTF8_MAXLEN+1);
        }
        luc = FALSE;
    }
    return lis;
}

/* *****************************************************************************/

static picoos_bool pr_isCmdType (pr_ioItemPtr it, picoos_uint8 type)
{
    if ((it != NULL) && (it->head.type == PICODATA_ITEM_CMD) && (it->head.info1 == type)) {
        return TRUE;
    } else {
        return FALSE;
    }
}


static picoos_bool pr_isCmdInfo2 (pr_ioItemPtr it, picoos_uint8 info2)
{
    if ((it != NULL) && (it->head.type == PICODATA_ITEM_CMD) && (it->head.info2 == info2)) {
        return TRUE;
    } else {
        return FALSE;
    }
}


static void pr_initPathEle (struct pr_PathEle * ele)
{
    ele->rnetwork = NULL;
    ele->rtok = 0;
    ele->ritemid =  -1;
    ele->rdepth = 1;
    ele->rlState = PR_LSInit;
    ele->rcompare =  -1;
    ele->rprodname = 0;
    ele->rprodprefcost = 0;
}

/* *****************************************************************************/

static void pr_disposeProdList (register picodata_ProcessingUnit this,  pr_ProdList * prodList)
{
    pr_ProdList p;

    while ((*prodList) != NULL) {
        p = (*prodList);
        (*prodList) = (*prodList)->rNext;
        picoos_deallocate(this->common->mm, (void *) &p);
    }
}


static pico_Status pr_addContext (register picodata_ProcessingUnit this,  pr_subobj_t * pr, pr_ContextList * ctxList, picokpr_VarStrPtr contextNamePtr, picokpr_VarStrPtr netNamePtr, picokpr_VarStrPtr prodNamePtr)
{
    picokpr_Preproc net;
    pr_ContextList ctx;
    pr_ProdList prod;
    int i;
    picokpr_VarStrPtr strp;
    picoos_int32 lprodarrlen;

    ctx = (*ctxList);
    while ((ctx != NULL) &&  !(pr_strEqual(contextNamePtr, ctx->rContextName))) {
        ctx = ctx->rNext;
    }
    if (ctx == NULL) {
        ctx = picoos_allocate(this->common->mm, sizeof(pr_Context));
        if (ctx == NULL) {
            return PICO_EXC_OUT_OF_MEM;
        }
        ctx->rNext = (*ctxList);
        ctx->rProdList = NULL;
        ctx->rContextName = contextNamePtr;
        (*ctxList) = ctx;
    }
    i = 0;
    net = pr->preproc[i];
    while ((i<PR_MAX_NR_PREPROC) && (net != NULL) && !(pr_strEqual(netNamePtr, picokpr_getPreprocNetName(net)))) {
        i++;
        net = pr->preproc[i];
    }
    if (net != NULL) {
        i = 0;
        strp = picokpr_getVarStrPtr(net, picokpr_getProdNameOfs(net, i));
        lprodarrlen = picokpr_getProdArrLen(net);
        while ((i < lprodarrlen) &&  !(pr_strEqual(prodNamePtr, strp))) {
            i++;
            if (i < lprodarrlen) {
                strp = picokpr_getVarStrPtr(net, picokpr_getProdNameOfs(net, i));
            }
        }
        if (i < lprodarrlen) {
            prod = picoos_allocate(this->common->mm, sizeof(pr_Prod));
            if (prod == NULL) {
              return PICO_EXC_OUT_OF_MEM;
            }
            prod->rNetwork = net;
            prod->rProdOfs = i;
            prod->rNext = ctx->rProdList;
            ctx->rProdList = prod;
        }
    }
    return PICO_OK;
}


static pico_Status pr_createContextList (register picodata_ProcessingUnit this)
{
    pr_subobj_t * pr = (pr_subobj_t *) this->subObj;
    picokpr_VarStrPtr ctxNamePtr;
    picokpr_VarStrPtr netNamePtr;
    picokpr_VarStrPtr prodNamePtr;
    picoos_int32 p, i, n;
    pico_Status status;

    pr->ctxList = NULL;
    for (p=0; p<PR_MAX_NR_PREPROC; p++) {
        if (pr->preproc[p] != NULL) {
            n = picokpr_getCtxArrLen(pr->preproc[p]);
            for (i = 1; i<n; i++) {
                ctxNamePtr = picokpr_getVarStrPtr(pr->preproc[p], picokpr_getCtxCtxNameOfs(pr->preproc[p], i));
                netNamePtr = picokpr_getVarStrPtr(pr->preproc[p], picokpr_getCtxNetNameOfs(pr->preproc[p], i));
                prodNamePtr = picokpr_getVarStrPtr(pr->preproc[p], picokpr_getCtxProdNameOfs(pr->preproc[p], i));
                status = pr_addContext(this, pr, &pr->ctxList, ctxNamePtr,netNamePtr, prodNamePtr);
                if (status != PICO_OK) {
                    return status;
                }
            }
        }
    }
    return PICO_OK;
}


static void pr_disposeContextList (register picodata_ProcessingUnit this)
{
    pr_subobj_t * pr = (pr_subobj_t *) this->subObj;
    pr_ContextList c;

    while (pr->ctxList != NULL) {
        c = pr->ctxList;
        pr->ctxList = pr->ctxList->rNext;
        pr_disposeProdList(this, & c->rProdList);
        picoos_deallocate(this->common->mm, (void *) &c);
    }
}


static pr_ContextList pr_findContext (pr_ContextList contextList, picoos_uchar contextName[])
{
    pr_ContextList context;

    context = contextList;
    while ((context != NULL) &&  !(pr_strEqual(context->rContextName,contextName))) {
        context = context->rNext;
    }
    return context;
}


static void pr_setContext (register picodata_ProcessingUnit this,  pr_subobj_t * pr, picoos_uchar context[])
{

    pr_ContextList ctx;

    ctx = pr_findContext(pr->ctxList,context);
    if (ctx != NULL) {
        pr->actCtx = ctx;
        pr->actCtxChanged = TRUE;
    } else {
        PICODBG_WARN(("context '%s' not found; no change",context));
        picoos_emRaiseWarning(this->common->em, PICO_ERR_CONTEXT_NOT_FOUND, (picoos_char*)"context '%s' not found; no change",(picoos_char*)context);
    }
}

/* *****************************************************************************/
/* item handling routines */


static picoos_uint32 pr_copyData(picoos_uint8 * dest, const picoos_uint8 * src, picoos_int32 nrBytes, picoos_bool zeroTerm)
{
    picoos_int32 i=0;

    if ((src != NULL) && (dest != NULL)) {
        i = 0;
        while ((i<nrBytes) && (i<PR_MAX_DATA_LEN)) {
            dest[i] = src[i];
            i++;
        }
        if (zeroTerm) {
            dest[i] = 0;
        }
    }
    return i;
}


static void pr_initItem(picodata_ProcessingUnit this, pr_ioItem * item)
{
    item->next = NULL;
    item->val = 0;
    item->head.len = 0;
    item->strci = NULL;
    item->strcis = NULL;
    item->suc = FALSE;
    item->alc = FALSE;
    item->auc = FALSE;
}


static void pr_newItem (picodata_ProcessingUnit this, pr_MemTypes mType, pr_ioItemPtr * item, picoos_uint8 itemType, picoos_int32 size, picoos_bool inItem)
{
    pr_subobj_t * pr = (pr_subobj_t *) this->subObj;

    if (mType == pr_WorkMem) {
        pr_ALLOCATE(this, mType, (void * *) & (*item),PR_IOITEM_MIN_SIZE+size+1);
        if (pr->outOfMemory) return;
        pr_initItem(this, *item);
    }
    else if ((mType == pr_DynMem) && inItem) {
        pr_ALLOCATE(this, mType, (void * *) & (*item), PR_IOITEM_MIN_SIZE+3*size+3);
        if (pr->outOfMemory) return;
        pr_initItem(this, *item);
        if (itemType == PICODATA_ITEM_TOKEN) {
            (*item)->strci = &((*item)->data[size+1]);
            (*item)->strcis = &((*item)->data[2*size+2]);
            (*item)->strci[0] = 0;
            (*item)->strcis[0] = 0;
        }
    }
    else if ((mType == pr_DynMem) && !inItem) {
        pr_ALLOCATE(this, mType, (void * *) & (*item), PR_IOITEM_MIN_SIZE+size+1);
        if (pr->outOfMemory) return;
        pr_initItem(this, *item);
    }

    (*item)->data[0] = 0;
}


static void pr_copyItemContent (picodata_ProcessingUnit this, pr_ioItem * inItem, pr_ioItem * outItem)
{
    if (outItem != NULL) {
        outItem->next = inItem->next;
        outItem->val = inItem->val;
        outItem->head = inItem->head;
        outItem->suc = inItem->suc;
        outItem->alc = inItem->alc;
        outItem->auc = inItem->auc;
        if (inItem->head.len > 0 ) {
            pr_copyData(outItem->data, inItem->data, inItem->head.len, /*zeroTerm*/TRUE);
            pr_copyData(outItem->strci, inItem->strci, inItem->head.len, /*zeroTerm*/TRUE);
            pr_copyData(outItem->strcis, inItem->strcis, inItem->head.len, /*zeroTerm*/TRUE);
        }
    }
}


static void pr_copyItem (picodata_ProcessingUnit this, pr_MemTypes mType, pr_ioItemPtr inItem, pr_ioItemPtr * outItem)
{
    pr_subobj_t * pr = (pr_subobj_t *) this->subObj;

    if (inItem != NULL) {
        pr_newItem(this, mType,& (*outItem), inItem->head.type, inItem->head.len, FALSE);
        if (pr->outOfMemory) return;
        pr_copyItemContent(this, inItem, *outItem);
    }
    else {
        outItem = NULL;
    }
}


static void pr_startItemList (pr_ioItemPtr * firstItem, pr_ioItemPtr * lastItem)
{
  *firstItem = NULL;
  *lastItem = NULL;
}


static void pr_appendItem (picodata_ProcessingUnit this, pr_ioItemPtr * firstItem, pr_ioItemPtr * lastItem, pr_ioItemPtr item)
{
    if (item != NULL) {
        item->next = NULL;
        if ((*lastItem) == NULL) {
            *firstItem = item;
        } else {
            (*lastItem)->next = item;
        }
        (*lastItem) = item;
    }
}


static void pr_disposeItem (picodata_ProcessingUnit this, pr_ioItemPtr * item)
{
    if ((*item) != NULL) {
        pr_DEALLOCATE(this, pr_DynMem, (void * *) & (*item));
    }
}


static void pr_putItem (picodata_ProcessingUnit this,  pr_subobj_t * pr,
                        pr_ioItemPtr * first, pr_ioItemPtr * last,
                        picoos_uint8 itemType, picoos_uint8 info1, picoos_uint8 info2,
                        picoos_uint16 val,
                        picoos_uchar str[])
{
    picoos_int32 i;
    pr_ioItemPtr item;

    pr->tmpItem.next = NULL;
    pr->tmpItem.val = 0;
    pr->tmpItem.head.type = itemType;
    pr->tmpItem.head.info1 = info1;
    pr->tmpItem.head.info2 = info2;

    pr_initItem(this, &pr->tmpItem);
    switch (itemType) {
    case PICODATA_ITEM_CMD:
        switch (info1) {
        case PICODATA_ITEMINFO1_CMD_CONTEXT:
        case PICODATA_ITEMINFO1_CMD_VOICE:
        case PICODATA_ITEMINFO1_CMD_MARKER:
        case PICODATA_ITEMINFO1_CMD_PLAY:
        case PICODATA_ITEMINFO1_CMD_SAVE:
        case PICODATA_ITEMINFO1_CMD_UNSAVE:
        case PICODATA_ITEMINFO1_CMD_PROSDOMAIN:
            pr->tmpItem.head.len = picoos_strlen((picoos_char*)str);
            for (i=0; i<pr->tmpItem.head.len; i++) {
                pr->tmpItem.data[i] = str[i];
            }
            pr_copyItem(this, pr_WorkMem, &pr->tmpItem, & item);
            if (pr->outOfMemory) return;
            pr_appendItem(this, & (*first),& (*last),item);
            break;
        case PICODATA_ITEMINFO1_CMD_IGNSIG:
        case PICODATA_ITEMINFO1_CMD_IGNORE:
        case PICODATA_ITEMINFO1_CMD_FLUSH:
            pr->tmpItem.head.len = 0;
            pr_copyItem(this, pr_WorkMem, &pr->tmpItem, & item);
            if (pr->outOfMemory) return;
            pr_appendItem(this, & (*first),& (*last),item);
            break;
        case PICODATA_ITEMINFO1_CMD_SPEED:
        case PICODATA_ITEMINFO1_CMD_PITCH:
        case PICODATA_ITEMINFO1_CMD_VOLUME:
        case PICODATA_ITEMINFO1_CMD_SPELL:
        case PICODATA_ITEMINFO1_CMD_SIL:
            pr->tmpItem.head.len = 2;
            pr->tmpItem.data[0] = val % 256;
            pr->tmpItem.data[1] = val / 256;
            pr_copyItem(this, pr_WorkMem, &pr->tmpItem, & item);
            if (pr->outOfMemory) return;
            pr_appendItem(this, & (*first),& (*last),item);
           break;
        case PICODATA_ITEMINFO1_CMD_PHONEME:
            PICODBG_WARN(("phoneme command not yet implemented"));
            break;
        default:
            PICODBG_WARN(("pr_putItem: unknown command type"));
        }
        break;
    case PICODATA_ITEM_TOKEN:
        pr->tmpItem.head.len = picoos_strlen((picoos_char*)str);
        for (i=0; i<pr->tmpItem.head.len; i++) {
            pr->tmpItem.data[i] = str[i];
        }
        pr_copyItem(this, pr_WorkMem, &pr->tmpItem, & item);
        if (pr->outOfMemory) return;
        pr_appendItem(this, & (*first),& (*last),item);
        break;
     default:
         PICODBG_WARN(("pr_putItem: unknown item type"));
    }
}


static void pr_appendItemToOutItemList (picodata_ProcessingUnit this, pr_subobj_t * pr,
                                        pr_ioItemPtr * firstItem, pr_ioItemPtr * lastItem, pr_ioItemPtr item)
{
    pr_ioItemPtr litem;
    picoos_int32 li;
    picoos_int32 li2;
    picoos_int32 lid;
    picoos_int32 ln;
    picoos_int32 ln2;
    picoos_uint8 ltype;
    picoos_int8 lsubtype;
    picoos_uchar lstr[10];
    picoos_bool ldone;

    item->next = NULL;
    if (((pr->spellMode != 0) && (item->head.type == PICODATA_ITEM_TOKEN) && (item->head.info1 != PICODATA_ITEMINFO1_TOKTYPE_SPACE))) {
        li = 0;
        ln = pr_strlen(item->data);
        while (li < ln) {
            ln2 = picobase_det_utf8_length(item->data[li]);
            for (li2 = 0; li2<ln2; li2++) {
                lstr[li2] = item->data[li];
                li++;
            }
            lstr[ln2] = 0;
            lid = picoktab_graphOffset(pr->graphs, lstr);
            if ((lid > 0) && picoktab_getIntPropTokenType(pr->graphs, lid, &ltype) &&
                ((ltype == PICODATA_ITEMINFO1_TOKTYPE_LETTERV) /*|| (ltype == PICODATA_ITEMINFO1_TOKTYPE_DIGIT)*/)) {
                ln2 = pr_strcat(lstr,(picoos_uchar*)SPEC_CHAR);
                picoktab_getIntPropTokenSubType(pr->graphs,lid, &lsubtype);
            }
            else {
                ltype = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
                lsubtype =  -(1);
            }
            pr_newItem(this, pr_DynMem,& litem, PICODATA_ITEM_TOKEN, ln2, /*inItem*/FALSE);
            if (pr->outOfMemory) return;
            litem->head.type = PICODATA_ITEM_TOKEN;
            litem->head.info1 = item->head.info1;
            litem->head.info2 = item->head.info2;
            pr_strcpy(litem->data, lstr);
            litem->data[ln2] = 0;
            litem->head.len = ln2;
            if (litem->head.info1 == PICODATA_ITEMINFO1_TOKTYPE_DIGIT) {
                litem->val = tok_tokenDigitStrToInt(this, pr, litem->data);
            } else {
                litem->val = 0;
            }
            picobase_lowercase_utf8_str(litem->data,litem->strci,PR_MAX_DATA_LEN, &ldone);
            pr_firstLetterToLowerCase(litem->data,litem->strcis);
            litem->alc = picobase_is_utf8_lowercase(litem->data,PR_MAX_DATA_LEN);
            litem->auc = picobase_is_utf8_uppercase(litem->data,PR_MAX_DATA_LEN);
            litem->suc = pr_isSUC(litem->data);

            pr_appendItem(this, firstItem, lastItem, litem);
            if (pr->spellMode == PR_SPELL_WITH_PHRASE_BREAK) {
                pr_newItem(this, pr_DynMem,& litem, PICODATA_ITEM_TOKEN, 2, /*inItem*/FALSE);
                if (pr->outOfMemory) return;
                litem->head.type = PICODATA_ITEM_TOKEN;
                litem->head.info1 = PICODATA_ITEMINFO1_TOKTYPE_CHAR;
                litem->head.info2 =  -1;
                litem->head.len =  1;
                litem->data[0] = ',';
                litem->data[1] = 0;
                litem->strci[0] = ',';
                litem->strci[1] = 0;
                litem->strcis[0] = ',';
                litem->strcis[1] = 0;
                litem->val = 0;
                pr_appendItem(this, firstItem, lastItem, litem);
            } else if (pr->spellMode == PR_SPELL_WITH_SENTENCE_BREAK) {
                pr_newItem(this, pr_DynMem,& litem, PICODATA_ITEM_CMD, 0, /*inItem*/FALSE);
                if (pr->outOfMemory) return;
                litem->head.type = PICODATA_ITEM_CMD;
                litem->head.info1 = PICODATA_ITEMINFO1_CMD_FLUSH;
                litem->head.info2 = PICODATA_ITEMINFO2_NA;
                litem->head.len = 0;
                pr_appendItem(this, firstItem, lastItem,litem);
            } else if (pr->spellMode > 0) {
                pr_newItem(this, pr_DynMem,& litem, PICODATA_ITEM_CMD, 2, /*inItem*/FALSE);
                if (pr->outOfMemory) return;
                litem->head.type = PICODATA_ITEM_CMD;
                litem->head.info1 = PICODATA_ITEMINFO1_CMD_SIL;
                litem->head.info2 = PICODATA_ITEMINFO2_NA;
                litem->head.len = 2;
                litem->data[0] = pr->spellMode % 256;
                litem->data[1] = pr->spellMode / 256;
                pr_appendItem(this, firstItem, lastItem, litem);
            }
        }
        pr_disposeItem(this, & item);
    } else if (pr_isCmdType(item, PICODATA_ITEMINFO1_CMD_SPELL) && pr_isCmdInfo2(item, PICODATA_ITEMINFO2_CMD_START)) {
        pr->spellMode = item->data[0]+256*item->data[1];
        pr_disposeItem(this, & item);
    } else if (pr_isCmdType(item, PICODATA_ITEMINFO1_CMD_SPELL) && pr_isCmdInfo2(item, PICODATA_ITEMINFO2_CMD_END)) {
        pr->spellMode = 0;
        pr_disposeItem(this, & item);
    } else {
        pr_appendItem(this, firstItem,lastItem,item);
    }
}


/* *****************************************************************************/

static pr_OutItemVarPtr pr_findVariable (pr_OutItemVarPtr vars, picoos_int32 id)
{
    while ((vars != NULL) && (vars->id != id)) {
        vars = vars->next;
    }
    if ((vars != NULL)) {
        return vars;
    } else {
        return NULL;
    }
}


static void pr_genCommands (picodata_ProcessingUnit this, pr_subobj_t * pr,
                            picokpr_Preproc network, picokpr_OutItemArrOffset outitem, pr_OutItemVarPtr vars, pr_ioItemPtr * first, pr_ioItemPtr * last)
{

    pr_ioItemPtr litem;
    pr_OutItemVarPtr lvar;
    picoos_uint8 lcmd;
    picoos_uint8 linfo2;
    picoos_bool ldone;
#if 0
    picoos_int32 lphontype;
#endif
    picokpr_VarStrPtr lstrp;
    picoos_int32 lnum;
    pr_ioItemPtr lf;
    pr_ioItemPtr ll;
    picoos_int32 lf0beg;
    picoos_int32 lf0end;
    ShortStrParam lxfadebeg;
    ShortStrParam lxfadeend;
    picoos_bool lout;
    picoos_int32 ltype;
    picoos_int32 argOfs;
    picoos_int32 nextOfs;
    picoos_int32 nextOfs2;
    picoos_int32 nextOfs3;
    picoos_int32 nextOfs4;
    picoos_uchar alphabet[32];

    lcmd = 0;
    lnum = 0;
    litem = NULL;
    ltype = picokpr_getOutItemType(network, outitem);
    switch (ltype) {
        case PR_OIgnore:
            if (picokpr_getOutItemVal(network, outitem) == 0) {
                pr_putItem(this, pr, & (*first),& (*last),PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_IGNORE,PICODATA_ITEMINFO2_CMD_START,0,(picoos_uchar*)"");
            } else {
                pr_putItem(this, pr, & (*first),& (*last),PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_IGNORE,PICODATA_ITEMINFO2_CMD_END,0,(picoos_uchar*)"");
            }
            break;
        case PR_OPitch:   case PR_OSpeed:   case PR_OVolume:
            switch (ltype) {
                case PR_OPitch:
                    lcmd = PICODATA_ITEMINFO1_CMD_PITCH;
                    lnum = PICO_PITCH_DEFAULT;
                    break;
                case PR_OSpeed:
                    lcmd = PICODATA_ITEMINFO1_CMD_SPEED;
                    lnum = PICO_SPEED_DEFAULT;
                    break;
                case PR_OVolume:
                    lcmd = PICODATA_ITEMINFO1_CMD_VOLUME;
                    lnum = PICO_VOLUME_DEFAULT;
                    break;
            default:
                break;
            }
            if ((picokpr_getOutItemArgOfs(network, outitem) != 0)) {
                switch (picokpr_getOutItemType(network, picokpr_getOutItemArgOfs(network, outitem))) {
                    case PR_OVal:
                        pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD, lcmd,PICODATA_ITEMINFO2_CMD_ABSOLUTE, picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, outitem)),(picoos_uchar*)"");
                        break;
                    case PR_OVar:
                        lvar = pr_findVariable(vars,picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, outitem)));
                        if ((lvar != NULL) && (lvar->first != NULL) && (lvar->first->head.type == PICODATA_ITEM_TOKEN)) {
                            pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,lcmd,PICODATA_ITEMINFO2_CMD_ABSOLUTE,picoos_atoi((picoos_char*)lvar->first->data),(picoos_uchar*)"");
                        }
                        break;
                default:
                    pr_startItemList(& lf,& ll);
                    pr_getOutputItemList(this, pr, network,picokpr_getOutItemArgOfs(network, outitem),vars,& lf,& ll);
                    if (pr->outOfMemory) return;
                    if (((lf != NULL) && (lf->head.type == PICODATA_ITEM_TOKEN))) {
                        pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,lcmd,PICODATA_ITEMINFO2_CMD_ABSOLUTE,picoos_atoi((picoos_char*)lf->data),(picoos_uchar*)"");
                    }
                    break;
                }
            } else {
                pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,lcmd,PICODATA_ITEMINFO2_CMD_ABSOLUTE,lnum,(picoos_uchar*)"");
            }
            break;

        case PR_OPhonSVOXPA:   case PR_OPhonSAMPA:
            if (picokpr_getOutItemArgOfs(network, outitem) != 0) {
                if (ltype == PR_OPhonSVOXPA) {
                    picoos_strlcpy(alphabet, PICODATA_SVOXPA, sizeof(alphabet));
                }
                else {
                    picoos_strlcpy(alphabet, PICODATA_SAMPA, sizeof(alphabet));
                }
                pr_startItemList(& lf,& ll);
                pr_getOutputItemList(this, pr, network,picokpr_getOutItemArgOfs(network, outitem),vars,& lf,& ll);
                if (pr->outOfMemory) return;
                if (lf != NULL) {
                    ldone = FALSE;
                    if (lf->head.type == PICODATA_ITEM_TOKEN) {
                        if (picodata_mapPAStrToPAIds(pr->transducer, this->common, pr->xsampa_parser, pr->svoxpa_parser, pr->xsampa2svoxpa_mapper, lf->data, alphabet, pr->tmpStr1, sizeof(pr->tmpStr1)-1) == PICO_OK) {
                            pr_putItem(this, pr, & (*first),& (*last), PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PHONEME,
                                PICODATA_ITEMINFO2_CMD_START, 0, pr->tmpStr1);
                            ldone = TRUE;
                        }
                        else {
                            PICODBG_WARN(("cannot map phonetic string '%s'; synthesizeing text instead", lf->data));
                            picoos_emRaiseWarning(this->common->em, PICO_ERR_OTHER,(picoos_char*)"", (picoos_char*)"cannot map phonetic string '%s'; synthesizing text instead", lf->data);
                        }
                    }
                    if (ldone) {
                        lf = lf->next;
                        while (lf != NULL) {
                            if (lf->head.type == PICODATA_ITEM_TOKEN) {
                                pr_putItem(this, pr, & (*first),& (*last), PICODATA_ITEM_TOKEN, PICODATA_ITEMINFO1_CMD_PHONEME,
                                               PICODATA_ITEMINFO2_CMD_END, 0, (picoos_char *)"");
                            }
                            lf = lf->next;
                        }
                        pr_putItem(this, pr, & (*first),& (*last), PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PHONEME,
                            PICODATA_ITEMINFO2_CMD_END, 0, (picoos_char *)"");
                    }
                }
            }
            break;

        case PR_OSent:
            pr_putItem(this, pr, & (*first),& (*last), PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_FLUSH, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
            break;
        case PR_OPara:
            pr_putItem(this, pr, & (*first),& (*last), PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_FLUSH, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
            if (picokpr_getOutItemVal(network, outitem) == 1) {
                pr_putItem(this, pr, & (*first),& (*last), PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SIL, PICODATA_ITEMINFO2_NA, PICO_PARAGRAPH_PAUSE_DUR, (picoos_uchar*)"");
            }
            break;
        case PR_OBreak:
            if ((picokpr_getOutItemArgOfs(network, outitem) != 0)) {
                switch (picokpr_getOutItemType(network, picokpr_getOutItemArgOfs(network, outitem))) {
                    case PR_OVal:
                        pr_putItem(this, pr, & (*first),& (*last), PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SIL, PICODATA_ITEMINFO2_NA, picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, outitem)), (picoos_uchar*)"");
                        break;
                    case PR_OVar:
                        lvar = pr_findVariable(vars,picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, outitem)));
                        if ((lvar != NULL) && (lvar->first != NULL) && (lvar->first->head.type == PICODATA_ITEM_TOKEN)) {
                            pr_putItem(this, pr, & (*first),& (*last), PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SIL, PICODATA_ITEMINFO2_NA, picoos_atoi((picoos_char*)lvar->first->data), (picoos_uchar*)"");
                        }
                        break;
                default:
                    pr_startItemList(& lf,& ll);
                    pr_getOutputItemList(this, pr, network,picokpr_getOutItemArgOfs(network, outitem),vars,& lf,& ll);
                    if (pr->outOfMemory) return;
                    if (((lf != NULL) && (lf->head.type == PICODATA_ITEM_TOKEN))) {
                        pr_putItem(this, pr, & (*first),& (*last), PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SIL, PICODATA_ITEMINFO2_NA, picoos_atoi((picoos_char*)lf->data), (picoos_uchar*)"");
                    }
                    break;
                }
            }
            break;
        case PR_OVoice:   case PR_OContext:   case PR_OMark:
            if (picokpr_getOutItemType(network, outitem) == PR_OVoice) {
                lcmd = PICODATA_ITEMINFO1_CMD_VOICE;
                pr->tmpStr1[0] = 0;
                lnum = 1;
            } else if (picokpr_getOutItemType(network, outitem) == PR_OContext) {
                lcmd = PICODATA_ITEMINFO1_CMD_CONTEXT;
                pr_strcpy(pr->tmpStr1, (picoos_uchar*)PICO_CONTEXT_DEFAULT);
                lnum = 1;
            } else if ((picokpr_getOutItemType(network, outitem) == PR_OMark)) {
                lcmd = PICODATA_ITEMINFO1_CMD_MARKER;
                pr->tmpStr1[0] = 0;
                lnum = 0;
            }
            if (picokpr_getOutItemArgOfs(network, outitem) != 0) {
                switch (picokpr_getOutItemType(network, picokpr_getOutItemArgOfs(network, outitem))) {
                    case PR_OVar:
                        lvar = pr_findVariable(vars,picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, outitem)));
                        if (lvar != NULL) {
                            litem = lvar->first;
                        }
                        pr->tmpStr1[0] = 0;
                        while (litem != NULL) {
                            if (litem->head.type == PICODATA_ITEM_TOKEN) {
                                pr_strcat(pr->tmpStr1,litem->data);
                            }
                            litem = litem->next;
                        }
                        pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD, lcmd,lnum,0,pr->tmpStr1);
                        break;
                    case PR_OStr:
                        if (picokpr_getOutItemStrOfs(network, picokpr_getOutItemArgOfs(network, outitem)) != 0) {
                            lstrp = picokpr_getOutItemStr(network, picokpr_getOutItemArgOfs(network, outitem));
                            pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,lcmd,lnum,0,lstrp);
                        }
                        break;
                default:
                    pr_startItemList(& lf,& ll);
                    pr_getOutputItemList(this, pr, network,picokpr_getOutItemArgOfs(network, outitem),vars,& lf,& ll);
                    if (pr->outOfMemory) return;
                    if (((lf != NULL) && (lf->head.type == PICODATA_ITEM_TOKEN))) {
                        pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,lcmd,lnum,0,lf->data);
                    }
                    break;
                }
            } else {
                pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,lcmd,lnum,0,pr->tmpStr1);
            }
            break;
        case PR_OGenFile:
            if (picokpr_getOutItemArgOfs(network, outitem) != 0) {
                lcmd = PICODATA_ITEMINFO1_CMD_SAVE;
            } else {
                lcmd = PICODATA_ITEMINFO1_CMD_UNSAVE;
            }
            pr->tmpStr1[0] = 0;
            lnum = 0;
            if (picokpr_getOutItemArgOfs(network, outitem) != 0) {
                switch (picokpr_getOutItemType(network, picokpr_getOutItemArgOfs(network, outitem))) {
                    case PR_OVar:
                        lvar = pr_findVariable(vars,picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, outitem)));
                        if (lvar != NULL) {
                            litem = lvar->first;
                        }
                        pr->tmpStr1[0] = 0;
                        while (litem != NULL) {
                            if (litem->head.type == PICODATA_ITEM_TOKEN) {
                                pr_strcat(pr->tmpStr1,litem->data);
                            }
                            litem = litem->next;
                        }
                        if ((lnum = picodata_getPuTypeFromExtension(pr->tmpStr1, /*input*/FALSE)) != PICODATA_ITEMINFO2_CMD_TO_UNKNOWN) {
                            if (pr->saveFile[0] != 0) {
                              pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_UNSAVE,
                                  picodata_getPuTypeFromExtension(pr->saveFile, /*input*/FALSE),0,pr->saveFile);
                            }
                            pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD, lcmd,lnum,0,pr->tmpStr1);
                            pr_strcpy(pr->saveFile, pr->tmpStr1);
                        }
                        break;
                    case PR_OStr:
                        if (picokpr_getOutItemStrOfs(network, picokpr_getOutItemArgOfs(network, outitem)) != 0) {
                            lstrp = picokpr_getOutItemStr(network, picokpr_getOutItemArgOfs(network, outitem));
                            if ((lnum = picodata_getPuTypeFromExtension(lstrp, /*input*/FALSE)) != PICODATA_ITEMINFO2_CMD_TO_UNKNOWN) {
                                if (pr->saveFile[0] != 0) {
                                    pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_UNSAVE,
                                        picodata_getPuTypeFromExtension(pr->saveFile, /*input*/FALSE),0,pr->saveFile);
                                }
                                pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD, lcmd,lnum,0,lstrp);
                                pr_strcpy(pr->saveFile, lstrp);
                            }
                            pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,lcmd,lnum,0,lstrp);
                        }
                        break;
                default:
                    pr_startItemList(& lf,& ll);
                    pr_getOutputItemList(this, pr, network,picokpr_getOutItemArgOfs(network, outitem),vars,& lf,& ll);
                    if (pr->outOfMemory) return;
                    if (((lf != NULL) && (lf->head.type == PICODATA_ITEM_TOKEN))) {
                        if ((lnum = picodata_getPuTypeFromExtension(lf->data, /*input*/FALSE)) != PICODATA_ITEMINFO2_CMD_TO_UNKNOWN) {
                            if (pr->saveFile[0] != 0) {
                                pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_UNSAVE,
                                    picodata_getPuTypeFromExtension(pr->saveFile, /*input*/FALSE),0,pr->saveFile);
                            }
                            pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD, lcmd,lnum,0,lf->data);
                            pr_strcpy(pr->saveFile, lf->data);
                        }
                    }
                    break;
                }
/*
            } else {
                pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,lcmd,lnum,0,pr->tmpStr1);
*/
            }
            break;
        case PR_OUseSig:   case PR_OPlay:
            lout = FALSE;
            lf0beg =  -(1);
            lf0end =  -(1);
            lxfadebeg[0] = 0;
            lxfadeend[0] = 0;
            pr->tmpStr1[0] = 0;
            if ((picokpr_getOutItemType(network, outitem) == PR_OUseSig)) {
                lcmd = PICODATA_ITEMINFO1_CMD_IGNSIG;
            } else {
                lcmd = PICODATA_ITEMINFO1_CMD_IGNORE;
            }
            if (picokpr_getOutItemArgOfs(network, outitem) != 0) {
                linfo2 = PICODATA_ITEMINFO2_CMD_START;
            } else {
                linfo2 = PICODATA_ITEMINFO2_CMD_END;
            }
            if (picokpr_getOutItemArgOfs(network, outitem) != 0) {
                switch (picokpr_getOutItemType(network, picokpr_getOutItemArgOfs(network, outitem))) {
                    case PR_OVar:
                        lvar = pr_findVariable(vars,picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, outitem)));
                        if (lvar != NULL) {
                            litem = lvar->first;
                        }
                        pr->tmpStr1[0] = 0;
                        while (litem != NULL) {
                            if (litem->head.type == PICODATA_ITEM_TOKEN) {
                                pr_strcat(pr->tmpStr1,litem->data);
                            }
                            litem = litem->next;
                        }
                        pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PLAY,
                            picodata_getPuTypeFromExtension(pr->tmpStr1, /*input*/TRUE),0, pr->tmpStr1);
                        lout = TRUE;
                        break;
                    case PR_OStr:
                        if (picokpr_getOutItemStrOfs(network, picokpr_getOutItemArgOfs(network, outitem)) != 0) {
                            lstrp = picokpr_getOutItemStr(network, picokpr_getOutItemArgOfs(network, outitem));
                            pr_strcpy(pr->tmpStr1, lstrp);
                            lout = TRUE;
                        }
                        break;
                default:
                    pr_startItemList(& lf,& ll);
                    pr_getOutputItemList(this, pr, network,picokpr_getOutItemArgOfs(network, outitem),vars,& lf,& ll);
                    if (pr->outOfMemory) return;
                    if ((lf != NULL) && (lf->head.type == PICODATA_ITEM_TOKEN)) {
                        pr_strcpy(pr->tmpStr1, lf->data);
                        lout = TRUE;
                    }
                    break;
                }
            }
            argOfs = picokpr_getOutItemArgOfs(network, outitem);
            if (argOfs != 0) {
                nextOfs = picokpr_getOutItemNextOfs(network, outitem);
                if (nextOfs != 0) {
                    if (picokpr_getOutItemType(network, nextOfs) == PR_OVal) {
                        lf0beg = picokpr_getOutItemVal(network, nextOfs);
                    }
                    nextOfs2 = picokpr_getOutItemNextOfs(network, nextOfs);
                    if (nextOfs2 != 0) {
                        if (picokpr_getOutItemType(network, nextOfs2) == PR_OVal) {
                            lf0end = picokpr_getOutItemVal(network, nextOfs2);
                        }
                        nextOfs3 = picokpr_getOutItemNextOfs(network, nextOfs2);
                        if (nextOfs3 != 0) {
                            if ((picokpr_getOutItemType(network, nextOfs3) == PR_OStr) && (picokpr_getOutItemStrOfs(network, nextOfs3) != 0)) {
                                lstrp = picokpr_getOutItemStr(network, nextOfs3);
                                pr_strcpy(lxfadebeg, lstrp);
                            }
                            nextOfs4 = picokpr_getOutItemNextOfs(network, nextOfs3);
                            if (nextOfs4 != 0) {
                                if ((picokpr_getOutItemType(network, nextOfs4) == PR_OStr) && (picokpr_getOutItemStrOfs(network, nextOfs4) != 0)) {
                                    lstrp = picokpr_getOutItemStr(network, nextOfs4);
                                    pr_strcpy(lxfadeend, lstrp);
                                }
                            }
                        }
                    }
                }
            }
            if (lout) {
                pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,PICODATA_ITEMINFO1_CMD_PLAY,
                    picodata_getPuTypeFromExtension(pr->tmpStr1, /*input*/TRUE),0,pr->tmpStr1);
            }
            pr_putItem(this, pr,& (*first),& (*last),PICODATA_ITEM_CMD,lcmd,linfo2,0,(picoos_uchar*)"");
            break;
    default:
        PICODBG_INFO(("unknown command"));
        break;
    }
}


static void pr_getOutputItemList (picodata_ProcessingUnit this,
                                  pr_subobj_t * pr,
                                  picokpr_Preproc network,
                                  picokpr_OutItemArrOffset outitem,
                                  pr_OutItemVarPtr vars,
                                  pr_ioItemPtr * first,
                                  pr_ioItemPtr * last)
{

    picokpr_OutItemArrOffset lo;
    picoos_int32 llen;
    picoos_int32 llen2;
    picokpr_VarStrPtr lstrp;
    picoos_int32 lval32;
    picoos_int32 li;
    picoos_int32 li2;
    picoos_int32 ln;
    picoos_int32 ln2;
    pr_ioItemPtr litem2;
    pr_ioItemPtr lf;
    pr_ioItemPtr ll;
    picoos_int32 lid;
    picoos_uint8 ltype;
    picoos_int8 lsubtype;
    pr_OutItemVarPtr lvar;
    picoos_int32 lspellmode;
    picoos_int32 largOfs;
    picoos_int32 lnextOfs;


    lo = outitem;
    while (lo != 0) {
        switch (picokpr_getOutItemType(network, lo)) {
            case PR_OStr:
                lstrp = picokpr_getOutItemStr(network, lo);
                if (pr->outOfMemory) return;
                pr_initItem(this, &pr->tmpItem);
                pr->tmpItem.head.type = PICODATA_ITEM_TOKEN;
                pr->tmpItem.head.info1 = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
                pr->tmpItem.head.info2 =  -1;
                pr->tmpItem.head.len = pr_strcpy(pr->tmpItem.data, lstrp);
                pr_copyItem(this, pr_WorkMem, &pr->tmpItem, &litem2);
                if (pr->outOfMemory) return;
                pr_appendItem(this, & (*first),& (*last),litem2);
                break;
            case PR_OVar:
                lvar = pr_findVariable(vars,picokpr_getOutItemVal(network, lo));
                if (lvar != NULL) {
                    lf = lvar->first;
                } else {
                    lf = NULL;
                }
                while (lf != NULL) {
                    pr_copyItem(this, pr_WorkMem,& (*lf),& litem2);
                    if (pr->outOfMemory) return;
                    pr_appendItem(this, & (*first),& (*last),litem2);
                    lf = lf->next;
                }
                break;
            case PR_OSpell:
                lspellmode = PR_SPELL;
                largOfs = picokpr_getOutItemArgOfs(network, lo);
                if ((largOfs!= 0) && ((lnextOfs = picokpr_getOutItemNextOfs(network, largOfs)) != 0)) {
                    lspellmode = picokpr_getOutItemVal(network, lnextOfs);
                }
                pr_startItemList(& lf,& ll);
                pr_getOutputItemList(this, pr, network,largOfs,vars,& lf,& ll);
                if (pr->outOfMemory) return;
                while (lf != NULL) {
                    switch (lf->head.type) {
                        case PICODATA_ITEM_TOKEN:
                            li = 0;
                            ln = pr_strlen(lf->data);
                            while (li < ln) {
                                pr_initItem(this, &pr->tmpItem);
                                if (pr->outOfMemory) return;
                                pr->tmpItem.head.type = PICODATA_ITEM_TOKEN;
                                pr->tmpItem.head.info1 = lf->head.info1;
                                pr->tmpItem.head.info2 = lf->head.info2;
                                pr->tmpItem.head.len = picobase_det_utf8_length(lf->data[li]);
                                for (li2 = 0; li2 < pr->tmpItem.head.len; li2++) {
                                    pr->tmpItem.data[li2] = lf->data[li];
                                    li++;
                                }
                                pr->tmpItem.data[pr->tmpItem.head.len] = 0;
                                pr->tmpItem.val = 0;
                                lid = picoktab_graphOffset(pr->graphs,pr->tmpItem.data);
                                if (lid > 0) {
                                    if (picoktab_getIntPropTokenType(pr->graphs, lid, &ltype)) {
                                        if ((ltype == PICODATA_ITEMINFO1_TOKTYPE_LETTERV)/* || (ltype == PICODATA_ITEMINFO1_TOKTYPE_DIGIT)*/) {
                                            pr->tmpItem.head.len = pr_strcat(pr->tmpItem.data,(picoos_uchar*)SPEC_CHAR);
                                        }
                                    }
                                    picoktab_getIntPropTokenSubType(pr->graphs, lid, &lsubtype);
                                } else {
                                    ltype = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
                                    lsubtype =  -(1);
                                }
                                pr_copyItem(this, pr_WorkMem, &pr->tmpItem, &litem2);
                                if (pr->outOfMemory) return;
                                pr_appendItem(this, & (*first),& (*last), litem2);
                                if (lspellmode == PR_SPELL_WITH_PHRASE_BREAK) {
                                    pr_initItem(this, &pr->tmpItem);
                                    pr->tmpItem.head.type = PICODATA_ITEM_TOKEN;
                                    pr->tmpItem.head.info1 = PICODATA_ITEMINFO1_TOKTYPE_CHAR;
                                    pr->tmpItem.head.info2 = lsubtype;
                                    pr->tmpItem.head.len = 1;
                                    pr->tmpItem.data[0] = ',';
                                    pr->tmpItem.data[1] = 0;
                                    pr->tmpItem.val = 0;
                                    pr_copyItem(this, pr_WorkMem, &pr->tmpItem, &litem2);
                                    if (pr->outOfMemory) return;
                                    pr_appendItem(this, & (*first),& (*last),litem2);
                                } else if (lspellmode == PR_SPELL_WITH_SENTENCE_BREAK) {
                                    pr_initItem(this, &pr->tmpItem);
                                    pr->tmpItem.head.type = PICODATA_ITEM_CMD;
                                    pr->tmpItem.head.info1 = PICODATA_ITEMINFO1_CMD_FLUSH;
                                    pr->tmpItem.head.info2 = PICODATA_ITEMINFO2_NA;
                                    pr->tmpItem.head.len = 0;
                                    pr_copyItem(this, pr_WorkMem, &pr->tmpItem, &litem2);
                                    if (pr->outOfMemory) return;
                                    pr_appendItem(this, & (*first),& (*last),litem2);
                                } else if (lspellmode > 0) {
                                    pr_initItem(this, &pr->tmpItem);
                                    pr->tmpItem.head.type = PICODATA_ITEM_TOKEN;
                                    pr->tmpItem.head.info1 = PICODATA_ITEMINFO1_TOKTYPE_CHAR;
                                    pr->tmpItem.head.info2 = lsubtype;
                                    pr->tmpItem.head.len = 1;
                                    pr->tmpItem.data[0] = ',';
                                    pr->tmpItem.data[1] = 0;
                                    pr->tmpItem.val = 0;
                                    pr_copyItem(this, pr_WorkMem, &pr->tmpItem, &litem2);
                                    if (pr->outOfMemory) return;
                                    pr_appendItem(this, & (*first),& (*last),litem2);
                                }
                            }
                            break;
                    default:
                        pr_copyItem(this, pr_WorkMem,& (*lf),& litem2);
                        if (pr->outOfMemory) return;
                        pr_appendItem(this, & (*first),& (*last),litem2);
                        break;
                    }
                    ll = lf;
                    lf = lf->next;
                    ll->next = NULL;
                }
                break;
            case PR_OConcat:
                pr_startItemList(& lf,& ll);
                pr_getOutputItemList(this, pr, network,picokpr_getOutItemArgOfs(network, lo),vars,& lf,& ll);
                if (pr->outOfMemory) return;
                pr_initItem(this, &pr->tmpItem);
                pr->tmpItem.head.type = PICODATA_ITEM_TOKEN;
                pr->tmpItem.head.info1 = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
                pr->tmpItem.head.info2 =  -(1);
                pr->tmpItem.head.len = 0;
                pr->tmpItem.data[0] = 0;
                pr->tmpItem.val = 0;
                while (lf != NULL) {
                    switch (lf->head.type) {
                        case PICODATA_ITEM_TOKEN:
                            pr->tmpItem.head.len = pr_strcat(pr->tmpItem.data,lf->data);
                            break;
                        case PICODATA_ITEM_CMD:
                            pr_copyItem(this, pr_WorkMem, &pr->tmpItem, &litem2);
                            if (pr->outOfMemory) return;
                            pr_appendItem(this, & (*first),& (*last),litem2);

                            pr_copyItem(this, pr_WorkMem, lf, &litem2);
                            if (pr->outOfMemory) return;
                            pr_appendItem(this, & (*first),& (*last),litem2);

                            pr_initItem(this, &pr->tmpItem);
                            pr->tmpItem.head.type = PICODATA_ITEM_TOKEN;
                            pr->tmpItem.head.info1 = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
                            pr->tmpItem.head.info2 =  -(1);
                            pr->tmpItem.head.len = 0;
                            pr->tmpItem.data[0] = 0;
                            pr->tmpItem.val = 0;
                            break;
                    default:
                        break;
                    }
                    lf = lf->next;
                }
                pr_copyItem(this, pr_WorkMem, &pr->tmpItem, &litem2);
                if (pr->outOfMemory) return;
                pr_appendItem(this, & (*first),& (*last),litem2);
                break;
            case PR_ORomanToCard:
                pr_startItemList(& lf,& ll);
                pr_getOutputItemList(this, pr, network,picokpr_getOutItemArgOfs(network, lo),vars,& lf,& ll);
                if (pr->outOfMemory) return;
                if ((lf != NULL) && (lf->head.type == PICODATA_ITEM_TOKEN)) {
                    pr_initItem(this, &pr->tmpItem);
                    pr_copyItemContent(this, lf, &pr->tmpItem);
                    if (pr_isLatinNumber(lf->data, & lval32)) {
                        pr_int_to_string(lval32, (picoos_char *)pr->tmpItem.data, PR_MAX_DATA_LEN_Z);
                        pr->tmpItem.head.len = pr_strlen(pr->tmpItem.data);
                        pr->tmpItem.val = lval32;
                    }
                    pr_copyItem(this, pr_WorkMem, &pr->tmpItem, &litem2);
                    pr_appendItem(this, & (*first),& (*last),litem2);
                }
                break;
            case PR_OVal:
                break;
            case PR_OLeft:
                pr_startItemList(& lf,& ll);
                pr_getOutputItemList(this, pr, network,picokpr_getOutItemNextOfs(network, picokpr_getOutItemArgOfs(network, lo)),vars,& lf,& ll);
                if (pr->outOfMemory) return;
                if ((lf != NULL) && (lf->head.type == PICODATA_ITEM_TOKEN)) {
                    pr_initItem(this, &pr->tmpItem);
                    pr_copyItemContent(this, lf, &pr->tmpItem);
                    llen = lf->head.len;
                    llen2 = picobase_utf8_length(pr->tmpItem.data,PR_MAX_DATA_LEN);
                    ln = 0;
                    ln2 = 0;
                    largOfs = picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, lo));
                    while ((ln < llen) && (ln2 < llen2) && (ln2 < largOfs)) {
                        ln = ln + picobase_det_utf8_length(pr->tmpItem.data[ln]);
                        ln2++;
                    }
                    pr->tmpItem.data[ln] = 0;
                    pr->tmpItem.head.len = ln;
                    pr_copyItem(this, pr_WorkMem, &pr->tmpItem, &litem2);
                    if (pr->outOfMemory) return;
                    pr_appendItem(this, & (*first),& (*last),litem2);
                }
                break;
            case PR_ORight:
                pr_startItemList(& lf,& ll);
                pr_getOutputItemList(this, pr, network,picokpr_getOutItemNextOfs(network, picokpr_getOutItemArgOfs(network, lo)),vars,& lf,& ll);
                if (pr->outOfMemory) return;
                if ((lf != NULL) && (lf->head.type == PICODATA_ITEM_TOKEN)) {
                    pr_initItem(this, &pr->tmpItem);
                    pr_copyItemContent(this, lf, & pr->tmpItem);
                    llen = lf->head.len;
                    llen2 = picobase_utf8_length(pr->tmpItem.data,PR_MAX_DATA_LEN);
                    ln = 0;
                    ln2 = 0;
                    while ((ln < llen) && (ln2 < llen2) && (ln2 < (llen2 - picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, lo))))) {
                        ln = ln + picobase_det_utf8_length(pr->tmpItem.data[ln]);
                        ln2++;
                    }
                    pr->tmpItem.head.len = pr_removeSubstring(0,ln,pr->tmpItem.data);
                    pr_copyItem(this, pr_WorkMem, &pr->tmpItem, & litem2);
                    if (pr->outOfMemory) return;
                    pr_appendItem(this, & (*first),& (*last),litem2);
                }
                break;
            case PR_OItem:
                pr_startItemList(& lf,& ll);
                pr_getOutputItemList(this, pr, network,picokpr_getOutItemNextOfs(network, picokpr_getOutItemArgOfs(network, lo)),vars,& lf,& ll);
                if (pr->outOfMemory) return;
                ln = picokpr_getOutItemVal(network, picokpr_getOutItemArgOfs(network, lo));
                li = 1;
                while ((li < ln) && (lf != NULL)) {
                    lf = lf->next;
                    li++;
                }
                if ((lf != NULL) && (li == ln) && (lf->head.type == PICODATA_ITEM_TOKEN)) {
                    pr_copyItem(this, pr_WorkMem, lf, & litem2);
                    if (pr->outOfMemory) return;
                    pr_appendItem(this, & (*first),& (*last),litem2);
                }
                break;
            case PR_ORLZ:
                pr_startItemList(& lf,& ll);
                pr_getOutputItemList(this, pr, network, picokpr_getOutItemArgOfs(network, lo),vars,& lf,& ll);
                if (pr->outOfMemory) return;
                if ((lf != NULL) && (lf->head.type == PICODATA_ITEM_TOKEN)) {
                    pr_initItem(this, &pr->tmpItem);
                    pr_copyItemContent(this, lf, & pr->tmpItem);
                    li = 0;
                    while ((li < lf->head.len) && (pr->tmpItem.data[li] == '0')) {
                        li++;
                    }
                    pr->tmpItem.head.len = pr_removeSubstring(0,li,pr->tmpItem.data);
                    pr_copyItem(this, pr_WorkMem, &pr->tmpItem, & litem2);
                    if (pr->outOfMemory) return;
                    pr_appendItem(this, & (*first),& (*last),litem2);
                }
                break;
            case PR_OIgnore:   case PR_OPitch:   case PR_OSpeed:   case PR_OVolume:   case PR_OPhonSVOXPA:   case PR_OPhonSAMPA:   case PR_OBreak:   case PR_OMark:   case PR_OPara:   case PR_OSent:   case PR_OPlay:
            case PR_OUseSig:   case PR_OGenFile:   case PR_OAudioEdit:   case PR_OContext:   case PR_OVoice:
                pr_genCommands(this, pr, network,lo,vars,& (*first),& (*last));
                if (pr->outOfMemory) return;
                break;
        default:
            PICODBG_INFO(("unkown command"));
            break;
        }
        lo = picokpr_getOutItemNextOfs(network, lo);
    }
}


static picoos_int32 pr_attrVal (picokpr_Preproc network, picokpr_TokArrOffset tok, pr_TokSetEleWP type)
{

    pr_TokSetEleWP tse;
    picoos_int32 n;
    picokpr_TokSetWP set;

    n = 0;
    tse = PR_FIRST_TSE_WP;
    set = picokpr_getTokSetWP(network, tok);
    while (tse < type) {
        if (((1<<tse) & set) != 0) {
            n++;
        }
        tse = (pr_TokSetEleWP)((picoos_int32)tse+1);
    }
    return picokpr_getAttrValArrInt32(network, picokpr_getTokAttribOfs(network, tok) + n);
}


static void pr_getOutput (picodata_ProcessingUnit this, pr_subobj_t * pr,
                          picoos_int32 * i, picoos_int32 d, pr_ioItemPtr * o, pr_ioItemPtr * ol)
{

    register struct pr_PathEle * with__0;
    pr_OutItemVarPtr lvars;
    pr_OutItemVarPtr lvar;
    pr_ioItemPtr lit;
    pr_ioItemPtr ldit;
    pr_ioItemPtr ldlit;
    picoos_bool lfirst;
    pr_ioItemPtr lcopy;
    picokpr_TokSetWP wpset;
    picokpr_TokSetNP npset;
    picoos_int32 li;

    lfirst = TRUE;
    (*i)++;
    lit = NULL;
    lvars = NULL;
    ldit = NULL;
    ldlit = NULL;
    while ((*i) < pr->rbestpath.rlen) {
        with__0 = & pr->rbestpath.rele[*i];
        li = 0;
        if (*i > 0) {
            while ((li < 127) && (li < pr->rbestpath.rele[*i].rdepth)) {
                pr->lspaces[li++] = ' ';
            }
        }
        pr->lspaces[li] = 0;
        if (with__0->rprodname != 0) {
            PICODBG_INFO(("pp path  :%s%s(", pr->lspaces, picokpr_getVarStrPtr(with__0->rnetwork,with__0->rprodname)));
        }
        if ((pr->ritems[with__0->ritemid+1] != NULL) && (pr->ritems[with__0->ritemid+1]->head.type == PICODATA_ITEM_TOKEN)) {
            PICODBG_INFO(("pp in (1): %s'%s'", pr->lspaces, pr->ritems[with__0->ritemid+1]->data));
        }
        if ((pr->ritems[with__0->ritemid+1] != NULL)) {
            while ((pr->rinItemList != NULL) && (pr->rinItemList != pr->ritems[with__0->ritemid+1]) && (pr->rinItemList->head.type != PICODATA_ITEM_TOKEN)) {
                lit = pr->rinItemList;
                pr->rinItemList = pr->rinItemList->next;
                lit->next = NULL;
                pr_copyItem(this, pr_WorkMem,& (*lit),& lcopy);
                if (pr->outOfMemory) return;
                pr_disposeItem(this, & lit);
                pr_appendItem(this, & (*o),& (*ol),lcopy);
            }
            if (pr->rinItemList != NULL) {
                lit = pr->rinItemList;
                pr->rinItemList = pr->rinItemList->next;
                lit->next = NULL;
            } else {
                lit = NULL;
            }
        }
        wpset = picokpr_getTokSetWP(with__0->rnetwork, with__0->rtok);
        npset = picokpr_getTokSetNP(with__0->rnetwork, with__0->rtok);

        if ((PR_TSE_MASK_PROD & wpset) != 0) {
            if ((PR_TSE_MASK_VAR & wpset) != 0) {
                lvar = pr_findVariable(lvars,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEVar));
                if (lvar == NULL) {
                    pr_ALLOCATE(this, pr_WorkMem, (void *) & lvar,(sizeof (*lvar)));
                    lvar->next = lvars;
                    lvar->id = pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEVar);
                    lvars = lvar;
                }
                pr_startItemList(& lvar->first,& lvar->last);
                pr_getOutput(this, pr, & (*i),(d + 1),& lvar->first,& lvar->last);
                if (pr->outOfMemory) return;
            } else {
                pr_startItemList(& ldit,& ldlit);
                pr_getOutput(this, pr, & (*i),(d + 1),& ldit,& ldlit);
                if (pr->outOfMemory) return;
            }
            (*i)++;
        } else if ((PR_TSE_MASK_VAR & wpset) != 0) {
            lvar = pr_findVariable(lvars,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEVar));
            if (lvar == NULL) {
                pr_ALLOCATE(this, pr_WorkMem, (void *) & lvar,(sizeof (*lvar)));
                lvar->next = lvars;
                lvar->id = pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEVar);
                lvars = lvar;
            }
            if (((PR_TSE_MASK_LEX & wpset) != 0) && ((PR_TSE_MASK_LETTER & npset) == 0)) {
                if (lfirst) {
                    pr_newItem(this, pr_WorkMem,& lit, PICODATA_ITEM_TOKEN, sizeof(struct pr_ioItem), /*inItem*/FALSE);
                    if (pr->outOfMemory) return;
                    lit->head.type = PICODATA_ITEM_TOKEN;
                    lit->head.info1 = pr->ritems[with__0->ritemid+1]->head.info1;
                    lit->head.info2 = pr->ritems[with__0->ritemid+1]->head.info2;
                    if (pr->ritems[with__0->ritemid+1]->head.info1 == PICODATA_ITEMINFO1_TOKTYPE_SPACE) {
                        lit->head.len = pr_strcpy(lit->data, (picoos_uchar*)"_");
                    } else {
                        lit->head.len = pr_strcpy(lit->data, pr->ritems[with__0->ritemid+1]->data);
                    }
                    lvar->first = lit;
                    lvar->last = lit;
                    lfirst = FALSE;
                } else {
                    if ((pr->ritems[with__0->ritemid+1]->head.info1 == PICODATA_ITEMINFO1_TOKTYPE_SPACE)) {
                        lvar->last->head.len = pr_strcat(lvar->last->data,(picoos_uchar*)"_");
                    } else {
                        lvar->last->head.len = pr_strcat(lvar->last->data,pr->ritems[with__0->ritemid+1]->data);
                    }
                    lvar->last->head.info1 = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
                    lvar->last->head.info2 =  -(1);
                }
            } else {
                lvar->first = pr->ritems[with__0->ritemid+1];
                lvar->last = pr->ritems[with__0->ritemid+1];
            }
            (*i)++;
        } else if ((PR_TSE_MASK_OUT & wpset) != 0) {
            pr_getOutputItemList(this, pr, with__0->rnetwork,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEOut),lvars,& (*o),& (*ol));
            if (pr->outOfMemory) return;
            (*i)++;
        } else if (((*i) < (pr->rbestpath.rlen - 1)) && (d != pr->rbestpath.rele[(*i) + 1].rdepth)) {
            if ((*i > 0) && (with__0->rdepth-1) == pr->rbestpath.rele[*i + 1].rdepth) {
                li = 0;
                while ((li < 127) && (li < with__0->rdepth-1)) {
                    pr->lspaces[li++] = ' ';
                }
                pr->lspaces[li] = 0;
                PICODBG_INFO(("pp path  :%s)", pr->lspaces));
            }
            return;
        } else {
            (*i)++;
        }
        if ((PR_TSE_MASK_LEX & wpset) == 0) {
            lfirst = TRUE;
        }
    }
    li = 0;
    while ((li < 127) && (li < pr->rbestpath.rele[*i].rdepth-1)) {
        pr->lspaces[li++] = ' ';
    }
    pr->lspaces[li] = 0;
    PICODBG_INFO(("pp path  :%s)", pr->lspaces));
}


static void pr_outputPath (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    register struct pr_PathEle * with__0;
    picoos_int32 li;
    pr_ioItemPtr lf;
    pr_ioItemPtr ll;
    pr_ioItemPtr lit;
    pr_ioItemPtr lit2;
    pr_MemState lmemState;
    picoos_bool lastPlayFileFound;

    pr_getMemState(this, pr_WorkMem, & lmemState);
    lf = NULL;
    ll = NULL;
    li =  -(1);
    pr_getOutput(this, pr, & li,1,& lf,& ll);
    if (pr->outOfMemory) return;
    lastPlayFileFound = TRUE;
    while (lf != NULL) {
        lit = lf;
        lf = lf->next;
        lit->next = NULL;
        if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PLAY)) {
            lastPlayFileFound = picoos_FileExists(this->common, (picoos_char*)lit->data);
            if (!lastPlayFileFound) {
                PICODBG_WARN(("file '%s' not found; synthesizing enclosed text instead", lit->data));
                picoos_emRaiseWarning(this->common->em, PICO_EXC_CANT_OPEN_FILE, (picoos_char*)"", (picoos_char*)"file '%s' not found; synthesizing enclosed text instead",lit->data);
            }
        }
        if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PHONEME) && pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_START)) {
            pr->insidePhoneme = TRUE;
        } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PHONEME)&& pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_END)) {
            pr->insidePhoneme = FALSE;
        }
        if ((pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PLAY) &&  !lastPlayFileFound)) {
        } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_IGNORE) && pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_START)) {
            if (lastPlayFileFound) {
                pr->rignore++;
            }
        } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_IGNORE) && pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_END)) {
            if (lastPlayFileFound) {
                if (pr->rignore > 0) {
                    pr->rignore--;
                }
            }
        } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_IGNSIG) && pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_START) &&  !lastPlayFileFound) {
        } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_IGNSIG) && pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_END) &&  !lastPlayFileFound) {
        } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_CONTEXT)) {
            if (pr->rignore <= 0) {
                pr_setContext(this, pr, lit->data);
            }
        } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_VOICE)) {
            if (pr->rignore <= 0) {
                pr_copyItem(this, pr_DynMem,lit,& lit2);
                if (pr->outOfMemory) return;
                pr_appendItem(this, & pr->routItemList,& pr->rlastOutItem, lit2);
            }
        } else {
            if ((pr->rignore <= 0) &&  !(pr->insidePhoneme && (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PLAY) ||
                                                               pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_IGNSIG)))) {
                PICODBG_INFO(("pp out(1): '%s'", lit->data));
                pr_copyItem(this, pr_DynMem,lit,& lit2);
                if (pr->outOfMemory) return;
                pr_appendItemToOutItemList(this, pr, & pr->routItemList,& pr->rlastOutItem,lit2);
                if (pr->outOfMemory) return;
            }
        }
    }
    for (li = 0; li<pr->rbestpath.rlen; li++) {
        with__0 = & pr->rbestpath.rele[li];
        pr_disposeItem(this, & pr->ritems[with__0->ritemid+1]);
    }
    pr_resetMemState(this, pr_WorkMem, lmemState);
}

/* *****************************************************************************/

static void pr_compare (picoos_uchar str1lc[], picoos_uchar str2[], picoos_int16 * result)
{

    picoos_int32 i;
    picoos_int32 j;
    picoos_int32 l;
    picoos_uint32 pos;
    picoos_bool finished1;
    picoos_bool finished2;
    picoos_bool done;
    picobase_utf8char utf8char;

    pos = 0;
    l = picobase_get_next_utf8char(str2, PR_MAX_DATA_LEN, & pos,utf8char);
    picobase_lowercase_utf8_str(utf8char,(picoos_char*)utf8char,PICOBASE_UTF8_MAXLEN+1, &done);
    l = picobase_det_utf8_length(utf8char[0]);
    j = 0;
    i = 0;
    while ((i < PR_MAX_DATA_LEN) && (str1lc[i] != 0) && (l > 0) && (j <= 3) && (str1lc[i] == utf8char[j])) {
        i++;
        j++;
        if (j >= l) {
            l = picobase_get_next_utf8char(str2, PR_MAX_DATA_LEN, & pos,utf8char);
            picobase_lowercase_utf8_str(utf8char,(picoos_char*)utf8char,PICOBASE_UTF8_MAXLEN+1, &done);
            l = picobase_det_utf8_length(utf8char[0]);
            j = 0;
        }
    }
    finished1 = (i >= PR_MAX_DATA_LEN) || (str1lc[i] == 0);
    finished2 = (j > 3) || (utf8char[j] == 0);
    if (finished1 && finished2) {
        *result = PR_EQUAL;
    } else if (finished1) {
        *result = PR_SMALLER;
    } else if (finished2) {
        *result = PR_LARGER;
    } else {
        if (str1lc[i] < utf8char[j]) {
            *result = PR_SMALLER;
        } else {
            *result = PR_LARGER;
        }
    }
}


static picoos_bool pr_hasToken (picokpr_TokSetWP * tswp, picokpr_TokSetNP * tsnp)
{
    return ((((  PR_TSE_MASK_SPACE | PR_TSE_MASK_DIGIT | PR_TSE_MASK_LETTER | PR_TSE_MASK_SEQ
               | PR_TSE_MASK_CHAR | PR_TSE_MASK_BEGIN | PR_TSE_MASK_END) & (*tsnp)) != 0) ||
            ((PR_TSE_MASK_LEX & (*tswp)) != 0));
}


static picoos_bool pr_getNextToken (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    register struct pr_PathEle * with__0;
    picoos_int32 len;
    picokpr_TokSetNP npset;

    len = pr->ractpath.rlen;
    with__0 = & pr->ractpath.rele[len - 1];
    npset = picokpr_getTokSetNP(with__0->rnetwork, with__0->rtok);
    if ((len > 0) && (pr->ractpath.rlen < PR_MAX_PATH_LEN) && ((PR_TSE_MASK_NEXT & npset) != 0)) {
        pr_initPathEle(& pr->ractpath.rele[len]);
        pr->ractpath.rele[len].rnetwork = with__0->rnetwork;
        pr->ractpath.rele[len].rtok = picokpr_getTokNextOfs(with__0->rnetwork, with__0->rtok);
        pr->ractpath.rele[len].rdepth = with__0->rdepth;
        pr->ractpath.rlen++;
        return TRUE;
    } else {
        if (len >= PR_MAX_PATH_LEN) {
          PICODBG_INFO(("max path len reached (pr_getNextToken)"));
        }
        return FALSE;
    }
}


static picoos_bool pr_getAltToken (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    register struct pr_PathEle * with__0;
    picokpr_TokArrOffset lTok;
    picokpr_TokSetNP npset;


    with__0 = & pr->ractpath.rele[pr->ractpath.rlen - 1];
    if ((pr->ractpath.rlen > 0) && (pr->ractpath.rlen < PR_MAX_PATH_LEN)) {
        npset = picokpr_getTokSetNP(with__0->rnetwork, with__0->rtok);
        if (with__0->rcompare == PR_SMALLER) {
            if ((PR_TSE_MASK_ALTL & npset) != 0) {
                lTok = picokpr_getTokAltLOfs(with__0->rnetwork, with__0->rtok);
            } else {
                return FALSE;
            }
        } else {
            if ((PR_TSE_MASK_ALTR & npset) != 0) {
                lTok = picokpr_getTokAltROfs(with__0->rnetwork, with__0->rtok);
            } else {
                return FALSE;
            }
        }
        with__0->rlState = PR_LSInit;
        with__0->rtok = lTok;
        with__0->ritemid =  -1;
        with__0->rcompare =  -1;
        with__0->rprodname = 0;
        with__0->rprodprefcost = 0;
        return TRUE;
    } else {
        if (pr->ractpath.rlen >= PR_MAX_PATH_LEN) {
          PICODBG_INFO(("max path len reached (pr_getAltToken)"));
        }
        return FALSE;
    }
}


static picoos_bool pr_findProduction (picodata_ProcessingUnit this, pr_subobj_t * pr,
                                      picoos_uchar str[], picokpr_Preproc * network, picokpr_TokArrOffset * tokOfs)
{
    picoos_bool found;
    picoos_int32 p;
    picoos_int32 ind;
    picoos_int32 i;
    picoos_bool done;
    picokpr_VarStrPtr lstrp;
    picoos_int32 lprodarrlen;

    ind = 0;
    pr_getTermPartStr(str,& ind,'.',pr->tmpStr1,& done);
    pr_getTermPartStr(str,& ind,'.',pr->tmpStr2,& done);
    found = FALSE;

    for (p=0; p<PR_MAX_NR_PREPROC; p++) {
        if (!found && (pr->preproc[p] != NULL)) {
            if (pr_strEqual(pr->tmpStr1, picokpr_getPreprocNetName(pr->preproc[p]))) {
                i = 0;
                lprodarrlen = picokpr_getProdArrLen(pr->preproc[p]);
                while (!found && (i <= (lprodarrlen - 1))) {
                    lstrp = picokpr_getVarStrPtr(pr->preproc[p],picokpr_getProdNameOfs(pr->preproc[p], i));
                    if (pr_strEqual(pr->tmpStr2, lstrp)) {
                        *network = pr->preproc[p];
                        *tokOfs = picokpr_getProdATokOfs(pr->preproc[p], i);
                        return TRUE;
                    }
                    i++;
                }
           }
        }
    }
    return FALSE;
}


static picoos_bool pr_getProdToken (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    register struct pr_PathEle * with__0;
    picokpr_VarStrPtr lstrp;
    picokpr_TokSetWP wpset;

    if ((pr->ractpath.rlen > 0) && (pr->ractpath.rlen < PR_MAX_PATH_LEN)) {
        with__0 = & pr->ractpath.rele[pr->ractpath.rlen - 1];
        wpset = picokpr_getTokSetWP(with__0->rnetwork, with__0->rtok);
        if ((PR_TSE_MASK_PROD & wpset) != 0) {
            if ((PR_TSE_MASK_PRODEXT & wpset) != 0) {
                pr_initPathEle(& pr->ractpath.rele[pr->ractpath.rlen]);
                lstrp = picokpr_getVarStrPtr(with__0->rnetwork, pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEProdExt));
                if (pr_findProduction(this, pr, lstrp,& pr->ractpath.rele[pr->ractpath.rlen].rnetwork,& pr->ractpath.rele[pr->ractpath.rlen].rtok)) {
                    with__0->rprodname = picokpr_getProdNameOfs(with__0->rnetwork, pr_attrVal(with__0->rnetwork, with__0->rtok,PR_TSEProd));
                    with__0->rprodprefcost = picokpr_getProdPrefCost(with__0->rnetwork, pr_attrVal(with__0->rnetwork,with__0->rtok,PR_TSEProd));
                    pr->ractpath.rele[pr->ractpath.rlen].rdepth = with__0->rdepth + 1;
                    pr->ractpath.rlen++;
                    return TRUE;
                } else {
                    return FALSE;
                }
            } else {
                pr_initPathEle(& pr->ractpath.rele[pr->ractpath.rlen]);
                pr->ractpath.rele[pr->ractpath.rlen].rnetwork = with__0->rnetwork;
                pr->ractpath.rele[pr->ractpath.rlen].rtok = picokpr_getProdATokOfs(with__0->rnetwork, pr_attrVal(with__0->rnetwork, with__0->rtok,PR_TSEProd));
                with__0->rprodname = picokpr_getProdNameOfs(with__0->rnetwork, pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEProd));
                with__0->rprodprefcost = picokpr_getProdPrefCost(with__0->rnetwork, pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEProd));
                pr->ractpath.rele[pr->ractpath.rlen].rdepth = with__0->rdepth + 1;
                pr->ractpath.rlen++;
                return TRUE;
            }
        }
    }
    if (pr->ractpath.rlen >= PR_MAX_PATH_LEN) {
        PICODBG_INFO(("max path len reached (pr_getProdToken)"));
    }
    return FALSE;
}


static picoos_bool pr_getProdContToken (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    picoos_int32 li;

    li = pr->ractpath.rlen - 1;
    while ((li > 0) &&  !((pr->ractpath.rele[li].rdepth == (pr->ractpath.rele[pr->ractpath.rlen - 1].rdepth - 1)) && ((PR_TSE_MASK_PROD &picokpr_getTokSetWP(pr->ractpath.rele[li].rnetwork, pr->ractpath.rele[li].rtok)) != 0))) {
        li--;
    }
    if (((li >= 0) && (pr->ractpath.rlen < PR_MAX_PATH_LEN) && (PR_TSE_MASK_NEXT &picokpr_getTokSetNP(pr->ractpath.rele[li].rnetwork, pr->ractpath.rele[li].rtok)) != 0)) {
        pr_initPathEle(& pr->ractpath.rele[pr->ractpath.rlen]);
        pr->ractpath.rele[pr->ractpath.rlen].rnetwork = pr->ractpath.rele[li].rnetwork;
        pr->ractpath.rele[pr->ractpath.rlen].rtok = picokpr_getTokNextOfs(pr->ractpath.rele[li].rnetwork, pr->ractpath.rele[li].rtok);
        pr->ractpath.rele[pr->ractpath.rlen].rdepth = pr->ractpath.rele[li].rdepth;
        pr->ractpath.rlen++;
        return TRUE;
    } else {
        if (pr->ractpath.rlen >= PR_MAX_PATH_LEN) {
            PICODBG_INFO(("max path len reached (pr_getProdContToken)"));
        }
        return FALSE;
    }
}

/* *****************************************************************************/

static picoos_bool pr_getTopLevelToken (picodata_ProcessingUnit this, pr_subobj_t * pr, picoos_bool firstprod)
{
    if (firstprod) {
        if (pr->actCtx != NULL) {
            pr->prodList = pr->actCtx->rProdList;
        } else {
            pr->prodList = NULL;
        }
    } else if (pr->prodList != NULL) {
        pr->prodList = pr->prodList->rNext;
    }
    if ((pr->prodList != NULL) && (pr->prodList->rProdOfs != 0) && (picokpr_getProdATokOfs(pr->prodList->rNetwork, pr->prodList->rProdOfs) != 0)) {
        pr_initPathEle(& pr->ractpath.rele[pr->ractpath.rlen]);
        pr->ractpath.rele[pr->ractpath.rlen].rdepth = 1;
        pr->ractpath.rele[pr->ractpath.rlen].rnetwork = pr->prodList->rNetwork;
        pr->ractpath.rele[pr->ractpath.rlen].rtok = picokpr_getProdATokOfs(pr->prodList->rNetwork, pr->prodList->rProdOfs);
        pr->ractpath.rele[pr->ractpath.rlen].rlState = PR_LSInit;
        pr->ractpath.rele[pr->ractpath.rlen].rcompare =  -1;
        pr->ractpath.rele[pr->ractpath.rlen].rprodname = picokpr_getProdNameOfs(pr->prodList->rNetwork, pr->prodList->rProdOfs);
        pr->ractpath.rele[pr->ractpath.rlen].rprodprefcost = picokpr_getProdPrefCost(pr->prodList->rNetwork, pr->prodList->rProdOfs);
        pr->ractpath.rlen++;
        return TRUE;
    } else {
        return FALSE;
    }
}


static picoos_bool pr_getToken (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    picoos_int32 ln;
    picoos_int32 lid;

    ln = (pr->ractpath.rlen - 2);
    while ((ln >= 0) && (pr->ractpath.rele[ln].ritemid ==  -1)) {
        ln = ln - 1;
    }
    if (ln >= 0) {
        lid = pr->ractpath.rele[ln].ritemid + 1;
    } else {
        lid = 0;
    }
    if (lid < pr->rnritems) {
        pr->ractpath.rele[pr->ractpath.rlen - 1].ritemid = lid;
    } else {
        pr->ractpath.rele[pr->ractpath.rlen - 1].ritemid =  -1;
    }
    return (lid < pr->rnritems);
}


static picoos_bool pr_getNextMultiToken (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    picoos_int32 len;

    len = pr->ractpath.rlen;
    if ((len > 0) && (len < PR_MAX_PATH_LEN)) {
        pr->ractpath.rele[len].rtok = pr->ractpath.rele[len - 1].rtok;
        pr->ractpath.rele[len].ritemid =  -(1);
        pr->ractpath.rele[len].rcompare = pr->ractpath.rele[len - 1].rcompare;
        pr->ractpath.rele[len].rdepth = pr->ractpath.rele[len - 1].rdepth;
        pr->ractpath.rele[len].rlState = PR_LSInit;
        pr->ractpath.rlen++;
        return TRUE;
    } else {
        if (len >= PR_MAX_PATH_LEN) {
            PICODBG_INFO(("max path len reached (pr_getNextMultiToken)"));
        }
        return FALSE;
    }
    return FALSE;
}


static pr_MatchState pr_matchMultiToken (picodata_ProcessingUnit this, pr_subobj_t * pr,
                                         picokpr_TokSetNP npset, picokpr_TokSetWP wpset)
{
    picoos_bool lcontinue=FALSE;
    picoos_bool lmatch=FALSE;

    if (lmatch) {
        return PR_MSMatchedMulti;
    } else if (lcontinue) {
        return PR_MSMatchedContinue;
    } else {
        return PR_MSNotMatched;
    }
    pr = pr;        /* avoid warning "var not used in this function"*/
    npset = npset;    /* avoid warning "var not used in this function"*/
    wpset = wpset;    /* avoid warning "var not used in this function"*/

}


static pr_MatchState pr_matchTokensSpace (picodata_ProcessingUnit this, pr_subobj_t * pr, picoos_int32 cmpres,
                                          picokpr_TokSetNP npset, picokpr_TokSetWP wpset)
{
    register struct pr_PathEle * with__0;
    picoos_int32 llen;
    picoos_int32 lulen;
    picoos_int32 li;
    picokpr_VarStrPtr lstrp;
    picoos_int32 leol;

    with__0 = & pr->ractpath.rele[pr->ractpath.rlen - 1];
    if ((PR_TSE_MASK_SPACE & npset) == 0) {
        return PR_MSNotMatched;
    }
    lstrp = (picokpr_VarStrPtr)&pr->ritems[with__0->ritemid+1]->data;
    lulen = picobase_utf8_length(lstrp,PR_MAX_DATA_LEN);
    if (((PR_TSE_MASK_LEN & wpset) != 0) && (lulen != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSELen))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_MIN & wpset) != 0) && (lulen < pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMin))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_MAX & wpset) != 0) && (lulen > pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMax))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_STR & wpset) != 0) && (cmpres != PR_EQUAL)) {
        return PR_MSNotMatched;
    }
    if ((PR_TSE_MASK_VAL & wpset) != 0) {
        leol = 0;
        llen = pr_strlen(lstrp);
        for (li = 0; li < llen; li++) {
            if (lstrp[li] == PR_EOL) {
                leol++;
            }
        }
        if (leol != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEVal)) {
            return PR_MSNotMatched;
        }
    }
    if (((PR_TSE_MASK_ID & wpset) != 0) && (pr->ritems[with__0->ritemid+1]->head.info2 != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEID))) {
        return PR_MSNotMatched;
    }
    return PR_MSMatched;
}


static pr_MatchState pr_matchTokensDigit (picodata_ProcessingUnit this, pr_subobj_t * pr, picoos_int32 cmpres,
                                          picokpr_TokSetNP npset, picokpr_TokSetWP wpset)
{
    register struct pr_PathEle * with__0;
    picoos_int32 lulen;
    picoos_int32 lval;
    picokpr_VarStrPtr lstrp;

    with__0 = & pr->ractpath.rele[pr->ractpath.rlen - 1];
    if ((PR_TSE_MASK_DIGIT & npset) == 0) {
        return PR_MSNotMatched;
    }
    lstrp = (picokpr_VarStrPtr)&pr->ritems[with__0->ritemid+1]->data;
    lulen = picobase_utf8_length(lstrp,PR_MAX_DATA_LEN);
    if ((((PR_TSE_MASK_LEN & wpset) != 0) && (lulen != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSELen)))) {
        return PR_MSNotMatched;
    }
    lval = pr->ritems[with__0->ritemid+1]->val;
    if (((PR_TSE_MASK_MIN & wpset) != 0) && (lval < pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMin))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_MAX & wpset) != 0) && (lval > pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMax))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_STR & wpset) != 0) && (cmpres != PR_EQUAL)) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_VAL & wpset) != 0) && (lval != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEVal))) {
        return PR_MSNotMatched;
    }
    if ((((PR_TSE_MASK_NLZ & npset) != 0) && lstrp[0] == '0')) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_HEAD & wpset) != 0) &&  !(picokpr_isEqualHead(with__0->rnetwork,lstrp,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEHead)))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_MID & wpset) != 0) &&  !(picokpr_isEqualMid(with__0->rnetwork,lstrp,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMid)))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_TAIL & wpset) != 0) &&  !(picokpr_isEqualTail(with__0->rnetwork,lstrp,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSETail)))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_ID & wpset) != 0) && (pr->ritems[with__0->ritemid+1]->head.info2 != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEID))) {
        return PR_MSNotMatched;
    }
    return PR_MSMatched;
}


static pr_MatchState pr_matchTokensSeq (picodata_ProcessingUnit this, pr_subobj_t * pr, picoos_int32 cmpres,
                                        picokpr_TokSetNP npset, picokpr_TokSetWP wpset)
{

    register struct pr_PathEle * with__0;
    picoos_int32 lulen;
    picokpr_VarStrPtr lstrp;

    with__0 = & pr->ractpath.rele[pr->ractpath.rlen - 1];

    if (!((PR_TSE_MASK_SEQ & npset) != 0)) {
        return PR_MSNotMatched;
    }
    lstrp = (picokpr_VarStrPtr)(void *) &pr->ritems[with__0->ritemid+1]->data;
    lulen = picobase_utf8_length(lstrp,PR_MAX_DATA_LEN);
    if (((PR_TSE_MASK_LEN & wpset) != 0) && (lulen != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSELen))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_MIN & wpset) != 0) && (lulen < pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMin))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_MAX & wpset) != 0) && (lulen > pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMax))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_STR & wpset) != 0) && (cmpres != PR_EQUAL)) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_HEAD & wpset) != 0) &&  !(picokpr_isEqualHead(with__0->rnetwork,lstrp,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEHead)))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_MID & wpset) != 0) &&  !(picokpr_isEqualMid(with__0->rnetwork,lstrp,PR_MAX_DATA_LEN ,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMid)))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_TAIL & wpset) != 0) &&  !(picokpr_isEqualTail(with__0->rnetwork,lstrp,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSETail)))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_ID & wpset) != 0) && (pr->ritems[with__0->ritemid+1]->head.info2 != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEID))) {
        return PR_MSNotMatched;
    }
    return PR_MSMatched;
}


static pr_MatchState pr_matchTokensChar (picodata_ProcessingUnit this, pr_subobj_t * pr, picoos_int32 cmpres,
                                         picokpr_TokSetNP npset, picokpr_TokSetWP wpset)
{
    register struct pr_PathEle * with__0;

    with__0 = & pr->ractpath.rele[pr->ractpath.rlen - 1];

    if (!((PR_TSE_MASK_CHAR & npset) != 0)) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_STR & wpset) != 0) && (cmpres != PR_EQUAL)) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_ID & wpset) != 0) && (pr->ritems[with__0->ritemid+1]->head.info2 != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEID))) {
        return PR_MSNotMatched;
    }
    return PR_MSMatched;
}


static pr_MatchState pr_matchTokensLetter (picodata_ProcessingUnit this, pr_subobj_t * pr, picoos_int32 cmpres,
                                           picokpr_TokSetNP npset, picokpr_TokSetWP wpset)
{

    register struct pr_PathEle * with__0;
    picoos_int32 lulen;
    picoos_int32 lromanval;

    with__0 = & pr->ractpath.rele[pr->ractpath.rlen - 1];

    if ( !((PR_TSE_MASK_LETTER & npset) != 0)) {
        return PR_MSNotMatched;
    }
    lulen = picobase_utf8_length(pr->ritems[with__0->ritemid+1]->data, PR_MAX_DATA_LEN);
    if (((PR_TSE_MASK_LEN & wpset) != 0) && (lulen != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSELen))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_MIN & wpset) != 0) && (lulen < pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMin))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_MAX & wpset) != 0) && (lulen > pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMax))) {
        return PR_MSNotMatched;
    }
    if ((PR_TSE_MASK_CI & npset) != 0) {
        if (((PR_TSE_MASK_STR & wpset) != 0) && (cmpres != PR_EQUAL)) {
            return PR_MSNotMatched;
        }
        if (((PR_TSE_MASK_HEAD & wpset) != 0) &&  !(picokpr_isEqualHead(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->strci,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEHead)))) {
            return PR_MSNotMatched;
        }
        if (((PR_TSE_MASK_MID & wpset) != 0) &&  !(picokpr_isEqualMid(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->strci,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMid)))) {
            return PR_MSNotMatched;
        }
        if (((PR_TSE_MASK_TAIL & wpset) != 0) &&  !(picokpr_isEqualTail(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->strci,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSETail)))) {
            return PR_MSNotMatched;
        }
    } else if ((PR_TSE_MASK_CIS & npset) != 0) {
        if (((PR_TSE_MASK_STR & wpset) != 0) &&  !(picokpr_isEqual(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->strcis,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEStr)))) {
            return PR_MSNotMatched;
        }
        if (((PR_TSE_MASK_HEAD & wpset) != 0) &&  !(picokpr_isEqualHead(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->strcis,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEHead)))) {
            return PR_MSNotMatched;
        }
        if (((PR_TSE_MASK_MID & wpset) != 0) &&  !(picokpr_isEqualMid(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->strcis,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMid)))) {
            return PR_MSNotMatched;
        }
        if (((PR_TSE_MASK_TAIL & wpset) != 0) &&  !(picokpr_isEqualTail(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->strcis,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSETail)))) {
            return PR_MSNotMatched;
        }
    } else {
        if (((PR_TSE_MASK_STR & wpset) != 0) &&  !(picokpr_isEqual(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->data,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEStr)))) {
            return PR_MSNotMatched;
        }
        if (((PR_TSE_MASK_HEAD & wpset) != 0) &&  !(picokpr_isEqualHead(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->data,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEHead)))) {
            return PR_MSNotMatched;
        }
        if (((PR_TSE_MASK_MID & wpset) != 0) &&  !(picokpr_isEqualMid(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->data,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEMid)))) {
            return PR_MSNotMatched;
        }
        if (((PR_TSE_MASK_TAIL & wpset) != 0) &&  !(picokpr_isEqualTail(with__0->rnetwork,pr->ritems[with__0->ritemid+1]->data,PR_MAX_DATA_LEN,pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSETail)))) {
            return PR_MSNotMatched;
        }
    }
    if (((PR_TSE_MASK_AUC & npset) != 0) &&  !(pr->ritems[with__0->ritemid+1]->auc)) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_ALC & npset) != 0) &&  !(pr->ritems[with__0->ritemid+1]->alc)) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_SUC & npset) != 0) &&  !(pr->ritems[with__0->ritemid+1]->suc)) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_ROMAN & npset) != 0) &&  !(pr_isLatinNumber(pr->ritems[with__0->ritemid+1]->data,& lromanval))) {
        return PR_MSNotMatched;
    }
    if (((PR_TSE_MASK_ID & wpset) != 0) && (pr->ritems[with__0->ritemid+1]->head.info2 != pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEID))) {
        return PR_MSNotMatched;
    }
    return PR_MSMatched;
}


static pr_MatchState pr_matchTokensBegin (picodata_ProcessingUnit this, pr_subobj_t * pr,
                                          picokpr_TokSetNP npset, picokpr_TokSetWP wpset)
{
    npset = npset;        /* avoid warning "var not used in this function"*/
    wpset = wpset;        /* avoid warning "var not used in this function"*/
    if ((PR_TSE_MASK_BEGIN &picokpr_getTokSetNP(pr->ractpath.rele[pr->ractpath.rlen - 1].rnetwork, pr->ractpath.rele[pr->ractpath.rlen - 1].rtok)) != 0) {
        return PR_MSMatched;
    } else {
        return PR_MSNotMatched;
    }
}



static pr_MatchState pr_matchTokensEnd (picodata_ProcessingUnit this, pr_subobj_t * pr,
                                        picokpr_TokSetNP npset, picokpr_TokSetWP wpset)
{
    npset = npset;        /* avoid warning "var not used in this function"*/
    wpset = wpset;        /* avoid warning "var not used in this function"*/
    if ((PR_TSE_MASK_END &picokpr_getTokSetNP(pr->ractpath.rele[pr->ractpath.rlen - 1].rnetwork, pr->ractpath.rele[pr->ractpath.rlen - 1].rtok)) != 0) {
        return PR_MSMatched;
    } else {
        return PR_MSNotMatched;
    }
}


static pr_MatchState pr_matchTokens (picodata_ProcessingUnit this, pr_subobj_t * pr, picoos_int16 * cmpres)
{

    register struct pr_PathEle * with__0;
    picokpr_VarStrPtr lstrp;
    picokpr_TokSetNP npset;
    picokpr_TokSetWP wpset;

    with__0 = & pr->ractpath.rele[pr->ractpath.rlen - 1];
    npset = picokpr_getTokSetNP(with__0->rnetwork, with__0->rtok);
    wpset = picokpr_getTokSetWP(with__0->rnetwork, with__0->rtok);

    *cmpres = PR_EQUAL;
    if ((PR_TSE_MASK_STR & wpset) != 0) {
        lstrp = picokpr_getVarStrPtr(with__0->rnetwork, pr_attrVal(with__0->rnetwork, with__0->rtok, PR_TSEStr));
        pr_compare(pr->ritems[with__0->ritemid+1]->strci,lstrp,cmpres);
    }
    if (((PR_TSE_MASK_LEX & wpset) == PR_TSE_MASK_LEX) && ((PR_TSE_MASK_LETTER & npset) == 0)) {
        return pr_matchMultiToken(this, pr, npset, wpset);
    } else {
        switch (pr->ritems[with__0->ritemid+1]->head.info1) {
            case PICODATA_ITEMINFO1_TOKTYPE_BEGIN:
                return pr_matchTokensBegin(this, pr, npset, wpset);
                break;
            case PICODATA_ITEMINFO1_TOKTYPE_END:
                return pr_matchTokensEnd(this, pr, npset, wpset);
                break;
            case PICODATA_ITEMINFO1_TOKTYPE_SPACE:
                return pr_matchTokensSpace(this, pr, *cmpres, npset, wpset);
                break;
            case PICODATA_ITEMINFO1_TOKTYPE_DIGIT:
                return pr_matchTokensDigit(this, pr, *cmpres, npset, wpset);
                break;
            case PICODATA_ITEMINFO1_TOKTYPE_LETTER:
                return pr_matchTokensLetter(this, pr, *cmpres, npset, wpset);
                break;
            case PICODATA_ITEMINFO1_TOKTYPE_SEQ:
                return pr_matchTokensSeq(this, pr, *cmpres, npset, wpset);
                break;
            case PICODATA_ITEMINFO1_TOKTYPE_CHAR:
                return pr_matchTokensChar(this, pr, *cmpres, npset, wpset);
                break;
        default:
            PICODBG_INFO(("pr_matchTokens: unknown token type"));
            return PR_MSNotMatched;
            break;
        }
    }
}


static void pr_calcPathCost (struct pr_Path * path)
{
    picoos_int32 li;
    picoos_bool lfirst;
    picokpr_TokSetWP wpset;
    picokpr_TokSetNP npset;
#if PR_TRACE_PATHCOST
    picoos_uchar str[1000];
    picoos_uchar * strp;
#endif

#if PR_TRACE_PATHCOST
    str[0] = 0;
#endif

    lfirst = TRUE;
    path->rcost = PR_COST_INIT;
    for (li = 0; li < path->rlen; li++) {
        if (li == 0) {
            path->rcost = path->rcost + path->rele[li].rprodprefcost;
        }
        wpset = picokpr_getTokSetWP(path->rele[li].rnetwork, path->rele[li].rtok);
        npset = picokpr_getTokSetNP(path->rele[li].rnetwork, path->rele[li].rtok);
        if ((PR_TSE_MASK_COST & wpset) != 0) {
            if (((PR_TSE_MASK_LEX & wpset) == PR_TSE_MASK_LEX) && ((PR_TSE_MASK_LETTER & npset) == 0)) {
                if (lfirst) {
                    path->rcost = path->rcost - PR_COST + pr_attrVal(path->rele[li].rnetwork, path->rele[li].rtok, PR_TSECost);
                } else {
                    path->rcost = path->rcost - PR_COST;
                }
                lfirst = FALSE;
            } else {
                path->rcost = path->rcost - PR_COST + pr_attrVal(path->rele[li].rnetwork, path->rele[li].rtok, PR_TSECost);
                lfirst = TRUE;
            }
        } else if (pr_hasToken(& wpset,& npset)) {
            path->rcost = path->rcost - PR_COST;
        }
#if PR_TRACE_PATHCOST
        if ((path->rele[li].rprodname != 0)) {
            strp = picokpr_getVarStrPtr(path->rele[li].rnetwork, path->rele[li].rprodname);
            picoos_strcat(str, (picoos_char *)" ");
            picoos_strcat(str, strp);
        }
#endif
    }
#if PR_TRACE_PATHCOST
    PICODBG_INFO(("pp cost: %i %s", path->rcost, str));
#endif
}


void pr_processToken (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    register struct pr_PathEle * with__0;
    picoos_bool ldummy;
    picoos_int32 li;
    picokpr_TokSetNP npset;
    picokpr_TokSetWP wpset;

    do {
        pr->rgState = PR_GSContinue;
        if ((pr->ractpath.rlen == 0)) {
            if (pr_getTopLevelToken(this, pr, FALSE)) {
                pr->rgState = PR_GSContinue;
            } else if (pr->rbestpath.rlen == 0) {
                pr->rgState = PR_GSNotFound;
            } else {
                pr->rgState = PR_GSFound;
            }
        } else {
            if (pr->maxPathLen < pr->ractpath.rlen) {
                pr->maxPathLen = pr->ractpath.rlen;
            }
            with__0 = & pr->ractpath.rele[pr->ractpath.rlen - 1];
            switch (with__0->rlState) {
                case PR_LSInit:
                    npset = picokpr_getTokSetNP(with__0->rnetwork, with__0->rtok);
                    wpset = picokpr_getTokSetWP(with__0->rnetwork, with__0->rtok);
                    if ((PR_TSE_MASK_ACCEPT & npset) != 0){
                        if (with__0->rdepth == 1) {
                            pr_calcPathCost(&pr->ractpath);
                            if ((pr->rbestpath.rlen == 0) || (pr->ractpath.rcost < pr->rbestpath.rcost)) {
                                pr->rbestpath.rlen = pr->ractpath.rlen;
                                pr->rbestpath.rcost = pr->ractpath.rcost;
                                for (li = 0; li < pr->ractpath.rlen; li++) {
                                    pr->rbestpath.rele[li] = pr->ractpath.rele[li];
                                }
                            }
                            with__0->rlState = PR_LSGetNextToken;
                        } else {
                            with__0->rlState = PR_LSGetProdContToken;
                        }
                    } else if ((PR_TSE_MASK_PROD & wpset) != 0) {
                        with__0->rlState = PR_LSGetProdToken;
                    } else if ((PR_TSE_MASK_OUT & wpset) != 0) {
                        with__0->rlState = PR_LSGetNextToken;
                    } else if (pr_hasToken(& wpset,& npset)) {
                        with__0->rlState = PR_LSGetToken;
                    } else {
                        with__0->rlState = PR_LSGetNextToken;
                    }
                    break;
                case PR_LSGetProdToken:
                    with__0->rlState = PR_LSGetAltToken;
                    ldummy = pr_getProdToken(this, pr);
                    break;
                case PR_LSGetProdContToken:
                    with__0->rlState = PR_LSGetAltToken;
                    ldummy = pr_getProdContToken(this, pr);
                    break;
                case PR_LSGoBack:
                    pr->ractpath.rlen--;
                    break;
                case PR_LSGetToken:
                    if (pr_getToken(this, pr)) {
                        with__0->rlState = PR_LSMatch;
                    } else if (pr->forceOutput) {
                        with__0->rlState = PR_LSGetAltToken;
                    } else {
                        with__0->rlState = PR_LSGetToken2;
                        pr->rgState = PR_GSNeedToken;
                    }
                    break;
                case PR_LSGetToken2:
                    if (pr_getToken(this, pr)) {
                        with__0->rlState = PR_LSMatch;
                    } else {
                        with__0->rlState = PR_LSGoBack;
                    }
                    break;
                case PR_LSMatch:
                    switch (pr_matchTokens(this, pr, & with__0->rcompare)) {
                        case PR_MSMatched:
                            with__0->rlState = PR_LSGetNextToken;
                            break;
                        case PR_MSMatchedContinue:
                            with__0->rlState = PR_LSGetAltToken;
                            ldummy = pr_getNextMultiToken(this, pr);
                            break;
                        case PR_MSMatchedMulti:
                            with__0->rlState = PR_LSGetNextToken;
                            ldummy = pr_getNextMultiToken(this, pr);
                            break;
                    default:
                        with__0->rlState = PR_LSGetAltToken;
                        break;
                    }
                    break;
                case PR_LSGetNextToken:
                    with__0->rlState = PR_LSGetAltToken;
                    ldummy = pr_getNextToken(this, pr);
                    break;
                case PR_LSGetAltToken:
                    with__0->rlState = PR_LSGoBack;
                    ldummy = pr_getAltToken(this, pr);
                    break;
            default:
                PICODBG_INFO(("unhandled local state"));
                break;
            }
        }
        pr->nrIterations--;
    } while ((pr->rgState == PR_GSContinue) && (pr->nrIterations > 0));
}


void pr_process (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    switch (pr->rgState) {
        case PR_GS_START:
        case PR_GSFound:
        case PR_GSNotFound:
            pr->ractpath.rlen = 0;
            pr->ractpath.rcost = PR_COST_INIT;
            pr->rbestpath.rlen = 0;
            pr->rbestpath.rcost = PR_COST_INIT;
            if (pr_getTopLevelToken(this, pr, TRUE)) {
                pr->rgState = PR_GSContinue;
            } else {
                pr->rgState = PR_GSNotFound;
            }
            break;
        case PR_GSContinue:
            pr_processToken(this, pr);
            break;
        case PR_GSNeedToken:
            pr->rgState = PR_GSContinue;
            break;
    default:
        pr->rgState = PR_GS_START;
        break;
    }
}


static void pr_prepareItem (picodata_ProcessingUnit this, pr_subobj_t * pr, pr_ioItemPtr item)
{
    pr->ritems[pr->rnritems + 1] = item;
    pr->rnritems++;
}


static void pr_processItems (picodata_ProcessingUnit this, pr_subobj_t * pr)
{
    pr_ioItemPtr lit;
    pr_MemState lmemState;

    pr_getMemState(this, pr_WorkMem,& lmemState);

    while ((pr->rinItemList != NULL) && (pr->rinItemList->head.type != PICODATA_ITEM_TOKEN)) {
        lit = pr->rinItemList;
        PICODBG_INFO(("pp in (0)"));
        PICODBG_INFO(("pp out(0)"));
        pr->rinItemList = pr->rinItemList->next;
        lit->next = NULL;
        if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PHONEME) && pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_START)) {
            pr->insidePhoneme = TRUE;
        } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PHONEME) && pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_END)) {
            pr->insidePhoneme = FALSE;
        }
        if (pr->insidePhoneme && (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PLAY) || pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_IGNSIG))) {
            pr_disposeItem(this, & lit);
        } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_CONTEXT)) {
            pr_setContext(this, pr, lit->data);
            pr_disposeItem(this, & lit);
        } else if (pr->rignore <= 0) {
            pr_appendItemToOutItemList(this, pr, & pr->routItemList,& pr->rlastOutItem,lit);
            if (pr->outOfMemory) return;
        } else {
            pr_disposeItem(this, & lit);
        }
        pr->rgState = PR_GS_START;
    }
    if (pr->rinItemList != NULL) {
        pr_process(this, pr);
        if (pr->rgState == PR_GSNotFound) {
            lit = pr->rinItemList;
            pr->rinItemList = pr->rinItemList->next;
            lit->next = NULL;
            PICODBG_INFO(("pp in (2): '%s'", lit->data));
            if (pr->rignore <= 0) {
                PICODBG_INFO(("pp out(2): '%s'", lit->data));
            }

            if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PHONEME) && pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_START)) {
                pr->insidePhoneme = TRUE;
            } else if (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PHONEME) && pr_isCmdInfo2(lit, PICODATA_ITEMINFO2_CMD_END)) {
                pr->insidePhoneme = FALSE;
            }
            if (((pr->rignore <= 0) &&  !((pr->insidePhoneme && (pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_PLAY) || pr_isCmdType(lit,PICODATA_ITEMINFO1_CMD_IGNSIG)))))) {
                pr_appendItemToOutItemList(this, pr, & pr->routItemList,& pr->rlastOutItem,lit);
                if (pr->outOfMemory) return;
            } else {
                pr_disposeItem(this, & lit);
            }
            pr->rgState = PR_GS_START;
            pr->rnritems = 0;
        } else if (pr->rgState == PR_GSFound) {
            pr_outputPath(this, pr);
            if (pr->outOfMemory) return;
            pr->rgState = PR_GS_START;
            pr->rnritems = 0;
        }
    }
    if (pr->rinItemList == NULL) {
        pr->rlastInItem = NULL;
    } else if (pr->rnritems == 0) {
        lit = pr->rinItemList;
        while (lit != NULL) {
            if (lit->head.type == PICODATA_ITEM_TOKEN) {
                pr_prepareItem(this, pr, lit);
            }
            lit = lit->next;
        }
    }
    pr_resetMemState(this, pr_WorkMem,lmemState);
}



extern void pr_treatItem (picodata_ProcessingUnit this, pr_subobj_t * pr, pr_ioItemPtr item)
{
    pr_ioItemPtr lit;

    pr_startItemList(& pr->routItemList,& pr->rlastOutItem);

    if (!PR_ENABLED || (pr->rgState == PR_GSNoPreproc)) {
        /* preprocessing disabled or no preproc networks available:
           append items directly to output item list */
        PICODBG_INFO(("pp in (3): '%s'", item->data));
        PICODBG_INFO(("pp out(3): '%s'", item->data));
        pr_appendItemToOutItemList(this, pr, & pr->routItemList,& pr->rlastOutItem,item);
    } else {

        if (pr->actCtxChanged) {
            pr->rgState = PR_GS_START;
            pr->ractpath.rcost = PR_COST_INIT;
            pr->ractpath.rlen = 0;
            pr->rbestpath.rcost = PR_COST_INIT;
            pr->rbestpath.rlen = 0;
            pr->prodList = NULL;
            pr->rnritems = 0;
            pr->actCtxChanged = FALSE;
        }
        if (pr_isCmdType(item , PICODATA_ITEMINFO1_CMD_CONTEXT) || pr_isCmdType(item, PICODATA_ITEMINFO1_CMD_FLUSH)) {
            /* context switch or flush: force processing and empty input item list */
            pr->forceOutput = TRUE;
        }
        pr_appendItem(this, & pr->rinItemList,& pr->rlastInItem, item);
        if (pr->rnritems == 0) {
            lit = pr->rinItemList;
            while (lit != NULL) {
                if (lit->head.type == PICODATA_ITEM_TOKEN) {
                    pr_prepareItem(this, pr, lit);
                }
                lit = lit->next;
            }
        } else if (item->head.type == PICODATA_ITEM_TOKEN) {
            pr_prepareItem(this, pr, item);
        }
    }
}

/* *****************************************************************************/
/* *****************************************************************************/
/* *****************************************************************************/


pico_status_t prReset(register picodata_ProcessingUnit this, picoos_int32 resetMode)
{

    picoos_int32 i;
    pr_subobj_t * pr;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pr = (pr_subobj_t *) this->subObj;

    pr->rinItemList = NULL;
    pr->rlastInItem = NULL;
    pr->routItemList = NULL;
    pr->rlastOutItem = NULL;
    pr->ractpath.rcost = PR_COST_INIT;
    pr->ractpath.rlen = 0;
    pr->rbestpath.rcost = PR_COST_INIT;
    pr->rbestpath.rlen = 0;
    pr->rnritems = 0;
    pr->ritems[0] = NULL;
    pr->rignore = 0;
    pr->spellMode = 0;
    pr->maxPathLen = 0;
    pr->insidePhoneme = FALSE;
    pr->saveFile[0] = 0;

    pr->outReadPos = 0;
    pr->outWritePos = 0;
    pr->inBufLen = 0;

    pr->rgState = PR_GSNoPreproc;
    for (i=0; i<PR_MAX_NR_PREPROC; i++) {
        if (pr->preproc[i] != NULL) {
            pr->rgState = PR_GS_START;
        }
    }
    pr->actCtx = pr_findContext(pr->ctxList, (picoos_uchar*)PICO_CONTEXT_DEFAULT);
    pr->actCtxChanged = FALSE;
    pr->prodList = NULL;

    if (((uintptr_t)pr->pr_WorkMem % PICOOS_ALIGN_SIZE) == 0) {
        pr->workMemTop = 0;
    }
    else {
        pr->workMemTop = PICOOS_ALIGN_SIZE - ((uintptr_t)pr->pr_WorkMem % PICOOS_ALIGN_SIZE);
    }
    pr->maxWorkMemTop=0;
    pr->dynMemSize=0;
    pr->maxDynMemSize=0;
    /* this is ok to be in 'initialize' because it is a private memory within pr. Creating a new mm
     * here amounts to resetting this internal memory
     */
    pr->dynMemMM = picoos_newMemoryManager((void *)pr->pr_DynMem, PR_DYN_MEM_SIZE,
            /*enableMemProt*/ FALSE);
    pr->outOfMemory = FALSE;

    pr->forceOutput = FALSE;

    if (resetMode == PICO_RESET_SOFT) {
        /*following initializations needed only at startup or after a full reset*/
        return PICO_OK;
    }

    pr->xsampa_parser = picokfst_getFST(this->voice->kbArray[PICOKNOW_KBID_FST_XSAMPA_PARSE]);

    pr->svoxpa_parser = picokfst_getFST(this->voice->kbArray[PICOKNOW_KBID_FST_SVOXPA_PARSE]);

    pr->xsampa2svoxpa_mapper = picokfst_getFST(this->voice->kbArray[PICOKNOW_KBID_FST_XSAMPA2SVOXPA]);



    return PICO_OK;
}


pico_status_t prInitialize(register picodata_ProcessingUnit this, picoos_int32 resetMode)
{
/*
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
*/
    return prReset(this, resetMode);
}


pico_status_t prTerminate(register picodata_ProcessingUnit this)
{
    return PICO_OK;
}

picodata_step_result_t prStep(register picodata_ProcessingUnit this, picoos_int16 mode, picoos_uint16 * numBytesOutput);

pico_status_t prSubObjDeallocate(register picodata_ProcessingUnit this,
        picoos_MemoryManager mm)
{
    pr_subobj_t * pr;

    if (NULL != this) {
        pr = (pr_subobj_t *) this->subObj;
        mm = mm;        /* avoid warning "var not used in this function"*/
        PICODBG_INFO(("max pr_WorkMem: %i of %i", pr->maxWorkMemTop, PR_WORK_MEM_SIZE));
        PICODBG_INFO(("max pr_DynMem: %i of %i", pr->maxDynMemSize, PR_DYN_MEM_SIZE));

        pr_disposeContextList(this);
        picoos_deallocate(this->common->mm, (void *) &this->subObj);
    }
    return PICO_OK;
}

picodata_ProcessingUnit picopr_newPreprocUnit(picoos_MemoryManager mm, picoos_Common common,
        picodata_CharBuffer cbIn, picodata_CharBuffer cbOut,
        picorsrc_Voice voice)
{
    picoos_int32 i;
    pr_subobj_t * pr;


    picodata_ProcessingUnit this = picodata_newProcessingUnit(mm, common, cbIn, cbOut, voice);
    if (this == NULL) {
        return NULL;
    }

    this->initialize = prInitialize;
    PICODBG_DEBUG(("set this->step to prStep"));
    this->step = prStep;
    this->terminate = prTerminate;
    this->subDeallocate = prSubObjDeallocate;
    this->subObj = picoos_allocate(mm, sizeof(pr_subobj_t));
#if PR_TRACE_MEM || PR_TRACE_MAX_MEM
    PICODBG_INFO(("preproc alloc: %i", sizeof(pr_subobj_t)));
    PICODBG_INFO(("max dyn size: %i", PR_MAX_PATH_LEN*((((PR_IOITEM_MIN_SIZE+2) + PICOOS_ALIGN_SIZE - 1) / PICOOS_ALIGN_SIZE) * PICOOS_ALIGN_SIZE + 16)));
#endif
    if (this->subObj == NULL) {
        picoos_deallocate(mm, (void *)&this);
        return NULL;
    }
    pr = (pr_subobj_t *) this->subObj;

    pr->graphs = picoktab_getGraphs(this->voice->kbArray[PICOKNOW_KBID_TAB_GRAPHS]);
    pr->preproc[0] = picokpr_getPreproc(this->voice->kbArray[PICOKNOW_KBID_TPP_MAIN]);
    for (i=0; i<PICOKNOW_MAX_NUM_UTPP; i++) {
      pr->preproc[1+i] = picokpr_getPreproc(this->voice->kbArray[PICOKNOW_KBID_TPP_USER_1+i]);
    }

   if (pr_createContextList(this) != PICO_OK) {
        pr_disposeContextList(this);
        picoos_deallocate(mm, (void *)&this);
        return NULL;
    }
    prInitialize(this, PICO_RESET_FULL);
    return this;
}

/**
 * fill up internal buffer
 */
picodata_step_result_t prStep(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * numBytesOutput)
{
    register pr_subobj_t * pr;
    pr_ioItemPtr it;
    picoos_int32 len, i;
    pico_status_t rv;
    picoos_int32 id;
    picoos_uint8 info1;
    picoos_uint8 info2;
    picoos_int32 nrUtfChars;
    picoos_uint32 pos;
    picobase_utf8char inUtf8char, outUtf8char;
    picoos_int32 inUtf8charlen, outUtf8charlen;
    picoos_int32 lenpos;
    picoos_bool ldone;
    picoos_bool split;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pr = (pr_subobj_t *) this->subObj;

    if (pr->outOfMemory) return PICODATA_PU_ERROR;

    mode = mode;        /* avoid warning "var not used in this function"*/
    pr->nrIterations = PR_MAX_NR_ITERATIONS;

    *numBytesOutput = 0;
    while (1) { /* exit via return */
        if ((pr->outWritePos - pr->outReadPos) > 0) {
            /* deliver the data in the output buffer */
            if (picodata_cbPutItem(this->cbOut, &pr->outBuf[pr->outReadPos], pr->outWritePos - pr->outReadPos, numBytesOutput) == PICO_OK) {
                pr->outReadPos += *numBytesOutput;
                if (pr->outWritePos == pr->outReadPos) {
                    pr->outWritePos = 0;
                    pr->outReadPos = 0;
                }
            }
            else {
                return PICODATA_PU_OUT_FULL;
            }
        }
        else if (pr->routItemList != NULL) {
            /* there are item(s) in the output item list, move them to the output buffer */
            it = pr->routItemList;
            pr->routItemList = pr->routItemList->next;
            if (pr->routItemList == NULL) {
                pr->rlastOutItem = NULL;
            }
            if (it->head.type == PICODATA_ITEM_TOKEN) {
                if ((it->head.info1 != PICODATA_ITEMINFO1_TOKTYPE_SPACE) && (it->head.len > 0)) {
                    nrUtfChars = picobase_utf8_length(it->data, PR_MAX_DATA_LEN);
                    if ((nrUtfChars == 1)
                        && (((id = picoktab_graphOffset(pr->graphs, it->data)) > 0))
                        && picoktab_getIntPropPunct(pr->graphs, id, &info1, &info2)) {
                        /* single punctuation chars have to be delivered as PICODATA_ITEM_PUNC items
                           instead as PICODATA_ITEM_WORDGRAPH items */
                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEM_PUNC;
                        pr->outBuf[pr->outWritePos++] = info1;
                        pr->outBuf[pr->outWritePos++] = info2;
                        pr->outBuf[pr->outWritePos++] = 0;
                        PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                            (picoos_uint8 *)"pr: ", pr->outBuf, pr->outWritePos);
                    }
                    else {
                        /* do subgraphs substitutions and deliver token items as PICODATA_ITEM_WORDGRAPH
                           items to the output buffer */
                        split = FALSE;
                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEM_WORDGRAPH;
                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEMINFO1_NA;
                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEMINFO2_NA;
                        lenpos=pr->outWritePos;
                        pr->outBuf[pr->outWritePos++] = 0;
                        pos = 0;
                        len = pr_strlen(it->data);
                        while (pos < (picoos_uint32)len) {
                            if (picobase_get_next_utf8char(it->data, it->head.len, &pos, inUtf8char)) {
                                if (inUtf8char[0] <= 32) {
                                    /* do not add whitespace characters to the output buffer,
                                       but initiate token splitting instead

                                    */
                                    split = TRUE;
                                }
                                else if (((id = picoktab_graphOffset(pr->graphs, inUtf8char)) > 0) && picoktab_getStrPropGraphsubs1(pr->graphs, id, outUtf8char)) {
                                    if (split) {
                                        /* split the token, eg. start a new item */
                                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEM_WORDGRAPH;
                                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEMINFO1_NA;
                                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEMINFO2_NA;
                                        lenpos=pr->outWritePos;
                                        pr->outBuf[pr->outWritePos++] = 0;
                                    }
                                    outUtf8charlen = picobase_det_utf8_length(outUtf8char[0]);
                                    for (i=0; i<outUtf8charlen; i++) {
                                        pr->outBuf[pr->outWritePos++] = outUtf8char[i];
                                        pr->outBuf[lenpos]++;
                                    }
                                    if (picoktab_getStrPropGraphsubs2(pr->graphs, id, outUtf8char)) {
                                        outUtf8charlen = picobase_det_utf8_length(outUtf8char[0]);
                                        for (i=0; i<outUtf8charlen; i++) {
                                            pr->outBuf[pr->outWritePos++] = outUtf8char[i];
                                            pr->outBuf[lenpos]++;
                                        }
                                    }
                                    split = FALSE;
                                }
                                else {
                                    if (split) {
                                        /* split the token, eg. start a new item */
                                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEM_WORDGRAPH;
                                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEMINFO1_NA;
                                        pr->outBuf[pr->outWritePos++] = PICODATA_ITEMINFO2_NA;
                                        lenpos=pr->outWritePos;
                                        pr->outBuf[pr->outWritePos++] = 0;
                                    }
                                    inUtf8charlen = picobase_det_utf8_length(inUtf8char[0]);
                                    for (i=0; i<inUtf8charlen; i++) {
                                        pr->outBuf[pr->outWritePos++] = inUtf8char[i];
                                        pr->outBuf[lenpos]++;
                                    }
                                    split = FALSE;
                                }
                            }
                        }
                        PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                            (picoos_uint8 *)"pr: ", pr->outBuf, pr->outWritePos);
                    }
                }
            }
            else {
                /* handle all other item types and put them to the output buffer */
                pr->outBuf[pr->outWritePos++] = it->head.type;
                pr->outBuf[pr->outWritePos++] = it->head.info1;
                pr->outBuf[pr->outWritePos++] = it->head.info2;
                pr->outBuf[pr->outWritePos++] = it->head.len;
                for (i=0; i<it->head.len; i++) {
                    pr->outBuf[pr->outWritePos++] = it->data[i];
                }
                PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                                   (picoos_uint8 *)"pr: ", pr->outBuf, pr->outWritePos);
            }
            pr_disposeItem(this, &it);
        }
        else if (pr->forceOutput) {
            pr_processItems(this, pr);
            if (pr->rinItemList == NULL) {
                pr->forceOutput = FALSE;
            }
        }
        else if ((pr->rgState != PR_GSNeedToken) && (pr->rinItemList != NULL)) {
            pr_processItems(this, pr);
        }
        else if (pr->inBufLen > 0) {
            /* input data is available in the input buffer, copy it to an input item
               and treat it */
            if (pr->dynMemSize < (45*PR_DYN_MEM_SIZE / 100)) {
                pr_newItem(this, pr_DynMem, &it, pr->inBuf[0], pr->inBuf[3], /*inItem*/TRUE);
                if (pr->outOfMemory) return PICODATA_PU_ERROR;
                it->head.type = pr->inBuf[0];
                it->head.info1 = pr->inBuf[1];
                it->head.info2 = pr->inBuf[2];
                it->head.len = pr->inBuf[3];
                for (i=0; i<pr->inBuf[3]; i++) {
                    it->data[i] = pr->inBuf[4+i];
                }
                it->data[pr->inBuf[3]] = 0;
                if ((pr->inBuf[0] == PICODATA_ITEM_TOKEN) && ((pr->inBuf[1] == PICODATA_ITEMINFO1_TOKTYPE_DIGIT))) {
                    it->val = tok_tokenDigitStrToInt(this, pr, it->data);
                } else {
                    it->val = 0;
                }
                if (pr->inBuf[0] == PICODATA_ITEM_TOKEN) {
                    picobase_lowercase_utf8_str(it->data,it->strci,PR_MAX_DATA_LEN, &ldone);
                    pr_firstLetterToLowerCase(it->data,it->strcis);
                    it->alc = picobase_is_utf8_lowercase(it->data,PR_MAX_DATA_LEN);
                    it->auc = picobase_is_utf8_uppercase(it->data,PR_MAX_DATA_LEN);
                    it->suc = pr_isSUC(it->data);
                }

                pr_treatItem(this, pr, it);
                if (pr->outOfMemory) return PICODATA_PU_ERROR;
                pr_processItems(this, pr);
                pr->inBufLen = 0;
            }
            else {
                pr->forceOutput = TRUE;
            }
        }
        else {
            /* there is not data in the output buffer and there is no data in the output item list, so
               check whether input data is available */
            rv = picodata_cbGetItem(this->cbIn, pr->inBuf, IN_BUF_SIZE+PICODATA_ITEM_HEADSIZE, &pr->inBufLen);
            if (PICO_OK == rv) {
            } else if (PICO_EOF == rv) {
                /* there was no item in the char buffer */
                return PICODATA_PU_IDLE;
            } else if ((PICO_EXC_BUF_UNDERFLOW == rv) || (PICO_EXC_BUF_OVERFLOW == rv)) {
                pr->inBufLen = 0;
                PICODBG_ERROR(("problem getting item"));
                picoos_emRaiseException(this->common->em, rv, NULL, NULL);
                return PICODATA_PU_ERROR;
            } else {
                pr->inBufLen = 0;
                PICODBG_ERROR(("problem getting item, unhandled"));
                picoos_emRaiseException(this->common->em, rv, NULL, NULL);
                return PICODATA_PU_ERROR;
            }
        }
#if PR_TRACE_MEM
        PICODBG_INFO(("memory: dyn=%u, work=%u", pr->dynMemSize, pr->workMemTop));
#endif
        if (pr->nrIterations <= 0) {
            return PICODATA_PU_BUSY;
        }
    } /* while */
    return PICODATA_PU_ERROR;
}

#ifdef __cplusplus
}
#endif



/* end */
