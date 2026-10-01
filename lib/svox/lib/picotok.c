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
 * @file picotok.c
 *
 * tokenizer
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */


/* ************************************************************/
/* tokenisation and markup handling */
/* ************************************************************/

/** @addtogroup picotok
  @b tokenisation_overview

  markup handling overview:

  The following markups are recognized
     - ignore
     - speed
     - pitch
     - volume
     - voice
     - preproccontext
     - mark
     - play
     - usesig
     - genfile
     - sentence
     - s
     - paragraph
     - p
     - break
     - spell            (pauses between letter)
     - phoneme

  All markups which are recognized but are not yet implemented in pico
  system have the mark.
*/


#include "picodefs.h"
#include "picoos.h"
#include "picobase.h"
#include "picodbg.h"
#include "picodata.h"
#include "picotok.h"
#include "picoktab.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* *****************************************************************************/

#define IN_BUF_SIZE   255
#define OUT_BUF_SIZE  IN_BUF_SIZE + 3 * PICODATA_ITEM_HEADSIZE + 3

#define MARKUP_STRING_BUF_SIZE (IN_BUF_SIZE*5)
#define MAX_NR_MARKUP_PARAMS 6
#define MARKUP_HANDLING_DISABLED  0
#define MARKUP_HANDLING_ENABLED 1
#define EOL '\n'


typedef picoos_int8 pico_tokenSubType;
typedef picoos_uint8 pico_tokenType;

/** @todo : consider adding these specialized exception codes: */

#define PICO_ERR_MARKUP_VALUE_OUT_OF_RANGE PICO_ERR_OTHER
#define PICO_ERR_INVALID_MARKUP_TAG        PICO_ERR_OTHER
#define PICO_ERR_INTERNAL_LIMIT            PICO_ERR_OTHER

typedef enum {MIDummyStart, MIIgnore,
              MIPitch, MISpeed, MIVolume,
              MIVoice, MIPreprocContext, MIMarker,
              MIPlay, MIUseSig, MIGenFile, MIParagraph,
              MISentence, MIBreak, MISpell, MIPhoneme, MIItem, MISpeaker, MIDummyEnd
             }  MarkupId;
typedef enum {MSNotInMarkup, MSGotStart, MSExpectingmarkupTagName, MSInmarkupTagName,
              MSGotmarkupTagName, MSInAttrName, MSGotAttrName, MSGotEqual, MSInAttrValue,
              MSInAttrValueEscaped, MSGotAttrValue, MSGotEndSlash, MSGotEnd,
              MSError, MSErrorTooLong, MSErrorSyntax
             }  MarkupState;
typedef enum {MENone, MEMissingStart, MEUnknownTag, MEIdent, MEMissingEqual,
              MEMissingQuote, MEMissingEnd, MEUnexpectedChar, MEInterprete
             }  MarkupParseError;

typedef enum {MTNone, MTStart, MTEnd, MTEmpty} MarkupTagType;

#define UTF_CHAR_COMPLETE   2
#define UTF_CHAR_INCOMPLETE 1
#define UTF_CHAR_MALFORMED  0

#define TOK_MARKUP_KW_IGNORE     (picoos_uchar*)"ignore"
#define TOK_MARKUP_KW_SPEED      (picoos_uchar*)"speed"
#define TOK_MARKUP_KW_PITCH      (picoos_uchar*)"pitch"
#define TOK_MARKUP_KW_VOLUME     (picoos_uchar*)"volume"
#define TOK_MARKUP_KW_VOICE      (picoos_uchar*)"voice"
#define TOK_MARKUP_KW_CONTEXT    (picoos_uchar*)"preproccontext"
#define TOK_MARKUP_KW_MARK       (picoos_uchar*)"mark"
#define TOK_MARKUP_KW_PLAY       (picoos_uchar*)"play"
#define TOK_MARKUP_KW_USESIG     (picoos_uchar*)"usesig"
#define TOK_MARKUP_KW_GENFILE    (picoos_uchar*)"genfile"
#define TOK_MARKUP_KW_SENTENCE   (picoos_uchar*)"sentence"
#define TOK_MARKUP_KW_S          (picoos_uchar*)"s"
#define TOK_MARKUP_KW_PARAGRAPH  (picoos_uchar*)"paragraph"
#define TOK_MARKUP_KW_P          (picoos_uchar*)"p"
#define TOK_MARKUP_KW_BREAK      (picoos_uchar*)"break"
#define TOK_MARKUP_KW_SPELL      (picoos_uchar*)"spell"
#define TOK_MARKUP_KW_PHONEME    (picoos_uchar*)"phoneme"
#define TOK_MARKUP_KW_ITEM       (picoos_uchar*)"item"
#define TOK_MARKUP_KW_SPEAKER    (picoos_uchar*)"speaker"

#define KWLevel (picoos_uchar *)"level"
#define KWName (picoos_uchar *)"name"
#define KWProsDomain (picoos_uchar *)"prosodydomain"
#define KWTime (picoos_uchar *)"time"
#define KWMode (picoos_uchar *)"mode"
#define KWSB (picoos_uchar *)"sb"
#define KWPB (picoos_uchar *)"pb"
#define KWFile (picoos_uchar *)"file"
#define KWType (picoos_uchar *)"type"
#define KWF0Beg (picoos_uchar *)"f0beg"
#define KWF0End (picoos_uchar *)"f0end"
#define KWXFadeBeg (picoos_uchar *)"xfadebeg"
#define KWXFadeEnd (picoos_uchar *)"xfadeend"
#define KWAlphabet (picoos_uchar *)"alphabet"
#define KWPH (picoos_uchar *)"ph"
#define KWOrthMode (picoos_uchar *)"orthmode"
#define KWIgnorePunct (picoos_uchar *)"ignorepunct"
#define KWInfo1 (picoos_uchar *)"info1"
#define KWInfo2 (picoos_uchar *)"info2"
#define KWDATA (picoos_uchar *)"data"

#define PICO_SPEED_MIN           20
#define PICO_SPEED_MAX          500
#define PICO_SPEED_DEFAULT      100
#define PICO_SPEED_FACTOR_MIN   500
#define PICO_SPEED_FACTOR_MAX  2000

#define PICO_PITCH_MIN           50
#define PICO_PITCH_MAX          200
#define PICO_PITCH_DEFAULT      100
#define PICO_PITCH_FACTOR_MIN   500
#define PICO_PITCH_FACTOR_MAX  2000
#define PICO_PITCH_ADD_MIN     -100
#define PICO_PITCH_ADD_MAX      100
#define PICO_PITCH_ADD_DEFAULT    0

#define PICO_VOLUME_MIN           0
#define PICO_VOLUME_MAX         500
#define PICO_VOLUME_DEFAULT     100
#define PICO_VOLUME_FACTOR_MIN  500
#define PICO_VOLUME_FACTOR_MAX 2000

#define PICO_SPEAKER_MIN          20
#define PICO_SPEAKER_MAX         180
#define PICO_SPEAKER_DEFAULT     100
#define PICO_SPEAKER_FACTOR_MIN  500
#define PICO_SPEAKER_FACTOR_MAX 2000

#define PICO_CONTEXT_DEFAULT   (picoos_uchar*)"DEFAULT"

#define PARAGRAPH_PAUSE_DUR 500
#define SPELL_WITH_PHRASE_BREAK  1
#define SPELL_WITH_SENTENCE_BREAK  2

/* *****************************************************************************/

#define TOK_PUNC_FLUSH  (picoos_char) '\0'

typedef picoos_uchar Word[MARKUP_STRING_BUF_SIZE];


struct MarkupParam {
    Word paramId;
    Word paramVal;
};

typedef struct MarkupParam MarkupParams[MAX_NR_MARKUP_PARAMS];

typedef picoos_uchar utf8char0c[5]; /* one more than needed so it is ended always with 0c*/

/** subobject : TokenizeUnit
 *  shortcut  : tok
 */
