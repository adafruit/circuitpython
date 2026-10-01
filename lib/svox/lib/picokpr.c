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
 * @file picokpr.c
 *
 * knowledge handling for text preprocessing
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
#include "picodata.h"
#include "picoknow.h"
#include "picokpr.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ************************************************************/
/* preproc */
/* ************************************************************/

/*
  overview:
*/


/* ************************************************************/
/* preproc data defines */
/* ************************************************************/

#define KPR_STR_SIZE 1
#define KPR_LEXCAT_SIZE 2
#define KPR_ATTRVAL_SIZE 4
#define KPR_OUTITEM_SIZE 7
#define KPR_TOK_SIZE 16
#define KPR_PROD_SIZE 12
#define KPR_CTX_SIZE 12

#define KPR_NETNAME_OFFSET        0
#define KPR_STRARRLEN_OFFSET      4
#define KPR_LEXCATARRLEN_OFFSET   8
#define KPR_ATTRVALARRLEN_OFFSET  12
#define KPR_OUTITEMARRLEN_OFFSET  16
#define KPR_TOKARRLEN_OFFSET      20
#define KPR_PRODARRLEN_OFFSET     24
#define KPR_CTXARRLEN_OFFSET      28

#define KPR_ARRAY_START           32

#define KPR_MAX_INT32             2147483647

#define KPR_STR_OFS               0

#define KPR_LEXCAT_OFS            0

#define KPR_ATTRVAL_INT_OFS       0
#define KPR_ATTRVAL_STROFS_OFS    0
#define KPR_ATTRVAL_PRODOFS_OFS   0
#define KPR_ATTRVAL_OUTITMOFS_OFS 0
#define KPR_ATTRVAL_LEXCATOFS_OFS 0

#define KPR_OUTITEM_NEXTOFS_OFS   0
#define KPR_OUTITEM_TYPE_OFS      2
#define KPR_OUTITEM_STROFS_OFS    3
#define KPR_OUTITEM_VAL_OFS       3
#define KPR_OUTITEM_ARGOFS_OFS    3

#define KPR_TOK_SETWP_OFS         0
#define KPR_TOK_SETNP_OFS         4
#define KPR_TOK_NEXTOFS_OFS       8
#define KPR_TOK_ALTLOFS_OFS       10
#define KPR_TOK_ALTROFS_OFS       12
#define KPR_TOK_ATTRIBOFS_OFS     14

#define KPR_PROD_PRODPREFCOST_OFS 0
#define KPR_PROD_PRODNAMEOFS_OFS  4
#define KPR_PROD_ATOKOFS_OFS      8
#define KPR_PROD_ETOKOFS_OFS      10

#define KPR_CTX_CTXNAMEOFS_OFS    0
#define KPR_CTX_NETNAMEOFS_OFS    4
#define KPR_CTX_PRODNAMEOFS_OFS   8

/* ************************************************************/
/* preproc type and loading */
/* ************************************************************/

/* variable array element types */
typedef picoos_uint8 picokpr_Str[KPR_STR_SIZE];
typedef picoos_uint16 picokpr_LexCat2;
typedef picoos_uint8 picokpr_AttrVal[KPR_ATTRVAL_SIZE];
typedef picoos_uint8 picokpr_OutItem[KPR_OUTITEM_SIZE];
typedef picoos_uint8 picokpr_Tok[KPR_TOK_SIZE];
typedef picoos_uint8 picokpr_Prod[KPR_PROD_SIZE];
typedef picoos_uint8 picokpr_Ctx[KPR_CTX_SIZE];

/* variable array types */
typedef picokpr_Str * picokpr_VarStrArr;
typedef picokpr_LexCat2 * picokpr_VarLexCatArr;
typedef picokpr_AttrVal * picokpr_VarAttrValArr;
typedef picokpr_OutItem * picokpr_VarOutItemArr;
typedef picokpr_Tok * picokpr_VarTokArr;
typedef picokpr_Prod * picokpr_VarProdArr;
typedef picokpr_Ctx * picokpr_VarCtxArr;

