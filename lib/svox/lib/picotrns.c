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
 * @file picotrns.c
 *
 * fst processing
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
/* #include "picodata.h" */
/* #include "picoknow.h" */
#include "picoktab.h"
#include "picokfst.h"
#include "picotrns.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif



picoos_uint8 picotrns_unplane(picoos_int16 symIn, picoos_uint8 * plane) {
    if (symIn < 0) {
        (*plane) = 0;
        return (picoos_uint8) symIn;
    } else {
        (*plane) = symIn >> 8;
        return (picoos_uint8) (symIn & 0xFF);
    }
}

#if defined(PICO_DEBUG)

void PICOTRNS_PRINTSYM1(picoknow_KnowledgeBase kbdbg, picoos_int16 insym, picoos_uint8 phonemic)
{
#include "picokdbg.h"
    picoos_int16 sym;
    picoos_uint8 plane;
    picokdbg_Dbg dbg = (NULL == kbdbg) ? NULL :  picokdbg_getDbg(kbdbg);
    sym = picotrns_unplane(insym, &plane);
    switch (plane) {
        case PICOKFST_PLANE_PHONEMES: /* phones */
            if ((NULL == dbg) || !phonemic) {
                PICODBG_INFO_MSG((" %c", sym));
            } else {
                PICODBG_INFO_MSG((" %s", picokdbg_getPhoneSym(dbg, (picoos_uint8) sym)));
            }
            break;
        case PICOKFST_PLANE_ACCENTS: /* accents */
            PICODBG_INFO_MSG((" {A%c}", sym));
            break;
        case PICOKFST_PLANE_XSAMPA: /* xsampa symbols */
            PICODBG_INFO_MSG((" {XS:(%i)}", sym));
            break;
        case PICOKFST_PLANE_POS: /* part of speech */
            PICODBG_INFO_MSG((" {P:%d}", sym));
            break;
        case PICOKFST_PLANE_PB_STRENGTHS: /* phrases */
            if (sym == 48) {
                PICODBG_INFO_MSG((" {WB}", sym));
            } else if (sym == 115) {
                PICODBG_INFO_MSG((" {P0}", sym));
            } else {
                PICODBG_INFO_MSG((" {P%c}", sym));
            }
            break;
        case PICOKFST_PLANE_INTERN: /* intern */
            PICODBG_INFO_MSG((" [%c]", sym));
            break;
    }
}

void PICOTRNS_PRINTSYM(picoknow_KnowledgeBase kbdbg, picoos_int16 insym)
{
    PICOTRNS_PRINTSYM1(kbdbg,insym,1);
}

void PICOTRNS_PRINTSYMSEQ1(picoknow_KnowledgeBase kbdbg, const picotrns_possym_t seq[], const picoos_uint16 seqLen,
                           picoos_uint8 phonemic) {
    picoos_uint16 i;
    for (i=0; i<seqLen; i++) {
        PICOTRNS_PRINTSYM1(kbdbg, seq[i].sym, phonemic);
    }
}

void PICOTRNS_PRINTSYMSEQ(picoknow_KnowledgeBase kbdbg, const picotrns_possym_t seq[], const picoos_uint16 seqLen) {
    PICOTRNS_PRINTSYMSEQ1(kbdbg,seq, seqLen, 1);
}

void picotrns_printSolution(const picotrns_possym_t outSeq[], const picoos_uint16 outSeqLen)
{
    PICODBG_INFO_CTX();
    PICODBG_INFO_MSG(("solution: "));
        PICOTRNS_PRINTSYMSEQ(NULL, outSeq, outSeqLen);
    PICODBG_INFO_MSG(("\n"));
}

void picotrns_printSolutionAscii(const picotrns_possym_t outSeq[], const picoos_uint16 outSeqLen)
{
    PICODBG_INFO_CTX();
    PICODBG_INFO_MSG(("solution: "));
        PICOTRNS_PRINTSYMSEQ1(NULL, outSeq, outSeqLen,0);
    PICODBG_INFO_MSG(("\n"));
}

#endif




