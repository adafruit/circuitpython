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
 * @file picosa.c
 *
 * sentence analysis - POS disambiguation
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
#include "picokdt.h"
#include "picoklex.h"
#include "picoktab.h"
#include "picokfst.h"
#include "picotrns.h"
#include "picodata.h"
#include "picosa.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* PU saStep states */
#define SA_STEPSTATE_COLLECT       0
#define SA_STEPSTATE_PROCESS_POSD 10
#define SA_STEPSTATE_PROCESS_WPHO 11
#define SA_STEPSTATE_PROCESS_TRNS_PARSE 12
#define SA_STEPSTATE_PROCESS_TRNS_FST 13
#define SA_STEPSTATE_FEED          2

#define SA_MAX_ALTDESC_SIZE (30*(PICOTRNS_MAX_NUM_POSSYM + 2))

#define SA_MSGSTR_SIZE 32

/*  subobject    : SentAnaUnit
 *  shortcut     : sa
 *  context size : one phrase, max. 30 non-PUNC items, for non-processed items
 *                 one item if internal input empty
 */

/** @addtogroup picosa

  internal buffers:

  - headx: array for extended item heads of fixed size (head plus
    index for content, plus two fields for boundary strength/type)

  - cbuf1, cbuf2: buffers for item contents (referenced by index in
    headx). Future: replace these two buffers by a single double-sided
    buffer (double shrink-grow type)

  0. bottom up filling of items in headx and cbuf1

  1. POS disambiguation (right-to-left, top-to-bottom):
  - number and sequence of items unchanged
  - item content can only get smaller (reducing nr of results in WORDINDEX)
  -> info stays in "headx, cbuf1" and changed in place                      \n
     WORDGRAPH(POSes,NA)graph             -> WORDGRAPH(POS,NA)graph         \n
     WORDINDEX(POSes,NA)POS1ind1...POSNindN  -> WORDINDEX(POS,NA)POS|ind    \n

  2. lex-index lookup and G2P (both directions possible, left-to-right done):
  - number and sequence of items unchanged, item head info and content
    changes
  -> headx changed in place; cbuf1 to cbuf2                                 \n
     WORDGRAPH(POS,NA)graph    -> WORDPHON(POS,NA)phon                      \n
     WORDINDEX(POS,NA)POS|ind  -> WORDPHON(POS,NA)phon                      \n

  3. phrasing (right-to-left):

     Previous (before introducing SBEG)\n
     ----------------------------------
                                           1|          2|             3|    4|    \n
     e.g. from      WP WP WP       WP WP PUNC  WP WP PUNC  WP WP WP PUNC FLUSH    \n
     e.g. to  BINIT WP WP WP BPHR3 WP WP BPHR1 WP WP BSEND WP WP WP BSEND BTERM   \n
              |1                         |2          |3             |4            \n

     3-level bound state: to keep track of bound strength from end of
     previous punc-phrase, then BOUND item output as first item
     (strength from prev punc-phrase and type from current
     punc-phrase).

     trailing PUNC item       bound states
                              INIT         SEND         PHR1
       PUNC(SENTEND, T)       B(I,T)>SEND  B(S,T)>SEND  B(P1,T)>SEND
       PUNC(SENTEND, Q)       B(I,Q)>SEND  B(S,Q)>SEND  B(P1,Q)>SEND
       PUNC(SENTEND, E)       B(I,E)>SEND  B(S,E)>SEND  B(P1,E)>SEND
       PUNC(PHRASEEND, P)     B(I,P)>PHR1  B(S,P)>PHR1  B(P1,P)>PHR1
       PUNC(PHRASEEND, FORC)  B(I,P)>PHR1  B(S,P)>PHR1  B(P1,P)>PHR1
       PUNC(FLUSH, T)         B(I,T)..     B(S,T)..     B(P1,T)..
                                B(T,NA)      B(T,NA)      B(T,NA)
                                >INIT        >INIT        >INIT

     PHR2/3 case:
     trailing PUNC item       bound states
                          INIT              SEND              PHR1
       PUNC(SENTEND, T)   B(I,P)B(P,T)>SEND B(S,P)B(P,T)>SEND B(P1,P)B(P,T)>SEND
       PUNC(SENTEND, Q)   B(I,P)B(P,Q)>SEND B(S,P)B(P,Q)>SEND B(P1,P)B(P,Q)>SEND
       PUNC(SENTEND, E)   B(I,P)B(P,E)>SEND B(S,P)B(P,E)>SEND B(P1,P)B(P,E)>SEND
       PUNC(PHRASEEND, P) B(I,P)B(P,P)>PHR1 B(S,P)B(P,P)>PHR1 B(P1,P)B(P,P)>PHR1
       PUNC(PHREND, FORC) B(I,P)B(P,P)>PHR1 B(S,P)B(P,P)>PHR1 B(P1,P)B(P,P)>PHR1
       PUNC(FLUSH, T)     B(I,P)B(P,T)..    B(S,T)B(P,T)..    B(P1,T)B(P,T)..
                            B(T,NA)             B(T,NA)             B(T,NA)
                            >INIT               >INIT               >INIT

     Current
     --------
     e.g. from      WP WP WP       WP WP PUNC  WP WP PUNC        WP WP WP PUNC  FLUSH
     e.g. to  BSBEG WP WP WP BPHR3 WP WP BPHR1 WP WP BSEND BSBEG WP WP WP BSEND BTERM
              |1                         |2                |3                   |4

     2-level bound state: The internal buffer contains one primary phrase (sometimes forced, if buffer
     allmost full), with the trailing PUNCT item included (last item).
     If the trailing PUNC is a a primary phrase separator, the
       item is not output, but instead, the bound state is set to PPHR, so that the correct BOUND can
       be output at the start of the next primary phrase.
     Otherwise,
       the item is converted to the corresponding BOUND and output. the bound state is set to SSEP,
       so that a BOUND of type SBEG is output at the start of the next primary phrase.

     trailing PUNC item       bound states
                              SSEP           PPHR
       PUNC(SENTEND, X)       B(B,X)>SSEP    B(P1,X)>SSEP  (X = T | Q | E)
       PUNC(FLUSH, T)         B(B,T)>SSEP*    B(P1,T)>SSEP
       PUNC(PHRASEEND, P)     B(B,P)>PPHR    B(P1,P)>PPHR
       PUNC(PHRASEEND, FORC)  B(B,P)>PPHR    B(P1,P)>PPHR

*    If more than one sentence separators follow each other (e.g. SEND-FLUSH, SEND-SEND) then
     all but the first will be treated as an (empty) phrase containing just this item.
     If this (single) item is a flush, creation of SBEG is suppressed.


  - dtphr phrasing tree (rather subphrasing tree it should be called)
    determines
      BOUND_PHR2
      BOUND_PHR3
  - boundary strenghts are determined for every word (except the
    first one) from right-to-left. The boundary types mark the phrase
    type of the phrase following the boundary.
  - number of items actually changed (new BOUND items added): because
    of fixed size without content, two fields are contained in headx
    to indicate if a BOUND needs to be added to the LEFT of the item.
    -> headx further extended with boundary strength and type info to
    indicate that to the left of the headx ele a BOUND needs to be
    inserted when outputting.

  4. accentuation:
  - number of items unchanged, content unchanged, only head info changes
  -> changed in place in headx
*/


typedef struct {
    picodata_itemhead_t head;
    picoos_uint16 cind;
} picosa_headx_t;