/* ************************************************************/
/* preproc type and loading */
/* ************************************************************/

/** object       : PreprocKnowledgeBase
 *  shortcut     : kpr
 *  derived from : picoknow_KnowledgeBase
 */

typedef struct kpr_subobj * kpr_SubObj;

typedef struct kpr_subobj
{
    picoos_uchar * rNetName;

    picoos_int32 rStrArrLen;
    picoos_int32 rLexCatArrLen;
    picoos_int32 rAttrValArrLen;
    picoos_int32 rOutItemArrLen;
    picoos_int32 rTokArrLen;
    picoos_int32 rProdArrLen;
    picoos_int32 rCtxArrLen;

    picoos_uint8 * rStrArr;
    picokpr_LexCat2 * rLexCatArr;
    picokpr_AttrVal * rAttrValArr;
    picokpr_OutItem * rOutItemArr;
    picokpr_Tok * rTokArr;
    picokpr_Prod * rProdArr;
    picokpr_Ctx * rCtxArr;
} kpr_subobj_t;


static picoos_uint32 kpr_getUInt32(picoos_uint8 * p)
{
    return (p[0]) | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}


static pico_status_t kprInitialize(register picoknow_KnowledgeBase this,
                                   picoos_Common common)
{
    picoos_uint32 offset = 0;
    kpr_subobj_t * kpr;

    PICODBG_DEBUG(("start"));

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    kpr = (kpr_subobj_t *) this->subObj;

    kpr->rStrArrLen = kpr_getUInt32(&(this->base[KPR_STRARRLEN_OFFSET]));
    kpr->rLexCatArrLen = kpr_getUInt32(&(this->base[KPR_LEXCATARRLEN_OFFSET]));
    kpr->rAttrValArrLen = kpr_getUInt32(&(this->base[KPR_ATTRVALARRLEN_OFFSET]));
    kpr->rOutItemArrLen = kpr_getUInt32(&(this->base[KPR_OUTITEMARRLEN_OFFSET]));
    kpr->rTokArrLen = kpr_getUInt32(&(this->base[KPR_TOKARRLEN_OFFSET]));
    kpr->rProdArrLen = kpr_getUInt32(&(this->base[KPR_PRODARRLEN_OFFSET]));
    kpr->rCtxArrLen = kpr_getUInt32(&(this->base[KPR_CTXARRLEN_OFFSET]));

    offset = KPR_ARRAY_START;
    kpr->rStrArr = &(this->base[offset]);
    PICODBG_DEBUG(("rStrArr     : cs: %i, ss: %i, offset: %i", sizeof(picokpr_Str), KPR_STR_SIZE, offset));
    offset = offset + kpr->rStrArrLen * 1;

    kpr->rLexCatArr = (picokpr_LexCat2 *)&(this->base[offset]);
    PICODBG_DEBUG(("rLexCatArr  : cs: %i, ss: %i, offset: %i", KPR_LEXCAT_SIZE, sizeof(picokpr_LexCat2), offset));
    offset = offset + kpr->rLexCatArrLen * KPR_LEXCAT_SIZE;

    kpr->rAttrValArr = (picokpr_AttrVal *)&(this->base[offset]);
    PICODBG_DEBUG(("rAttrValArr : cs: %i, ss: %i, offset: %i", KPR_ATTRVAL_SIZE, sizeof(picokpr_AttrVal), offset));
    offset = offset + kpr->rAttrValArrLen * KPR_ATTRVAL_SIZE;

    kpr->rOutItemArr = (picokpr_OutItem *)&(this->base[offset]);
    PICODBG_DEBUG(("rOutItemArr : cs: %i, ss: %i, offset: %i", KPR_OUTITEM_SIZE, sizeof(picokpr_OutItem), offset));
    offset = offset + kpr->rOutItemArrLen * KPR_OUTITEM_SIZE;

    kpr->rTokArr = (picokpr_Tok *)&(this->base[offset]);
    PICODBG_DEBUG(("rTokArr     : cs: %i, ss: %i, offset: %i", KPR_TOK_SIZE, sizeof(picokpr_Tok), offset));
    offset = offset + kpr->rTokArrLen * KPR_TOK_SIZE;

    kpr->rProdArr = (picokpr_Prod *)&(this->base[offset]);
    PICODBG_DEBUG(("rProdArr    : cs: %i, ss: %i, offset: %i", KPR_PROD_SIZE, sizeof(picokpr_Prod), offset));
    offset = offset + kpr->rProdArrLen * KPR_PROD_SIZE;

    kpr->rCtxArr = (picokpr_Ctx *)&(this->base[offset]);
    PICODBG_DEBUG(("rCtxArr     : cs: %i, ss: %i, offset: %i", KPR_CTX_SIZE, sizeof(picokpr_Ctx), offset));
    offset = offset + kpr->rCtxArrLen * KPR_CTX_SIZE;

    kpr->rNetName = &(kpr->rStrArr[kpr_getUInt32(&(this->base[KPR_NETNAME_OFFSET]))]);

    return PICO_OK;
}


