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
 * @file picocep.c
 *
 * Phonetic to Acoustic Mapping PU - Implementation
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

/**
 * @addtogroup picocep
 * <b> Pico Cepstral Smoothing </b>\n
 *
 itemtype, iteminfo1, iteminfo2, content -> TYPE(INFO1,INFO2)content
 in the following

 items input

 processed:

 - PHONE(PHONID,StatesPerPhone)[FramesPerState|f0Index|mgcIndex]{StatesPerPhone}

 (StatesPerPhone is a constant (5) for the time being. The content size is therfore allways 34)

 unprocessed:
 - all other item types are forwarded through the PU without modification

 items output

 FRAME(PHONID,F0 [?])ceps{25}

 each PHONE produces at least StatesPerPhone FRAME (if each state has FramesPerState == 1), but usually more.

 minimal input size (before processing starts)

 other limitations

 */
#include "picodefs.h"
#include "picoos.h"
#include "picodbg.h"
#include "picodata.h"
#include "picokpdf.h"
#include "picodsp.h"
#include "picocep.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#define PICOCEP_MAXWINLEN 10000  /* maximum number of frames that can be smoothed, i.e. maximum sentence length */
#define PICOCEP_MSGSTR_SIZE 32
#define PICOCEP_IN_BUFF_SIZE PICODATA_BUFSIZE_DEFAULT

#define PICOCEP_OUT_DATA_FORMAT PICODATA_ITEMINFO1_FRAME_PAR_DATA_FORMAT_FIXED /* we output coefficients as fixed point values */

#define PICOCEP_STEPSTATE_COLLECT         0
#define PICOCEP_STEPSTATE_PROCESS_PARSE   1
#define PICOCEP_STEPSTATE_PROCESS_SMOOTH  2
#define PICOCEP_STEPSTATE_PROCESS_FRAME   3
#define PICOCEP_STEPSTATE_FEED            4

#define PICOCEP_LFZINVPOW 31  /* cannot be higher than 31 because 1<<invpow must fit in uint32 */
#define PICOCEP_MGCINVPOW 24
#define PICOCEP_LFZDOUBLEDEC 1
#define PICOCEP_MGCDOUBLEDEC 0

typedef enum picocep_WantMeanOrIvar
{
    PICOCEP_WANTMEAN, PICOCEP_WANTIVAR
} picocep_WantMeanOrIvar_t;

typedef enum picocep_WantStaticOrDeltax
{
    PICOCEP_WANTSTATIC, PICOCEP_WANTDELTA, PICOCEP_WANTDELTA2
} picocep_WantStaticOrDelta_t;

/*
 *   Fixedpoint arithmetic (might go into a separate module if general enough and needed by other modules)
 */

#if defined(PICO_DEBUG) || defined(PICO_DEVEL_MODE)
int numlongmult = 0, numshortmult = 0;
#endif

#define POW1 (0x1)
#define POW2 (0x2)
#define POW3 (0x4)
#define POW4 (0x8)
#define POW5 (0x10)
#define POW6 (0x20)
#define POW7 (0x40)
#define POW8 (0x80)
#define POW9 (0x100)
#define POW10 (0x200)
#define POW11 (0x400)
#define POW12 (0x800)
#define POW13 (0x1000)
#define POW14 (0x2000)
#define POW15 (0x4000)
#define POW16 (0x8000)
#define POW17 (0x10000)
#define POW18 (0x20000)
#define POW19 (0x40000)
#define POW20 (0x80000)
#define POW21 (0x100000)
#define POW22 (0x200000)
#define POW23 (0x400000)
#define POW24 (0x800000)
#define POW25 (0x1000000)
#define POW26 (0x2000000)
#define POW27 (0x4000000)
#define POW28 (0x8000000)
#define POW29 (0x10000000)
#define POW30 (0x20000000)
#define POW31 (0x40000000)

/* item num restriction: maximum number of extended item heads in headx */
#define PICOCEP_MAXNR_HEADX    60
/* item num restriction: maximum size of all item contents together in cont */
#define PICOCEP_MAXSIZE_CBUF 7680 /* (128 * PICOCEP_MAXNR_HEADX) */

typedef struct
{
    picodata_itemhead_t head;
    picoos_uint16 cind;
    picoos_uint16 frame; /* sync position */
} picoacph_headx_t;

/*----------------------------------------------------------
 //    Name    :    cep_subobj
 //    Function:    subobject definition for the cep processing
 //    Shortcut:    cep
 //---------------------------------------------------------*/
typedef struct cep_subobj
{
    /*----------------------PU voice management------------------------------*/
    /* picorsrc_Voice voice; */
    /*----------------------PU state management------------------------------*/
    picoos_uint8 procState; /* where to take up work at next processing step */
    picoos_bool needMoreInput; /* more data necessary to start processing   */
    /* picoos_uint8 force; *//* forced processing (needMoreData but buffers full */
    picoos_uint8 sentenceEnd;
    picoos_uint8 feedFollowState;
    picoos_bool inIgnoreState;
    /*----------------------PU input management------------------------------*/
    picoos_uint8 inBuf[PICODATA_MAX_ITEMSIZE]; /* internal input buffer */
    picoos_uint16 inBufSize; /* actually allocated size */
    picoos_uint16 inReadPos, inWritePos; /* next pos to read/write from/to inBuf*/
    picoos_uint16 nextInPos;

    picoacph_headx_t headx[PICOCEP_MAXNR_HEADX];
    picoos_uint16 headxBottom; /* bottom */
    picoos_uint16 headxWritePos; /* next free position; headx is empty if headxBottom == headxWritePos */

    picoos_uint8 cbuf[PICOCEP_MAXSIZE_CBUF];
    picoos_uint16 cbufBufSize; /* actually allocated size */
    picoos_uint16 cbufWritePos; /* length, 0 if empty */

    /*----------------------PU output management-----------------------------*/
    picodata_itemhead_t framehead;
    picoos_uint8 outBuf[PICODATA_MAX_ITEMSIZE]; /* internal output buffer (one item) */
    picoos_uint16 outBufSize; /* allocated outBuf size */
    picoos_uint16 outReadPos, outWritePos; /* next pos to read/write from/to outBuf*/

    picoos_uint32 nNumFrames;
    /*---------------------- other working variables ---------------------------*/

    picoos_int32 diag0[PICOCEP_MAXWINLEN], diag1[PICOCEP_MAXWINLEN],
            diag2[PICOCEP_MAXWINLEN], WUm[PICOCEP_MAXWINLEN],
            invdiag0[PICOCEP_MAXWINLEN];

    /*---------------------- constants --------------------------------------*/
    picoos_int32 xi[5], x1[2], x2[3], xm[3], xn[2];
    picoos_int32 xsqi[5], xsq1[2], xsq2[3], xsqm[3], xsqn[2];

    picoos_uint32 scmeanpowLFZ, scmeanpowMGC;
    picoos_uint32 scmeanLFZ, scmeanMGC;

    /*---------------------- indices --------------------------------------*/
    /* index buffer to hold indices as input for smoothing */
    picoos_uint16 indicesLFZ[PICOCEP_MAXWINLEN];
    picoos_uint16 indicesMGC[PICOCEP_MAXWINLEN];
    picoos_uint16 indexReadPos, indexWritePos;
    picoos_uint16 activeEndPos; /* end position of indices to be considered */

    /* this is used for input and output */
    picoos_uint8 phoneId[PICOCEP_MAXWINLEN]; /* synchronised with indexReadPos */

    /*---------------------- coefficients --------------------------------------*/
    /* output coefficients buffer */
    picoos_int16 * outF0;
    picoos_uint16 outF0ReadPos, outF0WritePos;
    picoos_int16 * outXCep;
    picoos_uint32 outXCepReadPos, outXCepWritePos;  /* uint32 needed for MAXWINLEN*ceporder > 2^16 */
    picoos_uint8 * outVoiced;
    picoos_uint16 outVoicedReadPos, outVoicedWritePos;

    /*---------------------- LINGWARE related data -------------------*/
    /* pdflfz knowledge base */
    picokpdf_PdfMUL pdflfz, pdfmgc;

} cep_subobj_t;

/**
 * picocep_highestBit
 * @brief        find the highest non-zero bit in input x
 * @remarks        this may be implemented by comparing x to powers of 2
 *                or instead of calling this function perform multiplication
 *                and consult overflow register if available on target
 * @note        implemented as a series of macros
 */

#define picocep_highestBitNZ(x) (x>=POW17?(x>=POW25?(x>=POW29?(x>=POW31?31:(x>=POW30?30:29)):(x>=POW27?(x>=POW28?28:27):(x>=POW26?26:25))):(x>=POW21?(x>=POW23?(x>=POW24?24:23):(x>=POW22?22:21)):(x>=POW19?(x>=POW20?20:19):(x>=POW18?18:17)))):(x>=POW9?(x>=POW13?(x>=POW15?(x>=POW16?16:15):(x>=POW14?14:13)):(x>=POW11?(x>=POW12?12:11):(x>=POW10?10:9))):(x>=POW5?(x>=POW7?(x>=POW8?8:7):(x>=POW6?6:5)):(x>=POW3?(x>=POW4?4:3):(x>=POW2?2:1)))))
#define picocep_highestBitU(x) (x==0?0:picocep_highestBitNZ(x))
#define picocep_highestBitS(x,zz) (x==0?0:(x<0?((zz)=(-x),picocep_highestBitNZ(zz)):picocep_highestBitNZ(x)))

/* ------------------------------------------------------------------------------
 Internal function definitions
 ---------------------------------------------------------------------------------*/

static void initSmoothing(cep_subobj_t * cep);

static picoos_int32 getFromPdf(picokpdf_PdfMUL pdf, picoos_uint32 vecstart,
        picoos_uint8 cepnum, picocep_WantMeanOrIvar_t wantMeanOrIvar,
        picocep_WantStaticOrDelta_t wantStaticOrDeltax);

static void invMatrix(cep_subobj_t * cep, picoos_uint16 N,
        picoos_int16 *smoothcep, picoos_uint8 cepnum,
        picokpdf_PdfMUL pdf, picoos_uint8 invpow, picoos_uint8 invDoubleDec);

static picoos_uint8 makeWUWandWUm(cep_subobj_t * cep, picokpdf_PdfMUL pdf,
        picoos_uint16 *indices, picoos_uint16 b, picoos_uint16 N,
        picoos_uint8 cepnum);

