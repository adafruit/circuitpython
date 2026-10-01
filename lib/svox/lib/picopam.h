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
 * @file picopam.h
 *
 * Phonetic to Acoustic Mapping PU - Header file
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */
/**
 * @addtogroup picopam

 * <b> Phonetic to acoustic mapping module </b>\n
 *
 * This module is responsible for mapping the phonetic domain features generated from text analysis
 * into parametric representations suitable for signal generation.  As such it is the interface
 * between text analysis and signal generation
 *
 * Most the processing of PAM is logically splittable as follows
 * - building a suitable symbolic feature vector set for the sentence
 * - Feeding Decision Trees with the symbolic seqence vector set
 * - Colecting the parametric output of the Decision trees into suitable items to be sent to following PU's
 *
 * To perform the decision tree feeding and output collection the PU uses an internal buffer. This
 * buffer is used several times with different meanings.
 * - While building the symbolic feature vector set for the sentence this data structure stores syllable relevant data.
 *   The corresponding phonetic data is stored outside as a single string of phonetic id's for all the sentence.
 * - While feeding the decision trees the data structure is used to represent data for phonemes of the syllable
 * - Addiotional data strucures are mantained to temporarily store items not pertaining to the PAM processing, for later resynchronization
 *
 * A quite detailed description of PAM processing is in the Know How section of the repository.
 */

#ifndef PICOPAM_H_
#define PICOPAM_H_

#include "picodata.h"
#include "picokdt.h"
#include "picokpdf.h"
#include "picoktab.h"
#include "picokdbg.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/*------------------------------------------------------------------
Service routines
------------------------------------------------------------------*/
picodata_ProcessingUnit picopam_newPamUnit(
    picoos_MemoryManager mm,    picoos_Common common,
    picodata_CharBuffer cbIn,   picodata_CharBuffer cbOut,
    picorsrc_Voice voice);

#ifdef __cplusplus
}
#endif
#endif /*PICOPAM_H_*/