/* * +CT+ ***/
struct picotrns_transductionState {
    picoos_uint16 phase;   /* transduction phase:
                              0 = before start
                              1 = before regular recursion step
                              2 = before finish
                              3 = after finish */
    picoos_uint32 nrSol;   /* nr of solutions so far */
    picoos_int16  recPos;  /* recursion position; must be signed! */
};

typedef struct picotrns_altDesc {
    picokfst_state_t startFSTState;   /**< starting FST state in current recursion position */
    picoos_int32     inPos;           /**< corresponding position in input string */
    picokfst_state_t altState;        /**< state of alternatives search;
                                         - 0 = before pair search
                                         - 1 = search state is a valid pair search state
                                         - 2 = before inEps search
                                         - 3 = search state is a valid inEps trans search state
                                         - 4 = no more alternatives */
    picoos_int32     searchState;     /**< pair search state or inEps trans search state */
    picokfst_symid_t altOutSym;       /**< current output symbol at this recursion position */
    picoos_int32     altOutRefPos;    /**< output reference position at this recursion position */
} picotrns_altDesc_t;


picotrns_AltDesc picotrns_allocate_alt_desc_buf(picoos_MemoryManager mm, picoos_uint32 maxByteSize, picoos_uint16 * numAltDescs)
{
    picotrns_AltDesc buf;
    (*numAltDescs) = (picoos_uint32) (maxByteSize / sizeof(picotrns_altDesc_t));
    buf =  (picotrns_AltDesc) picoos_allocate(mm, (*numAltDescs) * sizeof(picotrns_altDesc_t));
    if (NULL == buf) {
        (*numAltDescs) = 0;
        return NULL;
    } else {
        return buf;
    }
}

 void picotrns_deallocate_alt_desc_buf(picoos_MemoryManager mm, picotrns_AltDesc * altDescBuf)
{
    picoos_deallocate(mm, (void *) altDescBuf);
}

/* copy elements from inSeq to outSeq, ignoring elements with epsilon symbol */
pico_status_t picotrns_eliminate_epsilons(const picotrns_possym_t inSeq[], picoos_uint16 inSeqLen,
        picotrns_possym_t outSeq[], picoos_uint16 * outSeqLen, picoos_uint16 maxOutSeqLen)
{
    picoos_uint16 i, j = 0;

    for (i=0; i < inSeqLen; i++) {
        /* it is assumed that PICOKFST_SYMID_EPS is a hardwired value and not shifted */
        if (PICOKFST_SYMID_EPS != inSeq[i].sym) {
            if (j < maxOutSeqLen) {
                outSeq[j].pos = inSeq[i].pos;
                outSeq[j].sym = inSeq[i].sym;
                j++;
            }
        }
        *outSeqLen = j;
    }
    return PICO_OK;
}


static void insertSym(picotrns_possym_t inSeq[], picoos_uint16 pos, picoos_int16 sym) {
    inSeq[pos].sym = sym;
    inSeq[pos].pos = PICOTRNS_POS_INSERT;
}

/* copy elements from inSeq to outSeq, inserting syllable separators in some trivial way.
 * inSeq is assumed to be at most PICOTRNS_MAX_NUM_POSSYM, outSeq at least of size PICOTRNS_MAX_NUM_POSSYM  */
