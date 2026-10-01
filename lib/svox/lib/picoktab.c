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
 * @file picoktab.c
 *
 * symbol tables needed at runtime
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#include "picoos.h"
#include "picodbg.h"
#include "picoknow.h"
#include "picobase.h"
#include "picoktab.h"
#include "picodata.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/** @todo : the following would be better part of a knowledge base.
 * Make sure it is consistent with the phoneme symbol table used in the lingware */

/* PLANE_PHONEMES */

/* PLANE_POS */

/* PLANE_PB_STRENGTHS */

/* PLANE_ACCENTS */

/* PLANE_INTERN */
#define PICOKTAB_TMPID_PHONSTART      '\x26'  /* 38  '&' */
#define PICOKTAB_TMPID_PHONTERM       '\x23'  /* 35  '#' */


/* ************************************************************/
/* fixed ids */
/* ************************************************************/


static pico_status_t ktabIdsInitialize(register picoknow_KnowledgeBase this,
                                       picoos_Common common)
{
    picoktab_FixedIds ids;

    PICODBG_DEBUG(("start"));

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    ids = (picoktab_FixedIds) this->subObj;

    ids->phonStartId = PICOKTAB_TMPID_PHONSTART;
    ids->phonTermId = PICOKTAB_TMPID_PHONTERM;
    return PICO_OK;
}


static pico_status_t ktabIdsSubObjDeallocate(register picoknow_KnowledgeBase this,
                                             picoos_MemoryManager mm)
{
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}

pico_status_t picoktab_specializeIdsKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common)
{
    if (NULL == this) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    this->subDeallocate = ktabIdsSubObjDeallocate;
    this->subObj = picoos_allocate(common->mm, sizeof(picoktab_fixed_ids_t));
    if (NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                       NULL, NULL);
    }
    return ktabIdsInitialize(this, common);
}

picoktab_FixedIds picoktab_getFixedIds(picoknow_KnowledgeBase this)
{
    return ((NULL == this) ? NULL : ((picoktab_FixedIds) this->subObj));
}


picoktab_FixedIds picoktab_newFixedIds(picoos_MemoryManager mm)
{
    picoktab_FixedIds this = (picoktab_FixedIds) picoos_allocate(mm,sizeof(*this));
    if (NULL != this) {
        /* initialize */
    }
    return this;
}


void picoktab_disposeFixedIds(picoos_MemoryManager mm, picoktab_FixedIds * this)
{
    if (NULL != (*this)) {
        /* terminate */
        picoos_deallocate(mm,(void *)this);
    }
}



/* ************************************************************/
/* Graphs */
/* ************************************************************/

/* overview binary file format for graphs kb:

    graphs-kb = NROFSENTRIES SIZEOFSENTRY ofstable graphs

    NROFSENTRIES  : 2 bytes, number of entries in offset table
    SIZEOFSENTRY  : 1 byte,  size of one entry in offset table

    ofstable = {OFFSET}=NROFSENTRIES (contains NROFSENTRIES entries of OFFSET)

    OFFSET: SIZEOFSENTRY bytes, offset to baseaddress of graphs-kb to entry in graphs

    graphs = {graph}=NROFSENTRIES (contains NROFSENTRIES entries of graph)

    graph = PROPSET FROM TO [TOKENTYPE] [TOKENSUBTYPE] [VALUE] [LOWERCASE] [GRAPHSUBS1] [GRAPHSUBS2]

    FROM          : 1..4 unsigned bytes, UTF8 character without terminating 0
    TO            : 1..4 unsigned bytes, UTF8 character without terminating 0
    PROPSET       : 1 unsigned byte, least significant bit : has TO field
                                                             next bit : has TOKENTYPE
                                                             next bit : has TOKENSUBTYPE
                                                             next bit : has VALUE
                                                             next bit : has LOWERCASE
                                                             next bit : has GRAPHSUBS1
                                                             next bit : has GRAPHSUBS2
                                                             next bit : has PUNC

    TOKENTYPE    : 1 unsigned byte
    TOKENSUBTYPE : 1 unsigned byte
    VALUE        : 1 unsigned byte
    LOWERCASE    : 1..4 unsigned bytes, UTF8 character without terminating 0
    GRAPHSUBS1   : 1..4 unsigned bytes, UTF8 character without terminating 0
    GRAPHSUBS2   : 1..4 unsigned bytes, UTF8 character without terminating 0
    PUNC         : 1 unsigned byte
*/

static picoos_uint32 ktab_propOffset (const picoktab_Graphs this, picoos_uint32 graphsOffset, picoos_uint32 prop);

#define KTAB_START_GRAPHS_NR_OFFSET     0
#define KTAB_START_GRAPHS_SIZE_OFFSET   2
#define KTAB_START_GRAPHS_OFFSET_TABLE  3
#define KTAB_START_GRAPHS_GRAPH_TABLE   0