static pico_status_t kprSubObjDeallocate(register picoknow_KnowledgeBase this,
                                         picoos_MemoryManager mm)
{
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}


/* we don't offer a specialized constructor for a PreprocKnowledgeBase but
 * instead a "specializer" of an allready existing generic
 * picoknow_KnowledgeBase */

pico_status_t picokpr_specializePreprocKnowledgeBase(picoknow_KnowledgeBase this,
                                                     picoos_Common common)
{
    if (NULL == this) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    this->subDeallocate = kprSubObjDeallocate;
    this->subObj = picoos_allocate(common->mm, sizeof(kpr_subobj_t));
    if (NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                       NULL, NULL);
    }
    return kprInitialize(this, common);
}

/* ************************************************************/
/* preproc getPreproc */
/* ************************************************************/

picokpr_Preproc picokpr_getPreproc(picoknow_KnowledgeBase this)
{
    if (NULL == this) {
        return NULL;
    } else {
        return (picokpr_Preproc) this->subObj;
    }
}


/* *****************************************************************************/
/* knowledge base access routines for strings in StrArr */
/* *****************************************************************************/

extern picokpr_VarStrPtr picokpr_getVarStrPtr(picokpr_Preproc preproc, picokpr_StrArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rStrArr[ofs]);

    return p;
}

/* *****************************************************************************/

extern picoos_bool picokpr_isEqual (picokpr_Preproc preproc, picoos_uchar str[], picoos_int32 len__9, picokpr_StrArrOffset str2)
{
    picokpr_VarStrPtr lstrp;
    len__9 = len__9;        /*PP 13.10.08 : fix warning "var not used in this function"*/
    lstrp = (picokpr_VarStrPtr)&((kpr_SubObj)preproc)->rStrArr[str2];
    return picoos_strcmp((picoos_char*)lstrp,(picoos_char*)str) == 0;
}



extern picoos_bool picokpr_isEqualHead (picokpr_Preproc preproc, picoos_uchar str[], picoos_int32 len__10, picokpr_StrArrOffset head)
{
    picokpr_VarStrPtr lstrp;
    len__10 = len__10;        /*PP 13.10.08 : fix warning "var not used in this function"*/
    lstrp = (picokpr_VarStrPtr)&((kpr_SubObj)preproc)->rStrArr[head];
    return (picoos_strstr((picoos_char*)str, (picoos_char*)lstrp) == (picoos_char*)str);
}



extern picoos_bool picokpr_isEqualMid (picokpr_Preproc preproc, picoos_uchar str[], picoos_int32 len__11, picokpr_StrArrOffset mid)
{
    picokpr_VarStrPtr lstrp;
    len__11 = len__11;        /*PP 13.10.08 : fix warning "var not used in this function"*/
    lstrp = (picokpr_VarStrPtr)(void *) &((kpr_SubObj)preproc)->rStrArr[mid];
    return (picoos_strstr((picoos_char*)str, (picoos_char*)lstrp) != NULL);
}



