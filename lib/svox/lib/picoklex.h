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
 * @file picoklex.h
 *
 * knowledge base: lexicon
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#ifndef PICOKLEX_H_
#define PICOKLEX_H_

#include "picoos.h"
#include "picoknow.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ************************************************************/
/* function to create specialized kb, */
/* to be used by picorsrc only */
/* ************************************************************/

pico_status_t picoklex_specializeLexKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common);


/* ************************************************************/
/* lexicon type and getLex function */
/* ************************************************************/

/* lexicon type */
typedef struct picoklex_lex * picoklex_Lex;

/* return kb lex for usage in PU */
picoklex_Lex picoklex_getLex(picoknow_KnowledgeBase this);


/* ************************************************************/
/* lexicon lookup result type */
/* ************************************************************/

/* max nr of results */
#define PICOKLEX_MAX_NRRES   4

/* nr of bytes used for pos and index, needs to fit in uint32, ie. max 4 */
#define PICOKLEX_POSIND_SIZE 4
/* nr of bytes used for index, needs to fit in uint32, ie. max 4 */
#define PICOKLEX_IND_SIZE    3
/* max len (in bytes) of ind, (PICOKLEX_MAX_NRRES * PICOKLEX_POSIND_SIZE) */
#define PICOKLEX_POSIND_MAXLEN 16


/* the lexicon lookup result(s) are stored in field posind, which
   contains a sequence of
     POS1-byte, IND1-bytes, POS2-byte, IND2-bytes, etc.

   the IND-bytes are the byte position(s) in the lexblocks part of the
   lexicon byte stream, starting at picoklex_lex_t.lexblocks.

   for lexentries without phones only the POS (there can be only one)
   is stored in posind, nrres equals one, and phonfound is FALSE.
*/

typedef struct {
    picoos_uint8 nrres;      /* number of results, 0 of no entry found */
    picoos_uint8 posindlen;  /* number of posind bytes */
    picoos_uint8 phonfound;  /* phones found flag, TRUE if found */
    picoos_uint8 posind[PICOKLEX_POSIND_MAXLEN]; /* sequence of multi-ind,
                                                    one per result */
} picoklex_lexl_result_t;


/* ************************************************************/
/* lexicon lookup functions */
/* ************************************************************/

/** lookup lex by graph; result(s) are in lexres, ie. the phones are
   not returned directly (because they are used later and space can be
   saved using indices first), lexres contains an index (or several)
   to the entry for later fast lookup once the phones are needed.
   PICOKLEX_IND_SIZE bytes are used for the index, these ind bytes are
   saved in the WORDINDEX items. If at least one entry is found TRUE
   is returned, FALSE otherwise */
picoos_uint8 picoklex_lexLookup(const picoklex_Lex this,
                                const picoos_uint8 *graph,
                                const picoos_uint16 graphlen,
                                picoklex_lexl_result_t *lexres);

/** lookup lex entry by index ind; ind is a sequence of bytes with
   length indlen (must be equal PICOKLEX_IND_SIZE) that is the content
   of a WORDINDEX item. Returns TRUE if okay, FALSE otherwise */
picoos_uint8 picoklex_lexIndLookup(const picoklex_Lex this,
                                   const picoos_uint8 *ind,
                                   const picoos_uint8 indlen,
                                   picoos_uint8 *pos,
                                   picoos_uint8 **phon,
                                   picoos_uint8 *phonlen);

#ifdef __cplusplus
}
#endif


#endif /*PICOKLEX_H_*/
