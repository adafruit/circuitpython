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
 * @file picoacph.c
 *
 * accentuation and phrasing
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
#include "picobase.h"
#include "picodata.h"
#include "picoacph.h"
#include "picokdt.h"
#include "picoklex.h"
#include "picoktab.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* PU acphStep states */
#define SA_STEPSTATE_COLLECT       0
#define SA_STEPSTATE_PROCESS_PHR  12
#define SA_STEPSTATE_PROCESS_ACC  13
#define SA_STEPSTATE_FEED          2


/* boundary strength state */
#define SA_BOUNDSTRENGTH_SSEP      0 /* sentence separator */
#define SA_BOUNDSTRENGTH_PPHR      1 /* primary phrase separator */


/*  subobject    : AccPhrUnit
 *  shortcut     : acph
 *  context size : one phrase, max. 30 non-PUNC items, for non-processed items
 *                 one item if internal input empty
 */

/**
 * @addtogroup picoacph
 *
 * <b> Pico Accentuation and Phrasing </b>\n
 *
  internal buffers:

  - headx : array for extended item heads of fixed size (head plus
    index for content, plus two fields for boundary strength/type)
  - cbuf : buffer for item contents (referenced by index in
    headx).

  0. bottom up filling of items in headx and cbuf

  1. phrasing (right-to-left):

     e.g. from      WP WP WP       WP WP PUNC  WP WP PUNC        WP WP WP PUNC  FLUSH    \n
     e.g. to  BSBEG WP WP WP BPHR3 WP WP BPHR1 WP WP BSEND BSBEG WP WP WP BSEND BTERM    \n
              |1                         |2                |3                   |4        \n

     2-level bound state: The internal buffer contains one primary phrase (sometimes forced, if buffer
     allmost full), with the trailing PUNCT item included (last item).\n
     If the trailing PUNC is a a primary phrase separator, the
       item is not output, but instead, the bound state is set to PPHR, so that the correct BOUND can
       be output at the start of the next primary phrase.\n
     Otherwise,
       the item is converted to the corresponding BOUND and output. the bound state is set to SSEP,
       so that a BOUND of type SBEG is output at the start of the next primary phrase.

     trailing PUNC item       bound states                                    \n
                              SSEP           PPHR                            \n
       PUNC(SENTEND, X)       B(B,X)>SSEP    B(P1,X)>SSEP  (X = T | Q | E)    \n
       PUNC(FLUSH, T)         B(B,T)>SSEP*    B(P1,T)>SSEP                    \n
       PUNC(PHRASEEND, P)     B(B,P)>PPHR    B(P1,P)>PPHR                    \n
       PUNC(PHRASEEND, FORC)  B(B,P)>PPHR    B(P1,P)>PPHR                    \n

    If more than one sentence separators follow each other (e.g. SEND-FLUSH, SEND-SEND) then
     all but the first will be treated as an (empty) phrase containing just this item.
     If this (single) item is a flush, creation of SBEG is suppressed.


  - dtphr phrasing tree ("subphrasing")
    determines
      - BOUND_PHR2
      - BOUND_PHR3
  - boundary strenghts are determined for every word (except the
    first one) from right-to-left. The boundary types mark the phrase
    type of the phrase following the boundary.
  - number of items actually changed (new BOUND items added): because
    of fixed size without content, two fields are contained in headx
    to indicate if a BOUND needs to be added to the LEFT of the item.
    -> headx further extended with boundary strength and type info to
    indicate that to the left of the headx ele a BOUND needs to be
    inserted when outputting.

  2. accentuation:
  - number of items unchanged, content unchanged, only head info changes
  -> changed in place in headx
*/


typedef struct {
    picodata_itemhead_t head;
    picoos_uint16 cind;
    picoos_uint8 boundstrength;  /* bstrength to the left, 0 if not set */
    picoos_uint8 boundtype;      /* btype for following phrase, 0 if not set */
} picoacph_headx_t;


typedef struct acph_subobj {
    picoos_uint8 procState; /* for next processing step decision */
    picoos_uint8 boundStrengthState;    /* boundary strength state */

    picoos_uint8 inspaceok;      /* flag: headx/cbuf has space for an item */
    picoos_uint8 needsmoreitems; /* flag: need more items */

    picoos_uint8 tmpbuf[PICODATA_MAX_ITEMSIZE];  /* tmp. location for an item */

    picoacph_headx_t headx[PICOACPH_MAXNR_HEADX];
    picoos_uint16 headxBottom; /* bottom */
    picoos_uint16 headxLen;    /* length, 0 if empty */

    picoos_uint8 cbuf[PICOACPH_MAXSIZE_CBUF];
    picoos_uint16 cbufBufSize; /* actually allocated size */
    picoos_uint16 cbufLen;     /* length, 0 if empty */

    /* tab knowledge base */
    picoktab_Phones tabphones;

    /* dtphr knowledge base */
    picokdt_DtPHR dtphr;

    /* dtacc knowledge base */
    picokdt_DtACC dtacc;
} acph_subobj_t;


