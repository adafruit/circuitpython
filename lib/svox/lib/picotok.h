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
 * @file picotok.h
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */


/** @addtogroup picotok
itemtype, iteminfo1, iteminfo2, content -> TYPE(INFO1,INFO2)content
in the following

input
=====

- UTF8 text

limitations: currently only german umlauts in addition to ASCII


minimal input size (before processing starts)
==================

processing (ie. tokenization) starts when
- 'PICO_EOF' char received (which happens whenever the cbIn buffer is empty)
- tok-internal buffer is full


items output
============

processing the character stream can result in one of the
following items:
-> WORDGRAPH(NA,NA)graph    <- mapped to lower case; incl. 1-2 digit nrs (0-99)
-> OTHER(NA,NA)string       <- skip or spell
-> PUNC(PUNCtype,PUNCsubtype)
-> CMD(CMDtype,CMDsubtype)args

with
- PUNCtype %d
    PICODATA_ITEMINFO1_PUNC_SENTEND
    PICODATA_ITEMINFO1_PUNC_PHRASEEND
- PUNCsubtype %d
    PICODATA_ITEMINFO2_PUNC_SENT_T
    PICODATA_ITEMINFO2_PUNC_SENT_Q
    PICODATA_ITEMINFO2_PUNC_SENT_E
    PICODATA_ITEMINFO2_PUNC_PHRASE
    (used later: PICODATA_ITEMINFO2_PUNC_PHRASE_FORCED)
- CMDtype %d
    PICODATA_ITEMINFO1_CMD_FLUSH    (no args)
    ? PICODATA_ITEMINFO1_CMD_PLAY ? (not yet)
- CMDsubtype %d
    PICODATA_ITEMINFO2_NA
    ? PICODATA_ITEMINFO2_CMD_PLAY_G2P ? (not yet)
- graph, len>0, utf8 graphemes, %s
- string, len>0, can be any string with printable ascii characters, %s


other limitations
=================

- item size: header plus len=256 (valid for Pico in general)
 */


#ifndef PICOTOK_H_
#define PICOTOK_H_

#include "picoos.h"
#include "picodata.h"
#include "picorsrc.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif



picodata_ProcessingUnit picotok_newTokenizeUnit(
        picoos_MemoryManager mm,
        picoos_Common common,
        picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut,
        picorsrc_Voice voice);

#define PICOTOK_OUTBUF_SIZE 256

#ifdef __cplusplus
}
#endif


#endif /*PICOTOK_H_*/