typedef struct tok_subobj
{
    picoos_int32 ignLevel;

    utf8char0c   utf;
    picoos_int32 utfpos;
    picoos_int32 utflen;

    MarkupParams markupParams;
    picoos_int32 nrMarkupParams;
    MarkupState markupState;
    picoos_uchar markupStr[MARKUP_STRING_BUF_SIZE];
    picoos_int32 markupPos;
    picoos_int32 markupLevel[MIDummyEnd+1];
    picoos_uchar markupTagName[IN_BUF_SIZE];
    MarkupTagType markupTagType;
    MarkupParseError markupTagErr;

    picoos_int32 strPos;
    picoos_uchar strDelim;
    picoos_bool isFileAttr;

    pico_tokenType tokenType;
    pico_tokenSubType tokenSubType;

    picoos_int32 tokenPos;
    picoos_uchar tokenStr[IN_BUF_SIZE];

    picoos_int32 nrEOL;

    picoos_bool markupHandlingMode;       /* to be moved ??? */
    picoos_bool aborted;                  /* to be moved ??? */

    picoos_bool start;

    picoos_uint8 outBuf[OUT_BUF_SIZE]; /* internal output buffer */
    picoos_uint16 outReadPos; /* next pos to read from outBuf */
    picoos_uint16 outWritePos; /* next pos to write to outBuf */

    picoos_uchar saveFile[IN_BUF_SIZE];
    Word phonemes;

    picotrns_SimpleTransducer transducer;

    /* kbs */

    picoktab_Graphs graphTab;
    picokfst_FST xsampa_parser;
    picokfst_FST svoxpa_parser;
    picokfst_FST xsampa2svoxpa_mapper;



} tok_subobj_t;

/* *****************************************************************************/

static void tok_treatMarkupAsSimpleToken (picodata_ProcessingUnit this, tok_subobj_t * tok);
static void tok_treatChar (picodata_ProcessingUnit this, tok_subobj_t * tok, picoos_uchar ch, picoos_bool markupHandling);
static void tok_treatMarkup (picodata_ProcessingUnit this, tok_subobj_t * tok);
static void tok_putToMarkup (picodata_ProcessingUnit this, tok_subobj_t * tok, picoos_uchar str[]);
static void tok_treatSimpleToken (picodata_ProcessingUnit this, tok_subobj_t * tok);
static MarkupId tok_markupTagId (picoos_uchar tagId[]);

/* *****************************************************************************/

static picoos_bool tok_strEqual(picoos_uchar * str1, picoos_uchar * str2)
{
   return (picoos_strcmp((picoos_char*)str1, (picoos_char*)str2) == 0);
}

static void tok_reduceBlanks(picoos_uchar * str)
            /* Remove leading and trailing blanks of 'str' and reduce
               groups of blanks within string to exactly one blank. */

{
    int i = 0;
    int j = 0;

     while (str[j] != 0) {
        if (str[j] == (picoos_uchar)' ') {
            /* note one blank except at the beginning of string */
            if (i > 0) {
                str[i] = (picoos_uchar)' ';
                i++;
            }
            j++;
            while (str[j] == (picoos_uchar)' ') {
                j++;
            }
        } else {
            str[i] = str[j];
            j++;
            i++;
        }
    }

    /* remove blanks at end of string */
    if ((i > 0) && (str[i - 1] == ' ')) {
        i--;
    }
    str[i] = 0;
}


static void tok_startIgnore (tok_subobj_t * tok)
{
    tok->ignLevel++;
}


static void tok_endIgnore (tok_subobj_t * tok)
{
    if (tok->ignLevel > 0) {
        tok->ignLevel--;
    }
}


static void tok_getParamIntVal (MarkupParams params, picoos_uchar paramId[], picoos_int32 * paramVal, picoos_bool * paramFound)
{
    int i=0;

    while ((i < MAX_NR_MARKUP_PARAMS) && !tok_strEqual(paramId,params[i].paramId)) {
        i++;
    }
    if ((i < MAX_NR_MARKUP_PARAMS)) {
        (*paramVal) = picoos_atoi((picoos_char*)params[i].paramVal);
        (*paramFound) = TRUE;
    } else {
        (*paramVal) =  -1;
        (*paramFound) = FALSE;
    }
}



static void tok_getParamStrVal (MarkupParams params, picoos_uchar paramId[], picoos_uchar paramStrVal[], picoos_bool * paramFound)
{
    int i=0;

    while ((i < MAX_NR_MARKUP_PARAMS) &&  !tok_strEqual(paramId,params[i].paramId)) {
        i++;
    }
    if (i < MAX_NR_MARKUP_PARAMS) {
        picoos_strcpy((picoos_char*)paramStrVal, (picoos_char*)params[i].paramVal);
        (*paramFound) = TRUE;
    } else {
        paramStrVal[0] = 0;
        (*paramFound) = FALSE;
    }
}


static void tok_getParamPhonesStr (MarkupParams params, picoos_uchar paramId[], picoos_uchar alphabet[], picoos_uchar phones[], picoos_int32 phoneslen, picoos_bool * paramFound)
{

    int i;
    picoos_bool done;

    i = 0;
    while ((i < MAX_NR_MARKUP_PARAMS) &&  !tok_strEqual(paramId, params[i].paramId)) {
        i++;
    }
    if (i < MAX_NR_MARKUP_PARAMS) {
        if (tok_strEqual(alphabet, PICODATA_XSAMPA) || tok_strEqual(alphabet, (picoos_uchar*)"")) {
            picoos_strlcpy((picoos_char*)phones, (picoos_char*)params[i].paramVal, phoneslen);
            done = TRUE;
        } else {
            done = FALSE;
        }
        (*paramFound) = TRUE;
    } else {
        done = FALSE;
        (*paramFound) = FALSE;
    }
    if (!done) {
        phones[0] = 0;
    }
}


static void tok_clearMarkupParams (MarkupParams params)
{
    int i;

    for (i = 0; i<MAX_NR_MARKUP_PARAMS; i++) {
        params[i].paramId[0] = 0;
        params[i].paramVal[0] = 0;
    }
}


static void tok_getDur (picoos_uchar durStr[], picoos_uint32 * dur, picoos_bool * done)
{

    int num=0;
    int i=0;
    picoos_uchar tmpWord[IN_BUF_SIZE];

    picoos_strlcpy((picoos_char*)tmpWord, (picoos_char*)durStr, sizeof(tmpWord));
    tok_reduceBlanks(tmpWord);
    while ((durStr[i] >= '0') && (durStr[i] <= '9')) {
        num = 10 * num + (int)durStr[i] - (int)'0';
        tmpWord[i] = ' ';
        i++;
    }
    tok_reduceBlanks(tmpWord);
    if (tok_strEqual(tmpWord, (picoos_uchar*)"s")) {
        (*dur) = (1000 * num);
        (*done) = TRUE;
    } else if (tok_strEqual(tmpWord,(picoos_uchar*)"ms")) {
        (*dur) = num;
        (*done) = TRUE;
    } else {
        (*dur) = 0;
        (*done) = FALSE;
    }
}


static picoos_int32 tok_putToUtf (tok_subobj_t * tok, picoos_uchar ch)
{
    if (tok->utfpos < PICOBASE_UTF8_MAXLEN) {
        tok->utf[tok->utfpos] = ch;
        if (tok->utfpos == 0) {
            tok->utflen = picobase_det_utf8_length(ch);
        } else if (((ch < (picoos_uchar)'\200') || (ch >= (picoos_uchar)'\300'))) {
            tok->utflen = 0;
        }
        (tok->utfpos)++;
        if ((tok->utfpos == tok->utflen)) {
            if ((tok->utfpos < PICOBASE_UTF8_MAXLEN)) {
                tok->utf[tok->utfpos] = 0;
            }
            return UTF_CHAR_COMPLETE;
        } else if (tok->utfpos < tok->utflen) {
            return UTF_CHAR_INCOMPLETE;
        } else {
            return UTF_CHAR_MALFORMED;
        }
    } else {
        return UTF_CHAR_MALFORMED;
    }
}


static picoos_bool tok_isRelative (picoos_uchar strval[], picoos_uint32 * val)
{
    picoos_int32 len;
    picoos_bool rel;

    rel = FALSE;
    len = picoos_strlen((picoos_char*)strval);
    if (len > 0) {
        if (strval[len - 1] == '%') {
            strval[len - 1] = 0;
            if ((strval[0] == '+') || (strval[0] == '-')) {
                (*val) = 1000 + (picoos_atoi((picoos_char*)strval) * 10);
            } else {
                (*val) = picoos_atoi((picoos_char*)strval) * 10;
            }
            rel = TRUE;
        }
    }
    return rel;
}


