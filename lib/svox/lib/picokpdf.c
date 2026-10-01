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
 * @file picokpdf.c
 *
 *  knowledge handling for pdf
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
#include "picokpdf.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ************************************************************/
/* pdf */
/* ************************************************************/

/*
 * @addtogroup picokpdf
 *
  overview: format of knowledge base pdf file

  This is the format for the dur pdf file:
    - Numframes:     1             uint16
    - Vecsize:       1             uint8
    - sampperframe:  1             uint8
    - Phonquantlen:  1             uint8
    - Phonquant:     Phonquantlen  uint8
    - Statequantlen: 1             uint8
    - Statequantlen: Statequantlen uint8
    - And then numframes x vecsize uint8

  This is the format for mul (mgc and lfz) pdf files:
    - numframes:         1         uint16
    - vecsize:           1         uint8
    - numstates:         1         uint8
    - numframesperstate: numstates uint16
    - ceporder:          1         uint8
    - numvuv             1         uint8
    - numdeltas:         1         uint8
    - scmeanpow:         1         uint8
    - maxbigpow:         1         uint8
    - scmeanpowum  KPDF_NUMSTREAMS * ceporder uint8
    - scivarpow    KPDF_NUMSTREAMS * ceporder uint8

    And then numframes x vecsize uint8

*/


/* ************************************************************/
/* pdf data defines */
/* may not be changed with current implementation */
/* ************************************************************/


#define KPDF_NUMSTREAMS  3 /* coeff, delta, deltadelta */


/* ************************************************************/
/* pdf loading */
/* ************************************************************/