static void getDirect(picokpdf_PdfMUL pdf, picoos_uint16 *indices,
        picoos_uint16 activeEndPos,
        picoos_uint8 cepnum, picoos_int16 *smoothcep);

static void getVoiced(picokpdf_PdfMUL pdf, picoos_uint16 *indices,
        picoos_uint16 activeEndPos,
        picoos_uint8 *smoothcep);

static picoos_uint16 get_pi_uint16(picoos_uint8 * buf, picoos_uint16 *pos);

static void treat_phone(cep_subobj_t * cep, picodata_itemhead_t * ihead);

static picoos_uint8 forwardingItem(picodata_itemhead_t * ihead);

static picodata_step_result_t cepStep(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * numBytesOutput);

/* --------------------------------------------
 *   generic PU management
 * --------------------------------------------
 */

/**
 * initialization of a cep PU (processing unit)
 * @param    this : handle to a cep PU struct
 * @return  PICO_OK : init succeded
 * @return  PICO_ERR_OTHER : init failed
 * @callgraph
 * @callergraph
 */
static pico_status_t cepInitialize(register picodata_ProcessingUnit this, picoos_int32 resetMode)
{
    /*pico_status_t nRes;*/
    cep_subobj_t * cep;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    cep = (cep_subobj_t *) this->subObj;
    /* inBuf */
    cep->inBufSize = PICODATA_BUFSIZE_CEP;
    cep->inReadPos = 0;
    cep->inWritePos = 0;
    /* headx and cbuf */
    cep->headxBottom = cep->headxWritePos = 0;
    cep->cbufBufSize = PICOCEP_MAXSIZE_CBUF;
    cep->cbufWritePos = 0;
    /* outBuf */
    cep->outBufSize = PICODATA_MAX_ITEMSIZE;
    cep->outReadPos = 0;
    cep->outWritePos = 0;
    /* indices* */
    cep->indexReadPos = 0;
    cep->indexWritePos = 0;
    /* outCep, outF0, outVoiced */
    cep->outXCepReadPos = 0;
    cep->outXCepWritePos = 0;
    cep->outVoicedReadPos = 0;
    cep->outVoicedWritePos = 0;
    cep->outF0ReadPos = 0;
    cep->outF0WritePos = 0;

    cep->needMoreInput = 0;
    cep->inIgnoreState = 0;
    cep->sentenceEnd = FALSE;
    cep->procState = PICOCEP_STEPSTATE_COLLECT;

    cep->nNumFrames = 0;

    /*-----------------------------------------------------------------
     * MANAGE Item I/O control management
     ------------------------------------------------------------------*/
    cep->activeEndPos = PICOCEP_MAXWINLEN;

    if (resetMode == PICO_RESET_FULL) {
        /* kb pdflfz */
        cep->pdflfz = picokpdf_getPdfMUL(
                this->voice->kbArray[PICOKNOW_KBID_PDF_LFZ]);

        /* kb pdfmgc */
        cep->pdfmgc = picokpdf_getPdfMUL(
                this->voice->kbArray[PICOKNOW_KBID_PDF_MGC]);

        /* kb tab phones */
        /* cep->phones =
         picoktab_getPhones(this->voice->kbArray[PICOKNOW_KBID_TAB_PHONES]); */

        /*---------------------- other working variables ---------------------------*/
        /* define the (constant) FRAME_PAR item header */
        cep->framehead.type = PICODATA_ITEM_FRAME_PAR;
        cep->framehead.info1 = PICOCEP_OUT_DATA_FORMAT;
        cep->framehead.info2 = cep->pdfmgc->ceporder;
        cep->framehead.len = sizeof(picoos_uint16) + (cep->framehead.info2 + 4)
                * sizeof(picoos_uint16);
        cep->scmeanpowLFZ = cep->pdflfz->bigpow - cep->pdflfz->meanpow;
        cep->scmeanpowMGC = cep->pdfmgc->bigpow - cep->pdfmgc->meanpow;

        cep->scmeanLFZ = (1 << (picoos_uint32) cep->scmeanpowLFZ);

        cep->scmeanMGC = (1 << (picoos_uint32) cep->scmeanpowMGC);

    }
    /* constants used in makeWUWandWUm */
    initSmoothing(cep);


    return PICO_OK;
}/*cepInitialize*/

/**
 * termination of a cep PU (processing unit)
 * @param    this : handle to a cep PU struct
 * @return  PICO_OK : termination succeded
 * @return  PICO_ERR_OTHER : termination failed
 * @callgraph
 * @callergraph
 */
static pico_status_t cepTerminate(register picodata_ProcessingUnit this)
{
    return PICO_OK;
}

/**
 * deallocation of a cep PU internal sub object
 * @param    this : handle to a cep PU struct
 * @param    mm : handle of the engine memory manager
 * @return  PICO_OK : deallocation succeded
 * @return  PICO_ERR_OTHER : deallocation failed
 * @callgraph
 * @callergraph
 */
static pico_status_t cepSubObjDeallocate(register picodata_ProcessingUnit this,
        picoos_MemoryManager mm)
{

    mm = mm; /* avoid warning "var not used in this function"*/
#if defined(PICO_DEVEL_MODE)
    printf("number of long mult is %d, number of short mult is %i\n",numlongmult,numshortmult);
#else
    PICODBG_INFO_MSG(("number of long mult is %d, number of short mult is %i\n",numlongmult,numshortmult));
#endif
    if (NULL != this) {
        cep_subobj_t * cep = (cep_subobj_t *) this->subObj;
        picoos_deallocate(this->common->mm, (void *) &cep->outXCep);
        picoos_deallocate(this->common->mm, (void *) &cep->outVoiced);
        picoos_deallocate(this->common->mm, (void *) &cep->outF0);
        picoos_deallocate(this->common->mm, (void *) &this->subObj);
    }
    return PICO_OK;
}

/**
 * creates a new cep PU (processing unit)
 * @param    mm : engine memory manager object pointer
 * @param    common : engine common object pointer
 * @param    cbIn : PU input buffer
 * @param    cbOut : PU output buffer
 * @param    voice : the voice descriptor object
 * @return  a valid PU handle if creation succeded
 * @return  NULL : creation failed
 * @callgraph
 * @callergraph
 */
picodata_ProcessingUnit picocep_newCepUnit(picoos_MemoryManager mm,
        picoos_Common common, picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut, picorsrc_Voice voice)
{
    picodata_ProcessingUnit this = picodata_newProcessingUnit(mm, common, cbIn,
            cbOut, voice);
    cep_subobj_t * cep;

    if (this == NULL) {
        return NULL;
    }
    this->initialize = cepInitialize;

    PICODBG_DEBUG(("set this->step to cepStep"));

    this->step = cepStep;
    this->terminate = cepTerminate;
    this->subDeallocate = cepSubObjDeallocate;
    this->subObj = picoos_allocate(mm, sizeof(cep_subobj_t));

    cep = (cep_subobj_t *) this->subObj;

    if (this->subObj == NULL) {
        picoos_deallocate(mm, (void*) &this);
        return NULL;
    };

    /* allocate output coeeficient buffers */
    cep->outF0 = (picoos_int16 *) picoos_allocate(this->common->mm,
            PICOCEP_MAXWINLEN * PICOKPDF_MAX_MUL_LFZ_CEPORDER
                    * sizeof(picoos_int16));
    cep->outXCep = (picoos_int16 *) picoos_allocate(this->common->mm,
            PICOCEP_MAXWINLEN * PICOKPDF_MAX_MUL_MGC_CEPORDER
                    * sizeof(picoos_int16));
    cep->outVoiced = (picoos_uint8 *) picoos_allocate(this->common->mm,
            PICOCEP_MAXWINLEN * sizeof(picoos_uint8));

    if ((NULL == cep->outF0) || (NULL == cep->outXCep) || (NULL
            == cep->outVoiced)) {
        picoos_deallocate(this->common->mm, (void *) &(cep->outF0));
        picoos_deallocate(this->common->mm, (void *) &(cep->outXCep));
        picoos_deallocate(this->common->mm, (void *) &(cep->outVoiced));
        picoos_deallocate(mm, (void*) &cep);
        picoos_deallocate(mm, (void*) &this);
        return NULL;
    }
    cepInitialize(this, PICO_RESET_FULL);

    return this;
}/*picocep_newCepUnit*/

/* --------------------------------------------
 *   processing and internal functions
 * --------------------------------------------
 */

/**
 * multiply by 1<<pow and check overflow
 * @param    a : input value
 * @param    pow : shift value
 * @return  multiplied value
 * @callgraph
 * @callergraph
 */
static picoos_int32 picocep_fixptmultpow(picoos_int32 a, picoos_uint8 pow)
{
    picoos_int32 b;
    picoos_int32 zzz;

    if (picocep_highestBitS(a,zzz) + pow < 32) {
        b = a << pow;
    } else {
        /* clip to maximum positive or negative value */
        b = 1 << 31; /* maximum negative value */
        if (a > 0) {
            b -= 1; /* maximum positive value */
        }PICODBG_WARN(("picocep_fixptmultpow warning: overflow in fixed point multiplication %i*1<<%i.  Clipping to %i\n", a, pow, b));
    }
    return b;
}

/**
 * divide by 1<<pow with rounding
 * @param    a : input value
 * @param    pow : shift value
 * @return  divided value
 * @callgraph
 * @callergraph
 */
static picoos_int32 picocep_fixptdivpow(picoos_int32 a, picoos_uint8 pow)
{
    picoos_int32 big;

    if (a == 0) {
        return a;
    }
    big = 1 << (pow - 1);
    if (a > 0) {
        a = (a + big) >> pow;
    } else {
        a = -1 * ((-1 * a + big) >> pow);
    }

    return a;
}

/**
 * fixed point multiplication of x and y for large values of x or y or both
 * @param    x,y  : operands 1 & 2, in fixed point S:M:N representation
 * @param    bigpow (int) : normalization factor=2**N, where N=number of binary decimal digits
 * @param    invDoubleDec : boolean indicating that x has double decimal size.
 *             do extra division by 1<<bigpow so that result has again single decimal size
 * @return  z(int) : result, in fixed point S:M:N representation
 * @callgraph
 * @callergraph
 */