extern picoos_bool picokpr_isEqualTail (picokpr_Preproc preproc, picoos_uchar str[], picoos_int32 len__12, picokpr_StrArrOffset tail)
{
    picoos_int32 lstart;
    picokpr_VarStrPtr lstrp;
    len__12 = len__12;        /* avoid warning "var not used in this function"*/
    lstrp = (picokpr_VarStrPtr)&((kpr_SubObj)preproc)->rStrArr[tail];
    lstart = picoos_strlen((picoos_char*)str) - picoos_strlen((picoos_char*)lstrp);
    if (lstart >= 0) {
        return (picoos_strstr((picoos_char*)&(str[lstart]), (picoos_char*)lstrp) != NULL);
    } else {
        return FALSE;
    }
}

/* *****************************************************************************/
/* knowledge base access routines for lexical categories in LexCatArr */
/* *****************************************************************************/

extern picokpr_LexCat picokpr_getLexCat(picokpr_Preproc preproc, picokpr_LexCatArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rLexCatArr[ofs]);

    return p[0] + 256*p[1];
}

/* *****************************************************************************/
/* knowledge base access routines for AttrVal fields in AttrValArr */
/* *****************************************************************************/

extern picoos_int32 picokpr_getAttrValArrInt32(picokpr_Preproc preproc, picokpr_AttrValArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rAttrValArr[ofs]);
    picoos_uint32 c = kpr_getUInt32(&p[KPR_ATTRVAL_INT_OFS]);

    if (c > KPR_MAX_INT32) {
        return (c - KPR_MAX_INT32) - 1;
    } else {
        return (((int)c + (int) -(KPR_MAX_INT32)) - 1);
    }
}

/* *****************************************************************************/
/* knowledge base access routines for AttrVal fields in AttrValArr */
/* *****************************************************************************/

extern picokpr_OutItemArrOffset picokpr_getOutItemNextOfs(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rOutItemArr[ofs]);

    return p[KPR_OUTITEM_NEXTOFS_OFS+0] + 256*p[KPR_OUTITEM_NEXTOFS_OFS+1];
}


extern picoos_int32 picokpr_getOutItemType(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rOutItemArr[ofs]);

    return p[KPR_OUTITEM_TYPE_OFS+0];
}


extern picokpr_StrArrOffset picokpr_getOutItemStrOfs(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rOutItemArr[ofs]);

    return             p[KPR_OUTITEM_STROFS_OFS+0] +
                   256*p[KPR_OUTITEM_STROFS_OFS+1] +
               256*256*p[KPR_OUTITEM_STROFS_OFS+2] +
           256*256*256*p[KPR_OUTITEM_STROFS_OFS+3];
}


extern picokpr_VarStrPtr picokpr_getOutItemStr(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rOutItemArr[ofs]);
    picoos_uint32 c =  p[KPR_OUTITEM_STROFS_OFS+0] +
                   256*p[KPR_OUTITEM_STROFS_OFS+1] +
               256*256*p[KPR_OUTITEM_STROFS_OFS+2] +
           256*256*256*p[KPR_OUTITEM_STROFS_OFS+3];

    return (picoos_uint8 *)&(((kpr_SubObj)preproc)->rStrArr[c]);
}

extern picoos_int32 picokpr_getOutItemVal(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rOutItemArr[ofs]);
    picoos_uint32 c = kpr_getUInt32(&p[KPR_OUTITEM_VAL_OFS]);

    if (c > KPR_MAX_INT32) {
        return (c - KPR_MAX_INT32) - 1;
    } else {
        return (((int)c + (int) -(KPR_MAX_INT32)) - 1);
    }
}


extern picokpr_OutItemArrOffset picokpr_getOutItemArgOfs(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rOutItemArr[ofs]);

    return             p[KPR_OUTITEM_ARGOFS_OFS+0] +
                   256*p[KPR_OUTITEM_ARGOFS_OFS+1] +
               256*256*p[KPR_OUTITEM_ARGOFS_OFS+2] +
           256*256*256*p[KPR_OUTITEM_ARGOFS_OFS+3];
}


