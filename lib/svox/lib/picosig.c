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
 * @file picosig.c
 *
 * Signal Generation PU - Implementation
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#include "picoos.h"
#include "picodsp.h"
#include "picosig2.h"
#include "picodata.h"
#include "picosig.h"
#include "picodbg.h"
#include "picokpdf.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#define PICOSIG_IN_BUFF_SIZE PICODATA_BUFSIZE_SIG   /*input buffer size for SIG */
#define PICOSIG_OUT_BUFF_SIZE PICODATA_BUFSIZE_SIG  /*output buffer size for SIG*/

#define PICOSIG_COLLECT     0
#define PICOSIG_SCHEDULE    1
#define PICOSIG_PLAY        2
#define PICOSIG_PROCESS     3
#define PICOSIG_FEED        4

/*----------------------------------------------------------
 // Internal function declarations
 //---------------------------------------------------------*/

static picodata_step_result_t sigStep(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * numBytesOutput);

/*----------------------------------------------------------
 // Name    :   sig_subobj
 // Function:   subobject definition for the sig processing
 // Shortcut:   sig
 //---------------------------------------------------------*/
typedef struct sig_subobj
{
    /*----------------------PU voice management------------------------------*/
    /* picorsrc_Voice voice; */
    /*----------------------PU state management------------------------------*/
    picoos_uint8 procState; /* where to take up work at next processing step */
    picoos_uint8 retState;  /* where to return after next processing step */
    picoos_uint8 needMoreInput; /* more data necessary to start processing   */
    /*----------------------PU input management------------------------------*/
    picoos_uint8 inBuf[PICOSIG_IN_BUFF_SIZE]; /* internal input buffer */
    picoos_uint16 inBufSize;/* actually allocated size */
    picoos_uint16 inReadPos, inWritePos; /* next pos to read/write from/to inBuf*/
    /*Input audio file management*/
    picoos_char sInSDFileName[255];
    picoos_SDFile sInSDFile;
    picoos_uint32 sInSDFilePos;
    /*----------------------PU output management-----------------------------*/
    picoos_uint8 outBuf[PICOSIG_OUT_BUFF_SIZE]; /* internal output buffer */
    picoos_uint16 outBufSize;                /* actually allocated size */
    picoos_uint16 outReadPos, outWritePos;  /* next pos to read/write from/to outBuf*/
    picoos_bool outSwitch;                  /* output destination switch 0:buffer, 1:file*/
    picoos_char sOutSDFileName[255];        /* output file name */
    picoos_SDFile sOutSDFile;               /* output file handle */
    picoos_single fSampNorm;                /* running normalization factor */
    picoos_uint32 nNumFrame;                /* running count for frame number in output items */
    /*---------------------- other working variables ---------------------------*/
    picoos_uint8 innerProcState; /*where to take up work at next processing step*/
    /*-----------------------Definition of the local storage for this PU--------*/
    sig_innerobj_t sig_inner;
    picoos_single pMod; /*pitch modifier*/
    picoos_single vMod; /*Volume modifier*/
    picoos_single sMod; /*speaker modifier*/
    /*knowledge bases */
    picokpdf_PdfMUL pdflfz, pdfmgc;
    picoos_uint32 scmeanpowLFZ, scmeanpowMGC;
    picoos_uint32 scmeanLFZ, scmeanMGC;
    picokpdf_PdfPHS pdfphs;

} sig_subobj_t;

/* ******************************************************************************
 *   generic PU management
 ********************************************************************************/

/**
 * initialization of the PU (processing unit)
 * @param    this : sig PU object
 * @return  PICO_OK : init ok
 * @return  PICO_ERR_OTHER : init failed
 * @callgraph
 * @callergraph
 */
static pico_status_t sigInitialize(register picodata_ProcessingUnit this, picoos_int32 resetMode)
{
    sig_subobj_t *sig_subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    sig_subObj = (sig_subobj_t *) this->subObj;
    sig_subObj->inBufSize = PICOSIG_IN_BUFF_SIZE;
    sig_subObj->outBufSize = PICOSIG_OUT_BUFF_SIZE;
    sig_subObj->inReadPos = 0;
    sig_subObj->inWritePos = 0;
    sig_subObj->outReadPos = 0;
    sig_subObj->outWritePos = 0;
    sig_subObj->needMoreInput = 0;
    sig_subObj->procState = PICOSIG_COLLECT;
    sig_subObj->retState = PICOSIG_COLLECT;
    sig_subObj->innerProcState = 0;
    sig_subObj->nNumFrame = 0;

    /*-----------------------------------------------------------------
     * MANAGE Item I/O control management
     ------------------------------------------------------------------*/
    sig_subObj->sInSDFile = NULL;
    sig_subObj->sInSDFilePos = 0;
    sig_subObj->sInSDFileName[0] = '\0';
    sig_subObj->outSwitch = 0; /*PU sends output to buffer (nextPU)*/
    sig_subObj->sOutSDFile = NULL;
    sig_subObj->sOutSDFileName[0] = '\0';
    sig_subObj->nNumFrame = 0;

    /*-----------------------------------------------------------------
     * MANAGE LINGWARE INITIALIZATION IF NEEDED
     ------------------------------------------------------------------*/
    if (resetMode == PICO_RESET_FULL) {
        /*not done when resetting SOFT*/
        sig_subObj->pdfmgc = picokpdf_getPdfMUL(
                this->voice->kbArray[PICOKNOW_KBID_PDF_MGC]);
        sig_subObj->pdflfz = picokpdf_getPdfMUL(
                this->voice->kbArray[PICOKNOW_KBID_PDF_LFZ]);
        sig_subObj->pdfphs = picokpdf_getPdfPHS(
                this->voice->kbArray[PICOKNOW_KBID_PDF_PHS]);

        sig_subObj->scmeanpowLFZ = sig_subObj->pdflfz->bigpow
                - sig_subObj->pdflfz->meanpow;
        sig_subObj->scmeanpowMGC = sig_subObj->pdfmgc->bigpow
                - sig_subObj->pdfmgc->meanpow;
        sig_subObj->scmeanLFZ = (1 << (picoos_uint32) sig_subObj->scmeanpowLFZ);
        sig_subObj->scmeanMGC = (1 << (picoos_uint32) sig_subObj->scmeanpowMGC);
        sig_subObj->fSampNorm = PICOSIG_NORM1 * sig_subObj->pdfmgc->amplif;
        /*-----------------------------------------------------------------
         * Initialize memory for DSP
         * ------------------------------------------------------------------*/
        sigDspInitialize(&(sig_subObj->sig_inner), resetMode);
        /*-----------------------------------------------------------------
         * Initialize modifiers
         * ------------------------------------------------------------------*/
        /*pitch , volume , speaker modifiers*/
        sig_subObj->pMod = 1.0f;
        sig_subObj->vMod = 1.0f;
        sig_subObj->sMod = 1.0f;
    } else {
        /*-----------------------------------------------------------------
         * Initialize memory for DSP
         * ------------------------------------------------------------------*/
        sigDspInitialize(&(sig_subObj->sig_inner), resetMode);
    }


    return PICO_OK;
}/*sigInitialize*/