static pico_status_t kpdfDURInitialize(register picoknow_KnowledgeBase this,
                                       picoos_Common common) {
    picokpdf_pdfdur_t *pdfdur;
    picoos_uint16 pos;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    pdfdur = (picokpdf_pdfdur_t *)this->subObj;

    pos = 0;

    pdfdur->numframes = ((picoos_uint16)(this->base[pos+1])) << 8 |
        this->base[pos];
    pos += 2;
    pdfdur->vecsize = this->base[pos++];
    pdfdur->sampperframe = this->base[pos++];
    pdfdur->phonquantlen = this->base[pos++];
    pdfdur->phonquant = &(this->base[pos]);
    pos += pdfdur->phonquantlen;
    pdfdur->statequantlen = this->base[pos++];
    pdfdur->statequant = &(this->base[pos]);
    pos += pdfdur->statequantlen;
    pdfdur->content = &(this->base[pos]);
    PICODBG_DEBUG(("numframes %d, vecsize %d, phonquantlen %d, "
                   "statequantlen %d", pdfdur->numframes, pdfdur->vecsize,
                   pdfdur->phonquantlen, pdfdur->statequantlen));
    if ((picoos_uint32)(pos + (pdfdur->numframes * pdfdur->vecsize)) != this->size) {
        PICODBG_DEBUG(("header-spec size %d, kb-size %d",
                       pos + (pdfdur->numframes * pdfdur->vecsize),
                       this->size));
        return picoos_emRaiseException(common->em, PICO_EXC_FILE_CORRUPT,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("dur pdf initialized"));
    return PICO_OK;
}

static picoos_uint8 convScaleFactorToBig(picoos_uint8 pow, picoos_uint8 bigpow)
{
    if (pow > 0x0F) {
        pow = bigpow + (0xFF - pow + 1);  /* take 2's complement of negative pow */
    } else if (bigpow >= pow) {
        pow = bigpow - pow;
    } else {
        /* error: bigpow is smaller than input pow */
        return 0;
    }
    return pow;
}

static pico_status_t kpdfMULInitialize(register picoknow_KnowledgeBase this,
                                       picoos_Common common) {
    picokpdf_pdfmul_t *pdfmul;
    picoos_uint16 pos;
    picoos_uint8 scmeanpow, maxbigpow, nummean;
    picoos_uint8 i;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    pdfmul = (picokpdf_pdfmul_t *)this->subObj;

    pos = 0;

    pdfmul->numframes = ((picoos_uint16)(this->base[pos+1])) << 8 |
        this->base[pos];
    pos += 2;
    pdfmul->vecsize = this->base[pos++];
    pdfmul->numstates = this->base[pos++];
    {
        pdfmul->stateoffset[0] = (picoos_uint16) 0;
        for (i=1; i<pdfmul->numstates; i++) {
            pdfmul->stateoffset[i] = pdfmul->stateoffset[i-1] + (this->base[pos] | ((picoos_uint16) this->base[pos+1] << 8));
            pos += 2;
        }
        pos += 2; /* we don't need the last number if we only need the offset (i.e. how to get to the vector start) */
    }

    pdfmul->ceporder = this->base[pos++];
    pdfmul->numvuv = this->base[pos++];
    pdfmul->numdeltas = this->base[pos++];
    scmeanpow = this->base[pos++];
    maxbigpow = this->base[pos++];
    if (maxbigpow < PICOKPDF_BIG_POW) {
        PICODBG_ERROR(("bigpow %i is larger than maxbigpow %i defined in pdf lingware", PICOKPDF_BIG_POW, maxbigpow));
        return picoos_emRaiseException(common->em, PICO_EXC_MAX_NUM_EXCEED,NULL,NULL);
    }
    pdfmul->bigpow = PICOKPDF_BIG_POW; /* what we have to use is the smaller number! */

    pdfmul->amplif = this->base[pos++];

    /* bigpow corrected by scmeanpow, multiply means by 2^meanpow to obtain fixed point representation */
    pdfmul->meanpow = convScaleFactorToBig(scmeanpow, pdfmul->bigpow);
    if (0 == pdfmul->meanpow) {
        PICODBG_ERROR(("error in convScaleFactorToBig"));
        return picoos_emRaiseException(common->em, PICO_EXC_MAX_NUM_EXCEED,NULL,NULL);
    }
    nummean = 3*pdfmul->ceporder;

    pdfmul->meanpowUm = picoos_allocate(common->mm,nummean*sizeof(picoos_uint8));
    pdfmul->ivarpow = picoos_allocate(common->mm,nummean*sizeof(picoos_uint8));
    if ((NULL == pdfmul->meanpowUm) || (NULL == pdfmul->ivarpow)) {
        picoos_deallocate(common->mm,(void *) &(pdfmul->meanpowUm));
        picoos_deallocate(common->mm,(void *) &(pdfmul->ivarpow));
        return picoos_emRaiseException(common->em,PICO_EXC_OUT_OF_MEM,NULL,NULL);
    }

    /*     read meanpowUm and convert on the fly */
    /*     meaning of meanpowUm becomes: multiply means from pdf stream by 2^meanpowUm
     * to achieve fixed point scaling by big
     */
    for (i=0; i<nummean; i++) {
        pdfmul->meanpowUm[i] = convScaleFactorToBig(this->base[pos++], pdfmul->bigpow);
    }

   /*read ivarpow  and convert on the fly */
    for (i=0; i<nummean; i++) {
        pdfmul->ivarpow[i] = convScaleFactorToBig(this->base[pos++], pdfmul->bigpow);
    }

    /* check numdeltas */
    if ((pdfmul->numdeltas == 0xFF) && (pdfmul->vecsize != (pdfmul->numvuv + pdfmul->ceporder * 3 * (2+1)))) {
        PICODBG_ERROR(("header has inconsistent values for vecsize, ceporder, numvuv, and numdeltas"));
        return picoos_emRaiseException(common->em,PICO_EXC_FILE_CORRUPT,NULL,NULL);
     }

/*     vecsize: 1 uint8 for vuv
         + ceporder short for static means
         + numdeltas uint8 and short for sparse delta means
         + ceporder*3 uint8 for static and delta inverse variances
*/
    if ((pdfmul->numdeltas != 0xFF) && (pdfmul->vecsize != pdfmul->numvuv+pdfmul->ceporder*2+pdfmul->numdeltas*3+pdfmul->ceporder*3)) {
        PICODBG_ERROR(("header has inconsistent values for vecsize, ceporder, numvuv, and numdeltas\n"
                "vecsize = %i while numvuv+ceporder*2 + numdeltas*3 + ceporder*3 = %i",
                pdfmul->vecsize, pdfmul->numvuv + pdfmul->ceporder*2 + pdfmul->numdeltas * 3 + pdfmul->ceporder * 3));
        return picoos_emRaiseException(common->em,PICO_EXC_FILE_CORRUPT,NULL,NULL);
    }
    pdfmul->content = &(this->base[pos]);
    PICODBG_DEBUG(("numframes %d, vecsize %d, numstates %d, ceporder %d, "
                   "numvuv %d, numdeltas %d, meanpow %d, bigpow %d",
                   pdfmul->numframes, pdfmul->vecsize, pdfmul->numstates,
                   pdfmul->ceporder, pdfmul->numvuv, pdfmul->numdeltas,
                   pdfmul->meanpow, pdfmul->bigpow));
    if ((picoos_uint32)(pos + (pdfmul->numframes * pdfmul->vecsize)) != this->size) {
        PICODBG_DEBUG(("header-spec size %d, kb-size %d",
                       pos + (pdfmul->numframes * pdfmul->vecsize),
                       this->size));
        return picoos_emRaiseException(common->em, PICO_EXC_FILE_CORRUPT,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("mul pdf initialized"));
    return PICO_OK;
}

static pico_status_t kpdfPHSInitialize(register picoknow_KnowledgeBase this,
                                       picoos_Common common) {
    picokpdf_pdfphs_t *pdfphs;
    picoos_uint16 pos;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    pdfphs = (picokpdf_pdfphs_t *)this->subObj;

    pos = 0;

    pdfphs->numvectors = ((picoos_uint16)(this->base[pos+1])) << 8 |
        this->base[pos];
    pos += 2;
    pdfphs->indexBase = &(this->base[pos]);
    pdfphs->contentBase = pdfphs->indexBase + pdfphs->numvectors * sizeof(picoos_uint32);
    PICODBG_DEBUG(("phs pdf initialized"));
    return PICO_OK;
}



static pico_status_t kpdfMULSubObjDeallocate(register picoknow_KnowledgeBase this,
                                          picoos_MemoryManager mm) {


    picokpdf_pdfmul_t *pdfmul;

    if ((NULL != this) && (NULL != this->subObj)) {
        pdfmul = (picokpdf_pdfmul_t *)this->subObj;
        picoos_deallocate(mm,(void *) &(pdfmul->meanpowUm));
        picoos_deallocate(mm,(void *) &(pdfmul->ivarpow));
        picoos_deallocate(mm, (void *) &(this->subObj));
    }
    return PICO_OK;
}

static pico_status_t kpdfDURSubObjDeallocate(register picoknow_KnowledgeBase this,
                                          picoos_MemoryManager mm) {
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}

static pico_status_t kpdfPHSSubObjDeallocate(register picoknow_KnowledgeBase this,
                                          picoos_MemoryManager mm) {
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}

/* we don't offer a specialized constructor for a *KnowledgeBase but
 * instead a "specializer" of an allready existing generic
 * picoknow_KnowledgeBase */

pico_status_t picokpdf_specializePdfKnowledgeBase(picoknow_KnowledgeBase this,
                                          picoos_Common common,
                                          const picokpdf_kpdftype_t kpdftype) {
    pico_status_t status;

    if (NULL == this) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    switch (kpdftype) {
        case PICOKPDF_KPDFTYPE_DUR:
            this->subDeallocate = kpdfDURSubObjDeallocate;
            this->subObj = picoos_allocate(common->mm,sizeof(picokpdf_pdfdur_t));
            if (NULL == this->subObj) {
                return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                               NULL, NULL);
            }
            status = kpdfDURInitialize(this, common);
            break;
        case PICOKPDF_KPDFTYPE_MUL:
            this->subDeallocate = kpdfMULSubObjDeallocate;
            this->subObj = picoos_allocate(common->mm,sizeof(picokpdf_pdfmul_t));
            if (NULL == this->subObj) {
                return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                               NULL, NULL);
            }
            status = kpdfMULInitialize(this, common);
            break;
        case PICOKPDF_KPDFTYPE_PHS:
            this->subDeallocate = kpdfPHSSubObjDeallocate;
            this->subObj = picoos_allocate(common->mm,sizeof(picokpdf_pdfphs_t));
            if (NULL == this->subObj) {
                return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                               NULL, NULL);
            }
            status = kpdfPHSInitialize(this, common);
            break;

        default:
            return picoos_emRaiseException(common->em, PICO_ERR_OTHER,
                                           NULL, NULL);
    }

    if (status != PICO_OK) {
        picoos_deallocate(common->mm, (void *) &this->subObj);
        return picoos_emRaiseException(common->em, status, NULL, NULL);
    }
    return PICO_OK;
}


/* ************************************************************/
/* pdf getPdf* */
/* ************************************************************/

picokpdf_PdfDUR picokpdf_getPdfDUR(picoknow_KnowledgeBase this) {
    return ((NULL == this) ? NULL : ((picokpdf_PdfDUR) this->subObj));
}

picokpdf_PdfMUL picokpdf_getPdfMUL(picoknow_KnowledgeBase this) {
    return ((NULL == this) ? NULL : ((picokpdf_PdfMUL) this->subObj));
}

picokpdf_PdfPHS picokpdf_getPdfPHS(picoknow_KnowledgeBase this) {
    return ((NULL == this) ? NULL : ((picokpdf_PdfPHS) this->subObj));
}


#ifdef __cplusplus
}
#endif


/* end */
