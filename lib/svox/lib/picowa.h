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
 * @file picowa.h
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */


/**
 * @addtogroup picowa
 * ---------------------------------------------------\n
 * <b> Pico Word Analysis </b>\n
 * ---------------------------------------------------\n
itemtype, iteminfo1, iteminfo2, content -> TYPE(INFO1,INFO2)content
in the following

items input\n
===========

processed by wa:
- WORDGRAPH(NA,NA)graph
- OTHER(NA,NA)string

unprocessed:
- all other item types are forwarded through the PU without modification:
  - PUNC
  - CMD


minimal input size (before processing starts)\n
==================

processing (ie. lex lookup and POS prediction) is possible with
- one item


items processed and output\n
==========================

processing an input WORDGRAPH results in one of the following items:
- WORDGRAPH(POSes,NA)graph
   - graph not in lex, POSes determined with dtree, or
   - graph in lex - single entry without phone (:G2P), POSes from lex
- WORDINDEX(POSes,NA)pos1|ind1...posN|indN
   - graph in lex - {1,4} entries with phone, pos1...posN from lex,
     {1,4} lexentries indices in content, POSes combined with map table
     in klex

processing an input OTHER results in the item being skipped (in the
future this can be extended to e.g. spelling)

see picotok.h for PUNC and CMD

- POSes %d
  - is the superset of all single POS and POS combinations defined
  in the lingware as unique symbol
- graph, len>0, utf8 graphemes, %s
- pos1|ind1, pos2|ind2, ..., posN|indN
  - pos? are the single, unambiguous POS only, one byte %d
  - ind? are the lexentry indices, three bytes %d %d %d


lexicon (system lexicon, but must also be ensured for user lexica)\n
=======

- POS GRAPH PHON, all mandatory, but
  - * PHON can be an empty string -> no pronunciation in the resulting TTS output
  - * PHON can be :G2P -> use G2P later to add pronunciation
- (POS,GRAPH) is a uniq key (only one entry allowed)
- (GRAPH) is almost a uniq key (2-4 entries with the same GRAPH, and
  differing POS and differing PHON possible)
  - for one graph we can have 2-4 solutions from the lex which all
     need to be passed on the the next PU
  - in this case GRAPH, POS, and PHON all must be available in lex
  - in this case for each entry only a non-ambiguous, unique POS ID
     is possible)

other limitations\n
=================

- item size: header plus len=256 (valid for Pico in general)
- wa uses one item context only -> internal buffer set to 256+4
 */


#ifndef PICOWA_H_
#define PICOWA_H_

#include "picoos.h"
#include "picodata.h"
#include "picorsrc.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* maximum length of an item incl. head for input and output buffers */
#define PICOWA_MAXITEMSIZE 260


picodata_ProcessingUnit picowa_newWordAnaUnit(
        picoos_MemoryManager mm,
    picoos_Common common,
        picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut,
        picorsrc_Voice voice);

#ifdef __cplusplus
}
#endif

#endif /*PICOWA_H_*/
