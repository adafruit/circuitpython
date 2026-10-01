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
 * @file picotrns.h
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

/** @addtogroup picotrns
 *
 * Conventions:
 *
 * - The input to the transducer is a list of pos/sym pairs, where pos are arbitrary position markers
 * - All positions are allowed on input (in particular all those coming as an output of a previous transduction)
 * - A phone sequence to be transduced has to begin with PICOKNOW_PHON_START_ID and end with PICOKNOW_PHON_TERM_ID
 *   These special symbols are kept in the transduction output (as first and last symbol)
 * - Symbols inserted by the transduction process allways get their position marker pos=PICOTRNS_POS_INSERT
 * - The order of positions on output must be the same as that on input, i.e. apart from inserted pairs, the
 *   output position sequence must be a sub-sequence of the input position sequence.
 * - Inserted symbols are allways preceded by a positioned pos/sym pair, e.g.
 *   if the sequence pos1/sym1, pos2/sym2 should be tranduced to x/sym3, y/sym4, z/sym5, then x must be pos1 or pos2
 *   and not PICOTRNS_POS_INSERT
 *
 *   For lingware developers: Insertions are always interpreted "to the right"
 *     - E.g.: The original sequence is phon1 , command , phon2
 *          - The input to the transducer is then  pos1/phon1 , pos2/phon2
 *          - The output is pos1/phon1'  -1/phon_ins pos2/phon2'  [assuming -1 is the special insertion pos]
 *     - Then the new sequence will be recomposed as phon1' , phon_ins , command , phon2'  [note position of command!]
 *     - To overwrite this behaviour, rules must be formulated such that the transduction output is
 *     pos1/phon1'  pos2/phon_ins  -1/phon2'
 */
#ifndef PICOTRNS_H_
#define PICOTRNS_H_

#include "picoos.h"
#include "picokfst.h"
#include "picoktab.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#define PICOTRNS_MAX_NUM_POSSYM 255

#define PICOTRNS_POS_INSERT   (picoos_int16) -1    /* position returned by transducer to mark symbols inserted by the transducer */
#define PICOTRNS_POS_INVALID  (picoos_int16) -2    /* value to mark an invalid (e.g. uninitiated) position */
#define PICOTRNS_POS_IGNORE   (picoos_int16) -3    /* value to mark a pos/sym pair to be ignored (e.g. start/term symbols only used by the transducer) */


typedef struct picotrns_possym {
    picoos_int16 pos;
    picoos_int16 sym;
} picotrns_possym_t;

picoos_uint8 picotrns_unplane(picoos_int16 symIn, picoos_uint8 * plane);


#if defined(PICO_DEBUG)

void PICOTRNS_PRINTSYM(picoknow_KnowledgeBase kbdbg, picoos_int16 insym);

void PICOTRNS_PRINTSYMSEQ(picoknow_KnowledgeBase kbdbg, const picotrns_possym_t seq[], const picoos_uint16 seqLen);

void picotrns_printSolution(const picotrns_possym_t outSeq[], const picoos_uint16 outSeqLen);

#else
#define PICOTRNS_PRINTSYM(x,y)
#define PICOTRNS_PRINTSYMSEQ(x,y,z)
#define picotrns_printSolution NULL
#endif


typedef struct picotrns_altDesc * picotrns_AltDesc;


picotrns_AltDesc picotrns_allocate_alt_desc_buf(picoos_MemoryManager mm, picoos_uint32 maxByteSize, picoos_uint16 * numAltDescs);

void picotrns_deallocate_alt_desc_buf(picoos_MemoryManager mm, picotrns_AltDesc * altDescBuf);


/* type of function for printing transduction solutions;
   only for testing purposes in transduction mode where all solutions
   are produced */
typedef void picotrns_printSolutionFct(const picotrns_possym_t outSeq[], const picoos_uint16 outSeqLen);



