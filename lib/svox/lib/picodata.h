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
 * @file picodata.h
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */
#ifndef PICODATA_H_
#define PICODATA_H_

#include "picodefs.h"
#include "picoos.h"
#include "picotrns.h"
#include "picokfst.h"
#include "picorsrc.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ***************************************************************
 *                   Constants                                   *
 *****************************************************************/

#define PICODATA_MAX_ITEMS_PER_PHRASE 30

/**
 * @addtogroup picodata
 * <b> Pico Data : Item Format </b>\n
 *
  The item header is identical for all item types and PUs. Item types
 that are not handled by a PU are copied.

 Item Header structure\n
 ---------------------
   - Byte     Content
   - 0x00     item type
   - 0x01     item info 1
   - 0x02     item info 2
   - 0x03     item length in bytes (not including the header)

 depending on the item type/info, a specific subheader may follow
 (included in length)
*/

/* item header fields (tmp.: use item functions below to acces header fields */
#define PICODATA_ITEMIND_TYPE  0
#define PICODATA_ITEMIND_INFO1 1
#define PICODATA_ITEMIND_INFO2 2
#define PICODATA_ITEMIND_LEN   3

/* ***************************************************************
 *                   CharBuffer                                  *
 *****************************************************************/
typedef struct picodata_char_buffer * picodata_CharBuffer;

picodata_CharBuffer picodata_newCharBuffer(picoos_MemoryManager mm,
        picoos_Common common, picoos_objsize_t size);

void picodata_disposeCharBuffer(picoos_MemoryManager mm,
                                picodata_CharBuffer * this);

/* should not be used for PUs but only for feeding the initial cb */
pico_status_t picodata_cbPutCh(register picodata_CharBuffer this, picoos_char ch);

/* should not be used for PUs other than first PU in the chain (picotok) */
picoos_int16 picodata_cbGetCh(register picodata_CharBuffer this);

/* reset cb (as if after newCharBuffer) */
pico_status_t picodata_cbReset (register picodata_CharBuffer this);

/* ** CharBuffer item functions, cf. below in items section ****/

/* ***************************************************************
 *                   items                                       *
 *****************************************************************/

/* item header size */
#define PICODATA_ITEM_HEADSIZE 4

typedef struct picodata_itemhead
{
    picoos_uint8 type;
    picoos_uint8 info1;
    picoos_uint8 info2;
    picoos_uint8 len;
} picodata_itemhead_t;


/* -------------- System wide defines referred to by items -------- */
/* ---- These maybe better stored in a knowledge module/resoruce*/
#define PICODATA_ACC0  '\x30' /*  48  '0' */
#define PICODATA_ACC1  '\x31' /*  49  '1' */
#define PICODATA_ACC2  '\x32' /*  50  '2' */
#define PICODATA_ACC3  '\x33' /*  51  '3' */
#define PICODATA_ACC4  '\x34' /*  52  '4' */

/* reserved for future use:
 * user-imposed Part-Of-Speech ids for user lexica and phoneme tags
 * These values should be applied BEFORE POS-disambiguation. The POS lingware either assigns the same
 * ids to corresponding internal unique or composed POS or else the POS-D will consider these values
 * "default" */
#define PICODATA_POS_XNPR 20
#define PICODATA_POS_XN   21
#define PICODATA_POS_XV   22
#define PICODATA_POS_XA   23
#define PICODATA_POS_XADV 24
#define PICODATA_POS_XX   25

/* ------------------------- item types ---------------------------- */
/* new item types, info1, info2 to be defined during PU development  */
/* make sure this stays in sync with "is_valid_itemtype" function    */
#define PICODATA_ITEM_WSEQ_GRAPH '\x73'  /* 115, 's' */
#define PICODATA_ITEM_TOKEN      '\x74'  /* 116  't' */
#define PICODATA_ITEM_WORDGRAPH  '\x67'  /* 103  'g' */
#define PICODATA_ITEM_WORDINDEX  '\x69'  /* 105  'i' */
#define PICODATA_ITEM_WORDPHON   '\x77'  /* 119  'w' */
#define PICODATA_ITEM_SYLLPHON   '\x79'  /* 121  'y' */
#define PICODATA_ITEM_BOUND      '\x62'  /*  98  'b' */
/* #define PICODATA_ITEM_BOUND_DUR  '\x64' */ /* 100  'd' */ /* duration-constrained bound */
#define PICODATA_ITEM_PUNC       '\x70'  /* 112  'p' */
#define PICODATA_ITEM_CMD        '\x63'  /*  99  'c' */
#define PICODATA_ITEM_PHONE      '\x68'  /* 104  'h' */ /*reserved for PAM*/
#define PICODATA_ITEM_FRAME_PAR  '\x6b'  /* 107  'k' */ /*reserved for CEP*/
#define PICODATA_ITEM_FRAME      '\x66'  /* 102  'f' */ /*reserved for SIG*/
#define PICODATA_ITEM_OTHER      '\x6f'  /* 111  'o' */
#define PICODATA_ITEM_ERR        '\x00'  /*   0 '^@' */