static void tok_putItem (picodata_ProcessingUnit this,  tok_subobj_t * tok,
                         picoos_uint8 itemType, picoos_uint8 info1, picoos_uint8 info2,
                         picoos_uint16 val,
                         picoos_uchar str[])
{
    picoos_int32 len, i;

    if ((itemType == PICODATA_ITEM_CMD) && (info1 == PICODATA_ITEMINFO1_CMD_FLUSH)) {
        tok->outBuf[tok->outWritePos++] = itemType;
        tok->outBuf[tok->outWritePos++] = info1;
        tok->outBuf[tok->outWritePos++] = info2;
        tok->outBuf[tok->outWritePos++] = 0;
    }
    else if (tok->ignLevel <= 0) {
        switch (itemType) {
        case PICODATA_ITEM_CMD:
            switch (info1) {
            case PICODATA_ITEMINFO1_CMD_CONTEXT:
            case PICODATA_ITEMINFO1_CMD_VOICE:
            case PICODATA_ITEMINFO1_CMD_MARKER:
            case PICODATA_ITEMINFO1_CMD_PLAY:
            case PICODATA_ITEMINFO1_CMD_SAVE:
            case PICODATA_ITEMINFO1_CMD_UNSAVE:
            case PICODATA_ITEMINFO1_CMD_PROSDOMAIN:
            case PICODATA_ITEMINFO1_CMD_PHONEME:
                len = picoos_strlen((picoos_char*)str);
                if (tok->outWritePos + 4 + len < OUT_BUF_SIZE) {
                    tok->outBuf[tok->outWritePos++] = itemType;
                    tok->outBuf[tok->outWritePos++] = info1;
                    tok->outBuf[tok->outWritePos++] = info2;
                    tok->outBuf[tok->outWritePos++] = len;
                    for (i=0; i<len; i++) {
                        tok->outBuf[tok->outWritePos++] = str[i];
                    }
                }
                else {
                    PICODBG_WARN(("tok_putItem: output buffer too small"));
                }
                break;
            case PICODATA_ITEMINFO1_CMD_IGNSIG:
            case PICODATA_ITEMINFO1_CMD_IGNORE:
                if (tok->outWritePos + 4 < OUT_BUF_SIZE) {
                    tok->outBuf[tok->outWritePos++] = itemType;
                    tok->outBuf[tok->outWritePos++] = info1;
                    tok->outBuf[tok->outWritePos++] = info2;
                    tok->outBuf[tok->outWritePos++] = 0;
                }
                else {
                    PICODBG_WARN(("tok_putItem: output buffer too small"));
                }
                break;
            case PICODATA_ITEMINFO1_CMD_SPEED:
            case PICODATA_ITEMINFO1_CMD_PITCH:
            case PICODATA_ITEMINFO1_CMD_VOLUME:
            case PICODATA_ITEMINFO1_CMD_SPELL:
            case PICODATA_ITEMINFO1_CMD_SIL:
            case PICODATA_ITEMINFO1_CMD_SPEAKER:
                if (tok->outWritePos + 4 + 2 < OUT_BUF_SIZE) {
                    tok->outBuf[tok->outWritePos++] = itemType;
                    tok->outBuf[tok->outWritePos++] = info1;
                    tok->outBuf[tok->outWritePos++] = info2;
                    tok->outBuf[tok->outWritePos++] = 2;
                    tok->outBuf[tok->outWritePos++] = val % 256;
                    tok->outBuf[tok->outWritePos++] = val / 256;
                }
                else {
                    PICODBG_WARN(("tok_putItem: output buffer too small"));
                }
                break;
            default:
                PICODBG_WARN(("tok_putItem: unknown command type"));
            }
            break;
        case PICODATA_ITEM_TOKEN:
            len = picoos_strlen((picoos_char*)str);
            if (tok->outWritePos + 4 + len < OUT_BUF_SIZE) {
                tok->outBuf[tok->outWritePos++] = itemType;
                tok->outBuf[tok->outWritePos++] = info1;
                tok->outBuf[tok->outWritePos++] = info2;
                tok->outBuf[tok->outWritePos++] = len;
                for (i=0; i<len; i++) {
                    tok->outBuf[tok->outWritePos++] = str[i];
                }
            }
            else {
                PICODBG_WARN(("tok_putItem: output buffer too small"));
            }
            break;
        default:
            PICODBG_WARN(("tok_putItem: unknown item type"));
        }
    }
}


static void tok_putItem2 (picodata_ProcessingUnit this,  tok_subobj_t * tok,
                          picoos_uint8 type,
                          picoos_uint8 info1, picoos_uint8 info2,
                          picoos_uint8 len,
                          picoos_uint8 data[])
{
    picoos_int32 i;

    if (is_valid_itemtype(type)) {
        tok->outBuf[tok->outWritePos++] = type;
        tok->outBuf[tok->outWritePos++] = info1;
        tok->outBuf[tok->outWritePos++] = info2;
        tok->outBuf[tok->outWritePos++] = len;
        for (i=0; i<len; i++) {
            tok->outBuf[tok->outWritePos++] = data[i];
        }
    }
}


static MarkupId tok_markupTagId (picoos_uchar tagId[])
{
    if (picoos_strstr(tagId,(picoos_char *)"svox:") == (picoos_char *)tagId) {
        tagId+=5;
    }
    if (tok_strEqual(tagId, TOK_MARKUP_KW_IGNORE)) {
        return MIIgnore;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_SPEED)) {
        return MISpeed;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_PITCH)) {
        return MIPitch;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_VOLUME)) {
        return MIVolume;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_SPEAKER)) {
        return MISpeaker;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_VOICE)) {
        return MIVoice;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_CONTEXT)) {
        return MIPreprocContext;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_MARK)) {
        return MIMarker;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_PLAY)) {
        return MIPlay;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_USESIG)) {
        return MIUseSig;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_GENFILE)) {
        return MIGenFile;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_SENTENCE) || tok_strEqual(tagId, TOK_MARKUP_KW_S)) {
        return MISentence;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_PARAGRAPH) || tok_strEqual(tagId, TOK_MARKUP_KW_P)) {
        return MIParagraph;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_BREAK)) {
        return MIBreak;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_SPELL)) {
        return MISpell;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_PHONEME)) {
        return MIPhoneme;
    } else if (tok_strEqual(tagId, TOK_MARKUP_KW_ITEM)) {
        return MIItem;
    } else {
        return MIDummyEnd;
    }
}


static void tok_checkLimits (picodata_ProcessingUnit this, picoos_uint32 * value, picoos_uint32 min, picoos_uint32 max, picoos_uchar valueType[])
{
    if ((((*value) < min) || ((*value) > max))) {
        picoos_emRaiseWarning(this->common->em, PICO_ERR_MARKUP_VALUE_OUT_OF_RANGE, (picoos_char*)"", (picoos_char*)"attempt to set illegal value %i for %s", *value, valueType);
        if (((*value) < min)) {
            (*value) = min;
        } else if (((*value) > max)) {
            (*value) = max;
        }
    }
}



/*

static void tok_checkRealLimits (picodata_ProcessingUnit this, picoos_single * value, picoos_single min, picoos_single max, picoos_uchar valueType[])
{
    if ((((*value) < min) || ((*value) > max))) {
          picoos_emRaiseWarning(this->common->em, PICO_ERR_MARKUP_VALUE_OUT_OF_RANGE, (picoos_char*)"", (picoos_char*)"attempt to set illegal value %f for %s", *value, valueType);
        if (((*value) < min)) {
            (*value) = min;
        } else if (((*value) > max)) {
            (*value) = max;
        }
    }
}
*/

#define VAL_STR_LEN 21

