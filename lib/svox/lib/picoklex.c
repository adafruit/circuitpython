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
 * @file picoklex.c
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
#include "picoklex.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* ************************************************************/
/* lexicon */
/* ************************************************************/

/**
 * @addtogroup picolex
 *
  overview:
  - lex consists of optional searchindex and a non-empty list of lexblocks
  - lexblocks are fixed size, at the start of a block there is also the
    start of an entry
  - using the searchindex a unambiguous lexblock can be determined which
    contains the entry (or there is no entry)
  - one lex entry has POS GRAPH PHON, all mandatory, but
    - PHON can be empty string -> no pronunciation in the resulting TTS output
    - PHON can be :G2P -> use G2P later to add pronunciation
  - (POS,GRAPH) is a uniq key (only one entry allowed)
  - (GRAPH) is almost a uniq key (2-4 entries with the same GRAPH, and
    differing POS and differing PHON possible)
    - for one graph we can have two or three solutions from the lex
       which all need to be passed on the the next PU
    - in this case GRAPH, POS, and PHON all must be available in lex

  sizing:
    - 3 bytes entry index -> 16MB addressable
    - 2 bytes searchindex nr -> 64K blocks possible
    - 5 bytes per searchindex entry
      - 3 bytes for graph-prefix
      - 2 bytes blockadr in searchindex -> 64K blocks possible
    - lexblock size 512B:
      - 32M possible
      - with ~20 bytes per entry
        -> max. average of ~26 entries to be searched per lookup
    - overhead of ~10 bytes per block to sync with
      block boundaries
    - examples:
      - 500KB lex -> 1000 blocks,
        1000 entries in searchindex, ~25.6K lex-entries,
        - ~5KB searchindex
           ~10KB overhead for block sync
      - 100KB lex -> 200 blocks,
        200 entries in searchindex, ~5.1K lex-entries,
        - ~1KB searchindex
           ~2KB overhead for block sync

  pil-file: lexicon knowledge base in binary form

    lex-kb = content

    content = searchindex {lexblock}1:NRBLOCKS2

    lexblock = {lexentry}1:        (lexblock size is fixed 512Bytes)

    searchindex = NRBLOCKS2 {GRAPH1 GRAPH1 GRAPH1 LEXBLOCKIND2}=NRBLOCKS2

    lexentry = LENGRAPH1 {GRAPH1}=LENGRAPH1-1
               LENPOSPHON1 POS1 {PHON1}=LENPOSPHON1-2

    - special cases:
      - PHON is empty string (no pronunciation in the resulting TTS output):
        lexentry = LENGRAPH1 {GRAPH1}=LENGRAPH1-1  2 POS1
      - PHON can be :G2P -> use G2P later to add pronunciation:
        lexentry = LENGRAPH1 {GRAPH1}=LENGRAPH1-1  3 POS1 <reserved-phon-val=5>
    - multi-byte values always little endian
*/


/* ************************************************************/
/* lexicon data defines */
/* may not be changed with current implementation */
/* ************************************************************/

/* nr bytes of nrblocks info */
#define PICOKLEX_LEX_NRBLOCKS_SIZE 2

/* search index entry: - nr graphs
                       - nr bytes of block index
                       - nr bytes per entry, NRGRAPHS*INDSIZE */
#define PICOKLEX_LEX_SIE_NRGRAPHS  3
#define PICOKLEX_LEX_SIE_INDSIZE   2
#define PICOKLEX_LEX_SIE_SIZE      5

/* nr of bytes per lexblock */
#define PICOKLEX_LEXBLOCK_SIZE   512


/* reserved values in klex to indicate :G2P needed for a lexentry */
#define PICOKLEX_NEEDS_G2P   5


/* ************************************************************/
/* lexicon type and loading */
/* ************************************************************/

/** object       : LexKnowledgeBase
 *  shortcut     : klex
 *  derived from : picoknow_KnowledgeBase
 */

typedef struct klex_subobj *klex_SubObj;

typedef struct klex_subobj
{
    picoos_uint16 nrblocks; /* nr lexblocks = nr eles in searchind */
    picoos_uint8 *searchind;
    picoos_uint8 *lexblocks;
} klex_subobj_t;