typedef struct sa_subobj {
    picoos_uint8 procState; /* for next processing step decision */

    picoos_uint8 inspaceok;      /* flag: headx/cbuf1 has space for an item */
    picoos_uint8 needsmoreitems; /* flag: need more items */
    picoos_uint8 phonesTransduced; /* flag: */

    picoos_uint8 tmpbuf[PICODATA_MAX_ITEMSIZE];  /* tmp. location for an item */

    picosa_headx_t headx[PICOSA_MAXNR_HEADX];
    picoos_uint16 headxBottom; /* bottom */
    picoos_uint16 headxLen;    /* length, 0 if empty */

    picoos_uint8 cbuf1[PICOSA_MAXSIZE_CBUF];
    picoos_uint16 cbuf1BufSize; /* actually allocated size */
    picoos_uint16 cbuf1Len;     /* length, 0 if empty */

    picoos_uint8 cbuf2[PICOSA_MAXSIZE_CBUF];
    picoos_uint16 cbuf2BufSize; /* actually allocated size */
    picoos_uint16 cbuf2Len;     /* length, 0 if empty */

    picotrns_possym_t phonBufA[PICOTRNS_MAX_NUM_POSSYM+1];
    picotrns_possym_t phonBufB[PICOTRNS_MAX_NUM_POSSYM+1];
    picotrns_possym_t * phonBuf;
    picotrns_possym_t * phonBufOut;
    picoos_uint16 phonReadPos, phonWritePos; /* next pos to read from phonBufIn, next pos to write to phonBufIn */
    picoos_uint16 nextReadPos; /* position of (potential) next item to read from */


    /* buffer for internal calculation of transducer */
    picotrns_AltDesc altDescBuf;
    /* the number of AltDesc in the buffer */
    picoos_uint16 maxAltDescLen;

    /* tab knowledge base */
    picoktab_Graphs tabgraphs;
    picoktab_Phones tabphones;
    picoktab_Pos tabpos;
    picoktab_FixedIds fixedIds;

    /* dtposd knowledge base */
    picokdt_DtPosD dtposd;

    /* dtg2p knowledge base */
    picokdt_DtG2P dtg2p;

    /* lex knowledge base */
    picoklex_Lex lex;

    /* ulex knowledge bases */
    picoos_uint8 numUlex;
    picoklex_Lex ulex[PICOKNOW_MAX_NUM_ULEX];

    /* fst knowledge bases */
    picoos_uint8 numFsts;
    picokfst_FST fst[PICOKNOW_MAX_NUM_WPHO_FSTS];
    picoos_uint8 curFst; /* the fst to be applied next */


} sa_subobj_t;