/**
 * terminates the PU (processing unit)
 * @param    this : sig PU object
 * @return  PICO_OK : termination ok
 * @return  PICO_ERR_OTHER : termination failed
 * @callgraph
 * @callergraph
 */
static pico_status_t sigTerminate(register picodata_ProcessingUnit this)
{

    sig_subobj_t *sig_subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    sig_subObj = (sig_subobj_t *) this->subObj;

    return PICO_OK;
}/*sigTerminate*/

/**
 * deallocates the PU (processing unit) sub object
 * @param    this : sig PU object
 * @param    mm : the engine memory manager
 * @return  PICO_OK : deallocation ok
 * @return  PICO_ERR_OTHER : deallocation failed
 * @callgraph
 * @callergraph
 */
static pico_status_t sigSubObjDeallocate(register picodata_ProcessingUnit this,
        picoos_MemoryManager mm)
{
    sig_subobj_t *sig_subObj;
    if ((NULL == this) || ((this)->subObj == NULL)) {
        return PICO_ERR_OTHER;
    }
    sig_subObj = (sig_subobj_t *) (this)->subObj;

    if (sig_subObj->sInSDFile != NULL) {
        picoos_sdfCloseIn(this->common, &(sig_subObj->sInSDFile));
        sig_subObj->sInSDFile = NULL;
        sig_subObj->sInSDFileName[0] = '\0';
    }

    if (sig_subObj->sOutSDFile != NULL) {
        picoos_sdfCloseOut(this->common, &(sig_subObj->sOutSDFile));
        sig_subObj->sOutSDFile = NULL;
        sig_subObj->sOutSDFileName[0] = '\0';
    }

    sigDeallocate(mm, &(sig_subObj->sig_inner));

    picoos_deallocate(this->common->mm, (void *) &this->subObj);

    return PICO_OK;
}/*sigSubObjDeallocate*/

/**
 * creates a new sig processing unit
 * @param    mm : the engine memory manager
 * @param    common : the engine common object
 * @param    cbIn : the PU input buffer
 * @param    cbOut : the PU output buffer
 * @param    voice : the voice descriptor object
 * @return  a valid PU handle if creation is OK
 * @return  NULL if creation is !=OK
 * @callgraph
 * @callergraph
 */
picodata_ProcessingUnit picosig_newSigUnit(picoos_MemoryManager mm,
        picoos_Common common, picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut, picorsrc_Voice voice)
{
    sig_subobj_t *sig_subObj;

    picodata_ProcessingUnit this = picodata_newProcessingUnit(mm, common, cbIn,
            cbOut, voice);
    if (NULL == this) {
        return NULL;
    }
    this->initialize = sigInitialize;

    PICODBG_DEBUG(("picosig_newSigUnit -- creating SIG PU"));
    /*Init function pointers*/
    this->step = sigStep;
    this->terminate = sigTerminate;
    this->subDeallocate = sigSubObjDeallocate;
    /*sub obj allocation*/
    this->subObj = picoos_allocate(mm, sizeof(sig_subobj_t));

    if (NULL == this->subObj) {
        PICODBG_ERROR(("Error in Sig Object allocation"));
        picoos_deallocate(mm, (void *) &this);
        return NULL;
    }
    sig_subObj = (sig_subobj_t *) this->subObj;

    /*-----------------------------------------------------------------
     * Allocate memory for DSP inner algorithms
     * ------------------------------------------------------------------*/
    if (sigAllocate(mm, &(sig_subObj->sig_inner)) != 0) {
        PICODBG_ERROR(("Error in Sig Sub Object Allocation"));
         picoos_deallocate(mm, (void *) &this);
        return NULL;
    }

    /*-----------------------------------------------------------------
     * Initialize memory for DSP (this may be re-used elsewhere, e.g.Reset)
     * ------------------------------------------------------------------*/
    if (PICO_OK != sigInitialize(this, PICO_RESET_FULL)) {
        PICODBG_ERROR(("Error in iSig Sub Object initialization"));
        sigDeallocate(mm, &(sig_subObj->sig_inner));
        picoos_deallocate(mm, (void *) &this);
        return NULL;
    }PICODBG_DEBUG(("SIG PU creation succeded!!"));
    return this;
}/*picosig_newSigUnit*/

/**
 * pdf access for phase
 * @param    this : sig object pointer
 * @param    phsIndex : index of phase vectot in the pdf
 * @param    phsVect : pointer to base of array where to store the phase values
 * @param    numComponents : pointer to the variable to store the number of components
 * @return   PICO_OK : pdf retrieved
 * @return   PICO_ERR_OTHER : pdf not retrieved
 * @callgraph
 * @callergraph
 */
static pico_status_t getPhsFromPdf(register picodata_ProcessingUnit this,
        picoos_uint16 phsIndex, picoos_int32 *phsVect,
        picoos_int16 *numComponents)
{
    sig_subobj_t *sig_subObj;
    picokpdf_PdfPHS pdf;
    static int nFrame = 0;

    picoos_uint32 nIndexValue;
    picoos_uint8 *nCurrIndexOffset, *nContent;
    picoos_uint16 nI;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    sig_subObj = (sig_subobj_t *) this->subObj;
    pdf = sig_subObj->pdfphs;
    /*check incoming index*/
    if (phsIndex >= pdf->numvectors) {
        return PICODATA_PU_ERROR;
    }
    nCurrIndexOffset = ((picoos_uint8*) pdf->indexBase) + phsIndex * sizeof(picoos_uint32);
    nIndexValue = (0xFF000000 & ((*(nCurrIndexOffset+3)) << 24)) | (0x00FF0000 & ((*(nCurrIndexOffset+2)) << 16)) |
                  (0x0000FF00 & ((*(nCurrIndexOffset+1)) << 8))  | (0x000000FF & ((*nCurrIndexOffset)));
    nContent = pdf->contentBase;
    nContent += nIndexValue;
    *numComponents = (picoos_int16) *nContent++;
    if (*numComponents>PICODSP_PHASEORDER) {
        PICODBG_DEBUG(("WARNING : Frame %d -- Phase vector[%d] Components = %d --> too big\n",  nFrame, phsIndex, *numComponents));
        *numComponents = PICODSP_PHASEORDER;
    }
    for (nI=0; nI<*numComponents; nI++) {
        phsVect[nI] = (picoos_int32) *nContent++;
    }
    for (nI=*numComponents; nI<PICODSP_PHASEORDER; nI++) {
        phsVect[nI] = 0;
    }
    nFrame++;
    return PICO_OK;
}/*getPhsFromPdf*/