pico_status_t picotrns_trivial_syllabify(picoktab_Phones phones,
        const picotrns_possym_t inSeq[], const picoos_uint16 inSeqLen,
        picotrns_possym_t outSeq[], picoos_uint16 * outSeqLen, picoos_uint16 maxOutSeqLen)
{
    picoos_uint16 i = 0, j = 0, out = 0, numInserted = 0;
    picoos_uint8 vowelFound = FALSE;
    picoos_uint16 accentpos = 0;
    picoos_int16 accent = 0;

    PICODBG_TRACE(("start"));


    while (i < inSeqLen) {
        /* make sure that at least one more sylSep can be inserted */
        if (inSeqLen+numInserted+1 >= maxOutSeqLen) {
            return PICO_EXC_BUF_OVERFLOW;
        }
       /* let j skip consonant cluster */
        accent = 0;
        accentpos = 0;
        while ((j < inSeqLen) && !picoktab_isSyllCarrier(phones,(picoos_uint8)inSeq[j].sym)) {
            if ((inSeq[j].sym == picoktab_getPrimstressID(phones))
                    || (inSeq[j].sym == picoktab_getPrimstressID(phones))) {
                PICODBG_TRACE(("j skipping stress symbol inSeq[%i].sym = %c", j, inSeq[j].sym));
                accent = inSeq[j].sym;
                accentpos = j;
            } else {
                PICODBG_TRACE(("j skipping consonant inSeq[%i].sym = %c", j, inSeq[j].sym));
            }
            j++;
        }
        if (j < inSeqLen) { /* j is at the start of a new vowel */
            /* copy consonant cluster (moving i) to output, insert syll separator if between vowels */
            while (i < j-1) {
                if ((accent > 0) && (i == accentpos)) {
                    PICODBG_TRACE(("skipping inSeq[%i].sym = %c (stress)", i, inSeq[i].sym));
                  i++;
                } else {
                PICODBG_TRACE(("copying inSeq[%i].sym = %c (consonant) into output buffer", i, inSeq[i].sym));
                 outSeq[out++] = inSeq[i++];
                }
            }
            if (vowelFound) { /* we're between vowels */
                PICODBG_TRACE(("inserting syllable separator into output buffer"));
                insertSym(outSeq,out++,picoktab_getSyllboundID(phones));
                if (accent > 0) {
                    insertSym(outSeq,out++,accent);
                }
                numInserted++;
            }
            if ((accent > 0) && (i == accentpos)) {
                PICODBG_TRACE(("skipping inSeq[%i].sym = %c (stress)", i, inSeq[i].sym));
              i++;
            } else {
            PICODBG_TRACE(("copying inSeq[%i].sym = %c (consonant) into output buffer", i, inSeq[i].sym));
             outSeq[out++] = inSeq[i++];
            }
            vowelFound = TRUE;
            /* now copy vowel cluster */
            while ((i < inSeqLen) && picoktab_isSyllCarrier(phones,(picoos_uint8)inSeq[i].sym)) {
                PICODBG_TRACE(("copying inSeq[%i].sym = %c (vowel) into output buffer", i, inSeq[i].sym));
                outSeq[out++] = inSeq[i++];
            }
            j = i;
        } else { /* j is at end of word or end of input */
            while (i < j) {
                PICODBG_TRACE(("copying inSeq[%i].sym = %c (consonant or stress) into output buffer", i, inSeq[i].sym));
                outSeq[out++] = inSeq[i++];
            }
        }
        *outSeqLen = out;
    }
    PICODBG_ASSERT((out == inSeqLen + numInserted));

    return PICO_OK;
}


/* ******** +CT+: full transduction procedure **********/


/* Gets next acceptable alternative for output symbol '*outSym' at current recursion position
   starting from previous alternative in 'altDesc'; possibly uses input symbol
   given by 'inSeq'/'inSeq'; returns whether alterative was found in '*found';
   if '*found', the other output values ('*outRefPos', '*endFSTstate', '*nextInPos'*)
   return the characteristics for next recursion step;
   if '*found' is false, the output values are undefined. */

