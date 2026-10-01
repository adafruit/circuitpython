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
 * @file picowa.c
 *
 * word analysis PU - lexicon lookup and POS prediction
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
#include "picowa.h"
#include "picoklex.h"
#include "picokdt.h"
#include "picoktab.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* PU waStep states */
#define WA_STEPSTATE_COLLECT  0
#define WA_STEPSTATE_PROCESS  1
#define WA_STEPSTATE_FEED     2


/*  subobject    : WordAnaUnit
 *  shortcut     : wa
 *  context size : one item
 */
typedef struct wa_subobj {
    picoos_uint8 procState; /* for next processing step decision */

    /* one item only */
    picoos_uint8 inBuf[PICOWA_MAXITEMSIZE]; /* internal input buffer */
    picoos_uint16 inBufSize; /* actually allocated size */
    picoos_uint16 inLen; /* length of item in inBuf, 0 for empty buf */

    picoos_uint8 outBuf[PICOWA_MAXITEMSIZE]; /* internal output buffer */
    picoos_uint16 outBufSize; /* actually allocated size */
    picoos_uint16 outLen; /* length of item in outBuf, 0 for empty buf */

    /* lex knowledge base */
    picoklex_Lex lex;

    /* ulex knowledge bases */
    picoos_uint8 numUlex;
    picoklex_Lex ulex[PICOKNOW_MAX_NUM_ULEX];

    /* tab knowledge base */
    picoktab_Pos tabpos;

    /* dtposp knowledge base */
    picokdt_DtPosP dtposp;
} wa_subobj_t;


