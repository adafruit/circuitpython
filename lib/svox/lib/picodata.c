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
 * @file picodata.c
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#include "picodefs.h"
#include "picoos.h"
#include "picodbg.h"
#include "picorsrc.h"
#include "picotrns.h"
#include "picodata.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* we output coefficients as fixed point values */
#define PICOCEP_OUT_DATA_FORMAT PICODATA_ITEMINFO1_FRAME_PAR_DATA_FORMAT_FIXED

/* ***************************************************************
 *                   CharBuffer                                  *
 *****************************************************************/

/*
 * method signatures
 */
typedef pico_status_t (* picodata_cbPutItemMethod) (register picodata_CharBuffer this,
        const picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen);

typedef pico_status_t (* picodata_cbGetItemMethod) (register picodata_CharBuffer this,
        picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen, const picoos_uint8 issd);

typedef pico_status_t (* picodata_cbSubResetMethod) (register picodata_CharBuffer this);
typedef pico_status_t (* picodata_cbSubDeallocateMethod) (register picodata_CharBuffer this, picoos_MemoryManager mm);

typedef struct picodata_char_buffer
{
    picoos_char *buf;
    picoos_uint16 rear; /* next free position to write */
    picoos_uint16 front; /* next position to read */
    picoos_uint16 len; /* empty: len = 0, full: len = size */
    picoos_uint16 size;

    picoos_Common common;

    picodata_cbGetItemMethod getItem;
    picodata_cbPutItemMethod putItem;

    picodata_cbSubResetMethod subReset;
    picodata_cbSubDeallocateMethod subDeallocate;
    void * subObj;
} char_buffer_t;


static pico_status_t data_cbPutItem(register picodata_CharBuffer this,
        const picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen);

static pico_status_t data_cbGetItem(register picodata_CharBuffer this,
        picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen, const picoos_uint8 issd);

pico_status_t picodata_cbReset(register picodata_CharBuffer this)
{
    this->rear = 0;
    this->front = 0;
    this->len = 0;
    if (NULL != this->subObj) {
        return this->subReset(this);
    } else {
        return PICO_OK;
    }
}

/* CharBuffer constructor */
picodata_CharBuffer picodata_newCharBuffer(picoos_MemoryManager mm,
        picoos_Common common,
        picoos_objsize_t size)
{
    picodata_CharBuffer this;

    this = (picodata_CharBuffer) picoos_allocate(mm, sizeof(*this));
    PICODBG_DEBUG(("new character buffer, size=%i", size));
    if (NULL == this) {
        return NULL;
    }
    this->buf = picoos_allocate(mm, size);
    if (NULL == this->buf) {
        picoos_deallocate(mm, (void*) &this);
        return NULL;
    }
    this->size = size;
    this->common = common;

    this->getItem = data_cbGetItem;
    this->putItem = data_cbPutItem;

    this->subReset = NULL;
    this->subDeallocate = NULL;
    this->subObj = NULL;

    picodata_cbReset(this);
    return this;
}

void picodata_disposeCharBuffer(picoos_MemoryManager mm,
                                picodata_CharBuffer *this)
{
    if (NULL != (*this)) {
        /* terminate */
        if (NULL != (*this)->subObj) {
            (*this)->subDeallocate(*this,mm);
        }
        picoos_deallocate(mm,(void*)&(*this)->buf);
        picoos_deallocate(mm,(void*)this);
    }
}

pico_status_t picodata_cbPutCh(register picodata_CharBuffer this,
                               picoos_char ch)
{
    if (this->len < this->size) {
        this->buf[this->rear++] = ch;
        this->rear %= this->size;
        this->len++;
        return PICO_OK;
    } else {
        return PICO_EXC_BUF_OVERFLOW;
    }
}


picoos_int16 picodata_cbGetCh(register picodata_CharBuffer this)
{
    picoos_char ch;
    if (this->len > 0) {
        ch = this->buf[this->front++];
        this->front %= this->size;
        this->len--;
        return ch;
    } else {
        return PICO_EOF;
    }
}

/* ***************************************************************
 *                   items: CharBuffer functions                 *
 *****************************************************************/