static void GetNextAlternative (picokfst_FST fst, picotrns_AltDesc altDesc,
                                const picotrns_possym_t inSeq[], picoos_uint16 inSeqLen,
                                picokfst_symid_t * outSym, picoos_int32 * outRefPos,
                                picokfst_state_t * endFSTState, picoos_int32 * nextInPos, picoos_bool * found)
{

    picoos_bool inSymFound;
    picoos_bool pairFound;
    picokfst_class_t pairClass;
    picoos_bool inEpsTransFound;
    picokfst_symid_t inSym;

    (*found) = 0;
    do {
        switch (altDesc->altState) {
            case 0:   /* before pair search */
                if (altDesc->inPos < inSeqLen) {
                    inSym = inSeq[altDesc->inPos].sym;
                    if (inSym == PICOKFST_SYMID_EPS) {
                        /* very special case: input epsilon simply produces eps in output
                           without fst state change */
                        (*found) = 1;
                        (*outSym) = PICOKFST_SYMID_EPS;
                        (*outRefPos) = inSeq[altDesc->inPos].pos;
                        (*endFSTState) = altDesc->startFSTState;
                        (*nextInPos) = altDesc->inPos + 1;
                        altDesc->altState = 2;
                    } else {
                        /* start search for alternatives using input symbol */
                        picokfst_kfstStartPairSearch(fst,inSeq[altDesc->inPos].sym,& inSymFound,& altDesc->searchState);
                        if (!inSymFound) {
                            altDesc->altState = 2;
                            PICODBG_INFO_CTX();
                            PICODBG_INFO_MSG((" didnt find symbol "));
                            PICOTRNS_PRINTSYM(NULL, inSeq[altDesc->inPos].sym);
                            PICODBG_INFO_MSG(("\n"));

                        } else {
                            altDesc->altState = 1;
                        }
                    }
                } else {
                    altDesc->altState = 2;
                }
                break;
            case 1:   /* within pair search */
                picokfst_kfstGetNextPair(fst,& altDesc->searchState,& pairFound,& (*outSym),& pairClass);
                if (pairFound) {
                    picokfst_kfstGetTrans(fst,altDesc->startFSTState,pairClass,& (*endFSTState));
                    if ((*endFSTState) > 0) {
                        (*found) = 1;
                        (*outRefPos) = inSeq[altDesc->inPos].pos;
                        (*nextInPos) = altDesc->inPos + 1;
                    }
                } else {
                    /* no more pair found */
                    altDesc->altState = 2;
                }
                break;
            case 2:   /* before inEps trans search */
                picokfst_kfstStartInEpsTransSearch(fst,altDesc->startFSTState,& inEpsTransFound,& altDesc->searchState);
                if (inEpsTransFound) {
                    altDesc->altState = 3;
                } else {
                    altDesc->altState = 4;
                }
                break;
            case 3:   /* within inEps trans search */
                picokfst_kfstGetNextInEpsTrans(fst,& altDesc->searchState,& inEpsTransFound,& (*outSym),& (*endFSTState));
                if (inEpsTransFound) {
                    (*found) = 1;
                    (*outRefPos) =  PICOTRNS_POS_INSERT;
                    (*nextInPos) = altDesc->inPos;
                } else {
                    altDesc->altState = 4;
                }
                break;
            case 4:   /* no more alternatives */
                break;
        }
    } while (! ((*found) || (altDesc->altState == 4)) );  /* i.e., until (*found) || (altState == 4) */
}



/* Transfers current alternatives path stored in 'altDesc' with current path length 'pathLen'
   into 'outSeq'/'outSeqLen'. The number of solutions is incremented. */

static void NoteSolution (picoos_uint32 * nrSol, picotrns_printSolutionFct printSolution,
                          picotrns_altDesc_t altDesc[], picoos_uint16 pathLen,
                          picotrns_possym_t outSeq[], picoos_uint16 * outSeqLen, picoos_uint16 maxOutSeqLen)
{
    register picotrns_AltDesc ap;
    picoos_uint32 i;

    (*nrSol)++;
    (*outSeqLen) = 0;
    for (i = 0; i < pathLen; i++) {
        if (i < maxOutSeqLen) {
            ap = &altDesc[i];
            outSeq[i].sym = ap->altOutSym;
            outSeq[i].pos = ap->altOutRefPos;
            (*outSeqLen)++;
        }
    }
    if (pathLen > maxOutSeqLen) {
        PICODBG_WARN(("**** output symbol array too small to hold full solution\n"));
    }
    if (printSolution != NULL) {
        printSolution(outSeq,(*outSeqLen));
    }
}



/* *
    general scheme to get all solutions ("position" refers to abstract backtracking recursion depth,
    which in the current solution is equal to the output symbol position):

    "set position to first position";
    "initialize alternatives in first position";
    REPEAT
      IF "current state in current position is a solution" THEN
        "note solution";
      END;
      "get first or next acceptable alternative in current position";
      IF "acceptable alternative found" THEN
        "note alternative";
        "go to next position";
        "initialize alternatives in that position";
      ELSE
        "step back to previous position";
      END;
    UNTIL "current position is before first position"
***/