/* generic iteminfo1 */
#define PICODATA_ITEMINFO1_ERR   '\x00'  /*   0 '^@' */  /* error state */
#define PICODATA_ITEMINFO1_NA    '\x01'  /*   1 '^A' */  /* not applicable */

/* generic iteminfo2 */
#define PICODATA_ITEMINFO2_ERR   '\x00'  /*   0 '^@' */ /* error state */
#define PICODATA_ITEMINFO2_NA    '\x01'  /*   1 '^A' */ /* not applicable */

/* ------------------------- PUNC item type ---------------------------- */
/* iteminfo1 */
#define PICODATA_ITEMINFO1_PUNC_SENTEND       '\x73'  /* 115  's' */
#define PICODATA_ITEMINFO1_PUNC_PHRASEEND     '\x70'  /* 112  'p' */
#define PICODATA_ITEMINFO1_PUNC_FLUSH         '\x66'  /* 102  'f' */
/* iteminfo2 */
#define PICODATA_ITEMINFO2_PUNC_SENT_T        '\x74'  /* 116  't' */
#define PICODATA_ITEMINFO2_PUNC_SENT_Q        '\x71'  /* 113  'q' */
#define PICODATA_ITEMINFO2_PUNC_SENT_E        '\x65'  /* 101  'e' */
#define PICODATA_ITEMINFO2_PUNC_PHRASE        '\x70'  /* 112  'p' */
#define PICODATA_ITEMINFO2_PUNC_PHRASE_FORCED '\x66'  /* 102  'f' */
/* len for PUNC item is ALWAYS = 0 */
/* ------------------------- BOUND item type ---------------------------- */
/* iteminfo1 : phrase strength*/
#define PICODATA_ITEMINFO1_BOUND_SBEG  '\x62'  /*  98 'b', at sentence begin */
#define PICODATA_ITEMINFO1_BOUND_SEND  '\x73'  /* 115 's', at sentence end */
#define PICODATA_ITEMINFO1_BOUND_TERM  '\x74'  /* 116 't', replaces a flush */
#define PICODATA_ITEMINFO1_BOUND_PHR0  '\x30'  /*  48 '0', no break, no item */
#define PICODATA_ITEMINFO1_BOUND_PHR1  '\x31'  /*  49 '1', pri. phrase bound. */
#define PICODATA_ITEMINFO1_BOUND_PHR2  '\x32'  /*  50 '2', short break */
#define PICODATA_ITEMINFO1_BOUND_PHR3  '\x33'  /*  51 '3', sec. phr. bound., no break*/
/* iteminfo2 : phrase type*/
#define PICODATA_ITEMINFO2_BOUNDTYPE_P '\x50'  /*  80 'P' */
#define PICODATA_ITEMINFO2_BOUNDTYPE_T '\x54'  /*  84 'T' */
#define PICODATA_ITEMINFO2_BOUNDTYPE_Q '\x51'  /*  81 'Q' */
#define PICODATA_ITEMINFO2_BOUNDTYPE_E '\x45'  /*  69 'E' */
/* len for BOUND item is ALWAYS = 0 */
/* ------------------------- CMD item type ---------------------------- */
/* iteminfo1 */
#define PICODATA_ITEMINFO1_CMD_FLUSH          'f' /* 102 flush command (all PUs)*/
#define PICODATA_ITEMINFO1_CMD_PLAY           'p' /* 112 play command : PU in info 2 will read items from file-->Filename in item content.*/
#define PICODATA_ITEMINFO1_CMD_SAVE           's' /* 115 save command : PU in info 2 will save items to file-->Filename in item content.*/
#define PICODATA_ITEMINFO1_CMD_UNSAVE         'u' /* 117 save command : PU in info 2 will stop saving items to file*/
#define PICODATA_ITEMINFO1_CMD_PROSDOMAIN     'd' /* 100 prosody domain : domain type in info 2, domain name in item content */
#define PICODATA_ITEMINFO1_CMD_SPELL          'e' /* 101 spell command : info 2 contains start/stop info,
                                                    spell type/pause len as little endian uint16 in item content */