static pico_status_t klexInitialize(register picoknow_KnowledgeBase this,
                                    picoos_Common common)
{
    picoos_uint32 curpos = 0;
    klex_subobj_t *klex;

    PICODBG_DEBUG(("start"));

    /* check whether (this->size != 0) done before calling this function */

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    klex = (klex_subobj_t *) this->subObj;

    if (PICO_OK == picoos_read_mem_pi_uint16(this->base, &curpos,
                                             &(klex->nrblocks))) {
        if (klex->nrblocks > 0) {
            PICODBG_DEBUG(("nr blocks: %i, curpos: %i", klex->nrblocks,curpos));
            klex->searchind = this->base + curpos;
        } else {
            klex->searchind = NULL;
        }
        klex->lexblocks = this->base + PICOKLEX_LEX_NRBLOCKS_SIZE +
                             (klex->nrblocks * (PICOKLEX_LEX_SIE_SIZE));
        return PICO_OK;
    } else {
        return picoos_emRaiseException(common->em, PICO_EXC_FILE_CORRUPT,
                                       NULL, NULL);
    }
}


static pico_status_t klexSubObjDeallocate(register picoknow_KnowledgeBase this,
                                          picoos_MemoryManager mm)
{
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}


/* we don't offer a specialized constructor for a LexKnowledgeBase but
 * instead a "specializer" of an allready existing generic
 * picoknow_KnowledgeBase */

pico_status_t picoklex_specializeLexKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common)
{
    if (NULL == this) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    if (this->size > 0) {
        this->subDeallocate = klexSubObjDeallocate;
        this->subObj = picoos_allocate(common->mm, sizeof(klex_subobj_t));
        if (NULL == this->subObj) {
            return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                           NULL, NULL);
        }
        return klexInitialize(this, common);
    } else {
        /* some dummy klex */
        return PICO_OK;
    }
}

/* for now we don't need to do anything special for the main lex */
/*
pico_status_t picoklex_specializeMainLexKnowledgeBase(
        picoknow_KnowledgeBase this,
        picoos_Common common)
{
    return picoklex_specializeLexKnowledgeBase(this,common);
}
*/


/* ************************************************************/
/* lexicon getLex */
/* ************************************************************/

picoklex_Lex picoklex_getLex(picoknow_KnowledgeBase this)
{
    if (NULL == this) {
        return NULL;
    } else {
        return (picoklex_Lex) this->subObj;
    }
}


/* ************************************************************/
/* functions on searchindex */
/* ************************************************************/


static picoos_uint32 klex_getSearchIndexVal(const klex_SubObj this,
                                            picoos_uint16 index)
{
    picoos_uint32 pos, val;
    pos = index * PICOKLEX_LEX_SIE_SIZE;
    val = this->searchind[pos];
    val = (val << 8) + this->searchind[pos + 1];
    val = (val << 8) + this->searchind[pos + 2];
    return val;
}


/* Determine first lexblock containing entries for specified
   grapheme. */