static pico_status_t saInitialize(register picodata_ProcessingUnit this, picoos_int32 resetMode) {
    sa_subobj_t * sa;
    picoos_uint16 i;
    picokfst_FST fst;
    picoknow_kb_id_t fstKbIds[PICOKNOW_MAX_NUM_WPHO_FSTS] = PICOKNOW_KBID_WPHO_ARRAY;
    picoklex_Lex ulex;
    picoknow_kb_id_t ulexKbIds[PICOKNOW_MAX_NUM_ULEX] = PICOKNOW_KBID_ULEX_ARRAY;

    PICODBG_DEBUG(("calling"));

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(this->common->em,
                                       PICO_ERR_NULLPTR_ACCESS, NULL, NULL);
    }
    sa = (sa_subobj_t *) this->subObj;

    /*  sa->common = this->common; */

    sa->procState = SA_STEPSTATE_COLLECT;

    sa->inspaceok = TRUE;
    sa->needsmoreitems = TRUE;

    sa->headxBottom = 0;
    sa->headxLen = 0;
    sa->cbuf1BufSize = PICOSA_MAXSIZE_CBUF;
    sa->cbuf2BufSize = PICOSA_MAXSIZE_CBUF;
    sa->cbuf1Len = 0;
    sa->cbuf2Len = 0;

    /* init headx, cbuf1, cbuf2 */
    for (i = 0; i < PICOSA_MAXNR_HEADX; i++){
        sa->headx[i].head.type = 0;
        sa->headx[i].head.info1 = PICODATA_ITEMINFO1_NA;
        sa->headx[i].head.info2 = PICODATA_ITEMINFO2_NA;
        sa->headx[i].head.len = 0;
        sa->headx[i].cind = 0;
    }
    for (i = 0; i < PICOSA_MAXSIZE_CBUF; i++) {
        sa->cbuf1[i] = 0;
        sa->cbuf2[i] = 0;
    }


    /* possym buffer */
    sa->phonesTransduced = FALSE;
    sa->phonBuf = sa->phonBufA;
    sa->phonBufOut = sa->phonBufB;
    sa->phonReadPos = 0;
    sa->phonWritePos = 0;
    sa->nextReadPos = 0;

    if (resetMode == PICO_RESET_SOFT) {
        /*following initializations needed only at startup or after a full reset*/
        return PICO_OK;
    }

    /* kb fst[] */
    sa->numFsts = 0;
    for (i = 0; i<PICOKNOW_MAX_NUM_WPHO_FSTS; i++) {
        fst = picokfst_getFST(this->voice->kbArray[fstKbIds[i]]);
        if (NULL != fst) {
            sa->fst[sa->numFsts++] = fst;
        }
    }
    sa->curFst = 0;
    PICODBG_DEBUG(("got %i fsts", sa->numFsts));
    /* kb fixedIds */
    sa->fixedIds = picoktab_getFixedIds(this->voice->kbArray[PICOKNOW_KBID_FIXED_IDS]);

    /* kb tabgraphs */
    sa->tabgraphs =
        picoktab_getGraphs(this->voice->kbArray[PICOKNOW_KBID_TAB_GRAPHS]);
    if (sa->tabgraphs == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got tabgraphs"));

    /* kb tabphones */
    sa->tabphones =
        picoktab_getPhones(this->voice->kbArray[PICOKNOW_KBID_TAB_PHONES]);
    if (sa->tabphones == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got tabphones"));

#ifdef PICO_DEBU
    {
        picoos_uint16 itmp;
        for (itmp = 0; itmp < 256; itmp++) {
            if (picoktab_hasVowelProp(sa->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasVowel: %d", itmp));
            }
            if (picoktab_hasDiphthProp(sa->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasDiphth: %d", itmp));
            }
            if (picoktab_hasGlottProp(sa->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasGlott: %d", itmp));
            }
            if (picoktab_hasNonsyllvowelProp(sa->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasNonsyllvowel: %d", itmp));
            }
            if (picoktab_hasSyllconsProp(sa->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones hasSyllcons: %d", itmp));
            }
            if (picoktab_isPrimstress(sa->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones isPrimstress: %d", itmp));
            }
            if (picoktab_isSecstress(sa->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones isSecstress: %d", itmp));
            }
            if (picoktab_isSyllbound(sa->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones isSyllbound: %d", itmp));
            }
            if (picoktab_isPause(sa->tabphones, itmp)) {
                PICODBG_DEBUG(("tabphones isPause: %d", itmp));
            }
        }

        PICODBG_DEBUG(("tabphones primstressID: %d",
                       picoktab_getPrimstressID(sa->tabphones)));
        PICODBG_DEBUG(("tabphones secstressID: %d",
                       picoktab_getSecstressID(sa->tabphones)));
        PICODBG_DEBUG(("tabphones syllboundID: %d",
                       picoktab_getSyllboundID(sa->tabphones)));
        PICODBG_DEBUG(("tabphones pauseID: %d",
                       picoktab_getPauseID(sa->tabphones)));
    }
#endif

    /* kb tabpos */
    sa->tabpos =
        picoktab_getPos(this->voice->kbArray[PICOKNOW_KBID_TAB_POS]);
    if (sa->tabpos == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got tabpos"));

    /* kb dtposd */
    sa->dtposd = picokdt_getDtPosD(this->voice->kbArray[PICOKNOW_KBID_DT_POSD]);
    if (sa->dtposd == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got dtposd"));

    /* kb dtg2p */
    sa->dtg2p = picokdt_getDtG2P(this->voice->kbArray[PICOKNOW_KBID_DT_G2P]);
    if (sa->dtg2p == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got dtg2p"));

    /* kb lex */
    sa->lex = picoklex_getLex(this->voice->kbArray[PICOKNOW_KBID_LEX_MAIN]);
    if (sa->lex == NULL) {
        return picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    PICODBG_DEBUG(("got lex"));

    /* kb ulex[] */
    sa->numUlex = 0;
    for (i = 0; i<PICOKNOW_MAX_NUM_ULEX; i++) {
        ulex = picoklex_getLex(this->voice->kbArray[ulexKbIds[i]]);
        if (NULL != ulex) {
            sa->ulex[sa->numUlex++] = ulex;
        }
    }
    PICODBG_DEBUG(("got %i user lexica", sa->numUlex));

    return PICO_OK;
}

static picodata_step_result_t saStep(register picodata_ProcessingUnit this,
                                     picoos_int16 mode,
                                     picoos_uint16 *numBytesOutput);

static pico_status_t saTerminate(register picodata_ProcessingUnit this) {
    return PICO_OK;
}

static pico_status_t saSubObjDeallocate(register picodata_ProcessingUnit this,
                                        picoos_MemoryManager mm) {
    sa_subobj_t * sa;
    if (NULL != this) {
        sa = (sa_subobj_t *) this->subObj;
        picotrns_deallocate_alt_desc_buf(mm,&sa->altDescBuf);
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}


picodata_ProcessingUnit picosa_newSentAnaUnit(picoos_MemoryManager mm,
                                              picoos_Common common,
                                              picodata_CharBuffer cbIn,
                                              picodata_CharBuffer cbOut,
                                              picorsrc_Voice voice) {
    picodata_ProcessingUnit this;
    sa_subobj_t * sa;
    this = picodata_newProcessingUnit(mm, common, cbIn, cbOut, voice);
    if (this == NULL) {
        return NULL;
    }

    this->initialize = saInitialize;
    PICODBG_DEBUG(("set this->step to saStep"));
    this->step = saStep;
    this->terminate = saTerminate;
    this->subDeallocate = saSubObjDeallocate;

    this->subObj = picoos_allocate(mm, sizeof(sa_subobj_t));
    if (this->subObj == NULL) {
        picoos_deallocate(mm, (void *)&this);
        picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM, NULL, NULL);
        return NULL;
    }

    sa = (sa_subobj_t *) this->subObj;

    sa->altDescBuf = picotrns_allocate_alt_desc_buf(mm, SA_MAX_ALTDESC_SIZE, &sa->maxAltDescLen);
    if (NULL == sa->altDescBuf) {
        picotrns_deallocate_alt_desc_buf(mm,&sa->altDescBuf);
        picoos_deallocate(mm, (void *)&sa);
        picoos_deallocate(mm, (void *)&this);
        picoos_emRaiseException(common->em,PICO_EXC_OUT_OF_MEM, NULL, NULL);
    }


    saInitialize(this, PICO_RESET_FULL);
    return this;
}


/* ***********************************************************************/
/* PROCESS_POSD disambiguation functions */
/* ***********************************************************************/

/* find next POS to the right of 'ind' and return its POS and index */
static picoos_uint8 saPosDItemSeqGetPosRight(register picodata_ProcessingUnit this,
                                            register sa_subobj_t *sa,
                                            const picoos_uint16 ind,
                                            const picoos_uint16 top,
                                            picoos_uint16 *rightind) {
    picoos_uint8 val;
    picoos_int32 i;

    val = PICOKDT_EPSILON;
    for (i = ind + 1; ((val == PICOKDT_EPSILON) && (i < top)); i++) {
        if ((sa->headx[i].head.type == PICODATA_ITEM_WORDGRAPH) ||
                (sa->headx[i].head.type == PICODATA_ITEM_WORDINDEX)  ||
                (sa->headx[i].head.type == PICODATA_ITEM_WORDPHON) ) {
            val = sa->headx[i].head.info1;
        }
    }
    *rightind = i - 1;
    return val;
}


/* left-to-right, for each WORDGRAPH/WORDINDEX/WORDPHON do posd */
static pico_status_t saDisambPos(register picodata_ProcessingUnit this,
                                 register sa_subobj_t *sa) {
    picokdt_classify_result_t dtres;
    picoos_uint8 half_nratt_posd = PICOKDT_NRATT_POSD >> 1;
    picoos_uint16 valbuf[PICOKDT_NRATT_POSD]; /* only [0..half_nratt_posd] can be >2^8 */
    picoos_uint16 prevout;   /* direct dt output (hist.) or POS of prev word */
    picoos_uint16 lastprev3; /* last index of POS(es) found to the left */
    picoos_uint16 curPOS;     /* POS(es) of current word */
    picoos_int32 first;    /* index of first item with POS(es) */
    picoos_int32 ci;
    picoos_uint8 okay;       /* two uses: processing okay and lexind resovled */
    picoos_uint8 i;
    picoos_uint16 inval;
    picoos_uint16 fallback;

    /* set initial values */
    okay = TRUE;
    prevout = PICOKDT_HISTORY_ZERO;
    curPOS = PICODATA_ITEMINFO1_ERR;
    first = 0;

    while ((first < sa->headxLen) &&
           (sa->headx[first].head.type != PICODATA_ITEM_WORDGRAPH) &&
           (sa->headx[first].head.type != PICODATA_ITEM_WORDINDEX) &&
           (sa->headx[first].head.type != PICODATA_ITEM_WORDPHON)) {
        first++;
    }
    if (first >= sa->headxLen) {
        /* phrase not containing an item with POSes info, e.g. single flush */
        PICODBG_DEBUG(("no item with POSes found"));
        return PICO_OK;
    }

    lastprev3 = first;

    for (i = 0; i <= half_nratt_posd; i++) {
        valbuf[i] = PICOKDT_HISTORY_ZERO;
    }
    /* set POS(es) of current word, will be shifted afterwards */
    valbuf[half_nratt_posd+1] = sa->headx[first].head.info1;
    for (i = half_nratt_posd+2; i < PICOKDT_NRATT_POSD; i++) {
    /* find next POS to the right and set valbuf[i] */
        valbuf[i] = saPosDItemSeqGetPosRight(this, sa, lastprev3, sa->headxLen, &lastprev3);
    }

    PICODBG_TRACE(("headxLen: %d", sa->headxLen));

    /* process from left to right all items in headx */
    for (ci = first; ci < sa->headxLen; ci++) {
        okay = TRUE;

        PICODBG_TRACE(("iter: %d, type: %c", ci, sa->headx[ci].head.type));

        /* if not (WORDGRAPH or WORDINDEX) */
        if ((sa->headx[ci].head.type != PICODATA_ITEM_WORDGRAPH) &&
                (sa->headx[ci].head.type != PICODATA_ITEM_WORDINDEX)  &&
                (sa->headx[ci].head.type != PICODATA_ITEM_WORDPHON)) {
            continue;
        }

        PICODBG_TRACE(("iter: %d, curPOS: %d", ci, sa->headx[ci].head.info1));

        /* no continue so far => at [ci] we have a WORDGRAPH / WORDINDEX item */
        /* shift all elements one position to the left */
        /* shift predicted values (history) */
        for (i=1; i<half_nratt_posd; i++) {
            valbuf[i-1] = valbuf[i];
        }
        /* insert previously predicted value (now history) */
        valbuf[half_nratt_posd-1] = prevout;
        /* shift not yet predicted values */
        for (i=half_nratt_posd+1; i<PICOKDT_NRATT_POSD; i++) {
            valbuf[i-1] = valbuf[i];
        }
        /* find next POS to the right and set valbuf[PICOKDT_NRATT_POSD-1] */
        valbuf[PICOKDT_NRATT_POSD-1] = saPosDItemSeqGetPosRight(this, sa, lastprev3, sa->headxLen, &lastprev3);

        /* just to be on the safe side; the following should never happen */
        if (sa->headx[ci].head.info1 != valbuf[half_nratt_posd]) {
            PICODBG_WARN(("syncing POS"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR,
                                  NULL, NULL);
            valbuf[half_nratt_posd] = sa->headx[ci].head.info1;
        }

        curPOS = valbuf[half_nratt_posd];

        /* Check if POS disambiguation not needed */
        if (picoktab_isUniquePos(sa->tabpos, (picoos_uint8) curPOS)) {
            /* not needed */
            inval = 0;
            fallback = 0;
            if (!picokdt_dtPosDreverseMapOutFixed(sa->dtposd, curPOS,
                                       &prevout, &fallback)) {
                if (fallback) {
                    prevout = fallback;

                } else {
                    PICODBG_ERROR(("problem doing reverse output mapping"));
                    prevout = curPOS;
                }
            }
            PICODBG_DEBUG(("keeping: %d", sa->headx[ci].head.info1));
            continue;
        }

        /* assuming PICOKDT_NRATT_POSD == 7 */
        PICODBG_DEBUG(("%d: [%d %d %d %d %d %d %d]",
                       ci, valbuf[0], valbuf[1], valbuf[2],
                       valbuf[3], valbuf[4], valbuf[5], valbuf[6]));

        /* no continue so far => POS disambiguation needed */
        /* construct input vector, which is set in dtposd */
        if (!picokdt_dtPosDconstructInVec(sa->dtposd, valbuf)) {
            /* error constructing invec */
            PICODBG_WARN(("problem with invec"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR,
                                  NULL, NULL);
            okay = FALSE;
        }
        /* classify */
        if (okay && (!picokdt_dtPosDclassify(sa->dtposd, &prevout))) {
            /* error doing classification */
            PICODBG_WARN(("problem classifying"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_CLASSIFICATION,
                                  NULL, NULL);
            okay = FALSE;
        }
        /* decompose */
        if (okay && (!picokdt_dtPosDdecomposeOutClass(sa->dtposd, &dtres))) {
            /* error decomposing */
            PICODBG_WARN(("problem decomposing"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_OUTVECTOR,
                                  NULL, NULL);
            okay = FALSE;
        }
        if (okay && dtres.set) {
            PICODBG_DEBUG(("in: %d, out: %d", valbuf[3], dtres.class));
        } else {
            PICODBG_WARN(("problem disambiguating POS"));
            dtres.class = PICODATA_ITEMINFO1_ERR;
        }

        if (dtres.class > 255) {
            PICODBG_WARN(("dt result outside valid range, setting pos to ERR"));
            dtres.class = PICODATA_ITEMINFO1_ERR;
        }

        sa->headx[ci].head.info1 = (picoos_uint8)dtres.class;
        if (sa->headx[ci].head.type == PICODATA_ITEM_WORDINDEX) {
            /* find pos/ind entry in cbuf matching unique,
               disambiguated POS, adapt current headx cind/len
               accordingly */
            PICODBG_DEBUG(("select phon based on POS disambiguation"));
            okay = FALSE;
            for (i = 0; i < sa->headx[ci].head.len; i += PICOKLEX_POSIND_SIZE) {
                PICODBG_DEBUG(("comparing POS at cind + %d", i));
                if (picoktab_isPartOfPosGroup(sa->tabpos,
                            (picoos_uint8)dtres.class,
                            sa->cbuf1[sa->headx[ci].cind + i])) {
                    PICODBG_DEBUG(("found match for entry %d",
                                   i/PICOKLEX_POSIND_SIZE + 1));
                    sa->headx[ci].cind += i;
                    okay = TRUE;
                    break;
                }
            }
            /* not finding a match is possible if posd predicts a POS that
               is not part of any of the input POSes -> no warning */
#if defined(PICO_DEBUG)
            if (!okay) {
                PICODBG_DEBUG(("no match found, selecting 1st entry"));
            }
#endif
            sa->headx[ci].head.len = PICOKLEX_POSIND_SIZE;
        }
    }
    return PICO_OK;
}


/* ***********************************************************************/
/* PROCESS_WPHO functions, copy, lexindex, and g2p */
/* ***********************************************************************/

/* ************** copy ***************/

static pico_status_t saCopyItemContent1to2(register picodata_ProcessingUnit this,
                                           register sa_subobj_t *sa,
                                           picoos_uint16 ind) {
    picoos_uint16 i;
    picoos_uint16 cind1;

    /* set headx.cind, and copy content, head unchanged */
    cind1 = sa->headx[ind].cind;
    sa->headx[ind].cind = sa->cbuf2Len;

    /* check cbufLen */
    if (sa->headx[ind].head.len > (sa->cbuf2BufSize - sa->cbuf2Len)) {
        sa->headx[ind].head.len = sa->cbuf2BufSize - sa->cbuf2Len;
        PICODBG_WARN(("phones skipped"));
        picoos_emRaiseWarning(this->common->em,
                              PICO_WARN_INCOMPLETE, NULL, NULL);
        if (sa->headx[ind].head.len == 0) {
            sa->headx[ind].cind = 0;
        }
    }

    for (i = 0; i < sa->headx[ind].head.len; i++) {
        sa->cbuf2[sa->cbuf2Len] = sa->cbuf1[cind1 + i];
        sa->cbuf2Len++;
    }

    PICODBG_DEBUG(("%c item, len: %d",
                   sa->headx[ind].head.type, sa->headx[ind].head.len));

    return PICO_OK;
}


/* ************** lexindex ***************/

static pico_status_t saLexIndLookup(register picodata_ProcessingUnit this,
                                    register sa_subobj_t *sa,
                                    picoklex_Lex lex,
                                    picoos_uint16 ind) {
    picoos_uint8 pos;
    picoos_uint8 *phones;
    picoos_uint8 plen;
    picoos_uint16 i;

    if (picoklex_lexIndLookup(lex, &(sa->cbuf1[sa->headx[ind].cind + 1]),
                              PICOKLEX_IND_SIZE, &pos, &phones, &plen)) {
        sa->headx[ind].cind = sa->cbuf2Len;

        /* check cbufLen */
        if (plen > (sa->cbuf2BufSize - sa->cbuf2Len)) {
            plen = sa->cbuf2BufSize - sa->cbuf2Len;
            PICODBG_WARN(("phones skipped"));
            picoos_emRaiseWarning(this->common->em,
                                  PICO_WARN_INCOMPLETE, NULL, NULL);
            if (plen == 0) {
                sa->headx[ind].cind = 0;
            }
        }

        /* set item head, info1, info2 unchanged */
        sa->headx[ind].head.type = PICODATA_ITEM_WORDPHON;
        sa->headx[ind].head.len = plen;

        for (i = 0; i < plen; i++) {
            sa->cbuf2[sa->cbuf2Len] = phones[i];
            sa->cbuf2Len++;
        }

        PICODBG_DEBUG(("%c item, pos: %d, plen: %d",
                       PICODATA_ITEM_WORDPHON, pos, plen));

    } else {
        PICODBG_WARN(("lexIndLookup problem"));
        picoos_emRaiseWarning(this->common->em, PICO_WARN_PU_IRREG_ITEM,
                              NULL, NULL);
    }
    return PICO_OK;
}



/* ************** g2p ***************/


/* Name    :   saGetNvowel
   Function:   returns vowel info in a word or word seq
   Input   :   sInChar         the grapheme string to be converted in phoneme
               inLen           number of bytes in grapheme buffer
               inPos           start position of current grapheme (0..inLen-1)
   Output  :   nVow            number of vowels in the word
               nVord           vowel order in the word
   Returns :   TRUE: processing successful;  FALSE: errors
*/
static picoos_uint8 saGetNrVowel(register picodata_ProcessingUnit this,
                                 register sa_subobj_t *sa,
                                 const picoos_uint8 *sInChar,
                                 const picoos_uint16 inLen,
                                 const picoos_uint8 inPos,
                                 picoos_uint8 *nVow,
                                 picoos_uint8 *nVord) {
    picoos_uint32 nCount;
    picoos_uint32 pos;
    picoos_uint8 cstr[PICOBASE_UTF8_MAXLEN + 1];

    /*defaults*/
    *nVow = 0;
    *nVord = 0;
    /*1:check wether the current char is a vowel*/
    pos = inPos;
    if (!picobase_get_next_utf8char(sInChar, inLen, &pos, cstr) ||
        !picoktab_hasVowellikeProp(sa->tabgraphs, cstr, PICOBASE_UTF8_MAXLEN)) {
        return FALSE;
    }
    /*2:count number of vowels in current word and find vowel order*/
    for (nCount = 0; nCount < inLen; ) {
      if (!picobase_get_next_utf8char(sInChar, inLen, &nCount, cstr)) {
            return FALSE;
      }
        if (picoktab_hasVowellikeProp(sa->tabgraphs, cstr,
                                      PICOBASE_UTF8_MAXLEN)) {
            (*nVow)++;
            if (nCount == pos) {
                (*nVord) = (*nVow);
        }
        }
    }
    return TRUE;
}


/* do g2p for a full word, right-to-left */
static picoos_uint8 saDoG2P(register picodata_ProcessingUnit this,
                            register sa_subobj_t *sa,
                            const picoos_uint8 *graph,
                            const picoos_uint8 graphlen,
                            const picoos_uint8 pos,
                            picoos_uint8 *phones,
                            const picoos_uint16 phonesmaxlen,
                            picoos_uint16 *plen) {
    picoos_uint16 outNp1Ch; /*last 3 outputs produced*/
    picoos_uint16 outNp2Ch;
    picoos_uint16 outNp3Ch;
    picoos_uint8 nPrimary;
    picoos_uint8 nCount;
    picoos_uint32 utfpos;
    picoos_uint16 nOutVal;
    picoos_uint8 okay;
    picoos_uint16 phonesind;
    picoos_uint8 nrvow;
    picoos_uint8 ordvow;
    picokdt_classify_vecresult_t dtresv;
    picoos_uint16 i;

    *plen = 0;
    okay = TRUE;

    /* use sa->tmpbuf[PICOSA_MAXITEMSIZE] to temporarly store the
       phones which are predicted in reverse order. Once all are
       available put them in phones in usuable order. phonesind is
       used to fille item in reverse order starting at the end of
       tmpbuf. */
    phonesind = PICOSA_MAXITEMSIZE - 1;

    /* prepare the data for loop operations */
    outNp1Ch = PICOKDT_HISTORY_ZERO;
    outNp2Ch = PICOKDT_HISTORY_ZERO;
    outNp3Ch = PICOKDT_HISTORY_ZERO;

    /* inner loop */
    nPrimary = 0;

    /* ************************************************/
    /* go backward grapheme by grapheme, it's utf8... */
    /* ************************************************/

    /* set start nCount to position of start of last utfchar */
    /* ! watch out! somethimes starting at 1, sometimes at 0,
       ! sometimes counting per byte, sometimes per UTF8 char */
    /* nCount is (start position + 1) of utf8 char */
    utfpos = graphlen;
    if (picobase_get_prev_utf8charpos(graph, 0, &utfpos)) {
        nCount = utfpos + 1;
    } else {
        /* should not occurr */
        PICODBG_ERROR(("invalid utf8 string, graphlen: %d", graphlen));
        return FALSE;
    }

    while (nCount > 0) {
        PICODBG_TRACE(("right-to-left g2p, count: %d", nCount));
        okay = TRUE;

        if (!saGetNrVowel(this, sa, graph, graphlen, nCount-1, &nrvow,
                          &ordvow)) {
            nrvow = 0;
            ordvow = 0;
        }

        /* prepare input vector, set inside tree object invec,
         * g2pBuildVector will call the constructInVec tree method */
        if (!picokdt_dtG2PconstructInVec(sa->dtg2p,
                                         graph, /*grapheme start*/
                                         graphlen, /*grapheme length*/
                                         nCount-1, /*grapheme current position*/
                                         pos, /*Word POS*/
                                         nrvow, /*nr vowels if vowel, 0 else */
                                         ordvow, /*ord of vowel if vowel, 0 el*/
                                         &nPrimary,  /*primary stress flag*/
                                         outNp1Ch, /*Right phoneme context +1*/
                                         outNp2Ch, /*Right phoneme context +2*/
                                         outNp3Ch)) { /*Right phon context +3*/
            /*Errors in preparing the input vector : skip processing*/
            PICODBG_WARN(("problem with invec"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR,
                                  NULL, NULL);
            okay = FALSE;
        }

        /* classify using the invec in the tree object and save the direct
           tree output also in the tree object */
        if (okay && (!picokdt_dtG2Pclassify(sa->dtg2p, &nOutVal))) {
            /* error doing classification */
            PICODBG_WARN(("problem classifying"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_CLASSIFICATION,
                                  NULL, NULL);
            okay = FALSE;
        }

        /* decompose the invec in the tree object and return result in dtresv */
        if (okay && (!picokdt_dtG2PdecomposeOutClass(sa->dtg2p, &dtresv))) {
            /* error decomposing */
            PICODBG_WARN(("problem decomposing"));
            picoos_emRaiseWarning(this->common->em, PICO_WARN_OUTVECTOR,
                                  NULL, NULL);
            okay = FALSE;
        }

        if (okay) {
            if ((dtresv.nr == 0) || (dtresv.classvec[0] == PICOKDT_EPSILON)) {
                /* no phones to be added */
                PICODBG_TRACE(("epsilon, no phone added %c", graph[nCount-1]));
                ;
            } else {
                /* add decomposed output to tmpbuf, reverse order */
                for (i = dtresv.nr; ((((PICOSA_MAXITEMSIZE - 1) -
                                       phonesind)<phonesmaxlen) &&
                                     (i > 0)); ) {
                    i--;
                    PICODBG_TRACE(("%c %d",graph[nCount-1],dtresv.classvec[i]));
                    if (dtresv.classvec[i] > 255) {
                        PICODBG_WARN(("dt result outside valid range, "
                                      "skipping phone"));
                        continue;
                    }
                    sa->tmpbuf[phonesind--] = (picoos_uint8)dtresv.classvec[i];
                    if (!nPrimary) {
                        if (picoktab_isPrimstress(sa->tabphones,
                          (picoos_uint8)dtresv.classvec[i])) {
                            nPrimary = 1;
            }
                    }
                    (*plen)++;
                }
                if (i > 0) {
                    PICODBG_WARN(("phones skipped"));
                    picoos_emRaiseWarning(this->common->em,
                                          PICO_WARN_INCOMPLETE, NULL, NULL);
                }
            }
        }

        /*shift tree output history and update*/
        outNp3Ch = outNp2Ch;
        outNp2Ch = outNp1Ch;
        outNp1Ch = nOutVal;

        /* go backward one utf8 char */
        /* nCount is in +1 domain */
        if (nCount <= 1) {
            /* end of str */
            nCount = 0;
        } else {
            utfpos = nCount - 1;
            if (!picobase_get_prev_utf8charpos(graph, 0, &utfpos)) {
                /* should not occur */
                PICODBG_ERROR(("invalid utf8 string, utfpos: %d", utfpos));
                return FALSE;
            } else {
                nCount = utfpos + 1;
            }
        }
    }

    /* a must be: (PICOSA_MAXITEMSIZE-1) - phonesind == *plen */
    /* now that we have all phone IDs, copy in correct order to phones */
    /* phonesind point to next free slot in the reverse domainn,
       ie. inc first */
    phonesind++;
    for (i = 0; i < *plen; i++, phonesind++) {
        phones[i] = sa->tmpbuf[phonesind];
    }
    return TRUE;
}


/* item in headx[ind]/cbuf1, out: modified headx and cbuf2 */

static pico_status_t saGraphemeToPhoneme(register picodata_ProcessingUnit this,
                                         register sa_subobj_t *sa,
                                         picoos_uint16 ind) {
    picoos_uint16 plen;

    PICODBG_TRACE(("starting g2p"));

    if (saDoG2P(this, sa, &(sa->cbuf1[sa->headx[ind].cind]),
                sa->headx[ind].head.len, sa->headx[ind].head.info1,
                &(sa->cbuf2[sa->cbuf2Len]), (sa->cbuf2BufSize - sa->cbuf2Len),
                &plen)) {

        /* check of cbuf2Len done in saDoG2P, phones skipped if needed */
        if (plen > 255) {
            PICODBG_WARN(("maximum number of phones exceeded (%d), skipping",
                          plen));
            plen = 255;
        }

        /* set item head, info1, info2 unchanged */
        sa->headx[ind].head.type = PICODATA_ITEM_WORDPHON;
        sa->headx[ind].head.len = (picoos_uint8)plen;
        sa->headx[ind].cind = sa->cbuf2Len;
        sa->cbuf2Len += plen;
        PICODBG_DEBUG(("%c item, plen: %d",
                       PICODATA_ITEM_WORDPHON, plen));
    } else {
        PICODBG_WARN(("problem doing g2p"));
        picoos_emRaiseWarning(this->common->em, PICO_WARN_PU_IRREG_ITEM,
                              NULL, NULL);
    }
    return PICO_OK;
}


/* ***********************************************************************/
/*                          extract phonemes of an item into a phonBuf   */
/* ***********************************************************************/

static pico_status_t saAddPhoneme(register sa_subobj_t *sa, picoos_uint16 pos, picoos_uint16 sym) {
    /* picoos_uint8 plane, unshifted; */

    /* just for debuging */
    /*
    unshifted = picotrns_unplane(sym,&plane);
    PICODBG_DEBUG(("adding %i/%i (%c on plane %i) at phonBuf[%i]",pos,sym,unshifted,plane,sa->phonWritePos));
    */
    if (PICOTRNS_MAX_NUM_POSSYM <= sa->phonWritePos) {
        /* not an error! */
        PICODBG_DEBUG(("couldn't add because phon buffer full"));
        return PICO_EXC_BUF_OVERFLOW;
    } else {
        sa->phonBuf[sa->phonWritePos].pos = pos;
        sa->phonBuf[sa->phonWritePos].sym = sym;
        sa->phonWritePos++;
        return PICO_OK;
    }
}

/*
static pico_status_t saAddStartPhoneme(register sa_subobj_t *sa) {
    return saAddPhoneme(sa, PICOTRNS_POS_IGNORE,
            (PICOKFST_PLANE_INTERN << 8) + sa->fixedIds->phonStartId);
}


static pico_status_t saAddTermPhoneme(register sa_subobj_t *sa) {
    return saAddPhoneme(sa, PICOTRNS_POS_IGNORE,
            (PICOKFST_PLANE_INTERN << 8) + sa->fixedIds->phonTermId);
}

*/

static pico_status_t saExtractPhonemes(register picodata_ProcessingUnit this,
        register sa_subobj_t *sa, picoos_uint16 pos,
        picodata_itemhead_t* head, const picoos_uint8* content)
{
    pico_status_t rv= PICO_OK;
    picoos_uint8 i;
    picoos_int16 fstSymbol;
#if defined(PICO_DEBUG)
    picoos_char msgstr[SA_MSGSTR_SIZE];
#endif

    PICODBG_TRACE(("doing item %s",
                    picodata_head_to_string(head,msgstr,SA_MSGSTR_SIZE)));
    /*
     Items  considered in a transduction are WORDPHON item. its starting offset within the inBuf is given as
     'pos'.
     Elements that go into the transduction receive "their" position in the buffer.
     */
    sa->phonWritePos = 0;
    /* WORDPHON(POS,WACC)phon */
    rv = saAddPhoneme(sa, PICOTRNS_POS_IGNORE,
                (PICOKFST_PLANE_INTERN << 8) + sa->fixedIds->phonStartId);
    for (i = 0; i < head->len; i++) {
        fstSymbol = /* (PICOKFST_PLANE_PHONEMES << 8) + */content[i];
        /*  */
        PICODBG_TRACE(("adding phoneme %c",fstSymbol));
        rv = saAddPhoneme(sa, pos+PICODATA_ITEM_HEADSIZE+i, fstSymbol);
    }
    rv = saAddPhoneme(sa, PICOTRNS_POS_IGNORE,
                (PICOKFST_PLANE_INTERN << 8) + sa->fixedIds->phonTermId);
    sa->nextReadPos = pos + PICODATA_ITEM_HEADSIZE +  head->len;
    return rv;
}


#define SA_POSSYM_OK           0
#define SA_POSSYM_OUT_OF_RANGE 1
#define SA_POSSYM_END          2
#define SA_POSSYM_INVALID     -3
/* *readPos is the next position in phonBuf to be read, and *writePos is the first position not to be read (may be outside
 * buf).
 * 'rangeEnd' is the first possym position outside the desired range.
 * Possible return values:
 * SA_POSSYM_OK            : 'pos' and 'sym' are set to the read possym, *readPos is advanced
 * SA_POSSYM_OUT_OF_RANGE  : pos is out of range. 'pos' is set to that of the read possym, 'sym' is undefined
 * SA_POSSYM_UNDERFLOW     : no more data in buf. 'pos' is set to PICOTRNS_POS_INVALID,    'sym' is undefined
 * SA_POSSYM_INVALID       : "strange" pos.       'pos' is set to PICOTRNS_POS_INVALID,    'sym' is undefined
 */
static pico_status_t getNextPosSym(sa_subobj_t * sa, picoos_int16 * pos, picoos_int16 * sym,
        picoos_int16 rangeEnd) {
    /* skip POS_IGNORE */
    while ((sa->phonReadPos < sa->phonWritePos) && (PICOTRNS_POS_IGNORE == sa->phonBuf[sa->phonReadPos].pos))  {
        PICODBG_DEBUG(("ignoring phone at sa->phonBuf[%i] because it has pos==IGNORE",sa->phonReadPos));
        sa->phonReadPos++;
    }
    if ((sa->phonReadPos < sa->phonWritePos)) {
        *pos = sa->phonBuf[sa->phonReadPos].pos;
        if ((PICOTRNS_POS_INSERT == *pos) || ((0 <= *pos) && (*pos < rangeEnd))) {
            *sym = sa->phonBuf[sa->phonReadPos++].sym;
            return SA_POSSYM_OK;
        } else if (*pos < 0){ /* *pos is "strange" (e.g. POS_INVALID) */
            return SA_POSSYM_INVALID;
        } else {
            return SA_POSSYM_OUT_OF_RANGE;
        }
    } else {
        /* no more possyms to read */
        *pos = PICOTRNS_POS_INVALID;
        return SA_POSSYM_END;
    }
}




/* ***********************************************************************/
/*                          saStep function                              */
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

static picodata_step_result_t saStep(register picodata_ProcessingUnit this,
                                     picoos_int16 mode,
                                     picoos_uint16 *numBytesOutput) {
    register sa_subobj_t *sa;
    pico_status_t rv = PICO_OK;
    pico_status_t rvP = PICO_OK;
    picoos_uint16 blen = 0;
    picoos_uint16 clen = 0;
    picoos_uint16 i;
    picoklex_Lex lex;


    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    sa = (sa_subobj_t *) this->subObj;
    mode = mode;        /* avoid warning "var not used in this function"*/
    *numBytesOutput = 0;
    while (1) { /* exit via return */
        PICODBG_DEBUG(("doing state %i, hLen|c1Len|c2Len: %d|%d|%d",
                       sa->procState, sa->headxLen, sa->cbuf1Len,
                       sa->cbuf2Len));

        switch (sa->procState) {

            /* *********************************************************/
            /* collect state: get item(s) from charBuf and store in
             * internal buffers, need a complete punctuation-phrase
             */
            case SA_STEPSTATE_COLLECT:

                while (sa->inspaceok && sa->needsmoreitems
                       && (PICO_OK ==
                           (rv = picodata_cbGetItem(this->cbIn, sa->tmpbuf,
                                            PICOSA_MAXITEMSIZE, &blen)))) {
                    rvP = picodata_get_itemparts(sa->tmpbuf,
                                            PICOSA_MAXITEMSIZE,
                                            &(sa->headx[sa->headxLen].head),
                                            &(sa->cbuf1[sa->cbuf1Len]),
                                            sa->cbuf1BufSize-sa->cbuf1Len,
                                            &clen);
                    if (rvP != PICO_OK) {
                        PICODBG_ERROR(("problem getting item parts"));
                        picoos_emRaiseException(this->common->em, rvP,
                                                NULL, NULL);
                        return PICODATA_PU_ERROR;
                    }

                    /* if CMD(...FLUSH...) -> PUNC(...FLUSH...),
                       construct PUNC-FLUSH item in headx */
                    if ((sa->headx[sa->headxLen].head.type ==
                         PICODATA_ITEM_CMD) &&
                        (sa->headx[sa->headxLen].head.info1 ==
                         PICODATA_ITEMINFO1_CMD_FLUSH)) {
                        sa->headx[sa->headxLen].head.type =
                            PICODATA_ITEM_PUNC;
                        sa->headx[sa->headxLen].head.info1 =
                            PICODATA_ITEMINFO1_PUNC_FLUSH;
                        sa->headx[sa->headxLen].head.info2 =
                            PICODATA_ITEMINFO2_PUNC_SENT_T;
                        sa->headx[sa->headxLen].head.len = 0;
                    }

                    /* convert opening phoneme command to WORDPHON
                     * and assign user-POS XX to it (Bug 432) */
                    sa->headx[sa->headxLen].cind = sa->cbuf1Len;
                    /* maybe overwritten later */
                    if ((sa->headx[sa->headxLen].head.type ==
                        PICODATA_ITEM_CMD) &&
                       (sa->headx[sa->headxLen].head.info1 ==
                        PICODATA_ITEMINFO1_CMD_PHONEME)&&
                        (sa->headx[sa->headxLen].head.info2 ==
                         PICODATA_ITEMINFO2_CMD_START)) {
                        picoos_uint8 i;
                        picoos_uint8 wordsep = picoktab_getWordboundID(sa->tabphones);
                        PICODBG_INFO(("wordsep id is %i",wordsep));
                        sa->headx[sa->headxLen].head.type = PICODATA_ITEM_WORDPHON;
                        sa->headx[sa->headxLen].head.info1 = PICODATA_POS_XX;
                        sa->headx[sa->headxLen].head.info2 = PICODATA_ITEMINFO2_NA;
                        /* cut off additional words */
                        i = 0;
                        while ((i < sa->headx[sa->headxLen].head.len) && (wordsep != sa->cbuf1[sa->headx[sa->headxLen].cind+i])) {
                            PICODBG_INFO(("accepting phoneme %i",sa->cbuf1[sa->headx[sa->headxLen].cind+i]));

                            i++;
                        }
                        if (i < sa->headx[sa->headxLen].head.len) {
                            PICODBG_INFO(("cutting off superfluous phonetic words at %i",i));
                            sa->headx[sa->headxLen].head.len = i;
                        }
                    }

                    /* check/set needsmoreitems */
                    if (sa->headx[sa->headxLen].head.type ==
                        PICODATA_ITEM_PUNC) {
                        sa->needsmoreitems = FALSE;
                    }

                    /* check/set inspaceok, keep spare slot for forcing */
                    if ((sa->headxLen >= (PICOSA_MAXNR_HEADX - 2)) ||
                        ((sa->cbuf1BufSize - sa->cbuf1Len) <
                         PICOSA_MAXITEMSIZE)) {
                        sa->inspaceok = FALSE;
                    }

                    if (clen > 0) {
                        sa->headx[sa->headxLen].cind = sa->cbuf1Len;
                        sa->cbuf1Len += clen;
                    } else {
                        sa->headx[sa->headxLen].cind = 0;
                    }
                    sa->headxLen++;
                }

                if (!sa->needsmoreitems) {
                    /* 1, phrase buffered */
                    sa->procState = SA_STEPSTATE_PROCESS_POSD;
                    return PICODATA_PU_ATOMIC;
                } else if (!sa->inspaceok) {
                    /* 2, forced phrase end */
                    /* at least one slot is still free, use it to
                       force a trailing PUNC item */
                    sa->headx[sa->headxLen].head.type = PICODATA_ITEM_PUNC;
                    sa->headx[sa->headxLen].head.info1 =
                        PICODATA_ITEMINFO1_PUNC_PHRASEEND;
                    sa->headx[sa->headxLen].head.info2 =
                        PICODATA_ITEMINFO2_PUNC_PHRASE_FORCED;
                    sa->headx[sa->headxLen].head.len = 0;
                    sa->needsmoreitems = FALSE; /* not really needed for now */
                    sa->headxLen++;
                    PICODBG_WARN(("forcing phrase end, added PUNC_PHRASEEND"));
                    picoos_emRaiseWarning(this->common->em,
                                          PICO_WARN_FALLBACK, NULL,
                                          (picoos_char *)"forced phrase end");
                    sa->procState = SA_STEPSTATE_PROCESS_POSD;
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
            /* process posd state: process items in headx/cbuf1
             * and change in place
             */
            case SA_STEPSTATE_PROCESS_POSD:
                /* ensure there is an item in inBuf */
                if (sa->headxLen > 0) {
                    /* we have a phrase in headx, cbuf1 (can be
                       single PUNC item without POS), do pos disamb */
                    if (PICO_OK != saDisambPos(this, sa)) {
                        picoos_emRaiseException(this->common->em,
                                                PICO_ERR_OTHER, NULL, NULL);
                        return PICODATA_PU_ERROR;
                    }
                    sa->procState = SA_STEPSTATE_PROCESS_WPHO;

                } else if (sa->headxLen == 0) {    /* no items in inBuf */
                    PICODBG_WARN(("no items in inBuf"));
                    sa->procState = SA_STEPSTATE_COLLECT;
                    return PICODATA_PU_BUSY;
                }

#if defined (PICO_DEBUG)
                if (1) {
                    picoos_uint8 i, j, ittype;
                    for (i = 0; i < sa->headxLen; i++) {
                        ittype = sa->headx[i].head.type;
                        PICODBG_INFO_CTX();
                        PICODBG_INFO_MSG(("sa-d: ("));
                        PICODBG_INFO_MSG(("'%c',", ittype));
                        if ((32 <= sa->headx[i].head.info1) &&
                            (sa->headx[i].head.info1 < 127) &&
                            (ittype != PICODATA_ITEM_WORDGRAPH) &&
                            (ittype != PICODATA_ITEM_WORDINDEX)) {
                            PICODBG_INFO_MSG(("'%c',",sa->headx[i].head.info1));
                        } else {
                            PICODBG_INFO_MSG(("%3d,", sa->headx[i].head.info1));
                        }
                        if ((32 <= sa->headx[i].head.info2) &&
                            (sa->headx[i].head.info2 < 127)) {
                            PICODBG_INFO_MSG(("'%c',",sa->headx[i].head.info2));
                        } else {
                            PICODBG_INFO_MSG(("%3d,", sa->headx[i].head.info2));
                        }
                        PICODBG_INFO_MSG(("%3d)", sa->headx[i].head.len));

                        for (j = 0; j < sa->headx[i].head.len; j++) {
                            if ((ittype == PICODATA_ITEM_WORDGRAPH) ||
                                (ittype == PICODATA_ITEM_CMD)) {
                                PICODBG_INFO_MSG(("%c",
                                        sa->cbuf1[sa->headx[i].cind+j]));
                            } else {
                                PICODBG_INFO_MSG(("%4d",
                                        sa->cbuf1[sa->headx[i].cind+j]));
                            }
                        }
                        PICODBG_INFO_MSG(("\n"));
                    }
                }
#endif

                break;


            /* *********************************************************/
            /* process wpho state: process items in headx/cbuf1 and modify
             * headx in place and fill cbuf2
             */
            case SA_STEPSTATE_PROCESS_WPHO:
                /* ensure there is an item in inBuf */
                if (sa->headxLen > 0) {
                    /* we have a phrase in headx, cbuf1 (can be single
                       PUNC item), do lex lookup, g2p, or copy */

                    /* check if cbuf2 is empty as it should be */
                    if (sa->cbuf2Len > 0) {
                        /* enforce emptyness */
                        PICODBG_WARN(("forcing empty cbuf2, discarding buf"));
                        picoos_emRaiseWarning(this->common->em,
                                              PICO_WARN_PU_DISCARD_BUF,
                                              NULL, NULL);
                    }

                    /* cbuf2 overflow avoided in saGrapheme*, saLexInd*,
                       saCopyItem*, phones skipped if needed */
                    for (i = 0; i < sa->headxLen; i++) {
                        switch (sa->headx[i].head.type) {
                            case PICODATA_ITEM_WORDGRAPH:
                                if (PICO_OK != saGraphemeToPhoneme(this, sa,
                                                                   i)) {
                                    /* not possible, phones skipped if needed */
                                    picoos_emRaiseException(this->common->em,
                                                            PICO_ERR_OTHER,
                                                            NULL, NULL);
                                    return PICODATA_PU_ERROR;
                                }
                                break;
                            case PICODATA_ITEM_WORDINDEX:
                                if (0 == sa->headx[i].head.info2) {
                                  lex = sa->lex;
                                } else {
                                    lex = sa->ulex[sa->headx[i].head.info2-1];
                                }
                                if (PICO_OK != saLexIndLookup(this, sa, lex, i)) {
                                    /* not possible, phones skipped if needed */
                                    picoos_emRaiseException(this->common->em,
                                                            PICO_ERR_OTHER,
                                                            NULL, NULL);
                                    return PICODATA_PU_ERROR;
                                }
                                break;
                            default:
                                /* copy item unmodified, ie. headx untouched,
                                   content from cbuf1 to cbuf2 */
                                if (PICO_OK != saCopyItemContent1to2(this, sa,
                                                                     i)) {
                                    /* not possible, phones skipped if needed */
                                    picoos_emRaiseException(this->common->em,
                                                            PICO_ERR_OTHER,
                                                            NULL, NULL);
                                    return PICODATA_PU_ERROR;
                                }
                                break;
                        }
                    }
                    /* set cbuf1 to empty */
                    sa->cbuf1Len = 0;
                    sa->procState = SA_STEPSTATE_PROCESS_TRNS_PARSE;

                } else if (sa->headxLen == 0) {    /* no items in inBuf */
                    PICODBG_WARN(("no items in inBuf"));
                    sa->procState = SA_STEPSTATE_COLLECT;
                    return PICODATA_PU_BUSY;
                }

#if defined (PICO_DEBUG)
                if (1) {
                    picoos_uint8 i, j, ittype;
                    for (i = 0; i < sa->headxLen; i++) {
                        ittype = sa->headx[i].head.type;
                        PICODBG_INFO_CTX();
                        PICODBG_INFO_MSG(("sa-g: ("));
                        PICODBG_INFO_MSG(("'%c',", ittype));
                        if ((32 <= sa->headx[i].head.info1) &&
                            (sa->headx[i].head.info1 < 127) &&
                            (ittype != PICODATA_ITEM_WORDPHON)) {
                            PICODBG_INFO_MSG(("'%c',",sa->headx[i].head.info1));
                        } else {
                            PICODBG_INFO_MSG(("%3d,", sa->headx[i].head.info1));
                        }
                        if ((32 <= sa->headx[i].head.info2) &&
                            (sa->headx[i].head.info2 < 127)) {
                            PICODBG_INFO_MSG(("'%c',",sa->headx[i].head.info2));
                        } else {
                            PICODBG_INFO_MSG(("%3d,", sa->headx[i].head.info2));
                        }
                        PICODBG_INFO_MSG(("%3d)", sa->headx[i].head.len));

                        for (j = 0; j < sa->headx[i].head.len; j++) {
                            if ((ittype == PICODATA_ITEM_CMD)) {
                                PICODBG_INFO_MSG(("%c",
                                        sa->cbuf2[sa->headx[i].cind+j]));
                            } else {
                                PICODBG_INFO_MSG(("%4d",
                                        sa->cbuf2[sa->headx[i].cind+j]));
                            }
                        }
                        PICODBG_INFO_MSG(("\n"));
                    }
                }
#endif

                break;


                /* *********************************************************/
                /* transduction parse state: extract phonemes of item in internal outBuf */
           case SA_STEPSTATE_PROCESS_TRNS_PARSE:

                PICODBG_DEBUG(("transduce item (bot, remain): (%d, %d)",
                                sa->headxBottom, sa->headxLen));

                /* check for termination condition first */
                if (0 == sa->headxLen) {
                    /* reset headx, cbuf2 */
                    sa->headxBottom = 0;
                    sa->cbuf2Len = 0;
                    /* reset collect state support variables */
                    sa->inspaceok = TRUE;
                    sa->needsmoreitems = TRUE;

                    sa->procState = SA_STEPSTATE_COLLECT;
                    return PICODATA_PU_BUSY;
                }

                sa->procState = SA_STEPSTATE_FEED;
                /* copy item unmodified */
                rv = picodata_put_itemparts(
                        &(sa->headx[sa->headxBottom].head),
                        &(sa->cbuf2[sa->headx[sa->headxBottom].cind]),
                        sa->headx[sa->headxBottom].head.len, sa->tmpbuf,
                        PICOSA_MAXITEMSIZE, &blen);

                if (PICODATA_ITEM_WORDPHON == sa->headx[sa->headxBottom].head.type) {
                   PICODBG_DEBUG(("PARSE found WORDPHON"));
                   rv = saExtractPhonemes(this, sa, 0, &(sa->headx[sa->headxBottom].head),
                           &(sa->cbuf2[sa->headx[sa->headxBottom].cind]));
                   if (PICO_OK == rv) {
                       PICODBG_DEBUG(("PARSE successfully returned from phoneme extraction"));
                       sa->procState = SA_STEPSTATE_PROCESS_TRNS_FST;
                   } else {
                       PICODBG_WARN(("PARSE phone extraction returned exception %i, output WORDPHON untransduced",rv));
                   }
               } else {
                   PICODBG_DEBUG(("PARSE found other item, just copying"));
               }
               if (SA_STEPSTATE_FEED == sa->procState) {
                    PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                            (picoos_uint8 *)"sa-p: ",
                            sa->tmpbuf, PICOSA_MAXITEMSIZE);

                }

                /* consume item */
                sa->headxBottom++;
                sa->headxLen--;

                break;

                /* *********************************************************/
                /* transduce state: copy item in internal outBuf to tmpBuf and transduce */
           case SA_STEPSTATE_PROCESS_TRNS_FST:





               /* if no word-level FSTs: doing trivial syllabification instead */
               if (0 == sa->numFsts) {
                   PICODBG_DEBUG(("doing trivial sylabification with %i phones", sa->phonWritePos));
#if defined(PICO_DEBUG)
                   {
                       PICODBG_INFO_CTX();
                       PICODBG_INFO_MSG(("sa trying to trivially syllabify: "));
                       PICOTRNS_PRINTSYMSEQ(this->voice->kbArray[PICOKNOW_KBID_DBG], sa->phonBuf, sa->phonWritePos);
                       PICODBG_INFO_MSG(("\n"));
                   }
#endif

                   picotrns_trivial_syllabify(sa->tabphones, sa->phonBuf,
                           sa->phonWritePos, sa->phonBufOut,
                           &sa->phonWritePos,PICOTRNS_MAX_NUM_POSSYM);
                   PICODBG_DEBUG(("returned from trivial sylabification with %i phones", sa->phonWritePos));
#if defined(PICO_DEBUG)
                   {
                       PICODBG_INFO_CTX();
                       PICODBG_INFO_MSG(("sa returned from syllabification: "));
                       PICOTRNS_PRINTSYMSEQ(this->voice->kbArray[PICOKNOW_KBID_DBG], sa->phonBufOut, sa->phonWritePos);
                       PICODBG_INFO_MSG(("\n"));
                   }
#endif

                   /* eliminate deep epsilons */
                   PICODBG_DEBUG(("doing epsilon elimination with %i phones", sa->phonWritePos));
                   picotrns_eliminate_epsilons(sa->phonBufOut,
                           sa->phonWritePos, sa->phonBuf,
                           &sa->phonWritePos,PICOTRNS_MAX_NUM_POSSYM);
                   PICODBG_DEBUG(("returning from epsilon elimination with %i phones", sa->phonWritePos));
                   sa->phonReadPos = 0;
                   sa->phonesTransduced = 1;
                   sa->procState = SA_STEPSTATE_FEED;
                   break;
               }

               /* there are word-level FSTs */
               /* termination condition first */
               if (sa->curFst >= sa->numFsts) {
                   /* reset for next transduction */
                   sa->curFst = 0;
                   sa->phonReadPos = 0;
                   sa->phonesTransduced = 1;
                   sa->procState = SA_STEPSTATE_FEED;
                   break;
               }

               /* transduce from phonBufIn to PhonBufOut */
               {

                   picoos_uint32 nrSteps;
#if defined(PICO_DEBUG)
                   {
                       PICODBG_INFO_CTX();
                       PICODBG_INFO_MSG(("sa trying to transduce: "));
                       PICOTRNS_PRINTSYMSEQ(this->voice->kbArray[PICOKNOW_KBID_DBG], sa->phonBuf, sa->phonWritePos);
                       PICODBG_INFO_MSG(("\n"));
                   }
#endif
                   picotrns_transduce(sa->fst[sa->curFst], FALSE,
                           picotrns_printSolution, sa->phonBuf, sa->phonWritePos, sa->phonBufOut,
                           &sa->phonWritePos,
                           PICOTRNS_MAX_NUM_POSSYM, sa->altDescBuf,
                           sa->maxAltDescLen, &nrSteps);
#if defined(PICO_DEBUG)
                   {
                       PICODBG_INFO_CTX();
                       PICODBG_INFO_MSG(("sa returned from transduction: "));
                       PICOTRNS_PRINTSYMSEQ(this->voice->kbArray[PICOKNOW_KBID_DBG], sa->phonBufOut, sa->phonWritePos);
                       PICODBG_INFO_MSG(("\n"));
                   }
#endif
               }



               /*
                The trasduction output will contain equivalent items i.e. (x,y')  for each (x,y) plus inserted deep symbols (-1,d).
                In case of deletions, (x,0) might also be omitted...
                */
               /* eliminate deep epsilons */
               picotrns_eliminate_epsilons(sa->phonBufOut,
                       sa->phonWritePos, sa->phonBuf, &sa->phonWritePos,PICOTRNS_MAX_NUM_POSSYM);
               sa->phonesTransduced = 1;

               sa->curFst++;

               return PICODATA_PU_ATOMIC;
               /* break; */

                /* *********************************************************/
                /* feed state: copy item in internal outBuf to output charBuf */

           case SA_STEPSTATE_FEED:

               PICODBG_DEBUG(("FEED"));

               if (sa->phonesTransduced) {
                   /* replace original phones by transduced */
                   picoos_uint16 phonWritePos = PICODATA_ITEM_HEADSIZE;
                   picoos_uint8 plane;
                   picoos_int16 sym, pos;
                   while (SA_POSSYM_OK == (rv = getNextPosSym(sa,&pos,&sym,sa->nextReadPos))) {
                       PICODBG_TRACE(("FEED inserting phoneme %c into inBuf[%i]",sym,phonWritePos));
                       sym = picotrns_unplane(sym, &plane);
                       PICODBG_ASSERT((PICOKFST_PLANE_PHONEMES == plane));
                       sa->tmpbuf[phonWritePos++] = (picoos_uint8) sym;
                   }
                   PICODBG_DEBUG(("FEED setting item length to %i",phonWritePos - PICODATA_ITEM_HEADSIZE));
                   picodata_set_itemlen(sa->tmpbuf,PICODATA_ITEM_HEADSIZE,phonWritePos - PICODATA_ITEM_HEADSIZE);
                   if (SA_POSSYM_INVALID == rv) {
                       PICODBG_ERROR(("FEED unexpected symbol or unexpected end of phoneme list"));
                       return (picodata_step_result_t)picoos_emRaiseException(this->common->em, PICO_WARN_INCOMPLETE, NULL, NULL);
                   }
                   sa->phonesTransduced = 0;

               } /* if (sa->phonesTransduced) */


                rvP = picodata_cbPutItem(this->cbOut, sa->tmpbuf,
                PICOSA_MAXITEMSIZE, &clen);

                *numBytesOutput += clen;

                PICODBG_DEBUG(("put item, status: %d", rvP));

                if (rvP == PICO_OK) {
                } else if (rvP == PICO_EXC_BUF_OVERFLOW) {
                    /* try again next time */
                    PICODBG_DEBUG(("feeding overflow"));
                    return PICODATA_PU_OUT_FULL;
                } else {
                    /* error, should never happen */
                    PICODBG_ERROR(("untreated return value, rvP: %d", rvP));
                    return PICODATA_PU_ERROR;
                }

                PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                        (picoos_uint8 *)"sana: ",
                        sa->tmpbuf, PICOSA_MAXITEMSIZE);

                sa->procState = SA_STEPSTATE_PROCESS_TRNS_PARSE;
                /* return PICODATA_PU_BUSY; */
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