static picoos_int32 picocep_fixptmultdouble(picoos_int32 x, picoos_int32 y,
        picoos_uint8 bigpow, picoos_uint8 invDoubleDec)
{
    picoos_int32 a, b, c, d, e, z;
    picoos_int32 big;

    big = 1 << bigpow;

    /* a = floor(x/big); */
    if (x >= 0) {
        a = x >> bigpow;
        b = x - (a << bigpow);
    } else {
        a = -1 * ((x * -1) >> bigpow); /* most significant 2 bytes of x */
        b = x - (a << bigpow);
    }

    /* least significant 2 bytes of x i.e. x modulo big */
    /* c = floor(y/big); */
    if (y >= 0) {
        c = y >> bigpow;
        d = y - (c << bigpow);
    } else {
        c = -1 * ((y * -1) >> bigpow);
        d = y - (c << bigpow);
    }

    if (invDoubleDec == 1) {
        e = a * d + b * c + picocep_fixptdivpow(b * d, bigpow);
        z = a * c + picocep_fixptdivpow(e, bigpow);
    } else {
        z = ((a * c) << bigpow) + (a * d + b * c) + picocep_fixptdivpow(b * d,
                bigpow); /* 4 mult and 3 add instead of 1 mult. */
    }

    return z;
}

/**
 * fixed point multiplication of x and y
 * @param    x,y : operands 1 & 2, in fixed point S:M:N representation
 * @param    bigpow (int) : normalization factor=2**N, where N=number of binary decimal digits
 * @param    invDoubleDec : boolean indicating that x has double decimal size.
 *             do extra division by 1<<bigpow so that result has again single decimal size
 * @return  z(int) : result, in fixed point S:M:N representation
 * Notes
 * - input and output values are 32 bit signed integers
 *   meant to represent a S.M.N encoding of a floating point value where
 *   - S : 1 sign bit
 *   - M : number of binary integer digits (M=32-1-N)
 *   - N : number of binary decimal digits (N=log2(big))
 *   the routine supports 2 methods
 * -# standard multiplication of x and y
 * -# long multiplication of x and y
 * under PICO_DEBUG the number of double and single precision multiplications is monitored for accuracy/performance tuning
 * Calls
 * - picocep_highestBit
 * - picocep_fixptmultdouble
 * @callgraph
 * @callergraph
 */
static picoos_int32 picocep_fixptmult(picoos_int32 x, picoos_int32 y,
        picoos_uint8 bigpow, picoos_uint8 invDoubleDec)
{
    picoos_int32 z;
    picoos_uint8 multsz, pow;
    picoos_int32 zz1, zz2;

    /* in C, the evaluation order of f() + g() is not defined, so
     * if both have a side effect on e.g. zz, the outcome of zz is not defined.
     * For that reason, picocep_highestBitS(x,zz) + picocep_highestBitS(y,zz)
     * would generate a warning "operation on zz may be undefined" which we
     * avoid by using two different variables zz1 and zz2 */
    multsz = picocep_highestBitS(x,zz1) + picocep_highestBitS(y,zz2);
    pow = bigpow;
    if (invDoubleDec == 1) {
        pow += bigpow;
    }

    if (multsz <= 30) { /* x*y < 1<<30 is safe including rounding in picocep_fixptdivpow, x*y < 1<<31 is safe but not with rounding */
        /* alternatively perform multiplication and consult overflow register */
        z = picocep_fixptdivpow(x * y, pow);
#if defined(PICO_DEBUG)
        numshortmult++; /*  keep track of number of short multiplications */
#endif
    } else {
#if defined(PICO_DEBUG)
        if (multsz> 31 + pow) {
            PICODBG_WARN(("picocep_fixptmult warning: overflow in fixed point multiplication %i*%i, multsz = %i, pow = %i, decrease bigpow\n", x, y, multsz, pow));
        }
#endif
        z = picocep_fixptmultdouble(x, y, bigpow, invDoubleDec); /*  perform long multiplication for large x and y */
#if defined(PICO_DEBUG)
        numlongmult++; /*  keep track of number of long multiplications */
#endif
    }
    return z;
}/* picocep_fixptmult */

/**
 * fixed point ^division of a vs b
 * @param    a,b : operands 1 & 2, in fixed point S:M:N representation
 * @param    bigpow (int) : normalization factor=2**N, where N=number of binary decimal digits
 * @return  z(int) : result, in fixed point S:M:N representation
 * Notes
 * - input and output values are 32 bit signed integers
 *   meant to represent a S.M.N encoding of a floating point value where
 *   - S : 1 sign bit
 *   - M : number of binary integer digits (M=32-1-N)
 *   - N : number of binary decimal digits (N=log2(big))
 * - standard implementation of division by iteratively calculating
 * the remainder and setting corresponding bit
 * calculate integer part by integer division (DIV),
 * then add X bits of precision in decimal part
 * @callgraph
 * @callergraph
 */
static picoos_int32 picocep_fixptdiv(picoos_int32 a, picoos_int32 b,
        picoos_uint8 bigpow)
{
    picoos_int32 r, c, f, h, stop;
    r = (a < 0) ? -a : a; /* take absolute value; b is already guaranteed to be positive in smoothing operation */
    if (r == 0) {
        return 0;
    }
    c = 0;
    stop = 0;

    /* can speed up slightly by setting stop = 2 => slightly less precision */
    h = r / b; /* in first loop h can be multiple bits, after first loop h can only be 0 or 1 */
    /* faster implementation on target by using comparison instead of DIV? */
    /* For our LDL even in first loop h <= 1, but not in backward step */
    c = c + (h << bigpow); /* after first loop simply set bit */
    r = r - h * b; /* corresponds to modulo operation */
    bigpow--;
    r <<= 1;

    while ((bigpow > stop) && (r != 0)) { /* calculate bigpow bits after fixed point */
        /* can speed up slightly by setting stop = 2 => slightly less precision */
        if (r >= b) {
            c += (1 << bigpow); /* after first loop simply set bit */
            r -= b; /* corresponds to modulo operation */
        }
        bigpow--;
        r <<= 1;
    }

    if (r != 0) {
        f = r + (b >> 1);
        if (f >= b) {
            if (f >= b + b) {
                c += 2;
            } else {
                c++;
            }
        }
    }
    /* final step: do rounding (experimentally improves accuracy for our problem) */
    c = (a >= 0) ? c : -c; /* b is guaranteed to be positive because corresponds to diag0 */
    return c;
}/* picocep_fixptdiv */

/**
 * perform inversion of diagonal element of WUW matrix
 * @param    d : diagonal element to be inverted
 * @param    rowscpow (int) : fixed point base for each dimension of the vectors stored in the database
 * @param    bigpow (int) : fixed point base used during cepstral smoothing
 * @param    invpow : fixed point base of inverted pivot elements
 * @return   inverted pivot element
 * @note
 * - d is guaranteed positive
 * @callgraph
 * @callergraph
 */
static picoos_int32 picocep_fixptInvDiagEle(picoos_uint32 d,
        picoos_uint8* rowscpow, picoos_uint8 bigpow, picoos_uint8 invpow)
{
    picoos_uint32 r, b, c, h, f, stop;
    picoos_uint8 dlen;
    /* picoos_int32 zz; */
    c = 0;
    stop = 0;

    dlen = picocep_highestBitU(d);
    if (invpow + bigpow > 30 + dlen) { /* c must be < 2^32, hence d which is >= 2^(dlen-1) must be > 2^(invpow+bigpow-32), or invpow+bigpow must be <= dlen+30*/
        *rowscpow = invpow + bigpow - 30 - dlen;PICODBG_DEBUG(("input to picocep_fixptInvDiagEle is %i <= 1<<%i = 1<<invpow+bigpow-32. Choose lower invpow. For now scaling row by 1<<%i\n", d, invpow+bigpow-32, *rowscpow));
    } else {
        *rowscpow = 0;
    }
    r = 1 << invpow;
    b = d << (*rowscpow);

    /* first */
    h = r / b;
    if (h > 0) {
        c += (h << bigpow);
        r -= h * b;
    }
    bigpow--;
    r <<= 1;

    /* loop */
    while ((bigpow > stop) && (r != 0)) {
        if (r >= b) {
            c += (1 << bigpow);
            r -= b;
        }
        bigpow--;
        r <<= 1;
    }

    if (r != 0) {
        f = r + (b >> 1);
        if (f >= b) {
            if (f >= b + b) {
                c += 2;
            } else {
                c++;
            }
        }
    }

    return c;
}/* picocep_fixptInvDiagEle */

/**
 * perform division of two operands a and b by multiplication by inverse of b
 * @param    a (int32) : operand 1 in fixed point S:M:N representation
 * @param    invb(uint32)   : inverse of operand b, in fixed point P:Q representation (sign is positive)
 * @param    bigpow(uint8)  : N = bigpow when invDoubleDec==0, else N = 2*bigpow
 * @param    invpow(uint8)  : Q = invpow = number of binary decimal digits for invb
 * @param      invDoubleDec   : boolean to indicate that a and the return value c have 2*N binary decimal digits instead of N
 * @return  c(int32)       : result in fixed point S:v:w where w = 2*N when invDoubleDec == 1
 * @note Calls
 * - picocep_fixptmult
 * @callgraph
 * @callergraph
 */
static picoos_int32 picocep_fixptinv(picoos_int32 a, picoos_uint32 invb,
        picoos_uint8 bigpow, picoos_uint8 invpow, picoos_uint8 invDoubleDec)
{
    picoos_int32 c;
    picoos_int8 normpow;

    c = picocep_fixptmult(a, invb, bigpow, invDoubleDec);

    /* if invDoubleDec==0, picocep_fixptmult assumes a and invb are in base 1<<bigpow and returns c = (a*b)/1<<bigpow
     Since invb is in base 1<<invpow instead of 1<<bigpow, normalize c by 1<<(bigpow-invpow)
     if invDoubleDec==1:
     multiply additionally by 1<<bigpow*2 (for invb and c) so that base of c is again 2*bigpow
     this can be seen by setting a=A*big, b=B*big, invb=big2/B, mult(a,invb) = a*invb/(big*big) = A/B*big*big2/(big*big) = A/B*big2/big
     and we want c = A/B*big*big => normfactor = big^3/big2
     */
    if (invDoubleDec == 1) {
        normpow = 3 * bigpow;
    } else {
        normpow = bigpow;
    }
    if (normpow < invpow) {
        /* divide with rounding */
        c = picocep_fixptdivpow(c, invpow - normpow);
    } else {
        c = picocep_fixptmultpow(c, normpow - invpow);
    }
    return c;
}