/* Initializes transduction state for further use in repeated application
   of 'TransductionStep'. */

static void StartTransduction (struct picotrns_transductionState * transductionState)
{
    (*transductionState).phase = 0;
}



/* Performs one step in the transduction of 'inSeqLen' input symbols with corresponding
   reference positions in 'inSeq'. '*transductionState' must have been
   initialized by 'StartTransduction'. Repeat calls to this procedure until '*finished' returns true.
   The output is returned in 'outSeqLen' symbols and reference positions in 'outSeq'.
   The output reference positions refer to the corresponding input reference positions.
   Inserted output symbols receive the reference position -1. If several solutions are possible,
   only the last found solution is returned.
   'altDesc' is a temporary workspace which should be at least one cell longer than 'outSeq'.
   'firstSolOnly' determines whether only the first solution should be found or if
   the search should go on to find all solutions (mainly for testing purposes).

   NOTE: current version written for use in single repetitive steps;
   could be simplified if full transduction can be done as an atomic operation */

static void TransductionStep (picokfst_FST fst, struct picotrns_transductionState * transductionState,
                              picotrns_altDesc_t altDesc[], picoos_uint16 maxAltDescLen,
                              picoos_bool firstSolOnly, picotrns_printSolutionFct printSolution,
                              const picotrns_possym_t inSeq[], picoos_uint16 inSeqLen,
                              picotrns_possym_t outSeq[], picoos_uint16 * outSeqLen, picoos_uint16 maxOutSeqLen,
                              picoos_bool * finished)
{
    register picotrns_AltDesc ap;
    picoos_int32 i;
    picokfst_state_t endFSTState;
    picoos_int32 nextInPos;
    picoos_bool found;
    picokfst_symid_t outSym;
    picoos_int32 outRefPos;
    picoos_int32 tmpRecPos;

    (*finished) = 0;
    tmpRecPos = (*transductionState).recPos;
    switch ((*transductionState).phase) {
        case 0:   /* before initialization */
            (*transductionState).nrSol = 0;

            /* check for initial solution (empty strings are always accepted) */
            if (inSeqLen == 0) {
                NoteSolution(& (*transductionState).nrSol,printSolution,altDesc,0,outSeq,outSeqLen,maxOutSeqLen);
            }

            /* initialize first recursion position */
            tmpRecPos = 0;
            ap = & altDesc[0];
            ap->startFSTState = 1;
            ap->inPos = 0;
            ap->altState = 0;
            (*transductionState).phase = 1;
            break;

        case 1:   /* before regular recursion step */
            if ((tmpRecPos < 0) || (firstSolOnly && ((*transductionState).nrSol > 0))) {
                /* end reached */
                (*transductionState).phase = 2;
            } else {
                /* not finished; do regular step */

                /* get first or next acceptable alternative in current position */
                GetNextAlternative(fst,& altDesc[tmpRecPos],inSeq,inSeqLen,& outSym,& outRefPos,& endFSTState,& nextInPos,& found);
                if (found) {
                    /* note alternative in current position */
                    ap = & altDesc[tmpRecPos];
                    ap->altOutSym = outSym;
                    ap->altOutRefPos = outRefPos;

                    /* check for solution after found alternative */
                    if ((nextInPos == inSeqLen) && picokfst_kfstIsAcceptingState(fst,endFSTState)) {
                        NoteSolution(& (*transductionState).nrSol,printSolution,altDesc,tmpRecPos+1,
                                     outSeq,outSeqLen,maxOutSeqLen);
                    }

                    /* go to next position if possible, start search for follower alternative symbols */
                    if (tmpRecPos < maxAltDescLen-1) {
                        /* got to next position */
                        tmpRecPos = tmpRecPos + 1;

                        /* initialize alternatives in new position */
                        ap = & altDesc[tmpRecPos];
                        ap->startFSTState = endFSTState;
                        ap->inPos = nextInPos;
                        ap->altState = 0;

                    } else {
                        /* do not go on due to limited path but still treat alternatives in current position */
                        PICODBG_WARN(("--- transduction path too long; may fail to find solution\n"));
                    }
                } else {  /* no more acceptable alternative found in current position */
                    /* backtrack to previous recursion */
                    tmpRecPos = tmpRecPos - 1;
                }
            }
            break;

        case 2:   /* before finish */
            if ((*transductionState).nrSol == 0) {
                PICODBG_WARN(("--- no transduction solution found, using input as output\n"));
                i = 0;
                while ((i < inSeqLen) && (i < maxOutSeqLen)) {
                    outSeq[i].sym = inSeq[i].sym;
                    outSeq[i].pos = inSeq[i].pos;
                    i++;
                }
                (*outSeqLen) = i;
            } else if ((*transductionState).nrSol > 1) {
                PICODBG_WARN(("--- more than one transducer solutions found\n"));
            }
            (*transductionState).phase = 3;
            break;

        case 3:   /* after finish */
            (*finished) = 1;
            break;
    }
    (*transductionState).recPos = tmpRecPos;
}