static pico_status_t acphInitialize(register picodata_ProcessingUnit this, picoos_int32 resetMode) {
    acph_subobj_t * acph;
    picoos_uint16 i;

    PICODBG_DEBUG(("calling"));

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(this->common->em,
                                       PICO_ERR_NULLPTR_ACCESS, NULL, NULL);
    }
    acph = (acph_subobj_t *) this->subObj;
    acph->procState = SA_STEPSTATE_COLLECT;
    acph->boundStrengthState = SA_BOUNDSTRENGTH_SSEP;

    acph->inspaceok = TRUE;
    acph->needsmoreitems = TRUE;

    acph->headxBottom = 0;
    acph->headxLen = 0;
    acph->cbufBufSize = PICOACPH_MAXSIZE_CBUF;
    acph->cbufLen = 0;

    /* init headx, cbuf */
    for (i = 0; i < PICOACPH_MAXNR_HEADX; i++){
        acph->headx[i].head.type = 0;
        acph->headx[i].head.info1 = 0;
        acph->headx[i].head.info2 = 0;
        acph->headx[i].head.len = 0;
        acph->headx[i].cind = 0;
        acph->headx[i].boundstrength = 0;
        acph->headx[i].boundtype = 0;
    }
    for (i = 0; i < PICOACPH_MAXSIZE_CBUF; i++) {
        acph->cbuf[i] = 0;
    }

    if (resetMode == PICO_RESET_SOFT) {
        /*following initializations needed only at startup or after a full reset*/
        return PICO_OK;
    }

    /* kb tabphones */
    acph->tabphones =
        picoktab_getPhones(this->voice->kbArray[PICOKNOW_KBID_TAB_PHONES]);
    if (acph->tabphones == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got tabphones"));

#ifdef PICO_DEBUG_1
    {
        picoos_uint16 itmp;
        for (itmp = 0; itmp < 256; itmp++) {
            if (picoktab_hasVowelProp(acph->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasVowel: %d", itmp));
            }
            if (picoktab_hasDiphthProp(acph->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasDiphth: %d", itmp));
            }
            if (picoktab_hasGlottProp(acph->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasGlott: %d", itmp));
            }
            if (picoktab_hasNonsyllvowelProp(acph->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasNonsyllvowel: %d", itmp));
            }
            if (picoktab_hasSyllconsProp(acph->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasSyllcons: %d", itmp));
            }

            if (picoktab_isPrimstress(acph->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones isPrimstress: %d", itmp));
            }
            if (picoktab_isSecstress(acph->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones isSecstress: %d", itmp));
            }
            if (picoktab_isSyllbound(acph->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones isSyllbound: %d", itmp));
            }
            if (picoktab_isPause(acph->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones isPause: %d", itmp));
            }
        }

        PICODBG_DEBUG(("tabphones primstressID: %d",
                       picoktab_getPrimstressID(acph->tabphones)));
        PICODBG_DEBUG(("tabphones secstressID: %d",
                       picoktab_getSecstressID(acph->tabphones)));
        PICODBG_DEBUG(("tabphones syllboundID: %d",
                       picoktab_getSyllboundID(acph->tabphones)));
        PICODBG_DEBUG(("tabphones pauseID: %d",
                       picoktab_getPauseID(acph->tabphones)));
    }
#endif


    /* kb dtphr */
    acph->dtphr = picokdt_getDtPHR(this->voice->kbArray[PICOKNOW_KBID_DT_PHR]);
    if (acph->dtphr == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got dtphr"));

    /* kb dtacc */
    acph->dtacc = picokdt_getDtACC(this->voice->kbArray[PICOKNOW_KBID_DT_ACC]);
    if (acph->dtacc == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got dtacc"));

    return PICO_OK;
}

static picodata_step_result_t acphStep(register picodata_ProcessingUnit this,
                                     picoos_int16 mode,
                                     picoos_uint16 *numBytesOutput);

static pico_status_t acphTerminate(register picodata_ProcessingUnit this)
{
    return PICO_OK;
}

static pico_status_t acphSubObjDeallocate(register picodata_ProcessingUnit this,
                                        picoos_MemoryManager mm) {
    mm = mm;        /* avoid warning "var not used in this function"*/
    if (NULL != this) {
        picoos_deallocate(this->common->mm, (void *) &this->subObj);
    }
    return PICO_OK;
}


picodata_ProcessingUnit picoacph_newAccPhrUnit(picoos_MemoryManager mm,
                                              picoos_Common common,
                                              picodata_CharBuffer cbIn,
                                              picodata_CharBuffer cbOut,
                                              picorsrc_Voice voice) {
    picodata_ProcessingUnit this;

    this = picodata_newProcessingUnit(mm, common, cbIn, cbOut, voice);
    if (this == NULL) {
        return NULL;
    }

    this->initialize = acphInitialize;
    PICODBG_DEBUG(("set this->step to acphStep"));
    this->step = acphStep;
    this->terminate = acphTerminate;
    this->subDeallocate = acphSubObjDeallocate;
    this->subObj = picoos_allocate(mm, sizeof(acph_subobj_t));
    if (this->subObj == NULL) {
        picoos_deallocate(mm, (void *)&this);
        picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM, NULL, NULL);
        return NULL;
    }

    acphInitialize(this, PICO_RESET_FULL);
    return this;
}


/* ***********************************************************************/
/* PROCESS_PHR/ACC support functions */
/* ***********************************************************************/


static picoos_uint8 acphGetNrSylls(register picodata_ProcessingUnit this,
                                 register acph_subobj_t *acph,
                                 const picoos_uint16 ind) {
    picoos_uint8 i;
    picoos_uint8 ch;
    picoos_uint8 count;

    count = 1;
    for (i = 0; i < acph->headx[ind].head.len; i++) {
        ch = acph->cbuf[acph->headx[ind].cind + i];
        if (picoktab_isSyllbound(acph->tabphones, ch)) {
            count++;
        }
    }
    return count;
}


/* ***********************************************************************/
/* PROCESS_PHR functions */
/* ***********************************************************************/


/* find next POS to the left of 'ind' and return its POS and index */
static picoos_uint8 acphPhrItemSeqGetPosLeft(register picodata_ProcessingUnit this,
                                           register acph_subobj_t *acph,
                                           const picoos_uint16 ind,
                                           picoos_uint16 *leftind) {
    picoos_uint8 val;
    picoos_int32 i;

    val = PICOKDT_EPSILON;
    for (i = ind - 1; ((val == PICOKDT_EPSILON) && (i >= 0)); i--) {
        if ((acph->headx[i].head.type == PICODATA_ITEM_WORDPHON)) {
            val = acph->headx[i].head.info1;
        }
    }
    *leftind = i + 1;
    return val;
}


/* right-to-left, for each WORDPHON do phr */
static pico_status_t acphSubPhrasing(register picodata_ProcessingUnit this,
                                   register acph_subobj_t *acph) {
    picokdt_classify_result_t dtres;
    picoos_uint8 valbuf[5];
    picoos_uint16 nrwordspre;
    picoos_uint16 nrwordsfol;
    picoos_uint16 nrsyllsfol;
    picoos_uint16 lastprev2; /* last index of POS(es) found to the left */
    picoos_uint8 curpos;     /* POS(es) of current word */
    picoos_uint16 upbound;   /* index of last WORDPHON item (with POS) */
    picoos_uint8 okay;
    picoos_uint8 nosubphrases;
    picoos_int32 i;

    /* set initial values */
    okay = TRUE;
    nosubphrases = TRUE;
    curpos = PICOKDT_EPSILON;   /* needs to be in 2^8 */

    /* set upbound to last WORDPHON, don't worry about first one */
    upbound = acph->headxLen - 1;
    while ((upbound > 0) &&
           (acph->headx[upbound].head.type != PICODATA_ITEM_WORDPHON)) {
        upbound--;
    }

    /* zero or one WORDPHON, no subphrasing needed, but handling of
       BOUND strength state is needed */
    if (upbound <= 0) {
        /* phrase not containing more than one WORDPHON */
        PICODBG_DEBUG(("less than two WORDPHON in phrase -> no subphrasing"));
    }

    lastprev2 = upbound;

    /* set initial nr pre/fol words/sylls, upbound is ind of last WORDPHON */
    nrwordsfol = 0;
    nrsyllsfol = 0;
    nrwordspre = 0;
    for (i = 0; i < upbound; i++) {
        if (acph->headx[i].head.type == PICODATA_ITEM_WORDPHON) {
            nrwordspre++;
        }
    }

    nrwordspre++;    /* because we later have a decrement before being used */


    /* set POS of current word in valbuf[1], will be shifted right afterwards */
    valbuf[1] = acph->headx[upbound].head.info1;
    /* find first POS to the left and set valbuf[0] */
    valbuf[0] = acphPhrItemSeqGetPosLeft(this, acph, lastprev2, &lastprev2);
    for (i = 2; i < 5; i++) {
        valbuf[i] = PICOKDT_EPSILON;
    }

    PICODBG_TRACE(("headxLen: %d", acph->headxLen));

    /* at least two WORDPHON items */
    /* process from right-to-left all items in headx, except for 1st WORDPHON */
    for (i = upbound; (i > 0) && (nrwordspre > 1); i--) {
        okay = TRUE;

        PICODBG_TRACE(("iter: %d, type: %c", i, acph->headx[i].head.type));

        /* if not (WORDPHON) */
        if ((acph->headx[i].head.type != PICODATA_ITEM_WORDPHON)) {
            continue;
        }

        PICODBG_TRACE(("iter: %d, curpos: %d", i, acph->headx[i].head.info1));

        /* get and set POS of current item, must be WORDPHON */
        curpos = acph->headx[i].head.info1;

        /* no continue so far => at [i] we have a WORDPHON item */
        /* shift all POS elements one position to the right */
        valbuf[4] = valbuf[3];
        valbuf[3] = valbuf[2];
        valbuf[2] = valbuf[1];
        valbuf[1] = valbuf[0];
        /* find next POS to the left and set valbuf[0] */
        valbuf[0] = acphPhrItemSeqGetPosLeft(this, acph, lastprev2, &lastprev2);

        /* better check double than never */
        if (curpos != valbuf[2]) {
            PICODBG_WARN(("syncing POS"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR,
                                  NULL, NULL);
            valbuf[2] = curpos;
        }

        nrwordsfol++;
        nrsyllsfol += acphGetNrSylls(this, acph, i);
        nrwordspre--;

        PICODBG_TRACE(("%d: [%d,%d|%d|%d,%d|%d,%d,%d]",
                       i, valbuf[0], valbuf[1], valbuf[2], valbuf[3],
                       valbuf[4], nrwordspre, nrwordsfol, nrsyllsfol));

        /* no continue so far => subphrasing needed */
        /* construct input vector, which is set in dtphr */
        if (!picokdt_dtPHRconstructInVec(acph->dtphr, valbuf[0], valbuf[1],
                                         valbuf[2], valbuf[3], valbuf[4],
                                         nrwordspre, nrwordsfol, nrsyllsfol)) {
            /* error constructing invec */
            PICODBG_WARN(("problem with invec"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR,
                                  NULL, NULL);
            okay = FALSE;
        }
        /* classify */
        if (okay && (!picokdt_dtPHRclassify(acph->dtphr))) {
            /* error doing classification */
            PICODBG_WARN(("problem classifying"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_CLASSIFICATION,
                                  NULL, NULL);
            okay = FALSE;
        }
        /* decompose */
        if (okay && (!picokdt_dtPHRdecomposeOutClass(acph->dtphr, &dtres))) {
            /* error decomposing */
            PICODBG_WARN(("problem decomposing"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_OUTVECTOR,
                                  NULL, NULL);
            okay = FALSE;
        }

        if (okay && dtres.set) {
            PICODBG_DEBUG(("%d - inpos: %d, out: %d", i,valbuf[2],dtres.class));
        } else {
            PICODBG_WARN(("problem determining subphrase boundary strength"));
            dtres.class = PICODATA_ITEMINFO1_ERR;
        }

        if (dtres.class > 255) {
            PICODBG_WARN(("dt class outside valid range, setting to PHR0"));
            dtres.class = PICODATA_ITEMINFO1_BOUND_PHR0;
        }
        acph->headx[i].boundstrength = (picoos_uint8)dtres.class;
        if ((dtres.class == PICODATA_ITEMINFO1_BOUND_PHR2) ||
            (dtres.class == PICODATA_ITEMINFO1_BOUND_PHR3)) {
            if (nosubphrases) {
                /* it's the last secondary phrase in the primary phrase */
                /* add type info */
                switch (acph->headx[acph->headxLen - 1].head.info2) {
                    case PICODATA_ITEMINFO2_PUNC_SENT_T:
                        acph->headx[i].boundtype =
                            PICODATA_ITEMINFO2_BOUNDTYPE_T;
                        break;
                    case PICODATA_ITEMINFO2_PUNC_SENT_Q:
                        acph->headx[i].boundtype =
                            PICODATA_ITEMINFO2_BOUNDTYPE_Q;
                        break;
                    case PICODATA_ITEMINFO2_PUNC_SENT_E:
                        acph->headx[i].boundtype =
                            PICODATA_ITEMINFO2_BOUNDTYPE_E;
                        break;
                    case PICODATA_ITEMINFO2_PUNC_PHRASE:
                    case PICODATA_ITEMINFO2_PUNC_PHRASE_FORCED:
                        acph->headx[i].boundtype =
                            PICODATA_ITEMINFO2_BOUNDTYPE_P;
                        break;
                    default:
                        PICODBG_WARN(("invalid boundary type, not set"));
                        break;
                }
                nosubphrases = FALSE;

            } else {
                acph->headx[i].boundtype =
                    PICODATA_ITEMINFO2_BOUNDTYPE_P;
            }
            /* reset nr following words and sylls counters */
            nrwordsfol = 0;
            nrsyllsfol = 0;
        }
    }

    /* process first item, add bound-info */
    switch (acph->boundStrengthState) {
        case SA_BOUNDSTRENGTH_SSEP:
            acph->headx[0].boundstrength =
                PICODATA_ITEMINFO1_BOUND_SBEG;
            break;
        case SA_BOUNDSTRENGTH_PPHR:
            acph->headx[0].boundstrength =
                PICODATA_ITEMINFO1_BOUND_PHR1;
            break;
        default:
            PICODBG_WARN(("invalid boundary strength, not set"));
            break;
    }

    /* set boundary strength state */
    switch (acph->headx[acph->headxLen - 1].head.info1) {
        case PICODATA_ITEMINFO1_PUNC_SENTEND:
        case PICODATA_ITEMINFO1_PUNC_FLUSH:
            acph->boundStrengthState = SA_BOUNDSTRENGTH_SSEP;
            break;
        case PICODATA_ITEMINFO1_PUNC_PHRASEEND:
            acph->boundStrengthState = SA_BOUNDSTRENGTH_PPHR;
            break;
        default:
            PICODBG_WARN(("invalid boundary strength state, not changed"));
            break;
    }

    if (nosubphrases) {
        /* process first item, add type info */
        switch (acph->headx[acph->headxLen - 1].head.info2) {
            case PICODATA_ITEMINFO2_PUNC_SENT_T:
                acph->headx[0].boundtype =
                    PICODATA_ITEMINFO2_BOUNDTYPE_T;
                break;
            case PICODATA_ITEMINFO2_PUNC_SENT_Q:
                acph->headx[0].boundtype =
                    PICODATA_ITEMINFO2_BOUNDTYPE_Q;
                break;
            case PICODATA_ITEMINFO2_PUNC_SENT_E:
                acph->headx[0].boundtype =
                    PICODATA_ITEMINFO2_BOUNDTYPE_E;
                break;
            case PICODATA_ITEMINFO2_PUNC_PHRASE:
            case PICODATA_ITEMINFO2_PUNC_PHRASE_FORCED:
                acph->headx[0].boundtype =
                    PICODATA_ITEMINFO2_BOUNDTYPE_P;
                break;
            default:
                PICODBG_WARN(("invalid boundary type, not set"));
                break;
        }
    } else {
        acph->headx[0].boundtype =
            PICODATA_ITEMINFO2_BOUNDTYPE_P;
    }

    return PICO_OK;
}


/* ***********************************************************************/
/* PROCESS_ACC functions */
/* ***********************************************************************/

/* find next POS to the left of 'ind' and return its POS and index */
static picoos_uint8 acphAccItemSeqGetPosLeft(register picodata_ProcessingUnit this,
                                           register acph_subobj_t *acph,
                                           const picoos_uint16 ind,
                                           picoos_uint16 *leftind) {
    picoos_uint8 val;
    picoos_int32 i;

    val = PICOKDT_EPSILON;
    for (i = ind - 1; ((val == PICOKDT_EPSILON) && (i >= 0)); i--) {
        if ((acph->headx[i].head.type == PICODATA_ITEM_WORDPHON)) {
            val = acph->headx[i].head.info1;
        }
    }
    *leftind = i + 1;
    return val;
}


/* s1: nr sylls in word before the first primary stressed syll,
   s2: nr sylls in word after (but excluding) the first primary stressed syll */
static picoos_uint8 acphAccNrSyllParts(register picodata_ProcessingUnit this,
                                     register acph_subobj_t *acph,
                                     const picoos_uint16 ind,
                                     picoos_uint8 *s1,
                                     picoos_uint8 *s2) {
    picoos_uint16 pind;
    picoos_uint16 pend;    /* phone string start+len */
    picoos_uint8 afterprim;

    /* check ind is in valid range */
    if (ind >= acph->headxLen) {
        return FALSE;
    }

    *s1 = 0;
    *s2 = 0;
    afterprim = FALSE;
    pend = acph->headx[ind].cind + acph->headx[ind].head.len;
    for (pind = acph->headx[ind].cind; pind < pend; pind++) {
        if (picoktab_isPrimstress(acph->tabphones, acph->cbuf[pind])) {
            afterprim = TRUE;
        } else if (picoktab_isSyllbound(acph->tabphones, acph->cbuf[pind])) {
            if (afterprim) {
                (*s2)++;
            } else {
                (*s1)++;
            }
        }
    }
    if (afterprim) {
        (*s2)++;
    } else {
        (*s1)++;
    }

    /* exclude the stressed syllable */
    if ((*s2) > 0) {
        (*s2)--;
    }
    /* handle the case when there is no primstress */
    if (!afterprim) {
        (*s2) = (*s1);
    }
    return TRUE;
}


static picoos_uint8 acphAccGetNrsRight(register picodata_ProcessingUnit this,
                                     register acph_subobj_t *acph,
                                     const picoos_uint16 ind,
                                     picoos_uint16 *nrwordsfol,
                                     picoos_uint16 *nrsyllsfol,
                                     picoos_uint16 *footwordsfol,
                                     picoos_uint16 *footsyllsfol) {
    picoos_uint16 i;
    picoos_uint8 s1;
    picoos_uint8 s2;

    if (!acphAccNrSyllParts(this, acph, ind, &s1, &s2)) {
        return FALSE;
    }

    *nrwordsfol = 0;
    *nrsyllsfol = s2;
    i = ind + 1;
    while ((i < acph->headxLen) &&
           (acph->headx[i].boundstrength == PICODATA_ITEMINFO1_BOUND_PHR0)) {
        if (acph->headx[i].head.type == PICODATA_ITEM_WORDPHON) {
            (*nrwordsfol)++;
            *nrsyllsfol += acphGetNrSylls(this, acph, i);
        }
        i++;
    }

    *footwordsfol = 0;
    *footsyllsfol = s2;
    i = ind + 1;
    while ((i < acph->headxLen) &&
           (acph->headx[i].head.info2 != PICODATA_ACC1)) {
        if (acph->headx[i].head.type == PICODATA_ITEM_WORDPHON) {
            (*footwordsfol)++;
            *footsyllsfol += acphGetNrSylls(this, acph, i);
        }
        i++;
    }
    if ((i < acph->headxLen) && (acph->headx[i].head.info2 == PICODATA_ACC1)) {
        if (!acphAccNrSyllParts(this, acph, i, &s1, &s2)) {
            return FALSE;
        }
        *footsyllsfol += s1;
    }
    return TRUE;
}


static picoos_uint8 acphAccGetNrsLeft(register picodata_ProcessingUnit this,
                                    register acph_subobj_t *acph,
                                    const picoos_uint16 ind,
                                    picoos_uint16 *nrwordspre,
                                    picoos_uint16 *nrsyllspre) {
    picoos_int32 i;
    picoos_uint8 s1;
    picoos_uint8 s2;

    if (!acphAccNrSyllParts(this, acph, ind, &s1, &s2)) {
        return FALSE;
    }

    *nrwordspre = 0;
    *nrsyllspre = s1;
    i = ind - 1;
    while ((i >= 0) &&
           (acph->headx[i].boundstrength == PICODATA_ITEMINFO1_BOUND_PHR0)) {
        if (acph->headx[i].head.type == PICODATA_ITEM_WORDPHON) {
            (*nrwordspre)++;
            *nrsyllspre += acphGetNrSylls(this, acph, i);
        }
        i--;
    }

    if ((acph->headx[i].boundstrength != PICODATA_ITEMINFO1_BOUND_PHR0) &&
        (acph->headx[i].head.type == PICODATA_ITEM_WORDPHON)) {
        (*nrwordspre)++;
        *nrsyllspre += acphGetNrSylls(this, acph, i);
    }
    return TRUE;
}


/* return TRUE if wordphon contains no stress, FALSE otherwise */
static picoos_uint8 acphIsWordWithoutStress(register picodata_ProcessingUnit this,
                                          register acph_subobj_t *acph,
                                          const picoos_uint16 ind) {
    picoos_uint8 i;
    picoos_uint16 pos;

    pos = acph->headx[ind].cind;
    for (i = 0; i < acph->headx[ind].head.len; i++) {
        if (picoktab_isPrimstress(acph->tabphones, acph->cbuf[pos + i]) ||
            picoktab_isSecstress(acph->tabphones, acph->cbuf[pos + i])) {
            return FALSE;
        }
    }
    return TRUE;
}


/* right-to-left, for each WORDPHON do acc */
static pico_status_t acphAccentuation(register picodata_ProcessingUnit this,
                                    register acph_subobj_t *acph) {
    picokdt_classify_result_t dtres;
    picoos_uint8 valbuf[5];
    picoos_uint16 hist1;
    picoos_uint16 hist2;
    picoos_uint16 nrwordspre;
    picoos_uint16 nrsyllspre;
    picoos_uint16 nrwordsfol;
    picoos_uint16 nrsyllsfol;
    picoos_uint16 footwordsfol;
    picoos_uint16 footsyllsfol;
    picoos_uint16 lastprev2; /* last index of POS(es) found to the left */
    picoos_uint8 curpos;     /* POS(es) of current word */
    picoos_uint16 prevout;
    picoos_uint8 okay;
    picoos_int32 upbound;   /* index of last WORDPHON item (with POS) */
    picoos_uint16 i;

    /* set initial values */
    okay = TRUE;
    curpos = PICOKDT_EPSILON;    /* needs to be < 2^8 */

    /* set upbound to last WORDPHON */
    upbound = acph->headxLen - 1;
    while ((upbound >= 0) &&
           (acph->headx[upbound].head.type != PICODATA_ITEM_WORDPHON)) {
        upbound--;
    }

    if (upbound < 0) {
        /* phrase containing zero WORDPHON */
        PICODBG_DEBUG(("no WORDPHON in phrase -> no accentuation"));
        return PICO_OK;
    }

    lastprev2 = upbound;

    /* set initial history values */
    prevout = PICOKDT_HISTORY_ZERO;
    hist1 = PICOKDT_HISTORY_ZERO;
    hist2 = PICOKDT_HISTORY_ZERO;

    /* set initial nr pre/fol words/sylls, upbound is ind of last WORDPHON */
    nrwordsfol = 0;
    nrsyllsfol = 0;
    footwordsfol = 0;
    footsyllsfol = 0;
    nrwordspre = 0;
    nrsyllspre = 0;

    /* set POS of current word in valbuf[1], will be shifted right afterwards */
    valbuf[1] = acph->headx[upbound].head.info1;
    /* find first POS to the left and set valbuf[0] */
    valbuf[0] = acphAccItemSeqGetPosLeft(this, acph, lastprev2, &lastprev2);
    for (i = 2; i < 5; i++) {
        valbuf[i] = PICOKDT_EPSILON;
    }

    PICODBG_TRACE(("headxLen: %d", acph->headxLen));

    /* process from right-to-left all items in headx */
    for (i = upbound+1; i > 0; ) {
        i--;

        okay = TRUE;

        PICODBG_TRACE(("iter: %d, type: %c", i, acph->headx[i].head.type));

        /* if not (WORDPHON) */
        if ((acph->headx[i].head.type != PICODATA_ITEM_WORDPHON)) {
            continue;
        }

        PICODBG_TRACE(("iter: %d, curpos: %d", i, acph->headx[i].head.info1));

        /* get and set POS of current item, must be WORDPHON */
        curpos = acph->headx[i].head.info1;

        /* no continue so far => at [i] we have a WORDPHON item */
        /* shift all POS elements one position to the right */
        valbuf[4] = valbuf[3];
        valbuf[3] = valbuf[2];
        valbuf[2] = valbuf[1];
        valbuf[1] = valbuf[0];
        /* find next POS to the left and set valbuf[0] */
        valbuf[0] = acphAccItemSeqGetPosLeft(this, acph, lastprev2, &lastprev2);

        /* better check double than never */
        if (curpos != valbuf[2]) {
            PICODBG_WARN(("syncing POS"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR,
                                  NULL, NULL);
            valbuf[2] = curpos;
        }

        /* set history values */
        hist2 = hist1;
        hist1 = prevout;

        /* ************************************************************ */
        /* many speedups possible by avoiding double calc of attribtues */
        /* ************************************************************ */

        /* get distances */
        if ((!acphAccGetNrsRight(this, acph, i, &nrwordsfol, &nrsyllsfol,
                               &footwordsfol, &footsyllsfol)) ||
            (!acphAccGetNrsLeft(this, acph, i, &nrwordspre, &nrsyllspre))) {
            PICODBG_WARN(("problem setting distances in invec"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR,
                                  NULL, NULL);
            okay = FALSE;
        }

        PICODBG_TRACE(("%d: [%d,%d,%d,%d,%d|%d,%d|%d,%d,%d,%d|%d,%d]", i,
                       valbuf[0], valbuf[1], valbuf[2], valbuf[3], valbuf[4],
                       hist1, hist2, nrwordspre, nrsyllspre,
                       nrwordsfol, nrsyllsfol, footwordsfol, footsyllsfol));

        /* no continue so far => accentuation needed */
        /* construct input vector, which is set in dtacc */
        if (!picokdt_dtACCconstructInVec(acph->dtacc, valbuf[0], valbuf[1],
                                         valbuf[2], valbuf[3], valbuf[4],
                                         hist1, hist2, nrwordspre, nrsyllspre,
                                         nrwordsfol, nrsyllsfol, footwordsfol,
                                         footsyllsfol)) {
            /* error constructing invec */
            PICODBG_WARN(("problem with invec"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR,
                                  NULL, NULL);
            okay = FALSE;
        }
        /* classify */
        if (okay && (!picokdt_dtACCclassify(acph->dtacc, &prevout))) {
            /* error doing classification */
            PICODBG_WARN(("problem classifying"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_CLASSIFICATION,
                                  NULL, NULL);
            okay = FALSE;
        }
        /* decompose */
        if (okay && (!picokdt_dtACCdecomposeOutClass(acph->dtacc, &dtres))) {
            /* error decomposing */
            PICODBG_WARN(("problem decomposing"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_OUTVECTOR,
                                  NULL, NULL);
            okay = FALSE;
        }

        if (dtres.class > 255) {
            PICODBG_WARN(("dt class outside valid range, setting to ACC0"));
            dtres.class = PICODATA_ACC0;
        }

        if (okay && dtres.set) {
            PICODBG_DEBUG(("%d - inpos: %d, out: %d", i,valbuf[2],dtres.class));
            if (acphIsWordWithoutStress(this, acph, i)) {
                if (dtres.class != PICODATA_ACC0) {
                    acph->headx[i].head.info2 = PICODATA_ACC3;
                } else {
                    acph->headx[i].head.info2 = (picoos_uint8)dtres.class;
                }
            } else {
                acph->headx[i].head.info2 = (picoos_uint8)dtres.class;
            }
            PICODBG_DEBUG(("%d - after-nostress-corr: %d",
                           i, acph->headx[i].head.info2));
        } else {
            PICODBG_WARN(("problem determining accentuation level"));
            dtres.class = PICODATA_ITEMINFO1_ERR;
        }
    }
    return PICO_OK;
}



/* ***********************************************************************/
/* acphStep support functions */
/* ***********************************************************************/

static picoos_uint8 acphPutBoundItem(register picodata_ProcessingUnit this,
                                   register acph_subobj_t *acph,
                                   const picoos_uint8 strength,
                                   const picoos_uint8 type,
                                   picoos_uint8 *dopuoutfull,
                                   picoos_uint16 *numBytesOutput) {
    pico_status_t rv = PICO_OK;
    picoos_uint16 blen = 0;
    picodata_itemhead_t tmphead;

    *dopuoutfull = FALSE;

    /* construct BOUND item in tmpbuf and put item */
    tmphead.type = PICODATA_ITEM_BOUND;
    tmphead.info1 = strength;
    tmphead.info2 = type;
    tmphead.len = 0;
    rv = picodata_put_itemparts(&tmphead, NULL, 0, acph->tmpbuf,
                                PICODATA_MAX_ITEMSIZE, &blen);
    if (rv != PICO_OK) {
        PICODBG_ERROR(("problem creating BOUND item"));
        picoos_emRaiseException(this->common->em, rv, NULL, NULL);
        return FALSE;
    }
    /* put constructed item to ext. charbuf */
    rv = picodata_cbPutItem(this->cbOut, acph->tmpbuf, blen, &blen);

    *numBytesOutput += blen;
    if (rv == PICO_EXC_BUF_OVERFLOW) {
        PICODBG_DEBUG(("overflow in cb output buffer"));
        *dopuoutfull = TRUE;    /* ie. do PU_OUT_FULL later */
        return FALSE;
    } else if (rv != PICO_OK) {
        PICODBG_ERROR(("problem putting BOUND item"));
        picoos_emRaiseException(this->common->em, rv, NULL, NULL);
        return FALSE;
    }

    PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                       (picoos_uint8 *)"acph: ", acph->tmpbuf, blen);

    return TRUE;
}



/* ***********************************************************************/
/*                          acphStep function                              */
/* ***********************************************************************/

/*
complete phrase processed in one step, if not fast enough -> rework

init, collect into internal buffer, process, and then feed to
output buffer

init state: INIT ext           ext
state trans:     in hc1  hc2   out

INIT | putItem   =  0    0    +1      | BUSY  -> COLL (put B-SBEG item,
                                                   set do-init to false)

                                    inspace-ok-hc1
                                  needs-more-items-(phrase-or-flush)
COLL1 |getItems -n +n             0 1 | ATOMIC -> PPOSD     (got items,
                                                      if flush set do-init)
COLL2 |getItems -n +n             1 0 | ATOMIC -> PPOSD (got items, forced)
COLL3 |getItems -n +n             1 1 | IDLE          (got items, need more)
COLL4 |getItems  =  =             1 1 | IDLE             (got no items)

PPOSD | posd     = ~n~n               | BUSY     -> PWP     (posd done)
PWP   | lex/g2p  = ~n-n  0+n          | BUSY     -> PPHR    (lex/g2p done)
PPHR  | phr      = -n 0 +m=n          | BUSY     -> PACC    (phr done, m>=n)
PACC  | acc      =  0 0 ~m=n          | BUSY     -> FEED    (acc done)

                                  doinit-flag
FEED | putItems  0  0 0 -m-n  +m  0   | BUSY -> COLL    (put items)
FEED | putItems  0  0 0 -m-n  +m  1   | BUSY -> INIT    (put items)
FEED | putItems  0  0 0 -d-d  +d      | OUT_FULL        (put some items)
*/

static picodata_step_result_t acphStep(register picodata_ProcessingUnit this,
                                     picoos_int16 mode,
                                     picoos_uint16 *numBytesOutput) {
    register acph_subobj_t *acph;
    pico_status_t rv = PICO_OK;
    pico_status_t rvP = PICO_OK;
    picoos_uint16 blen = 0;
    picoos_uint16 clen = 0;
    picoos_uint16 i;


    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    acph = (acph_subobj_t *) this->subObj;
    mode = mode;        /* avoid warning "var not used in this function"*/
    *numBytesOutput = 0;
    while (1) { /* exit via return */
        PICODBG_DEBUG(("doing state %i, hLen|c1Len: %d|%d",
                       acph->procState, acph->headxLen, acph->cbufLen));

        switch (acph->procState) {

            /* *********************************************************/
            /* collect state: get item(s) from charBuf and store in
             * internal buffers, need a complete punctuation-phrase
             */
            case SA_STEPSTATE_COLLECT:

                while (acph->inspaceok && acph->needsmoreitems && (PICO_OK ==
                (rv = picodata_cbGetItem(this->cbIn, acph->tmpbuf,
                                PICODATA_MAX_ITEMSIZE, &blen)))) {
                    rvP = picodata_get_itemparts(acph->tmpbuf,
                    PICODATA_MAX_ITEMSIZE, &(acph->headx[acph->headxLen].head),
                            &(acph->cbuf[acph->cbufLen]), acph->cbufBufSize
                                    - acph->cbufLen, &clen);
                    if (rvP != PICO_OK) {
                        PICODBG_ERROR(("problem getting item parts"));
                        picoos_emRaiseException(this->common->em, rvP,
                        NULL, NULL);
                        return PICODATA_PU_ERROR;
                    }

                    /* if CMD(...FLUSH...) -> PUNC(...FLUSH...),
                     construct PUNC-FLUSH item in headx */
                    if ((acph->headx[acph->headxLen].head.type
                            == PICODATA_ITEM_CMD)
                            && (acph->headx[acph->headxLen].head.info1
                                    == PICODATA_ITEMINFO1_CMD_FLUSH)) {
                        acph->headx[acph->headxLen].head.type
                                = PICODATA_ITEM_PUNC;
                        acph->headx[acph->headxLen].head.info1
                                = PICODATA_ITEMINFO1_PUNC_FLUSH;
                        acph->headx[acph->headxLen].head.info2
                                = PICODATA_ITEMINFO2_PUNC_SENT_T;
                        acph->headx[acph->headxLen].head.len = 0;
                    }

                    /* check/set needsmoreitems */
                    if (acph->headx[acph->headxLen].head.type
                            == PICODATA_ITEM_PUNC) {
                        acph->needsmoreitems = FALSE;
                    }

                    /* check/set inspaceok, keep spare slot for forcing */
                    if ((acph->headxLen >= (PICOACPH_MAXNR_HEADX - 2))
                            || ((acph->cbufBufSize - acph->cbufLen)
                                    < PICODATA_MAX_ITEMSIZE)) {
                        acph->inspaceok = FALSE;
                    }

                    if (clen > 0) {
                        acph->headx[acph->headxLen].cind = acph->cbufLen;
                        acph->cbufLen += clen;
                    } else {
                        acph->headx[acph->headxLen].cind = 0;
                    }
                    acph->headxLen++;
                }

                if (!acph->needsmoreitems) {
                    /* 1, phrase buffered */
                    acph->procState = SA_STEPSTATE_PROCESS_PHR;
                    return PICODATA_PU_ATOMIC;
                } else if (!acph->inspaceok) {
                    /* 2, forced phrase end */
                    /* at least one slot is still free, use it to
                       force a trailing PUNC item */
                    acph->headx[acph->headxLen].head.type = PICODATA_ITEM_PUNC;
                    acph->headx[acph->headxLen].head.info1 =
                        PICODATA_ITEMINFO1_PUNC_PHRASEEND;
                    acph->headx[acph->headxLen].head.info2 =
                        PICODATA_ITEMINFO2_PUNC_PHRASE_FORCED;
                    acph->headx[acph->headxLen].head.len = 0;
                    acph->needsmoreitems = FALSE; /* not really needed for now */
                    acph->headxLen++;
                    PICODBG_WARN(("forcing phrase end, added PUNC_PHRASEEND"));
                    picoos_emRaiseWarning(this->common->em,
                                          PICO_WARN_FALLBACK, NULL,
                                          (picoos_char *)"forced phrase end");
                    acph->procState = SA_STEPSTATE_PROCESS_PHR;
                    return PICODATA_PU_ATOMIC;
                } else if (rv == PICO_EOF) {
                    /* 3, 4 */
                    return PICODATA_PU_IDLE;
                } else if ((rv == PICO_EXC_BUF_UNDERFLOW) ||
                           (rv == PICO_EXC_BUF_OVERFLOW)) {
                    /* error, no valid item in cb (UNDER) */
                    /*        or tmpbuf not large enough, not possible (OVER) */
                    /* no exception raised, left for ctrl to handle */
                    PICODBG_ERROR(("buffer under/overflow, rv: %d", rv));
                    return PICODATA_PU_ERROR;
                } else {
                    /* error, only possible if cbGetItem implementation
                       changes without this function being adapted*/
                    PICODBG_ERROR(("untreated return value, rv: %d", rv));
                    return PICODATA_PU_ERROR;
                }
                break;




            /* *********************************************************/
            /* process phr state: process items in headx and modify
             * headx in place
             */
            case SA_STEPSTATE_PROCESS_PHR:
                /* ensure there is an item in inBuf */
                if (acph->headxLen > 0) {
                    /* we have a phrase in headx, cbuf1 (can be
                       single PUNC item), do phrasing and modify headx */

                    if (PICO_OK != acphSubPhrasing(this, acph)) {
                        picoos_emRaiseException(this->common->em,
                                                PICO_ERR_OTHER, NULL, NULL);
                        return PICODATA_PU_ERROR;
                    }
                    acph->procState = SA_STEPSTATE_PROCESS_ACC;
                } else if (acph->headxLen == 0) {    /* no items in inBuf */
                    PICODBG_WARN(("no items in inBuf"));
                    acph->procState = SA_STEPSTATE_COLLECT;
                    return PICODATA_PU_BUSY;
                }

#if defined (PICO_DEBUG_NOTNEEDED)
                if (1) {
                    picoos_uint8 i, j, ittype;
                    for (i = 0; i < acph->headxLen; i++) {
                        if ((acph->headx[i].boundstrength != 0) &&
                            (acph->headx[i].boundstrength !=
                             PICODATA_ITEMINFO1_BOUND_PHR0)) {
                            PICODBG_INFO(("acph-p: boundstrength '%c', "
                                          "boundtype '%c'",
                                          acph->headx[i].boundstrength,
                                          acph->headx[i].boundtype));
                        }

                        ittype = acph->headx[i].head.type;
                        PICODBG_INFO_CTX();
                        PICODBG_INFO_MSG(("acph-p: ("));
                        PICODBG_INFO_MSG(("'%c',", ittype));
                        if ((32 <= acph->headx[i].head.info1) &&
                            (acph->headx[i].head.info1 < 127) &&
                            (ittype != PICODATA_ITEM_WORDPHON)) {
                            PICODBG_INFO_MSG(("'%c',",acph->headx[i].head.info1));
                        } else {
                            PICODBG_INFO_MSG(("%3d,", acph->headx[i].head.info1));
                        }
                        if ((32 <= acph->headx[i].head.info2) &&
                            (acph->headx[i].head.info2 < 127)) {
                            PICODBG_INFO_MSG(("'%c',",acph->headx[i].head.info2));
                        } else {
                            PICODBG_INFO_MSG(("%3d,", acph->headx[i].head.info2));
                        }
                        PICODBG_INFO_MSG(("%3d)", acph->headx[i].head.len));

                        for (j = 0; j < acph->headx[i].head.len; j++) {
                            if ((ittype == PICODATA_ITEM_CMD)) {
                                PICODBG_INFO_MSG(("%c",
                                        acph->cbuf[acph->headx[i].cind+j]));
                            } else {
                                PICODBG_INFO_MSG(("%4d",
                                        acph->cbuf[acph->headx[i].cind+j]));
                            }
                        }
                        PICODBG_INFO_MSG(("\n"));
                    }
                }
#endif

                break;


            /* *********************************************************/
            /* process acc state: process items in headx and modify
             * headx in place
             */
            case SA_STEPSTATE_PROCESS_ACC:
                /* ensure there is an item in inBuf */
                if (acph->headxLen > 0) {
                    /* we have a phrase in headx, cbuf (can be
                       single PUNC item), do accentuation and modify headx */
                    if (PICO_OK != acphAccentuation(this, acph)) {
                        picoos_emRaiseException(this->common->em,
                                                PICO_ERR_OTHER, NULL, NULL);
                        return PICODATA_PU_ERROR;
                    }
                    acph->procState = SA_STEPSTATE_FEED;
                } else if (acph->headxLen == 0) {    /* no items in inBuf */
                    PICODBG_WARN(("no items in inBuf"));
                    acph->procState = SA_STEPSTATE_COLLECT;
                    return PICODATA_PU_BUSY;
                }
                break;


            /* *********************************************************/
            /* feed state: copy item in internal outBuf to output charBuf */
            case SA_STEPSTATE_FEED: {
                picoos_uint16 indupbound;
                picoos_uint8 dopuoutfull;

                PICODBG_DEBUG(("put out items (bot, len): (%d, %d)",
                               acph->headxBottom, acph->headxLen));

                indupbound = acph->headxBottom + acph->headxLen;
                dopuoutfull = FALSE;

                if (acph->headxBottom == 0) {
                    /* construct first BOUND item in tmpbuf and put item */
                    /* produce BOUND unless it is followed by a term/flush) */
                    if (acph->headx[0].head.info1
                            != PICODATA_ITEMINFO1_PUNC_FLUSH) {
                        if (!acphPutBoundItem(this, acph,
                                acph->headx[0].boundstrength,
                                acph->headx[0].boundtype, &dopuoutfull,
                                numBytesOutput)) {
                            if (dopuoutfull) {
                                PICODBG_DEBUG(("feeding overflow"));
                                return PICODATA_PU_OUT_FULL;
                            } else {
                                /* ERR-msg and exception done in acphPutBoundItem */
                                return PICODATA_PU_ERROR;
                            }
                        }
                    }
                }

                /* for all items in headx, cbuf */
                for (i = acph->headxBottom; i < indupbound; i++) {

                    switch (acph->headx[i].head.type) {
                        case PICODATA_ITEM_PUNC:
                            /* if sentence end, put SEND bound */
                            if ((acph->headx[i].head.info1 ==
                                 PICODATA_ITEMINFO1_PUNC_SENTEND) &&
                                (i == (indupbound - 1))) {
                                /* construct and put BOUND item */
                                if (!acphPutBoundItem(this, acph,
                                            PICODATA_ITEMINFO1_BOUND_SEND,
                                            PICODATA_ITEMINFO2_NA,
                                            &dopuoutfull, numBytesOutput)) {
                                    if (dopuoutfull) {
                                        PICODBG_DEBUG(("feeding overflow"));
                                        return PICODATA_PU_OUT_FULL;
                                    } else {
                                        /* ERR-msg and exception done
                                           in acphPutBoundItem */
                                        return PICODATA_PU_ERROR;
                                    }
                                }
                            } else if ((acph->headx[i].head.info1 ==
                                 PICODATA_ITEMINFO1_PUNC_FLUSH) &&
                                (i == (indupbound - 1))) {
                                /* construct and put BOUND item */
                                if (!acphPutBoundItem(this, acph,
                                            PICODATA_ITEMINFO1_BOUND_TERM,
                                            PICODATA_ITEMINFO2_NA,
                                            &dopuoutfull, numBytesOutput)) {
                                    if (dopuoutfull) {
                                        PICODBG_DEBUG(("feeding overflow"));
                                        return PICODATA_PU_OUT_FULL;
                                    } else {
                                        /* ERR-msg and exception done
                                           in acphPutBoundItem */
                                        return PICODATA_PU_ERROR;
                                    }
                                }
                            }
                            /* else, good-bye PUNC, not needed anymore */
                            break;
                        default:

                            /* PHR2/3 maybe existing, check and add
                               BOUND item now, if needed */
                            if ((acph->headx[i].boundstrength ==
                                 PICODATA_ITEMINFO1_BOUND_PHR2) ||
                                (acph->headx[i].boundstrength ==
                                 PICODATA_ITEMINFO1_BOUND_PHR3)) {
                                if (!acphPutBoundItem(this, acph,
                                            acph->headx[i].boundstrength,
                                            acph->headx[i].boundtype,
                                            &dopuoutfull, numBytesOutput)) {
                                    if (dopuoutfull) {
                                        PICODBG_DEBUG(("feeding overflow"));
                                        return PICODATA_PU_OUT_FULL;
                                    } else {
                                        /* ERR-msg and exception done
                                           in acphPutBoundItem */
                                        return PICODATA_PU_ERROR;
                                    }
                                }
                            }

                            /* copy item unmodified */
                            rv = picodata_put_itemparts(&(acph->headx[i].head),
                                     &(acph->cbuf[acph->headx[i].cind]),
                                     acph->headx[i].head.len,
                                     acph->tmpbuf, PICODATA_MAX_ITEMSIZE,
                                     &blen);

                            rvP = picodata_cbPutItem(this->cbOut, acph->tmpbuf,
                                    PICODATA_MAX_ITEMSIZE, &clen);

                            *numBytesOutput += clen;

                            PICODBG_DEBUG(("put item, status: %d", rvP));

                            if (rvP == PICO_OK) {
                                acph->headxBottom++;
                                acph->headxLen--;
                            } else if (rvP == PICO_EXC_BUF_OVERFLOW) {
                                /* try again next time, but PHR2/3
                                   bound already added if existing,
                                   ensure it's not output a 2nd
                                   time */
                                PICODBG_DEBUG(("feeding overflow"));
                                acph->headx[i].boundstrength = 0;
                                return PICODATA_PU_OUT_FULL;
                            } else {
                                /* error, should never happen */
                                PICODBG_ERROR(("untreated return value, rvP: %d", rvP));
                                return PICODATA_PU_ERROR;
                            }

                            PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                                               (picoos_uint8 *)"acph: ",
                                               acph->tmpbuf, PICODATA_MAX_ITEMSIZE);

                            break;
                    } /*switch*/
                } /*for*/

                /* reset headx, cbuf */
                acph->headxBottom = 0;
                acph->headxLen = 0;
                acph->cbufLen = 0;
                for (i = 0; i < PICOACPH_MAXNR_HEADX; i++) {
                    acph->headx[i].boundstrength = 0;
                }

                /* reset collect state support variables */
                acph->inspaceok = TRUE;
                acph->needsmoreitems = TRUE;

                acph->procState = SA_STEPSTATE_COLLECT;
                return PICODATA_PU_BUSY;
                break;
            }

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
