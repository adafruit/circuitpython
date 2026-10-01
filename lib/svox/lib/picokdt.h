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
 * @file picokdt.h
 *
 * knowledge handling for decision trees
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#ifndef PICOKDT_H_
#define PICOKDT_H_

#include "picoos.h"
#include "picoknow.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ************************************************************/
/*
   Several specialized decision trees kb are provided by this
   knowledge handling module:

   - Part of speech prediction decision tree:     ...kdt_PosP
   - Part of speech disambiguation decision tree: ...kdt_PosD
   - Grapheme-to-phoneme decision tree:           ...kdt_G2P
   - Phrasing decision tree:                      ...kdt_PHR
   - Accentuation decision tree:                  ...kdt_ACC
   these 5 tree types may be unified in the future to a single type

   - Phono-acoustical model trees:                ...kdt_PAM
     (actually 11 trees, but all have the same characteristics and
      are instances of the same class)
*/
/* ************************************************************/


/* ************************************************************/
/* defines and functions to create specialized kb, */
/* to be used by picorsrc only */
/* ************************************************************/

typedef enum {
    PICOKDT_KDTTYPE_POSP,
    PICOKDT_KDTTYPE_POSD,
    PICOKDT_KDTTYPE_G2P,
    PICOKDT_KDTTYPE_PHR,
    PICOKDT_KDTTYPE_ACC,
    PICOKDT_KDTTYPE_PAM
} picokdt_kdttype_t;

pico_status_t picokdt_specializeDtKnowledgeBase(picoknow_KnowledgeBase this,
                                                picoos_Common common,
                                                const picokdt_kdttype_t type);


/* ************************************************************/
/* decision tree types (opaque) and get Tree functions */
/* ************************************************************/

/* decision tree types */
typedef struct picokdt_dtposp * picokdt_DtPosP;
typedef struct picokdt_dtposd * picokdt_DtPosD;
typedef struct picokdt_dtg2p  * picokdt_DtG2P;
typedef struct picokdt_dtphr  * picokdt_DtPHR;
typedef struct picokdt_dtacc  * picokdt_DtACC;
typedef struct picokdt_dtpam  * picokdt_DtPAM;

/* return kb decision tree for usage in PU */
picokdt_DtPosP picokdt_getDtPosP(picoknow_KnowledgeBase this);
picokdt_DtPosD picokdt_getDtPosD(picoknow_KnowledgeBase this);
picokdt_DtG2P  picokdt_getDtG2P (picoknow_KnowledgeBase this);
picokdt_DtPHR  picokdt_getDtPHR (picoknow_KnowledgeBase this);
picokdt_DtACC  picokdt_getDtACC (picoknow_KnowledgeBase this);
picokdt_DtPAM  picokdt_getDtPAM (picoknow_KnowledgeBase this);


/* number of attributes (= input vector size) for each tree type */
typedef enum {
    PICOKDT_NRATT_POSP = 12,
    PICOKDT_NRATT_POSD =  7,
    PICOKDT_NRATT_G2P  = 16,
    PICOKDT_NRATT_PHR  =  8,
    PICOKDT_NRATT_ACC  = 13,
    PICOKDT_NRATT_PAM  = 60
} kdt_nratt_t;


/* ************************************************************/
/* decision tree classification result type */
/* ************************************************************/

typedef struct {
    picoos_uint8 set;    /* TRUE if class set, FALSE otherwise */
    picoos_uint16 class;
} picokdt_classify_result_t;


/* maximum number of output values the tree output is mapped to */
#define PICOKDT_MAXSIZE_OUTVEC 8

typedef struct {
    picoos_uint8 nr;    /* 0 if no class set, nr of values set otherwise */
    picoos_uint16 classvec[PICOKDT_MAXSIZE_OUTVEC];
} picokdt_classify_vecresult_t;


/* ************************************************************/
/* decision tree functions */
/* ************************************************************/