static pico_status_t data_cbGetItem(register picodata_CharBuffer this,
        picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen, const picoos_uint8 issd)
{
    picoos_uint16 i;

    if (this->len < PICODATA_ITEM_HEADSIZE) {    /* item not in cb? */
        *blen = 0;
        if (this->len == 0) {    /* is cb empty? */
            PICODBG_DEBUG(("no item to get"));
            return PICO_EOF;
        } else {    /* cb not empty, but not a valid item */
            PICODBG_WARN(("problem getting item, incomplete head, underflow"));
        }
        return PICO_EXC_BUF_UNDERFLOW;
    }
    *blen = PICODATA_ITEM_HEADSIZE + (picoos_uint8)(this->buf[((this->front) +
                                      PICODATA_ITEMIND_LEN) % this->size]);

    /* if getting speech data in item */
    if (issd) {
        /* check item type */
        if (this->buf[this->front] != PICODATA_ITEM_FRAME) {
            PICODBG_WARN(("item type mismatch for speech data: %c",
                          this->buf[this->front]));
            for (i=0; i<*blen; i++) {
                this->front++;
                this->front %= this->size;
                this->len--;
            }
            *blen = 0;
            return PICO_OK;
        }
    }

    if (*blen > this->len) {    /* item in cb not complete? */
        PICODBG_WARN(("problem getting item, incomplete content, underflow; "
                      "blen=%d, len=%d", *blen, this->len));
        *blen = 0;
        return PICO_EXC_BUF_UNDERFLOW;
    }
    if (blenmax < *blen) {    /* buf not large enough? */
        PICODBG_WARN(("problem getting item, overflow"));
        *blen = 0;
        return PICO_EXC_BUF_OVERFLOW;
    }

    /* if getting speech data in item */
    if (issd) {
        /* skip item header */
        for (i = 0; i < PICODATA_ITEM_HEADSIZE; i++) {
            this->front++;
            this->front %= this->size;
            this->len--;
        }
        *blen -= PICODATA_ITEM_HEADSIZE;
    }

    /* all ok, now get item (or speech data only) */
    for (i = 0; i < *blen; i++) {
        buf[i] = (picoos_uint8)(this->buf[this->front++]);
        this->front %= this->size;
        this->len--;
    }

#if defined(PICO_DEBUG)
    if (issd) {
        PICODBG_DEBUG(("got speech data: %d samples", *blen));
    } else {
        PICODBG_DEBUG(("got item: %c|%d|%d|%d||", buf[PICODATA_ITEMIND_TYPE],
                       buf[PICODATA_ITEMIND_INFO1], buf[PICODATA_ITEMIND_INFO2],
                       buf[PICODATA_ITEMIND_LEN]));
        for (i=PICODATA_ITEM_HEADSIZE; i<*blen; i++) {
            if (buf[PICODATA_ITEMIND_TYPE] == PICODATA_ITEM_WORDGRAPH) {
                PICODBG_DEBUG(("%c", buf[i]));
            } else {
                PICODBG_DEBUG((" %d", buf[i]));
            }
        }
    }
#endif

    return PICO_OK;
}

static pico_status_t data_cbPutItem(register picodata_CharBuffer this,
        const picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen)
{
    picoos_uint16 i;

    if (blenmax < PICODATA_ITEM_HEADSIZE) {    /* itemlen not accessible? */
        PICODBG_WARN(("problem putting item, underflow"));
        *blen = 0;
        return PICO_EXC_BUF_UNDERFLOW;
    }
    *blen = buf[PICODATA_ITEMIND_LEN] + PICODATA_ITEM_HEADSIZE;
    if (*blen > (this->size - this->len)) {    /* cb not enough space? */
        PICODBG_WARN(("problem putting item, overflow"));
        *blen = 0;
        return PICO_EXC_BUF_OVERFLOW;
    }
    if (*blen > blenmax) {    /* item in buf not completely accessible? */
        PICODBG_WARN(("problem putting item, underflow"));
        *blen = 0;
        return PICO_EXC_BUF_UNDERFLOW;
    }
    /* all ok, now put complete item */

#if defined(PICO_DEBUG)
    PICODBG_DEBUG(("putting item: %c|%d|%d|%d||",
                   buf[PICODATA_ITEMIND_TYPE],
                   buf[PICODATA_ITEMIND_INFO1],
                   buf[PICODATA_ITEMIND_INFO2],
                   buf[PICODATA_ITEMIND_LEN]));
    for (i=PICODATA_ITEM_HEADSIZE; i<*blen; i++) {
        if (buf[PICODATA_ITEMIND_TYPE] == PICODATA_ITEM_WORDGRAPH) {
            PICODBG_DEBUG(("%c", buf[i]));
        } else {
            PICODBG_DEBUG((" %d", buf[i]));
        }
    }
#endif

    for (i = 0; i < *blen; i++) {
        /* put single byte */
        this->buf[this->rear++] = (picoos_char)buf[i];
        this->rear %= this->size;
        this->len++;
    }
    return PICO_OK;
}

/*----------------------------------------------------------
 *  Names   : picodata_cbGetItem
 *            picodata_cbGetSpeechData
 *  Function: gets one item from 'this' and writes it on 'blenmax' sized 'buf'.
 *            gets one item from 'this' and writes the speech data to
 *              'blenmax' sized 'buf'.
 *  Returns : PICO_OK : one item was copied
 *            PICO_EOF : 'this' is empty; no new items available
 *            PICO_BUF_UNDERFLOW : 'this' doesn't contain a full/valid item
 *            PICO_BUF_OVERFLOW : 'buf[blenmax]' too small to hold item
 *            on return, '*blen' contains the number of bytes written to 'buf'
 * ---------------------------------------------------------*/
pico_status_t picodata_cbGetItem(register picodata_CharBuffer this,
        picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen)
{
    return this->getItem(this, buf, blenmax, blen, FALSE);
}

pico_status_t picodata_cbGetSpeechData(register picodata_CharBuffer this,
        picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen)
{

    return this->getItem(this, buf, blenmax, blen, TRUE);
}