/* bitmasks to extract the grapheme properties info from the property set */
#define KTAB_GRAPH_PROPSET_TO            ((picoos_uint8)'\x01')
#define KTAB_GRAPH_PROPSET_TOKENTYPE     ((picoos_uint8)'\x02')
#define KTAB_GRAPH_PROPSET_TOKENSUBTYPE  ((picoos_uint8)'\x04')
#define KTAB_GRAPH_PROPSET_VALUE         ((picoos_uint8)'\x08')
#define KTAB_GRAPH_PROPSET_LOWERCASE     ((picoos_uint8)'\x010')
#define KTAB_GRAPH_PROPSET_GRAPHSUBS1    ((picoos_uint8)'\x020')
#define KTAB_GRAPH_PROPSET_GRAPHSUBS2    ((picoos_uint8)'\x040')
#define KTAB_GRAPH_PROPSET_PUNCT         ((picoos_uint8)'\x080')


typedef struct ktabgraphs_subobj *ktabgraphs_SubObj;

typedef struct ktabgraphs_subobj {
    picoos_uint16 nrOffset;
    picoos_uint16 sizeOffset;

    picoos_uint8 * offsetTable;
    picoos_uint8 * graphTable;
} ktabgraphs_subobj_t;



static pico_status_t ktabGraphsInitialize(register picoknow_KnowledgeBase this,
                                          picoos_Common common) {
    ktabgraphs_subobj_t * ktabgraphs;

    PICODBG_DEBUG(("start"));

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    ktabgraphs = (ktabgraphs_subobj_t *) this->subObj;
    ktabgraphs->nrOffset = ((int)(this->base[KTAB_START_GRAPHS_NR_OFFSET])) + 256*(int)(this->base[KTAB_START_GRAPHS_NR_OFFSET+1]);
    ktabgraphs->sizeOffset  = (int)(this->base[KTAB_START_GRAPHS_SIZE_OFFSET]);
    ktabgraphs->offsetTable = &(this->base[KTAB_START_GRAPHS_OFFSET_TABLE]);
    ktabgraphs->graphTable  = &(this->base[KTAB_START_GRAPHS_GRAPH_TABLE]);
    return PICO_OK;
}

static pico_status_t ktabGraphsSubObjDeallocate(register picoknow_KnowledgeBase this,
                                                picoos_MemoryManager mm) {
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}


pico_status_t picoktab_specializeGraphsKnowledgeBase(picoknow_KnowledgeBase this,
                                                     picoos_Common common) {
    if (NULL == this) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    this->subDeallocate = ktabGraphsSubObjDeallocate;
    this->subObj = picoos_allocate(common->mm, sizeof(ktabgraphs_subobj_t));
    if (NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                       NULL, NULL);
    }
    return ktabGraphsInitialize(this, common);
}


picoktab_Graphs picoktab_getGraphs(picoknow_KnowledgeBase this) {
    if (NULL == this) {
        return NULL;
    } else {
        return (picoktab_Graphs) this->subObj;
    }
}


/* Graphs methods */

picoos_uint8 picoktab_hasVowellikeProp(const picoktab_Graphs this,
                                       const picoos_uint8 *graph,
                                       const picoos_uint8 graphlenmax) {

  picoos_uint8 ui8App;
  picoos_uint32 graphsOffset;
  ktabgraphs_subobj_t * g = (ktabgraphs_SubObj)this;

  ui8App = graphlenmax;        /* avoid warning "var not used in this function"*/

  graphsOffset = picoktab_graphOffset (this, (picoos_uchar *)graph);
  return g->graphTable[graphsOffset + ktab_propOffset (this, graphsOffset, KTAB_GRAPH_PROPSET_TOKENTYPE)] == PICODATA_ITEMINFO1_TOKTYPE_LETTERV;
}


static void ktab_getStrProp (const picoktab_Graphs this, picoos_uint32 graphsOffset, picoos_uint32 propOffset, picoos_uchar * str)
{
  ktabgraphs_subobj_t * g = (ktabgraphs_SubObj)this;
  picoos_uint32 i, l;

  i = 0;
  l = picobase_det_utf8_length(g->graphTable[graphsOffset+propOffset]);
  while (i<l) {
    str[i] = g->graphTable[graphsOffset+propOffset+i];
    i++;
  }
  str[l] = 0;
}


static picoos_uint32 ktab_propOffset(const picoktab_Graphs this,
        picoos_uint32 graphsOffset, picoos_uint32 prop)