#define PICODATA_ITEMINFO1_CMD_IGNSIG         'i' /* ignore signal command : info 2 contains start/stop info */
#define PICODATA_ITEMINFO1_CMD_PHONEME        'o' /* phoneme command : info 2 contains start/stop info, phonemes in item content */
#define PICODATA_ITEMINFO1_CMD_IGNORE         'I' /* ignore text command : info 2 contains start/stop info */
#define PICODATA_ITEMINFO1_CMD_SIL            'z' /* silence command : info 2 contains type of silence;
                                                     silence duration as little endian uint16 in item content */
#define PICODATA_ITEMINFO1_CMD_CONTEXT        'c' /* context command : context name in item content */
#define PICODATA_ITEMINFO1_CMD_VOICE          'v' /* context command : voice name in item content */
#define PICODATA_ITEMINFO1_CMD_MARKER         'm' /* marker command : marker name in item content */
#define PICODATA_ITEMINFO1_CMD_PITCH          'P' /* 80 pitch command : abs/rel info in info 2; pitch level as little endian
                                                     uint16 in item content; relative value is in promille */
#define PICODATA_ITEMINFO1_CMD_SPEED          'R' /* 82 speed command : abs/rel info in info 2, speed level as little endian
                                                     uint16 in item content; elative value is in promille */
#define PICODATA_ITEMINFO1_CMD_VOLUME         'V' /* 86 volume command : abs/rel info in info 2, volume level as little endian
                                                     uint16 in item content; relative value is in promille */
#define PICODATA_ITEMINFO1_CMD_SPEAKER        'S' /* 83 speaker command : abs/rel info in info 2, speaker level as little endian
                                                     uint16 in item content; relative value is in promille */

/* iteminfo2 for PLAY/SAVE */
#define PICODATA_ITEMINFO2_CMD_TO_TOK  't'  /* CMD+PLAY/SAVE+TOKENISATION*/
#define PICODATA_ITEMINFO2_CMD_TO_PR   'g'  /* CMD+PLAY/SAVE+PREPROC*/
#define PICODATA_ITEMINFO2_CMD_TO_WA   'w'  /* CMD+PLAY/SAVE+WORDANA*/
#define PICODATA_ITEMINFO2_CMD_TO_SA   'a'  /* CMD+PLAY/SAVE+SENTANA*/
#define PICODATA_ITEMINFO2_CMD_TO_ACPH 'h'  /* CMD+PLAY/SAVE+ACCENTUATION&PHRASING*/
#define PICODATA_ITEMINFO2_CMD_TO_SPHO 'p'  /* CMD+PLAY/SAVE+ACCENTUATION&PHRASING*/
#define PICODATA_ITEMINFO2_CMD_TO_PAM  'q'  /* CMD+PLAY/SAVE+PHONETIC-ACOUSTIC MAPPING*/
#define PICODATA_ITEMINFO2_CMD_TO_CEP  'c'  /* CMD+PLAY/SAVE+CEP_SMOOTHER*/
#define PICODATA_ITEMINFO2_CMD_TO_SIG  's'  /* CMD+PLAY/SAVE+SIG_GEN */

#if 0
#define PICODATA_ITEMINFO2_CMD_TO_FST  'f'  /* CMD+PLAY/SAVE+FST for Syll and Phonotactic constraints*/
#endif

#define PICODATA_ITEMINFO2_CMD_TO_UNKNOWN 255

/* iteminfo2 for start/end commands */
#define PICODATA_ITEMINFO2_CMD_START  's'
#define PICODATA_ITEMINFO2_CMD_END    'e'

/* iteminfo2 for speed/pitch/volume commands */
#define PICODATA_ITEMINFO2_CMD_ABSOLUTE 'a'
#define PICODATA_ITEMINFO2_CMD_RELATIVE 'r'