/** overall transduction; transduces 'inSeq' with 'inSeqLen' elements
   to '*outSeqLen' elements in 'outSeq';
 *
 * @param fst the finite-state transducer used for transduction
 * @param firstSolOnly determines whether only the first solution (usually)
   or all solutions should be produced (for testing); only the last found
   solution is returned in 'outSeq';
 * @param printSolution if not NULL, every found solution is displayed using
   the given function
 * @param inSeq the input sequence
 * @param inSeqLen the input sequence length
 * @retval outSeq the output sequence
 * @retval outSeqLen the output sequence length
 * @param maxOutSeqLen   must provide the maximum length of 'outSeq'
 * @param altDescBuf must provide a working array of length 'maxAltDescLen'
 * @param maxAltDescLen should be chosen at least 'maxOutSeqLen' + 1
 * @retval nrSteps returns the overall internal number of iterative steps done
 * @return status of the transduction: PICO_OK, if transduction successful
   @note if 'outSeq' or 'altDesc' are too small to hold a solution,
   an error occurs and the input is simply transfered to the output
   (up to maximum possible length)
 */
extern pico_status_t picotrns_transduce (picokfst_FST fst, picoos_bool firstSolOnly,
                                         picotrns_printSolutionFct printSolution,
                                         const picotrns_possym_t inSeq[], picoos_uint16 inSeqLen,
                                         picotrns_possym_t outSeq[], picoos_uint16 * outSeqLen, picoos_uint16 maxOutSeqLen,
                                         picotrns_AltDesc altDescBuf, picoos_uint16 maxAltDescLen,
                                         picoos_uint32 *nrSteps);



/* transduce 'inSeq' into 'outSeq' 'inSeq' has to be terminated with the id for symbol '#'. 'outSeq' is terminated in the same way. */
/*
pico_status_t picotrns_transduce_sequence(picokfst_FST fst, const picotrns_possym_t inSeq[], picoos_uint16 inSeqLen,
        picotrns_possym_t outSeq[], picoos_uint16 * outSeqLen);
*/

/* copy elements from inSeq to outSeq, ignoring elements with epsilon symbol */
pico_status_t picotrns_eliminate_epsilons(const picotrns_possym_t inSeq[], picoos_uint16 inSeqLen,
        picotrns_possym_t outSeq[], picoos_uint16 * outSeqLen, picoos_uint16 maxOutSeqLen);

/* copy elements from inSeq to outSeq, inserting syllable separators in some trivial way.
 * inSeq is assumed to be at most, outSeq at least of size PICOTRNS_MAX_NUM_POSSYM  */
pico_status_t picotrns_trivial_syllabify(picoktab_Phones phones,
        const picotrns_possym_t inSeq[], const picoos_uint16 inSeqLen,
        picotrns_possym_t outSeq[], picoos_uint16 * outSeqLen, picoos_uint16 maxOutSeqLen);


/**  object   : SimpleTransducer
 *   shortcut : st
 *
 */
typedef struct picotrns_simple_transducer * picotrns_SimpleTransducer;

picotrns_SimpleTransducer picotrns_newSimpleTransducer(picoos_MemoryManager mm,
                                              picoos_Common common,
                                              picoos_uint16 maxAltDescLen);

pico_status_t picotrns_disposeSimpleTransducer(picotrns_SimpleTransducer * this,
        picoos_MemoryManager mm);

pico_status_t  picotrns_stInitialize(picotrns_SimpleTransducer transducer);

pico_status_t picotrns_stAddWithPlane(picotrns_SimpleTransducer this, picoos_char * inStr, picoos_uint8 plane);

pico_status_t picotrns_stTransduce(picotrns_SimpleTransducer this, picokfst_FST fst);

pico_status_t picotrns_stGetSymSequence(
        picotrns_SimpleTransducer this,
        picoos_uint8 * outputSymIds,
        picoos_uint32 maxOutputSymIds);





#ifdef __cplusplus
}
#endif

#endif /*PICOTRNS_H_*/