/* Returns offset of property 'prop' inside the graph with offset 'graphsOffset' in graphs table;
 If the property is found, a value > 0 is returned otherwise 0 */
{
    picoos_uint32 n = 0;
    ktabgraphs_subobj_t * g = (ktabgraphs_SubObj) this;

    if ((g->graphTable[graphsOffset] & prop) == prop) {
        n = n + 1; /* overread PROPSET field */
        n = n + picobase_det_utf8_length(g->graphTable[graphsOffset+n]); /* overread FROM field */
        if (prop > KTAB_GRAPH_PROPSET_TO) {
            if ((g->graphTable[graphsOffset] & KTAB_GRAPH_PROPSET_TO)
                    == KTAB_GRAPH_PROPSET_TO) {
                n = n + picobase_det_utf8_length(g->graphTable[graphsOffset+n]); /* overread TO field */
            }
        } else {
            return n;
        }
        if (prop > KTAB_GRAPH_PROPSET_TOKENTYPE) {
            if ((g->graphTable[graphsOffset] & KTAB_GRAPH_PROPSET_TOKENTYPE)
                    == KTAB_GRAPH_PROPSET_TOKENTYPE) {
                n = n + 1; /* overread TOKENTYPE field */
            }
        } else {
            return n;
        }
        if (prop > KTAB_GRAPH_PROPSET_TOKENSUBTYPE) {
            if ((g->graphTable[graphsOffset] & KTAB_GRAPH_PROPSET_TOKENSUBTYPE)
                    == KTAB_GRAPH_PROPSET_TOKENSUBTYPE) {
                n = n + 1; /* overread stokentype field */
            }
        } else {
            return n;
        }
        if (prop > KTAB_GRAPH_PROPSET_VALUE) {
            if ((g->graphTable[graphsOffset] & KTAB_GRAPH_PROPSET_VALUE)
                    == KTAB_GRAPH_PROPSET_VALUE) {
                n = n + 1; /* overread value field */
            }
        } else {
            return n;
        }
        if (prop > KTAB_GRAPH_PROPSET_LOWERCASE) {
            if ((g->graphTable[graphsOffset] & KTAB_GRAPH_PROPSET_LOWERCASE)
                    == KTAB_GRAPH_PROPSET_LOWERCASE) {
                n = n + picobase_det_utf8_length(g->graphTable[graphsOffset+n]); /* overread lowercase field */
            }
        } else {
            return n;
        }
        if (prop > KTAB_GRAPH_PROPSET_GRAPHSUBS1) {
            if ((g->graphTable[graphsOffset] & KTAB_GRAPH_PROPSET_GRAPHSUBS1)
                    == KTAB_GRAPH_PROPSET_GRAPHSUBS1) {
                n = n + picobase_det_utf8_length(g->graphTable[graphsOffset+n]); /* overread graphsubs1 field */
            }
        } else {
            return n;
        }
        if (prop > KTAB_GRAPH_PROPSET_GRAPHSUBS2) {
            if ((g->graphTable[graphsOffset] & KTAB_GRAPH_PROPSET_GRAPHSUBS2)
                    == KTAB_GRAPH_PROPSET_GRAPHSUBS2) {
                n = n + picobase_det_utf8_length(g->graphTable[graphsOffset+n]); /* overread graphsubs2 field */
            }
        } else {
            return n;
        }
        if (prop > KTAB_GRAPH_PROPSET_PUNCT) {
            if ((g->graphTable[graphsOffset] & KTAB_GRAPH_PROPSET_PUNCT)
                    == KTAB_GRAPH_PROPSET_PUNCT) {
                n = n + 1; /* overread value field */
            }
        } else {
            return n;
        }
    }

    return n;
}


picoos_uint32 picoktab_graphOffset (const picoktab_Graphs this, picoos_uchar * utf8graph)
{  ktabgraphs_subobj_t * g = (ktabgraphs_SubObj)this;
   picoos_int32 a, b, m;
   picoos_uint32 graphsOffset;
   picoos_uint32 propOffset;
   picobase_utf8char from;
   picobase_utf8char to;
   picoos_bool utfGEfrom;
   picoos_bool utfLEto;

   if (g->nrOffset > 0) {
     a = 0;
     b = g->nrOffset-1;
     do  {
       m = (a+b) / 2;

       /* get offset to graph[m] */
       if (g->sizeOffset == 1) {
         graphsOffset = g->offsetTable[g->sizeOffset*m];
       }
       else {
         graphsOffset =     g->offsetTable[g->sizeOffset*m    ] +
                        256*g->offsetTable[g->sizeOffset*m + 1];
         /* PICODBG_DEBUG(("picoktab_graphOffset: %i %i %i %i", m, g->offsetTable[g->sizeOffset*m], g->offsetTable[g->sizeOffset*m + 1], graphsOffset));
         */
       }

       /* get FROM and TO field of graph[m] */
       ktab_getStrProp(this, graphsOffset, 1, from);
       propOffset = ktab_propOffset(this, graphsOffset, KTAB_GRAPH_PROPSET_TO);
       if (propOffset > 0) {
         ktab_getStrProp(this, graphsOffset, propOffset, to);
       }
       else {
         picoos_strcpy((picoos_char *)to, (picoos_char *)from);
       }

       /* PICODBG_DEBUG(("picoktab_graphOffset: %i %i %i '%s' '%s' '%s'", a, m, b, from, utf8graph, to));
       */
       utfGEfrom = picoos_strcmp((picoos_char *)utf8graph, (picoos_char *)from) >= 0;
       utfLEto = picoos_strcmp((picoos_char *)utf8graph, (picoos_char *)to) <= 0;

       if (utfGEfrom && utfLEto) {
         /* PICODBG_DEBUG(("picoktab_graphOffset: utf char '%s' found", utf8graph));
          */
         return graphsOffset;
       }
       if (!utfGEfrom) {
         b = m-1;
       }
       else if (!utfLEto) {
         a = m+1;
       }
     } while (a<=b);
   }
   PICODBG_DEBUG(("picoktab_graphOffset: utf char '%s' not found", utf8graph));
   return 0;
}