static pico_status_t waInitialize(register picodata_ProcessingUnit this, picoos_int32 resetMode) {
    picoos_uint8 i;
    picoklex_Lex ulex;
    wa_subobj_t * wa;

    picoknow_kb_id_t ulexKbIds[PICOKNOW_MAX_NUM_ULEX] = PICOKNOW_KBID_ULEX_ARRAY;

    PICODBG_DEBUG(("calling"));

    if (NULL == this || NULL == this->subObj) {
        return (picodata_step_result_t) picoos_emRaiseException(this->common->em,
                                       PICO_ERR_NULLPTR_ACCESS, NULL, NULL);
    }
    wa = (wa_subobj_t *) this->subObj;
    wa->procState = WA_STEPSTATE_COLLECT;
    wa->inBufSize = PICOWA_MAXITEMSIZE;
    wa->inLen = 0;
    wa->outBufSize = PICOWA_MAXITEMSIZE;
    wa->outLen = 0;

    if (resetMode == PICO_RESET_SOFT) {
        /*following initializations needed only at startup or after a full reset*/
        return PICO_OK;
    }
    /* kb lex */
    wa->lex = picoklex_getLex(this->voice->kbArray[PICOKNOW_KBID_LEX_MAIN]);
    if (wa->lex == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got lex"));

    /* kb ulex[] */
    wa->numUlex = 0;
    for (i = 0; i<PICOKNOW_MAX_NUM_ULEX; i++) {
        ulex = picoklex_getLex(this->voice->kbArray[ulexKbIds[i]]);
        if (NULL != ulex) {
            wa->ulex[wa->numUlex++] = ulex;
        }
    }
    PICODBG_DEBUG(("got %i user lexica", wa->numUlex));

    /* kb tabpos */
    wa->tabpos =
        picoktab_getPos(this->voice->kbArray[PICOKNOW_KBID_TAB_POS]);
    if (wa->tabpos == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got tabpos"));

    /* kb dtposp */
    wa->dtposp = picokdt_getDtPosP(this->voice->kbArray[PICOKNOW_KBID_DT_POSP]);
    if (wa->dtposp == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got dtposp"));
    return PICO_OK;
}

static picodata_step_result_t waStep(register picodata_ProcessingUnit this,
                                     picoos_int16 mode,
                                     picoos_uint16 *numBytesOutput);

static pico_status_t waTerminate(register picodata_ProcessingUnit this) {
    return PICO_OK;
}

static pico_status_t waSubObjDeallocate(register picodata_ProcessingUnit this,
                                        picoos_MemoryManager mm) {
    if (NULL != this) {
        picoos_deallocate(this->common->mm, (void *) &this->subObj);
    }
    mm = mm;        /* avoid warning "var not used in this function"*/
    return PICO_OK;
}


picodata_ProcessingUnit picowa_newWordAnaUnit(picoos_MemoryManager mm,
                                              picoos_Common common,
                                              picodata_CharBuffer cbIn,
                                              picodata_CharBuffer cbOut,
                                              picorsrc_Voice voice) {
    picodata_ProcessingUnit this;

    this = picodata_newProcessingUnit(mm, common, cbIn, cbOut, voice);
    if (this == NULL) {
        return NULL;
    }

    this->initialize = waInitialize;
    PICODBG_DEBUG(("set this->step to waStep"));
    this->step = waStep;
    this->terminate = waTerminate;
    this->subDeallocate = waSubObjDeallocate;
    this->subObj = picoos_allocate(mm, sizeof(wa_subobj_t));
    if (this->subObj == NULL) {
        picoos_deallocate(mm, (void *)&this);
        picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM, NULL, NULL);
        return NULL;
    }

    waInitialize(this, PICO_RESET_FULL);
    return this;
}

/* ***********************************************************************/
/*                       WORDGRAPH proc functions                        */
/* ***********************************************************************/

static picoos_uint8 waClassifyPos(register picodata_ProcessingUnit this,
                                  register wa_subobj_t *wa,
                                  const picoos_uint8 *graph,
                                  const picoos_uint16 graphlen) {
    picokdt_classify_result_t dtres;
    picoos_uint8 specchar;
    picoos_uint16 i;

    PICODBG_DEBUG(("graphlen %d", graphlen));

    /* check existence of special char (e.g. hyphen) in graph:
       for now, check existence of hard-coded ascii hyphen,
       ie. preproc needs to match all UTF8 hyphens to the ascii
       hyphen. */
    /*  @todo : consider specifying special char(s) in lingware. */
    specchar = FALSE;
    i = 0;
    while ((i < graphlen) && (!specchar)) {
        if (graph[i++] == '-') {
            specchar = TRUE;
        }
    }

    /* construct input vector, which is set in dtposp */
    if (!picokdt_dtPosPconstructInVec(wa->dtposp, graph, graphlen, specchar)) {
        /* error constructing invec */
        PICODBG_WARN(("problem with invec"));
        picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR, NULL, NULL);
        return PICODATA_ITEMINFO1_ERR;
    }

    /* classify */
    if (!picokdt_dtPosPclassify(wa->dtposp)) {
        /* error doing classification */
        PICODBG_WARN(("problem classifying"));
        picoos_emRaiseWarning(this->common->em, PICO_WARN_CLASSIFICATION,
                              NULL, NULL);
        return PICODATA_ITEMINFO1_ERR;
    }

    /* decompose */
    if (!picokdt_dtPosPdecomposeOutClass(wa->dtposp, &dtres)) {
        /* error decomposing */
        PICODBG_WARN(("problem decomposing"));
        picoos_emRaiseWarning(this->common->em, PICO_WARN_OUTVECTOR,
                              NULL, NULL);
        return PICODATA_ITEMINFO1_ERR;
    }

    if (dtres.set) {
        PICODBG_DEBUG(("class %d", dtres.class));
        return (picoos_uint8)dtres.class;
    } else {
        PICODBG_WARN(("result not set"));
        picoos_emRaiseWarning(this->common->em, PICO_WARN_CLASSIFICATION,
                              NULL, NULL);
        return PICODATA_ITEMINFO1_ERR;
    }
}


static pico_status_t waProcessWordgraph(register picodata_ProcessingUnit this,
                                        register wa_subobj_t *wa /*inout*/,
                                        picodata_itemhead_t *head /*inout*/,
                                        const picoos_uint8 *content) {
    pico_status_t status;
    picoklex_lexl_result_t lexres;
    picoos_uint8 posbuf[PICOKTAB_MAXNRPOS_IN_COMB];
    picoos_uint8 i;
    picoos_uint8 foundIndex;
    picoos_bool found;


    PICODBG_DEBUG(("type %c, len %d", head->type, head->len));

    /* do lookup
       if no entry found:
         do POS prediction:     -> WORDGRAPH(POSes,NA)graph
       else:
         if incl-phone:
           N entries possible  -> WORDINDEX(POSes,NA)POS1|ind1...POSN|indN
           (N in {1,...,PICOKLEX_MAX_NRRES}, now up to 4)
         else:
           no phone, one entry  -> WORDGRAPH(POS,NA)graph
    */

    found = FALSE;
    i = 0;
    while (!found && (i < wa->numUlex)) {
        found = picoklex_lexLookup(wa->ulex[i], content, head->len, &lexres);
        i++;
    }
    /* note that if found, i will be incremented nevertheless, so i >= 1 */
    if (found) {
        foundIndex = i;
    } else {
        foundIndex = 0;
    }
    if (!found && !picoklex_lexLookup(wa->lex, content, head->len, &lexres)) {
        /* no lex entry found, WORDGRAPH(POS,NA)graph */
        if (PICO_OK == picodata_copy_item(wa->inBuf, wa->inLen,
                                          wa->outBuf, wa->outBufSize,
                                          &wa->outLen)) {
            wa->inLen = 0;
            /* predict and modify pos in info1 */
            if (PICO_OK != picodata_set_iteminfo1(wa->outBuf, wa->outLen,
                                   waClassifyPos(this, wa, content, head->len))) {
                return picoos_emRaiseException(this->common->em,
                                               PICO_EXC_BUF_OVERFLOW,NULL,NULL);
            }
        }

    } else {    /* at least one entry found */
        PICODBG_DEBUG(("at least one entry found in lexicon %i",foundIndex));
        if (lexres.phonfound) {    /* incl. ind-phone and possibly multi-ent. */
            if (lexres.nrres > PICOKLEX_MAX_NRRES) {
                /* not possible with system lexicon, needs to be
                   ensured for user lex too */
                picoos_emRaiseWarning(this->common->em, PICO_WARN_FALLBACK,NULL,
                        (picoos_char *)"using %d lexicon lookup results",
                        PICOKLEX_MAX_NRRES);
                lexres.nrres = PICOKLEX_MAX_NRRES;
            }
            head->type = PICODATA_ITEM_WORDINDEX;
            if (lexres.nrres == 1) {
                head->info1 = lexres.posind[0];
            } else {
                /* more than one result, POSgroup info needs to be
                   determined for later POS disambiguation */
                for (i = 0; i < lexres.nrres; i++) {
                    posbuf[i] = lexres.posind[i * PICOKLEX_POSIND_SIZE];
                }
                head->info1 = picoktab_getPosGroup(wa->tabpos, posbuf,
                                                   lexres.nrres);
            }
            head->info2 = foundIndex;
            head->len = lexres.posindlen;
            if ((status = picodata_put_itemparts(head, lexres.posind,
                                                 lexres.posindlen,
                                                 wa->outBuf, wa->outBufSize,
                                                 &wa->outLen)) == PICO_OK) {
                wa->inLen = 0;
            } else {
                return picoos_emRaiseException(this->common->em, status,
                                               NULL, NULL);
            }

        } else {    /* no phone, :G2P, one entry: WORDGRAPH(POS,NA)graph */
            if (PICO_OK == picodata_copy_item(wa->inBuf, wa->inLen,
                                              wa->outBuf, wa->outBufSize,
                                              &wa->outLen)) {
                wa->inLen = 0;
                /* set lex pos in info1 */
                if (PICO_OK != picodata_set_iteminfo1(wa->outBuf, wa->outLen,
                                                      lexres.posind[0])) {
                    return picoos_emRaiseException(this->common->em,
                                                   PICO_EXC_BUF_OVERFLOW,
                                                   NULL, NULL);
                }
            }
        }
    }
    return PICO_OK;
}


/* ***********************************************************************/
/*                          waStep function                              */
/* ***********************************************************************/

/*
   collect into internal buffer, process, and then feed to output buffer

   init state: COLLECT      ext      ext
   state transitions:       in IN OUTout
   COLLECT | getOneItem  ->-1 +1  0  0   | (ATOMIC) -> PROCESS (got item)
   COLLECT | getOneItem  -> 0  0  0  0   | IDLE                (got no item)

   PROCESS | procOneItem -> 0 -1 +1  0   | (ATOMIC) -> FEED    (proc'ed item)
   PROCESS | procOneItem -> 0 -1  0  0   | BUSY     -> COLLECT (item skipped)

   FEED    | putOneItem  -> 0  0 -1 +1   | BUSY     -> COLLECT (put item)
   FEED    | putOneItem  -> 0  0  1  0   | OUT_FULL            (put no item)
*/

static picodata_step_result_t waStep(register picodata_ProcessingUnit this,
                                     picoos_int16 mode,
                                     picoos_uint16 * numBytesOutput) {
    register wa_subobj_t *wa;
    pico_status_t rv = PICO_OK;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    wa = (wa_subobj_t *) this->subObj;
    mode = mode;        /* avoid warning "var not used in this function"*/
    *numBytesOutput = 0;
    while (1) { /* exit via return */
        PICODBG_DEBUG(("doing state %i, inLen: %d, outLen: %d",
                       wa->procState, wa->inLen, wa->outLen));

        switch (wa->procState) {
            /* collect state: get item from charBuf and store in
             * internal inBuf
             */
            case WA_STEPSTATE_COLLECT:
                if (wa->inLen == 0) { /* is input buffer empty? */
                    picoos_uint16 blen;
                    /* try to get one item */
                    rv = picodata_cbGetItem(this->cbIn, wa->inBuf,
                                            wa->inBufSize, &blen);
                    PICODBG_DEBUG(("after getting item, status: %d", rv));
                    if (PICO_OK == rv) {
                        /* we now have one item */
                        wa->inLen = blen;
                        wa->procState = WA_STEPSTATE_PROCESS;
                        /* uncomment next line to split into two steps */
                        /* return PICODATA_PU_ATOMIC; */
                    } else if (PICO_EOF == rv) {
                        /* there was no item in the char buffer */
                        return PICODATA_PU_IDLE;
                    } else if ((PICO_EXC_BUF_UNDERFLOW == rv)
                               || (PICO_EXC_BUF_OVERFLOW == rv)) {
                        PICODBG_ERROR(("problem getting item"));
                        picoos_emRaiseException(this->common->em, rv,
                                                NULL, NULL);
                        return PICODATA_PU_ERROR;
                    } else {
                        PICODBG_ERROR(("problem getting item, unhandled"));
                        picoos_emRaiseException(this->common->em, rv,
                                                NULL, NULL);
                        return PICODATA_PU_ERROR;
                    }
                } else { /* there already is an item in the input buffer */
                    PICODBG_WARN(("item already in input buffer"));
                    picoos_emRaiseWarning(this->common->em,
                                          PICO_WARN_PU_IRREG_ITEM, NULL, NULL);
                    wa->procState = WA_STEPSTATE_PROCESS;
                    /* uncomment next to split into two steps */
                    /* return PICODATA_PU_ATOMIC; */
                }
                break;


            /* process state: process item in internal inBuf and put
             * result in internal outBuf
             */
            case WA_STEPSTATE_PROCESS:

                /* ensure there is an item in inBuf and it is valid */
                if ((wa->inLen > 0) && picodata_is_valid_item(wa->inBuf,
                                                              wa->inLen)) {
                    picodata_itemhead_t ihead;
                    picoos_uint8 *icontent;
                    pico_status_t rvP = PICO_OK;

                    rv = picodata_get_iteminfo(wa->inBuf, wa->inLen, &ihead,
                                               &icontent);
                    if (PICO_OK == rv) {

                        switch (ihead.type) {
                            case PICODATA_ITEM_WORDGRAPH:

                                if (0 < ihead.len) {
                                    rvP = waProcessWordgraph(this, wa, &ihead,
                                                             icontent);
                                } else {
                                    /* else ignore empty WORDGRAPH */
                                    wa->inLen = 0;
                                    wa->procState = WA_STEPSTATE_COLLECT;
                                    return PICODATA_PU_BUSY;
                                }
                                break;
                            case PICODATA_ITEM_OTHER:
                                /* skip item */
                                rvP = PICO_WARN_PU_DISCARD_BUF;
                                break;
                            default:
                                /* copy item unmodified */
                                rvP = picodata_copy_item(wa->inBuf,
                                                         wa->inLen, wa->outBuf,
                                                         wa->outBufSize, &wa->outLen);
                                break;
                        }

                        if (PICO_OK == rvP) {
                            wa->inLen = 0;
                            wa->procState = WA_STEPSTATE_FEED;
                            /* uncomment next to split into two steps */
                            /* return PICODATA_PU_ATOMIC; */
                        } else if (PICO_WARN_PU_DISCARD_BUF == rvP) {
                            /* discard input buffer and get a new item */
                            PICODBG_INFO(("skipping OTHER item"));
/*                            picoos_emRaiseWarning(this->common->em,
                                                  PICO_WARN_PU_DISCARD_BUF, NULL, NULL);
*/
                            wa->inLen = 0;
                            wa->procState = WA_STEPSTATE_COLLECT;
                            return PICODATA_PU_BUSY;
                        } else {
                            /* PICO_EXC_BUF_OVERFLOW   <- overflow in outbuf
                               PICO_ERR_OTHER          <- no valid item in inbuf
                               or return from processWordgraph
                            */
                            PICODBG_ERROR(("problem processing item", rvP));
                            picoos_emRaiseException(this->common->em, rvP,
                                                    NULL, NULL);
                            return PICODATA_PU_ERROR;
                        }

                    } else {    /* could not get iteminfo */
                        /* PICO_EXC_BUF_OVERFLOW   <- overflow in outbuf
                           PICO_ERR_OTHER          <- no valid item in inbuf
                        */
                        PICODBG_ERROR(("problem getting item info, "
                                       "discard buffer content"));
                        wa->inLen = 0;
                        wa->procState = WA_STEPSTATE_COLLECT;
                        picoos_emRaiseException(this->common->em, rv,
                                                NULL, NULL);
                        return PICODATA_PU_ERROR;
                    }

                } else if (wa->inLen == 0) {    /* no item in inBuf */
                    PICODBG_INFO(("no item in inBuf"));
                    /* wa->inLen = 0;*/
                    wa->procState = WA_STEPSTATE_COLLECT;
                    return PICODATA_PU_BUSY;

                } else {    /* no valid item in inBuf */
                    /* bad state/item, discard buffer content */
                    PICODBG_WARN(("no valid item, discard buffer content"));
                    picoos_emRaiseWarning(this->common->em,
                                          PICO_WARN_PU_IRREG_ITEM, NULL, NULL);
                    picoos_emRaiseWarning(this->common->em,
                                          PICO_WARN_PU_DISCARD_BUF, NULL, NULL);
                    wa->inLen = 0;
                    wa->procState = WA_STEPSTATE_COLLECT;
                    return PICODATA_PU_BUSY;
                }
                break;


            /* feed state: copy item in internal outBuf to output charBuf */
            case WA_STEPSTATE_FEED:

                /* check that item fits in cb should not be needed */
                rv = picodata_cbPutItem(this->cbOut, wa->outBuf,
                                        wa->outLen, numBytesOutput);

                PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                                   (picoos_uint8 *)"wana: ", wa->outBuf,
                                   wa->outLen);

                PICODBG_DEBUG(("put item, status: %d", rv));
                if (PICO_OK == rv) {
                    wa->outLen = 0;
                    wa->procState = WA_STEPSTATE_COLLECT;
                    return PICODATA_PU_BUSY;
                } else if (PICO_EXC_BUF_OVERFLOW == rv) {
                    PICODBG_INFO(("feeding, overflow, PICODATA_PU_OUT_FULL"));
                    return PICODATA_PU_OUT_FULL;
                } else if ((PICO_EXC_BUF_UNDERFLOW == rv)
                           || (PICO_ERR_OTHER == rv)) {
                    PICODBG_WARN(("feeding problem, discarding item"));
                    wa->outLen = 0;
                    wa->procState = WA_STEPSTATE_COLLECT;
                    picoos_emRaiseWarning(this->common->em, rv, NULL,NULL);
                    return PICODATA_PU_BUSY;
                }
                break;

            default:
                break;

        } /* switch */

    } /* while */

    /* should be never reached */
    PICODBG_ERROR(("reached end of function"));
    picoos_emRaiseException(this->common->em, PICO_ERR_OTHER, NULL, NULL);
    return PICODATA_PU_ERROR;
}

#ifdef __cplusplus
}
#endif


/* end */