/**
 * processes one item with sig algo
 * @param    this : the PU object pointer
 * @param    inReadPos : read position in input buffer
 * @param    numinb : number of bytes in input buffer (including header)
 * @param    outWritePos : write position in output buffer
 * @param    numoutb : number of bytes produced in output buffer
 * @return  PICO_OK : processing successful and terminated
 * @return  PICO_STEP_BUSY : processing successful but still things to do
 * @return  PICO_ERR_OTHER : errors
 * @remarks processes a full input item
 * @remarks input data is one or more item, taken from local storage
 * @remarks output data is one or more item, written in local storage
 * @callgraph
 * @callergraph
 */
static pico_status_t sigProcess(register picodata_ProcessingUnit this,
        picoos_uint16 inReadPos, picoos_uint16 numinb,
        picoos_uint16 outWritePos, picoos_uint16 *numoutb)
{

    register sig_subobj_t * sig_subObj;
    picoos_int16 n_i;
    picoos_int16 n_frames, n_count;
    picoos_int16 *s_data, offset;
    picoos_int32 f_data, mlt, *t1, *tmp1, *tmp2;
    picoos_uint16 tmp_uint16;
    picopal_int16 tmp_int16;
    picoos_uint16 i, cnt;
    picoos_int16 hop_p_half;

    sig_subObj = (sig_subobj_t *) this->subObj;

    numinb = numinb; /* avoid warning "var not used in this function"*/

    /*defaults to 0 for output bytes*/
    *numoutb = 0;

    /*Input buffer contains an item FRAME_PAR*/
    switch (sig_subObj->innerProcState) {

        case 0:
            /*---------------------------------------------
             Shifting old values
             ---------------------------------------------*/
            for (n_count = 0; n_count < CEPST_BUFF_SIZE-1; n_count++) {
                sig_subObj->sig_inner.F0Buff[n_count]=sig_subObj->sig_inner.F0Buff[n_count+1];
                sig_subObj->sig_inner.PhIdBuff[n_count]=sig_subObj->sig_inner.PhIdBuff[n_count+1];
                sig_subObj->sig_inner.VoicingBuff[n_count]=sig_subObj->sig_inner.VoicingBuff[n_count+1];
                sig_subObj->sig_inner.FuVBuff[n_count]=sig_subObj->sig_inner.FuVBuff[n_count+1];
            }
            for (n_count = 0; n_count < PHASE_BUFF_SIZE-1; n_count++) {
                sig_subObj->sig_inner.VoxBndBuff[n_count]=sig_subObj->sig_inner.VoxBndBuff[n_count+1];
            }

            tmp1 = sig_subObj->sig_inner.CepBuff[0];
            for (n_count = 0; n_count < CEPST_BUFF_SIZE-1; n_count++) {
                sig_subObj->sig_inner.CepBuff[n_count]=sig_subObj->sig_inner.CepBuff[n_count+1];
            }
            sig_subObj->sig_inner.CepBuff[CEPST_BUFF_SIZE-1]=tmp1;

            tmp1 = sig_subObj->sig_inner.PhsBuff[0];
            for (n_count = 0; n_count < PHASE_BUFF_SIZE-1; n_count++) {
                sig_subObj->sig_inner.PhsBuff[n_count]=sig_subObj->sig_inner.PhsBuff[n_count+1];
            }
            sig_subObj->sig_inner.PhsBuff[PHASE_BUFF_SIZE-1]=tmp1;

            /*---------------------------------------------
             Frame related initializations
             ---------------------------------------------*/
            sig_subObj->sig_inner.prevVoiced_p = sig_subObj->sig_inner.voiced_p;
            /*---------------------------------------------
             Get input data from PU buffer in internal buffers
             -------------------------------------------------*/
            /*load the phonetic id code*/
            picoos_mem_copy((void *) &sig_subObj->inBuf[inReadPos
                    + sizeof(picodata_itemhead_t)],                   /*src*/
            (void *) &tmp_uint16, sizeof(tmp_uint16));                /*dest+size*/
            sig_subObj->sig_inner.PhIdBuff[CEPST_BUFF_SIZE-1] = (picoos_int16) tmp_uint16; /*store into newest*/
            tmp_uint16 = (picoos_int16) sig_subObj->sig_inner.PhIdBuff[0];                 /*assign oldest*/
            sig_subObj->sig_inner.phId_p = (picoos_int16) tmp_uint16;                      /*assign oldest*/

            /*load pitch values*/
            for (i = 0; i < sig_subObj->pdflfz->ceporder; i++) {
                picoos_mem_copy((void *) &(sig_subObj->inBuf[inReadPos
                        + sizeof(picodata_itemhead_t) + sizeof(tmp_uint16) + 3
                        * i * sizeof(tmp_uint16)]),                   /*src*/
                (void *) &tmp_uint16, sizeof(tmp_uint16));            /*dest+size*/

                sig_subObj->sig_inner.F0Buff[CEPST_BUFF_SIZE-1] = (picoos_int16) tmp_uint16;/*store into newest*/
                tmp_uint16 = (picoos_int16) sig_subObj->sig_inner.F0Buff[0];                /*assign oldest*/

                /*convert in float*/
                sig_subObj->sig_inner.F0_p
                        = (tmp_uint16 ? ((picoos_single) tmp_uint16
                                / sig_subObj->scmeanLFZ) : (picoos_single) 0.0);

                if (sig_subObj->sig_inner.F0_p != (picoos_single) 0.0f) {
                    sig_subObj->sig_inner.F0_p = (picoos_single) exp(
                            (picoos_single) sig_subObj->sig_inner.F0_p);

                }
                /* voicing */
                picoos_mem_copy((void *) &(sig_subObj->inBuf[inReadPos
                        + sizeof(picodata_itemhead_t) + sizeof(tmp_uint16) + 3
                        * i * sizeof(tmp_uint16) + sizeof(tmp_uint16)]),/*src*/
                (void *) &tmp_uint16, sizeof(tmp_uint16));              /*dest+size*/

                sig_subObj->sig_inner.VoicingBuff[CEPST_BUFF_SIZE-1] = (picoos_int16) tmp_uint16;/*store into newest*/
                tmp_uint16 = (picoos_int16) sig_subObj->sig_inner.VoicingBuff[0];                /*assign oldest*/

                sig_subObj->sig_inner.voicing = (picoos_single) ((tmp_uint16
                        & 0x01) * 8 + (tmp_uint16 & 0x0e) / 2)
                        / (picoos_single) 15.0f;

                /* unrectified f0 */
                picoos_mem_copy((void *) &(sig_subObj->inBuf[inReadPos
                        + sizeof(picodata_itemhead_t) + sizeof(tmp_uint16) + 3
                        * i * sizeof(tmp_uint16) + 2 * sizeof(tmp_uint16)]),/*src*/
                (void *) &tmp_uint16, sizeof(tmp_uint16));                  /*dest+size*/

                sig_subObj->sig_inner.FuVBuff[CEPST_BUFF_SIZE-1] = (picoos_int16) tmp_uint16;/*store into newest*/
                tmp_uint16 = (picoos_int16) sig_subObj->sig_inner.FuVBuff[0];                /*assign oldest*/

                sig_subObj->sig_inner.Fuv_p = (picoos_single) tmp_uint16
                        / sig_subObj->scmeanLFZ;
                sig_subObj->sig_inner.Fuv_p = (picoos_single) EXP((double)sig_subObj->sig_inner.Fuv_p);
            }
            /*load cep values*/
            offset = inReadPos + sizeof(picodata_itemhead_t)
                    + sizeof(tmp_uint16) +
                    3 * sig_subObj->pdflfz->ceporder * sizeof(tmp_int16);

            tmp1 = sig_subObj->sig_inner.CepBuff[CEPST_BUFF_SIZE-1];   /*store into CURR */
            tmp2 = sig_subObj->sig_inner.CepBuff[0];                   /*assign oldest*/

            for (i = 0; i < sig_subObj->pdfmgc->ceporder; i++) {
                picoos_mem_copy((void *) &(sig_subObj->inBuf[offset + i
                        * sizeof(tmp_int16)]),                /*src*/
                (void *) &tmp_int16, sizeof(tmp_int16));    /*dest+size*/

                tmp1 [i] = (picoos_int32) tmp_int16;
                sig_subObj->sig_inner.wcep_pI[i] = (picoos_int32) tmp2[i];
            }

            if (sig_subObj->inBuf[inReadPos+ 3] > sig_subObj->inBuf[inReadPos+ 2]*2 + 8) {
                /*load phase values*/
                /*get the index*/
                picoos_mem_copy((void *) &(sig_subObj->inBuf[offset + sig_subObj->pdfmgc->ceporder
                        * sizeof(tmp_int16)]),                /*src*/
                (void *) &tmp_int16, sizeof(tmp_int16));    /*dest+size*/

                /*store into buffers*/
                tmp1 = sig_subObj->sig_inner.PhsBuff[PHASE_BUFF_SIZE-1];
                /*retrieve values from pdf*/
                getPhsFromPdf(this, tmp_int16, tmp1, &(sig_subObj->sig_inner.VoxBndBuff[PHASE_BUFF_SIZE-1]));
            } else {
                /* no support for phase found */
                sig_subObj->sig_inner.VoxBndBuff[PHASE_BUFF_SIZE-1] = 0;
            }

            /*pitch modifier*/
            sig_subObj->sig_inner.F0_p *= sig_subObj->pMod;
            sig_subObj->sig_inner.Fuv_p *= sig_subObj->pMod;
            if (sig_subObj->sig_inner.F0_p > 0.0f) {
                sig_subObj->sig_inner.voiced_p = 1;
            } else {
                sig_subObj->sig_inner.voiced_p = 0;
            }
            sig_subObj->sig_inner.n_available++;
            if (sig_subObj->sig_inner.n_available>3)  sig_subObj->sig_inner.n_available = 3;

            if (sig_subObj->sig_inner.n_available < 3) {
                return PICO_STEP_BUSY;
            }

            sig_subObj->innerProcState = 3;
            return PICO_STEP_BUSY;

        case 3:
            /*Convert from mfcc to power spectrum*/
            save_transition_frame(&(sig_subObj->sig_inner));
            mel_2_lin_lookup(&(sig_subObj->sig_inner), sig_subObj->scmeanpowMGC);
            sig_subObj->innerProcState += 1;
            return PICO_STEP_BUSY;

        case 4:
            /*Reconstruct PHASE SPECTRUM */
            phase_spec2(&(sig_subObj->sig_inner));
            sig_subObj->innerProcState += 1;
            return PICO_STEP_BUSY;

        case 5:
            /*Prepare Envelope spectrum for inverse FFT*/
            env_spec(&(sig_subObj->sig_inner));
            sig_subObj->innerProcState += 1;
            return PICO_STEP_BUSY;

        case 6:
            /*Generate the impulse response of the vocal tract */
            impulse_response(&(sig_subObj->sig_inner));
            sig_subObj->innerProcState += 1;
            return PICO_STEP_BUSY;

        case 7:
            /*Sum up N impulse responses according to excitation  */
            td_psola2(&(sig_subObj->sig_inner));
            sig_subObj->innerProcState += 1;
            return PICO_STEP_BUSY;

        case 8:
            /*Ovladd */
            overlap_add(&(sig_subObj->sig_inner));
            sig_subObj->innerProcState += 1;
            return PICO_STEP_BUSY;

        case 9:
            /*-----------------------------------------
             Save the output FRAME item (0:hop-1)
             swap remaining buffer
             ---------------------------------------------*/
            n_frames = 2;
            *numoutb = 0;
            hop_p_half = (sig_subObj->sig_inner.hop_p) / 2;
            for (n_count = 0; n_count < n_frames; n_count++) {
                sig_subObj->outBuf[outWritePos]
                        = (picoos_uint8) PICODATA_ITEM_FRAME;
                sig_subObj->outBuf[outWritePos + 1]
                        = (picoos_uint8) (hop_p_half);
                sig_subObj->outBuf[outWritePos + 2]
                        = (picoos_uint8) (sig_subObj->nNumFrame % ((hop_p_half)));
                sig_subObj->outBuf[outWritePos + 3]
                        = (picoos_uint8) sig_subObj->sig_inner.hop_p;
                s_data = (picoos_int16 *) &(sig_subObj->outBuf[outWritePos + 4]);

                /*range control and clipping*/
                mlt = (picoos_int32) ((sig_subObj->fSampNorm * sig_subObj->vMod)
                        * PICODSP_END_FLOAT_NORM);
                t1 = &(sig_subObj->sig_inner.WavBuff_p[n_count * (hop_p_half)]);
                for (n_i = 0; n_i < hop_p_half; n_i++) { /*Normalization*/
                    f_data = *t1++ * mlt;
                    if (f_data >= 0)
                        f_data >>= 14;
                    else
                        f_data = -(-f_data >> 14);
                    if (f_data > PICOSIG_MAXAMP)
                        f_data = PICOSIG_MAXAMP;
                    if (f_data < PICOSIG_MINAMP)
                        f_data = PICOSIG_MINAMP;
                    *s_data = (picoos_int16) (f_data);
                    s_data++;
                }
                sig_subObj->nNumFrame = sig_subObj->nNumFrame + 1;
                *numoutb += ((picoos_int16) n_i * sizeof(picoos_int16)) + 4;
                outWritePos += *numoutb;
            }/*end for n_count*/
            /*Swap remaining buffer*/
            cnt = sig_subObj->sig_inner.m2_p - sig_subObj->sig_inner.hop_p;
            tmp1 = sig_subObj->sig_inner.WavBuff_p;
            tmp2
                    = &(sig_subObj->sig_inner.WavBuff_p[sig_subObj->sig_inner.hop_p]);
            FAST_DEVICE(cnt,*(tmp1++)=*(tmp2++);)
            ;
            cnt = sig_subObj->sig_inner.m2_p - (sig_subObj->sig_inner.m2_p
                    - sig_subObj->sig_inner.hop_p);
            FAST_DEVICE(cnt,*(tmp1++)=0;)
            ;
            sig_subObj->innerProcState = 0; /*reset to step 0*/
            sig_subObj->nNumFrame += 2;
            return PICO_OK;
        default:
            return PICO_ERR_OTHER;
    }
    return PICO_ERR_OTHER;
}/*sigProcess*/