/* len for CMD item could be >= 0 */
/* ------------------------- TOKEN item type ---------------------------- */
/* iteminfo1: simple token type : */
#define PICODATA_ITEMINFO1_TOKTYPE_SPACE     'W'
#define PICODATA_ITEMINFO1_TOKTYPE_LETTERV   'V'
#define PICODATA_ITEMINFO1_TOKTYPE_LETTER    'L'
#define PICODATA_ITEMINFO1_TOKTYPE_DIGIT     'D'
#define PICODATA_ITEMINFO1_TOKTYPE_SEQ       'S'
#define PICODATA_ITEMINFO1_TOKTYPE_CHAR      'C'
#define PICODATA_ITEMINFO1_TOKTYPE_BEGIN     'B'
#define PICODATA_ITEMINFO1_TOKTYPE_END       'E'
#define PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED 'U'
/* iteminfo2 : token subtype */
/* len for WORDTOK item is ALWAYS > 0, if len==0 an error should be raised */

/**
 * @addtogroup picodata
 *
 * ------------------------- WORDGRAPH item type ----------------------------
 * - iteminfo1 : POS and multi-POS values defined in lingware
 * - iteminfo2 : not applicable
 * - len for WORDGRAPH item is ALWAYS > 0, if len==0 an error should be raised
 *     (currently picopr may produce empty WORDGRAPH that is eliminated by picowa)
 * \n------------------------- WORDINDEX item type ----------------------------
 * - iteminfo1 : POS and multi-POS values defined in lingware
 * - iteminfo2 : not applicable
 * - len for WORDINDEX item is ALWAYS > 0, if len==0 an error should be raised
 * \n------------------------- WORDPHON item type ----------------------------
 * - iteminfo1 : POS values defined in lingware
 * - iteminfo2 : Uses PICODATA_ACC0 .. ACC4
 *  -len WORDPHON item is ALWAYS > 0, if len==0 an error should be raised
 * \n------------------------- SYLLPHON item type ----------------------------
 * - iteminfo1 : not applicable
 * - iteminfo2 : Uses PICODATA_ACC0 .. ACC4
 * - len for SYLLPHON item is ALWAYS > 0, if len==0 an error should be raised
 * \n------------------------- PHONE item type (PRODUCED BY PAM)-----------------
 * - iteminfo1 : phonId : the phonetic identity of the phone
 * - iteminfo2 : n_S_P_Phone : number of states per phoneme
 * - len for PHON item is ALWAYS > 0, if len==0 an error should be raised
 * \n------------------------- FRAME_PAR item type (PRODUCED BY CEP) --------
 * - iteminfo1 : format (float, fixed)
 * - iteminfo2 : vector size
 * - len for FRAME_PAR item is ALWAYS > 0, if len==0 an error should be raised
 * \n------------------------- FRAME  item type (PRODUCED BY SIG) -----------
 * - iteminfo1 : number of samples per frame
 * - iteminfo2 : number of bytes per sample
 * - len for FRAME item is ALWAYS > 0, if len==0 an error should be raised
 *
 */
#define PICODATA_ITEMINFO1_FRAME_PAR_DATA_FORMAT_FIXED  '\x78' /* 120 'x' fixed point */
#define PICODATA_ITEMINFO1_FRAME_PAR_DATA_FORMAT_FLOAT  '\x66' /* 102 'f' floating point */

/* ***************************************************************
 *                   items: CharBuffer functions                 *
 *****************************************************************/

/* gets a single item (head and content) from a CharBuffer in buf;
   blenmax is the max length (in number of bytes) of buf; blen is
   set to the number of bytes gotten in buf; return values:
     PICO_OK                 <- one item gotten
     PICO_EOF                <- no item available, cb is empty
     PICO_EXC_BUF_UNDERFLOW  <- cb not empty, but no valid item
     PICO_EXC_BUF_OVERFLOW   <- buf not large enough
*/
pico_status_t picodata_cbGetItem(register picodata_CharBuffer this,
        picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen);