/* see description in header */
pico_status_t picotrns_transduce (picokfst_FST fst, picoos_bool firstSolOnly,
                                         picotrns_printSolutionFct printSolution,
                                         const picotrns_possym_t inSeq[], picoos_uint16 inSeqLen,
                                         picotrns_possym_t outSeq[], picoos_uint16 * outSeqLen, picoos_uint16 maxOutSeqLen,
                                         picotrns_AltDesc altDescBuf, picoos_uint16 maxAltDescLen,
                                         picoos_uint32 *nrSteps)
{
    struct picotrns_transductionState transductionState;
    picoos_bool finished;

#if defined(PICO_DEBUG)
    {
        picoos_uint16 i;

        PICODBG_INFO_CTX();
        PICODBG_INFO_MSG(("got input: "));
        for (i=0; i<inSeqLen; i++) {
            PICODBG_INFO_MSG((" %d", inSeq[i].sym));
        }
        PICODBG_INFO_MSG((" ("));
        PICOTRNS_PRINTSYMSEQ(NULL,inSeq,inSeqLen);
        PICODBG_INFO_MSG((")\n"));
    }
#endif
   StartTransduction(&transductionState);
    finished = 0;
    *nrSteps = 0;
    while (!finished) {
        TransductionStep(fst,&transductionState,altDescBuf,maxAltDescLen,firstSolOnly,printSolution,
                         inSeq,inSeqLen,outSeq,outSeqLen,maxOutSeqLen,&finished);
        (*nrSteps)++;
    }

    return PICO_OK;
}


/**
 * Data structure for picotrns_SimpleTransducer object.
 */
typedef struct picotrns_simple_transducer {
    picoos_Common common;
    picotrns_possym_t possymBufA[PICOTRNS_MAX_NUM_POSSYM+1];
    picotrns_possym_t possymBufB[PICOTRNS_MAX_NUM_POSSYM+1];
    picotrns_possym_t * possymBuf; /**< the buffer of the pos/sym pairs */
    picotrns_possym_t * possymBufTmp;
    picoos_uint16 possymReadPos, possymWritePos; /* next pos to read from phonBufIn, next pos to write to phonBufIn */

    /* buffer for internal calculation of transducer */
    picotrns_AltDesc altDescBuf;
    /* the number of AltDesc in the buffer */
    picoos_uint16 maxAltDescLen;
} picotrns_simple_transducer_t;


pico_status_t  picotrns_stInitialize(picotrns_SimpleTransducer transducer)
{
    transducer->possymBuf = transducer->possymBufA;
    transducer->possymBufTmp = transducer->possymBufB;
    transducer->possymReadPos = 0;
    transducer->possymWritePos = 0;
    return PICO_OK;
}
/** creates a SimpleTranducer with a working buffer of given size
 *
 * @param mm      MemoryManager handle
 * @param common  Common handle
 * @param maxAltDescLen maximal size for working buffer (in bytes)
 * @return handle to new SimpleTransducer or NULL if error
 */