/* constructInVec:
   for every tree type there is a constructInVec function to construct
   the size-optimized input vector for the tree using the input map
   tables that are part of the decistion tree knowledge base. The
   constructed input vector is stored in the tree object (this->invec
   and this->inveclen) and will be used in the following call to the
   classify function.

   classify:
   for every tree type there is a classify function to apply the
   decision tree to the previously constructed input vector. The
   size-optimized, encoded output is stored in the tree object
   (this->outval) and will be used in the following call to the
   decompose function. Where needed (hitory attribute) the direct tree
   output is returned by the classify function in a variable.

   decomposeOutClass:
   for every tree type there is a decompose function to decompose the
   size-optimized, encoded tree output and map it to the outside the
   tree usable class value.
*/


/* ************************************************************/
/* decision tree defines */
/* ************************************************************/

/* to construct the input vectors several hard-coded values are used
   to handle attributes that, at the given position, are outside the
   context. */

/* graph attributes: values to be used if the graph attribute is
   outside the grapheme string (ie. word) */
#define PICOKDT_OUTSIDEGRAPH_DEFCH     (picoos_uint8)'\x30'   /* ascii "0" */
#define PICOKDT_OUTSIDEGRAPH_DEFSTR  (picoos_uint8 *)"\x30"   /* ascii "0" */
#define PICOKDT_OUTSIDEGRAPH_DEFLEN  1

/* graph attributes (special case for g2p): values to be used if the
   graph attribute is directly outside the grapheme string (ie. at the
   word boundary word). Use PICOKDT_OUTSIDEGRAPH_DEF* if further
   outside. */
#define PICOKDT_OUTSIDEGRAPH_EOW_DEFCH     (picoos_uint8)'\x31' /* ascii "1" */
#define PICOKDT_OUTSIDEGRAPH_EOW_DEFSTR  (picoos_uint8 *)"\x31" /* ascii "1" */
#define PICOKDT_OUTSIDEGRAPH_EOW_DEFLEN  1

/* byte and word type attributes: value to be used if a byte or word
   attribute is outside the context, e.g. for POS */
#define PICOKDT_EPSILON  7

/* byte and word type attributes: for attribute with history info a
   'zero' value is needed when starting the sequence of predictions.
   Use the following value to initialize history. Note that the direct
   tree outputs (not mapped with output map table) of previous
   predictions need to be used when constructing the input vector for
   a following prediction. This direct tree output will then be mapped
   together with the rest of the input vector by the input map
   table. */
#define PICOKDT_HISTORY_ZERO  30000


/* ************************************************************/
/* decision tree POS prediction (PosP) functions */
/* ************************************************************/

/* construct a POS prediction input vector
   tree input vector: 0-3 prefix UTF8 graphemes
                      4-9 suffex UTF8 graphemes
                       10 special grapheme existence flag (TRUE/FALSE)
                       11 number of graphemes
   graph:         the grapheme string of the word for wich POS will be predicted
   graphlen:      length of graph in number of bytes
   specgraphflag: existence of a special grapheme boolean
   returns:       TRUE if okay, FALSE otherwise
   note:          use PICOKDT_OUTSIDEGRAPH* for att values outside context
*/
picoos_uint8 picokdt_dtPosPconstructInVec(const picokdt_DtPosP this,
                                          const picoos_uint8 *graph,
                                          const picoos_uint16 graphlen,
                                          const picoos_uint8 specgraphflag);


/* classify a previously constructed input vector using tree 'this'
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPosPclassify(const picokdt_DtPosP this);

/* decompose the tree output and return the class in dtres
   dtres:         POS or POSgroup ID classification result
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPosPdecomposeOutClass(const picokdt_DtPosP this,
                                             picokdt_classify_result_t *dtres);


/* ************************************************************/
/* decision tree POS disambiguation (PosD) functions */
/* ************************************************************/