/* gets the speech data (without item head) from a CharBuffer in buf;
   blenmax is the max length (in number of bytes) of buf; blen is
   set to the number of bytes gotten in buf; return values:
     PICO_OK                 <- speech data of one item gotten
     PICO_EOF                <- no item available, cb is empty
     PICO_EXC_BUF_UNDERFLOW  <- cb not empty, but no valid item
     PICO_EXC_BUF_OVERFLOW   <- buf not large enough
*/
pico_status_t picodata_cbGetSpeechData(register picodata_CharBuffer this,
        picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen);

/* puts a single item (head and content) to a CharBuffer; clenmax is
   the max length (in number of bytes) accessible in content; clen is
   set to the number of bytes put from content; return values:
     PICO_OK                 <- one item put
     PICO_EXC_BUF_UNDERFLOW  <- no valid item in buf
     PICO_EXC_BUF_OVERFLOW   <- cb not large enough
*/
pico_status_t picodata_cbPutItem(register picodata_CharBuffer this,
        const picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen);

/* unsafe, just for measuring purposes */
picoos_uint8 picodata_cbGetFrontItemType(register picodata_CharBuffer this);

/* ***************************************************************
 *                   items: support function                     *
 *****************************************************************/

/* checks, whether item of type 'ch' is a valid item type */
picoos_uint8 is_valid_itemtype(const picoos_uint8 ch);

/* gets from buf a single item, values in head set and item content
   copied to content; blenmax and clenmax are the max lengths (in
   number of bytes) accessible in buf and content; clen is set to the
   number of bytes gotten in content; return values:
     PICO_OK                 <- all ok
     PICO_EXC_BUF_UNDERFLOW  <- blenmax problem, or no valid item
     PICO_EXC_BUF_OVERFLOW   <- overflow in content
*/
pico_status_t picodata_get_itemparts_nowarn(
        const picoos_uint8 *buf, const picoos_uint16 blenmax,
        picodata_itemhead_t *head, picoos_uint8 *content,
        const picoos_uint16 clenmax, picoos_uint16 *clen);

/* gets from buf a single item, values in head set and item content
   copied to content; blenmax and clenmax are the max lengths (in
   number of bytes) accessible in buf and content; clen is set to the
   number of bytes gotten in content; return values:
     PICO_OK                 <- all ok
     PICO_EXC_BUF_UNDERFLOW  <- blenmax problem, or no valid item
     PICO_EXC_BUF_OVERFLOW   <- overflow in content
*/
pico_status_t picodata_get_itemparts(
        const picoos_uint8 *buf, const picoos_uint16 blenmax,
        picodata_itemhead_t *head, picoos_uint8 *content,
        const picoos_uint16 clenmax, picoos_uint16 *clen);

/* puts a single item to buf; values in head and content copied to
   buf; clenmax is the max length (in number of bytes) accessible in
   content; blenmax is the max length (bytes) accessible in buf; blen
   is set to the number of bytes put to buf; return values:
     PICO_OK                 <- all ok
     PICO_EXC_BUF_UNDERFLOW  <- clenmax problem, or no valid item
     PICO_EXC_BUF_OVERFLOW   <- overflow in buf
*/
pico_status_t picodata_put_itemparts(const picodata_itemhead_t *head,
        const picoos_uint8 *content, const picoos_uint16 clenmax,
        picoos_uint8 *buf, const picoos_uint16 blenmax, picoos_uint16 *blen);

/* gets from buf info of a single item, values in head are set and
   content is set to the start of content in buf (not copied!);
   content is set to NULL if the content length is 0; blenmax is the
   max lengths (in number of bytes) accessible in buf; return values:
     PICO_OK                 <- all ok
     PICO_EXC_BUF_UNDERFLOW  <- blenmax problem, or no valid item
*/
pico_status_t picodata_get_iteminfo(
        picoos_uint8 *buf, const picoos_uint16 blenmax,
        picodata_itemhead_t *head, picoos_uint8 **content);

/* copies the item in inbuf to outbuf after first checking if there is
   a valid item in inbuf; inlenmax and outlenmax are the max length
   (in number of byte) accessible in the buffers); in *numb the total
   number of bytes copied to outbuf (incl. header) is returned; return
   values:
   PICO_OK                 <- item copied
   PICO_EXC_BUF_OVERFLOW   <- overflow in outbuf
   PICO_ERR_OTHER          <- no valid item in inbuf
*/
pico_status_t picodata_copy_item(const picoos_uint8 *inbuf,
        const picoos_uint16 inlenmax, picoos_uint8 *outbuf,
        const picoos_uint16 outlenmax, picoos_uint16 *numb);

