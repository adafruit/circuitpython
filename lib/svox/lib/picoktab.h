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
 * @file picoktab.h
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
/**
 * @addtogroup picoktab

 * <b> Symbol tables needed at runtime </b>\n
 *
*/

#ifndef PICOKTAB_H_
#define PICOKTAB_H_

#include "picoos.h"
#include "picoknow.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ************************************************************/
/* fixed IDs type and functions */
/* ************************************************************/

/**  object   : FixedIds
 *   shortcut : ids
 */
typedef struct picoktab_fixed_ids * picoktab_FixedIds;

typedef struct picoktab_fixed_ids {
    picoos_uint8 phonStartId;
    picoos_uint8 phonTermId;
} picoktab_fixed_ids_t;

/* to be used by picorsrc only */
pico_status_t picoktab_specializeIdsKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common);

picoktab_FixedIds picoktab_getFixedIds(picoknow_KnowledgeBase this);


/* ************************************************************/
/* Graphs type and functions */
/* ************************************************************/

typedef struct picoktab_graphs *picoktab_Graphs;

/* to be used by picorsrc only */
pico_status_t picoktab_specializeGraphsKnowledgeBase(picoknow_KnowledgeBase this,
                                                     picoos_Common common);

/* return kb graphs for usage in PU */
picoktab_Graphs picoktab_getGraphs(picoknow_KnowledgeBase this);

/* graph access routine: if the desired graph 'utf8graph' exists in
   the graph table a graph offset > 0 is returned, which then can be
   used to access the properties */
picoos_uint32 picoktab_graphOffset(const picoktab_Graphs this,
                                   picoos_uchar * utf8graph);


/* check if UTF8 char 'graph' has property vowellike, return non-zero
   if 'ch' has the property, 0 otherwise */
picoos_uint8 picoktab_hasVowellikeProp(const picoktab_Graphs this,
                                       const picoos_uint8 *graph,
                                       const picoos_uint8 graphlenmax);

/* graph properties access routines: if graph with offset 'graphsOffset' has the
   desired property, returns TRUE if 'ch' has the property, FALSE otherwise  */
picoos_bool  picoktab_getIntPropTokenType(const picoktab_Graphs this,
                                           picoos_uint32 graphsOffset,
                                           picoos_uint8 *stokenType);
picoos_bool  picoktab_getIntPropTokenSubType(const picoktab_Graphs this,
                                              picoos_uint32 graphsOffset,
                                              picoos_int8 *stokenSubType);
picoos_bool  picoktab_getIntPropValue(const picoktab_Graphs this,
                                      picoos_uint32 graphsOffset,
                                      picoos_uint32 *value);
picoos_bool  picoktab_getStrPropLowercase(const picoktab_Graphs this,
                                          picoos_uint32 graphsOffset,
                                          picoos_uchar *lowercase);
picoos_bool  picoktab_getStrPropGraphsubs1(const picoktab_Graphs this,
                                           picoos_uint32 graphsOffset,
                                           picoos_uchar *graphsubs1);
picoos_bool  picoktab_getStrPropGraphsubs2(const picoktab_Graphs this,
                                           picoos_uint32 graphsOffset,
                                           picoos_uchar *graphsubs2);
picoos_bool  picoktab_getIntPropPunct(const picoktab_Graphs this,
                                      picoos_uint32 graphsOffset,
                                      picoos_uint8 *info1,
                                      picoos_uint8 *info2);

picoos_uint16 picoktab_graphsGetNumEntries(const picoktab_Graphs this);
void picoktab_graphsGetGraphInfo(const picoktab_Graphs this,
        picoos_uint16 graphIndex, picoos_uchar * from, picoos_uchar * to,
        picoos_uint8 * propset,
        picoos_uint8 * stokenType, picoos_uint8 * stokenSubType,
        picoos_uint8 * value, picoos_uchar * lowercase,
        picoos_uchar * graphsubs1, picoos_uchar * graphsubs2,
        picoos_uint8 * punct);


/* ************************************************************/
/* Phones type and functions */
/* ************************************************************/

