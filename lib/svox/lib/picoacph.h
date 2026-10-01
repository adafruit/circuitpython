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
 * @file picoacph.h
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */


/**
 * @addtogroup picoacph
 *
itemtype, iteminfo1, iteminfo2, content -> TYPE(INFO1,INFO2)content
in the following

items input
===========

processed by sa (POS disambiguation):
- WORDGRAPH(POSes,NA)graph
- WORDINDEX(POSes,NA)POS|1ind1...POSN|indN
- CMD(PICODATA_ITEMINFO1_CMD_FLUSH,PICODATA_ITEMINFO2_NA)

processed by sa (Phrasing, Accentuation):
- PUNC(PUNCtype,PUNCsubtype)

unprocessed:
- all other item types are forwarded through the PU without modification:
  CMD


minimal input size (before processing starts)
==================

processing (POS disambiguation, g2p, lexind, phrasing, accentuation)
is possible with

- one punctuation-phrase, consisting of a sequence (see below for
  limits) of items terminated by a PUNC item.

(possible but not implemented: as long as the internal buffer is
empty, non-processed item types can be processed immediately)

Ensuring terminal PUNC item:
- when reading items from the external buffer a CMD(...FLUSH...) is
  converted to a PUNC(...FLUSH...) item
- If needed, a PUNC(PHRASE) is artificially added to ensure a phrase
  fits in the PUs memory and processing can start.


items processed and output
==========================

precondition:
CMD(...FLUSH...) already converted to PUNC(...FLUSH...) and trailing
PUNC item enforced if necessary.

----
-# PROCESS_POSD: processing input WORDGRAPH or WORDINDEX items, after
POS disambiguation (POSes -> POS), results in a sequence of:
  -
  - WORDGRAPH(POS,NA)graph
  - WORDINDEX(POS,NA)POS|ind
  -
  .
-# PROCESS_WPHO: then, after lex-index lookup and G2P in a
sequence of:
  - WORDPHON(POS,NA)phon

(phon containing primary and secondary word-level stress)

----
3. PROCESS_PHR: then, after processing these WORDPHON items,
together with the trailing PUNC item results in:

-> BOUND(BOUNDstrength,BOUNDtype)

being added in the sequence of WORDPHON (respectively inserted instead
of the PUNC). All PUNC, incl PUNC(...FLUSH...) now gone.

----
4. PROCESS_ACC: then, after processing the WORDPHON and BOUND items
results in:

-> WORDPHON(POS,ACC)phon

A postprocessing step of accentuation is hard-coded in the
accentuation module: In case the whole word does not have any stress
at all (primary or secondary or both) then do the following mapping:

  ACC0 nostress -> ACC0
  ACC1 nostress -> ACC3
  ACC2 nostress -> ACC3
  ACC3 nostress -> ACC3

----
- POS
  a single, unambiguous POS

cf. picodata.h for
- ACC    (sentence-level accent (aka prominence)) %d
  - PICODATA_ACC0
  - PICODATA_ACC1
  - PICODATA_ACC2  (<- maybe mapped to ACC1, ie. no ACC2 in output)
  - PICODATA_ACC3

- BOUNDstrength %d
  - PICODATA_ITEMINFO1_BOUND_SBEG (at sentence start)
  - PICODATA_ITEMINFO1_BOUND_SEND (at sentence end)
  - PICODATA_ITEMINFO1_BOUND_TERM (replaces a flush)
  - PICODATA_ITEMINFO1_BOUND_PHR1 (primary boundary)
  - PICODATA_ITEMINFO1_BOUND_PHR2 (short break)
  - PICODATA_ITEMINFO1_BOUND_PHR3 (secondary phrase boundary, no break)
  - PICODATA_ITEMINFO1_BOUND_PHR0 (no break, not produced by sa, not existing
          BOUND in item sequence equals PHR0 bound strength)

- BOUNDtype    (created in sa base on punctuation, indicates type of phrase
                following the boundary) %d
  - PICODATA_ITEMINFO2_BOUNDTYPE_P
  - PICODATA_ITEMINFO2_BOUNDTYPE_T
  - PICODATA_ITEMINFO2_BOUNDTYPE_Q
  - PICODATA_ITEMINFO2_BOUNDTYPE_E


output sequence (without CMDs):

<output> = { BOUND(BOUND_SBEG,PHRASEtype) <sentence> BOUND(BOUND_SEND,..)} BOUND(BOUND_TERM,..)

<sentence> =   <phrase> { BOUND(BOUND_PHR1|2|3,BOUNDtype) <phrase> }

<phrase> = WORDPHON(POS,ACC)phon { WORDPHON(POS,ACC)phon }

Done in later PU: mapping ACC & word-level stress to syllable accent value
  - ACC0 prim -> 0
  - ACC1 prim -> 1
  - ACC2 prim -> 2
  - ACC3 prim -> 3
  - ACC0 sec  -> 0
  - ACC1 sec  -> 4
  - ACC2 sec  -> 4
  - ACC3 sec  -> 4

other limitations
=================

- item size: header plus len=256 (valid for Pico in general)
- see defines below for max nr of items. Item heads plus ref. to contents
  buffer are stored in array with fixed size elements. Two restrictions:
  - MAXNR_HEADX (max nr elements==items in headx array)
  - CONTENTSSIZE (max size of all contents together
 */


#ifndef PICOACPH_H_
#define PICOACPH_H_

#include "picoos.h"
#include "picodata.h"
#include "picorsrc.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* nr item restriction: maximum number of extended item heads in headx */
#define PICOACPH_MAXNR_HEADX    60

/* nr item restriction: maximum size of all item contents together in cont */
#define PICOACPH_MAXSIZE_CBUF 7680



picodata_ProcessingUnit picoacph_newAccPhrUnit(
        picoos_MemoryManager mm,
        picoos_Common common,
        picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut,
        picorsrc_Voice voice);

#ifdef __cplusplus
}
#endif

#endif /*PICOACPH_H_*/
