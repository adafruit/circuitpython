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
 * @file picokpr.h
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
/**
 * @addtogroup picokpr

 * <b> Knowledge handling for text preprocessing </b>\n
 *
*/

#ifndef PICOKPR_H_
#define PICOKPR_H_

#include "picoos.h"
#include "picoknow.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ************************************************************/
/* function to create specialized kb, */
/* to be used by picorsrc only */
/* ************************************************************/

pico_status_t picokpr_specializePreprocKnowledgeBase(picoknow_KnowledgeBase this,
                                                     picoos_Common common);


/* ************************************************************/
/* preproc type and getPreproc function */
/* ************************************************************/

/* maximal array length in preproc knowledge */
#define NPPMaxStrArrLen       100000000
#define NPPMaxLexCatArrLen    65536
#define NPPMaxAttrValLen      65536
#define NPPMaxOutItemArrLen   65536
#define NPPMaxTokArrLen       65536
#define NPPMaxProdArrLen      65536
#define NPPMaxCtxArrLen       65536

/* array offset types */
typedef picoos_uint32 picokpr_StrArrOffset;
typedef picoos_uint16 picokpr_LexCatArrOffset;
typedef picoos_uint16 picokpr_AttrValArrOffset;
typedef picoos_uint16 picokpr_OutItemArrOffset;
typedef picoos_uint16 picokpr_TokArrOffset;
typedef picoos_uint16 picokpr_ProdArrOffset;
typedef picoos_uint16 picokpr_CtxArrOffset;

typedef picoos_uchar * picokpr_VarStrPtr;

typedef picoos_int16 picokpr_LexCat;
typedef picoos_uint32 picokpr_TokSetNP;
typedef picoos_uint32 picokpr_TokSetWP;

/* preproc types */
typedef struct picokpr_preproc * picokpr_Preproc;

/* return kb preproc for usage in PU */
picokpr_Preproc picokpr_getPreproc(picoknow_KnowledgeBase this);


/* *****************************************************************************/
/* access routines */
/* *****************************************************************************/

/* knowledge base access routines for strings in StrArr */
extern picokpr_VarStrPtr picokpr_getVarStrPtr(picokpr_Preproc preproc, picokpr_StrArrOffset ofs);
extern picoos_bool picokpr_isEqual (picokpr_Preproc preproc, picoos_uchar str[], picoos_int32 len__9, picokpr_StrArrOffset str2);
extern picoos_bool picokpr_isEqualHead (picokpr_Preproc preproc, picoos_uchar str[], picoos_int32 len__10, picokpr_StrArrOffset head);
extern picoos_bool picokpr_isEqualMid (picokpr_Preproc preproc, picoos_uchar str[], picoos_int32 len__11, picokpr_StrArrOffset mid);
extern picoos_bool picokpr_isEqualTail (picokpr_Preproc preproc, picoos_uchar str[], picoos_int32 len__12, picokpr_StrArrOffset tail);

/* knowledge base access routines for lexical categories in LexCatArr */
extern picokpr_LexCat picokpr_getLexCat(picokpr_Preproc preproc, picokpr_LexCatArrOffset ofs);

/* knowledge base access routines for AttrVal fields in AttrValArr */
extern picoos_int32 picokpr_getAttrValArrInt32(picokpr_Preproc preproc, picokpr_AttrValArrOffset ofs);

/* knowledge base access routines for out items fields in OutItemArr */
extern picokpr_StrArrOffset picokpr_getOutItemStrOfs(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs);
extern picokpr_VarStrPtr picokpr_getOutItemStr(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs);
extern picoos_int32 picokpr_getOutItemType(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs);
extern picoos_int32 picokpr_getOutItemVal(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs);
extern picokpr_OutItemArrOffset picokpr_getOutItemArgOfs(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs);
extern picokpr_OutItemArrOffset picokpr_getOutItemNextOfs(picokpr_Preproc preproc, picokpr_OutItemArrOffset ofs);

/* knowledge base access routines for tokens in TokArr */
extern picokpr_TokSetNP picokpr_getTokSetNP(picokpr_Preproc preproc, picokpr_TokArrOffset ofs);
extern picokpr_TokSetWP picokpr_getTokSetWP(picokpr_Preproc preproc, picokpr_TokArrOffset ofs);
extern picokpr_TokArrOffset picokpr_getTokNextOfs(picokpr_Preproc preproc, picokpr_TokArrOffset ofs);
extern picokpr_TokArrOffset picokpr_getTokAltLOfs(picokpr_Preproc preproc, picokpr_TokArrOffset ofs);
extern picokpr_TokArrOffset picokpr_getTokAltROfs(picokpr_Preproc preproc, picokpr_TokArrOffset ofs);
extern picokpr_AttrValArrOffset picokpr_getTokAttribOfs(picokpr_Preproc preproc, picokpr_TokArrOffset ofs);

/* knowledge base access routines for productions in ProdArr */
extern picoos_int32 picokpr_getProdArrLen(picokpr_Preproc preproc);
extern picoos_int32 picokpr_getProdPrefCost(picokpr_Preproc preproc, picokpr_ProdArrOffset ofs);
extern picokpr_StrArrOffset picokpr_getProdNameOfs(picokpr_Preproc preproc, picokpr_ProdArrOffset ofs);
extern picokpr_TokArrOffset picokpr_getProdATokOfs(picokpr_Preproc preproc, picokpr_ProdArrOffset ofs);
extern picokpr_TokArrOffset picokpr_getProdETokOfs(picokpr_Preproc preproc, picokpr_ProdArrOffset ofs);

/* knowledge base access routines for contexts in CtxArr */
extern picoos_int32 picokpr_getCtxArrLen(picokpr_Preproc preproc);
extern picokpr_StrArrOffset picokpr_getCtxCtxNameOfs(picokpr_Preproc preproc, picokpr_CtxArrOffset ofs);
extern picokpr_StrArrOffset picokpr_getCtxNetNameOfs(picokpr_Preproc preproc, picokpr_CtxArrOffset ofs);
extern picokpr_StrArrOffset picokpr_getCtxProdNameOfs(picokpr_Preproc preproc, picokpr_CtxArrOffset ofs);

/* knowledge base access routines for preprocs */
extern picokpr_VarStrPtr picokpr_getPreprocNetName(picokpr_Preproc preproc);

#ifdef __cplusplus
}
#endif


#endif /*PICOKPR_H_*/
