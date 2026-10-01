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
 * @file picoctrl.c
 *
 * Control PU -- Implementation
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
#include "picodata.h"
#include "picorsrc.h"

/* processing unit definitions */
#include "picotok.h"
#include "picopr.h"
#include "picowa.h"
#include "picosa.h"
#include "picoacph.h"
#include "picospho.h"
#include "picopam.h"
#include "picocep.h"
#include "picosig.h"
#if defined(PICO_DEVEL_MODE)
#include "../history/picosink.h"
#endif

#include "picoctrl.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/**
 * @addtogroup picoctrl
 * @b Control
 * The "control" is a processing unit (PU) that contains and governs a sequence of sub-PUs
 * (TTS processing chain).
 * At each step (ctrlStep) it passes control to one of the sub-PUs (currrent PU). It may re-assign
 * the role of "current PU" to another sub-PU, according to the status information returned from each PU.
 */

/*----------------------------------------------------------
 *  object   : Control
 *  shortcut     : ctrl
 *  derived from : picodata_ProcessingUnit
 *  implements a ProcessingUnit by creating and controlling
 *  a sequence of Processing Units (of possibly different
 *  implementations) exchanging data via CharBuffers
 * ---------------------------------------------------------*/
/* control sub-object */
typedef struct ctrl_subobj {
    picoos_uint8 numProcUnits;
    picoos_uint8 curPU;
    picoos_uint8 lastItemTypeProduced;
    picodata_ProcessingUnit procUnit [PICOCTRL_MAX_PROC_UNITS];
    picodata_step_result_t procStatus [PICOCTRL_MAX_PROC_UNITS];
    picodata_CharBuffer procCbOut [PICOCTRL_MAX_PROC_UNITS];
} ctrl_subobj_t;

/**
 * performs Control PU initialization
 * @param    this : pointer to Control PU
 * @return    PICO_OK : processing done
 * @return    PICO_ERR_OTHER : init error
 * @callgraph
 * @callergraph
 */
static pico_status_t ctrlInitialize(register picodata_ProcessingUnit this, picoos_int32 resetMode) {
    register ctrl_subobj_t * ctrl;
    pico_status_t status= PICO_OK;
    picoos_int8 i;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    ctrl = (ctrl_subobj_t *) this->subObj;
    ctrl->curPU = 0;
    ctrl->lastItemTypeProduced=0;    /*no item produced by default*/
    status = PICO_OK;
    for (i = 0; i < ctrl->numProcUnits; i++) {
        if (PICO_OK == status) {
            status = ctrl->procUnit[i]->initialize(ctrl->procUnit[i], resetMode);
            PICODBG_DEBUG(("(re-)initializing procUnit[%i] returned status %i",i, status));
        }
        if (PICO_OK == status) {
            status = picodata_cbReset(ctrl->procCbOut[i]);
            PICODBG_DEBUG(("(re-)initializing procCbOut[%i] returned status %i",i, status));
        }
    }
    if (PICO_OK != status) {
        picoos_emRaiseException(this->common->em,status,NULL,(picoos_char*)"problem (re-)initializing the engine");
    }
    return status;
}/*ctrlInitialize*/


/**
 * performs one processing step
 * @param    this : pointer to Control PU
 * @param    mode : activation mode (unused)
 * @param    bytesOutput : number of bytes produced during this step (output)
 * @return    PICO_OK : processing done
 * @return    PICO_EXC_OUT_OF_MEM : no more memory available
 * @return    PICO_ERR_OTHER : other error
 * @callgraph
 * @callergraph
 */