/**
 * selects items to be dealth with by this PU
 * @param    item : pointer to current item head
 * @return  TRUE : item should be managed
 * @return  FALSE : item should be passed on next PU
 * @remarks item pointed to by *item should be already valid
 * @callgraph
 * @callergraph
 */
static pico_status_t sig_deal_with(const picoos_uint8 *item)
{
    picodata_itemhead_t head;
    pico_status_t s_result;
    s_result = FALSE;
    head.type = item[0];
    head.info1 = item[1];
    head.info2 = item[2];
    head.len = item[3];
    switch (head.type) {
        case PICODATA_ITEM_FRAME_PAR:
            /*the only item that is managed by sig, so far, is "FRAME_PAR"*/
            s_result = TRUE;
            break;
        case PICODATA_ITEM_CMD:
            if ((head.info1 == PICODATA_ITEMINFO1_CMD_PLAY) || (head.info1
                    == PICODATA_ITEMINFO1_CMD_SAVE) || (head.info1
                    == PICODATA_ITEMINFO1_CMD_UNSAVE)) {
                if (head.info2 == PICODATA_ITEMINFO2_CMD_TO_SIG) {
                    return TRUE;
                }
            }
            if ((head.info1 == PICODATA_ITEMINFO1_CMD_PITCH) || (head.info1
                    == PICODATA_ITEMINFO1_CMD_VOLUME) || (head.info1
                    == PICODATA_ITEMINFO1_CMD_SPEAKER)) {
                return TRUE;
            }
            break;
        default:
            break;
    }
    return s_result;
} /*sig_deal_with*/

