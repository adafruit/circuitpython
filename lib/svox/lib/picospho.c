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
 * @file picospho.c
 *
 * sentence phonemic/phonetic FSTs PU
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
#include "picokfst.h"
#include "picoktab.h"
#include "picotrns.h"

#include "picospho.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#define SPHO_BUFSIZE (3 * PICODATA_BUFSIZE_DEFAULT)



#define SPHO_MAX_ALTDESC_SIZE (60 * PICOTRNS_MAX_NUM_POSSYM)


#define SPHO_SMALLEST_SIL_DUR 1


/** @addtogroup picospho
 *
 * Algorithmic description
 * =======================
 * The main function, sphoStep, is divided into the subprocesses (processing states) described further down.
 *
 * Flow control:
 * ------------
 * The processing flow is controlled by setting
 *                       - 'procState' :       the next state to be processed
 *                       - 'feedFollowState' : the state to be processed after the feed state (the feed state is treated like a primitive "subroutine")
 *                       - some other flags
 *
 * Buffering:
 * ---------
 * - The input items are mainly stored and processed in two buffers, collectively called 'inBuf'
 *                       - cbuf  : unstructured buffer containing item contents
 *                       - headx : structured buffer containing item heads, each expanded by a pointer to the item contents
 *                                 and space for a boundary potentially to be inserted (to the left of the original item)
 * - For transduction, phonemes and their position are extracted from inBuf into
 *                       - phonBuf,
 *   processed there, and the resulting phonemes realigned with inBuf.
 * - Word items are split into syllables, stored in
 *                       - sylBuf
 * - Items to be output are stored in outBuf
 *
 * Windowing:
 * ---------
 *   Optimal solutions are achieved if a whole sentence is processed at once. However, if any of the buffers are too small,
 *   only sentence parts are processed. To improve the quality of such sub-optimal solutions, a moving-window-with-overlap is applied:
 *   - [0,headxReadPos[              : the window considered for transduction
 *   - [activeStartPos,activeEndPos[ : the "active" subrange of the window actually used for output
 *   - penultima                     : the position (within the active range) that should be used as new window start when shifting the window
 *
 * After PROCESS_PARSE:
 *   0             activeStartPos      penultima    activeEndPos   headxReadPos              headxWritePos
 *  |             |                   |            |              |                         |
 *  |-------------=================================---------------|                                         ['----': context '====' : active subrange)
 *
 * After PROCESS_SHIFT:
 *                                     0            activeStartPos                           headWritePos
 *                  |                 |            |                                        |
 *                                    |------------... (only left context is known; new active range,  penultima, and right context to be established at next parse)
 *
 * Processing states:
 * -----------------
 * - INIT              : initialize state variables
 * - COLLECT           : collect items into internal buffers ("inBuf")
 * - PROCESS_PARSE     : go through inBuf items and extract position/phoneme pairs into phoneme buffer 'phonBuf'
 *                       word boundary phonemes are inserted between words
 * - PROCESS_TRANSDUCE : transduce phonBuf
 * - PROCESS_BOUNDS    : go through inBuf items again and match against transduced pos/phoneme
 *                       this is the first round of alignment, only inserting/deleting/modifying bounds, according to
 *                       - existing BOUND items
 *                       - newly produced word bounds separating WORDPHON items
 *                       - bound upgrades/downgrades from transduction
 *                       - bound upgrades/downgrades/insertions from SIL command items (originating e.g. from <break> text commands)
 *                       all relevant bounds are placed in the corresponding headx extention; original bound items become invalid.
 * - PROCESS_RECOMB    : go through inBuf items again and match against transduced pos/phoneme
 *                       this is the second round of alignment, treating non-BOUND items
 *                       - WORDPHONs are broken into syllables by "calling" PROCESS_SYL
 *                       - "side-bounds" (in the headx extension) are output by "calling" FEED
 *                       - BOUND items are consumed with no effect
 *                       - other items are output unchanged "calling" FEED
 * - PROCESS_SYL       : the WORDPHON coming from RECOMB is matched against the phonBuf and (new) SYLLPHON items
 *                       are created. (the original wordphon is consumed)
 * - FEED              : feeds one item and returns to spho->feedFollowState
 * - SHIFT             : items in inBuf are shifted left to make room for new items. If a sentence doesn't fit
 *                       inBuf in its entirety, left and/or right contexts are kept so they can be considered in
 *                       the next transduction.
 */



/* PU sphoStep states */
#define SPHO_STEPSTATE_INIT               0
#define SPHO_STEPSTATE_COLLECT            1
#define SPHO_STEPSTATE_PROCESS_PARSE      2
#define SPHO_STEPSTATE_PROCESS_TRANSDUCE  3
#define SPHO_STEPSTATE_PROCESS_BOUNDS     4
#define SPHO_STEPSTATE_PROCESS_RECOMB     5
#define SPHO_STEPSTATE_PROCESS_SYL        6
#define SPHO_STEPSTATE_FEED               7
#define SPHO_STEPSTATE_SHIFT              8

#define SPHO_POS_INVALID (PICOTRNS_POS_INVALID)   /* indicates that no position was set yet */

/* nr item restriction: maximum number of extended item heads in headx */
#define SPHO_MAXNR_HEADX    60

/* nr item restriction: maximum size of all item contents together in cont */
#define SPHO_MAXSIZE_CBUF (30 * 255)

/* "expanded head": item head expanded by a content position and a by boundary information
 *  potentially inserted "to the left" of the item */
typedef struct {
    picodata_itemhead_t head;
    picoos_uint16 cind;
    picoos_uint8 boundstrength; /* bstrength to the left, 0 if not set */
    picoos_uint8 phrasetype; /* btype for following phrase, 0 if not set */
    picoos_int16 sildur; /* silence duration for boundary, -1 if not set */
} picospho_headx_t;



#define SPHO_MSGSTR_SIZE 32

/** object       : SentPhoUnit
 *  shortcut     : spho
 *  derived from : picodata_ProcessingUnit
 */
typedef struct spho_subobj {
    picoos_Common common;

    /* we use int16 for buffer positions so we can indicate exceptional positions (invalid etc.) with negative
     * integers */
    picoos_uint8 procState; /* for next processing step decision */

    /* buffer for item headers */
    picoos_uint8 tmpbuf[PICODATA_MAX_ITEMSIZE]; /* tmp. location for an item */

    picospho_headx_t headx[SPHO_MAXNR_HEADX]; /* "expanded head" buffer */
    picoos_uint16 headxBufSize; /* actually allocated size (if one day headxBuf is allocated dynamically) */
    picoos_uint16 headxReadPos, headxWritePos;

    picoos_uint8 cbuf[SPHO_MAXSIZE_CBUF];
    picoos_uint16 cbufBufSize; /* actually allocated size */
    picoos_uint16 cbufWritePos; /* next position to write to, 0 if buffer empty */

    picoos_uint8 outBuf[PICODATA_BUFSIZE_DEFAULT]; /* internal output buffer to hold just one item */
    picoos_uint16 outBufSize; /* actually allocated size (if one day outBuf is allocated dynamically) */
    picoos_uint16 outReadPos; /* next pos to read from inBuf for output */

    /* picoos_int16 outWritePos; */ /* next pos to output from in buf */

    picoos_uint8 sylBuf[255]; /* internal buffer to hold contents of syl item to be output */
    picoos_uint8 sylReadPos, sylWritePos; /* next pos to read from sylBuf, next pos to write to sylBuf */

    /* buffer for internal calculation of transducer */
    picotrns_AltDesc altDescBuf;
    /* the number of AltDesc in the buffer */
    picoos_uint16 maxAltDescLen;

    /* the input to a transducer should not be larger than PICOTRNS_MAX_NUM_POSSYM
     * so the output may expand (up to 4*PICOTRNS_MAX_NUM_POSSYM) */

    picotrns_possym_t phonBufA[4 * PICOTRNS_MAX_NUM_POSSYM + 1];
    picotrns_possym_t phonBufB[4 * PICOTRNS_MAX_NUM_POSSYM + 1];
    picotrns_possym_t * phonBuf;
    picotrns_possym_t * phonBufOut;
    picoos_uint16 phonReadPos, phonWritePos; /* next pos to read from phonBufIn, next pos to write to phonBufIn */

    picoos_int16 activeStartPos; /* start position of items to be treated (at end of left context) */
    picoos_int16 penultima, activeEndPos; /* positions of last two bounds/words; SPHO_POS_INVALID means uninitialized */
    picoos_int16 lastPhraseBoundPos; /* position of the last bound encountered (<0 if inexistent or not reachable */
    picoos_uint8 lastPhraseType; /* phrase type of the last phrase boundary, 0 if not set */

    picoos_uint8 needMoreInput, /* more data necessary to decide on token */
    suppressParseWordBound, /* dont produce word boundary */
    suppressRecombWordBound, /* dont produce word boundary */
    breakPending, /* received a break but didn't interpret it yet */
    /* sentEnd, */ /* sentence end detected */
    force, /* in forced state */
    wordStarted, /* is it the first syl in the word: expect POS */
    sentenceStarted;

    picoos_uint16 breakTime; /* time argument of the pending break command */

    picoos_uint8 feedFollowState; /* where to return after feed */

    /* fst knowledge bases */
    picoos_uint8 numFsts;
    picokfst_FST fst[PICOKNOW_MAX_NUM_SPHO_FSTS];
    picoos_uint8 curFst; /* the fst to be applied next */

    /* fixed ids knowledge base */
    picoktab_FixedIds fixedIds;

    /* phones kb */
    picoktab_Phones phones;

    /* some soecial ids from phones */
    picoos_uint8 primStressId, secondStressId, syllSepId;

} spho_subobj_t;