static picodata_step_result_t ctrlStep(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * bytesOutput) {
    /* rules/invariants:
     * - all pu's above current have status idle except possibly pu+1, which may  be busy.
     *   (The latter is set if any pu->step produced output)
     * - a pu returns idle iff its cbIn is empty and it has no more data ready for output */

    register ctrl_subobj_t * ctrl = (ctrl_subobj_t *) this->subObj;
    picodata_step_result_t status;
    picoos_uint16 puBytesOutput;
#if defined(PICO_DEVEL_MODE)
    picoos_uint8  btype;
#endif

    *bytesOutput = 0;
    ctrl->lastItemTypeProduced=0; /*no item produced by default*/

    /* --------------------- */
    /* do step of current pu */
    /* --------------------- */
    status = ctrl->procStatus[ctrl->curPU] = ctrl->procUnit[ctrl->curPU]->step(
            ctrl->procUnit[ctrl->curPU], mode, &puBytesOutput);

    if (puBytesOutput) {

#if defined(PICO_DEVEL_MODE)
        /*store the type of item produced*/
        btype =  picodata_cbGetFrontItemType(ctrl->procUnit[ctrl->curPU]->cbOut);
        ctrl->lastItemTypeProduced=(picoos_uint8)btype;
#endif

        if (ctrl->curPU < ctrl->numProcUnits-1) {
            /* data was output to internal PU buffers : set following pu to busy */
            ctrl->procStatus[ctrl->curPU + 1] = PICODATA_PU_BUSY;
        } else {
            /* data was output to caller output buffer */
            *bytesOutput = puBytesOutput;
        }
    }
    /* recalculate state depending on pu status returned from curPU */
    switch (status) {
        case PICODATA_PU_ATOMIC:
            PICODBG_DEBUG(("got PICODATA_PU_ATOMIC"));
            return status;
            break;

        case PICODATA_PU_BUSY:
            PICODBG_DEBUG(("got PICODATA_PU_BUSY"));
            if ( (ctrl->curPU+1 < ctrl->numProcUnits) && (PICODATA_PU_BUSY
                    == ctrl->procStatus[ctrl->curPU+1])) {
                ctrl->curPU++;
            }
            return status;
            break;

        case PICODATA_PU_IDLE:
            PICODBG_DEBUG(("got PICODATA_PU_IDLE"));
            if ( (ctrl->curPU+1 < ctrl->numProcUnits) && (PICODATA_PU_BUSY
                    == ctrl->procStatus[ctrl->curPU+1])) {
                /* still data to process below */
                ctrl->curPU++;
            } else if (0 == ctrl->curPU) { /* all pu's are idle */
                /* nothing to do */
            } else { /* find non-idle pu above */
                PICODBG_DEBUG((
                    "find non-idle pu above from pu %d with status %d",
                    ctrl->curPU, ctrl->procStatus[ctrl->curPU]));
                while ((ctrl->curPU > 0) && (PICODATA_PU_IDLE
                        == ctrl->procStatus[ctrl->curPU])) {
                    ctrl->curPU--;
                }
                ctrl->procStatus[ctrl->curPU] = PICODATA_PU_BUSY;
            }
            PICODBG_DEBUG(("going to pu %d with status %d",
                           ctrl->curPU, ctrl->procStatus[ctrl->curPU]));
            /*update last scheduled PU*/
            return ctrl->procStatus[ctrl->curPU];
            break;

        case PICODATA_PU_OUT_FULL:
            PICODBG_DEBUG(("got PICODATA_PU_OUT_FULL"));
            if (ctrl->curPU+1 < ctrl->numProcUnits) { /* let pu below empty buffer */
                ctrl->curPU++;
                ctrl->procStatus[ctrl->curPU] = PICODATA_PU_BUSY;
            } else {
                /* nothing more to do, out_full will be returned to caller */
            }
            return ctrl->procStatus[ctrl->curPU];
            break;
        default:
            return PICODATA_PU_ERROR;
            break;
    }
}/*ctrlStep*/

/**
 * terminates Control PU
 * @param    this : pointer to Control PU
 * @return    PICO_OK : processing done
 * @return    PICO_ERR_OTHER : other error
 * @callgraph
 * @callergraph
 */
static pico_status_t ctrlTerminate(register picodata_ProcessingUnit this) {
    pico_status_t status = PICO_OK;
    picoos_int16 i;
    register ctrl_subobj_t * ctrl;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    ctrl = (ctrl_subobj_t *) this->subObj;
    for (i = 0; i < ctrl->numProcUnits; i++) {
        status = ctrl->procUnit[i]->terminate(ctrl->procUnit[i]);
        PICODBG_DEBUG(("terminating procUnit[%i] returned status %i",i, status));
        if (PICO_OK != status) {
            return status;
        }
    }
    return status;
}/*ctrlTerminate*/