/* sets the info1 field in the header contained in the item in buf;
   return values:
   PICO_OK                 <- all ok
   PICO_EXC_BUF_UNDERFLOW  <- underflow in buf
*/
pico_status_t picodata_set_iteminfo1(picoos_uint8 *buf,
        const picoos_uint16 blenmax, const picoos_uint8 info);

/* sets the info2 field in the header contained in the item in buf;
   return values:
   PICO_OK                 <- all ok
   PICO_EXC_BUF_UNDERFLOW  <- underflow in buf
*/
pico_status_t picodata_set_iteminfo2(picoos_uint8 *buf,
        const picoos_uint16 blenmax, const picoos_uint8 info);

/* sets the len field in the header contained in the item in buf;
   return values:
   PICO_OK                 <- all ok
   PICO_EXC_BUF_UNDERFLOW  <- underflow in buf
*/
pico_status_t picodata_set_itemlen(picoos_uint8 *buf,
        const picoos_uint16 blenmax, const picoos_uint8 len);

/* check item validity and return TRUE if valid; return FALSE if
   invalid; ilenmax is the max index to be used in item
*/
picoos_uint8 picodata_is_valid_item(const picoos_uint8 *item,
        const picoos_uint16 ilenmax);

/* return TRUE if head is a valid item head, FALSE otherwise */
picoos_uint8 picodata_is_valid_itemhead(const picodata_itemhead_t *head);


/* ***************************************************************
 *                   ProcessingUnit                              *
 *****************************************************************/
/* public */

#define PICODATA_MAX_ITEMSIZE (picoos_uint16) (PICODATA_ITEM_HEADSIZE + 256)

/* different buffer sizes per processing unit */
#define PICODATA_BUFSIZE_DEFAULT (picoos_uint16) PICODATA_MAX_ITEMSIZE
#define PICODATA_BUFSIZE_TEXT    (picoos_uint16)  1 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_TOK     (picoos_uint16)  2 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_PR      (picoos_uint16)  2 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_WA      (picoos_uint16)  2 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_SA      (picoos_uint16)  2 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_ACPH    (picoos_uint16)  2 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_SPHO    (picoos_uint16)  4 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_PAM     (picoos_uint16)  4 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_CEP     (picoos_uint16) 16 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_SIG     (picoos_uint16) 16 * PICODATA_BUFSIZE_DEFAULT
#define PICODATA_BUFSIZE_SINK     (picoos_uint16) 1 * PICODATA_BUFSIZE_DEFAULT

/* different types of processing units */
typedef enum picodata_putype {
    PICODATA_PUTYPE_TEXT,   /* text */
    PICODATA_PUTYPE_TOK,    /* tokenizer output */
    PICODATA_PUTYPE_PR,     /* preprocessor output */
    PICODATA_PUTYPE_WA,     /* word analysis */
    PICODATA_PUTYPE_SA,     /* sentence analysis */
    PICODATA_PUTYPE_ACPH,     /* accentuation and phrasing */
    PICODATA_PUTYPE_SPHO,   /* sentence phonology (textana postproc) */
    PICODATA_PUTYPE_PAM,    /* phonetics to acoustics mapper processing unit */
    PICODATA_PUTYPE_CEP,    /* cepstral smoothing processing unit */
    PICODATA_PUTYPE_SIG,     /* signal generation processing unit*/
    PICODATA_PUTYPE_SINK     /* item sink unit*/
} picodata_putype_t;

picoos_uint16 picodata_get_default_buf_size (picodata_putype_t puType);

/* result values returned from the pu->puStep() methode */
typedef enum picodata_step_result {
    PICODATA_PU_ERROR,
    /* PICODATA_PU_EMPTY, *//* reserved (no internal data to be processed) */
    PICODATA_PU_IDLE, /* need more input to process internal data */
    PICODATA_PU_BUSY, /* processing internal data */
    PICODATA_PU_ATOMIC, /* same as pu_busy, but wants to get next time slot (while in an "atomar" operation) */
    PICODATA_PU_OUT_FULL /* can't proceed because output is full. (next time slot to be assigned to pu's output's consumer) */
} picodata_step_result_t;

typedef struct picodata_processing_unit * picodata_ProcessingUnit;