static void tok_interpretMarkup (picodata_ProcessingUnit this, tok_subobj_t * tok, picoos_bool isStartTag, MarkupId mId)
{
    picoos_bool done;
    picoos_int32 ival;
    picoos_uint32 uval;
    picoos_int32 ival2;
    picoos_uchar valStr[VAL_STR_LEN];
    picoos_uchar valStr2[VAL_STR_LEN];
    picoos_uchar valStr3[VAL_STR_LEN];
    picoos_int32 i2;
    picoos_uint32 dur;
    picoos_bool done1;
    picoos_bool paramFound;
    picoos_uint8 type, info1, info2;
    picoos_uint8 data[256];
    picoos_int32 pos, n, len;
    picoos_uchar part[10];

    done = FALSE;
    switch (mId) {
        case MIIgnore:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId,(picoos_uchar*)"")) {
                tok_startIgnore(tok);
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_endIgnore(tok);
                done = TRUE;
            }
            break;
        case MISpeed:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWLevel)) {
                if (tok_isRelative(tok->markupParams[0].paramVal, & uval)) {
                    tok_checkLimits(this, & uval, PICO_SPEED_FACTOR_MIN, PICO_SPEED_FACTOR_MAX,(picoos_uchar*)"relative speed factor");
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SPEED, PICODATA_ITEMINFO2_CMD_RELATIVE, uval, (picoos_uchar*)"");
                } else {
                    uval = picoos_atoi((picoos_char*)tok->markupParams[0].paramVal);
                    tok_checkLimits(this, & uval, PICO_SPEED_MIN, PICO_SPEED_MAX,(picoos_uchar*)"speed");
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SPEED, PICODATA_ITEMINFO2_CMD_ABSOLUTE, uval, (picoos_uchar*)"");
                }
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SPEED, PICODATA_ITEMINFO2_CMD_ABSOLUTE, PICO_SPEED_DEFAULT, (picoos_uchar*)"");
                done = TRUE;
            }
            break;
        case MIPitch:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWLevel)) {
                if (tok_isRelative(tok->markupParams[0].paramVal, & uval)) {
                    tok_checkLimits(this, & uval,PICO_PITCH_FACTOR_MIN,PICO_PITCH_FACTOR_MAX, (picoos_uchar*)"relative pitch factor");
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PITCH, PICODATA_ITEMINFO2_CMD_RELATIVE, uval, (picoos_uchar*)"");
                } else {
                    uval = picoos_atoi((picoos_char*)tok->markupParams[0].paramVal);
                    tok_checkLimits(this, & uval,PICO_PITCH_MIN,PICO_PITCH_MAX, (picoos_uchar*)"pitch");
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PITCH,PICODATA_ITEMINFO2_CMD_ABSOLUTE, uval, (picoos_uchar*)"");
                }
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PITCH,PICODATA_ITEMINFO2_CMD_ABSOLUTE, PICO_PITCH_DEFAULT, (picoos_uchar*)"");
                done = TRUE;
            }
            break;
        case MIVolume:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWLevel)) {
                if (tok_isRelative(tok->markupParams[0].paramVal, & uval)) {
                    tok_checkLimits(this, & uval, PICO_VOLUME_FACTOR_MIN, PICO_VOLUME_FACTOR_MAX, (picoos_uchar*)"relative volume factor");
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_VOLUME, PICODATA_ITEMINFO2_CMD_RELATIVE, uval, (picoos_uchar*)"");
                } else {
                    uval = picoos_atoi((picoos_char*)tok->markupParams[0].paramVal);
                    tok_checkLimits(this, & uval, PICO_VOLUME_MIN, PICO_VOLUME_MAX, (picoos_uchar*)"volume");
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_VOLUME, PICODATA_ITEMINFO2_CMD_ABSOLUTE, uval, (picoos_uchar*)"");
                }
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_VOLUME, PICODATA_ITEMINFO2_CMD_ABSOLUTE, PICO_VOLUME_DEFAULT, (picoos_uchar*)"");
                done = TRUE;
            }
            break;
        case MISpeaker:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWLevel)) {
                if (tok_isRelative(tok->markupParams[0].paramVal, & uval)) {
                    tok_checkLimits(this, & uval, PICO_SPEAKER_FACTOR_MIN, PICO_SPEAKER_FACTOR_MAX, (picoos_uchar*)"relative speaker factor");
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SPEAKER, PICODATA_ITEMINFO2_CMD_RELATIVE, uval, (picoos_uchar*)"");
                } else {
                    uval = picoos_atoi((picoos_char*)tok->markupParams[0].paramVal);
                    tok_checkLimits(this, & uval, PICO_SPEAKER_MIN, PICO_SPEAKER_MAX, (picoos_uchar*)"volume");
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SPEAKER, PICODATA_ITEMINFO2_CMD_ABSOLUTE, uval, (picoos_uchar*)"");
                }
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SPEAKER, PICODATA_ITEMINFO2_CMD_ABSOLUTE, PICO_SPEAKER_DEFAULT, (picoos_uchar*)"");
                done = TRUE;
            }
            break;

        case MIVoice:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWName)) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_VOICE, PICODATA_ITEMINFO2_NA, 0, tok->markupParams[0].paramVal);
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_FLUSH, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PROSDOMAIN, 0, 0, (picoos_uchar*)"");
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId,(picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_VOICE, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_FLUSH, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PROSDOMAIN, 0, 0, (picoos_uchar*)"");
                done = TRUE;
            }
            break;
        case MIPreprocContext:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWName)) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_CONTEXT, PICODATA_ITEMINFO2_NA, 0, tok->markupParams[0].paramVal);
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId,(picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_CONTEXT, PICODATA_ITEMINFO2_NA, 0, PICO_CONTEXT_DEFAULT);
                done = TRUE;
            }
            break;
        case MIMarker:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWName)) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_MARKER, PICODATA_ITEMINFO2_NA, 0, tok->markupParams[0].paramVal);
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId,(picoos_uchar*)"")) {
                done = TRUE;
            }
            break;
        case MISentence:
            if (isStartTag) {
                tok_getParamStrVal(tok->markupParams, KWProsDomain, (picoos_uchar*)valStr, & paramFound);
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_FLUSH, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PROSDOMAIN, 2, 0, valStr);
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_FLUSH, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PROSDOMAIN, 2, 0, (picoos_uchar*)"");
                done = TRUE;
            }
            break;
        case MIParagraph:
            if (isStartTag) {
                tok_getParamStrVal(tok->markupParams, KWProsDomain, (picoos_uchar*)valStr, & paramFound);
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_FLUSH, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PROSDOMAIN, 1, 0, valStr);
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_FLUSH, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SIL, PICODATA_ITEMINFO2_NA, PARAGRAPH_PAUSE_DUR, (picoos_uchar*)"");
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PROSDOMAIN, 1, 0, (picoos_uchar*)"");
                done = TRUE;
            }
            break;
        case MIBreak:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWTime)) {
                tok_getDur(tok->markupParams[0].paramVal, & dur, & done1);
                tok_checkLimits (this, &dur, 0, 65535, (picoos_uchar*)"time");
                if (done1) {
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SIL, PICODATA_ITEMINFO2_NA, dur, (picoos_uchar*)"");
                    done = TRUE;
                }
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                done = TRUE;
            }
            break;
        case MISpell:
            if (isStartTag) {
                if (tok_strEqual(tok->markupParams[0].paramId, KWMode)) {
                    if (tok_strEqual(tok->markupParams[0].paramVal, KWPB)) {
                        uval = SPELL_WITH_PHRASE_BREAK;
                    } else if (tok_strEqual(tok->markupParams[0].paramVal, KWSB)) {
                        uval = SPELL_WITH_SENTENCE_BREAK;
                    } else {
                        tok_getDur(tok->markupParams[0].paramVal, & uval, & done1);
                        tok_checkLimits (this, & uval, 0, 65535, (picoos_uchar*)"time");
                        if (done1) {
                            done = TRUE;
                        }
                    }
                } else {
                    uval = SPELL_WITH_PHRASE_BREAK;
                }
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SPELL, PICODATA_ITEMINFO2_CMD_START, uval, (picoos_uchar*)"");
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SPELL, PICODATA_ITEMINFO2_CMD_END, 0, (picoos_uchar*)"");
                done = TRUE;
            }
            break;
        case MIGenFile:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWFile)) {
                if (tok->saveFile[0] != 0) {
                   tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_UNSAVE,
                               picodata_getPuTypeFromExtension(tok->saveFile, /*input*/FALSE), 0, tok->saveFile);
                   tok->saveFile[0] = 0;
                }
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_SAVE,
                            picodata_getPuTypeFromExtension(tok->markupParams[0].paramVal,  /*input*/FALSE), 0, tok->markupParams[0].paramVal);
                picoos_strcpy((picoos_char*)tok->saveFile, (picoos_char*)tok->markupParams[0].paramVal);
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                if (tok->saveFile[0] != 0) {
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_UNSAVE,
                                picodata_getPuTypeFromExtension(tok->saveFile, /*input*/FALSE), 0, (picoos_uchar*)"");
                    tok->saveFile[0] = 0;
                }
                done = TRUE;
            }
            break;
        case MIPlay:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWFile)) {
                if (picoos_FileExists(this->common, (picoos_char*)tok->markupParams[0].paramVal)) {
                    tok_getParamIntVal(tok->markupParams,KWF0Beg,& ival,& paramFound);
                    tok_getParamIntVal(tok->markupParams,KWF0End,& ival2,& paramFound);
                    tok_getParamStrVal(tok->markupParams,KWAlphabet,valStr3,& paramFound);
                    tok_getParamPhonesStr(tok->markupParams,KWXFadeBeg,valStr3,valStr,VAL_STR_LEN,& paramFound);
                    tok_getParamPhonesStr(tok->markupParams,KWXFadeEnd,valStr3,valStr2,VAL_STR_LEN,& paramFound);
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PLAY,
                                picodata_getPuTypeFromExtension(tok->markupParams[0].paramVal, /*input*/TRUE), 0, tok->markupParams[0].paramVal);
                    tok_startIgnore(tok);
                } else {
                    if (tok->ignLevel > 0) {
                        tok_startIgnore(tok);
                    } else {
                       picoos_emRaiseWarning(this->common->em, PICO_EXC_CANT_OPEN_FILE, (picoos_char*)"", (picoos_char*)"file '%s' not found; synthesizing enclosed text instead\n", tok->markupParams[0].paramVal);
                    }
                }
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_endIgnore(tok);
                done = TRUE;
            }
            break;
        case MIUseSig:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWFile)) {
                if (picoos_FileExists(this->common, (picoos_char*)tok->markupParams[0].paramVal)) {
                    tok_getParamIntVal(tok->markupParams,KWF0Beg,& ival,& paramFound);
                    tok_getParamIntVal(tok->markupParams,KWF0End,& ival2,& paramFound);
                    tok_getParamStrVal(tok->markupParams,KWAlphabet,valStr3, & paramFound);
                    tok_getParamPhonesStr(tok->markupParams,KWXFadeBeg,valStr3,valStr,VAL_STR_LEN,& paramFound);
                    tok_getParamPhonesStr(tok->markupParams,KWXFadeEnd,valStr3,valStr2,VAL_STR_LEN,& paramFound);
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PLAY,
                                picodata_getPuTypeFromExtension(tok->markupParams[0].paramVal, /*input*/TRUE), 0, tok->markupParams[0].paramVal);
                    tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_IGNSIG, PICODATA_ITEMINFO2_CMD_START, 0, (picoos_uchar*)"");
                } else {
                    if (tok->ignLevel <= 0) {
                        picoos_emRaiseWarning(this->common->em, PICO_EXC_CANT_OPEN_FILE, (picoos_char*)"", (picoos_char*)"file '%s' not found; synthesizing enclosed text instead", tok->markupParams[0].paramVal);
                    }
                }
                done = TRUE;
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_IGNSIG, PICODATA_ITEMINFO2_CMD_END, 0, (picoos_uchar*)"");
                done = TRUE;
            }
            break;
        case MIPhoneme:
            i2 = 0;
            if (isStartTag) {
                if (tok_strEqual(tok->markupParams[0].paramId, KWAlphabet) && tok_strEqual(tok->markupParams[1].paramId, KWPH)) {
                    if (tok_strEqual(tok->markupParams[2].paramId, KWOrthMode)
                        && tok_strEqual(tok->markupParams[2].paramVal, KWIgnorePunct)) {
                        i2 = 1;
                    }
                    if (picodata_mapPAStrToPAIds(tok->transducer, this->common, tok->xsampa_parser, tok->svoxpa_parser, tok->xsampa2svoxpa_mapper, tok->markupParams[1].paramVal, tok->markupParams[0].paramVal, tok->phonemes, sizeof(tok->phonemes)-1) == PICO_OK) {
                        tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PHONEME,
                            PICODATA_ITEMINFO2_CMD_START, i2, tok->phonemes);
                        done = TRUE;
                    } else {
                        PICODBG_WARN(("cannot map phonetic string '%s'; synthesizeing text instead", tok->markupParams[1].paramVal));
                        picoos_emRaiseWarning(this->common->em, PICO_ERR_MARKUP_VALUE_OUT_OF_RANGE,(picoos_char*)"", (picoos_char*)"cannot map phonetic string '%s'; synthesizeing text instead", tok->markupParams[1].paramVal);
                        done = TRUE;
                    }
                } else if (tok_strEqual(tok->markupParams[0].paramId, KWPH)) {
                    if (tok_strEqual(tok->markupParams[1].paramId, KWOrthMode)
                        && tok_strEqual(tok->markupParams[1].paramVal, KWIgnorePunct)) {
                        i2 = 1;
                    }
                    if (picodata_mapPAStrToPAIds(tok->transducer, this->common, tok->xsampa_parser, tok->svoxpa_parser, tok->xsampa2svoxpa_mapper, tok->markupParams[0].paramVal, PICODATA_XSAMPA, tok->phonemes, sizeof(tok->phonemes)) == PICO_OK) {
                        tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PHONEME,
                            PICODATA_ITEMINFO2_CMD_START, i2, tok->phonemes);
                        done = TRUE;
                    }
                    else {
                        PICODBG_WARN(("cannot map phonetic string '%s'; synthesizeing text instead", tok->markupParams[1].paramVal));
                        picoos_emRaiseWarning(this->common->em, PICO_ERR_MARKUP_VALUE_OUT_OF_RANGE,(picoos_char*)"", (picoos_char*)"cannot map phonetic string '%s'; synthesizing text instead", tok->markupParams[0].paramVal);
                        done = TRUE;
                    }
                }
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId, (picoos_uchar*)"")) {
                tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_PHONEME,
                    PICODATA_ITEMINFO2_CMD_END, i2, (picoos_uchar*)"");
                done = TRUE;
            }
            break;
        case MIItem:
            if (isStartTag && tok_strEqual(tok->markupParams[0].paramId, KWType) &&
                              tok_strEqual(tok->markupParams[1].paramId, KWInfo1)&&
                              tok_strEqual(tok->markupParams[2].paramId, KWInfo2)&&
                              tok_strEqual(tok->markupParams[3].paramId, KWDATA)) {
                  picoos_int32 len2, n2;
                  type = picoos_atoi(tok->markupParams[0].paramVal);
                  info1 = picoos_atoi(tok->markupParams[1].paramVal);
                  info2 = picoos_atoi(tok->markupParams[2].paramVal);
                  n = 0; n2 = 0;
                  len2 = (picoos_int32)picoos_strlen(tok->markupParams[3].paramVal);
                  while (n<len2) {
                      while ((tok->markupParams[3].paramVal[n] != 0) && (tok->markupParams[3].paramVal[n] <= 32)) {
                          n++;
                      }
                      tok->markupParams[3].paramVal[n2] = tok->markupParams[3].paramVal[n];
                      n++;
                      n2++;
                  }
                  if (is_valid_itemtype(type)) {
                      done = TRUE;
                      len = 0;
                      pos = 0;
                      picoos_get_sep_part_str(tok->markupParams[3].paramVal, picoos_strlen(tok->markupParams[3].paramVal),
                                          &pos, ',', part, 10, &done1);
                      while (done && done1) {
                          n = picoos_atoi(part);
                          if ((n>=0) && (n<256) && (len<256)) {
                              data[len++] = n;
                          }
                          else {
                              done = FALSE;
                          }
                          picoos_get_sep_part_str(tok->markupParams[3].paramVal, picoos_strlen(tok->markupParams[3].paramVal),
                                          &pos, ',', part, 10, &done1);
                      }
                      if (done) {
                          tok_putItem2(this, tok, type, info1, info2, len, data);
                      }
                  }
                  else {
                      done = FALSE;
                  }
            } else if (!isStartTag && tok_strEqual(tok->markupParams[0].paramId,(picoos_uchar*)"")) {
                done = TRUE;
            }
            break;
    default:
        break;
    }
    if (!done) {
        tok->markupTagErr = MEInterprete;
    }
    if (isStartTag) {
        tok->markupLevel[mId]++;
    } else if ((tok->markupLevel[mId] > 0)) {
        tok->markupLevel[mId]--;
    }
}