picoos_bool  picoktab_getIntPropTokenType (const picoktab_Graphs this, picoos_uint32 graphsOffset, picoos_uint8 * stokenType)
{
  picoos_uint32 propOffset;
  ktabgraphs_subobj_t * g = (ktabgraphs_SubObj)this;

  propOffset = ktab_propOffset(this, graphsOffset, KTAB_GRAPH_PROPSET_TOKENTYPE);
  if (propOffset > 0) {
    *stokenType = (picoos_uint8)(g->graphTable[graphsOffset+propOffset]);
    return TRUE;
  }
  else {
    return FALSE;
  }
}


picoos_bool  picoktab_getIntPropTokenSubType (const picoktab_Graphs this, picoos_uint32 graphsOffset, picoos_int8 * stokenSubType)
{
  picoos_uint32 propOffset;
  ktabgraphs_subobj_t * g = (ktabgraphs_SubObj)this;

  propOffset = ktab_propOffset(this, graphsOffset, KTAB_GRAPH_PROPSET_TOKENSUBTYPE);
  if (propOffset > 0) {
    *stokenSubType = (picoos_int8)(g->graphTable[graphsOffset+propOffset]);
    return TRUE;
  }
  else {
    return FALSE;
  }
}

picoos_bool  picoktab_getIntPropValue (const picoktab_Graphs this, picoos_uint32 graphsOffset, picoos_uint32 * value)
{
  picoos_uint32 propOffset;
  ktabgraphs_subobj_t * g = (ktabgraphs_SubObj)this;

  propOffset = ktab_propOffset(this, graphsOffset, KTAB_GRAPH_PROPSET_VALUE);
  if (propOffset > 0) {
    *value = (picoos_uint32)(g->graphTable[graphsOffset+propOffset]);
    return TRUE;
  }
  else {
    return FALSE;
  }
}


picoos_bool  picoktab_getIntPropPunct (const picoktab_Graphs this, picoos_uint32 graphsOffset, picoos_uint8 * info1, picoos_uint8 * info2)
{
  picoos_uint32 propOffset;
  ktabgraphs_subobj_t * g = (ktabgraphs_SubObj)this;

  propOffset = ktab_propOffset(this, graphsOffset, KTAB_GRAPH_PROPSET_PUNCT);
  if (propOffset > 0) {
      if (g->graphTable[graphsOffset+propOffset] == 2) {
          *info1 = PICODATA_ITEMINFO1_PUNC_SENTEND;
      }
      else {
          *info1 = PICODATA_ITEMINFO1_PUNC_PHRASEEND;
      }
    if (g->graphTable[graphsOffset+1] == '.') {
        *info2 = PICODATA_ITEMINFO2_PUNC_SENT_T;
    }
    else if (g->graphTable[graphsOffset+1] == '?') {
        *info2 = PICODATA_ITEMINFO2_PUNC_SENT_Q;
    }
    else if (g->graphTable[graphsOffset+1] == '!') {
        *info2 = PICODATA_ITEMINFO2_PUNC_SENT_E;
    }
    else {
        *info2 = PICODATA_ITEMINFO2_PUNC_PHRASE;
    }
    return TRUE;
  }
  else {
    return FALSE;
  }
}


picoos_bool  picoktab_getStrPropLowercase (const picoktab_Graphs this, picoos_uint32 graphsOffset, picoos_uchar * lowercase)
{
  picoos_uint32 propOffset;

  propOffset = ktab_propOffset(this, graphsOffset, KTAB_GRAPH_PROPSET_LOWERCASE);
  if (propOffset > 0) {
    ktab_getStrProp(this, graphsOffset, propOffset, lowercase);
    return TRUE;
  }
  else {
    return FALSE;
  }
}


picoos_bool  picoktab_getStrPropGraphsubs1 (const picoktab_Graphs this, picoos_uint32 graphsOffset, picoos_uchar * graphsubs1)
{
  picoos_uint32 propOffset;

  propOffset = ktab_propOffset(this, graphsOffset, KTAB_GRAPH_PROPSET_GRAPHSUBS1);
  if (propOffset > 0) {
    ktab_getStrProp(this, graphsOffset, propOffset, graphsubs1);
    return TRUE;
  }
  else {
    return FALSE;
  }
}