picodata_ProcessingUnit picodata_newProcessingUnit(
        picoos_MemoryManager mm,
        picoos_Common common,
        picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut,
        picorsrc_Voice voice);

void picodata_disposeProcessingUnit(
        picoos_MemoryManager mm,
        picodata_ProcessingUnit * this);

picodata_CharBuffer picodata_getCbIn(picodata_ProcessingUnit this);
picodata_CharBuffer picodata_getCbOut(picodata_ProcessingUnit this);
pico_status_t picodata_setCbIn(picodata_ProcessingUnit this, picodata_CharBuffer cbIn);
pico_status_t picodata_setCbOut(picodata_ProcessingUnit this, picodata_CharBuffer cbOut);

/* protected */
typedef pico_status_t (* picodata_puInitializeMethod) (register picodata_ProcessingUnit this, picoos_int32 mode);
typedef pico_status_t (* picodata_puTerminateMethod) (register picodata_ProcessingUnit this);
typedef picodata_step_result_t (* picodata_puStepMethod) (register picodata_ProcessingUnit this, picoos_int16 mode, picoos_uint16 * numBytesOutput);
typedef pico_status_t (* picodata_puSubDeallocateMethod) (register picodata_ProcessingUnit this, picoos_MemoryManager mm);

typedef struct picodata_processing_unit
{
    /* public */
    picodata_puInitializeMethod initialize;
    picodata_puStepMethod       step;
    picodata_puTerminateMethod  terminate;
    picorsrc_Voice              voice;

    /* protected */
    picoos_Common                  common;
    picodata_CharBuffer            cbIn, cbOut;
    picodata_puSubDeallocateMethod subDeallocate;
    void * subObj;

} picodata_processing_unit_t;

/* currently, only wav input and output is supported */
#define PICODATA_PUTYPE_TEXT_OUTPUT_EXTENSION   (picoos_uchar*)".txt"
#define PICODATA_PUTYPE_TOK_INPUT_EXTENSION     PICODATA_PUTYPE_TEXT_OUTPUT_EXTENSION
#define PICODATA_PUTYPE_TOK_OUTPUT_EXTENSION    (picoos_uchar*)".tok"
#define PICODATA_PUTYPE_PR_INPUT_EXTENSION      PICODATA_PUTYPE_TOK_OUTPUT_EXTENSION
#define PICODATA_PUTYPE_PR_OUTPUT_EXTENSION     (picoos_uchar*)".pr"
#define PICODATA_PUTYPE_WA_INPUT_EXTENSION      PICODATA_PUTYPE_PR_OUTPUT_EXTENSION
#define PICODATA_PUTYPE_WA_OUTPUT_EXTENSION     (picoos_uchar*)".wa"
#define PICODATA_PUTYPE_SA_INPUT_EXTENSION      PICODATA_PUTYPE_WA_OUTPUT_EXTENSION
#define PICODATA_PUTYPE_SA_OUTPUT_EXTENSION     (picoos_uchar*)".sa"
#define PICODATA_PUTYPE_ACPH_INPUT_EXTENSION    PICODATA_PUTYPE_SA_OUTPUT_EXTENSION
#define PICODATA_PUTYPE_ACPH_OUTPUT_EXTENSION   (picoos_uchar*)".acph"
#define PICODATA_PUTYPE_SPHO_INPUT_EXTENSION    PICODATA_PUTYPE_ACPH_OUTPUT_EXTENSION
#define PICODATA_PUTYPE_SPHO_OUTPUT_EXTENSION   (picoos_uchar*)".spho"
#define PICODATA_PUTYPE_PAM_INPUT_EXTENSION     PICODATA_PUTYPE_SPHO_OUTPUT_EXTENSION
#define PICODATA_PUTYPE_PAM_OUTPUT_EXTENSION    (picoos_uchar*)".pam"
#define PICODATA_PUTYPE_CEP_INPUT_EXTENSION     PICODATA_PUTYPE_PAM_OUTPUT_EXTENSION
#define PICODATA_PUTYPE_CEP_OUTPUT_EXTENSION    (picoos_uchar*)".cep"
#define PICODATA_PUTYPE_SIG_INPUT_EXTENSION     PICODATA_PUTYPE_CEP_OUTPUT_EXTENSION   /*PP 11.7.08*/
#define PICODATA_PUTYPE_SIG_OUTPUT_EXTENSION    (picoos_uchar*)".sig"
#define PICODATA_PUTYPE_SINK_INPUT_EXTENSION    PICODATA_PUTYPE_SIG_OUTPUT_EXTENSION