static pico_status_t sphoReset(register picodata_ProcessingUnit this)
{

    spho_subobj_t * spho;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(this->common->em,
                                       PICO_ERR_NULLPTR_ACCESS, NULL, NULL);
    }
    spho = (spho_subobj_t *) this->subObj;

    spho->curFst = 0;

/* processing state */
    spho->procState = SPHO_STEPSTATE_INIT;
    spho->needMoreInput = TRUE;
    spho->suppressParseWordBound = FALSE;
    spho->suppressRecombWordBound = FALSE;
    spho->breakPending = FALSE;
    spho->force = 0;
    spho->sentenceStarted = 0;


    /* item buffer headx/cbuf */
    spho->headxBufSize = SPHO_MAXNR_HEADX;
    spho->headxReadPos = 0;
    spho->headxWritePos = 0;

    spho->cbufWritePos = 0;
    spho->cbufBufSize = SPHO_MAXSIZE_CBUF;

    /* possym buffer */
    spho->phonBuf = spho->phonBufA;
    spho->phonBufOut = spho->phonBufB;
    spho->phonReadPos = 0;

    /* overlapping */
    spho->activeStartPos = 0;
    spho->penultima = SPHO_POS_INVALID;
    spho->activeEndPos = SPHO_POS_INVALID;

    return PICO_OK;
}


static pico_status_t sphoInitialize(register picodata_ProcessingUnit this, picoos_int32 resetMode)
{
    picoos_uint8 i;
    spho_subobj_t * spho;
    picokfst_FST fst;

    picoknow_kb_id_t myKbIds[PICOKNOW_MAX_NUM_SPHO_FSTS] = PICOKNOW_KBID_SPHO_ARRAY;

    PICODBG_DEBUG(("init"));

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(this->common->em,
                                       PICO_ERR_NULLPTR_ACCESS, NULL, NULL);
    }

    spho = (spho_subobj_t *) this->subObj;

    spho->numFsts = 0;

    spho->curFst = 0;

    for (i = 0; i<PICOKNOW_MAX_NUM_SPHO_FSTS; i++) {
        fst = picokfst_getFST(this->voice->kbArray[myKbIds[i]]);
        if (NULL != fst) {
            spho->fst[spho->numFsts++] = fst;
        }
    }
    spho->fixedIds = picoktab_getFixedIds(this->voice->kbArray[PICOKNOW_KBID_FIXED_IDS]);
    spho->phones = picoktab_getPhones(this->voice->kbArray[PICOKNOW_KBID_TAB_PHONES]);

    spho->syllSepId = picoktab_getSyllboundID(spho->phones);
    spho->primStressId = picoktab_getPrimstressID(spho->phones);
    spho->secondStressId = picoktab_getSecstressID(spho->phones);

    PICODBG_DEBUG(("got %i fsts", spho->numFsts));


    return sphoReset(this);

}

static picodata_step_result_t sphoStep(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 *numBytesOutput);




static pico_status_t sphoTerminate(register picodata_ProcessingUnit this)
{
    return PICO_OK;
}


static pico_status_t sphoSubObjDeallocate(register picodata_ProcessingUnit this,
        picoos_MemoryManager mm)
{
    spho_subobj_t * spho;

    spho = (spho_subobj_t *) this->subObj;

    if (NULL != this) {
        if (NULL != this->subObj) {
            spho = (spho_subobj_t *) (this->subObj);
            picotrns_deallocate_alt_desc_buf(spho->common->mm,&spho->altDescBuf);
            picoos_deallocate(mm, (void *) &this->subObj);
        }
    }
    return PICO_OK;
}

picodata_ProcessingUnit picospho_newSentPhoUnit(picoos_MemoryManager mm,
        picoos_Common common, picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut, picorsrc_Voice voice)
{
    spho_subobj_t * spho;

    picodata_ProcessingUnit this = picodata_newProcessingUnit(mm, common, cbIn, cbOut, voice);
    if (this == NULL) {
        return NULL;
    }

    this->initialize = sphoInitialize;
    this->step = sphoStep;
    this->terminate = sphoTerminate;
    this->subDeallocate = sphoSubObjDeallocate;

    this->subObj = picoos_allocate(mm, sizeof(spho_subobj_t));
    if (this->subObj == NULL) {
        picoos_deallocate(mm, (void **)(void*)&this);
        return NULL;
    }
    spho = (spho_subobj_t *) this->subObj;

    spho->common = this->common;

    /* these are given by the pre-allocated array sizes */
    spho->outBufSize = PICODATA_BUFSIZE_DEFAULT;


    spho->altDescBuf = picotrns_allocate_alt_desc_buf(spho->common->mm, SPHO_MAX_ALTDESC_SIZE, &spho->maxAltDescLen);
    if (NULL == spho->altDescBuf) {
        picotrns_deallocate_alt_desc_buf(spho->common->mm,&spho->altDescBuf);
        picoos_emRaiseException(spho->common->em,PICO_EXC_OUT_OF_MEM, NULL,NULL);
        return NULL;
    }

    sphoInitialize(this, PICO_RESET_FULL);
    return this;
}


/* ***********************************************************************/
/*                          process buffered item list                   */
/* ***********************************************************************/


/* shift relevant data in headx/'cbuf' (between 'readPos' incl and writePos non-incl) to 'start'.
 * modify read/writePos accordingly */
static picoos_int16 shift_range_left_1(spho_subobj_t *spho, picoos_int16 * from, picoos_int16 to)
{

    /* remember shift parameters for cbuf */
    picoos_uint16
        c_i,
        c_j,
        c_diff,
        c_writePos,
        i,
        j,
        diff,
        writePos;
    i = to;
    j = *from;
    diff = j-i;
    writePos = spho->headxWritePos;
    c_i = spho->headx[to].cind;
    if (j < writePos) {
      c_j = spho->headx[j].cind;
    } else {
        c_j = spho->cbufWritePos;
    }
    c_diff = c_j - c_i;
    c_writePos = spho->cbufWritePos;

    PICODBG_DEBUG((
                    "shifting buffer region [%i,%i[ down to %i",*from, writePos, to
                    ));


    /* PICODBG_ASSERT((i<j)); */
    if (i > j) {
        return -1;
    }
    /* shift cbuf */
    while (c_j < c_writePos) {
        spho->cbuf[c_i++] = spho->cbuf[c_j++];
    }
    /* shift headx */
    while (j < writePos) {
        spho->headx[j].cind -= c_diff;
        spho->headx[i++] = spho->headx[j++];
    }
    spho->headxWritePos -= diff;
    *from = to;
    spho->cbufWritePos -= c_diff;
    /*  */
    PICODBG_DEBUG((
                    "readPos,WritePos are now [%i,%i[, returning shift amount %i",*from, spho->headxWritePos, diff
            ));
    return diff;
}

static pico_status_t sphoAddPhoneme(register spho_subobj_t *spho, picoos_int16 pos, picoos_int16 sym) {
    picoos_uint8 plane, unshifted;
    /* just for debuging */
    unshifted = picotrns_unplane(sym,&plane);
    PICODBG_TRACE(("adding %i/%i (%c on plane %i) at phonBuf[%i]",pos,sym,unshifted,plane,spho->phonWritePos));
    if (2* PICOTRNS_MAX_NUM_POSSYM <= spho->phonWritePos) {
        /* not an error! */
        PICODBG_DEBUG(("couldn't add because phon buffer full"));
        return PICO_EXC_BUF_OVERFLOW;
    } else {
        spho->phonBuf[spho->phonWritePos].pos = pos;
        spho->phonBuf[spho->phonWritePos].sym = sym;
        spho->phonWritePos++;
        return PICO_OK;
    }
}

static pico_status_t sphoAddStartPhoneme(register spho_subobj_t *spho) {
    return sphoAddPhoneme(spho, PICOTRNS_POS_IGNORE,
            (PICOKFST_PLANE_INTERN << 8) + spho->fixedIds->phonStartId);
}

static pico_status_t sphoAddTermPhonemes(register spho_subobj_t *spho, picoos_uint16 pos) {
    return sphoAddPhoneme(spho, pos,
            (PICOKFST_PLANE_PB_STRENGTHS << 8) + PICODATA_ITEMINFO1_BOUND_SEND)
            && sphoAddPhoneme(spho, PICOTRNS_POS_IGNORE,
                    (PICOKFST_PLANE_INTERN << 8) + spho->fixedIds->phonTermId);
}