picoos_bool  picoktab_getStrPropGraphsubs2 (const picoktab_Graphs this, picoos_uint32 graphsOffset, picoos_uchar * graphsubs2)
{
  picoos_uint32 propOffset;

  propOffset = ktab_propOffset(this, graphsOffset, KTAB_GRAPH_PROPSET_GRAPHSUBS2);
  if (propOffset > 0) {
    ktab_getStrProp(this, graphsOffset, propOffset, graphsubs2);
    return TRUE;
  }
  else {
    return FALSE;
  }
}
/* *****************************************************************/
/* used for tools */

static void ktab_getUtf8 (picoos_uchar ** pos, picoos_uchar * to)
{
  picoos_uint32 l;
  l = picobase_det_utf8_length(**pos);
  while (l>0) {
    *(to++) = *((*pos)++);
    l--;
  }
  *to = 0;
}

picoos_uint16 picoktab_graphsGetNumEntries(const picoktab_Graphs this)
{
    ktabgraphs_subobj_t * g = (ktabgraphs_SubObj) this;
    return g->nrOffset;
}

void picoktab_graphsGetGraphInfo(const picoktab_Graphs this,
        picoos_uint16 graphIndex, picoos_uchar * from, picoos_uchar * to,
        picoos_uint8 * propset,
        picoos_uint8 * stokenType, picoos_uint8 * stokenSubType,
        picoos_uint8 * value, picoos_uchar * lowercase,
        picoos_uchar * graphsubs1, picoos_uchar * graphsubs2,
        picoos_uint8 * punct) {
    ktabgraphs_subobj_t * g = (ktabgraphs_SubObj) this;
    picoos_uint32 graphsOffset;
    picoos_uint8 * pos;

    /* calculate offset of graph[graphIndex] */
    if (g->sizeOffset == 1) {
        graphsOffset = g->offsetTable[graphIndex];
    } else {
        graphsOffset = g->offsetTable[2 * graphIndex]
                + (g->offsetTable[2 * graphIndex + 1] << 8);
    }
    pos = &(g->graphTable[graphsOffset]);
    *propset = *pos;

    pos++; /* advance to FROM */
    ktab_getUtf8(&pos, from); /* get FROM and advance */
    if ((*propset) & KTAB_GRAPH_PROPSET_TO) {
        ktab_getUtf8(&pos, to); /* get TO and advance */
    } else {
        picoos_strcpy((picoos_char *)to, (picoos_char *)from);
    }
    if ((*propset) & KTAB_GRAPH_PROPSET_TOKENTYPE) {
        (*stokenType) = *(pos++); /* get TOKENTYPE and advance */
    } else {
        (*stokenType) = -1;
    }
    if ((*propset) & KTAB_GRAPH_PROPSET_TOKENSUBTYPE) {
        (*stokenSubType) = *(pos++); /* get TOKENSUBTYPE and advance */
    } else {
        (*stokenSubType) = -1;
    }
    if ((*propset) & KTAB_GRAPH_PROPSET_VALUE) {
        (*value) = *(pos++); /* get VALUE and advance */
    } else {
        (*value) = -1;
    }
    if ((*propset) & KTAB_GRAPH_PROPSET_LOWERCASE) {
        ktab_getUtf8(&pos, lowercase); /* get LOWERCASE and advance */
    } else {
        lowercase[0] = NULLC;
    }
    if ((*propset) & KTAB_GRAPH_PROPSET_GRAPHSUBS1) {
        ktab_getUtf8(&pos, graphsubs1); /* get GRAPHSUBS1 and advance */
    } else {
        graphsubs1[0] = NULLC;
    }
    if ((*propset) & KTAB_GRAPH_PROPSET_GRAPHSUBS2) {
        ktab_getUtf8(&pos, graphsubs2); /* get GRAPHSUBS2 and advance */
    } else {
        graphsubs2[0] = NULLC;
    }
    if ((*propset) & KTAB_GRAPH_PROPSET_PUNCT) {
        (*punct) = *(pos++); /* get PUNCT and advance */
    } else {
        (*punct) = -1;
    }
}

/* ************************************************************/
/* Phones */
/* ************************************************************/

/* overview binary file format for phones kb:

    phones-kb = specids propertytable

    specids = PRIMSTRESSID1 SECSTRESSID1 SYLLBOUNDID1 PAUSEID1 WORDBOUNDID1
              RESERVE1 RESERVE1 RESERVE1

    propertytable = {PHONEPROP2}=256

    PRIMSTRESSID1: one byte, ID of primary stress
    SECSTRESSID1: one byte, ID of secondary stress
    SYLLBOUNDID1: one byte, ID of syllable boundary
    PAUSEID1: one byte, ID of pause
    RESERVE1: reserved for future use

    PHONEPROP2: one byte, max. of 256 phones directly access this table
                to check a property for a phone; binary properties
                encoded (1 bit per prop)
       least significant bit: vowel
                    next bit: diphth
                    next bit: glott
                    next bit: nonsyllvowel
                    next bit: syllcons
       3 bits spare
 */