/**
 * selects items to be managed as commands by this PU
 * @param    item : pointer to current item head
 * @return  TRUE : item should be managed as a command
 * @return  FALSE : item is not a PU command
 * @remarks item pointed to by *item should be already valid
 * @callgraph
 * @callergraph
 */
static pico_status_t sig_is_command(const picoos_uint8 *item)
{
    picodata_itemhead_t head;
    head.type = item[0];
    head.info1 = item[1];
    head.info2 = item[2];
    head.len = item[3];
    switch (head.type) {
        case PICODATA_ITEM_CMD:
            if ((head.info1 == PICODATA_ITEMINFO1_CMD_PLAY) || (head.info1
                    == PICODATA_ITEMINFO1_CMD_SAVE) || (head.info1
                    == PICODATA_ITEMINFO1_CMD_UNSAVE)) {
                if (head.info2 == PICODATA_ITEMINFO2_CMD_TO_SIG) {
                    return TRUE;
                }
            }
            if ((head.info1 == PICODATA_ITEMINFO1_CMD_PITCH) || (head.info1
                    == PICODATA_ITEMINFO1_CMD_VOLUME) || (head.info1
                    == PICODATA_ITEMINFO1_CMD_SPEAKER)) {
                return TRUE;
            }
            break;
        default:
            break;
    }
    return FALSE;
} /*sig_is_command*/

/**
 * performs a step of the sig processing
 * @param    this : pointer to current PU (Control Unit)
 * @param    mode : mode for the PU
 * @param    numBytesOutput : pointer to number of bytes produced (output)
 * @param    this : pointer to current PU (Control Unit)
 * @return  one of the "picodata_step_result_t" values
 * @callgraph
 * @callergraph
 */