/* return "syllable accent" (or prominence) symbol, given "word accent" symbol 'wacc' and stress value (no=0, primary=1, secondary=2) */
static picoos_uint16 sphoGetSylAccent(register spho_subobj_t *spho,
        picoos_uint8 wacc, picoos_uint8 sylStress)
{
    PICODBG_ASSERT(sylStress <= 2);

    spho = spho;        /* avoid warning "var not used in this function"*/

    switch (sylStress) {
        case 0: /* non-stressed syllable gets no prominence */
            /* return spho->fixedIds->accId[0]; */
            return PICODATA_ACC0;
            break;
        case 1: /* primary-stressed syllable gets word prominence */
            return wacc;
            break;
        case 2: /* secondary-stressed syllable gets no prominence or secondary stress prom. (4) */
            return (PICODATA_ACC0 == wacc) ? PICODATA_ACC0
                     : PICODATA_ACC4;
            /*return (spho->fixedIds->accId[0] == wacc) ? spho->fixedIds->accId[0]
                     : spho->fixedIds->accId[4]; */
             break;
        default:
            /* never occurs :-) */
            return PICODATA_ACC0;
            break;
    }
}


/* ***********************************************************************/
/*                          extract phonemes of an item into a phonBuf   */
/* ***********************************************************************/
static pico_status_t sphoExtractPhonemes(register picodata_ProcessingUnit this,
        register spho_subobj_t *spho, picoos_uint16 pos,
        picoos_uint8 convertAccents, picoos_uint8 * suppressWB)
{
    pico_status_t rv = PICO_OK;
    picoos_uint16 i, j;
    picoos_int16 fstSymbol;
    picoos_uint8 curStress;
    picotrns_possym_t tmpPosSym;
    picoos_uint16 oldPos, curPos;
    picodata_itemhead_t * head;
    picoos_uint8* content;

#if defined(PICO_DEBUG)
    picoos_char msgstr[SPHO_MSGSTR_SIZE];
#endif


    /*
     Items considered in a transduction are a BOUND or a WORDPHON item. its starting offset within the
     headxBuf is given as 'pos'.
     Elements that go into the transduction receive "their" position in the buffer.
     */

    oldPos = spho->phonWritePos;

    head = &(spho->headx[pos].head);
    content = spho->cbuf + spho->headx[pos].cind;

    PICODBG_TRACE(("doing item %s\n",
            picodata_head_to_string(head,msgstr,SPHO_MSGSTR_SIZE)));

    switch (head->type) {
        case PICODATA_ITEM_BOUND:
            /* map SBEG, SEND and TERM (as sentence closing) to SEND */
            fstSymbol = (PICODATA_ITEMINFO1_BOUND_SBEG == head->info1 || PICODATA_ITEMINFO1_BOUND_TERM == head->info1) ? PICODATA_ITEMINFO1_BOUND_SEND : head->info1;
            PICODBG_TRACE(("found bound of type %c\n",head->info1));
           /* BOUND(<bound strength><phrase type>) */
            /* insert bound strength */
            PICODBG_TRACE(("inserting phrase bound phoneme %c and setting suppresWB=1\n",fstSymbol));
            fstSymbol += (PICOKFST_PLANE_PB_STRENGTHS << 8);
            rv = sphoAddPhoneme(spho,pos,fstSymbol);
            /* phrase type not used */
            /* suppress next word boundary */
            (*suppressWB) = 1;
            break;

        case PICODATA_ITEM_WORDPHON:
            /* WORDPHON(POS,WACC)phon */
            PICODBG_TRACE(("found WORDPHON"));
            /* insert word boundary if not suppressed */
            if (!(*suppressWB)) {
                fstSymbol = (PICOKFST_PLANE_PB_STRENGTHS << 8) + PICODATA_ITEMINFO1_BOUND_PHR0;
                PICODBG_TRACE(("adding word boundary phone"));
                rv = sphoAddPhoneme(spho,pos,fstSymbol);
            }
            (*suppressWB) = 0;
            /* for the time being, we force to use POS so we can transduce all fsts in a row without reconsulting the items */


            /* If 'convertAccents' then the accentuation is not directly encoded. It rather influences the mapping of
             * the word accent symbol to the actual accent phoneme which is put after the syllable separator. */
            if (convertAccents) {
                PICODBG_TRACE(("converting accents"));
                /* extracting phonemes IN REVERSE order replacing syllable symbols with prominence symbols */
                curPos = spho->phonWritePos;
                curStress = 0; /* no stress */
                for (i = head->len; i > 0 ;) {
                    i--;
                    if (spho->primStressId == content[i]) {
                        curStress = 1;
                        PICODBG_DEBUG(("skipping primary stress at pos %i (in 1 .. %i)",i, head->len));
                        continue; /* skip primary stress symbol */
                    } else if (spho->secondStressId == content[i]) {
                        curStress = 2;
                        PICODBG_DEBUG(("skipping secondary stress at pos %i (in 1 .. %i)",i, head->len));
                        continue; /* skip secundary stress symbol */
                    } else if (spho->syllSepId == content[i]) {
                        fstSymbol = (PICOKFST_PLANE_POS << 8) + head->info1;
                        rv = sphoAddPhoneme(spho, pos, fstSymbol);
                        /* replace syllSepId by combination of syllable stress and word prominence */
                        fstSymbol = sphoGetSylAccent(spho,head->info2,curStress);
                        curStress = 0;
                        /* add accent */
                        fstSymbol += (PICOKFST_PLANE_ACCENTS << 8);
                        rv = sphoAddPhoneme(spho,pos,fstSymbol);
                        if (PICO_OK != rv) {
                            break;
                        }
                       /* and keep syllable boundary */
                        fstSymbol = (PICOKFST_PLANE_PHONEMES << 8) + content[i];
                    } else {
                        /* normal phoneme */
                        fstSymbol = (PICOKFST_PLANE_PHONEMES << 8) + content[i];
                    }
                    if (PICO_OK == rv) {
                        rv = sphoAddPhoneme(spho,pos,fstSymbol);
                    }
                }
                if (PICO_OK == rv) {
                    /* bug 366: we position the "head" into the item header and not on the first phoneme
                     * because there might be no phonemes at all */
                    /* insert head of the first syllable of a word */
                         fstSymbol = (PICOKFST_PLANE_POS << 8) + head->info1;
                        rv = sphoAddPhoneme(spho,pos,fstSymbol);
                    fstSymbol = sphoGetSylAccent(spho,head->info2,curStress);
                    curStress = 0;
                   fstSymbol += (PICOKFST_PLANE_ACCENTS << 8);
                   rv = sphoAddPhoneme(spho,pos,fstSymbol);
                }
                if (PICO_OK == rv) {
                    /* invert sympos portion */
                    i = curPos;
                    j=spho->phonWritePos-1;
                    while (i < j) {
                        tmpPosSym.pos = spho->phonBuf[i].pos;
                        tmpPosSym.sym = spho->phonBuf[i].sym;
                        spho->phonBuf[i].pos = spho->phonBuf[j].pos;
                        spho->phonBuf[i].sym = spho->phonBuf[j].sym;
                        spho->phonBuf[j].pos = tmpPosSym.pos;
                        spho->phonBuf[j].sym = tmpPosSym.sym;
                        i++;
                        j--;
                    }
                }
            } else { /* convertAccents */
                for (i = 0; i <head->len; i++) {
                    fstSymbol = (PICOKFST_PLANE_PHONEMES << 8) + content[i];
                    rv = sphoAddPhoneme(spho,pos,fstSymbol);
                }
            }
            break;
        default:
            picoos_emRaiseException(this->common->em,rv,NULL,NULL);
            break;
    } /* switch(head->type) */
    if (PICO_OK != rv) {
        spho->phonWritePos = oldPos;
    }
    return rv;
}





#define SPHO_POSSYM_OK           0
#define SPHO_POSSYM_OUT_OF_RANGE 1
#define SPHO_POSSYM_END          2
#define SPHO_POSSYM_INVALID     -3
/* *readPos is the next position in phonBuf to be read, and *writePos is the first position not to be read (may be outside
 * buf).
 * 'rangeEnd' is the first possym position outside the desired range.
 * Possible return values:
 * SPHO_POSSYM_OK            : 'pos' and 'sym' are set to the read possym, *readPos is advanced
 * SPHO_POSSYM_OUT_OF_RANGE  : pos is out of range. 'pos' is set to that of the read possym, 'sym' is undefined
 * SPHO_POSSYM_UNDERFLOW     : no more data in buf. 'pos' is set to PICOTRNS_POS_INVALID,    'sym' is undefined
 * SPHO_POSSYM_INVALID       : "strange" pos.       'pos' is set to PICOTRNS_POS_INVALID,    'sym' is undefined
 */
static pico_status_t getNextPosSym(spho_subobj_t * spho, picoos_int16 * pos, picoos_int16 * sym,
        picoos_int16 rangeEnd) {
    /* skip POS_IGNORE */
    while ((spho->phonReadPos < spho->phonWritePos) && (PICOTRNS_POS_IGNORE == spho->phonBuf[spho->phonReadPos].pos))  {
        PICODBG_DEBUG(("ignoring phone at spho->phonBuf[%i] because it has pos==IGNORE",spho->phonReadPos));
        spho->phonReadPos++;
    }
    if ((spho->phonReadPos < spho->phonWritePos)) {
        *pos = spho->phonBuf[spho->phonReadPos].pos;
        if ((PICOTRNS_POS_INSERT == *pos) || ((0 <= *pos) && (*pos < rangeEnd))) {
            *sym = spho->phonBuf[spho->phonReadPos++].sym;
            return SPHO_POSSYM_OK;
        } else if (*pos < 0){ /* *pos is "strange" (e.g. POS_INVALID) */
            return SPHO_POSSYM_INVALID;
        } else {
            return SPHO_POSSYM_OUT_OF_RANGE;
        }
    } else {
        /* no more possyms to read */
        *pos = PICOTRNS_POS_INVALID;
        return SPHO_POSSYM_END;
    }
}