#define KTAB_START_SPECIDS   0
#define KTAB_IND_PRIMSTRESS  0
#define KTAB_IND_SECSTRESS   1
#define KTAB_IND_SYLLBOUND   2
#define KTAB_IND_PAUSE       3
#define KTAB_IND_WORDBOUND   4

#define KTAB_START_PROPS     8


typedef struct ktabphones_subobj *ktabphones_SubObj;

typedef struct ktabphones_subobj {
    picoos_uint8 *specids;
    picoos_uint8 *props;
} ktabphones_subobj_t;


/* bitmasks to extract the property info from props */
#define KTAB_PPROP_VOWEL        '\x01'
#define KTAB_PPROP_DIPHTH       '\x02'
#define KTAB_PPROP_GLOTT        '\x04'
#define KTAB_PPROP_NONSYLLVOWEL '\x08'
#define KTAB_PPROP_SYLLCONS     '\x10'


static pico_status_t ktabPhonesInitialize(register picoknow_KnowledgeBase this,
                                          picoos_Common common) {
    ktabphones_subobj_t * ktabphones;

    PICODBG_DEBUG(("start"));

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    ktabphones = (ktabphones_subobj_t *) this->subObj;
    ktabphones->specids = &(this->base[KTAB_START_SPECIDS]);
    ktabphones->props   = &(this->base[KTAB_START_PROPS]);
    return PICO_OK;
}

static pico_status_t ktabPhonesSubObjDeallocate(register picoknow_KnowledgeBase this,
                                                picoos_MemoryManager mm) {
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}

pico_status_t picoktab_specializePhonesKnowledgeBase(picoknow_KnowledgeBase this,
                                                     picoos_Common common) {
    if (NULL == this) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    this->subDeallocate = ktabPhonesSubObjDeallocate;
    this->subObj = picoos_allocate(common->mm, sizeof(ktabphones_subobj_t));
    if (NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                       NULL, NULL);
    }
    return ktabPhonesInitialize(this, common);
}

picoktab_Phones picoktab_getPhones(picoknow_KnowledgeBase this) {
    if (NULL == this) {
        return NULL;
    } else {
        return (picoktab_Phones) this->subObj;
    }
}


/* Phones methods */

picoos_uint8 picoktab_hasVowelProp(const picoktab_Phones this,
                                   const picoos_uint8 ch) {
    return (KTAB_PPROP_VOWEL & ((ktabphones_SubObj)this)->props[ch]);
}
picoos_uint8 picoktab_hasDiphthProp(const picoktab_Phones this,
                                    const picoos_uint8 ch) {
    return (KTAB_PPROP_DIPHTH & ((ktabphones_SubObj)this)->props[ch]);
}
picoos_uint8 picoktab_hasGlottProp(const picoktab_Phones this,
                                   const picoos_uint8 ch) {
    return (KTAB_PPROP_GLOTT & ((ktabphones_SubObj)this)->props[ch]);
}
picoos_uint8 picoktab_hasNonsyllvowelProp(const picoktab_Phones this,
                                          const picoos_uint8 ch) {
    return (KTAB_PPROP_NONSYLLVOWEL & ((ktabphones_SubObj)this)->props[ch]);
}
picoos_uint8 picoktab_hasSyllconsProp(const picoktab_Phones this,
                                      const picoos_uint8 ch) {
    return (KTAB_PPROP_SYLLCONS & ((ktabphones_SubObj)this)->props[ch]);
}

picoos_bool picoktab_isSyllCarrier(const picoktab_Phones this,
                                    const picoos_uint8 ch) {
    picoos_uint8 props;
    props = ((ktabphones_SubObj)this)->props[ch];
    return (((KTAB_PPROP_VOWEL & props) &&
             !(KTAB_PPROP_NONSYLLVOWEL & props))
            || (KTAB_PPROP_SYLLCONS & props));
}

picoos_bool picoktab_isPrimstress(const picoktab_Phones this,
                                   const picoos_uint8 ch) {
    return (ch == ((ktabphones_SubObj)this)->specids[KTAB_IND_PRIMSTRESS]);
}
picoos_bool picoktab_isSecstress(const picoktab_Phones this,
                                  const picoos_uint8 ch) {
    return (ch == ((ktabphones_SubObj)this)->specids[KTAB_IND_SECSTRESS]);
}
picoos_bool picoktab_isSyllbound(const picoktab_Phones this,
                                  const picoos_uint8 ch) {
    return (ch == ((ktabphones_SubObj)this)->specids[KTAB_IND_SYLLBOUND]);
}
picoos_bool picoktab_isWordbound(const picoktab_Phones this,
                                  const picoos_uint8 ch) {
    return (ch == ((ktabphones_SubObj)this)->specids[KTAB_IND_WORDBOUND]);
}
picoos_bool picoktab_isPause(const picoktab_Phones this,
                              const picoos_uint8 ch) {
    return (ch == ((ktabphones_SubObj)this)->specids[KTAB_IND_PAUSE]);
}

