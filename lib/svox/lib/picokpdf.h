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
 * @file picokpdf.h
 *
 * knowledge handling for pdf
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#ifndef PICOKPDF_H_
#define PICOKPDF_H_

#include "picoos.h"
#include "picoknow.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ************************************************************/
/**
 * @addtogroup picokpdf
 *
   Two specialized pdf kb types are provided by this knowledge
   handling module:

   - pdf dur:     ...kpdf_DUR         (for dur)
   - pdf mul:     ...kpdf_MUL         (for lfz and mgc)
   - pdf phs:     ...kpdf_PHS         (for phase)

*/
/* ************************************************************/


/* ************************************************************/
/* defines and functions to create specialized kb, */
/* to be used by picorsrc only */
/* ************************************************************/

#define PICOKPDF_MAX_NUM_STATES 10

#define PICOKPDF_MAX_MUL_LFZ_CEPORDER 1
#define PICOKPDF_MAX_MUL_MGC_CEPORDER 25

/* trade accuracy against computation: more long multiplications.
 * Maximum is 15 when invdiag0=(1<<(2*bigpow))/diag0 used
 * currently observing instability in mlpg when bigpow >= 14, this needs to be investigated */

#define PICOKPDF_BIG_POW 12

typedef enum {
    PICOKPDF_KPDFTYPE_DUR,
    PICOKPDF_KPDFTYPE_MUL,
    PICOKPDF_KPDFTYPE_PHS
} picokpdf_kpdftype_t;

pico_status_t picokpdf_specializePdfKnowledgeBase(picoknow_KnowledgeBase this,
                                              picoos_Common common,
                                              const picokpdf_kpdftype_t type);


/* ************************************************************/
/* pdf types and get Pdf functions */
/* ************************************************************/

/** object       : PdfDur, PdfMUL
 *  shortcut     : kpdf*
 *  derived from : picoknow_KnowledgeBase
 */

typedef struct picokpdf_pdfdur *picokpdf_PdfDUR;
typedef struct picokpdf_pdfmul *picokpdf_PdfMUL;
typedef struct picokpdf_pdfphs *picokpdf_PdfPHS;

/* subobj specific for pdf dur type */
typedef struct picokpdf_pdfdur {
    picoos_uint16 numframes;
    picoos_uint8 vecsize;
    picoos_uint8 sampperframe;
    picoos_uint8 phonquantlen;
    picoos_uint8 *phonquant;
    picoos_uint8 statequantlen;
    picoos_uint8 *statequant;
    picoos_uint8 *content;
} picokpdf_pdfdur_t;

/* subobj specific for pdf mul type */
typedef struct picokpdf_pdfmul {
    picoos_uint16 numframes;
    picoos_uint8 vecsize;
    picoos_uint8 numstates;
    picoos_uint16 stateoffset[PICOKPDF_MAX_NUM_STATES]; /* offset within a phone to find the state ? */
    picoos_uint8 ceporder;
    picoos_uint8 numvuv;
    picoos_uint8 numdeltas;
    picoos_uint8 meanpow;
    picoos_uint8 bigpow;
    picoos_uint8 amplif;
    picoos_uint8 *meanpowUm;  /* KPDF_NUMSTREAMS x ceporder values */
    picoos_uint8 *ivarpow;    /* KPDF_NUMSTREAMS x ceporder values */
    picoos_uint8 *content;
} picokpdf_pdfmul_t;

/* subobj specific for pdf phs type */
typedef struct picokpdf_pdfphs {
    picoos_uint16 numvectors;
    picoos_uint8 *indexBase;
    picoos_uint8 *contentBase;
} picokpdf_pdfphs_t;

/* return kb pdf for usage in PU */
picokpdf_PdfDUR picokpdf_getPdfDUR(picoknow_KnowledgeBase this);
picokpdf_PdfMUL picokpdf_getPdfMUL(picoknow_KnowledgeBase this);
picokpdf_PdfPHS picokpdf_getPdfPHS(picoknow_KnowledgeBase this);


/* ************************************************************/
/* PDF DUR functions */
/* ************************************************************/

/* e.g. */
/*picoos_uint8 picokpdf_pdfDURgetEle(const picokpdf_PdfDUR this,
                                   const picoos_uint16 row,
                                   const picoos_uint16 col,
                                   picoos_uint16 *val);
*/

/* ************************************************************/
/* PDF MUL functions */
/* ************************************************************/

#ifdef __cplusplus
}
#endif


#endif /*PICOKPDF_H_*/