static picoos_bool tok_attrChar (picoos_uchar ch, picoos_bool first)
{
    return ((((ch >= (picoos_uchar)'A') && (ch <= (picoos_uchar)'Z')) ||
             ((ch >= (picoos_uchar)'a') && (ch <= (picoos_uchar)'z'))) ||
             ( !(first) && ((ch >= (picoos_uchar)'0') && (ch <= (picoos_uchar)'9'))));
}



static picoos_bool tok_idChar (picoos_uchar ch, picoos_bool first)
{
    return tok_attrChar(ch, first) || ( !(first) && (ch == (picoos_uchar)':'));
}


static void tok_setIsFileAttr (picoos_uchar name[], picoos_bool * isFile)
{
    (*isFile) = tok_strEqual(name, KWFile);
}

/* *****************************************************************************/

static void tok_putToSimpleToken (picodata_ProcessingUnit this, tok_subobj_t * tok, picoos_uchar str[], pico_tokenType type, pico_tokenSubType subtype)
{
    int i, len;

    if (str[0] != 0) {
        len = picoos_strlen((picoos_char*)str);
        for (i = 0; i < len; i++) {
            if (tok->tokenPos >= IN_BUF_SIZE) {
                picoos_emRaiseWarning(this->common->em, PICO_ERR_INTERNAL_LIMIT, (picoos_char*)"", (picoos_char*)"simple token too long; forced treatment");
                tok_treatSimpleToken(this, tok);
            }
            tok->tokenStr[tok->tokenPos] = str[i];
            tok->tokenPos++;
        }
    }
    tok->tokenType = type;
    tok->tokenSubType = subtype;
}