/**
 * deallocates Control PU's subobject
 * @param    this : pointer to Control PU
 * @return    PICO_OK : processing done
 * @return    PICO_ERR_OTHER : other error
 * @callgraph
 * @callergraph
 */
static pico_status_t ctrlSubObjDeallocate(register picodata_ProcessingUnit this,
        picoos_MemoryManager mm) {
    register ctrl_subobj_t * ctrl;
    picoos_int16 i;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    ctrl = (ctrl_subobj_t *) this->subObj;
    mm = mm;        /* fix warning "var not used in this function"*/
    /* deallocate members (procCbOut and procUnit) */
    for (i = ctrl->numProcUnits-1; i >= 0; i--) {
        picodata_disposeProcessingUnit(this->common->mm,&ctrl->procUnit[i]);
        picodata_disposeCharBuffer(this->common->mm, &ctrl->procCbOut[i]);
    }
    /* deallocate object itself */
    picoos_deallocate(this->common->mm, (void *) &this->subObj);

    return PICO_OK;
}/*ctrlSubObjDeallocate*/

/**
 * inserts a new PU in the TTS processing chain
 * @param    this : pointer to Control PU
 * @param    puType : type of the PU to be inserted
 * @param    last : if true, inserted PU is the last in the TTS processing chain
 * @return    PICO_OK : processing done
 * @return    PICO_EXC_OUT_OF_MEM : no more memory available
 * @return    PICO_ERR_OTHER : other error
 * @remarks    Calls the PU object creation method
 * @callgraph
 * @callergraph
 */