static picoos_uint16 klex_getLexblockNr(const klex_SubObj this,
                                        const picoos_uint8 *graphsi) {
    /* graphsi is of len PICOKLEX_LEX_SI_NGRAPHS */
    picoos_int32 low, mid, high;
    picoos_uint32 searchval, indval;

    /* PICOKLEX_LEX_SIE_NRGRAPHS */

    /* convert graph-prefix to number with 'lexicographic' ordering */
    searchval = graphsi[0];
    searchval = (searchval << 8) + graphsi[1];
    searchval = (searchval << 8) + graphsi[2];

    low = 0;
    high = this->nrblocks;

    /* do binary search */
    while (low < high) {
        mid = (low + high) / 2;
        indval = klex_getSearchIndexVal(this, mid);
        if (indval < searchval) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    PICODBG_ASSERT(high == low);
    /* low points to the first entry greater than or equal to searchval */

    if (low < this->nrblocks) {
        indval = klex_getSearchIndexVal(this, low);
        if (indval > searchval) {
            low--;
            /* if there are identical elements in the search index we have
               to move to the first one */
            if (low > 0) {
                indval = klex_getSearchIndexVal(this, low);
                while (indval == klex_getSearchIndexVal(this, low-1)) {
                    low--;
                }
            }
        }
    } else {
        low = this->nrblocks - 1;
    }

#if defined(PICO_DEBUG)
    {
        picoos_uint32 pos = low * PICOKLEX_LEX_SIE_SIZE;
        PICODBG_DEBUG(("binary search result is %c%c%c (%d)",
                       this->searchind[pos], this->searchind[pos + 1],
                       this->searchind[pos + 2], low));
    }
#endif

    return (picoos_uint16) low;
}


/* Determine number of adjacent lexblocks containing entries for
   the same grapheme search prefix (identified by search index). */

static picoos_uint16 klex_getLexblockRange(const klex_SubObj this,
                                           picoos_uint16 index)
{
    picoos_uint16 count;
    picoos_uint32 sval1, sval2;

    sval1 = klex_getSearchIndexVal(this, index);

#if defined(PICO_DEBUG)
    /* 'index' must point to first lexblock of its kind */
    if (index > 0) {
        sval2 = klex_getSearchIndexVal(this, index - 1);
        PICODBG_ASSERT(sval1 != sval2);
    }
#endif

    index++;
    sval2 = klex_getSearchIndexVal(this, index);

    count = 1;
    while (sval1 == sval2) {
        count++;
        index++;
        sval2 = klex_getSearchIndexVal(this, index);
    }

    return count;
}


/* ************************************************************/
/* functions on single lexblock */
/* ************************************************************/

static picoos_int8 klex_lexMatch(picoos_uint8 *lexentry,
                                 const picoos_uint8 *graph,
                                 const picoos_uint16 graphlen) {
    picoos_uint8 i;
    picoos_uint8 lexlen;
    picoos_uint8 *lexgraph;

    lexlen = lexentry[0] - 1;
    lexgraph = &(lexentry[1]);
    for (i=0; (i<graphlen) && (i<lexlen); i++) {
        PICODBG_TRACE(("%d|%d  graph|lex: %c|%c", graphlen, lexlen,
                       graph[i], lexgraph[i]));
        if (lexgraph[i] < graph[i]) {
            return -1;
        } else if (lexgraph[i] > graph[i]) {
            return 1;
        }
    }
    if (graphlen == lexlen) {
        return 0;
    } else if (lexlen < graphlen) {
        return -1;
    } else {
        return 1;
    }
}


static void klex_setLexResult(const picoos_uint8 *lexentry,
                              const picoos_uint32 lexpos,
                              picoklex_lexl_result_t *lexres) {
    picoos_uint8 i;

    /* check if :G2P */
    if ((2 < (lexentry[lexentry[0]])) && ((lexentry[lexentry[0] + 2]) == PICOKLEX_NEEDS_G2P)) {
        /* set pos */
        lexres->posind[0] = lexentry[lexentry[0] + 1];
        /* set rest */
        lexres->phonfound = FALSE;
        lexres->posindlen = 1;
        lexres->nrres = 1;
        PICODBG_DEBUG(("result %d :G2P", lexres->nrres));
    } else {
        i = lexres->nrres * (PICOKLEX_POSIND_SIZE);
        lexres->posindlen += PICOKLEX_POSIND_SIZE;
        lexres->phonfound = TRUE;
        /* set pos */
        lexres->posind[i++] = lexentry[lexentry[0] + 1];
        /* set ind, PICOKLEX_IND_SIZE */
        lexres->posind[i++] = 0x000000ff & (lexpos);
        lexres->posind[i++] = 0x000000ff & (lexpos >>  8);
        lexres->posind[i]   = 0x000000ff & (lexpos >> 16);
        lexres->nrres++;
        PICODBG_DEBUG(("result %d", lexres->nrres));
    }
}


static void klex_lexblockLookup(klex_SubObj this,
                                const picoos_uint32 lexposStart,
                                const picoos_uint32 lexposEnd,
                                const picoos_uint8 *graph,
                                const picoos_uint16 graphlen,
                                picoklex_lexl_result_t *lexres) {
    picoos_uint32 lexpos;
    picoos_int8 rv;

    lexres->nrres = 0;

    lexpos = lexposStart;
    rv = -1;
    while ((rv < 0) && (lexpos < lexposEnd)) {

        rv = klex_lexMatch(&(this->lexblocks[lexpos]), graph, graphlen);

        if (rv == 0) { /* found */
            klex_setLexResult(&(this->lexblocks[lexpos]), lexpos, lexres);
            if (lexres->phonfound) {
                /* look for more results, up to MAX_NRRES, don't even
                   check if more results would be available */
                while ((lexres->nrres < PICOKLEX_MAX_NRRES) &&
                       (lexpos < lexposEnd)) {
                    lexpos += this->lexblocks[lexpos];
                    lexpos += this->lexblocks[lexpos];
                    /* if there are no more entries in this block, advance
                       to next block by skipping all zeros */
                    while ((this->lexblocks[lexpos] == 0) &&
                           (lexpos < lexposEnd)) {
                        lexpos++;
                    }
                    if (lexpos < lexposEnd) {
                        if (klex_lexMatch(&(this->lexblocks[lexpos]), graph,
                                          graphlen) == 0) {
                            klex_setLexResult(&(this->lexblocks[lexpos]),
                                              lexpos, lexres);
                        } else {
                            /* no more results, quit loop */
                            lexpos = lexposEnd;
                        }
                    }
                }
            } else {
                /* :G2P mark */
            }
        } else if (rv < 0) {
            /* not found, goto next entry */
            lexpos += this->lexblocks[lexpos];
            lexpos += this->lexblocks[lexpos];
            /* if there are no more entries in this block, advance
               to next block by skipping all zeros */
            while ((this->lexblocks[lexpos] == 0) && (lexpos < lexposEnd)) {
                lexpos++;
            }
        } else {
            /* rv > 0, not found, won't show up later in block */
        }
    }
}


/* ************************************************************/
/* lexicon lookup functions */
/* ************************************************************/

picoos_uint8 picoklex_lexLookup(const picoklex_Lex this,
                                const picoos_uint8 *graph,
                                const picoos_uint16 graphlen,
                                picoklex_lexl_result_t *lexres) {
    picoos_uint16 lbnr, lbc;
    picoos_uint32 lexposStart, lexposEnd;
    picoos_uint8 i;
    picoos_uint8 tgraph[PICOKLEX_LEX_SIE_NRGRAPHS];
    klex_SubObj klex = (klex_SubObj) this;

    if (NULL == klex) {
        PICODBG_ERROR(("no lexicon loaded"));
        /* no exception here needed, already checked at initialization */
        return FALSE;
    }

    lexres->nrres = 0;
    lexres->posindlen = 0;
    lexres->phonfound = FALSE;

    for (i = 0; i<PICOKLEX_LEX_SIE_NRGRAPHS; i++) {
        if (i < graphlen) {
            tgraph[i] = graph[i];
        } else {
            tgraph[i] = '\0';
        }
    }
    PICODBG_DEBUG(("tgraph: %c%c%c", tgraph[0],tgraph[1],tgraph[2]));

    if ((klex->nrblocks) == 0) {
        /* no searchindex, no lexblock */
        PICODBG_WARN(("no searchindex, no lexblock"));
        return FALSE;
    } else {
        lbnr = klex_getLexblockNr(klex, tgraph);
        PICODBG_ASSERT(lbnr < klex->nrblocks);
        lbc = klex_getLexblockRange(klex, lbnr);
        PICODBG_ASSERT((lbc >= 1) && (lbc <= klex->nrblocks));
    }
    PICODBG_DEBUG(("lexblock nr: %d (#%d)", lbnr, lbc));

    lexposStart = lbnr * PICOKLEX_LEXBLOCK_SIZE;
    lexposEnd = lexposStart + lbc * PICOKLEX_LEXBLOCK_SIZE;

    PICODBG_DEBUG(("lookup start, lexpos range %d..%d", lexposStart,lexposEnd));
    klex_lexblockLookup(klex, lexposStart, lexposEnd, graph, graphlen, lexres);
    PICODBG_DEBUG(("lookup done, %d found", lexres->nrres));

    return (lexres->nrres > 0);
}


picoos_uint8 picoklex_lexIndLookup(const picoklex_Lex this,
                                   const picoos_uint8 *ind,
                                   const picoos_uint8 indlen,
                                   picoos_uint8 *pos,
                                   picoos_uint8 **phon,
                                   picoos_uint8 *phonlen) {
    picoos_uint32 pentry;
    klex_SubObj klex = (klex_SubObj) this;

    /* check indlen */
    if (indlen != PICOKLEX_IND_SIZE) {
        return FALSE;
    }

    /* PICOKLEX_IND_SIZE */
    pentry = 0x000000ff & (ind[0]);
    pentry |= ((picoos_uint32)(ind[1]) <<  8);
    pentry |= ((picoos_uint32)(ind[2]) << 16);

    /* check ind if it is within lexblocks byte stream, if not, return FALSE */
    if (pentry >= ((picoos_uint32)klex->nrblocks * PICOKLEX_LEXBLOCK_SIZE)) {
        return FALSE;
    }

    pentry += (klex->lexblocks[pentry]);
    *phonlen = (klex->lexblocks[pentry++]) - 2;
    *pos = klex->lexblocks[pentry++];
    *phon = &(klex->lexblocks[pentry]);

    PICODBG_DEBUG(("pentry: %d, phonlen: %d", pentry, *phonlen));
    return TRUE;
}

#ifdef __cplusplus
}
#endif


/* end */