/** Calculate bound strength modified by transduction
 *
 * Given the original bound strength 'orig' and the desired target strength 'target' (suggested by fst),
 *  calculate the modified bound strength.
 *
 * @param orig  original bound strength
 * @param target target bound strength
 * @return resulting bound strength
 */
static picoos_uint8 fstModifiedBoundStrength(picoos_uint8 orig, picoos_uint8 target)
{
    switch (orig) {
        case PICODATA_ITEMINFO1_BOUND_PHR1:
        case PICODATA_ITEMINFO1_BOUND_PHR2:
            /* don't allow primary phrase bounds to be demoted to word bound */
            if (PICODATA_ITEMINFO1_BOUND_PHR0 == target) {
                return PICODATA_ITEMINFO1_BOUND_PHR3;
            }
        case PICODATA_ITEMINFO1_BOUND_PHR0:
        case PICODATA_ITEMINFO1_BOUND_PHR3:
            return target;
            break;
        default:
            /* don't allow bounds other than phrase or word bounds to be changed */
            return orig;
            break;
    }
}

/** Calculate bound strength modified by a \<break> command
 *
 * Given the original (predicted and possibly fst-modified) bound strength, and a time value from an
 * overwriding \<break> command, calculate the modified bound strength.
 *
 * @param orig original bound strength
 * @param time time given as property of \<break> command
 * @param wasPrimary
 * @return modified bound strength
 */
static picoos_uint8 breakModifiedBoundStrength(picoos_uint8 orig, picoos_uint16 time, picoos_bool wasPrimary)
{
    picoos_uint8 modified = (0 == time) ? PICODATA_ITEMINFO1_BOUND_PHR3 :
        (50 < time) ? PICODATA_ITEMINFO1_BOUND_PHR1 : PICODATA_ITEMINFO1_BOUND_PHR2;
    switch (orig) {
        /* for word and phrase breaks, return 'modified', unless a non-silence gets time==0, in which
         * case return no break (word break) */
        case PICODATA_ITEMINFO1_BOUND_PHR0:
            if (0 == time) {
                return PICODATA_ITEMINFO1_BOUND_PHR0;
            }
        case PICODATA_ITEMINFO1_BOUND_PHR3:
            if (!wasPrimary && (0 == time)) {
                return PICODATA_ITEMINFO1_BOUND_PHR0;
            }
        case PICODATA_ITEMINFO1_BOUND_PHR1:
        case PICODATA_ITEMINFO1_BOUND_PHR2:
            return modified;
            break;
        default:
            return orig;
            break;
    }
}

static picoos_bool breakStateInterrupting(picodata_itemhead_t * head,
        picoos_bool * breakBefore, picoos_bool * breakAfter) {

    picoos_bool result = 1;

    *breakBefore = 0;
    *breakAfter = 0;

    if (PICODATA_ITEM_WORDPHON == head->type) {

    } else if (PICODATA_ITEM_CMD == head->type) {
        if ((PICODATA_ITEMINFO1_CMD_PLAY == head->info1)
                || (PICODATA_ITEMINFO1_CMD_SAVE == head->info1)
                || (PICODATA_ITEMINFO1_CMD_UNSAVE == head->info1)) {
            *breakBefore = 1;
            *breakAfter = 1;
        } else if (PICODATA_ITEMINFO1_CMD_SAVE == head->info1) {
            *breakBefore = 1;
        } else if (PICODATA_ITEMINFO1_CMD_UNSAVE == head->info1) {
            *breakAfter = 1;
        } else if (PICODATA_ITEMINFO1_CMD_IGNSIG == head->info1) {
            if (PICODATA_ITEMINFO2_CMD_START == head->info2) {
                *breakBefore = 1;
            } else {
                *breakAfter = 1;
            }
        }
    } else {
        result = 0;
    }
    return result;
}


static void putSideBoundToOutput(spho_subobj_t * spho)
{

    picodata_itemhead_t ohead;
    picoos_uint8 ocontent[2*sizeof(picoos_uint16)];
    picoos_int16 sildur;
    picoos_uint16 clen;

    /* create boundary */
    ohead.type = PICODATA_ITEM_BOUND;
    ohead.info1 = spho->headx[spho->outReadPos].boundstrength;
    ohead.info2 = spho->headx[spho->outReadPos].phrasetype;
    sildur = spho->headx[spho->outReadPos].sildur;
    if ((sildur < 0)
            || (PICODATA_ITEMINFO1_BOUND_PHR0 == ohead.info1)
            || (PICODATA_ITEMINFO1_BOUND_PHR3 == ohead.info1)) {
        PICODBG_DEBUG(("outputting a bound of strength '%c' and type '%c' without duration constraints",ohead.info1, ohead.info2));
        ohead.len = 0;
    } else {
        picoos_uint32 pos = 0;
        picoos_write_mem_pi_uint16(ocontent,&pos,sildur);
        picoos_write_mem_pi_uint16(ocontent,&pos,sildur);
        PICODBG_DEBUG(("outputting a bound of strength '%c' and type '%c' with duration constraints [%i,%i]",ohead.info1, ohead.info2,sildur, sildur));
        ohead.len = pos;
    }
    picodata_put_itemparts(&ohead, ocontent, ohead.len,
            spho->outBuf, spho->outBufSize, &clen);
    /* disable side bound */
    spho->headx[spho->outReadPos].boundstrength = 0;
}

/** Set bound strength and sil dur.
 *
 * given the original bound strength 'orig_strength' and the fst-suggested bound strength 'fst_strength'
 * and possibly being in a pending break state, calculate the resulting bound strength and set boundstrength
 * and sildur of the current item (spho->headx[spho->outReadPos]) accordingly.
 * if a boundstrength was set, also calculate the phrasetype and if necessary (and reachable), modify the phrase type
 * of the previous phrase boundary.
 *
 * @param spho
 * @param orig_strength
 * @param orig_type
 * @param fst_strength
 */
static void setSideBound(spho_subobj_t * spho, picoos_uint8 orig_strength, picoos_uint8 orig_type, picoos_uint8 fst_strength) {
    picoos_uint8 strength;

    /* insert modified bound according to transduction symbol, if any */
    if (PICODATA_ITEMINFO1_NA == orig_strength) {
        /* no original/fst strength given */
        orig_strength = PICODATA_ITEMINFO1_BOUND_PHR0;
        strength = PICODATA_ITEMINFO1_BOUND_PHR0;
    } else {
        strength = fstModifiedBoundStrength(orig_strength,fst_strength);
        spho->headx[spho->outReadPos].boundstrength = strength;
        spho->headx[spho->outReadPos].sildur = -1;
        PICODBG_DEBUG(("setting bound strength to fst-suggested value %c (was %c)",strength, spho->headx[spho->outReadPos].boundstrength, spho->breakTime));
    }

    /* insert modified bound according to pending break, if any */
    if (spho->breakPending) {
        /* the calculation is based on the fst-modified value (because this is what the customer wants to
         * override)
         */
        strength = breakModifiedBoundStrength(strength, spho->breakTime, (PICODATA_ITEMINFO1_BOUND_PHR1 == orig_strength));
        PICODBG_DEBUG(("setting bound strength to break-imposed value %c (was %c) and time to %i",strength, spho->headx[spho->outReadPos].boundstrength, spho->breakTime));
        spho->headx[spho->outReadPos].boundstrength =  strength;
        spho->headx[spho->outReadPos].sildur = spho->breakTime;
        spho->breakPending = FALSE;
    }
    if (spho->headx[spho->outReadPos].boundstrength) {
        /* we did set a bound strength, possibly promoting or demoting a boundary; now set the phrase type
         * possibly also changing the phrase type of the previous phrase bound
         */
        picoos_uint8 fromPhrase = ((PICODATA_ITEMINFO1_BOUND_PHR0 != orig_strength));
        picoos_uint8 toPhrase = ((PICODATA_ITEMINFO1_BOUND_PHR0 != strength));

        PICODBG_DEBUG(("setting phrase type (wasPhrase=%i, isPhrase=%i)",fromPhrase,toPhrase));
        if (toPhrase) {
            if (fromPhrase) {
                spho->lastPhraseType = orig_type;
            } else { /*promote */
                if (spho->activeStartPos <= spho->lastPhraseBoundPos) {
                    /* we still can change prev phrase bound */
                    /* since a new phrase boundary is introduced, we have to 'invent'
                     * an additional phrase type here. For that, we have to use some of the
                     * knowledge that otherwise is handled in picoacph.
                     */
                    spho->headx[spho->lastPhraseBoundPos].phrasetype
                            = PICODATA_ITEMINFO2_BOUNDTYPE_P;
                }
            }
            spho->lastPhraseBoundPos = spho->outReadPos;
            spho->headx[spho->lastPhraseBoundPos].phrasetype
                    = spho->lastPhraseType;

        } else {
            spho->headx[spho->outReadPos].phrasetype = PICODATA_ITEMINFO2_NA;
            if (fromPhrase) { /* demote */
                spho->lastPhraseType = orig_type;
                if (spho->activeStartPos <= spho->lastPhraseBoundPos) {
                    /* we still can change prev phrase bound */
                    spho->headx[spho->lastPhraseBoundPos].phrasetype
                        = spho->lastPhraseType;
                }
            }
        }
    }
}