/**
 * initializes the coefficients to calculate delta and delta-delta values and the squares of the coefficients
 * @param    cep : the CEP PU sub-object handle
 * @callgraph
 * @callergraph
 */
static void initSmoothing(cep_subobj_t * cep)
{
    cep->xi[0] = 1;
    cep->xi[1] = -1;
    cep->xi[2] = 2;
    cep->xi[3] = -4;
    cep->xi[4] = 2;
    cep->xsqi[0] = 1;
    cep->xsqi[1] = 1;
    cep->xsqi[2] = 4;
    cep->xsqi[3] = 16;
    cep->xsqi[4] = 4;

    cep->x1[0] = -1;
    cep->x1[1] = 2;
    cep->xsq1[0] = 1;
    cep->xsq1[1] = 4;

    cep->x2[0] = -1;
    cep->x2[1] = -4;
    cep->x2[2] = 2;
    cep->xsq2[0] = 1;
    cep->xsq2[1] = 16;
    cep->xsq2[2] = 4;

    cep->xm[0] = 1;
    cep->xm[1] = 2;
    cep->xm[2] = -4;
    cep->xsqm[0] = 1;
    cep->xsqm[1] = 4;
    cep->xsqm[2] = 16;

    cep->xn[0] = 1;
    cep->xn[1] = 2;
    cep->xsqn[0] = 1;
    cep->xsqn[1] = 4;
}

/**
 * matrix inversion
 * @param    cep : PU sub object pointer
 * @param    N
 * @param    smoothcep : pointer to picoos_int16, sequence of smoothed cepstral vectors
 * @param    cepnum :  cepstral dimension to be treated
 * @param    pdf :  pdf resource
 * @param    invpow :  fixed point base for inverse
 * @param    invDoubleDec : boolean indicating that result of picocep_fixptinv has fixed point base 2*bigpow
 *             picocep_fixptmult absorbs double decimal size by dividing its result by extra factor big
 * @return  void
 * @remarks diag0, diag1, diag2, WUm, invdiag0  globals needed in this function (object members in pico)
 * @callgraph
 * @callergraph
 */
static void invMatrix(cep_subobj_t * cep, picoos_uint16 N,
        picoos_int16 *smoothcep, picoos_uint8 cepnum,
        picokpdf_PdfMUL pdf, picoos_uint8 invpow, picoos_uint8 invDoubleDec)
{
    picoos_int32 j, v1, v2, h;
    picoos_uint32 k;
    picoos_uint8 rowscpow, prevrowscpow;
    picoos_uint8 ceporder = pdf->ceporder;
    picoos_uint8 bigpow = pdf->bigpow;
    picoos_uint8 meanpow = pdf->meanpow;

    /* LDL factorization */
    prevrowscpow = 0;
    cep->invdiag0[0] = picocep_fixptInvDiagEle(cep->diag0[0], &rowscpow,
            bigpow, invpow); /* inverse has fixed point basis 1<<invpow */
    cep->diag1[0] = picocep_fixptinv((cep->diag1[0]) << rowscpow,
            cep->invdiag0[0], bigpow, invpow, invDoubleDec); /* perform division via inverse */
    cep->diag2[0] = picocep_fixptinv((cep->diag2[0]) << rowscpow,
            cep->invdiag0[0], bigpow, invpow, invDoubleDec);
    cep->WUm[0] = (cep->WUm[0]) << rowscpow; /* if diag0 too low, multiply LHS and RHS of row in matrix equation by 1<<rowscpow */
    for (j = 1; j < N; j++) {
        /* do forward substitution */
        cep->WUm[j] = cep->WUm[j] - picocep_fixptmult(cep->diag1[j - 1],
                cep->WUm[j - 1], bigpow, invDoubleDec);
        if (j > 1) {
            cep->WUm[j] = cep->WUm[j] - picocep_fixptmult(cep->diag2[j - 2],
                    cep->WUm[j - 2], bigpow, invDoubleDec);
        }

        /* update row j */
        v1 = picocep_fixptmult((cep->diag1[j - 1]) / (1 << rowscpow),
                cep->diag0[j - 1], bigpow, invDoubleDec); /* undo scaling by 1<<rowscpow because diag1(j-1) refers to symm ele in column j-1 not in row j-1 */
        cep->diag0[j] = cep->diag0[j] - picocep_fixptmult(cep->diag1[j - 1],
                v1, bigpow, invDoubleDec);
        if (j > 1) {
            v2 = picocep_fixptmult((cep->diag2[j - 2]) / (1 << prevrowscpow),
                    cep->diag0[j - 2], bigpow, invDoubleDec); /* undo scaling by 1<<prevrowscpow because diag1(j-2) refers to symm ele in column j-2 not in row j-2 */
            cep->diag0[j] = cep->diag0[j] - picocep_fixptmult(
                    cep->diag2[j - 2], v2, bigpow, invDoubleDec);
        }
        prevrowscpow = rowscpow;
        cep->invdiag0[j] = picocep_fixptInvDiagEle(cep->diag0[j], &rowscpow,
                bigpow, invpow); /* inverse has fixed point basis 1<<invpow */
        cep->WUm[j] = (cep->WUm[j]) << rowscpow;
        if (j < N - 1) {
            h = picocep_fixptmult(cep->diag2[j - 1], v1, bigpow, invDoubleDec);
            cep->diag1[j] = picocep_fixptinv((cep->diag1[j] - h) << rowscpow,
                    cep->invdiag0[j], bigpow, invpow, invDoubleDec); /* eliminate column j below pivot */
        }
        if (j < N - 2) {
            cep->diag2[j] = picocep_fixptinv((cep->diag2[j]) << rowscpow,
                    cep->invdiag0[j], bigpow, invpow, invDoubleDec); /* eliminate column j below pivot */
        }
    }

    /* divide all entries of WUm by diag0 */
    for (j = 0; j < N; j++) {
        cep->WUm[j] = picocep_fixptinv(cep->WUm[j], cep->invdiag0[j], bigpow,
                invpow, invDoubleDec);
        if (invDoubleDec == 1) {
            cep->WUm[j] = picocep_fixptdivpow(cep->WUm[j], bigpow);
        }
    }

    /* backward substitution */
    for (j = N - 2; j >= 0; j--) {
        cep->WUm[j] = cep->WUm[j] - picocep_fixptmult(cep->diag1[j], cep->WUm[j
                + 1], bigpow, invDoubleDec);
        if (j < N - 2) {
            cep->WUm[j] = cep->WUm[j] - picocep_fixptmult(cep->diag2[j],
                    cep->WUm[j + 2], bigpow, invDoubleDec);
        }
    }
    /* copy N frames into smoothcep (only for coeff # "cepnum")  */
    /* coefficients normalized to occupy short; for correct waveform energy, divide by (1<<(bigpow-meanpow)) then convert e.g. to picoos_single */
    k = cepnum;
    for (j = 0; j < N; j++) {
        smoothcep[k] = (picoos_int16)(cep->WUm[j]/(1<<meanpow));
        k += ceporder;
    }

}/* invMatrix*/

/**
 * Calculate matrix products needed to implement the solution
 * @param    cep : PU sub object pointer
 * @param    pdf :  pointer to picoos_uint8, sequence of pdf vectors, each vector of length 1+ceporder*2+numdeltas*3+ceporder*3
 * @param    indices : indices of pdf vectors for all frames in current sentence
 * @param    b, N :  to be smoothed frames indices (range will be from b to b+N-1)
 * @param    cepnum :  cepstral dimension to be treated
 * @return  void
 * @remarks diag0, diag1, diag2, WUm, invdiag0  globals needed in this function (object members in pico)
 * @remarks WUW --> At x W x A
 * @remarks WUm --> At x W x b
 * @callgraph
 * @callergraph
 */