static pico_status_t ctrlAddPU(register picodata_ProcessingUnit this,
        picodata_putype_t puType,
        picoos_bool levelAwareCbOut,
        picoos_bool last)
{
    picoos_uint16 bufSize;
    register ctrl_subobj_t * ctrl;
    picodata_CharBuffer cbIn;
    picoos_uint8 newPU;
    if (this == NULL) {
        return PICO_ERR_OTHER;
    }
    ctrl = (ctrl_subobj_t *) this->subObj;
    if (ctrl == NULL) {
        return PICO_ERR_OTHER;
    }
    newPU = ctrl->numProcUnits;
    if (0 == newPU) {
        PICODBG_DEBUG(("taking cbIn of this because adding first pu"));
        cbIn = this->cbIn;
    } else {
        PICODBG_DEBUG(("taking cbIn of previous pu"));
        cbIn = ctrl->procCbOut[newPU-1];
    }
    if (last) {
        PICODBG_DEBUG(("taking cbOut of this because adding last pu"));
        ctrl->procCbOut[newPU] = this->cbOut;
    } else {
        PICODBG_DEBUG(("creating intermediate cbOut of pu[%i]", newPU));
        bufSize = picodata_get_default_buf_size(puType);
        ctrl->procCbOut[newPU] = picodata_newCharBuffer(this->common->mm,
                this->common,bufSize);

        PICODBG_DEBUG(("intermediate cbOut of pu[%i] (address %i)", newPU,
                       (picoos_uint32) ctrl->procCbOut[newPU]));
        if (NULL == ctrl->procCbOut[newPU]) {
            return PICO_EXC_OUT_OF_MEM;
        }
    }
    ctrl->procStatus[newPU] = PICODATA_PU_IDLE;
    /*...............*/
    switch (puType) {
    case PICODATA_PUTYPE_TOK:
            PICODBG_DEBUG(("creating TokenizeUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picotok_newTokenizeUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
        break;
    case PICODATA_PUTYPE_PR:
            PICODBG_DEBUG(("creating PreprocUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picopr_newPreprocUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
        break;
    case PICODATA_PUTYPE_WA:
            PICODBG_DEBUG(("creating WordAnaUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picowa_newWordAnaUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
        break;
    case PICODATA_PUTYPE_SA:
            PICODBG_DEBUG(("creating SentAnaUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picosa_newSentAnaUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
        break;
    case PICODATA_PUTYPE_ACPH:
            PICODBG_DEBUG(("creating AccPhrUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picoacph_newAccPhrUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
        break;
    case PICODATA_PUTYPE_SPHO:
            PICODBG_DEBUG(("creating SentPhoUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picospho_newSentPhoUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
            break;
    case PICODATA_PUTYPE_PAM:
            PICODBG_DEBUG(("creating PAMUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picopam_newPamUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
        break;
    case PICODATA_PUTYPE_CEP:
            PICODBG_DEBUG(("creating CepUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picocep_newCepUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
        break;
#if defined(PICO_DEVEL_MODE)
        case PICODATA_PUTYPE_SINK:
            PICODBG_DEBUG(("creating SigUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picosink_newSinkUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
        break;
#endif
        case PICODATA_PUTYPE_SIG:
            PICODBG_DEBUG(("creating SigUnit for pu %i", newPU));
            ctrl->procUnit[newPU] = picosig_newSigUnit(this->common->mm,
                    this->common, cbIn, ctrl->procCbOut[newPU], this->voice);
        break;
    default:
            ctrl->procUnit[newPU] = picodata_newProcessingUnit(
                    this->common->mm, this->common, cbIn,
                    ctrl->procCbOut[newPU], this->voice);
        break;
    }
    if (NULL == ctrl->procUnit[newPU]) {
        picodata_disposeCharBuffer(this->common->mm,&ctrl->procCbOut[newPU]);
        return PICO_EXC_OUT_OF_MEM;
    }
    ctrl->numProcUnits++;
    return PICO_OK;
}/*ctrlAddPU*/

/*forward declaration : see below for full function body*/
void picoctrl_disposeControl(picoos_MemoryManager mm,
        picodata_ProcessingUnit * this);

/**
 * initializes a control PU object
 * @param    mm : memory manager
 * @param    common : the common object
 * @param    cbIn : the input char buffer
 * @param    cbOut : the output char buffer
 * @param    voice : the voice object
 * @return    the pointer to the PU object created if OK
 * @return    PICO_EXC_OUT_OF_MEM : no more memory available
 * @return    NULL otherwise
 * @callgraph
 * @callergraph
 */
picodata_ProcessingUnit picoctrl_newControl(picoos_MemoryManager mm,
        picoos_Common common, picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut, picorsrc_Voice voice) {
    picoos_int16 i;
    register ctrl_subobj_t * ctrl;
    picodata_ProcessingUnit this = picodata_newProcessingUnit(mm, common, cbIn,
            cbOut,voice);
    if (this == NULL) {
        return NULL;
    }

    this->initialize = ctrlInitialize;
    this->step = ctrlStep;
    this->terminate = ctrlTerminate;
    this->subDeallocate = ctrlSubObjDeallocate;

    this->subObj = picoos_allocate(mm, sizeof(ctrl_subobj_t));
    if (this->subObj == NULL) {
        picoos_deallocate(mm, (void **)(void*)&this);
        return NULL;
    }

    ctrl = (ctrl_subobj_t *) this->subObj;

    for (i=0; i < PICOCTRL_MAX_PROC_UNITS; i++) {
        ctrl->procUnit[i] = NULL;
        ctrl->procStatus[i] = PICODATA_PU_IDLE;
        ctrl->procCbOut[i] = NULL;
    }
    ctrl->numProcUnits = 0;

    if (
            (PICO_OK == ctrlAddPU(this,PICODATA_PUTYPE_TOK, FALSE, /*last*/FALSE)) &&
            (PICO_OK == ctrlAddPU(this,PICODATA_PUTYPE_PR, FALSE, FALSE)) &&
            (PICO_OK == ctrlAddPU(this,PICODATA_PUTYPE_WA, FALSE, FALSE)) &&
            (PICO_OK == ctrlAddPU(this,PICODATA_PUTYPE_SA, FALSE, FALSE)) &&
            (PICO_OK == ctrlAddPU(this,PICODATA_PUTYPE_ACPH, FALSE, FALSE)) &&
            (PICO_OK == ctrlAddPU(this,PICODATA_PUTYPE_SPHO, FALSE, FALSE)) &&
            (PICO_OK == ctrlAddPU(this,PICODATA_PUTYPE_PAM, FALSE, FALSE)) &&
            (PICO_OK == ctrlAddPU(this,PICODATA_PUTYPE_CEP, FALSE, FALSE)) &&
            (PICO_OK == ctrlAddPU(this,PICODATA_PUTYPE_SIG, FALSE, TRUE))
         ) {

        /* we don't call ctrlInitialize here because ctrlAddPU does initialize the PUs allready and the only thing
         * remaining to initialize is:
         */
        ctrl->curPU = 0;
        return this;
    } else {
        picoctrl_disposeControl(this->common->mm,&this);
        return NULL;
    }

}/*picoctrl_newControl*/

/**
 * disposes a Control PU
 * @param    mm : memory manager
 * @param    this : pointer to Control PU
 *
 * @return    void
 * @callgraph
 * @callergraph
 */
void picoctrl_disposeControl(picoos_MemoryManager mm,
        picodata_ProcessingUnit * this)
{
    picodata_disposeProcessingUnit(mm, this);
}/*picoctrl_disposeControl*/

/* **************************************************************************
 *
 *      Engine
 *
 ****************************************************************************/
/** object       : Engine
 *  shortcut     : eng
 */
typedef struct picoctrl_engine {
    picoos_uint32 magic;        /* magic number used to validate handles */
    void *raw_mem;
    picoos_Common common;
    picorsrc_Voice voice;
    picodata_ProcessingUnit control;
    picodata_CharBuffer cbIn, cbOut;
} picoctrl_engine_t;


#define MAGIC_MASK 0x5069436F  /* PiCo */

#define SET_MAGIC_NUMBER(eng) \
    (eng)->magic = ((picoos_uint32) (uintptr_t) (eng)) ^ MAGIC_MASK

#define CHECK_MAGIC_NUMBER(eng) \
    ((eng)->magic == (((picoos_uint32) (uintptr_t) (eng)) ^ MAGIC_MASK))

/**
 * performs an engine reset
 * @param    this : the engine object
 * @return    PICO_OK : reset performed
 * @return    otherwise error code
 * @callgraph
 * @callergraph
 */
pico_status_t picoctrl_engReset(picoctrl_Engine this, picoos_int32 resetMode)
{
    pico_status_t status;

    if (NULL == this) {
        return PICO_ERR_NULLPTR_ACCESS;
    }
    picoos_emReset(this->common->em);

    status = this->control->terminate(this->control);
    if (PICO_OK == status) {
        status = this->control->initialize(this->control, resetMode);
    }
    if (PICO_OK == status) {
        status = picodata_cbReset(this->cbIn);
    }
    if (PICO_OK == status) {
        status = picodata_cbReset(this->cbOut);
    }
    if (PICO_OK != status) {
        picoos_emRaiseException(this->common->em,status,NULL,(picoos_char*) "problem resetting engine");
    }
    return status;
}

/**
 * checks an engine handle
 * @param    this : the engine object
 * @return    PICO_OK : reset performed
 * @return    non-zero if 'this' is a valid engine handle
 * @return  zero otherwise
 * @callgraph
 * @callergraph
 */
picoos_int16 picoctrl_isValidEngineHandle(picoctrl_Engine this)
{
    return (this != NULL) && CHECK_MAGIC_NUMBER(this);
}/*picoctrl_isValidEngineHandle*/

/**
 * creates a new engine object
 * @param    mm : memory manager to be used for this engine
 * @param    rm : resource manager to be used for this engine
 * @param    voiceName : voice definition to be used for this engine
 * @return    PICO_OK : reset performed
 * @return    new engine handle
 * @return  NULL otherwise
 * @callgraph
 * @callergraph
 */
picoctrl_Engine picoctrl_newEngine(picoos_MemoryManager mm,
        picorsrc_ResourceManager rm, const picoos_char * voiceName) {
    picoos_uint8 done= TRUE;

    picoos_uint16 bSize;

    picoos_MemoryManager engMM;
    picoos_ExceptionManager engEM;

    picoctrl_Engine this = (picoctrl_Engine) picoos_allocate(mm, sizeof(*this));

    PICODBG_DEBUG(("creating engine for voice '%s'",voiceName));

    done = (NULL != this);

    if (done) {
        this->magic = 0;
        this->common = NULL;
        this->voice = NULL;
        this->control = NULL;
        this->cbIn = NULL;
        this->cbOut = NULL;

        this->raw_mem = picoos_allocate(mm, PICOCTRL_DEFAULT_ENGINE_SIZE);
        if (NULL == this->raw_mem) {
            done = FALSE;
        }
    }

    if (done) {
        engMM = picoos_newMemoryManager(this->raw_mem, PICOCTRL_DEFAULT_ENGINE_SIZE,
                    /*enableMemProt*/ FALSE);
        done = (NULL != engMM);
    }
    if (done) {
        this->common = picoos_newCommon(engMM);
        engEM = picoos_newExceptionManager(engMM);
        done = (NULL != this->common) && (NULL != engEM);
    }
    if (done) {
        this->common->mm = engMM;
        this->common->em = engEM;

        done = (PICO_OK == picorsrc_createVoice(rm,voiceName,&(this->voice)));
    }
    if (done)  {
        bSize = picodata_get_default_buf_size(PICODATA_PUTYPE_TEXT);

        this->cbIn = picodata_newCharBuffer(this->common->mm,
                this->common, bSize);
        bSize = picodata_get_default_buf_size(PICODATA_PUTYPE_SIG);

        this->cbOut = picodata_newCharBuffer(this->common->mm,
                this->common, bSize);

        PICODBG_DEBUG(("cbOut has address %i", (picoos_uint32) this->cbOut));


        this->control = picoctrl_newControl(this->common->mm, this->common,
                this->cbIn, this->cbOut, this->voice);
        done = (NULL != this->cbIn) && (NULL != this->cbOut)
                && (NULL != this->control);
    }
    if (done) {
        SET_MAGIC_NUMBER(this);
    } else {
        if (NULL != this) {
            if (NULL != this->voice) {
                picorsrc_releaseVoice(rm,&(this->voice));
            }
            if(NULL != this->raw_mem) {
                picoos_deallocate(mm,&(this->raw_mem));
            }
            picoos_deallocate(mm,(void *)&this);
        }
    }
    return this;
}/*picoctrl_newEngine*/

/**
 * disposes an engine object
 * @param    mm : memory manager associated to the engine
 * @param    rm : resource manager associated to the engine
 * @param    this : handle of the engine to dispose
 * @return    PICO_OK : reset performed
 * @return    void
 * @callgraph
 * @callergraph
 */
void picoctrl_disposeEngine(picoos_MemoryManager mm, picorsrc_ResourceManager rm,
        picoctrl_Engine * this)
{
    if (NULL != (*this)) {
        if (NULL != (*this)->voice) {
            picorsrc_releaseVoice(rm,&((*this)->voice));
        }
        if(NULL != (*this)->control) {
            picoctrl_disposeControl((*this)->common->mm,&((*this)->control));
        }
        if(NULL != (*this)->raw_mem) {
            picoos_deallocate(mm,&((*this)->raw_mem));
        }
        (*this)->magic ^= 0xFFFEFDFC;
        picoos_deallocate(mm,(void **)this);
    }
}/*picoctrl_disposeEngine*/

/**
 * resets the exception manager of an engine
 * @param    this : handle of the engine
 * @return    void
 * @callgraph
 * @callergraph
 */
void picoctrl_engResetExceptionManager(
        picoctrl_Engine this
        )
{
        picoos_emReset(this->common->em);
}/*picoctrl_engResetExceptionManager*/

/**
 * returns the engine common pointer
 * @param    this : handle of the engine
 * @return    PICO_OK : reset performed
 * @return    the engine common pointer
 * @return    NULL if error
 * @callgraph
 * @callergraph
 */
picoos_Common picoctrl_engGetCommon(picoctrl_Engine this) {
    if (NULL == this) {
        return NULL;
    } else {
        return this->common;
    }
}/*picoctrl_engGetCommon*/

/**
 * feed raw 'text' into 'engine'. text may contain '\\0'.
 * @param    this : handle of the engine
 * @param    text : the input text
 * @param    textSize : size of the input text
 * @param    *bytesPut : the number of bytes effectively consumed from 'text'.
 * @return    PICO_OK : feeding succeded
 * @return    PICO_ERR_OTHER : if error
 * @callgraph
 * @callergraph
 */
pico_status_t picoctrl_engFeedText(picoctrl_Engine this,
        picoos_char * text,
        picoos_int16 textSize, picoos_int16 * bytesPut) {
    if (NULL == this) {
        return PICO_ERR_OTHER;
    }
    PICODBG_DEBUG(("get \"%.100s\"", text));
    *bytesPut = 0;
    while ((*bytesPut < textSize) && (PICO_OK == picodata_cbPutCh(this->cbIn, text[*bytesPut]))) {
        (*bytesPut)++;
    }

    return PICO_OK;
}/*picoctrl_engFeedText*/

/**
 * gets engine output bytes
 * @param    this : handle of the engine
 * @param    buffer : the destination buffer
 * @param    bufferSize : max size of the destinatioon buffer
 * @param    *bytesReceived : the number of bytes effectively returned
 * @return    PICO_OK : feeding succeded
 * @return    PICO_ERR_OTHER : if error
 * @callgraph
 * @callergraph
 */
picodata_step_result_t picoctrl_engFetchOutputItemBytes(
        picoctrl_Engine this,
        picoos_char *buffer,
        picoos_int16 bufferSize,
        picoos_int16 *bytesReceived) {
    picoos_uint16 ui;
    picodata_step_result_t stepResult;
    pico_status_t rv;

    if (NULL == this) {
        return (picodata_step_result_t)PICO_STEP_ERROR;
    }
    PICODBG_DEBUG(("doing one step"));
    stepResult = this->control->step(this->control,/* mode */0,&ui);
    if (PICODATA_PU_ERROR != stepResult) {
        PICODBG_TRACE(("filling output buffer"));
        rv = picodata_cbGetSpeechData(this->cbOut, (picoos_uint8 *)buffer,
                                      bufferSize, &ui);

        if (ui > 255) {   /* because picoapi uses signed int16 */
            return (picodata_step_result_t)PICO_STEP_ERROR;
        } else {
            *bytesReceived = ui;
        }
        if ((rv == PICO_EXC_BUF_UNDERFLOW) || (rv == PICO_EXC_BUF_OVERFLOW)) {
            PICODBG_ERROR(("problem getting speech data"));
            return (picodata_step_result_t)PICO_STEP_ERROR;
        }
        /* rv must now be PICO_OK or PICO_EOF */
        PICODBG_ASSERT(((PICO_EOF == rv) || (PICO_OK == rv)));
        if ((PICODATA_PU_IDLE == stepResult) && (PICO_EOF == rv)) {
            PICODBG_DEBUG(("IDLE"));
            return (picodata_step_result_t)PICO_STEP_IDLE;
        } else if (PICODATA_PU_ERROR == stepResult) {
            PICODBG_DEBUG(("ERROR"));
            return (picodata_step_result_t)PICO_STEP_ERROR;
        } else {
            PICODBG_DEBUG(("BUSY"));
            return (picodata_step_result_t)PICO_STEP_BUSY;
        }
    } else {
        return (picodata_step_result_t)PICO_STEP_ERROR;
    }
}/*picoctrl_engFetchOutputItemBytes*/

/**
 * returns the last scheduled PU
 * @param    this : handle of the engine
 * @return    a value >= 0 : last scheduled PU index
 * @remarks    designed to be used for performance evaluation
 * @callgraph
 * @callergraph
 */
picodata_step_result_t picoctrl_getLastScheduledPU(
        picoctrl_Engine this
        )
{
    ctrl_subobj_t * ctrl;
    if (NULL == this || NULL == this->control->subObj) {
        return PICO_ERR_OTHER;
    }
    ctrl = (ctrl_subobj_t *) ((*this).control->subObj);
    return (picodata_step_result_t) ctrl->curPU;
}/*picoctrl_getLastScheduledPU*/

/**
 * returns the last item type produced by the last scheduled PU
 * @param    this : handle of the engine
 * @return    a value >= 0 : item type (see picodata.h for item types)
 * @return    a value = 0 : no item produced
 * @remarks    designed to be used for performance evaluation
 * @callgraph
 * @callergraph
 */
picodata_step_result_t picoctrl_getLastProducedItemType(
        picoctrl_Engine this
        )
{
    ctrl_subobj_t * ctrl;
    if (NULL == this || NULL == this->control->subObj) {
        return PICO_ERR_OTHER;
    }
    ctrl = (ctrl_subobj_t *) ((*this).control->subObj);
    return (picodata_step_result_t) ctrl->lastItemTypeProduced;
}/*picoctrl_getLastProducedItemType*/


#ifdef __cplusplus
}
#endif

/* Picoctrl.c end */