static picodata_step_result_t sigStep(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * numBytesOutput)
{
    register sig_subobj_t * sig_subObj;
    pico_status_t s_result;
    picoos_bool b_res;
    pico_status_t s_deal_with;
    picoos_uint16 blen;
    picoos_uint16 numinb, numoutb;
    pico_status_t rv;
    picoos_int16 *s_data;
    picoos_int16 hop_p_half;
    picoos_uint32 n_samp, n_i;
    picoos_char s_temp_file_name[255];
    picoos_uint32 n_start, n_fram, n_bytes;
    picoos_single f_value;
    picoos_uint16 n_value;
    picoos_uint32 n_pos;
    /*wav file play volume control*/
    picoos_int16 *s_t1;
    picoos_int32 sf_data, sf_mlt;
    picoos_uint32 sf;
    picoos_encoding_t enc;
    picoos_uint32 numSamples;

    numinb = 0;
    numoutb = 0;
    rv = PICO_OK;
    s_result = PICO_OK;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    sig_subObj = (sig_subobj_t *) this->subObj;

    /*Init number of output bytes*/
    *numBytesOutput = 0;

    mode = mode; /* avoid warning "var not used in this function" */

    while (1) { /* exit via return */

        PICODBG_DEBUG(("picosig.sigStep -- doing state %i",sig_subObj->procState));

        switch (sig_subObj->procState) {

            case PICOSIG_COLLECT:
                /* ************** item collector ***********************************/
                /*collecting items from the PU input buffer*/
                s_result = picodata_cbGetItem(this->cbIn,
                        &(sig_subObj->inBuf[sig_subObj->inWritePos]),
                        sig_subObj->inBufSize - sig_subObj->inWritePos, &blen);

                PICODBG_DEBUG(("picosig.sigStep -- got item, status: %d",rv));

                if (s_result == PICO_EOF) {
                    /*no items available : remain in state 0 and return idle*/
                    return PICODATA_PU_IDLE;
                }
                if ((PICO_OK == s_result) && (blen > 0)) {
                    /* we now have one item : CHECK IT */
                    s_result = picodata_is_valid_item(
                            &(sig_subObj->inBuf[sig_subObj->inWritePos]), blen);
                    if (s_result != TRUE) {
                        PICODBG_DEBUG(("picosig.sigStep -- item is not valid: discard"));
                        /*Item not valid : remain in state PICOSIG_COLLECT*/
                        return PICODATA_PU_BUSY;
                    }
                    /*item ok: it could be sent to schedule state*/
                    sig_subObj->inWritePos += blen;
                    sig_subObj->needMoreInput = FALSE;
                    sig_subObj->procState = PICOSIG_SCHEDULE;
                    /* uncomment next to split into two steps */
                    return PICODATA_PU_ATOMIC;
                }
                /*no EOF, no OK --> errors : remain in state PICOSIG_COLLECT and return error*/
                return PICODATA_PU_ERROR;
                break;

            case PICOSIG_SCHEDULE:
                /* *************** item processing ***********************************/
                numinb = PICODATA_ITEM_HEADSIZE
                        + sig_subObj->inBuf[sig_subObj->inReadPos + 3];

                /*verify that current item has to be dealth with by this PU*/
                s_deal_with = sig_deal_with(
                        &(sig_subObj->inBuf[sig_subObj->inReadPos]));

                switch (s_deal_with) {

                    case TRUE:
                        /* we have to manage this item */
                        if (FALSE == sig_is_command(
                                &(sig_subObj->inBuf[sig_subObj->inReadPos])))
                        {
                            /*no commands, item to deal with : switch to process state*/
                            sig_subObj->procState = PICOSIG_PROCESS;
                            sig_subObj->retState = PICOSIG_COLLECT;
                            return PICODATA_PU_BUSY; /*data still to process or to feed*/

                        } else {

                            /*we need to manage this item as a SIG command-item*/

                            switch (sig_subObj->inBuf[sig_subObj->inReadPos + 1]) {

                                case PICODATA_ITEMINFO1_CMD_PLAY:
                                    /*CMD recognized : consume the command */
                                    sig_subObj->inReadPos += numinb;
                                    if (sig_subObj->inReadPos
                                            >= sig_subObj->inWritePos) {
                                        sig_subObj->inReadPos = 0;
                                        sig_subObj->inWritePos = 0;
                                    }
                                    /*default next state setting*/
                                    sig_subObj->procState =
                                       sig_subObj->retState = PICOSIG_COLLECT;

                                    /*--------- wav file play management --------------*/
                                    if (sig_subObj->sInSDFile != NULL) {
                                        /*input wav file is already open : return*/
                                        return PICODATA_PU_BUSY;
                                    }
                                    /*get temp file name*/
                                    picoos_strlcpy(
                                            (picoos_char*) s_temp_file_name,
                                            (picoos_char*) &(sig_subObj->inBuf[sig_subObj->inReadPos
                                                    + 4]),
                                            sig_subObj->inBuf[sig_subObj->inReadPos
                                                    + 3] + 1);
                                    /*avoid input/output file name clashes*/
                                    if (sig_subObj->sOutSDFile != NULL) {
                                        if (picoos_strncmp(
                                                (picoos_char*) s_temp_file_name,
                                                (picoos_char*) sig_subObj->sOutSDFileName,
                                                picoos_strlen(
                                                        (picoos_char*) s_temp_file_name))
                                                == 0) {
                                            PICODBG_WARN(("input and output files has the same name!\n"));
                                            return PICODATA_PU_BUSY;
                                        }
                                    }
                                    /*actual sampled data file open*/
                                    b_res = picoos_sdfOpenIn(this->common,
                                                &(sig_subObj->sInSDFile),
                                        s_temp_file_name, &sf,
                                                &enc, &numSamples);
                                    if (b_res != TRUE) {
                                        PICODBG_DEBUG(("Error on opening file %s\n", s_temp_file_name));
                                        sig_subObj->sInSDFile = NULL;
                                        sig_subObj->sInSDFileName[0] = '\0';
                                        return PICODATA_PU_BUSY;
                                    }
                                    /*input file handle is now valid : store filename*/
                                    picoos_strlcpy(
                                            (picoos_char*) sig_subObj->sInSDFileName,
                                            (picoos_char*) s_temp_file_name,
                                            sig_subObj->inBuf[sig_subObj->inReadPos
                                                    + 3] + 1);
                                    sig_subObj->sInSDFilePos = 0;
                                    /*switch to state PLAY and return*/
                                    sig_subObj->procState =
                                        sig_subObj->retState = PICOSIG_PLAY;
                                    return PICODATA_PU_BUSY;
                                    break;

                                case PICODATA_ITEMINFO1_CMD_SAVE:
                                    /*CMD recognized : consume the command */
                                    sig_subObj->inReadPos += numinb;
                                    if (sig_subObj->inReadPos
                                            >= sig_subObj->inWritePos) {
                                        sig_subObj->inReadPos = 0;
                                        sig_subObj->inWritePos = 0;
                                    }
                                    /*prepare return state*/
                                    sig_subObj->procState = PICOSIG_COLLECT;
                                    sig_subObj->retState = PICOSIG_COLLECT;
                                    /*check about output file*/
                                    if ((sig_subObj->sOutSDFile != NULL)
                                            || (sig_subObj->outSwitch == 1)) {
                                        /*output sig file is already active : return*/
                                        return PICODATA_PU_BUSY;
                                    }
                                    /*get temp file name*/
                                    picoos_strlcpy(
                                            (picoos_char*) s_temp_file_name,
                                            (picoos_char*) &(sig_subObj->inBuf[sig_subObj->inReadPos
                                                    + 4]),
                                            sig_subObj->inBuf[sig_subObj->inReadPos
                                                    + 3] + 1);
                                    /*check extension*/
                                    if (picoos_has_extension(s_temp_file_name,
                                            PICODATA_PUTYPE_WAV_OUTPUT_EXTENSION)
                                            == FALSE){
                                        /*extension unsupported : return*/
                                        return PICODATA_PU_BUSY;
                                    }
                                    /*avoid input/output file name clashes*/
                                    if (sig_subObj->sInSDFile != NULL) {
                                        if (picoos_strncmp(
                                                (picoos_char*) sig_subObj->sInSDFileName,
                                                (picoos_char*) s_temp_file_name,
                                                picoos_strlen(
                                                        (picoos_char*) sig_subObj->sInSDFileName))
                                                == 0) {
                                            /*input and output files has the same name : do not allow opening for writing*/
                                            PICODBG_WARN(("input and output files has the same name!\n"));
                                            return PICODATA_PU_BUSY;
                                        }
                                    }
                                    /*try to open*/
                                    picoos_sdfOpenOut(this->common,
                                            &(sig_subObj->sOutSDFile),
                                            s_temp_file_name,
                                            SAMPLE_FREQ_16KHZ, PICOOS_ENC_LIN);
                                    if (sig_subObj->sOutSDFile == NULL) {
                                        PICODBG_DEBUG(("Error on opening file %s\n", sig_subObj->sOutSDFileName));
                                        sig_subObj->outSwitch = 0;
                                        sig_subObj->sOutSDFileName[0] = '\0';
                                    } else {
                                        /*open OK*/
                                        sig_subObj->outSwitch = 1;
                                        /*store output filename*/
                                        picoos_strlcpy(
                                                (picoos_char*) sig_subObj->sOutSDFileName,
                                                (picoos_char*) s_temp_file_name,
                                                sig_subObj->inBuf[sig_subObj->inReadPos + 3] + 1);
                                    }
                                    return PICODATA_PU_BUSY;
                                    break;

                                case PICODATA_ITEMINFO1_CMD_UNSAVE:
                                    /*CMD recognized : consume the command */
                                    sig_subObj->inReadPos += numinb;
                                    if (sig_subObj->inReadPos
                                            >= sig_subObj->inWritePos) {
                                        sig_subObj->inReadPos = 0;
                                        sig_subObj->inWritePos = 0;
                                    }
                                    /*prepare return state*/
                                    sig_subObj->procState = PICOSIG_COLLECT;
                                    sig_subObj->retState = PICOSIG_COLLECT;
                                    /*close the output file if any*/
                                    if ((sig_subObj->sOutSDFile == NULL)
                                            || (sig_subObj->outSwitch == 0)) {
                                        /*output sig file is not active : return*/
                                        PICODBG_DEBUG(("Error on requesting a binary samples file output closing : no file output handle exist\n"));
                                        return PICODATA_PU_BUSY;
                                    }
                                    picoos_sdfCloseOut(this->common,
                                            &(sig_subObj->sOutSDFile));
                                    sig_subObj->outSwitch = 0;
                                    sig_subObj->sOutSDFile = NULL;
                                    sig_subObj->sOutSDFileName[0] = '\0';
                                    return PICODATA_PU_BUSY;
                                    break;

                                case PICODATA_ITEMINFO1_CMD_PITCH:
                                case PICODATA_ITEMINFO1_CMD_VOLUME:
                                case PICODATA_ITEMINFO1_CMD_SPEAKER:
                                    n_pos = 4;
                                    picoos_read_mem_pi_uint16(
                                            &(sig_subObj->inBuf[sig_subObj->inReadPos]),
                                            &n_pos, &n_value);
                                    b_res = FALSE;
                                    switch (sig_subObj->inBuf[sig_subObj->inReadPos + 2]) {
                                        case 'a' :
                                        /*absloute modifier*/
                                        f_value = (picoos_single) n_value
                                                / (picoos_single) 100.0f;
                                            b_res = TRUE;
                                            break;
                                        case 'r' :
                                            /*relative modifier*/
                                            f_value = (picoos_single) n_value
                                                    / (picoos_single) 1000.0f;
                                            b_res = TRUE;
                                            break;
                                        default :
                                            f_value = (picoos_single)0; /*avoid warnings*/
                                            break;
                                    }
                                    if (b_res) {
                                        switch (sig_subObj->inBuf[sig_subObj->inReadPos + 1]) {
                                            case PICODATA_ITEMINFO1_CMD_PITCH :
                                            sig_subObj->pMod = f_value;
                                                break;
                                            case PICODATA_ITEMINFO1_CMD_VOLUME :
                                            sig_subObj->vMod = f_value;
                                                break;
                                            case PICODATA_ITEMINFO1_CMD_SPEAKER :
                                            sig_subObj->sMod = f_value;
                                            sig_subObj->sig_inner.sMod_p
                                                    = sig_subObj->sMod;
                                            /*call the function needed to initialize the speaker factor*/
                                            mel_2_lin_init(
                                                    &(sig_subObj->sig_inner));
                                                 break;
                                            default :
                                                break;
                                        }
                                    }

                                    /*CMD recognized : consume the command */
                                    sig_subObj->inReadPos += numinb;
                                    if (sig_subObj->inReadPos
                                            >= sig_subObj->inWritePos) {
                                        sig_subObj->inReadPos = 0;
                                        sig_subObj->inWritePos = 0;
                                    }
                                    /*prepare proc state*/
                                    sig_subObj->procState = PICOSIG_COLLECT;
                                    sig_subObj->retState = PICOSIG_COLLECT;
                                    return PICODATA_PU_BUSY;
                                    break;
                                default:
                                    break;
                            }/*switch command type*/
                        } /*end if is command*/
                        break;

                    case FALSE:

                        /*we DO NOT have to deal with this item on this PU.
                         * Normally these are still alive boundary or flush items*/
                        /*copy item from PU input to PU output buffer,
                         * i.e. make it ready to FEED*/
                        s_result = picodata_copy_item(
                                &(sig_subObj->inBuf[sig_subObj->inReadPos]),
                                numinb,
                                &(sig_subObj->outBuf[sig_subObj->outWritePos]),
                                sig_subObj->outBufSize - sig_subObj->outWritePos,
                                &numoutb);
                        if (s_result != PICO_OK) {
                            /*item not prepared properly to be sent to next PU :
                             * do not change state and retry next time*/
                            sig_subObj->procState = PICOSIG_SCHEDULE;
                            sig_subObj->retState = PICOSIG_COLLECT;
                            return PICODATA_PU_BUSY; /*data still to process or to feed*/
                        }

                        /*if end of sentence reset number of frames(only needed for debugging purposes)*/
                        if ((sig_subObj->inBuf[sig_subObj->inReadPos]
                                == PICODATA_ITEM_BOUND)
                                && ((sig_subObj->inBuf[sig_subObj->inReadPos + 1]
                                        == PICODATA_ITEMINFO1_BOUND_SEND)
                                        || (sig_subObj->inBuf[sig_subObj->inReadPos
                                                + 1]
                                                == PICODATA_ITEMINFO1_BOUND_TERM))) {
                            PICODBG_INFO(("End of sentence - Processed frames : %d",
                                            sig_subObj->nNumFrame));
                            sig_subObj->nNumFrame = 0;
                        }

                        /*item processed and put in oputput buffer : consume the item*/
                            sig_subObj->inReadPos += numinb;
                        sig_subObj->outWritePos += numoutb;
                            if (sig_subObj->inReadPos >= sig_subObj->inWritePos) {
                                /* inBuf exhausted */
                                sig_subObj->inReadPos = 0;
                                sig_subObj->inWritePos = 0;
                                sig_subObj->needMoreInput = FALSE;
                            }
                            sig_subObj->procState = PICOSIG_FEED;
                            sig_subObj->retState = PICOSIG_COLLECT;
                        return PICODATA_PU_BUSY; /*data still to process or to feed*/
                        break;

                    default:
                        break;
                }/*end switch s_deal_with*/

                break; /*end case sig_schedule*/

            case PICOSIG_PROCESS:
                /* *************** item processing ***********************************/
                numinb = PICODATA_ITEM_HEADSIZE
                        + sig_subObj->inBuf[sig_subObj->inReadPos + 3];

                /*Process a full item*/
                s_result = sigProcess(this, sig_subObj->inReadPos, numinb,
                        sig_subObj->outWritePos, &numoutb);

                if (s_result == PICO_OK) {
                    sig_subObj->inReadPos += numinb;
                    if (sig_subObj->inReadPos >= sig_subObj->inWritePos) {
                        sig_subObj->inReadPos = 0;
                        sig_subObj->inWritePos = 0;
                        sig_subObj->needMoreInput = FALSE;
                    }
                    sig_subObj->outWritePos += numoutb;
                    sig_subObj->procState = PICOSIG_FEED;
                    sig_subObj->retState = PICOSIG_COLLECT;
                    PICODBG_DEBUG(("picosig.sigStep -- leaving PICO_PROC, inReadPos = %i, outWritePos = %i",sig_subObj->inReadPos, sig_subObj->outWritePos));
                    return PICODATA_PU_BUSY; /*data to feed*/
                }
                return PICODATA_PU_BUSY; /*data still to process : remain in PROCESS STATE*/
                break;

            case PICOSIG_PLAY:

                /*management of wav file play*/
                s_data = (picoos_int16 *) &(sig_subObj->outBuf[sig_subObj->outWritePos + 4]);
                hop_p_half = sig_subObj->sig_inner.hop_p / 2;
                /*read directly into PU output buffer*/
                n_samp = hop_p_half;
                b_res = picoos_sdfGetSamples(sig_subObj->sInSDFile,
                        sig_subObj->sInSDFilePos, &n_samp, s_data);
                sig_subObj->sInSDFilePos += n_samp;

                if ((FALSE == b_res) || (0 == n_samp)) {
                    /*file play is complete or file read error*/
                    picoos_sdfCloseIn(this->common, &(sig_subObj->sInSDFile));
                    sig_subObj->sInSDFile = NULL;
                    sig_subObj->sInSDFileName[0] = '\0';
                    sig_subObj->procState = PICOSIG_COLLECT;
                    sig_subObj->retState = PICOSIG_COLLECT;
                    return PICODATA_PU_BUSY; /*check if data in input buffer*/
                }
                /*-----------------------------------*/
                /*Volume control of wav file playback*/
                /*    (code borrowed from sigProcess)*/
                /*Volume mod and clipping control    */
                /*     directly into PU output buffer*/
                /*-----------------------------------*/
                sf_mlt = (picoos_int32) ((sig_subObj->vMod) * 16.0f);
                s_t1 = &(s_data[0]);

                for (n_i = 0; n_i < n_samp; n_i++) {
                    if (*s_t1 != 0) {
                        sf_data = (picoos_int32) (*s_t1) * sf_mlt;
                        sf_data >>= 4;
                        if (sf_data > PICOSIG_MAXAMP) {
                            sf_data = PICOSIG_MAXAMP;
                        } else if (sf_data < PICOSIG_MINAMP) {
                            sf_data = PICOSIG_MINAMP;
                        }
                        *s_t1 = (picoos_int16) (sf_data);
                    }
                    s_t1++;
                }
                /*Add header info*/
                sig_subObj->outBuf[sig_subObj->outWritePos]
                        = (picoos_uint8) PICODATA_ITEM_FRAME;
                sig_subObj->outBuf[sig_subObj->outWritePos + 1]
                        = (picoos_uint8) n_samp;
                sig_subObj->outBuf[sig_subObj->outWritePos + 2]
                        = (picoos_uint8) (sig_subObj->nNumFrame % (hop_p_half)); /*number of frame % 64*/
                sig_subObj->outBuf[sig_subObj->outWritePos + 3]
                        = (picoos_uint8) n_samp * 2;
                /*Item content*/
                sig_subObj->outWritePos += (n_samp * sizeof(picoos_int16)) + 4; /*including header*/
                sig_subObj->procState = PICOSIG_FEED;
                sig_subObj->retState = PICOSIG_PLAY;
                break;

            case PICOSIG_FEED:
                /* ************** item output/feeding ***********************************/
                switch (sig_subObj->outSwitch) {
                    case 0:
                        /*feeding items to PU output buffer*/
                        s_result = picodata_cbPutItem(this->cbOut,
                                &(sig_subObj->outBuf[sig_subObj->outReadPos]),
                                sig_subObj->outWritePos - sig_subObj->outReadPos,
                                &numoutb);
                        break;
                    case 1:
                        /*feeding items to file*/
                        if (sig_subObj->outBuf[sig_subObj->outReadPos]
                                == PICODATA_ITEM_FRAME) {
                            if ((sig_subObj->sOutSDFile) != NULL) {
                                n_start = (picoos_uint32) (sig_subObj->outReadPos)
                                                + PICODATA_ITEM_HEADSIZE;
                                n_bytes = (picoos_uint32) sig_subObj->outBuf[(sig_subObj->outReadPos)
                                                + PICODATA_ITEMIND_LEN];
                                n_fram = (picoos_uint32) sig_subObj->outBuf[(sig_subObj->outReadPos)
                                                + PICODATA_ITEMIND_INFO2];
                                if (picoos_sdfPutSamples(
                                        sig_subObj->sOutSDFile,
                                        n_bytes / 2,
                                        (picoos_int16*) &(sig_subObj->outBuf[n_start]))) {
                                    PICODBG_DEBUG(("Nframe:%d - Nbytes %d\n", n_fram, n_bytes));
                                    numoutb = n_bytes + 4;
                                    s_result = PICO_OK;
                                    /* also feed item to next PU */
                                    s_result = picodata_cbPutItem(
                                                    this->cbOut,
                                                    &(sig_subObj->outBuf[sig_subObj->outReadPos]),
                                                    sig_subObj->outWritePos
                                                            - sig_subObj->outReadPos,
                                                    &numoutb);
                                } else {
                                    /*write error : close file + cleanup handles*/
                                    if (sig_subObj->sOutSDFile != NULL) {
                                        picoos_sdfCloseOut(this->common, &(sig_subObj->sOutSDFile));
                                        sig_subObj->sOutSDFile = NULL;
                                    }
                                    sig_subObj->sOutSDFileName[0] = '\0';
                                    sig_subObj->outSwitch = 0;
                                    PICODBG_DEBUG(("Error in writing :%d bytes to output file %s\n", numoutb, &(sig_subObj->sOutSDFileName[0])));
                                    s_result = PICO_ERR_OTHER;
                                }
                            }
                        } else {
                            /*continue to feed following PU with items != FRAME */
                            s_result = picodata_cbPutItem(
                                            this->cbOut,
                                            &(sig_subObj->outBuf[sig_subObj->outReadPos]),
                                sig_subObj->outWritePos  - sig_subObj->outReadPos,
                                            &numoutb);
                        }
                        break;
                    default:
                        s_result = PICO_ERR_OTHER;
                        break;
                }
                PICODBG_DEBUG(("picosig.sigStep -- put item, status: %d",s_result));

                if (PICO_OK == s_result) {

                    sig_subObj->outReadPos += numoutb;
                    *numBytesOutput = numoutb;
                    /*-------------------------*/
                    /*reset the output pointers*/
                    /*-------------------------*/
                    if (sig_subObj->outReadPos >= sig_subObj->outWritePos) {
                        sig_subObj->outReadPos = 0;
                        sig_subObj->outWritePos = 0;
                        sig_subObj->procState = sig_subObj->retState;
                    }
                    return PICODATA_PU_BUSY;

                } else if (PICO_EXC_BUF_OVERFLOW == s_result) {

                    PICODBG_DEBUG(("picosig.sigStep ** feeding, overflow, PICODATA_PU_OUT_FULL"));
                    return PICODATA_PU_OUT_FULL;

                } else if ((PICO_EXC_BUF_UNDERFLOW == s_result)
                        || (PICO_ERR_OTHER == s_result)) {

                    PICODBG_DEBUG(("picosig.sigStep ** feeding problem, discarding item"));
                    sig_subObj->outReadPos = 0;
                    sig_subObj->outWritePos = 0;
                    sig_subObj->procState = sig_subObj->retState;
                    return PICODATA_PU_ERROR;

                }
                break;
            default:
                /*NOT feeding items*/
                s_result = PICO_EXC_BUF_IGNORE;
                break;
        }/*end switch*/
        return PICODATA_PU_BUSY; /*check if there is more data to process after feeding*/

    }/*end while*/
    return PICODATA_PU_IDLE;
}/*sigStep*/

#ifdef __cplusplus
}
#endif

/* Picosig.c end */