static picoos_uint8 makeWUWandWUm(cep_subobj_t * cep, picokpdf_PdfMUL pdf,
        picoos_uint16 *indices, picoos_uint16 b, picoos_uint16 N,
        picoos_uint8 cepnum)
{
    picoos_uint16 Id[2], Idd[3];
    /*picoos_uint32      vecstart, k;*/
    picoos_uint32 vecstart;
    picoos_int32 *x = NULL, *xsq = NULL;
    picoos_int32 mean, ivar;
    picoos_uint16 i, j, numd = 0, numdd = 0;
    picoos_uint8 vecsize = pdf->vecsize;
    picoos_int32 prev_WUm, prev_diag0, prev_diag1, prev_diag1_1, prev_diag2;

    prev_WUm = prev_diag0 = prev_diag1 = prev_diag1_1 = prev_diag2 = 0;
    for (i = 0; i < N; i++) {

        if ((1 < i) && (i < N - 2)) {
            x = cep->xi;
            xsq = cep->xsqi;
            numd = 2;
            numdd = 3;
            Id[0] = Idd[0] = i - 1;
            Id[1] = Idd[2] = i + 1;
            Idd[1] = i;
        } else if (i == 0) {
            x = cep->x1;
            xsq = cep->xsq1;
            numd = numdd = 1;
            Id[0] = Idd[0] = 1;
        } else if (i == 1) {
            x = cep->x2;
            xsq = cep->xsq2;
            numd = 1;
            numdd = 2;
            Id[0] = Idd[1] = 2;
            Idd[0] = 1;
        } else if (i == N - 2) {
            x = cep->xm;
            xsq = cep->xsqm;
            numd = 1;
            numdd = 2;
            Id[0] = Idd[0] = N - 3;
            Idd[1] = N - 2;
        } else if (i == N - 1) {
            x = cep->xn;
            xsq = cep->xsqn;
            numd = numdd = 1;
            Id[0] = Idd[0] = N - 2;
        }

        /* process static means and static inverse variances */
        if (i > 0 && indices[b + i] == indices[b + i - 1]) {
            cep->diag0[i] = prev_diag0;
            cep->WUm[i] = prev_WUm;
        } else {
            vecstart = indices[b + i] * vecsize;
            ivar = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTIVAR,
                    PICOCEP_WANTSTATIC);
            prev_diag0 = cep->diag0[i] = ivar << 2; /* multiply ivar by 4 (4 used to be first entry of xsq) */
            mean = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTMEAN,
                    PICOCEP_WANTSTATIC);
            prev_WUm = cep->WUm[i] = mean << 1; /* multiply mean by 2 (2 used to be first entry of x) */
        }

        /* process delta means and delta inverse variances */
        for (j = 0; j < numd; j++) {
            vecstart = indices[b + Id[j]] * vecsize;
            ivar = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTIVAR,
                    PICOCEP_WANTDELTA);
            cep->diag0[i] += xsq[j] * ivar;

            mean = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTMEAN,
                    PICOCEP_WANTDELTA);
            if (mean != 0) {
                cep->WUm[i] += x[j] * mean;
            }
        }

        /* process delta delta means and delta delta inverse variances */
        for (j = 0; j < numdd; j++) {
            vecstart = indices[b + Idd[j]] * vecsize;
            ivar = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTIVAR,
                    PICOCEP_WANTDELTA2);
            cep->diag0[i] += xsq[numd + j] * ivar;

            mean = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTMEAN,
                    PICOCEP_WANTDELTA2);
            if (mean != 0) {
                cep->WUm[i] += x[numd + j] * mean;
            }
        }

        cep->diag0[i] = (cep->diag0[i] + 2) / 4; /* long DIV with rounding */
        cep->WUm[i] = (cep->WUm[i] + 1) / 2; /* long DIV with rounding */

        /* calculate diag(A,-1) */
        if (i < N - 1) {
            if (i < N - 2) {
                if (i > 0 && indices[b + i + 1] == indices[b + i]) {
                    cep->diag1[i] = prev_diag1;
                } else {
                    vecstart = indices[b + i + 1] * vecsize;
                    /*
                     diag1[i] = getFromPdf(pdf, vecstart, numvuv, ceporder, numdeltas, cepnum,
                     bigpow, meanpowUm, ivarpow, PICOCEP_WANTIVAR, PICOCEP_WANTDELTA2);
                     */
                    prev_diag1 = cep->diag1[i] = getFromPdf(pdf, vecstart,
                            cepnum, PICOCEP_WANTIVAR, PICOCEP_WANTDELTA2);
                }
                /*
                 k = vecstart +pdf->numvuv+pdf->ceporder*2 +    pdf->numdeltas*3 +
                 pdf->ceporder*2 +cepnum;
                 cep->diag1[i] = (picoos_int32)(pdf->content[k]) << pdf->bigpow;
                 */
            } else {
                cep->diag1[i] = 0;
            }
            if (i > 0) {
                if (i > 1 && indices[b + i] == indices[b + i - 1]) {
                    cep->diag1[i] += prev_diag1_1;
                } else {
                    vecstart = indices[b + i] * vecsize;
                    /*
                     k = vecstart + pdf->numvuv + pdf->ceporder * 2 + pdf->numdeltas * 3 + pdf->ceporder * 2 + cepnum;
                     cep->diag1[i] += (picoos_int32)(pdf->content[k]) << pdf->bigpow; */
                    /* cepnum'th delta delta ivar */

                    prev_diag1_1 = getFromPdf(pdf, vecstart, cepnum,
                            PICOCEP_WANTIVAR, PICOCEP_WANTDELTA2);
                    cep->diag1[i] += prev_diag1_1;
                }

            } /*i < N-1 */
            cep->diag1[i] *= -2;
        }
    }

    /* calculate diag(A,-2) */
    for (i = 0; i < N - 2; i++) {
        if (i > 0 && indices[b + i + 1] == indices[b + i]) {
            cep->diag2[i] = prev_diag2;
        } else {
            vecstart = indices[b + i + 1] * vecsize;
            /*
             k = vecstart + pdf->numvuv + pdf->ceporder * 2 + pdf->numdeltas * 3 + pdf->ceporder * 2 + cepnum;
             cep->diag2[i] = (picoos_int32)(pdf->content[k]) << pdf->bigpow;
             k -= pdf->ceporder;
             ivar = (picoos_int32)(pdf->content[k]) << pdf->bigpow;
             */
            cep->diag2[i] = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTIVAR,
                    PICOCEP_WANTDELTA2);
            ivar = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTIVAR,
                    PICOCEP_WANTDELTA);
            cep->diag2[i] -= (ivar + 2) / 4;
            prev_diag2 = cep->diag2[i];
        }
    }

    return 0;
}/* makeWUWandWUm */

/**
 * Retrieve actual values for MGC from PDF resource
 * @param    pdf :  pointer to picoos_uint8, sequence of pdf vectors, each vector of length 1+ceporder*2+numdeltas*3+ceporder*3
 * @param    vecstart : indices of pdf vectors for all frames in current sentence
 * @param    cepnum :  cepstral dimension to be treated
 * @param    wantMeanOrIvar :  flag to select mean or variance values
 * @param    wantStaticOrDeltax :  flag to select static or delta values
 * @return  the actual value retrieved
 * @callgraph
 * @callergraph
 */
static picoos_int32 getFromPdf(picokpdf_PdfMUL pdf, picoos_uint32 vecstart,
        picoos_uint8 cepnum, picocep_WantMeanOrIvar_t wantMeanOrIvar,
        picocep_WantStaticOrDelta_t wantStaticOrDeltax)
{
    picoos_uint8 s, ind;
    picoos_uint8 *p;
    picoos_uint8 ceporder, ceporder2, cc;
    picoos_uint32 k;
    picoos_int32 mean = 0, ivar = 0;

    if (pdf->numdeltas == 0xFF) {
        switch (wantMeanOrIvar) {
            case PICOCEP_WANTMEAN:
                switch (wantStaticOrDeltax) {
                    case PICOCEP_WANTSTATIC:
                        p = pdf->content
                                + (vecstart + pdf->numvuv + cepnum * 2); /* cepnum'th static mean */
                        mean = ((picoos_int32) ((picoos_int16) (*(p + 1) << 8))
                                | *p) << (pdf->meanpowUm[cepnum]);
                        break;
                    case PICOCEP_WANTDELTA:
                cc = pdf->ceporder + cepnum;
                p = pdf->content + (vecstart + pdf->numvuv + cc * 2); /* cepnum'th delta mean */
                        mean = ((picoos_int32) ((picoos_int16) (*(p + 1) << 8))
                                | *p) << (pdf->meanpowUm[cc]);
                        break;
                    case PICOCEP_WANTDELTA2:
                cc = pdf->ceporder * 2 + cepnum;
                p = pdf->content + (vecstart + pdf->numvuv + cc * 2); /* cepnum'th delta delta mean */
                        mean = ((picoos_int32) ((picoos_int16) (*(p + 1) << 8))
                                | *p) << (pdf->meanpowUm[cc]);
                        break;
                    default:
                        /* should never come here */
                PICODBG_ERROR(("unknown type wantStaticOrDeltax = %i", wantStaticOrDeltax));
            }
                return mean;
                break;
            case PICOCEP_WANTIVAR:
                switch (wantStaticOrDeltax) {
                    case PICOCEP_WANTSTATIC:
                k = vecstart + pdf->numvuv + pdf->ceporder * 6 + cepnum; /* cepnum'th static ivar */
                ivar = (picoos_int32) (pdf->content[k])
                        << (pdf->ivarpow[cepnum]);
                        break;
                    case PICOCEP_WANTDELTA:
                ceporder = pdf->ceporder;
                k = vecstart + pdf->numvuv + ceporder * 7 + cepnum; /* cepnum'th delta ivar */
                ivar = (picoos_int32) (pdf->content[k])
                        << (pdf->ivarpow[ceporder + cepnum]);
                        break;
                    case PICOCEP_WANTDELTA2:
                ceporder = pdf->ceporder;
                k = vecstart + pdf->numvuv + ceporder * 8 + cepnum; /* cepnum'th delta delta ivar */
                        ivar = (picoos_int32) (pdf->content[k])
                                << (pdf->ivarpow[2 * ceporder + cepnum]);
                        break;
                    default:
                        /* should never get here */
                PICODBG_ERROR(("unknown type wantStaticOrDeltax = %i", wantStaticOrDeltax));
            }
                return ivar;
                break;
            default:
                /* should never come here */
            PICODBG_ERROR(("unknown type wantMeanOrIvar = %n", wantMeanOrIvar));
                return 0;
        }
    } else {
        switch (wantMeanOrIvar) {
            case PICOCEP_WANTMEAN:
                switch (wantStaticOrDeltax) {
                    case PICOCEP_WANTSTATIC:
                        p = pdf->content
                                + (vecstart + pdf->numvuv + cepnum * 2); /* cepnum'th static mean */
                        mean = ((picoos_int32) ((picoos_int16) (*(p + 1) << 8))
                                | *p) << (pdf->meanpowUm[cepnum]);
                return mean;
                        break;
                    case PICOCEP_WANTDELTA:
                ceporder = pdf->ceporder;
                s = 0;
                ind = 0;
                        while ((s < pdf->numdeltas) && (ind < cepnum || (ind
                                == 0 && cepnum == 0))) { /* rawmean deltas are sparse so investigate indices in column */
                    k = vecstart + pdf->numvuv + ceporder * 2 + s; /* s'th delta index */
                    ind = (picoos_uint8) (pdf->content[k]); /* is already picoos_uint8 but just to make explicit */
                    if (ind == cepnum) {
                        k = vecstart + pdf->numvuv + ceporder * 2
                                + pdf->numdeltas + s * 2; /* s'th sparse delta mean, corresponds to cepnum'th delta mean */
                                mean
                                        = ((picoos_int32) ((picoos_int16) ((pdf->content[k
                                + 1]) << 8)) | pdf->content[k])
                                                << (pdf->meanpowUm[ceporder
                                                        + cepnum]);
                        return mean;
                    }
                    s++;
                }
                return 0;
                        break;
                    case PICOCEP_WANTDELTA2:
                ceporder = pdf->ceporder;
                ceporder2 = ceporder * 2;
                s = pdf->numdeltas;
                ind = 2 * ceporder;
                while ((s-- > 0) && (ind > ceporder + cepnum)) { /* rawmean deltas are sparse so investigate indices in column */
                    k = vecstart + pdf->numvuv + ceporder2 + s; /* s'th delta index */
                    ind = (picoos_uint8) (pdf->content[k]); /* is already picoos_uint8 but just to make explicit */
                    if (ind == ceporder + cepnum) {
                                k = vecstart + pdf->numvuv + ceporder2
                                        + pdf->numdeltas + s * 2; /* s'th sparse delta mean, corresponds to cepnum'th delta delta mean */
                                mean
                                        = ((picoos_int32) ((picoos_int16) ((pdf->content[k
                                + 1]) << 8)) | pdf->content[k])
                                                << (pdf->meanpowUm[ceporder2
                                                        + cepnum]);
                        return mean;
                    }
                }
                return 0;
                        break;
                    default:
                PICODBG_ERROR(("getFromPdf: unknown type wantStaticOrDeltax = %i\n", wantStaticOrDeltax));
                        return 0;
            }
                break;
            case PICOCEP_WANTIVAR:
                switch (wantStaticOrDeltax) {
                    case PICOCEP_WANTSTATIC:
                        k = vecstart + pdf->numvuv + pdf->ceporder * 2
                                + pdf->numdeltas * 3 + cepnum; /* cepnum'th static ivar */
                ivar = (picoos_int32) (pdf->content[k])
                        << (pdf->ivarpow[cepnum]);
                        break;
                    case PICOCEP_WANTDELTA:
                ceporder = pdf->ceporder;
                        k = vecstart + pdf->numvuv + ceporder * 3
                                + pdf->numdeltas * 3 + cepnum; /* cepnum'th delta ivar */
                ivar = (picoos_int32) (pdf->content[k])
                        << (pdf->ivarpow[ceporder + cepnum]);
                        break;
                    case PICOCEP_WANTDELTA2:
                ceporder2 = 2 * pdf->ceporder;
                        k = vecstart + pdf->numvuv + ceporder2 + pdf->numdeltas
                                * 3 + ceporder2 + cepnum; /* cepnum'th delta delta ivar */
                ivar = (picoos_int32) (pdf->content[k])
                        << (pdf->ivarpow[ceporder2 + cepnum]);
                        break;
                    default:
                PICODBG_ERROR(("unknown type wantStaticOrDeltax = %i", wantStaticOrDeltax));
            }
                return ivar;
                break;
            default:
            PICODBG_ERROR(("unknown type wantMeanOrIvar = %i", wantMeanOrIvar));
                return 0;
        }
    }
    return 0;
}