/* construct a POS disambiguation input vector (run in left-to-right mode)
   tree input vector: 0-2 POS or POSgroup for each of the three previous words
                        3 POSgroup for current word
                      4-6 POS or POSgroup (can be history) for each of
                          the three following words
   pre3 - pre1:     POSgroup or POS for the previous three words
   src:             POSgroup of current word (if unique POS no posdisa possible)
   fol1 - fol3:     POS or history for the following three words (the more
                    complicated the better... :-(  NEEDS TO BE uint16
   ishist1-ishist3: flag to indicate if fol1-3 are predicted tree
                    output values (history) or the HISTORY_ZERO (TRUE)
                    or an already unambiguous POS (FALSE)
   returns:         TRUE if okay, FALSE otherwise
   note:            use PICOKDT_EPSILON for att values outside context,
                    if POS in fol* unique use this POS instead of real
                    history, use reverse output mapping in these cases
*/
picoos_uint8 picokdt_dtPosDconstructInVec(const picokdt_DtPosD this,
                                          const picoos_uint16 * input);


/* classify a previously constructed input vector using tree 'this'
   treeout:       direct tree output value
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPosDclassify(const picokdt_DtPosD this,
                                    picoos_uint16 *treeout);

/* decompose the tree output and return the class in dtres
   dtres:         POS classification result
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPosDdecomposeOutClass(const picokdt_DtPosD this,
                                             picokdt_classify_result_t *dtres);

/* convert (unique) POS index into corresponding tree output index */
picoos_uint8 picokdt_dtPosDreverseMapOutFixed(const picokdt_DtPosD this,
                                          const picoos_uint16 inval,
                                          picoos_uint16 *outval,
                                          picoos_uint16 *outfallbackval);

/* ************************************************************/
/* decision tree grapheme-to-phoneme (G2P) functions */
/* ************************************************************/

/* construct a G2P input vector (run in right-to-left mode)
   tree input vector: 0-8 the 4 previous, current, and 4 following graphemes
                        9 POS
                    10-11 vowel count and vowel ID
                       12 primary stress flag (TRUE/FALSE)
                    13-15 the three following phones predicted
   graph:         the grapheme string used to determine invec[0:8]
   graphlen:      length of graph in number of bytes
   count:         the grapheme number for which invec will be constructed [0..]
   pos:           the part of speech of the word
   nrvow          number of vowel-like graphemes in graph if vowel,
                  set to 0 otherwise
   ordvow         order of 'count' vowel in graph if vowel,
                  set to 0 otherwise
   primstressflag: flag indicating if primary stress was already predicted
   phonech1-3:    the three following phon chunks predicted (right-to-left)
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtG2PconstructInVec(const picokdt_DtG2P this,
                                         const picoos_uint8 *graph,
                                         const picoos_uint16 graphlen,
                                         const picoos_uint8 count,
                                         const picoos_uint8 pos,
                                         const picoos_uint8 nrvow,
                                         const picoos_uint8 ordvow,
                                         picoos_uint8 *primstressflag,
                                         const picoos_uint16 phonech1,
                                         const picoos_uint16 phonech2,
                                         const picoos_uint16 phonech3);

/* classify a previously constructed input vector using tree 'this'
   treeout:       direct tree output value
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtG2Pclassify(const picokdt_DtG2P this,
                                   picoos_uint16 *treeout);

/* decompose the tree output and return the class vector in dtvres
   dtvres:        phones vector classification result
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtG2PdecomposeOutClass(const picokdt_DtG2P this,
                                  picokdt_classify_vecresult_t *dtvres);


/* ************************************************************/
/* decision tree phrasing (PHR) functions */
/* ************************************************************/

/* construct a PHR input vector (run in right-to-left mode)
   tree input vector: 0-1 POS for each of the two previous words
                        2 POS for current word
                      3-4 POS for each of the two following words
                        5 nr words left
                        6 nr words right
                        7 nr syllables right
   pre2 - pre1:     POS for the previous two words
   src:             POS of current word
   fol1 - fol2:     POS for the following two words
   nrwordspre:      number of words left (previous) of current word
   nrwordsfol:      number of words right (following) of current word,
                    incl. current word, up to next BOUND (also
                    considering previously predicted PHR2/3)
   nrsyllsfol:      number of syllables right (following) of current word,
                    incl. syllables of current word, up to next BOUND
                    (also considering previously predicted PHR2/3)
   returns:         TRUE if okay, FALSE otherwise
   note:            use PICOKDT_EPSILON for att values outside context
*/
picoos_uint8 picokdt_dtPHRconstructInVec(const picokdt_DtPHR this,
                                         const picoos_uint8 pre2,
                                         const picoos_uint8 pre1,
                                         const picoos_uint8 src,
                                         const picoos_uint8 fol1,
                                         const picoos_uint8 fol2,
                                         const picoos_uint16 nrwordspre,
                                         const picoos_uint16 nrwordsfol,
                                         const picoos_uint16 nrsyllsfol);