static void tok_putToMarkup (picodata_ProcessingUnit this, tok_subobj_t * tok, picoos_uchar str[])
{
    picoos_int32 i, len;
    picoos_uint8 ok;

    tok->markupTagErr = MENone;
    len = picoos_strlen((picoos_char*)str);
    for (i = 0; i< len; i++) {
        if (tok->markupPos >= (MARKUP_STRING_BUF_SIZE - 1)) {
            if ((tok->markupPos == (MARKUP_STRING_BUF_SIZE - 1)) && (tok_markupTagId(tok->markupTagName) != MIDummyEnd)) {
                picoos_emRaiseWarning(this->common->em, PICO_ERR_INTERNAL_LIMIT ,(picoos_char*)"", (picoos_char*)"markup tag too long");
            }
            tok->markupState = MSErrorTooLong;
        } else if ((str[i] == (picoos_uchar)' ') && ((tok->markupState == MSExpectingmarkupTagName) || (tok->markupState == MSGotmarkupTagName) || (tok->markupState == MSGotAttrName) || (tok->markupState == MSGotEqual) || (tok->markupState == MSGotAttrValue))) {
        } else if ((str[i] == (picoos_uchar)'>') && ((tok->markupState == MSGotmarkupTagName) || (tok->markupState == MSInmarkupTagName) || (tok->markupState == MSGotAttrValue))) {
            tok->markupState = MSGotEnd;
        } else if ((str[i] == (picoos_uchar)'/') && ((tok->markupState == MSGotmarkupTagName) || (tok->markupState == MSInmarkupTagName) || (tok->markupState == MSGotAttrValue))) {
            if (tok->markupTagType == MTEnd) {
                tok->markupTagErr = MEUnexpectedChar;
                tok->markupState = MSError;
            } else {
                tok->markupTagType = MTEmpty;
                tok->markupState = MSGotEndSlash;
            }
        } else {
            switch (tok->markupState) {
                case MSNotInMarkup:
                    if (str[i] == (picoos_uchar)'<') {
                        tok_clearMarkupParams(tok->markupParams);
                        tok->nrMarkupParams = 0;
                        tok->strPos = 0;
                        tok->markupTagType = MTStart;
                        tok->markupState = MSGotStart;
                    } else {
                        tok->markupTagErr = MEMissingStart;
                        tok->markupState = MSError;
                    }
                    break;
                case MSGotStart:
                    if (str[i] == (picoos_uchar)'/') {
                        tok->markupTagType = MTEnd;
                        tok->markupState = MSExpectingmarkupTagName;
                    } else if (str[i] == (picoos_uchar)' ') {
                        tok->markupState = MSExpectingmarkupTagName;
                    } else if (tok_idChar(str[i],TRUE)) {
                        tok->markupTagType = MTStart;
                        tok->markupTagName[tok->strPos] = str[i];
                        tok->strPos++;
                        tok->markupTagName[tok->strPos] = 0;
                        tok->markupState = MSInmarkupTagName;
                    } else {
                        tok->markupTagErr = MEUnexpectedChar;
                        tok->markupState = MSError;
                    }
                    break;
                case MSInmarkupTagName:   case MSExpectingmarkupTagName:
                    if (tok_idChar(str[i],tok->markupState == MSExpectingmarkupTagName)) {
                        tok->markupTagName[tok->strPos] = str[i];
                        tok->strPos++;
                        tok->markupTagName[(tok->strPos)] = 0;
                        tok->markupState = MSInmarkupTagName;
                    } else if ((tok->markupState == MSInmarkupTagName) && (str[i] == (picoos_uchar)' ')) {
                        tok->markupState = MSGotmarkupTagName;
                        picobase_lowercase_utf8_str(tok->markupTagName, (picoos_char*)tok->markupTagName, IN_BUF_SIZE, &ok);
                        tok->strPos = 0;
                    } else {
                        tok->markupTagErr = MEIdent;
                        tok->markupState = MSError;
                    }
                    break;
                case MSGotmarkupTagName:   case MSGotAttrValue:
                    if (tok_attrChar(str[i], TRUE)) {
                        if (tok->markupTagType == MTEnd) {
                            tok->markupTagErr = MEUnexpectedChar;
                            tok->markupState = MSError;
                        } else {
                            if (tok->nrMarkupParams < MAX_NR_MARKUP_PARAMS) {
                                tok->markupParams[tok->nrMarkupParams].paramId[tok->strPos] = str[i];
                                tok->strPos++;
                                tok->markupParams[tok->nrMarkupParams].paramId[tok->strPos] = 0;
                            } else {
                                picoos_emRaiseWarning(this->common->em, PICO_ERR_INTERNAL_LIMIT ,(picoos_char*)"", (picoos_char*)"too many attributes in markup; ignoring");
                            }
                            tok->markupState = MSInAttrName;
                        }
                    } else {
                        tok->markupTagErr = MEUnexpectedChar;
                        tok->markupState = MSError;
                    }
                    break;
                case MSInAttrName:
                    if (tok_attrChar(str[i], FALSE)) {
                        if (tok->nrMarkupParams < MAX_NR_MARKUP_PARAMS) {
                            tok->markupParams[tok->nrMarkupParams].paramId[tok->strPos] = str[i];
                            tok->strPos++;
                            tok->markupParams[tok->nrMarkupParams].paramId[tok->strPos] = 0;
                        }
                        tok->markupState = MSInAttrName;
                    } else if (str[i] == (picoos_uchar)' ') {
                        picobase_lowercase_utf8_str(tok->markupParams[tok->nrMarkupParams].paramId, (picoos_char*)tok->markupParams[tok->nrMarkupParams].paramId, IN_BUF_SIZE, &ok);
                        tok_setIsFileAttr(tok->markupParams[tok->nrMarkupParams].paramId, & tok->isFileAttr);
                        tok->markupState = MSGotAttrName;
                    } else if (str[i] == (picoos_uchar)'=') {
                        picobase_lowercase_utf8_str(tok->markupParams[tok->nrMarkupParams].paramId, (picoos_char*)tok->markupParams[tok->nrMarkupParams].paramId, IN_BUF_SIZE, &ok);
                        tok_setIsFileAttr(tok->markupParams[tok->nrMarkupParams].paramId, & tok->isFileAttr);
                        tok->markupState = MSGotEqual;
                    } else {
                        tok->markupTagErr = MEMissingEqual;
                        tok->markupState = MSError;
                    }
                    break;
                case MSGotAttrName:
                    if (str[i] == (picoos_uchar)'=') {
                        tok->markupState = MSGotEqual;
                    } else {
                        tok->markupTagErr = MEMissingEqual;
                        tok->markupState = MSError;
                    }
                    break;
                case MSGotEqual:
                    if ((str[i] == (picoos_uchar)'"') || (str[i] == (picoos_uchar)'\'')) {
                        tok->strDelim = str[i];
                        tok->strPos = 0;
                        tok->markupState = MSInAttrValue;
                    } else {
                        tok->markupTagErr = MEMissingQuote;
                        tok->markupState = MSError;
                    }
                    break;
                case MSInAttrValue:
                    if (!(tok->isFileAttr) && (str[i] == (picoos_uchar)'\\')) {
                        tok->markupState = MSInAttrValueEscaped;
                    } else if (str[i] == tok->strDelim) {
                        if (tok->nrMarkupParams < MAX_NR_MARKUP_PARAMS) {
                            tok->nrMarkupParams++;
                        }
                        tok->strPos = 0;
                        tok->markupState = MSGotAttrValue;
                    } else {
                        if (tok->nrMarkupParams < MAX_NR_MARKUP_PARAMS) {
                            tok->markupParams[tok->nrMarkupParams].paramVal[tok->strPos] = str[i];
                            tok->strPos++;
                            tok->markupParams[tok->nrMarkupParams].paramVal[tok->strPos] = 0;
                        }
                        tok->markupState = MSInAttrValue;
                    }
                    break;
                case MSInAttrValueEscaped:
                    if (tok->nrMarkupParams < MAX_NR_MARKUP_PARAMS) {
                        tok->markupParams[tok->nrMarkupParams].paramVal[tok->strPos] = str[i];
                        tok->strPos++;
                        tok->markupParams[tok->nrMarkupParams].paramVal[tok->strPos] = 0;
                    }
                    tok->markupState = MSInAttrValue;
                    break;
                case MSGotEndSlash:
                    if (str[i] == (picoos_uchar)'>') {
                        tok->markupState = MSGotEnd;
                    } else {
                        tok->markupTagErr = MEUnexpectedChar;
                        tok->markupState = MSError;
                    }
                    break;
            default:
                tok->markupTagErr = MEUnexpectedChar;
                tok->markupState = MSError;
                break;
            }
        }
        if (tok->markupTagErr == MENone) {
            tok->markupStr[tok->markupPos] = str[i];
            tok->markupPos++;
        } /* else restart parsing at current char */
        tok->markupStr[tok->markupPos] = 0;
    }
    /*
    PICODBG_DEBUG(("putToMarkup %s", tok->markupStr));
    */
}