pico_status_t picodata_cbPutItem(register picodata_CharBuffer this,
        const picoos_uint8 *buf, const picoos_uint16 blenmax,
        picoos_uint16 *blen)
{

        return this->putItem(this,buf,blenmax,blen);
}

/* unsafe, just for measuring purposes */
picoos_uint8 picodata_cbGetFrontItemType(register picodata_CharBuffer this)
{
    return  this->buf[this->front];
}
/* ***************************************************************
 *                   items: support function                     *
 *****************************************************************/

picoos_uint8 is_valid_itemtype(const picoos_uint8 ch) {
    switch (ch) {
        case PICODATA_ITEM_WSEQ_GRAPH:
        case PICODATA_ITEM_TOKEN:
        case PICODATA_ITEM_WORDGRAPH:
        case PICODATA_ITEM_WORDINDEX:
        case PICODATA_ITEM_WORDPHON:
        case PICODATA_ITEM_SYLLPHON:
        case PICODATA_ITEM_BOUND:
        case PICODATA_ITEM_PUNC:
        case PICODATA_ITEM_CMD:
        case PICODATA_ITEM_PHONE:
        case PICODATA_ITEM_FRAME:
        case PICODATA_ITEM_FRAME_PAR:
            return TRUE;
            break;
        case PICODATA_ITEM_OTHER:
        default:
            break;
    }
    PICODBG_WARN(("item type problem: %c", ch));
    return FALSE;
}

picoos_uint8 picodata_is_valid_itemhead(const picodata_itemhead_t *head) {
    if ((NULL != head) && is_valid_itemtype(head->type)) {
        return TRUE;
    } else {
        PICODBG_WARN(("item header problem"));
        return FALSE;
    }
}

/* ***************************************************/


pico_status_t picodata_get_itemparts_nowarn(
        const picoos_uint8 *buf, const picoos_uint16 blenmax,
        picodata_itemhead_t *head, picoos_uint8 *content,
        const picoos_uint16 clenmax, picoos_uint16 *clen)
{
    if (blenmax >= PICODATA_ITEM_HEADSIZE) {
        head->type = buf[PICODATA_ITEMIND_TYPE];
        head->info1 = buf[PICODATA_ITEMIND_INFO1];
        head->info2 = buf[PICODATA_ITEMIND_INFO2];
        head->len = buf[PICODATA_ITEMIND_LEN];
        *clen = head->len;
        if (blenmax >= (*clen + PICODATA_ITEM_HEADSIZE)) {
            if (clenmax >= head->len) {
                picoos_uint16 i;
                for (i=0; i<head->len; i++) {
                    content[i] = buf[PICODATA_ITEM_HEADSIZE+i];
                }
                return PICO_OK;
            }
            *clen = 0;
            return PICO_EXC_BUF_OVERFLOW;
        }
    }
    *clen = 0;
    return PICO_EXC_BUF_UNDERFLOW;
}

pico_status_t picodata_get_itemparts(
        const picoos_uint8 *buf, const picoos_uint16 blenmax,
        picodata_itemhead_t *head, picoos_uint8 *content,
        const picoos_uint16 clenmax, picoos_uint16 *clen)
{
    pico_status_t status = picodata_get_itemparts_nowarn(buf,blenmax,head,content,clenmax,clen);
    if (PICO_EXC_BUF_OVERFLOW == status) {
        PICODBG_WARN(("problem getting item, overflow"));
    } else if  (PICO_EXC_BUF_UNDERFLOW == status) {
        PICODBG_WARN(("problem getting item, overflow"));
    }
    return status;
}
pico_status_t picodata_put_itemparts(const picodata_itemhead_t *head,
        const picoos_uint8 *content, const picoos_uint16 clenmax,
        picoos_uint8 *buf, const picoos_uint16 blenmax, picoos_uint16 *blen)
{
    *blen = head->len + PICODATA_ITEM_HEADSIZE;
    if (blenmax >= *blen) {
        if (clenmax >= head->len) {
            picoos_uint16 i;
            buf[PICODATA_ITEMIND_TYPE] = head->type;
            buf[PICODATA_ITEMIND_INFO1] = head->info1;
            buf[PICODATA_ITEMIND_INFO2] = head->info2;
            buf[PICODATA_ITEMIND_LEN] = head->len;
            for (i=0; i<head->len; i++) {
                buf[PICODATA_ITEM_HEADSIZE+i] = content[i];
            }
            return PICO_OK;
        }
        PICODBG_WARN(("problem putting item, underflow"));
        *blen = 0;
        return PICO_EXC_BUF_UNDERFLOW;
    }
    PICODBG_WARN(("problem putting item, overflow"));
    *blen = 0;
    return PICO_EXC_BUF_OVERFLOW;
}

pico_status_t picodata_get_iteminfo(
        picoos_uint8 *buf, const picoos_uint16 blenmax,
        picodata_itemhead_t *head, picoos_uint8 **content) {
    if (blenmax >= PICODATA_ITEM_HEADSIZE) {
        head->type = buf[PICODATA_ITEMIND_TYPE];
        head->info1 = buf[PICODATA_ITEMIND_INFO1];
        head->info2 = buf[PICODATA_ITEMIND_INFO2];
        head->len = buf[PICODATA_ITEMIND_LEN];
        if (head->len == 0) {
            *content = NULL;
        } else {
            *content = &buf[PICODATA_ITEM_HEADSIZE];
        }
        return PICO_OK;
    }
    return PICO_EXC_BUF_UNDERFLOW;
}