/* *****************************************************************************/
/* knowledge base access routines for tokens in TokArr */
/* *****************************************************************************/

extern picokpr_TokSetNP picokpr_getTokSetNP(picokpr_Preproc preproc, picokpr_TokArrOffset ofs)
{
    picoos_uint32 c/*, b*/;
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]) + KPR_TOK_SETNP_OFS;
    /*picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]);*/
    picoos_uint32 p0, p1, p2, p3;

        p0 = *(p++);
        p1 = *(p++);
        p2 = *(p++);
        p3 = *p;

        c = p0 + (p1<<8) + (p2<<16) + (p3<<24);

        return c;

        /*
    c =                p[KPR_TOK_SETNP_OFS+0] +
                   256*p[KPR_TOK_SETNP_OFS+1] +
               256*256*p[KPR_TOK_SETNP_OFS+2] +
           256*256*256*p[KPR_TOK_SETNP_OFS+3];

    b = 0;
    i = 0;
    while ((i <= 31) && (c > 0)) {
        if (c % 2 == 1) {
            b |= (1<<i);
        }
        c = c / 2;
        i++;
    }

    return b;
        */
}


extern picokpr_TokSetWP picokpr_getTokSetWP(picokpr_Preproc preproc, picokpr_TokArrOffset ofs)
{
    picoos_uint32 c/*, b*/;
    /* picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]);*/
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]) + KPR_TOK_SETWP_OFS;
    picoos_uint32 p0, p1, p2, p3;

    p0 = *(p++);
    p1 = *(p++);
    p2 = *(p++);
    p3 = *p;

    c = p0 + (p1<<8) + (p2<<16) + (p3<<24);
    return c;

    /*
    c =                p[KPR_TOK_SETWP_OFS+0] +
                   256*p[KPR_TOK_SETWP_OFS+1] +
               256*256*p[KPR_TOK_SETWP_OFS+2] +
           256*256*256*p[KPR_TOK_SETWP_OFS+3];

    b = 0;
    i = 0;
    while ((i <= 31) && (c > 0)) {
        if (c % 2 == 1) {
            b |= (1<<i);
        }
        c = c / 2;
        i++;
    }

    return b;
    */
}


extern picokpr_TokArrOffset picokpr_getTokNextOfs(picokpr_Preproc preproc, picokpr_TokArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]);

    return p[KPR_TOK_NEXTOFS_OFS+0] + 256*p[KPR_TOK_NEXTOFS_OFS+1];
}


extern picokpr_TokArrOffset picokpr_getTokAltLOfs(picokpr_Preproc preproc, picokpr_TokArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]) + KPR_TOK_ALTLOFS_OFS;
    picokpr_TokArrOffset c = *p++;
    return c +   256**p;

    /*picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]);

      return p[KPR_TOK_ALTLOFS_OFS+0] + 256*p[KPR_TOK_ALTLOFS_OFS+1];
    */
}


extern picokpr_TokArrOffset picokpr_getTokAltROfs(picokpr_Preproc preproc, picokpr_TokArrOffset ofs)
{
     picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]) + KPR_TOK_ALTROFS_OFS;
    picokpr_TokArrOffset c = *p++;
    return c +   256**p;

    /*picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]);

      return p[KPR_TOK_ALTROFS_OFS+0] + 256*p[KPR_TOK_ALTROFS_OFS+1];*/
}


extern picokpr_AttrValArrOffset picokpr_getTokAttribOfs(picokpr_Preproc preproc, picokpr_TokArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rTokArr[ofs]);

    return p[KPR_TOK_ATTRIBOFS_OFS+0] + 256*p[KPR_TOK_ATTRIBOFS_OFS+1];
}

/* *****************************************************************************/
/* knowledge base access routines for productions in ProdArr */
/* *****************************************************************************/