/* *****************************************************************************/

static void tok_treatMarkupAsSimpleToken (picodata_ProcessingUnit this, tok_subobj_t * tok)
{
    picoos_int32 i;

    tok->utfpos = 0;
    tok->utflen = 0;
    tok->markupState = MSNotInMarkup;
    for (i = 0; i < tok->markupPos; i++) {
        tok_treatChar(this, tok, tok->markupStr[i], FALSE);
    }
    tok->markupPos = 0;
    tok->strPos = 0;
}


static void tok_treatMarkup (picodata_ProcessingUnit this, tok_subobj_t * tok)
{
    MarkupId mId;

    if (tok_markupTagId(tok->markupTagName) != MIDummyEnd) {
        if (tok->markupTagErr == MENone) {
            tok->markupState = MSNotInMarkup;
            if ((tok->tokenType != PICODATA_ITEMINFO1_TOKTYPE_SPACE) && (tok->tokenType != PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED)) {
                tok_treatSimpleToken(this, tok);
            }
            tok_putToSimpleToken(this, tok, (picoos_uchar*)" ", PICODATA_ITEMINFO1_TOKTYPE_SPACE, -1);
            mId = tok_markupTagId(tok->markupTagName);
            if ((tok->markupTagType == MTStart) || (tok->markupTagType == MTEmpty)) {
                tok_interpretMarkup(this, tok, TRUE, mId);
            }
            if (((tok->markupTagType == MTEnd) || (tok->markupTagType == MTEmpty))) {
                tok_clearMarkupParams(tok->markupParams);
                tok->nrMarkupParams = 0;
                tok_interpretMarkup(this, tok, FALSE,mId);
            }
        }
        if (tok->markupTagErr != MENone) {
            if (!tok->aborted) {
              picoos_emRaiseWarning(this->common->em, PICO_ERR_INVALID_MARKUP_TAG, (picoos_char*)"", (picoos_char*)"syntax error in markup token '%s'",tok->markupStr);
            }
            tok_treatMarkupAsSimpleToken(this, tok);
        }
    } else {
        tok_treatMarkupAsSimpleToken(this, tok);
    }
    tok->markupState = MSNotInMarkup;
    tok->markupPos = 0;
    tok->strPos = 0;
}



static void tok_treatChar (picodata_ProcessingUnit this, tok_subobj_t * tok, picoos_uchar ch, picoos_bool markupHandling)
{
    picoos_int32 i, id;
    picoos_uint8 uval8;
    pico_tokenType type = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
    pico_tokenSubType subtype = -1;
    picoos_bool dummy;
    utf8char0c utf2;
    picoos_int32 utf2pos;

    if (ch == NULLC) {
      tok_treatSimpleToken(this, tok);
      tok_putItem(this, tok, PICODATA_ITEM_CMD, PICODATA_ITEMINFO1_CMD_FLUSH, PICODATA_ITEMINFO2_NA, 0, (picoos_uchar*)"");
    }
    else {
      switch (tok_putToUtf(tok, ch)) {
        case UTF_CHAR_MALFORMED:
            tok->utfpos = 0;
            tok->utflen = 0;
            break;
        case UTF_CHAR_INCOMPLETE:
            break;
        case UTF_CHAR_COMPLETE:
            markupHandling = (markupHandling && (tok->markupHandlingMode == MARKUP_HANDLING_ENABLED));
            id = picoktab_graphOffset(tok->graphTab, tok->utf);
            if (id > 0) {
                if (picoktab_getIntPropTokenType(tok->graphTab, id, &uval8)) {
                    type = (pico_tokenType)uval8;
                    if (type == PICODATA_ITEMINFO1_TOKTYPE_LETTERV) {
                        type = PICODATA_ITEMINFO1_TOKTYPE_LETTER;
                    }
                }
                dummy = picoktab_getIntPropTokenSubType(tok->graphTab, id, &subtype);
            } else if (tok->utf[tok->utfpos-1] <= (picoos_uchar)' ') {
                type = PICODATA_ITEMINFO1_TOKTYPE_SPACE;
                subtype =  -1;
            } else {
                type = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
                subtype =  -1;
            }
            if ((tok->utf[tok->utfpos-1] > (picoos_uchar)' ')) {
                tok->nrEOL = 0;
            } else if ((tok->utf[tok->utfpos-1] == EOL)) {
                tok->nrEOL++;
            }
            if (markupHandling && (tok->markupState != MSNotInMarkup)) {
                tok_putToMarkup(this, tok, tok->utf);
                if (tok->markupState >= MSError) {
                    picoos_strlcpy(utf2, tok->utf, 5);
                    utf2pos = tok->utfpos;
                    /* treat string up to (but not including) current char as simple
                       token and restart markup tag parsing with current char */
                    tok_treatMarkupAsSimpleToken(this, tok);
                    for (i = 0; i < utf2pos; i++) {
                        tok_treatChar(this, tok, utf2[i], markupHandling);
                    }
                } else if (tok->markupState == MSGotEnd) {
                    tok_treatMarkup(this, tok);
                }
            } else if ((markupHandling && (tok->utf[tok->utfpos-1] == (picoos_uchar)'<'))) {
                tok_putToMarkup(this, tok, tok->utf);
            } else if (type != PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED) {
                if ((type != tok->tokenType) || (type == PICODATA_ITEMINFO1_TOKTYPE_CHAR) || (subtype != tok->tokenSubType)) {
                    tok_treatSimpleToken(this, tok);
                } else if ((tok->utf[tok->utfpos-1] == EOL) && (tok->nrEOL == 2)) {
                    tok_treatSimpleToken(this, tok);
                    tok_putToSimpleToken(this, tok, (picoos_uchar*)".", PICODATA_ITEMINFO1_TOKTYPE_CHAR, -1);
                    tok_treatSimpleToken(this, tok);
                }
                tok_putToSimpleToken(this, tok, tok->utf, type, subtype);
            } else {
                tok_treatSimpleToken(this, tok);
            }
            tok->utfpos = 0;
            tok->utflen = 0;
            break;
      }
    }
}