pico_status_t picodata_copy_item(const picoos_uint8 *inbuf,
        const picoos_uint16 inlenmax, picoos_uint8 *outbuf,
        const picoos_uint16 outlenmax, picoos_uint16 *numb)
{
    if (picodata_is_valid_item(inbuf, inlenmax)) {
        *numb = PICODATA_ITEM_HEADSIZE + inbuf[PICODATA_ITEMIND_LEN];
    } else {
        *numb = 0;
    }
    if (*numb > 0) {
        if (outlenmax >= inlenmax) {
            picoos_uint16 i;
            for (i=0; i<*numb; i++) {
                outbuf[i] = inbuf[i];
            }
            return PICO_OK;
        } else {
            PICODBG_WARN(("buffer problem, out: %d > in: %d", outlenmax, inlenmax));
            *numb = 0;
            return PICO_EXC_BUF_OVERFLOW;
        }
    } else {
        PICODBG_WARN(("item problem in inbuf"));
        return PICO_ERR_OTHER;
    }
}

pico_status_t picodata_set_iteminfo1(picoos_uint8 *buf,
        const picoos_uint16 blenmax, const picoos_uint8 info) {
    if (PICODATA_ITEMIND_INFO1 < blenmax) {
        buf[PICODATA_ITEMIND_INFO1] = info;
        return PICO_OK;
    } else
        return PICO_EXC_BUF_UNDERFLOW;
}

pico_status_t picodata_set_iteminfo2(picoos_uint8 *buf,
        const picoos_uint16 blenmax, const picoos_uint8 info) {
    if (PICODATA_ITEMIND_INFO2 < blenmax) {
        buf[PICODATA_ITEMIND_INFO2] = info;
        return PICO_OK;
    } else
        return PICO_EXC_BUF_UNDERFLOW;
}

/* sets the len field in the header contained in the item in buf;
   return values:
   PICO_OK                 <- all ok
   PICO_EXC_BUF_UNDERFLOW  <- underflow in buf
*/
pico_status_t picodata_set_itemlen(picoos_uint8 *buf,
        const picoos_uint16 blenmax, const picoos_uint8 len) {
    if (PICODATA_ITEMIND_LEN < blenmax) {
        buf[PICODATA_ITEMIND_LEN] = len;
        return PICO_OK;
    } else
        return PICO_EXC_BUF_UNDERFLOW;
}

