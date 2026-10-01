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
 * @file picospho.h
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

/** @addtogroup picospho
itemtype, iteminfo1, iteminfo2, content -> TYPE(INFO1,INFO2)content
in the following

items input
===========

processed:

- WORDPHON(POS,WACC)phon

- BOUND(BOUNDstrength,BOUNDtype)


unprocessed:
- all other item types are forwared through the PU without modification



- POS
  a the single, unambiguous POS

cf. picodata.h for
- WACC    (sentence-level accent (aka prominence))
  PICODATA_ACC0
  PICODATA_ACC1
  PICODATA_ACC2  (<- maybe mapped to ACC1, ie. no ACC2 in output)
  PICODATA_ACC3


- BOUNDstrength
  PICODATA_ITEMINFO1_BOUND_SBEG (sentence start)
  PICODATA_ITEMINFO1_BOUND_SEND (at sentence end)
  PICODATA_ITEMINFO1_BOUND_TERM (replaces a flush)
  PICODATA_ITEMINFO1_BOUND_PHR0 (no break)
  PICODATA_ITEMINFO1_BOUND_PHR1 (primary boundary)
  PICODATA_ITEMINFO1_BOUND_PHR2 (short break)
  PICODATA_ITEMINFO1_BOUND_PHR3 (secondary phrase boundary, no break)

- BOUNDtype    (actually phrase type of the following phrase)
  PICODATA_ITEMINFO2_BOUNDTYPE_P (non-terminal phrase)
  PICODATA_ITEMINFO2_BOUNDTYPE_T (terminal phrase)
  PICODATA_ITEMINFO2_BOUNDTYPE_Q (question terminal phrase)
  PICODATA_ITEMINFO2_BOUNDTYPE_E (exclamation terminal phrase)


output sequence (without CMDs):

<output> = { BOUND(BOUND_SBEG,PHRASEtype) <sentence> BOUND(BOUND_SEND,..)} BOUND(BOUND_TERM,..)

<sentence> =   <phrase> { BOUND(BOUND_PHR1|2|3,PHRASEtype) <phrase> }

<phrase> = WORDPHON(POS,ACC)phon { WORDPHON(POS,ACC)phon }



mapping ACC & word-level stress to syllable accent value

  ACC0 prim -> 0
  ACC1 prim -> 1
  ACC2 prim -> 2
  ACC3 prim -> 3

  ACC0 sec  -> 0
  ACC1 sec  -> 4
  ACC2 sec  -> 4
  ACC3 sec  -> 4

Mapping of values to FST symbol id (has to identical to the symbol table used when compiling the FST)

Value                   FST symbol id
phoneme_id      ->      phoneme_id     +  256 *  PICOKFST_PLANE_PHONEMES
POS_id          ->      POS_id         +  256 *  PICOKFST_PLANE_POS
phrasetype_id   ->      phrasetype_id  +  256 *  PICOKFST_PLANE_PHRASETYPES
accentlevel_id  ->      accentlevel_id +  256 *  PICOKFST_PLANE_ACCENTS







minimal input size (before processing starts)
==================

processing (ie. sequencially applying spho transducers to phoneme sequence composed of
            - phonemes inside WORDPHON items and
            - pseudo-phonemes derived from boundaries and POS) is possible with

- one phrase, consisting of a sequence of maximal 30 non-PUNC items
  terminated by a PUNC item. A PUNC is artificially enforced if
  needed to start processing.

- as long as the internal buffer is empty, non-processed item types
  can be processed immediately



items output
============
- BOUND(BOUNDstrength,BOUNDtype)

  bound strength may be changed by the fsts

  in addition, BOUNDs of BOUNDstrength = PHR0 are inserted to mark word boundaries

- SYLLPHON(POS,ACC)phon
  where POS is only set for the first syllable of a word, otherwise NA






other limitations
=================


 */
#ifndef PICOSPHO_H_
#define PICOSPHO_H_

#include "picoos.h"
#include "picodata.h"
#include "picorsrc.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


picodata_ProcessingUnit picospho_newSentPhoUnit(
        picoos_MemoryManager mm,
        picoos_Common common,
        picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut,
        picorsrc_Voice voice);

#ifdef __cplusplus
}
#endif


#endif /*PICOSPHO_H_*/