static void tok_treatSimpleToken (picodata_ProcessingUnit this, tok_subobj_t * tok)
{
    if (tok->tokenPos < IN_BUF_SIZE) {
        tok->tokenStr[tok->tokenPos] = 0;
    }
    if (tok->markupState != MSNotInMarkup) {
        if (!(tok->aborted) && (tok->markupState >= MSGotmarkupTagName) && (tok_markupTagId(tok->markupTagName) != MIDummyEnd)) {
            picoos_emRaiseWarning(this->common->em, PICO_ERR_INVALID_MARKUP_TAG, (picoos_char*)"", (picoos_char*)"unfinished markup tag '%s'",tok->markupStr);
        }
        tok_treatMarkupAsSimpleToken(this, tok);
        tok_treatSimpleToken(this, tok);
    } else if ((tok->tokenPos > 0) && ((tok->ignLevel <= 0) || (tok->tokenType == PICODATA_ITEMINFO1_TOKTYPE_SPACE))) {
        tok_putItem(this, tok, PICODATA_ITEM_TOKEN, tok->tokenType, (picoos_uint8)tok->tokenSubType, 0, tok->tokenStr);
    }
    tok->tokenPos = 0;
    tok->tokenType = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
    tok->tokenSubType =  -1;
}

/* *****************************************************************************/

static pico_status_t tokReset(register picodata_ProcessingUnit this, picoos_int32 resetMode)
{
    tok_subobj_t * tok;
    MarkupId mId;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    tok = (tok_subobj_t *) this->subObj;

    tok->ignLevel = 0;

    tok->utfpos = 0;
    tok->utflen = 0;

    tok_clearMarkupParams(tok->markupParams);
    tok->nrMarkupParams = 0;
    tok->markupState = MSNotInMarkup;
    tok->markupPos = 0;
    for (mId = MIDummyStart; mId <= MIDummyEnd; mId++) {
        tok->markupLevel[mId] = 0;
    }
    tok->markupTagName[0] = 0;
    tok->markupTagType = MTNone;
    tok->markupTagErr = MENone;

    tok->strPos = 0;
    tok->strDelim = 0;
    tok->isFileAttr = FALSE;

    tok->tokenType = PICODATA_ITEMINFO1_TOKTYPE_UNDEFINED;
    tok->tokenSubType =  -1;
    tok->tokenPos = 0;

    tok->nrEOL = 0;


    tok->markupHandlingMode = TRUE;
    tok->aborted = FALSE;

    tok->start = TRUE;

    tok->outReadPos = 0;
    tok->outWritePos = 0;

    tok->saveFile[0] = 0;


    tok->graphTab = picoktab_getGraphs(this->voice->kbArray[PICOKNOW_KBID_TAB_GRAPHS]);

    tok->xsampa_parser = picokfst_getFST(this->voice->kbArray[PICOKNOW_KBID_FST_XSAMPA_PARSE]);
    PICODBG_TRACE(("got xsampa_parser @ %i",tok->xsampa_parser));

    tok->svoxpa_parser = picokfst_getFST(this->voice->kbArray[PICOKNOW_KBID_FST_SVOXPA_PARSE]);
    PICODBG_TRACE(("got svoxpa_parser @ %i",tok->svoxpa_parser));

    tok->xsampa2svoxpa_mapper = picokfst_getFST(this->voice->kbArray[PICOKNOW_KBID_FST_XSAMPA2SVOXPA]);
    PICODBG_TRACE(("got xsampa2svoxpa_mapper @ %i",tok->xsampa2svoxpa_mapper));



    return PICO_OK;
}

static pico_status_t tokInitialize(register picodata_ProcessingUnit this, picoos_int32 resetMode)
{
/*

    tok_subobj_t * tok;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    tok = (tok_subobj_t *) this->subObj;
*/
    return tokReset(this, resetMode);
}


static pico_status_t tokTerminate(register picodata_ProcessingUnit this)
{
    return PICO_OK;
}

static picodata_step_result_t tokStep(register picodata_ProcessingUnit this, picoos_int16 mode, picoos_uint16 * numBytesOutput);

static pico_status_t tokSubObjDeallocate(register picodata_ProcessingUnit this,
        picoos_MemoryManager mm)
{

    if (NULL != this) {
        picoos_deallocate(this->common->mm, (void *) &this->subObj);
    }
    mm = mm;        /* avoid warning "var not used in this function"*/
    return PICO_OK;
}

picodata_ProcessingUnit picotok_newTokenizeUnit(picoos_MemoryManager mm, picoos_Common common,
        picodata_CharBuffer cbIn, picodata_CharBuffer cbOut,
        picorsrc_Voice voice)
{
    tok_subobj_t * tok;
    picodata_ProcessingUnit this = picodata_newProcessingUnit(mm, common, cbIn, cbOut, voice);
    if (this == NULL) {
        return NULL;
    }
    this->initialize = tokInitialize;
    PICODBG_DEBUG(("set this->step to tokStep"));
    this->step = tokStep;
    this->terminate = tokTerminate;
    this->subDeallocate = tokSubObjDeallocate;
    this->subObj = picoos_allocate(mm, sizeof(tok_subobj_t));
    if (this->subObj == NULL) {
        picoos_deallocate(mm, (void *)&this);
        return NULL;
    }
    tok = (tok_subobj_t *) this->subObj;
    tok->transducer = picotrns_newSimpleTransducer(mm, common, 10*(PICOTRNS_MAX_NUM_POSSYM+2));
    if (NULL == tok->transducer) {
        tokSubObjDeallocate(this,mm);
        picoos_deallocate(mm, (void *)&this);
        return NULL;
    }
    tokInitialize(this, PICO_RESET_FULL);
    return this;
}

/**
 * fill up internal buffer, try to locate token, write token to output
 */
picodata_step_result_t tokStep(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * numBytesOutput)
{
    register tok_subobj_t * tok;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    tok = (tok_subobj_t *) this->subObj;

    mode = mode;        /* avoid warning "var not used in this function"*/

    *numBytesOutput = 0;
    while (1) { /* exit via return */
        picoos_int16 ch;

        if ((tok->outWritePos - tok->outReadPos) > 0) {
            if (picodata_cbPutItem(this->cbOut, &tok->outBuf[tok->outReadPos], tok->outWritePos - tok->outReadPos, numBytesOutput) == PICO_OK) {
                PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                    (picoos_uint8 *)"tok:", &tok->outBuf[tok->outReadPos], tok->outWritePos - tok->outReadPos);
                tok->outReadPos += *numBytesOutput;
                if (tok->outWritePos == tok->outReadPos) {
                    tok->outWritePos = 0;
                    tok->outReadPos = 0;
                }
            }
            else {
                return PICODATA_PU_OUT_FULL;
            }

        }
        else if (PICO_EOF != (ch = picodata_cbGetCh(this->cbIn))) {
            PICODBG_DEBUG(("read in %c", (picoos_char) ch));
            tok_treatChar(this, tok, (picoos_uchar) ch, /*markupHandling*/TRUE);
        }
        else {
            return PICODATA_PU_IDLE;
        }
    }
}

#ifdef __cplusplus
}
#endif

/* end */