picoos_uint8 picodata_is_valid_item(const picoos_uint8 *item,
        const picoos_uint16 ilenmax)
{
    if (ilenmax >= PICODATA_ITEM_HEADSIZE) {
        picodata_itemhead_t head;
        head.type = item[0];
        head.info1 = item[1];
        head.info2 = item[2];
        head.len = item[3];
        if ((ilenmax >= (head.len + PICODATA_ITEM_HEADSIZE)) &&
            picodata_is_valid_itemhead(&head))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/* ***************************************************************
 *                   ProcessingUnit                              *
 *****************************************************************/
picoos_uint16 picodata_get_default_buf_size (picodata_putype_t puType)
{
    return  (PICODATA_PUTYPE_TEXT == puType) ? PICODATA_BUFSIZE_TEXT
          : (PICODATA_PUTYPE_TOK  == puType) ? PICODATA_BUFSIZE_TOK
          : (PICODATA_PUTYPE_PR   == puType) ? PICODATA_BUFSIZE_PR
          : (PICODATA_PUTYPE_PR   == puType) ? PICODATA_BUFSIZE_PR
          : (PICODATA_PUTYPE_WA   == puType) ? PICODATA_BUFSIZE_WA
          : (PICODATA_PUTYPE_SA   == puType) ? PICODATA_BUFSIZE_SA
          : (PICODATA_PUTYPE_ACPH == puType) ? PICODATA_BUFSIZE_ACPH
          : (PICODATA_PUTYPE_SPHO == puType) ? PICODATA_BUFSIZE_SPHO
          : (PICODATA_PUTYPE_PAM  == puType) ? PICODATA_BUFSIZE_PAM
          : (PICODATA_PUTYPE_CEP  == puType) ? PICODATA_BUFSIZE_CEP
          : (PICODATA_PUTYPE_SIG  == puType) ? PICODATA_BUFSIZE_SIG
          : (PICODATA_PUTYPE_SINK  == puType) ? PICODATA_BUFSIZE_SINK
          :                                    PICODATA_BUFSIZE_DEFAULT;
}


typedef struct simple_pu_data
{
    picorsrc_Voice voice;
} simple_pu_data_t;

static pico_status_t puSimpleInitialize (register picodata_ProcessingUnit this, picoos_int32 resetMode) {
    return PICO_OK;
}

static pico_status_t puSimpleTerminate (register picodata_ProcessingUnit this) {
    return PICO_OK;
}

static picodata_step_result_t puSimpleStep (register picodata_ProcessingUnit this,
                                            picoos_int16 mode,
                                            picoos_uint16 * numBytesOutput) {
    picoos_int16 ch;
    picoos_int16 result = PICO_OK;
    mode = mode;        /*PP 13.10.08 : fix warning "var not used in this function"*/
    *numBytesOutput = 0;
    while ((result == PICO_OK) && (ch = picodata_cbGetCh(this->cbIn)) != PICO_EOF) {
        result = picodata_cbPutCh(this->cbOut,(picoos_char) ch);
        (*numBytesOutput)++;
    }
    if (PICO_OK != result) {
        (*numBytesOutput)--;
    }
    if (PICO_OK == result) {
        return PICODATA_PU_IDLE;
    } else {
        return PICODATA_PU_ERROR;
    }
}


picodata_ProcessingUnit picodata_newProcessingUnit(
        picoos_MemoryManager mm,
        picoos_Common common,
        picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut,
        picorsrc_Voice voice)
{
    picodata_ProcessingUnit this = (picodata_ProcessingUnit) picoos_allocate(mm, sizeof(*this));
    if (this == NULL) {
        picoos_emRaiseException(common->em,PICO_EXC_OUT_OF_MEM,NULL,NULL);
        return NULL;
    }
    this->common = common;
    this->cbIn = cbIn;
    this->cbOut = cbOut;
    this->voice = voice;
    this->initialize = puSimpleInitialize;
    this->terminate = puSimpleTerminate;
    this->step = puSimpleStep;
    this->subDeallocate = NULL;
    this->subObj = NULL;
    return this;
}

void picodata_disposeProcessingUnit(
        picoos_MemoryManager mm,
        picodata_ProcessingUnit * this)
{
    if (NULL != (*this)) {
        /* terminate */
        if (NULL != (*this)->subObj) {
            (*this)->subDeallocate(*this,mm);
        }
        picoos_deallocate(mm,(void *)this);
    }

}


picodata_CharBuffer picodata_getCbIn(picodata_ProcessingUnit this)
{
    if (NULL == this) {
        picoos_emRaiseException(this->common->em,PICO_ERR_NULLPTR_ACCESS,NULL,NULL);
        return NULL;
    } else {
        return this->cbIn;
    }
}

picodata_CharBuffer picodata_getCbOut(picodata_ProcessingUnit this)
{
    if (NULL == this) {
        picoos_emRaiseException(this->common->em,PICO_ERR_NULLPTR_ACCESS,NULL,NULL);
        return NULL;
    } else {
        return this->cbOut;
    }
}

pico_status_t picodata_setCbIn(picodata_ProcessingUnit this, picodata_CharBuffer cbIn)
{
    if (NULL == this) {
        picoos_emRaiseException(this->common->em,PICO_ERR_NULLPTR_ACCESS,NULL,NULL);
        return PICO_ERR_OTHER;
    } else {
        this->cbIn = cbIn;
        return PICO_OK;
    }

}

pico_status_t picodata_setCbOut(picodata_ProcessingUnit this, picodata_CharBuffer cbOut)
{
    if (NULL == this) {
        picoos_emRaiseException(this->common->em,PICO_ERR_NULLPTR_ACCESS,NULL,NULL);
        return PICO_ERR_OTHER;
    } else {
        this->cbOut = cbOut;
        return PICO_OK;
    }
}


/* ***************************************************************
 *                   auxiliary functions                         *
 *****************************************************************/

static void transDurUniform(
        picoos_uint8 frame_duration_exp, /* 2's exponent of frame duration in ms, e.g. 2 for 4ms, 3 for 8ms */
        picoos_int8 array_length,
        picoos_uint8 * inout,
        picoos_int16 inputdur, /* input duration in ms */
        picoos_int16 targetdur, /* target duration in ms */
        picoos_int16 * restdur /* in/out, rest in ms */
        )
{
    picoos_int8 i;
    picoos_int32 fact, rest;

    /* calculate rest and factor in number of frames (in PICODATA_PICODATA_PRECISION) */
    rest = (*restdur) << (PICODATA_PRECISION - frame_duration_exp);
    fact = (targetdur << (PICODATA_PRECISION - frame_duration_exp)) / inputdur;

    for (i = 0; i < array_length; i++) {
        rest += fact * inout[i];
        /* instead of rounding, we carry the rest to the next state */
        inout[i] = rest >> PICODATA_PRECISION;
        rest -= inout[i] << PICODATA_PRECISION;
    }
    (*restdur) = rest >> (PICODATA_PRECISION - frame_duration_exp);
}

static void transDurWeighted(
        picoos_uint8 frame_duration_exp, /* 2's exponent of frame duration in ms, e.g. 2 for 4ms, 3 for 8ms */
        picoos_int8 array_length,
        picoos_uint8 * inout,
        const picoos_uint16 * weight,  /* integer weights */
        picoos_int16 inputdur, /* input duration in ms */
        picoos_int16 targetdur, /* target duration in ms */
        picoos_int16 * restdur /* in/out, rest in ms */
        )
{
    picoos_int8 i;
    picoos_int32 fact, rest, out, weighted_sum;

    /* calculate rest and factor in number of frames (in PICODATA_PICODATA_PRECISION) */
    rest = (*restdur) << (PICODATA_PRECISION - frame_duration_exp);
    weighted_sum = 0;
    for (i=0; i < array_length; i++) {
        weighted_sum += inout[i] * weight[i];
    }
    if (0 == weighted_sum) {
        transDurUniform(frame_duration_exp,array_length,inout,inputdur,targetdur,restdur);
        return;
    }
    /* get the additive change factor in PICODATA_PRECISION: */
    if (targetdur > inputdur) {
        fact = ((targetdur - inputdur) << (PICODATA_PRECISION-frame_duration_exp))/ weighted_sum;
    } else {
        fact = -((inputdur - targetdur) << (PICODATA_PRECISION-frame_duration_exp))/ weighted_sum;
    }

    /* input[i] * fact * weight[i] is the additive modification in PICODATA_PRECISION */
    for (i=0; i < array_length; i++) {
        rest += fact * inout[i] * weight[i];
        /* instead of rounding, we carry the rest to the next state */
        out = inout[i] + (rest >> PICODATA_PRECISION);
        if (out < 0) {
            out = 0;
        }
        rest -= ((out-inout[i]) << PICODATA_PRECISION);
        inout[i] = out;
    }
   (*restdur) = rest >> (PICODATA_PRECISION - frame_duration_exp);
}



void picodata_transformDurations(
        picoos_uint8 frame_duration_exp,
        picoos_int8 array_length,
        picoos_uint8 * inout,
        const picoos_uint16 * weight,  /* integer weights */
        picoos_int16 mintarget, /* minimum target duration in ms */
        picoos_int16 maxtarget, /* maximum target duration in ms */
        picoos_int16 facttarget, /* factor to be multiplied with original length to get the target
                                     the factor is fixed-point with PICODATA_PRECISION PICODATA_PRECISION, i.e.
                                     the factor as float would be facttarget / PICODATA_PRECISION_FACT
                                     if factor is 0, only min/max are considered */
        picoos_int16 * dur_rest /* in/out, rest in ms */
        )
{
    picoos_int32 inputdur, targetdur;
    picoos_int8 i;

    /* find the original duration in ms */
    inputdur = 0;
    for (i=0; i < array_length; i++) {
        inputdur += inout[i];
    }

    PICODBG_TRACE(("######## transforming duration fact=%i, limits = [%i,%i] (input frames: %i)",facttarget,mintarget,maxtarget, inputdur));

    inputdur <<= frame_duration_exp;

    /* find the target duration */
    if (facttarget) {
        targetdur = (facttarget * inputdur + PICODATA_PREC_HALF) >> PICODATA_PRECISION;
    } else {
        targetdur = inputdur;
    }

    /* we need to modify input if there is an explicit factor or input is not in the target range */
    if (facttarget || (targetdur < mintarget) || (maxtarget < targetdur)) {
        /* make sure we are in the limits */
        if (targetdur < mintarget) {
            targetdur = mintarget;
        } else if (maxtarget < targetdur) {
            targetdur = maxtarget;
        }
        /* perform modification */
        if (NULL == weight) {
            transDurUniform(frame_duration_exp,array_length,inout,inputdur,targetdur,dur_rest);
        } else {
            transDurWeighted(frame_duration_exp,array_length,inout,weight,inputdur,targetdur,dur_rest);
        }
    }
}



extern picoos_uint8 picodata_getPuTypeFromExtension(picoos_uchar * filename, picoos_bool input)
{
    if (input) {
        if (picoos_has_extension(filename, PICODATA_PUTYPE_TOK_INPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_TOK;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_PR_INPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_PR;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_WA_INPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_WA;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_SA_INPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_SA;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_ACPH_INPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_ACPH;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_SPHO_INPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_SPHO;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_PAM_INPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_PAM;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_CEP_INPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_CEP;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_SIG_INPUT_EXTENSION) ||
                picoos_has_extension(filename, PICODATA_PUTYPE_WAV_INPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_SIG;
        }
        else {
            return PICODATA_ITEMINFO2_CMD_TO_UNKNOWN;
        }
    }
    else {
        if (picoos_has_extension(filename, PICODATA_PUTYPE_TOK_OUTPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_TOK;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_PR_OUTPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_PR;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_WA_OUTPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_WA;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_SA_OUTPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_SA;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_ACPH_OUTPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_ACPH;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_SPHO_OUTPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_SPHO;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_PAM_OUTPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_PAM;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_CEP_OUTPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_CEP;
        }
        else if (picoos_has_extension(filename, PICODATA_PUTYPE_SIG_OUTPUT_EXTENSION) ||
                picoos_has_extension(filename, PICODATA_PUTYPE_WAV_OUTPUT_EXTENSION)) {
            return PICODATA_ITEMINFO2_CMD_TO_SIG;
        }
        else {
            return PICODATA_ITEMINFO2_CMD_TO_UNKNOWN;
        }
    }
    return PICODATA_ITEMINFO2_CMD_TO_UNKNOWN;
}




/**
 *
 * @param transducer
 * @param common
 * @param xsampa_parser
 * @param svoxpa_parser
 * @param xsampa2svoxpa_mapper
 * @param inputPhones
 * @param alphabet
 * @param outputPhoneIds
 * @param maxOutputPhoneIds
 * @return
 */
pico_status_t picodata_mapPAStrToPAIds(picotrns_SimpleTransducer transducer,
        picoos_Common common, picokfst_FST xsampa_parser,
        picokfst_FST svoxpa_parser, picokfst_FST xsampa2svoxpa_mapper,
        picoos_uchar * inputPhones, picoos_uchar * alphabet,
        picoos_uint8 * outputPhoneIds, picoos_int32 maxOutputPhoneIds)
{
    pico_status_t status = PICO_OK;
    if (picoos_strcmp(alphabet, PICODATA_XSAMPA) == 0) {
        if ((NULL != xsampa_parser) && (NULL != xsampa2svoxpa_mapper)) {
            picotrns_stInitialize(transducer);
            status = picotrns_stAddWithPlane(transducer, inputPhones,
                    PICOKFST_PLANE_ASCII);
            if (PICO_OK != status) {
                picoos_emRaiseWarning(common->em, status, NULL,
                        (picoos_char *) "phoneme sequence too long (%s)",
                        inputPhones);
            } else {
                status = picotrns_stTransduce(transducer, xsampa_parser);
                if (PICO_OK != status) {
                    picoos_emRaiseWarning(common->em, status, NULL,
                            (picoos_char *) "not recognized as xsampa (%s)",
                            inputPhones);
                } else {
                    status = picotrns_stTransduce(transducer, xsampa2svoxpa_mapper);
                    if (PICO_OK != status) {
                        picoos_emRaiseWarning(common->em, status, NULL,
                                (picoos_char *) "illeagal phoneme sequence (%s)",
                                inputPhones);
                    } else {
                        status = picotrns_stGetSymSequence(transducer, outputPhoneIds,
                                maxOutputPhoneIds);
                    }
                }
            }
            return status;
        }
    } else if (picoos_strcmp(alphabet, PICODATA_SVOXPA) == 0) {
        if ((NULL != svoxpa_parser)) {
            picotrns_stInitialize(transducer);
            status = picotrns_stAddWithPlane(transducer, inputPhones,
                    PICOKFST_PLANE_ASCII);
            if (PICO_OK == status) {
                status = picotrns_stTransduce(transducer, svoxpa_parser);
            }
            if (PICO_OK == status) {
                status = picotrns_stGetSymSequence(transducer, outputPhoneIds,
                        maxOutputPhoneIds);
            }
            return status;
        }
    }
    picoos_strlcpy(outputPhoneIds, (picoos_char *) "", maxOutputPhoneIds);
    picoos_emRaiseWarning(common->em, PICO_EXC_NAME_ILLEGAL, NULL,
            (picoos_char *) "alphabet not supported (%s)", alphabet);
    return PICO_EXC_NAME_ILLEGAL;

}

#if defined (PICO_DEBUG)
/* ***************************************************************
 *                   For Debugging only                          *
 *****************************************************************/


/* put string representation of 'itemtype' into 'str' (allocated size 'strsize')
 * return 'str' */
static picoos_char * data_itemtype_to_string(const picoos_uint8 itemtype,
        picoos_char * str, picoos_uint16 strsize)
{
    picoos_char * tmpstr;
    switch (itemtype) {
        case PICODATA_ITEM_BOUND:
            tmpstr = (picoos_char *)"BOUND";
            break;
        case PICODATA_ITEM_FRAME_PAR:
            tmpstr = (picoos_char *)"FRAME_PAR";
            break;
        case PICODATA_ITEM_PHONE:
            tmpstr = (picoos_char *)"PHONE";
            break;
        case PICODATA_ITEM_CMD:
            tmpstr = (picoos_char *)"CMD";
            break;
        case PICODATA_ITEM_ERR:
            tmpstr = (picoos_char *)"ERR";
            break;
        case PICODATA_ITEM_FRAME:
            tmpstr = (picoos_char *)"FRAME";
            break;
        case PICODATA_ITEM_OTHER:
            tmpstr = (picoos_char *)"OTHER";
            break;
        case PICODATA_ITEM_PUNC:
            tmpstr = (picoos_char *)"PUNC";
            break;
        case PICODATA_ITEM_SYLLPHON:
            tmpstr = (picoos_char *)"SYLLPHON";
            break;
        case PICODATA_ITEM_WORDGRAPH:
            tmpstr = (picoos_char *)"WORDGRAPH";
            break;
        case PICODATA_ITEM_WORDINDEX:
            tmpstr = (picoos_char *)"WORDINDEX";
            break;
        case PICODATA_ITEM_WORDPHON:
            tmpstr = (picoos_char *)"WORDPHON";
            break;
        case PICODATA_ITEM_WSEQ_GRAPH:
            tmpstr = (picoos_char *)"WSEQ_GRAPH";
            break;
        default:
            tmpstr = (picoos_char *)"UNKNOWN";
            break;
    }
    picopal_slprintf((picopal_char *) str, strsize, (picopal_char *)"%s",
            tmpstr);
    return str;
}


picoos_char * picodata_head_to_string(const picodata_itemhead_t *head,
        picoos_char * str, picoos_uint16 strsize)
{
    picoos_uint16 typelen;

    if (NULL == head) {
        picoos_strlcpy(str,(picoos_char *)"[head is NULL]",strsize);
    } else {
        data_itemtype_to_string(head->type, str, strsize);
        typelen = picoos_strlen(str);
        picopal_slprintf((picopal_char *) str+typelen, strsize-typelen,
                (picopal_char *)"|%c|%c|%i", head->info1, head->info2,
                head->len);
    }

    return str;
}

void picodata_info_item(const picoknow_KnowledgeBase kb,
                        const picoos_uint8 *pref6ch,
                        const picoos_uint8 *item,
                        const picoos_uint16 itemlenmax,
                        const picoos_char *filterfn)
{
#define SA_USE_PHST 1
    picoos_uint16 i;
    picoos_uint8 ch;

    if ((itemlenmax < 4) || (item == NULL)) {
        PICODBG_INFO_MSG(("invalid item\n"));
    }

    /* first 6 char used for prefix */
    PICODBG_INFO_MSG_F(filterfn, ("%6s(", pref6ch));

    /* type */
    ch = item[0];
    if ((32 <= ch) && (ch < 127)) {
        PICODBG_INFO_MSG_F(filterfn, ("'%c',", ch));
    } else {
        PICODBG_INFO_MSG_F(filterfn, ("%3d,", ch));
    }
    /* info1 */
    ch = item[1];
    if ((32 <= ch) && (ch < 127))
        switch (item[0]) {
            case PICODATA_ITEM_PUNC:
            case PICODATA_ITEM_BOUND:
            case PICODATA_ITEM_CMD:
            case PICODATA_ITEM_TOKEN:
            case PICODATA_ITEM_FRAME_PAR:
                PICODBG_INFO_MSG_F(filterfn, ("'%c',", ch));
                break;
            default:
                PICODBG_INFO_MSG_F(filterfn, ("%3d,", ch));
        }
    else
        PICODBG_INFO_MSG_F(filterfn, ("%3d,", ch));

    /* info2 */
    ch = item[2];
    if ((32 <= ch) && (ch < 127))
        switch (item[0]) {
            case PICODATA_ITEM_PUNC:
            case PICODATA_ITEM_BOUND:
            case PICODATA_ITEM_CMD:
            case PICODATA_ITEM_WORDPHON:
            case PICODATA_ITEM_SYLLPHON:
                PICODBG_INFO_MSG_F(filterfn, ("'%c',", ch));
                break;
            default:
                PICODBG_INFO_MSG_F(filterfn, ("%3d,", ch));
        }
    else
        PICODBG_INFO_MSG_F(filterfn, ("%3d,", ch));

    /* len */
    ch = item[3];
    PICODBG_INFO_MSG_F(filterfn, ("%3d)", ch));

    for (i = 0; i < ch; i++) {
        if ((item[0] == PICODATA_ITEM_WSEQ_GRAPH) ||
            (item[0] == PICODATA_ITEM_TOKEN) ||
            (item[0] == PICODATA_ITEM_WORDGRAPH) ||
            ((item[0] == PICODATA_ITEM_CMD) && !((item[1] == PICODATA_ITEMINFO1_CMD_SPEED) ||
                                                 (item[1] == PICODATA_ITEMINFO1_CMD_PITCH) ||
                                                 (item[1] == PICODATA_ITEMINFO1_CMD_VOLUME) ||
                                                 (item[1] == PICODATA_ITEMINFO1_CMD_SPELL) ||
                                                 (item[1] == PICODATA_ITEMINFO1_CMD_SIL)))) {
            PICODBG_INFO_MSG_F(filterfn, ("%c", item[4 + i]));
        } else {
            PICODBG_INFO_MSG_F(filterfn, ("%4d", item[4 + i]));
        }
    }

#if defined (SA_USE_PHST)
    {
#include "picokdbg.h"
        picoos_uint8 j;
        picokdbg_Dbg kdbg;
        kdbg = picokdbg_getDbg(kb);

        if ((item[0] == PICODATA_ITEM_WORDPHON) ||
                (item[0] == PICODATA_ITEM_SYLLPHON) ||
                ((item[0] == PICODATA_ITEM_CMD) && (item[1] == PICODATA_ITEMINFO1_CMD_PHONEME))) {
            if (picokdbg_getPhoneSym(kdbg, item[4])) {
                PICODBG_INFO_MSG_F(filterfn, ("  "));
                for (j = 0; j < item[3]; j++) {
                    PICODBG_INFO_MSG_F(filterfn, ("%s",
                            picokdbg_getPhoneSym(kdbg, item[4 + j])));
                }
            }
        }
    }
#endif

    PICODBG_INFO_MSG_F(filterfn, ("\n"));
}


#endif   /* PICO_DEBUG */

#ifdef __cplusplus
}
#endif


/* picodata.c end */