/**
 * Retrieve actual values for MGC from PDF resource - Variant "Direct"
 * @param    pdf :  pointer to picoos_uint8, sequence of pdf vectors, each vector of length 1+ceporder*2+numdeltas*3+ceporder*3
 * @param    indices : indices of pdf vectors for all frames in current sentence
 * @param    activeEndPos :  ??
 * @param    cepnum :  cepstral dimension to be treated
 * @param    smoothcep :  ??
 * @return  the actual value retrieved
 * @callgraph
 * @callergraph
 */
static void getDirect(picokpdf_PdfMUL pdf, picoos_uint16 *indices,
        picoos_uint16 activeEndPos,
        picoos_uint8 cepnum, picoos_int16 *smoothcep)
{
    picoos_uint16 i;
    picoos_uint32 j;
    picoos_uint32 vecstart;
    picoos_int32 mean, ivar;
    picoos_int32 prev_mean;
    picoos_uint8 vecsize = pdf->vecsize;
    picoos_uint8 order = pdf->ceporder;

    j = cepnum;
    prev_mean = 0;
    for (i = 0; i < activeEndPos; i++) {
        if (i > 0 && indices[i] == indices[i - 1]) {
            mean = prev_mean;
        } else {
            vecstart = indices[i] * vecsize;
            mean = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTMEAN,
                    PICOCEP_WANTSTATIC);
            ivar = getFromPdf(pdf, vecstart, cepnum, PICOCEP_WANTIVAR,
                    PICOCEP_WANTSTATIC);
            prev_mean = mean = picocep_fixptdiv(mean, ivar, pdf->bigpow);
        }
        smoothcep[j] = (picoos_int16)(mean/(1<<pdf->meanpow));
        j += order;
    }
}

/**
 * Retrieve actual values for voicing from PDF resource
 * @param    pdf :  pointer to picoos_uint8, sequence of pdf vectors, each vector of length 1+ceporder*2+numdeltas*3+ceporder*3
 * @param    indices : indices of pdf vectors for all frames in current sentence
 * @param    activeEndPos : end position of indices to be considered
 * @return    smoothcep : the values retrieved
 * @callgraph
 * @callergraph
 */
static void getVoiced(picokpdf_PdfMUL pdf, picoos_uint16 *indices,
        picoos_uint16 activeEndPos,
        picoos_uint8 *smoothcep)
{
    picoos_uint16 i, j;
    picoos_uint32 vecstart;
    picoos_uint8 vecsize = pdf->vecsize;

    if (pdf->numvuv == 0) {
        /* do nothing */
    } else {
        for (i = 0, j = 0; i < activeEndPos; i++, j++) {
            vecstart = indices[i] * vecsize;
            smoothcep[j] = pdf->content[vecstart]; /* odd value is voiced, even if unvoiced */
        }
    }
}

/** reads platform-independent uint16 from buffer at *pos and advances *pos
 * @param    buf : buffer picoos_uint8
 * @param    *pos : start position inside buffer of pi uint16
 * @return   the uint16
 * @callgraph
 * @callergraph
 */
static picoos_uint16 get_pi_uint16(picoos_uint8 * buf, picoos_uint16 *pos)
{
    picoos_uint16 res;
    res = buf[(*pos)] | ((picoos_uint16) buf[(*pos) + 1] << 8);
    *pos += 2;
    return res;
}
/**
 * Looks up indices of one phone item and fills index buffers. Consumes Item
 * @param    cep :  the CEP PU sub object pointer
 * @param    ihead : pointer to the start of the phone item
 * @callgraph
 * @callergraph
 */
static void treat_phone(cep_subobj_t * cep, picodata_itemhead_t * ihead)
{
    picoos_uint16 state, frame, frames;
    picoos_uint16 indlfz, indmgc;
    picoos_uint16 pos;
    picoos_uint8  bufferFull;

    /* treat all states
     *    for each state, repeat putting the index into the index buffer framesperstate times.
     */
    /* set state and frame to the first state and frame in the phone to be considered */
    state = 0; /* the first state to be considered */
    frame = 0; /* the first frame to be considered */
    /* numFramesPerState: 2 byte, lf0Index: 2 byte, mgcIndex: 2 byte -> 6 bytes per state */
    PICODBG_DEBUG(("skipping to phone state %i ",state));
    pos = cep->inReadPos + PICODATA_ITEM_HEADSIZE + state * 6;
    /*  */
    PICODBG_DEBUG(("state info starts at inBuf pos %i ",pos));
    /* get the current frames per state */
    frames = get_pi_uint16(cep->inBuf, &pos);
    /*  */
    PICODBG_DEBUG(("number of frames for this phone state: %i",frames));
    /*  */
    PICODBG_DEBUG(("PARSE starting with frame %i",frame));

    bufferFull = cep->indexWritePos >= PICOCEP_MAXWINLEN;
    while ((state < ihead->info2) && (bufferFull == FALSE)) {

        /* get the current state's lf0 and mgc indices and adjust according to state */
        /*   the indices have to be calculated as follows:
         *   new index = (index-1) + stateoffset(state) */

        indlfz = get_pi_uint16(cep->inBuf, &pos); /* lfz index */
        indlfz += -1 + cep->pdflfz->stateoffset[state]; /* transform index */
        indmgc = get_pi_uint16(cep->inBuf, &pos); /* mgc index */
        indmgc += -1 + cep->pdfmgc->stateoffset[state]; /* transform index */

        /* are we reaching the end of the index buffers? */
        if ((cep->indexWritePos - frame) + frames > PICOCEP_MAXWINLEN) {
            /* number of frames that will still fit */
            frames = PICOCEP_MAXWINLEN - (cep->indexWritePos - frame);
            bufferFull = TRUE;
            PICODBG_DEBUG(("smoothing buffer full at state=%i frame=%i",state, frame));
        }
        while (frame < frames) {
            cep->indicesMGC[cep->indexWritePos] = indmgc;
            cep->indicesLFZ[cep->indexWritePos] = indlfz;
            cep->phoneId[cep->indexWritePos] = ihead->info1;
            cep->indexWritePos++;
            frame++;
        }
        /* proceed to next state */
        PICODBG_DEBUG(("finished state %i with %i frames, now at index write pos %i",
                        state, frames,cep->indexWritePos));
        state++;
        if (state < ihead->info2) {
            frame = 0;
            frames = get_pi_uint16(cep->inBuf, &pos);
        }
    }
    /* consume the phone item */
    cep->inReadPos = cep->nextInPos;
    /* */
    PICODBG_DEBUG(("finished phone, advancing inReadPos to %i",cep->inReadPos));
}

/**
 * Returns true if an Item has to be forwarded to next PU
 * @param   ihead : pointer to item head structure
 * @return  TRUE : the item should be forwarded
 * @return  FALSE : the item should be consumed
 * @callgraph
 * @callergraph
 */