/* ***********************************************************************/
/*                          sphoStep function                            */
/* ***********************************************************************/


static picodata_step_result_t sphoStep(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * numBytesOutput)
{

    register spho_subobj_t *spho;
    pico_status_t rv= PICO_OK;
    picoos_uint16 blen;
    picodata_itemhead_t ihead, ohead;
    picoos_uint8 *icontent;
    picoos_uint16 nextInPos;
#if defined(PICO_DEBUG)
    picoos_char msgstr[SPHO_MSGSTR_SIZE];
#endif

    /* used in FEED and FEED_SYM */
    picoos_uint16 clen;
    picoos_int16 pos, sym, sylsym;
    picoos_uint8 plane;

    /* used in BOUNDS */
    picoos_bool breakBefore, breakAfter;

    /* pico_status_t rvP= PICO_OK; */

    picoos_uint16 curPos /*, nextPos */;
    picoos_uint16 remHeadxSize, remCbufSize;


    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    spho = (spho_subobj_t *) this->subObj;

    mode = mode;        /* avoid warning "var not used in this function"*/

    *numBytesOutput = 0;
    while (1) { /* exit via return */
        PICODBG_INFO(("doing state %i, headxReadPos: %d, headxWritePos: %d",
                        spho->procState, spho->headxReadPos, spho->headxWritePos));

        switch (spho->procState) {

            case SPHO_STEPSTATE_INIT:
                /* **********************************************************************/
                /* INIT                                                              */
                /* **********************************************************************/
                PICODBG_DEBUG(("INIT"));
            /* (re)set values for PARSE */
            spho->penultima = SPHO_POS_INVALID;
            spho->activeEndPos = SPHO_POS_INVALID;
            spho->headxReadPos = 0;
            spho->phonReadPos = 0;
            spho->phonWritePos = 0;
            spho->lastPhraseType = PICODATA_ITEMINFO2_NA;
            spho->lastPhraseBoundPos = -1;

            spho->procState = SPHO_STEPSTATE_COLLECT;
            break;


            case SPHO_STEPSTATE_COLLECT:
                /* **********************************************************************/
                /* COLLECT                                                              */
                /* **********************************************************************/
                /* collect state: get items from charBuf and store in
                 * internal inBuf
                 */
                PICODBG_TRACE(("COLLECT"));
                rv = PICO_OK;
                remHeadxSize = spho->headxBufSize - spho->headxWritePos;
                remCbufSize = spho->cbufBufSize - spho->cbufWritePos;
                curPos = spho->headxWritePos;
                while ((PICO_OK == rv) && (remHeadxSize > 0) && (remCbufSize > 0)) {
                    PICODBG_DEBUG(("COLLECT getting item at headxWritePos %i (remaining %i)",spho->headxWritePos, remHeadxSize));
                    rv = picodata_cbGetItem(this->cbIn, spho->tmpbuf, PICODATA_MAX_ITEMSIZE, &blen);
                    if (PICO_OK == rv) {
                        rv = picodata_get_itemparts(spho->tmpbuf,
                                            PICODATA_MAX_ITEMSIZE, &(spho->headx[spho->headxWritePos].head),
                                                    &(spho->cbuf[spho->cbufWritePos]), remCbufSize, &blen);
                        if (PICO_OK == rv) {
                            spho->headx[spho->headxWritePos].cind = spho->cbufWritePos;
                            spho->headx[spho->headxWritePos].boundstrength = 0;
                            spho->headxWritePos++;
                            remHeadxSize--;
                            spho->cbufWritePos += blen;
                            remCbufSize -= blen;
                        }
                    }
                }
                if ((PICO_OK == rv) && ((remHeadxSize <= 0) || (remCbufSize <= 0))) {
                    rv = PICO_EXC_BUF_OVERFLOW;
                }

                /* in normal circumstances, rv is either PICO_EOF (no more items in cbIn) or PICO_BUF_OVERFLOW
                 * (if no more items fit into headx) */
                if ((PICO_EOF != rv) && (PICO_EXC_BUF_OVERFLOW != rv)) {
                    PICODBG_DEBUG(("COLLECT ** problem getting item, unhandled, rv: %i", rv));
                    picoos_emRaiseException(this->common->em, rv,
                    NULL, NULL);
                    return PICODATA_PU_ERROR;
                }
                if (PICO_EOF == rv) { /* there are no more items available */
                    if (curPos < spho->headxWritePos) { /* we did get some new items */
                        PICODBG_DEBUG(("COLLECT read %i items",
                                        spho->headxWritePos - curPos));
                        spho->needMoreInput = FALSE;
                    }
                    if (spho->needMoreInput) { /* not enough items to proceed */
                        PICODBG_DEBUG(("COLLECT need more data, returning IDLE"));
                        return PICODATA_PU_IDLE;
                    } else {
                        spho->procState = SPHO_STEPSTATE_PROCESS_PARSE;
                        /* uncomment next to split into two steps */
                        /* return PICODATA_PU_ATOMIC; */
                    }
                } else { /* input buffer full */
                    PICODBG_DEBUG(("COLLECT input buffer full"));
                    if (spho->needMoreInput) { /* forced output because we can't get more data */
                        spho->needMoreInput = FALSE;
                        spho->force = TRUE;
                    }
                    spho->procState = SPHO_STEPSTATE_PROCESS_PARSE;
                }
                break;

           case SPHO_STEPSTATE_PROCESS_PARSE:

                /* **********************************************************************/
                /* PARSE: items -> input pos/phon pairs */
                /* **********************************************************************/

                /* parse one item at a time */
                /* If
                 *    - the item is a sentence end or
                 *    - it is the last item and force=1 or
                 *    - the phon buffer is full
                 * then set inReadPos to 0 and go to TRANSDUCE
                 * else advance by one item */

                /* look at the current item */
                PICODBG_TRACE(("PARSE"));
                if (spho->headxReadPos >= spho->headxWritePos) {
                    /* no more items in headx */
                    if (spho->force) {
                        PICODBG_INFO(("no more items in headx but we are forced to transduce"));

                        /* headx is full; we are forced to transduce before reaching the sentence end */
                        spho->force = FALSE;
                        if (SPHO_POS_INVALID == spho->activeEndPos) {
                            spho->activeEndPos = spho->headxReadPos;
                        }
                        spho->procState = SPHO_STEPSTATE_PROCESS_TRANSDUCE;
                    } else {
                        /* we try to get more data */
                        PICODBG_INFO(("no more items in headx, try to collect more"));
                        spho->needMoreInput = TRUE;
                        spho->procState = SPHO_STEPSTATE_COLLECT;
                    }
                    break;
                }

                ihead = spho->headx[spho->headxReadPos].head;
                icontent = spho->cbuf + spho->headx[spho->headxReadPos].cind;

                PICODBG_DEBUG(("PARSE looking at item %s",picodata_head_to_string(&ihead,msgstr,SPHO_MSGSTR_SIZE)));
                /* treat header */
                if (PICODATA_ITEM_BOUND == ihead.type) {
                    /* see if it is a sentence end or termination boundary (flush) */
                    if ((PICODATA_ITEMINFO1_BOUND_SEND == ihead.info1)
                    || (PICODATA_ITEMINFO1_BOUND_TERM == ihead.info1)) {
                        PICODBG_INFO(("PARSE found sentence  end or term BOUND"));

                        if (spho->sentenceStarted) {
                            /* its the end of the sentence */
                            PICODBG_INFO(("PARSE found sentence end"));
                            spho->sentenceStarted = 0;
                            /* there is no need for a right context; move the active end to the end */
                            /* add sentence termination phonemes */
                            sphoAddTermPhonemes(spho, spho->headxReadPos);
                            spho->headxReadPos++;
                            spho->activeEndPos = spho->headxReadPos;
                            /* we may discard all information up to activeEndPos, after processing of last
                             * sentence part
                             */
                            spho->penultima = spho->activeEndPos;

                            /* transduce */
                            spho->procState = SPHO_STEPSTATE_PROCESS_TRANSDUCE;
                            /* uncomment to split */
                            /* return PICODATA_PU_BUSY; */
                            break;
                        } else {
                            if (PICODATA_ITEMINFO1_BOUND_TERM == ihead.info1) {
                                /* its the end of input (flush) */
                                PICODBG_INFO(("PARSE forwarding input end (flush)"));
                                /* copy item unmodified */
                                picodata_put_itemparts(&ihead,
                                         icontent,
                                         ihead.len,
                                         spho->outBuf, spho->outBufSize,
                                         &clen);

                                spho->headxReadPos++;
                                spho->activeEndPos = spho->headxReadPos;
                                spho->penultima = SPHO_POS_INVALID;
                                spho->feedFollowState = SPHO_STEPSTATE_SHIFT;
                                spho->procState = SPHO_STEPSTATE_FEED;
                                break;
                            } else {
                                /* this should never happen */
                                /* eliminate bound */
                                spho->headxReadPos++;
                                spho->activeEndPos = spho->headxReadPos;
                                spho->penultima = SPHO_POS_INVALID;
                                PICODBG_ERROR(("PARSE found a sentence end without a sentence start; eliminated"));
                            }
                        }
                    } else if (PICODATA_ITEMINFO1_BOUND_SBEG == ihead.info1) {
                            /* its the start of the sentence */
                            PICODBG_INFO(("PARSE found sentence start"));
                            /* add sentence starting phoneme */
                            sphoAddStartPhoneme(spho);

                            spho->sentenceStarted = 1;
                    }
                }

                if ((PICODATA_ITEM_WORDPHON == ihead.type)
                        || (PICODATA_ITEM_BOUND == ihead.type)) {
                    /* if it is a word or a bound try to extract phonemes */
                    PICODBG_INFO(("PARSE found WORD phon or phrase BOUND"));
                    rv = sphoExtractPhonemes(this, spho, spho->headxReadPos,
                            TRUE /* convertAccents */,
                            &spho->suppressParseWordBound);
                    if (PICO_OK == rv) {
                        PICODBG_INFO(("PARSE successfully returned from phoneme extraction"));
                        /* replace activeEndPos if the new item is a word, or activeEndPos was not set yet, or
                         * activeEndPos was a bound */
                        if ((spho->activeStartPos <= spho->headxReadPos) && ((PICODATA_ITEM_WORDPHON == ihead.type)
                                || (SPHO_POS_INVALID == spho->activeEndPos)
                                || (PICODATA_ITEM_BOUND == spho->headx[spho->activeEndPos].head.type))) {
                            PICODBG_INFO(("PARSE found new activeEndPos: %i,%i -> %i,%i",
                                            spho->penultima,spho->activeEndPos,spho->activeEndPos,spho->headxReadPos));
                            spho->penultima = spho->activeEndPos;
                            spho->activeEndPos = spho->headxReadPos;
                        }

                    } else if (PICO_EXC_BUF_OVERFLOW == rv) {
                        /* phoneme buffer cannot take this item anymore;
                           if the phoneme buffer has some contents, we are forced to transduce before reaching the sentence end
                           else we skip the (too long word) */
                        PICODBG_INFO(("PARSE returned from phoneme extraction with overflow, number of phonemes in phonBuf: %i; forced to TRANSDUCE", spho->phonWritePos));
                        if ((SPHO_POS_INVALID == spho->activeEndPos) || (spho->activeStartPos == spho->activeEndPos)) {
                            spho->activeEndPos = spho->headxReadPos;
                        }
                        spho->procState = SPHO_STEPSTATE_PROCESS_TRANSDUCE;
                        break;
                    } else {
                        PICODBG_ERROR(("PARSE returned from phoneme extraction with exception %i",rv));
                        return (picodata_step_result_t)picoos_emRaiseException(this->common->em,
                        PICO_ERR_OTHER, NULL, NULL);
                    }
                } else {
                    PICODBG_INFO(("PARSE found other item, passing over"));
                    /* it is "other" item, ignore */
                }
                /* set pos at next item */
                PICODBG_INFO(("PARSE going to next item: %i -> %i",spho->headxReadPos, spho->headxReadPos + 1));
                spho->headxReadPos++;
                break;

            case SPHO_STEPSTATE_PROCESS_TRANSDUCE:

                /* **********************************************************************/
                /* TRANSDUCE: transduction input pos/phon pairs to output pos/phon pairs */
                /* **********************************************************************/
                PICODBG_DEBUG(("TRANSDUCE (%i-th of %i fsts",spho->curFst+1, spho->numFsts));

                /* termination condition first */
                if (spho->curFst >= spho->numFsts) {

#if defined(PICO_DEBUG)
                    {
                        PICODBG_INFO_CTX();
                        PICODBG_INFO_MSG(("result of all transductions: "));
                        PICOTRNS_PRINTSYMSEQ(this->voice->kbArray[PICOKNOW_KBID_DBG], spho->phonBufOut, spho->phonWritePos);
                        PICODBG_INFO_MSG(("\n"));
                    }
#endif

                    /* reset for next transduction */
                    spho->curFst = 0;
                    /* prepare BOUNDS */
                    spho->outReadPos = 0;
                    spho->phonReadPos = 0;

                    spho->procState = SPHO_STEPSTATE_PROCESS_BOUNDS;
                    break;
                }

                /* transduce from phonBufIn to PhonBufOut */
                {

                    picoos_uint32 nrSteps;
#if defined(PICO_DEBUG)
                    {
                        PICODBG_INFO_CTX();
                        PICODBG_INFO_MSG(("spho trying to transduce: "));
                        PICOTRNS_PRINTSYMSEQ(this->voice->kbArray[PICOKNOW_KBID_DBG], spho->phonBuf, spho->phonWritePos);
                        PICODBG_INFO_MSG(("\n"));
                    }
#endif
                    rv = picotrns_transduce(spho->fst[spho->curFst], FALSE,
                    picotrns_printSolution, spho->phonBuf, spho->phonWritePos, spho->phonBufOut,
                            &spho->phonWritePos,
                            4*PICOTRNS_MAX_NUM_POSSYM, spho->altDescBuf,
                            spho->maxAltDescLen, &nrSteps);
                    if (PICO_OK == rv) {
#if defined(PICO_DEBUG)
                    {
                        PICODBG_INFO_CTX();
                        PICODBG_INFO_MSG(("result of transduction: (output symbols: %i)", spho->phonWritePos));
                        PICOTRNS_PRINTSYMSEQ(this->voice->kbArray[PICOKNOW_KBID_DBG], spho->phonBufOut, spho->phonWritePos);
                        PICODBG_INFO_MSG(("\n"));
                    }
#endif
                        PICODBG_TRACE(("number of steps done in tranduction: %i", nrSteps));
                    } else {
                        picoos_emRaiseWarning(this->common->em, PICO_WARN_FALLBACK,NULL,(picoos_char *)"phon buffer full");
                    }
                }
                /* eliminate deep epsilons */
                picotrns_eliminate_epsilons(spho->phonBufOut, spho->phonWritePos, spho->phonBuf,
                        &spho->phonWritePos,4*PICOTRNS_MAX_NUM_POSSYM);

                spho->curFst++;

                /* return PICODATA_PU_ATOMIC */
                break;


            case SPHO_STEPSTATE_PROCESS_BOUNDS:
                /* ************************************************************************/
                /* BOUNDS: combine input item with pos/phon pairs to insert/modify bounds */
                /* ************************************************************************/

                PICODBG_INFO(("BOUNDS"));

                /* get the suppressRecombWordBound in the left context */
                spho->suppressRecombWordBound = FALSE;
                while (spho->outReadPos < spho->activeStartPos) {
                    /* look at the current item */
                    ihead = spho->headx[spho->outReadPos].head;
                    /* icontent = spho->cbuf + spho->headx[spho->outReadPos].cind; */
                    PICODBG_INFO(("in position %i, looking at item %s",spho->outReadPos,picodata_head_to_string(&ihead,msgstr,SPHO_MSGSTR_SIZE)));
                    if (PICODATA_ITEM_BOUND == ihead.type) {
                        spho->suppressRecombWordBound = TRUE;
                    } else if (PICODATA_ITEM_WORDPHON == ihead.type) {
                        spho->suppressRecombWordBound = FALSE;
                    }
                    spho->outReadPos++;
                }
                /* spho->outReadPos point now to the active region */

                /* advance the phone reading pos to the active range */
                spho->phonReadPos = 0;
                while (SPHO_POSSYM_OK == (rv = getNextPosSym(spho, &pos, &sym,
                        spho->activeStartPos))) {
                    /* ignore */
                }
                PICODBG_INFO(("skipping left context phones results in %s", (SPHO_POSSYM_OUT_OF_RANGE==rv) ? "OUT_OF_RANGE" : (SPHO_POSSYM_END ==rv) ? "END" : "OTHER"));

                /*
                 * Align input items with transduced phones and note bound stregth changes and break commands
                 */

                while (spho->outReadPos < spho->activeEndPos) {

                    /* look at the current item */
                    ihead = spho->headx[spho->outReadPos].head;
                    icontent = spho->cbuf + spho->headx[spho->outReadPos].cind;
                    nextInPos = spho->outReadPos + 1;
                    /*  */
                    PICODBG_INFO(("in position %i, looking at item %s",spho->outReadPos,picodata_head_to_string(&ihead,msgstr,SPHO_MSGSTR_SIZE)));

                    if ((PICODATA_ITEM_BOUND == ihead.type)
                            || ((PICODATA_ITEM_WORDPHON == ihead.type)
                                    && (!spho->suppressRecombWordBound))) {
                        /* there was a boundary originally */
                        picoos_uint8 orig_strength, orig_type;
                        if (PICODATA_ITEM_BOUND == ihead.type) {
                            orig_strength = ihead.info1;
                            orig_type = ihead.info2;
                            spho->suppressRecombWordBound = TRUE;
                        } else {
                            orig_strength = PICODATA_ITEMINFO1_BOUND_PHR0;
                            orig_type = PICODATA_ITEMINFO2_NA;
                        }
                        /* i expect a boundary phone here */
                        /* consume FST bound phones, consider pending break and set the side-bound */
                        PICODBG_INFO(("got BOUND or WORDPHON item and expects corresponding phone"));
                        rv = getNextPosSym(spho, &pos, &sym, nextInPos);
                        if (SPHO_POSSYM_OK != rv) {
                            PICODBG_ERROR(("unexpected symbol or unexpected end of phoneme list (%s)", (SPHO_POSSYM_OUT_OF_RANGE==rv) ? "OUT_OF_RANGE" : (SPHO_POSSYM_END ==rv) ? "END" :"OTHER"));
                            return (picodata_step_result_t)picoos_emRaiseException(this->common->em,
                                    PICO_ERR_OTHER, NULL, NULL);
                        }
                        sym = picotrns_unplane(sym, &plane);
                        /*   */
                        PICODBG_ASSERT((PICOKFST_PLANE_PB_STRENGTHS == plane));

                        /* insert modified bound according to transduction and possibly pending break */
                        setSideBound(spho, orig_strength, orig_type,
                                (picoos_uint8) sym);
                    } else if ((PICODATA_ITEM_CMD == ihead.type)
                            && (PICODATA_ITEMINFO1_CMD_SIL == ihead.info1)) {
                        /* it's a SIL (break) command */
                        picoos_uint16 time;
                        picoos_uint32 pos = 0;
                        picoos_read_mem_pi_uint16(icontent, &pos, &time);
                        if (spho->breakPending) {
                            spho->breakTime += time;
                        } else {
                            spho->breakTime = time;
                            spho->breakPending = TRUE;
                        }
                    } else if ((PICODATA_ITEM_CMD == ihead.type) && (PICODATA_ITEMINFO1_CMD_PLAY == ihead.info1)) {
                        /* insert break of at least one ms */
                        if (!spho->breakPending || (spho->breakTime <= 0)) {
                            spho->breakTime = SPHO_SMALLEST_SIL_DUR;
                            spho->breakPending = TRUE;
                        }
                        setSideBound(spho, PICODATA_ITEMINFO1_NA,
                                PICODATA_ITEMINFO2_NA, PICODATA_ITEMINFO1_NA);
                        /* force following break to be at least one ms */
                        spho->breakTime = SPHO_SMALLEST_SIL_DUR;
                        spho->breakPending = TRUE;
                    } else if (breakStateInterrupting(&ihead, &breakBefore, &breakAfter)) {

                        if (breakBefore &&(!spho->breakPending || (spho->breakTime <= 0))) {
                            spho->breakTime = SPHO_SMALLEST_SIL_DUR;
                            spho->breakPending = TRUE;
                        }
                        setSideBound(spho, PICODATA_ITEMINFO1_NA,
                                PICODATA_ITEMINFO2_NA, PICODATA_ITEMINFO1_NA);

                        if (breakAfter) {
                            spho->breakTime = SPHO_SMALLEST_SIL_DUR;
                            spho->breakPending = TRUE;
                        }
                        if (PICODATA_ITEM_WORDPHON == ihead.type) {
                            spho->suppressRecombWordBound = FALSE;
                        }
                    }

                    /* skip phones of that item */
                    while (SPHO_POSSYM_OK == (rv = getNextPosSym(spho, &pos,
                            &sym, nextInPos))) {
                        /* ignore */
                    }
                    spho->outReadPos++;
                }

                /* reset for RECOMB */
                spho->outReadPos = 0;
                spho->phonReadPos = 0;
                spho->suppressRecombWordBound = FALSE;

                spho->procState = SPHO_STEPSTATE_PROCESS_RECOMB;
                return PICODATA_PU_ATOMIC;

                break;

           case SPHO_STEPSTATE_PROCESS_RECOMB:
                /* **********************************************************************/
                /* RECOMB: combine input item with pos/phon pairs to output item */
                /* **********************************************************************/

                PICODBG_TRACE(("RECOMB"));

                /* default place to come after feed: here */
                spho->feedFollowState = SPHO_STEPSTATE_PROCESS_RECOMB;

                /* check termination condition first */
                if (spho->outReadPos >= spho->activeEndPos) {
                    PICODBG_DEBUG(("RECOMB reached active region's end at %i",spho->outReadPos));
                    spho->procState = SPHO_STEPSTATE_SHIFT;
                    break;
                }

                /* look at the current item */
                ihead = spho->headx[spho->outReadPos].head;
                icontent = spho->cbuf + spho->headx[spho->outReadPos].cind;

                PICODBG_DEBUG(("RECOMB looking at item %s",picodata_head_to_string(&ihead,msgstr,SPHO_MSGSTR_SIZE)));

                nextInPos = spho->outReadPos + 1;

                PICODBG_DEBUG(("RECOMB treating item in headx at pos %i",spho->outReadPos));
                if (nextInPos <= spho->activeStartPos) { /* we're in the (passive) left context. Just skip it */
                    PICODBG_DEBUG(("RECOMB skipping item in the left context (%i <= %i)",nextInPos, spho->activeStartPos));
                    if (PICODATA_ITEM_BOUND == ihead.type) {
                        spho->suppressRecombWordBound = 1;
                    } else if (PICODATA_ITEM_WORDPHON == ihead.type) {
                        spho->suppressRecombWordBound = 0;
                    }

                    /* consume possyms */
                    while (SPHO_POSSYM_OK == (rv = getNextPosSym(spho,&pos,&sym,nextInPos))) {
                        /* ignore */
                    }
                    if (rv == SPHO_POSSYM_INVALID) {
                        return (picodata_step_result_t)picoos_emRaiseException(this->common->em,
                        PICO_ERR_OTHER, NULL, NULL);
                    }
                    spho->outReadPos = nextInPos;
                } else { /* active region */
                    if (spho->headx[spho->outReadPos].boundstrength) {
/* ***************** "side-bound" *********************/
                        /* copy to outbuf */
                        putSideBoundToOutput(spho);
                        /* mark as processed */
                        spho->headx[spho->outReadPos].boundstrength = 0;
                        /* output it */
                        spho->procState = SPHO_STEPSTATE_FEED;
                    } else if (PICODATA_ITEM_BOUND == ihead.type) {
/* ***************** BOUND *********************/
                        /* expect a boundary phone here */
                        PICODBG_DEBUG(("RECOMB got BOUND item and expects corresponding phone"));
                        rv = getNextPosSym(spho, &pos, &sym, nextInPos);
                        if (SPHO_POSSYM_OK != rv) {
                            PICODBG_ERROR(("unexpected symbol or unexpected end of phoneme list"));
                            return (picodata_step_result_t)picoos_emRaiseException(
                                    this->common->em, PICO_ERR_OTHER, NULL,
                                    NULL);
                        }
                        sym = picotrns_unplane(sym, &plane);
                        /*   */
                        PICODBG_ASSERT((PICOKFST_PLANE_PB_STRENGTHS == plane));

                        spho->suppressRecombWordBound = TRUE; /* if word following, don't need word boundary */
                        /* just consume item and come back here*/
                        spho->outReadPos = nextInPos;

                    } else if (PICODATA_ITEM_WORDPHON == ihead.type) {
/* ***************** WORDPHON *********************/
                        spho->wordStarted = TRUE;
                        /* i expect a word boundary symbol in this range unless a phrase boundary was encountered before */
                        if (spho->suppressRecombWordBound) {
                            PICODBG_DEBUG(("RECOMB got WORDPHON item but skips expecting BOUND"));
                            spho->suppressRecombWordBound = FALSE;
                        } else {
                            PICODBG_DEBUG(("RECOMB got WORDPHON item and expects corresponding bound phone"));
                            rv = getNextPosSym(spho, &pos, &sym, nextInPos);
                            if (SPHO_POSSYM_OK != rv) {
                                PICODBG_ERROR(("unexpected symbol or unexpected end of phoneme list"));
                                return (picodata_step_result_t)picoos_emRaiseException(this->common->em,
                                PICO_ERR_OTHER, NULL, NULL);
                            }
                        }
                        spho->procState = SPHO_STEPSTATE_PROCESS_SYL;
                    } else if ((PICODATA_ITEM_CMD == ihead.type) && (PICODATA_ITEMINFO1_CMD_SIL == ihead.info1)) {
/* ***************** BREAK COMMAND *********************/
                        /* just consume and come back here */
                        PICODBG_DEBUG(("RECOMB consuming item from inBuf %i -> %i",spho->outReadPos, nextInPos));
                        spho->outReadPos = nextInPos;
                    } else {
/* ***************** OTHER *********************/
                        /* just copy item */
                        PICODBG_DEBUG(("RECOMB found other item, just copying"));
                        picodata_put_itemparts(&ihead, icontent, ihead.len,
                                spho->outBuf, spho->outBufSize, &clen);
                        PICODBG_DEBUG(("RECOMB consuming item from inBuf %i -> %i",spho->outReadPos, nextInPos));
                        spho->outReadPos = nextInPos;
                        /* and output it */
                        spho->procState = SPHO_STEPSTATE_FEED;
                    } /* if (ihead.type) */

                }

                /* return PICODATA_PU_BUSY; */
                break;

            case SPHO_STEPSTATE_PROCESS_SYL:
                /* **********************************************************************/
                /* SYL: combine input word item with pos/phon pairs to syl output item */
                /* **********************************************************************/

                /* consume all transduced phonemes with pos in in the range [spho->outReadPos,nextInPos[ */
               PICODBG_DEBUG(("SYL"));

               spho->feedFollowState = SPHO_STEPSTATE_PROCESS_SYL;

               /* look at the current item */
               ihead = spho->headx[spho->outReadPos].head;
               icontent = spho->cbuf + spho->headx[spho->outReadPos].cind;
                nextInPos = spho->outReadPos + 1;
                PICODBG_DEBUG(("SYL (1) treating item in headx at pos %i",spho->outReadPos));
                /* create syllable item in ohead (head) and sylBuf (contents) */
                ohead.type = PICODATA_ITEM_SYLLPHON;

                PICODBG_TRACE(("SYL expects accent at phonBuf[%i] = (%i,%i) (outReadPos=%i)", spho->phonReadPos, spho->phonBuf[spho->phonReadPos].pos, spho->phonBuf[spho->phonReadPos].sym,spho->outReadPos));
                rv = getNextPosSym(spho,&pos,&sym,nextInPos);
                if (SPHO_POSSYM_OK != rv) {
                    PICODBG_ERROR(("unexpected symbol or unexpected end of phoneme list (%i)",rv));
                    return (picodata_step_result_t)picoos_emRaiseException(this->common->em, PICO_ERR_OTHER, NULL, NULL);
                }
                ohead.info2 = picotrns_unplane(sym, &plane);
                PICODBG_ASSERT((PICOKFST_PLANE_ACCENTS == plane));
                PICODBG_DEBUG(("SYL sets accent to %c", sym));

                /* for the time being, we force to use POS so we can transduce all fsts in a row without reconsulting the items */
                PICODBG_TRACE(("SYL expects POS"));
                PICODBG_DEBUG(("SYL (2) treating item in inBuf range [%i,%i[",spho->outReadPos,nextInPos));
                rv = getNextPosSym(spho,&pos,&sym,nextInPos);
                if (SPHO_POSSYM_OK != rv) {
                    PICODBG_ERROR(("unexpected symbol or unexpected end of phoneme list"));
                    return (picodata_step_result_t)picoos_emRaiseException(this->common->em, PICO_ERR_OTHER, NULL, NULL);
                }
                if (spho->wordStarted) {
                    spho->wordStarted = FALSE;
                    ohead.info1 = picotrns_unplane(sym, &plane);
                    /*  */
                    PICODBG_ASSERT(PICOKFST_PLANE_POS == plane);
                    /*  */
                    PICODBG_DEBUG(("SYL setting POS to %c", ohead.info1));
                } else {
                    ohead.info1 = PICODATA_ITEMINFO1_NA;
                }

                PICODBG_DEBUG(("SYL (3) treating item in inBuf range [%i,%i[",spho->outReadPos,nextInPos));
                /* get phonemes of that syllable; stop if syllable boundary or outside word */
                sylsym = (PICOKFST_PLANE_PHONEMES << 8)
                        + spho->syllSepId;
                PICODBG_DEBUG(("collecting syllable phonemes before headx position %i",nextInPos));
                spho->sylWritePos = 0;
                while (SPHO_POSSYM_OK == (rv = getNextPosSym(spho,&pos,&sym,nextInPos)) && (sym != sylsym)) {
                    spho->sylBuf[spho->sylWritePos++] = picotrns_unplane(sym, &plane);
                    /*  */
                   PICODBG_TRACE(("SYL adding phoneme to syllable: (pos %i,sym %i)[plane %i,sym %c]",pos,sym,plane,sym  & 0xFF));
                    PICODBG_ASSERT((PICOKFST_PLANE_PHONEMES == plane));
                }
                PICODBG_DEBUG(("SYL (4) treating item in inBuf range [%i,%i[",spho->outReadPos,nextInPos));
                ohead.len = spho->sylWritePos;
                if (SPHO_POS_INVALID == rv) {
                    PICODBG_ERROR(("unexpected symbol or unexpected end of phoneme list"));
                    return (picodata_step_result_t)picoos_emRaiseException(this->common->em, PICO_WARN_INCOMPLETE, NULL, NULL);
                } else if ((SPHO_POSSYM_OUT_OF_RANGE == rv) || (SPHO_POSSYM_END == rv)) {
                    PICODBG_DEBUG(("SYL arrived at end of word and/or end of phon buffer, go to next word"));
                    spho->outReadPos = nextInPos; /* advance to next item */
                    spho->feedFollowState = SPHO_STEPSTATE_PROCESS_RECOMB; /* go to RECOMB after feed */
                 } else {
                    PICODBG_ASSERT((sym == sylsym));
                }
                PICODBG_DEBUG(("SYL (5) treating item in inBuf range [%i,%i[",spho->outReadPos,nextInPos));

                if (ohead.len > 0) {
                    /* prepare syllable output */
                    picodata_put_itemparts(&ohead, spho->sylBuf,
                            PICODATA_BUFSIZE_DEFAULT, spho->outBuf,
                            spho->outBufSize, &clen);

                    spho->procState = SPHO_STEPSTATE_FEED;
                } else { /* skip feeding output of empty syllable */
                    spho->procState = spho->feedFollowState;
                }
                break;

             case SPHO_STEPSTATE_FEED:
                /* **********************************************************************/
                /* FEED: output output item and proceed to feedFollowState */
                /* **********************************************************************/

                PICODBG_DEBUG(("FEED"));

                PICODBG_DEBUG(("FEED putting outBuf item into cb"));

                /*feeding items to PU output buffer*/
                rv = picodata_cbPutItem(this->cbOut, spho->outBuf,
                        spho->outBufSize, &clen);

                PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                        (picoos_uint8 *)"spho: ",
                        spho->outBuf, spho->outBufSize);

                if (PICO_EXC_BUF_OVERFLOW == rv) {
                    /* we have to redo this item */
                    PICODBG_DEBUG(("FEED got overflow, returning ICODATA_PU_OUT_FULL"));
                    return PICODATA_PU_OUT_FULL;
                } else if (PICO_OK == rv) {
                    *numBytesOutput += clen;
                    spho->procState = spho->feedFollowState;
                    PICODBG_DEBUG(("FEED ok, going back to procState %i", spho->procState));
                    return PICODATA_PU_BUSY;
                } else {
                    PICODBG_DEBUG(("FEED got exception %i when trying to output item",rv));
                    spho->procState = spho->feedFollowState;
                    return (picodata_step_result_t)rv;
                }
                break;

            case SPHO_STEPSTATE_SHIFT:
                /* **********************************************************************/
                /* SHIFT                                                              */
                /* **********************************************************************/
                /* If there exists a valid penultima, it should replace any left context (from 0 to activeStartPos)
                 * else discard the current active range (from activeStartPos to activeEndPos), leaving the current
                 * left context intact. Often, PARSE would move activeStartPos to 0, so that there is no left context
                 * after the shift.
                 */

                PICODBG_DEBUG(("SHIFT"));

                if (spho->penultima != SPHO_POS_INVALID) {
                    picoos_int16 shift;
                    /* set penultima as new left context and set activeStartPos to the shifted activeEndPos */
                    PICODBG_DEBUG((
                                    "SHIFT shifting penultima from %i to 0",
                                    spho->penultima));
                    shift = shift_range_left_1(spho, &spho->penultima, 0);
                    if (shift < 0) {
                        picoos_emRaiseException(this->common->em,PICO_ERR_OTHER,NULL,NULL);
                        return PICODATA_PU_ERROR;
                    }
                    spho->activeStartPos = spho->activeEndPos
                            - shift;
                    spho->lastPhraseBoundPos -= shift;
                    spho->suppressParseWordBound = FALSE;
                    spho->suppressRecombWordBound = FALSE;

                } else {
                    picoos_int16 shift;
                    picoos_bool lastPhraseBoundActive;
                    if (spho->activeStartPos == spho->activeEndPos) {
                        /* no items consumed; we have to abandon left context */
                        spho->activeStartPos = 0;
                    }
                    lastPhraseBoundActive = (spho->lastPhraseBoundPos >= spho->activeStartPos);
                    /* dummy comment */
                    PICODBG_DEBUG(("SHIFT shift active end from %i to %i",
                                    spho->activeEndPos, spho->activeStartPos));
                    shift = shift_range_left_1(spho, &spho->activeEndPos, spho->activeStartPos);
                    if (shift < 0) {
                        picoos_emRaiseException(this->common->em,PICO_ERR_OTHER,NULL,NULL);
                        return PICODATA_PU_ERROR;
                    }
                    if (lastPhraseBoundActive) {
                        spho->lastPhraseBoundPos -= shift;
                    }
                }

                spho->procState = SPHO_STEPSTATE_INIT;
                break;

            default:
                picoos_emRaiseException(this->common->em, PICO_ERR_OTHER, NULL, NULL);
                return PICODATA_PU_ERROR;
                break;

        } /* switch (spho->procState) */

    } /* while (1) */

    /* should be never reached */
    picoos_emRaiseException(this->common->em, PICO_ERR_OTHER, NULL, NULL);
    return PICODATA_PU_ERROR;
}

#ifdef __cplusplus
}
#endif

/* end picospho.c */