extern picoos_int32 picokpr_getProdArrLen(picokpr_Preproc preproc)
{
    return ((kpr_SubObj)preproc)->rProdArrLen;
}

extern picoos_int32 picokpr_getProdPrefCost(picokpr_Preproc preproc, picokpr_ProdArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rProdArr[ofs]);
    picoos_uint32 c =  kpr_getUInt32(&p[KPR_PROD_PRODPREFCOST_OFS]);

    if (c > KPR_MAX_INT32) {
        return (c - KPR_MAX_INT32) - 1;
    } else {
        return (((int)c + (int) -(KPR_MAX_INT32)) - 1);
    }
}


extern picokpr_StrArrOffset picokpr_getProdNameOfs(picokpr_Preproc preproc, picokpr_ProdArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rProdArr[ofs]);

    return             p[KPR_PROD_PRODNAMEOFS_OFS+0] +
                   256*p[KPR_PROD_PRODNAMEOFS_OFS+1] +
               256*256*p[KPR_PROD_PRODNAMEOFS_OFS+2] +
           256*256*256*p[KPR_PROD_PRODNAMEOFS_OFS+3];

}


extern picokpr_TokArrOffset picokpr_getProdATokOfs(picokpr_Preproc preproc, picokpr_ProdArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rProdArr[ofs]);

    return p[KPR_PROD_ATOKOFS_OFS+0] + 256*p[KPR_PROD_ATOKOFS_OFS+1];
}


extern picokpr_TokArrOffset picokpr_getProdETokOfs(picokpr_Preproc preproc, picokpr_ProdArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rProdArr[ofs]);

    return p[KPR_PROD_ETOKOFS_OFS+0] + 256*p[KPR_PROD_ETOKOFS_OFS+1];
}

/* *****************************************************************************/
/* knowledge base access routines for contexts in CtxArr */
/* *****************************************************************************/

extern picoos_int32 picokpr_getCtxArrLen(picokpr_Preproc preproc)
{
    return ((kpr_SubObj)preproc)->rCtxArrLen;
}

extern picokpr_StrArrOffset picokpr_getCtxCtxNameOfs(picokpr_Preproc preproc, picokpr_CtxArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rCtxArr[ofs]);

    return             p[KPR_CTX_CTXNAMEOFS_OFS+0] +
                   256*p[KPR_CTX_CTXNAMEOFS_OFS+1] +
               256*256*p[KPR_CTX_CTXNAMEOFS_OFS+2] +
           256*256*256*p[KPR_CTX_CTXNAMEOFS_OFS+3];
}


extern picokpr_StrArrOffset picokpr_getCtxNetNameOfs(picokpr_Preproc preproc, picokpr_CtxArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rCtxArr[ofs]);

    return             p[KPR_CTX_NETNAMEOFS_OFS+0] +
                   256*p[KPR_CTX_NETNAMEOFS_OFS+1] +
               256*256*p[KPR_CTX_NETNAMEOFS_OFS+2] +
           256*256*256*p[KPR_CTX_NETNAMEOFS_OFS+3];
}


extern picokpr_StrArrOffset picokpr_getCtxProdNameOfs(picokpr_Preproc preproc, picokpr_CtxArrOffset ofs)
{
    picoos_uint8 * p = (picoos_uint8 *)&(((kpr_SubObj)preproc)->rCtxArr[ofs]);

    return             p[KPR_CTX_PRODNAMEOFS_OFS+0] +
                   256*p[KPR_CTX_PRODNAMEOFS_OFS+1] +
               256*256*p[KPR_CTX_PRODNAMEOFS_OFS+2] +
           256*256*256*p[KPR_CTX_PRODNAMEOFS_OFS+3];
}

/* *****************************************************************************/
/* knowledge base access routines for networks */
/* *****************************************************************************/

extern picokpr_VarStrPtr picokpr_getPreprocNetName(picokpr_Preproc preproc)
{
    return ((kpr_SubObj)preproc)->rNetName;
}


#ifdef __cplusplus
}
#endif


/* end */