/* to be used by picorsrc only */
pico_status_t picoktab_specializePhonesKnowledgeBase(picoknow_KnowledgeBase this,
                                                     picoos_Common common);

typedef struct picoktab_phones *picoktab_Phones;

/* return kb Phones for usage in PU */
picoktab_Phones picoktab_getPhones(picoknow_KnowledgeBase this);

/* check if 'ch' has a property, return non-zero if 'ch' has the
   property, 0 otherwise */
picoos_uint8 picoktab_hasVowelProp(const picoktab_Phones this,
                                   const picoos_uint8 ch);
picoos_uint8 picoktab_hasDiphthProp(const picoktab_Phones this,
                                    const picoos_uint8 ch);
picoos_uint8 picoktab_hasGlottProp(const picoktab_Phones this,
                                   const picoos_uint8 ch);
picoos_uint8 picoktab_hasNonsyllvowelProp(const picoktab_Phones this,
                                          const picoos_uint8 ch);
picoos_uint8 picoktab_hasSyllconsProp(const picoktab_Phones this,
                                      const picoos_uint8 ch);

/* to speed up processing for often used combinations of properties
   the following functions are provided, which check if the property
   combination is true for 'ch' */
picoos_bool picoktab_isSyllCarrier(const picoktab_Phones this,
                                    const picoos_uint8 ch);

/* some properties can be assigned to a single sym only, check if 'ch'
   is a special sym, return TRUE if it is the special sym, FALSE
   otherwise */
picoos_bool picoktab_isPrimstress(const picoktab_Phones this,
                                   const picoos_uint8 ch);
picoos_bool picoktab_isSecstress(const picoktab_Phones this,
                                  const picoos_uint8 ch);
picoos_bool picoktab_isSyllbound(const picoktab_Phones this,
                                  const picoos_uint8 ch);
picoos_bool picoktab_isWordbound(const picoktab_Phones this,
                                  const picoos_uint8 ch);
picoos_bool picoktab_isPause(const picoktab_Phones this,
                              const picoos_uint8 ch);

/* get specific sym values */
picoos_uint8 picoktab_getPrimstressID(const picoktab_Phones this);
picoos_uint8 picoktab_getSecstressID(const picoktab_Phones this);
picoos_uint8 picoktab_getSyllboundID(const picoktab_Phones this);
picoos_uint8 picoktab_getWordboundID(const picoktab_Phones this);
picoos_uint8 picoktab_getPauseID(const picoktab_Phones this);

/* ************************************************************/
/* Pos type and functions */
/* ************************************************************/

/* to be used by picorsrc only */
pico_status_t picoktab_specializePosKnowledgeBase(picoknow_KnowledgeBase this,
                                                  picoos_Common common);

typedef struct picoktab_pos *picoktab_Pos;

#define PICOKTAB_MAXNRPOS_IN_COMB  8

/* return kb Pos for usage in PU */
picoktab_Pos picoktab_getPos(picoknow_KnowledgeBase this);

/* returns TRUE if 'pos' is the ID of a unique (ie. non-combined) POS,
   returns FALSE otherwise */
picoos_bool picoktab_isUniquePos(const picoktab_Pos this,
                                  const picoos_uint8 pos);

/* returns TRUE if the non-combined 'pos' is one of the POSes in the
   combined POS group 'posgroup, returns FALSE otherwise. Note: if
   'posgroup' is itself non-combined, this function returns TRUE if it
   matches with 'pos', and FALSE otherwise */
picoos_bool picoktab_isPartOfPosGroup(const picoktab_Pos this,
                                       const picoos_uint8 pos,
                                       const picoos_uint8 posgroup);

/* return the combined POS group ID that is a representative ID for
   all the 'poslistlen' POSes (which can be combined themselves) in
   poslist. Returns '0' in case of error. */
picoos_uint8 picoktab_getPosGroup(const picoktab_Pos this,
                                  const picoos_uint8 *poslist,
                                  const picoos_uint8 poslistlen);

#ifdef __cplusplus
}
#endif


#endif /*PICOKTAB_H_*/