/*wav input is for play wav files in sig */
#define PICODATA_PUTYPE_WAV_INPUT_EXTENSION    (picoos_uchar*)".wav"                    /*PP 11.7.08*/

/*wav output is for saving wav (binary) files in sig*/
#define PICODATA_PUTYPE_WAV_OUTPUT_EXTENSION    (picoos_uchar*)".wav"                    /*PP 14.7.08*/

/* ***************************************************************
 *                   auxiliary routines                          *
 *****************************************************************/

picoos_uint8 picodata_getPuTypeFromExtension(picoos_uchar * filename, picoos_bool input);

#define PICODATA_XSAMPA (picoos_uchar *)"xsampa"
#define PICODATA_SAMPA (picoos_uchar *)"sampa"
#define PICODATA_SVOXPA (picoos_uchar *)"svoxpa"

/*----------------------------------------------------------*/
/** @brief   maps an input phone string to its internal representation
 *
 * @param transducer initialized SimpleTransducer
 * @param xsampa_parser fst converting xsampa char input to xsampa ids
 * @param svoxpa_parser
 * @param xsampa2svoxpa_mapper
 * @param inputPhones input phone string in alphabet 'alphabet'
 * @param alphabet input alphabet
 * @retval outputPhoneIds output phone string in internal representation
 * @param maxOutputPhoneIds
 * @return PICO_OK=mapping done, PICO_ERR_OTHER:unknown alphabet, unknown phones
 */
/*---------------------------------------------------------*/
pico_status_t picodata_mapPAStrToPAIds(
        picotrns_SimpleTransducer transducer,
        picoos_Common common,
        picokfst_FST xsampa_parser,
        picokfst_FST svoxpa_parser,
        picokfst_FST xsampa2svoxpa_mapper,
        picoos_uchar * inputPhones,
        picoos_uchar * alphabet,
        picoos_uint8 * outputPhoneIds,
        picoos_int32 maxOutputPhoneIds);

/* number of binary digits after the comma for fixed-point calculation */
#define PICODATA_PRECISION 10
/* constant 0.5 in PICODATA_PRECISION */
#define PICODATA_PREC_HALF 512

void picodata_transformDurations(
        picoos_uint8 frame_duration_exp,
        picoos_int8 array_length,
        picoos_uint8 * inout,
        const picoos_uint16 * weight,  /* integer weights */
        picoos_int16 mintarget, /* minimum target duration in ms */
        picoos_int16 maxtarget, /* maximum target duration in ms */
        picoos_int16 facttarget, /* factor to be multiplied with original length to get the target
                                     the factor is fixed-point with precision PRECISION, i.e.
                                     the factor as float would be facttarget / PRECISION_FACT
                                     if factor is 0, only min/max are considered */
        picoos_int16 * dur_rest /* in/out, rest in ms */
        );



/* ***************************************************************
 *                   For Debugging only                          *
 *****************************************************************/

#if defined (PICO_DEBUG)

/* convert (pretty print) item head 'head' and put output in 'str',
   strsize is the maximum length of 'str' in bytes */
picoos_char * picodata_head_to_string(const picodata_itemhead_t *head,
                                      picoos_char * str, picoos_uint16 strsize);

/* put 'pref6ch' (max. 6 char prefix) and a pretty print output of
   'item' in 'str', strlenmax is the maximum length of 'str' in
   bytes */
void picodata_info_item(const picoknow_KnowledgeBase kb,
                        const picoos_uint8 *pref6ch,
                        const picoos_uint8 *item,
                        const picoos_uint16 itemlenmax,
                        const picoos_char *filterfn);


#define PICODATA_INFO_ITEM(kb, pref, item, itemlenmax)   \
    PICODBG_INFO_CTX(); \
    picodata_info_item(kb, pref, item, itemlenmax, (picoos_char *)__FILE__)



#else

#define PICODATA_INFO_ITEM(kb, pref, item, itemlenmax)

#endif

#ifdef __cplusplus
}
#endif

#endif /*PICODATA_H_*/