static picoos_uint8 forwardingItem(picodata_itemhead_t * ihead)
{
    if ((PICODATA_ITEM_CMD == ihead->type) && (PICODATA_ITEMINFO1_CMD_IGNSIG
            == ihead->info1)) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * performs a step of the cep processing
 * @param    this : pointer to current PU (Control Unit)
 * @param    mode : mode for the PU (not used)
 * @param    numBytesOutput : pointer to output number fo bytes produced
 * @return  one of the "picodata_step_result_t" values
 * @callgraph
 * @callergraph
 */
static picodata_step_result_t cepStep(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * numBytesOutput)
{
    register cep_subobj_t * cep;
    picodata_itemhead_t ihead /* , ohead */;
    picoos_uint8 * icontents;
    pico_status_t sResult = PICO_OK;
    picoos_uint16 blen, clen;
    picoos_uint16 numinb, numoutb;

#if defined (PICO_DEBUG)
    picoos_char msgstr[PICOCEP_MSGSTR_SIZE];
#endif
    numinb = 0;
    numoutb = 0;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    cep = (cep_subobj_t *) this->subObj;
    mode = mode; /* avoid warning "var not used in this function"*/

    /*Init number of output bytes*/
    *numBytesOutput = 0;

    while (1) { /* exit via return */

        PICODBG_DEBUG(("doing pu state %i", cep->procState));

        switch (cep->procState) {

            case PICOCEP_STEPSTATE_COLLECT:
                /* *************** item collector ***********************************/

                PICODBG_TRACE(("COLLECT"));

                /*collecting items from the PU input buffer*/
                sResult = picodata_cbGetItem(this->cbIn,
                        &(cep->inBuf[cep->inWritePos]), cep->inBufSize
                                - cep->inWritePos, &blen);
                if (PICO_EOF == sResult) { /* there are no more items available and we always need more data here */
                    PICODBG_DEBUG(("COLLECT need more data, returning IDLE"));
                    return PICODATA_PU_IDLE;
                }

                PICODBG_DEBUG(("got item, status: %d", sResult));

                if ((PICO_OK == sResult) && (blen > 0)) {
                    /* we now have one item */
                    cep->inWritePos += blen;
                    cep->procState = PICOCEP_STEPSTATE_PROCESS_PARSE;
                } else {
                    /* ignore item and stay in collect */
                    PICODBG_ERROR(("COLLECT got bad result %i", sResult));
                    cep->inReadPos = cep->inWritePos = 0;
                }
                /* return PICODATA_PU_ATOMIC; */
                break;

            case PICOCEP_STEPSTATE_PROCESS_PARSE:
                /* **************** put item indices into index buffers (with repetition)  ******************/

                PICODBG_TRACE(("PARSE"));

                PICODBG_DEBUG(("getting info from inBuf in range: [%i,%i[", cep->inReadPos, cep->inWritePos));
                if (cep->inWritePos <= cep->inReadPos) {
                    /* no more items in inBuf */
                    /* we try to get more data */
                    PICODBG_DEBUG(("no more items in inBuf, try to collect more"));
                    /* cep->needMoreInput = TRUE; */
                    cep->inReadPos = cep->inWritePos = 0;
                    cep->procState = PICOCEP_STEPSTATE_COLLECT;
                    break;
                }
                /* look at the current item */
                /*verify that current item is valid */
                if (!picodata_is_valid_item(cep->inBuf + cep->inReadPos,
                        cep->inWritePos - cep->inReadPos)) {
                    PICODBG_ERROR(("found invalid item"));
                    sResult = picodata_get_iteminfo(
                            cep->inBuf + cep->inReadPos, cep->inWritePos
                                    - cep->inReadPos, &ihead, &icontents);PICODBG_DEBUG(("PARSE bad item %s",picodata_head_to_string(&ihead,msgstr,PICOCEP_MSGSTR_SIZE)));

                    return PICODATA_PU_ERROR;
                }

                sResult = picodata_get_iteminfo(cep->inBuf + cep->inReadPos,
                        cep->inWritePos - cep->inReadPos, &ihead, &icontents);

                if (PICO_EXC_BUF_UNDERFLOW == sResult) {
                    /* no more items in inBuf */
                    /* we try to get more data */
                    PICODBG_DEBUG(("no more items in inBuf, try to collect more"));
                    /* cep->needMoreInput = TRUE; */
                    cep->inReadPos = cep->inWritePos = 0;
                    cep->procState = PICOCEP_STEPSTATE_COLLECT;
                    break;
                } else if (PICO_OK != sResult) {
                    PICODBG_ERROR(("unknown exception (sResult == %i)",sResult));
                    return (picodata_step_result_t) picoos_emRaiseException(
                            this->common->em, sResult, NULL, NULL);
                    break;
                }

                PICODBG_DEBUG(("PARSE looking at item %s",picodata_head_to_string(&ihead,msgstr,PICOCEP_MSGSTR_SIZE)));

                cep->nextInPos = PICODATA_ITEM_HEADSIZE + ihead.len;

                /* we decide what to do next depending on the item and the state of the index buffer:
                 * - process buffer if buffer not empty and sentence end or flush or ignsig/start (don't consume item yet)
                 * - fill buffer with (part of) phone contents if item is a phone
                 * - consume or copy for later output otherwise
                 */

                if (cep->inIgnoreState) {
                    if ((PICODATA_ITEM_CMD == ihead.type)
                            && (PICODATA_ITEMINFO1_CMD_IGNSIG == ihead.info1)
                            && (PICODATA_ITEMINFO2_CMD_END == ihead.info2)) {
                        cep->inIgnoreState = 0;
                    }PICODBG_DEBUG(("cep: PARSE consuming item of inBuf"));
                    cep->inReadPos = cep->nextInPos;
                    break;
                }

                /* see if it is a sentence end boundary or termination boundary (flush) and there are indices to smooth -> smooth */
                if ((PICODATA_ITEM_BOUND == ihead.type)
                        && ((PICODATA_ITEMINFO1_BOUND_SEND == ihead.info1)
                                || (PICODATA_ITEMINFO1_BOUND_TERM == ihead.info1))
                        && (cep->indexWritePos > 0)) {
                    /* we smooth the buffer */
                    cep->activeEndPos = cep->indexWritePos;
                    cep->sentenceEnd = TRUE;
                    /* output whatever we got */
                    PICODBG_DEBUG(("cep: PARSE found sentence terminator; setting activeEndPos to %i",cep->activeEndPos));
                    cep->procState = PICOCEP_STEPSTATE_PROCESS_SMOOTH;
                    break;
                } else if (PICODATA_ITEM_PHONE == ihead.type) {
                    /* it is a phone */
                    PICODBG_DEBUG(("cep: PARSE treating PHONE"));
                    treat_phone(cep, &ihead);

                } else {
                    if ((PICODATA_ITEM_CMD == ihead.type)
                            && (PICODATA_ITEMINFO1_CMD_IGNSIG == ihead.info1)
                            && (PICODATA_ITEMINFO2_CMD_START == ihead.info2)) {
                        cep->inIgnoreState = 1;
                    }
                    /* sentence end or flush remaining after frame or other non-processable item, e.g. command */
                    /* do we have to forward? */
                    if (forwardingItem(&ihead)) {
                        /* if no active frames, output immediately */
                        if (cep->indexWritePos <= 0) {
                            /* copy item to outBuf */
                            PICODBG_DEBUG(("PARSE copy item in inBuf to outBuf"));
                            picodata_copy_item(cep->inBuf + cep->inReadPos,
                                    cep->inWritePos - cep->inReadPos,
                                    cep->outBuf, cep->outBufSize, &blen);
                            cep->outWritePos += blen;
                            PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                                    (picoos_uint8 *)"cep: do forward item ",
                                    cep->outBuf, PICODATA_MAX_ITEMSIZE);
                            /* output item and then go to parse to treat a new item. */
                            cep->feedFollowState
                                    = PICOCEP_STEPSTATE_PROCESS_PARSE;
                            cep->procState = PICOCEP_STEPSTATE_FEED;
                        } else if ((cep->headxWritePos < PICOCEP_MAXNR_HEADX)
                                && (cep->cbufWritePos + ihead.len
                                        < cep->cbufBufSize)) {
                            /* there is enough space to store item */
                            PICODBG_DEBUG(("unhandled item (type %c, length %i). Storing associated with index %i",ihead.type, ihead.len, cep->indexWritePos));
                            sResult
                                    = picodata_get_itemparts(
                                            cep->inBuf + cep->inReadPos,
                                            cep->inWritePos - cep->inReadPos,
                                            &(cep->headx[cep->headxWritePos].head),
                                            &(cep->cbuf[cep->cbufWritePos]),
                                            cep->cbufBufSize
                                                    - cep->cbufWritePos, &clen);

                            if (sResult != PICO_OK) {
                                PICODBG_ERROR(("problem getting item parts"));
                                picoos_emRaiseException(this->common->em,
                                        sResult, NULL, NULL);
                                return PICODATA_PU_ERROR;
                            }
                            /* remember sync position */
                            cep->headx[cep->headxWritePos].frame
                                    = cep->indexWritePos;

                            if (clen > 0) {
                                cep->headx[cep->headxWritePos].cind
                                        = cep->cbufWritePos;
                                cep->cbufWritePos += clen;
                            } else {
                                cep->headx[cep->headxWritePos].cind = 0;
                            }
                            cep->headxWritePos++;
                        } else {
                            /* buffer full, smooth and output whatever we got */
                            PICODBG_DEBUG(("PARSE is forced to smooth prematurely; setting activeEndPos to %i", cep->activeEndPos));
                            cep->procState = PICOCEP_STEPSTATE_PROCESS_SMOOTH;
                            /* don't consume item yet */
                            break;
                        }
                    } else {
                    }PICODBG_DEBUG(("cep: PARSE consuming item of inBuf"));
                    cep->inReadPos = cep->nextInPos;
                }
                break;

            case PICOCEP_STEPSTATE_PROCESS_SMOOTH:
                /* **************** smooth (indexed) coefficients and store smoothed in outBuffers  ****************/

                PICODBG_TRACE(("SMOOTH"));
                {
                    picokpdf_PdfMUL pdf;

                    /* picoos_uint16 framesTreated = 0; */
                    picoos_uint8 cepnum;
                    picoos_uint16 N;

                    N = cep->activeEndPos; /* numframes in current step */

                    /* the range to be smoothed starts at 0 and is N long */

                    /* smooth each cepstral dimension separately */
                    /* still to be experimented if higher order coeff can remain unsmoothed, i.e. simple copy from pdf */

                    /* reset the f0, ceps and voiced outfuffers */
                    cep->outXCepReadPos = cep->outXCepWritePos = 0;
                    cep->outVoicedReadPos = cep->outVoicedWritePos = 0;
                    cep->outF0ReadPos = cep->outF0WritePos = 0;

                    PICODBG_DEBUG(("smoothing %d frames\n", N));

                    /* smooth f0 */
                    pdf = cep->pdflfz;
                    for (cepnum = 0; cepnum < pdf->ceporder; cepnum++) {
                        if (cep->activeEndPos <= 0) {
                            /* do nothing */
                        } else if (3 < N) {
                            makeWUWandWUm(cep, pdf, cep->indicesLFZ, 0, N,
                                    cepnum); /* update diag0, diag1, diag2, WUm */
                            invMatrix(cep, N, cep->outF0 + cep->outF0WritePos, cepnum, pdf,
                                    PICOCEP_LFZINVPOW, PICOCEP_LFZDOUBLEDEC);
                        } else {
                            getDirect(pdf, cep->indicesLFZ, cep->activeEndPos,
                                    cepnum, cep->outF0 + cep->outF0WritePos);
                        }
                    }/* end for cepnum  */
                    cep->outF0WritePos += cep->activeEndPos * pdf->ceporder;

                    /* smooth mgc */
                    pdf = cep->pdfmgc;
                    for (cepnum = 0; cepnum < pdf->ceporder; cepnum++) {
                        if (cep->activeEndPos <= 0) {
                            /* do nothing */
                        } else if (3 < N) {
                            makeWUWandWUm(cep, pdf, cep->indicesMGC, 0, N,
                                    cepnum); /* update diag0, diag1, diag2, WUm */
                            invMatrix(cep, N, cep->outXCep
                                            + cep->outXCepWritePos, cepnum,
                                    pdf, PICOCEP_MGCINVPOW,
                                    PICOCEP_MGCDOUBLEDEC);
                        } else {
                            getDirect(pdf, cep->indicesMGC, cep->activeEndPos,
                                    cepnum, cep->outXCep + cep->outXCepWritePos);
                        }
                    }/* end for cepnum  */
                    cep->outXCepWritePos += cep->activeEndPos * pdf->ceporder;

                    getVoiced(pdf, cep->indicesMGC, cep->activeEndPos, cep->outVoiced
                                    + cep->outVoicedWritePos);
                    cep->outVoicedWritePos += cep->activeEndPos;

                }
                /* setting indexReadPos to the next active index to be used. (will be advanced by FRAME when
                 * reading the phoneId */
                cep->indexReadPos = 0;
                cep->procState = PICOCEP_STEPSTATE_PROCESS_FRAME;
                return PICODATA_PU_BUSY; /*data to feed*/

                break;

            case PICOCEP_STEPSTATE_PROCESS_FRAME:

                /* *************** creating output items (type FRAME) ***********************************/

                PICODBG_TRACE(("FRAME"));

                if ((cep->headxBottom < cep->headxWritePos)
                        && (cep->headx[cep->headxBottom].frame
                                <= cep->indexReadPos)) {

                    /* output item in headx/cbuf */
                    /* copy item to outBuf */
                    PICODBG_DEBUG(("FRAME copy item in inBuf to outBuf"));
                    picodata_put_itemparts(
                            &(cep->headx[cep->headxBottom].head),
                            &(cep->cbuf[cep->headx[cep->headxBottom].cind]),
                            cep->headx[cep->headxBottom].head.len, cep->outBuf,
                            cep->outBufSize, &blen);
                    cep->outWritePos += blen;
                    /* consume item in headx/cbuf */
                    PICODBG_DEBUG(("PARSE consuming item of headx/cbuf"));
                    cep->headxBottom++;

                    /* output item and then go to parse to treat a new item. */
                    cep->feedFollowState = PICOCEP_STEPSTATE_PROCESS_FRAME;
                    cep->procState = PICOCEP_STEPSTATE_FEED;
                    break;
                }

                if (cep->indexReadPos < cep->activeEndPos) {
                    /*------------  there are frames to output ----------------------------------------*/
                    /* still frames to output, create new FRAME_PAR item */

                    cep->nNumFrames++;

                    PICODBG_DEBUG(("FRAME creating FRAME_PAR: active: [0,%i[, read=%i, write=%i",
                                    cep->activeEndPos, cep->indexReadPos, cep->indexWritePos
                            ));

                    /* create FRAME_PAR items from cep->outXX one by one */

                    /* converting the ceps shorts into floats:
                     * scmeanpow = pdf->bigpow - pdf->meanpow;
                     * for all sval:
                     *   fval = (picoos_single) sval / scmeanpow;
                     */

                    cep->outWritePos = cep->outReadPos = 0;
                    cep->outBuf[cep->outWritePos++] = cep->framehead.type;
                    cep->outBuf[cep->outWritePos++] = cep->framehead.info1;
                    cep->outBuf[cep->outWritePos++] = cep->framehead.info2;
                    cep->outBuf[cep->outWritePos++] = cep->framehead.len;

                    PICODBG_DEBUG(("FRAME  writing position after header: %i",cep->outWritePos));

                    {
                        picoos_uint16 tmpUint16;
                        picoos_int16 tmpInt16;
                        picoos_uint16 i;

                        /* */
                        PICODBG_DEBUG(("FRAME reading phoneId[%i] = %c:",cep->indexReadPos, cep->phoneId[cep->indexReadPos]));
                        /* */

                        tmpUint16
                                = (picoos_uint16) cep->phoneId[cep->indexReadPos];

                        picoos_mem_copy((void *) &tmpUint16,
                                (void *) &cep->outBuf[cep->outWritePos],
                                sizeof(tmpUint16));

                        cep->outWritePos += sizeof(tmpUint16);

                        PICODBG_DEBUG(("FRAME  writing position after phone id: %i",cep->outWritePos));

                        for (i = 0; i < cep->pdflfz->ceporder; i++) {

                            tmpUint16 = (cep->outVoiced[cep->outVoicedReadPos]
                                    & 0x01) ? cep->outF0[cep->outF0ReadPos]
                                    : (picoos_uint16) 0;

                            picoos_mem_copy((void *) &tmpUint16,
                                    (void *) &cep->outBuf[cep->outWritePos],
                                    sizeof(tmpUint16));
                            cep->outWritePos += sizeof(tmpUint16);

                            tmpUint16
                                    = (picoos_uint16) (cep->outVoiced[cep->outVoicedReadPos]);
                            picoos_mem_copy((void *) &tmpUint16,
                                    (void *) &cep->outBuf[cep->outWritePos],
                                    sizeof(tmpUint16));
                            cep->outWritePos += sizeof(tmpUint16);
                            tmpUint16
                                    = (picoos_uint16) (cep->outF0[cep->outF0ReadPos]);
                            picoos_mem_copy((void *) &tmpUint16,
                                    (void *) &cep->outBuf[cep->outWritePos],
                                    sizeof(tmpUint16));
                            cep->outWritePos += sizeof(tmpUint16);

                            cep->outVoicedReadPos++;
                            cep->outF0ReadPos++;
                        }

                        PICODBG_DEBUG(("FRAME writing position after f0: %i",cep->outWritePos));

                        for (i = 0; i < cep->pdfmgc->ceporder; i++) {
                            tmpInt16 = cep->outXCep[cep->outXCepReadPos++];
                            picoos_mem_copy((void *) &tmpInt16,
                                    (void *) &cep->outBuf[cep->outWritePos],
                                    sizeof(tmpInt16));
                            cep->outWritePos += sizeof(tmpInt16);
                        }

                        PICODBG_DEBUG(("FRAME  writing position after cepstrals: %i",cep->outWritePos));

                        tmpUint16
                                = (picoos_uint16) cep->indicesMGC[cep->indexReadPos++];

                        picoos_mem_copy((void *) &tmpUint16,
                                (void *) &cep->outBuf[cep->outWritePos],
                                sizeof(tmpUint16));

                        PICODBG_DEBUG(("FRAME  writing position after mgc index: %i",cep->outWritePos));

                        cep->outWritePos += sizeof(tmpUint16);

                    }
                    /* finished to create FRAME_PAR, now output and then come back*/
                    cep->feedFollowState = PICOCEP_STEPSTATE_PROCESS_FRAME;
                    cep->procState = PICOCEP_STEPSTATE_FEED;

                } else if (cep->sentenceEnd) {
                    /*------------  no more frames to output at end of sentence ----------------------------------------*/
                    PICODBG_INFO(("End of sentence - Processed frames : %d",
                                    cep->nNumFrames));
                    cep->nNumFrames = 0;PICODBG_DEBUG(("FRAME no more active frames for this sentence"));
                    /* no frames left in this sentence*/
                    /* reset for new sentence */
                    initSmoothing(cep);
                    cep->sentenceEnd = FALSE;
                    cep->indexReadPos = cep->indexWritePos = 0;
                    cep->activeEndPos = PICOCEP_MAXWINLEN;
                    cep->headxBottom = cep->headxWritePos = 0;
                    cep->cbufWritePos = 0;
                    cep->procState = PICOCEP_STEPSTATE_PROCESS_PARSE;
                } else {
                    /*------------  no more frames can be output but sentence end not reached ----------------------------------------*/
                    PICODBG_DEBUG(("Maximum number of frames per sentence reached"));
                    cep->procState = PICOCEP_STEPSTATE_PROCESS_PARSE;
                }
                /*----------------------------------------------------*/
                break;

            case PICOCEP_STEPSTATE_FEED:
                /* ***********************************************************************/
                /* FEED: combine input item with pos/phon pairs to output item */
                /* ***********************************************************************/

                PICODBG_DEBUG(("FEED"));

                PICODBG_DEBUG(("FEED putting outBuf item into cb"));

                /*feeding items to PU output buffer*/
                sResult = picodata_cbPutItem(this->cbOut, cep->outBuf,
                        cep->outBufSize, &blen);

                if (PICO_EXC_BUF_OVERFLOW == sResult) {
                    /* we have to redo this item */
                    PICODBG_DEBUG(("FEED got overflow, returning PICODATA_PU_OUT_FULL"));
                    return PICODATA_PU_OUT_FULL;
                } else if (PICO_OK == sResult) {

                    if (cep->outBuf[0] != 'k') {
                        PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                                (picoos_uint8 *)"cep: ",
                                cep->outBuf, PICODATA_MAX_ITEMSIZE);
                    }

                    *numBytesOutput += blen;
                    /*-------------------------*/
                    /*reset the output pointers*/
                    /*-------------------------*/
                    if (cep->outReadPos >= cep->outWritePos) {
                        cep->outReadPos = 0;
                        cep->outWritePos = 0;
                    }
                    cep->procState = cep->feedFollowState;
                    PICODBG_DEBUG(("FEED ok, going back to procState %i", cep->procState));
                    return PICODATA_PU_BUSY;
                } else {
                    PICODBG_DEBUG(("FEED got exception %i when trying to output item",sResult));
                    cep->procState = cep->feedFollowState;
                    return (picodata_step_result_t) sResult;
                }
                break;

            default:
                /*NOT feeding items*/
                sResult = PICO_EXC_BUF_IGNORE;
                break;
        }/*end switch (cep->procState) */
        return PICODATA_PU_BUSY; /*check if there is more data to process after feeding*/

    }/*end while*/
    /* we should never come here */
    return PICODATA_PU_ERROR;
}/*cepStep*/

/* Picocep.c end */