picoos_uint8 picoktab_getPrimstressID(const picoktab_Phones this) {
    return ((ktabphones_SubObj)this)->specids[KTAB_IND_PRIMSTRESS];
}
picoos_uint8 picoktab_getSecstressID(const picoktab_Phones this) {
    return ((ktabphones_SubObj)this)->specids[KTAB_IND_SECSTRESS];
}
picoos_uint8 picoktab_getSyllboundID(const picoktab_Phones this) {
    return ((ktabphones_SubObj)this)->specids[KTAB_IND_SYLLBOUND];
}
picoos_uint8 picoktab_getWordboundID(const picoktab_Phones this) {
    return ((ktabphones_SubObj)this)->specids[KTAB_IND_WORDBOUND];
}
picoos_uint8 picoktab_getPauseID(const picoktab_Phones this) {
    return ((ktabphones_SubObj)this)->specids[KTAB_IND_PAUSE];
}

/* ************************************************************/
/* Pos */
/* ************************************************************/

/* overview binary file format for pos kb:

    pos-kb = header posids
    header = {COUNT2 OFFS2}=8
    posids = {POSID1 {PARTID1}0:8}1:

    where POSID1 is the value of the (combined) part-of-speech symbol,
    and {PARTID1} are the symbol values of its components (empty if it
    is not a combined symbol). The {PARTID1} list is sorted.
    Part-of-speech symbols with equal number of components are grouped
    together.

    The header contains information about these groups:

    COUNT2 specifies the number of elements in the group, and OFFS2
    specifies the offset (relative to the beginning of the kb) where
    the group data starts, i.e.:

    25   32  -> 25 not-combined elements, starting at offset 32
    44   57  -> 44 elements composed of 2 symbols, starting at offset 57
    23  189  -> 23 elements composed of 3 symbols, starting at offset 189
    ...

    Currently, each symbol may be composed of up to 8 other symbols.
    Therefore, the header has 8 entries, too. The header starts with
    the unique POS list, and then in increasing order, 2 symbols, 3
    symbols,...

Zur Anschauung die ge-printf-te Version:

 25   32
 44   57
 23  189
 12  281
  4  341
  1  365
  0    0
  0    0
 33 |
 34 |
 35 |
 60 |
 etc.
 36 |  35  60
 50 |  35  95
 51 |  35  97
 58 |  35 120
 59 |  35 131
 61 |  60  75
 63 |  60  95
 64 |  60  97
 etc.
 42 |  35  60 117
 44 |  35  60 131
 45 |  35  73  97
 48 |  35  84  97
 54 |  35  97 131
 56 |  35 113 120
 57 |  35 117 120
 62 |  60  84 122
 etc.
 */

typedef struct ktabpos_subobj *ktabpos_SubObj;

typedef struct ktabpos_subobj {
    picoos_uint16 nrcomb[PICOKTAB_MAXNRPOS_IN_COMB];
    picoos_uint8 *nrcombstart[PICOKTAB_MAXNRPOS_IN_COMB];
} ktabpos_subobj_t;


static pico_status_t ktabPosInitialize(register picoknow_KnowledgeBase this,
                                       picoos_Common common) {
    ktabpos_subobj_t *ktabpos;
    picoos_uint16 osprev;
    picoos_uint16 os, pos;
    picoos_uint8 i;

    PICODBG_DEBUG(("start"));

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    ktabpos = (ktabpos_subobj_t *)this->subObj;

    os = 0;
    for (i = 0, pos = 0; i < PICOKTAB_MAXNRPOS_IN_COMB; i++, pos += 4) {
        ktabpos->nrcomb[i] = ((picoos_uint16)(this->base[pos+1])) << 8 |
            this->base[pos];
        if (ktabpos->nrcomb[i] > 0) {
            osprev = os;
            os = ((picoos_uint16)(this->base[pos+3])) << 8 | this->base[pos+2];
            ktabpos->nrcombstart[i] = &(this->base[os]);
            PICODBG_TRACE(("i %d, pos %d, nr %d, osprev %d, os %d", i, pos,
                           ktabpos->nrcomb[i], osprev, os));
            if (osprev >= os) {
                /* cannot be, in a valid kb */
                return picoos_emRaiseException(common->em,
                                               PICO_EXC_FILE_CORRUPT,
                                               NULL, NULL);
            }
        } else {
            if (i == 0) {
                /* cannot be, in a valid kb */
                return picoos_emRaiseException(common->em,
                                               PICO_EXC_FILE_CORRUPT,
                                               NULL, NULL);
            }
            ktabpos->nrcombstart[i] = NULL;
        }
    }
    return PICO_OK;
}

static pico_status_t ktabPosSubObjDeallocate(register picoknow_KnowledgeBase this,
                                             picoos_MemoryManager mm) {
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}

pico_status_t picoktab_specializePosKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common) {
    if (NULL == this) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    this->subDeallocate = ktabPosSubObjDeallocate;
    this->subObj = picoos_allocate(common->mm, sizeof(ktabpos_subobj_t));
    if (NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                       NULL, NULL);
    }
    return ktabPosInitialize(this, common);
}

