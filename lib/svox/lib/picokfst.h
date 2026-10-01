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
 * @file picokfst.h
 *
 * FST knowledge loading and access
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */
#ifndef PICOKFST_H_
#define PICOKFST_H_

#include "picodefs.h"
#include "picodbg.h"
#include "picoos.h"
#include "picoknow.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

typedef picoos_int16 picokfst_symid_t; /* type of symbol identifiers */
typedef picoos_int16 picokfst_class_t; /* type of symbol pair classes */
typedef picoos_int16 picokfst_state_t; /* type of states */

#define PICOKFST_SYMID_EPS    (picokfst_symid_t)   0   /* epsilon symbol id */
#define PICOKFST_SYMID_ILLEG  (picokfst_symid_t)  -1   /* illegal symbol id */

/**
 * @addtogroup picokfst
 *
 * Mapping of values to FST symbol id (relevant for compiling the FST) \n
 * Value                   FST symbol id                    \n
 * --------------------------------------                    \n
 * phoneme_id      ->      phoneme_id     +  256 *  PICOKFST_PLANE_PHONEMES    \n
 * accentlevel_id  ->      accentlevel_id +  256 *  PICOKFST_PLANE_ACCENTS    \n
 * POS_id          ->      POS_id         +  256 *  PICOKFST_PLANE_POS        \n
 * pb_strength_id  ->      pb_strength_id +  256 *  PICOKFST_PLANE_PB_STRENGTHS    \n
 * phon_term_id    ->      phon_term_id   +  256 *  PICOKFST_PLANE_INTERN    \n
*/
enum picokfst_symbol_plane {
    PICOKFST_PLANE_PHONEMES = 0,       /* phoneme plane */
    PICOKFST_PLANE_ASCII = 1,          /* "ascii" plane (values > 127 may be used internally) */
    PICOKFST_PLANE_XSAMPA = 2,         /* x-sampa primitives plane (pico-specific table) */
    PICOKFST_PLANE_ACCENTS = 4,        /* accent plane */
    PICOKFST_PLANE_POS = 5,            /* part of speech plane */
    PICOKFST_PLANE_PB_STRENGTHS = 6,   /* phrase boundary strength plane */
    PICOKFST_PLANE_INTERN = 7          /* internal plane, e.g. phonStartId, phonTermId */
};

/* to be used as bit set, e.g.
 * picoos_uint8 transductionMode = PICOKFST_TRANSMODE_NEWSYMS | PICOKFST_TRANSMODE_POSUSED;
 */
enum picofst_transduction_mode {
    PICOKFST_TRANSMODE_NEWSYMS = 1, /* e.g. {#WB},{#PB-S},{#PB-W},{#ACC0},{#ACC1},{#ACC2},{#ACC3}, */
    PICOKFST_TRANSMODE_POSUSED = 2 /* FST contains Part Of Speech symbols */

};


/* ************************************************************/
/* function to create specialized kb, */
/* to be used by knowledge layer (picorsrc) only */
/* ************************************************************/

/* calculates a small number of data (e.g. addresses) from kb for fast access.
 * This data is encapsulated in a picokfst_FST that can later be retrieved
 * with picokfst_getFST. */
pico_status_t picokfst_specializeFSTKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common);


/* ************************************************************/
/* FST type and getFST function */
/* ************************************************************/

/* FST type */
typedef struct picokfst_fst * picokfst_FST;

/* return kb FST for usage in PU */
picokfst_FST picokfst_getFST(picoknow_KnowledgeBase this);


/* ************************************************************/
/* FST access methods */
/* ************************************************************/

/* returns transduction mode specified with rule sources;
   result to be interpreted as set of picofst_transduction_mode */
picoos_uint8 picokfst_kfstGetTransductionMode(picokfst_FST this);

/* returns number of states and number of pair classes in FST;
   legal states are 1..nrStates, legal classes are 1..nrClasses */
void picokfst_kfstGetFSTSizes (picokfst_FST this, picoos_int32 *nrStates, picoos_int32 *nrClasses);

/* starts search for all pairs with input symbol 'inSym'; '*inSymFound' returns whether
   such pairs exist at all; '*searchState' returns a search state to be used in
   subsequent calls to function 'picokfst_kfstGetNextPair', which must be used
   to get the symbol pairs */
void picokfst_kfstStartPairSearch (picokfst_FST this, picokfst_symid_t inSym,
                                          picoos_bool * inSymFound, picoos_int32 * searchState);

/* gets next pair for input symbol specified with preceding call to 'picokfst_kfstStartPairSearch';
   '*searchState' maintains the search state, 'pairFound' returns whether any more pair was found,
   '*outSym' returns the output symbol of the found pair, and '*pairClass' returns the
   transition class of the found symbol pair */
void picokfst_kfstGetNextPair (picokfst_FST this, picoos_int32 * searchState,
                                      picoos_bool * pairFound,
                                      picokfst_symid_t * outSym, picokfst_class_t * pairClass);

/* attempts to do FST transition from state 'startState' with pair class 'transClass';
   if such a transition exists, 'endState' returns the end state of the transition (> 0),
   otherwise 'endState' returns <= 0 */
void picokfst_kfstGetTrans (picokfst_FST this, picokfst_state_t startState, picokfst_class_t transClass,
                                   picokfst_state_t * endState);

/* starts search for all pairs with input epsilon symbol and all correponding
   FST transitions starting in state 'startState'; to be used for fast
   computation of epsilon closures;
   '*inEpsTransFound' returns whether any such transition was found at all;
   if so, '*searchState' returns a search state to be used in subsequent calls
   to 'picokfst_kfstGetNextInEpsTrans' */
void picokfst_kfstStartInEpsTransSearch (picokfst_FST this, picokfst_state_t startState,
                                                picoos_bool * inEpsTransFound, picoos_int32 * searchState);

/* gets next FST transition with a pair with empty input symbol starting from a state
   previoulsy specified in 'picokfst_kfstStartInEpsTransSearch';
   '*searchState' maintains the search state, '*inEpsTransFound' returns
   whether a new transition with input epsilon was found, '*outSym 'returns
   the output symbol of the found pair, and '*endState' returns the end state
   of the found transition with that pair */
void picokfst_kfstGetNextInEpsTrans (picokfst_FST this, picoos_int32 * searchState,
                                            picoos_bool * inEpsTransFound,
                                            picokfst_symid_t * outSym, picokfst_state_t * endState);

/* returns whether 'state' is an accepting state of FST; originally, only
   state 1 was an accepting state; however, in order to remove the need to
   always do a last transition with a termination symbol pair, this function
   defines a state as an accepting state if there is transition to state 1
   with the terminator symbol pair */
picoos_bool picokfst_kfstIsAcceptingState (picokfst_FST this, picokfst_state_t state);

#ifdef __cplusplus
}
#endif


#endif /*PICOKFST_H_*/