/* classify a previously constructed input vector using tree 'this'
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPHRclassify(const picokdt_DtPHR this);

/* decompose the tree output and return the class vector in dtres
   dtres:         phrasing classification result
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPHRdecomposeOutClass(const picokdt_DtPHR this,
                                            picokdt_classify_result_t *dtres);


/* ************************************************************/
/* decision tree accentuation (ACC) functions */
/* ************************************************************/

/* construct an ACC input vector (run in right-to-left mode)
   tree input vector: 0-1 POS for each of the two previous words
                        2 POS for current word
                      3-4 POS for each of the two following words
                      5-6 history values (already predicted following)
                        7 nr words left (previous) to any bound
                        8 nr syllables left to any bound
                        9 nr words right (following) to any bound
                       10 nr syllables right to any bound
                       11 nr words right to predicted "1" prominence (foot)
                       12 nr syllables right to predicted "1" prominence (foot)
   pre2 - pre1:     POS for the previous two words
   src:             POS of current word
   fol1 - fol2:     POS for the following two words
   hist1 - hist2:   previously predicted ACC values
   nrwordspre:      number of words left (previous) of current word
   nrsyllspre:      number of syllables left (previous) of current word,
                    incl. initial non-prim stress syllables of current word
   nrwordsfol:      number of words right (following) of current word,
                    incl. current word, up to next BOUND (any strength != 0)
   nrsyllsfol:      number of syllables right (following) of current word,
                    incl. syllables of current word starting with prim. stress
                    syllable
   footwordsfol:    nr of words to the following prominence '1'
   footsyllspre:    nr of syllables to the previous prominence '1'
   returns:         TRUE if okay, FALSE otherwise
   note:            use PICOKDT_EPSILON for att 0-4 values outside context
*/
picoos_uint8 picokdt_dtACCconstructInVec(const picokdt_DtACC this,
                                         const picoos_uint8 pre2,
                                         const picoos_uint8 pre1,
                                         const picoos_uint8 src,
                                         const picoos_uint8 fol1,
                                         const picoos_uint8 fol2,
                                         const picoos_uint16 hist1,
                                         const picoos_uint16 hist2,
                                         const picoos_uint16 nrwordspre,
                                         const picoos_uint16 nrsyllspre,
                                         const picoos_uint16 nrwordsfol,
                                         const picoos_uint16 nrsyllsfol,
                                         const picoos_uint16 footwordsfol,
                                         const picoos_uint16 footsyllsfol);

/* classify a previously constructed input vector using tree 'this'
   treeout:       direct tree output value
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtACCclassify(const picokdt_DtACC this,
                                   picoos_uint16 *treeout);

/* decompose the tree output and return the class vector in dtres
   dtres:         phrasing classification result
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtACCdecomposeOutClass(const picokdt_DtACC this,
                                            picokdt_classify_result_t *dtres);


/* ************************************************************/
/* decision tree phono-acoustical model (PAM) functions */
/* ************************************************************/

/* construct a Pam input vector and store the tree-specific encoded
   input vector in the tree object.
   vec:           tree input vector, 60 single-byte-sized attributes
   veclen:        length of vec in number of bytes
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPAMconstructInVec(const picokdt_DtPAM this,
                                         const picoos_uint8 *vec,
                                         const picoos_uint8 veclen);

/* classify a previously constructed input vector using tree 'this'
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPAMclassify(const picokdt_DtPAM this);

/* decompose the tree output and return the class in dtres
   dtres:         phones vector classification result
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPAMdecomposeOutClass(const picokdt_DtPAM this,
                                            picokdt_classify_result_t *dtres);

#ifdef __cplusplus
}
#endif



#endif /*PICOKDT_H_*/