picoktab_Pos picoktab_getPos(picoknow_KnowledgeBase this) {
    if (NULL == this) {
        return NULL;
    } else {
        return (picoktab_Pos) this->subObj;
    }
}


/* Pos methods */

static picoos_int16 ktab_isEqualPosGroup(const picoos_uint8 *grp1,
                                         const picoos_uint8 *grp2,
                                         picoos_uint8 len)
{
    /* if both, grp1 and grp2 would be sorted in ascending order
       we could implement a function picoktab_comparePosGroup in
       a similar manner as strcmp */

    picoos_uint16 i, j, equal;

    equal = 1;

    i = 0;
    while (equal && (i < len)) {
        /* search grp1[i] in grp2 */
        j = 0;
        while ((j < len) && (grp1[i] != grp2[j])) {
            j++;
        }
        equal = (j < len);
        i++;
    }

    return equal;
}


picoos_bool picoktab_isUniquePos(const picoktab_Pos this,
                                  const picoos_uint8 pos) {
    ktabpos_subobj_t *ktabpos;
    picoos_uint16 i;

    /* speed-up possible with e.g. binary search */

    ktabpos = (ktabpos_subobj_t *)this;
    PICODBG_TRACE(("pos %d, nrcombinations %d", pos, ktabpos->nrcomb[0]));
    i = 0;
    while ((i < ktabpos->nrcomb[0]) && (pos > ktabpos->nrcombstart[0][i])) {
        PICODBG_TRACE(("compare with pos %d at position %d",
                       ktabpos->nrcombstart[0][i], pos, i));
        i++;
    }
    return ((i < ktabpos->nrcomb[0]) && (pos == ktabpos->nrcombstart[0][i]));
}


picoos_bool picoktab_isPartOfPosGroup(const picoktab_Pos this,
                                       const picoos_uint8 pos,
                                       const picoos_uint8 posgroup)
{
    ktabpos_subobj_t *ktabpos;
    picoos_uint8 *grp;
    picoos_uint16 i, j, n, s, grplen;
    picoos_uint8 *e;
    picoos_uint8 found;

    ktabpos = (ktabpos_subobj_t *) this;

    grp = NULL;
    found = FALSE;
    grplen = 0;

    /* currently, a linear search is required to find 'posgroup'; the
       knowledge base should be extended to allow for a faster search */

    /* treat case i==0, grplen==0, ie. pos == posgroup */
    if (pos == posgroup) {
        found = TRUE;
    }

    i = 1;
    while ((grp == NULL) && (i < PICOKTAB_MAXNRPOS_IN_COMB)) {
        n = ktabpos->nrcomb[i];       /* number of entries */
        e = ktabpos->nrcombstart[i];  /* ptr to first entry */
        s = i + 2;                    /* size of an entry in bytes */
        /* was with while starting at 0:
        s = i > 0 ? i + 2 : 1;
        */
        j = 0;
        while ((grp == NULL) && (j < n)) {
            if (posgroup == e[0]) {
                grp = e + 1;
                grplen = s - 1;
            }
            e += s;
            j++;
        }
        i++;
    }

    /* test if 'pos' is contained in the components of 'posgroup' */
    if (grp != NULL) {
        for (i = 0; !found && (i < grplen); i++) {
            if (pos == grp[i]) {
                found = TRUE;
            }
        }

        /* just a way to test picoktab_getPosGroup */
        /*
        PICODBG_ASSERT(picoktab_getPosGroup(this, grp, grplen) == posgroup);
        */
    }

    return found;
}


picoos_uint8 picoktab_getPosGroup(const picoktab_Pos this,
                                  const picoos_uint8 *poslist,
                                  const picoos_uint8 poslistlen)
{
    picoos_uint8 poscomb;
    ktabpos_subobj_t *ktabpos;
    picoos_uint16 i, j, n, s;
    picoos_uint8 *e;

    ktabpos = (ktabpos_subobj_t *) this;
    poscomb = 0;

    if ((poslistlen > 0) && (poslistlen <= PICOKTAB_MAXNRPOS_IN_COMB)) {
        i = poslistlen - 1;
        if (i > 0) {
            n = ktabpos->nrcomb[i];       /* number of entries */
            e = ktabpos->nrcombstart[i];  /* ptr to first entry */
            s = i + 2;                    /* size of an entry in bytes */
            j = 0;
            while (!poscomb && (j < n)) {
                if (ktab_isEqualPosGroup(poslist, e + 1, poslistlen)) {
                    poscomb = *e;
                }
                e += s;
                j++;
            }
            if (!poscomb) {
                /* combination not found; shouldn't occur if lingware OK! */
                /* contingency solution: take first */
                PICODBG_WARN(("dynamically created POS combination not found in table; taking first (%i)",poslist[0]));
                poscomb = poslist[0];
            }
        } else {  /* not a composed POS */
            poscomb = poslist[0];
        }
    }

    return poscomb;
}

#ifdef __cplusplus
}
#endif


/* end */