picotrns_SimpleTransducer picotrns_newSimpleTransducer(picoos_MemoryManager mm,
                                              picoos_Common common,
                                              picoos_uint16 maxAltDescLen)
{
    picotrns_SimpleTransducer this;
    this = picoos_allocate(mm, sizeof(picotrns_simple_transducer_t));
    if (this == NULL) {
        picoos_deallocate(mm, (void *)&this);
        picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM, NULL, NULL);
        return NULL;
    }

    /* allocate working buffer */
    this->altDescBuf = picotrns_allocate_alt_desc_buf(mm, maxAltDescLen, &this->maxAltDescLen);
    if (this->altDescBuf == NULL) {
        picoos_deallocate(mm, (void *)&this);
        picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM, NULL, NULL);
        return NULL;
    }
    this->common = common;
    picotrns_stInitialize(this);
    return this;
}
/** disposes a SimpleTransducer
 *
 * @param this
 * @param mm
 * @return PICO_OK
 */
pico_status_t picotrns_disposeSimpleTransducer(picotrns_SimpleTransducer * this,
                                        picoos_MemoryManager mm)
{
    if (NULL != (*this)) {
        picotrns_deallocate_alt_desc_buf(mm,&(*this)->altDescBuf);
        picoos_deallocate(mm, (void *) this);
        (*this) = NULL;
    }
    return PICO_OK;
}

/** transduces the contents previously inserted via @ref picotrns_newSimpleTransducer and @ref
 *  picotrns_disposeSimpleTransducer.
 *
 * @param this
 * @param fst
 * @return
 */
pico_status_t picotrns_stTransduce(picotrns_SimpleTransducer this, picokfst_FST fst)
{
    picoos_uint16 outSeqLen;
    picoos_uint32 nrSteps;
    pico_status_t status;

    status = picotrns_transduce(fst,TRUE,NULL,
            this->possymBuf, this->possymWritePos,
            this->possymBufTmp,&outSeqLen, PICOTRNS_MAX_NUM_POSSYM,
            this->altDescBuf,this->maxAltDescLen,&nrSteps);
    if (PICO_OK != status) {
        return status;
    }
    return picotrns_eliminate_epsilons(this->possymBufTmp,outSeqLen,this->possymBuf,&this->possymWritePos,PICOTRNS_MAX_NUM_POSSYM);
}

/**
 * Add chars from NULLC-terminated string \c inStr, shifted to plane \c plane, to internal input buffer of
 *  \c transducer.
 *
 * @param this is an initialized picotrns_SimpleTransducer
 * @param inStr NULLC-terminated byte sequence
 * @param plane
 * @return PICO_OK, if all bytes fit into buffer, or PICO_EXC_BUF_OVERFLOW otherwise
 */
pico_status_t picotrns_stAddWithPlane(picotrns_SimpleTransducer this, picoos_char * inStr, picoos_uint8 plane)
{
    while ((*inStr) && (this->possymWritePos < PICOTRNS_MAX_NUM_POSSYM)) {
        this->possymBuf[this->possymWritePos].pos = PICOTRNS_POS_INSERT;
        this->possymBuf[this->possymWritePos].sym = (plane << 8) + (*inStr);
        PICODBG_DEBUG(("inserting pos/sym = %i/'%c' at pos %i",
                this->possymBuf[this->possymWritePos].pos,
                this->possymBuf[this->possymWritePos].sym,
                this->possymWritePos));
        this->possymWritePos++;
        inStr++;
    }
    if (!(*inStr)) {
        return PICO_OK;
    } else {
        return PICO_EXC_BUF_OVERFLOW;
    }
}

pico_status_t picotrns_stGetSymSequence(
        picotrns_SimpleTransducer this,
        picoos_uint8 * outputSymIds,
        picoos_uint32 maxOutputSymIds)
{
    picoos_uint8 plane;
    picoos_uint32 outputCount = 0;
    while ((this->possymReadPos < this->possymWritePos) && (outputCount < maxOutputSymIds)) {
        *outputSymIds++ = picotrns_unplane(this->possymBuf[this->possymReadPos++].sym, &plane);
        outputCount++;
    }
    *outputSymIds = NULLC;
    if (outputCount <= maxOutputSymIds) {
        return PICO_OK;
    } else {
        return PICO_EXC_BUF_OVERFLOW;
    }
}

#ifdef __cplusplus
}
#endif

/* end picotrns.c */
