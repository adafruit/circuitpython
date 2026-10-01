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
 * @file picosig.h
 *
 * Signal Generation PU - Header file
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */
/**
 * @addtogroup picosig
 *
 * <b> Pico Signal Generation module </b>\n
 *
 * Pico Sig is the PU that makes the parametric representation produce a speech signal.
 * The PU receives parametric vectors and translates them into signal vectors.
 * Most of the processing is based on this 1 to 1 relationship between input and output vectors.
 *
 * NOTE : "picosig" includes logically "picosig2" module, that is the DSP implementation of the signal generation.
*/

#ifndef PICOSIG_H_
#define PICOSIG_H_

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* *******************************************************************************
 *   items related to the generic interface
 ********************************************************************************/
picodata_ProcessingUnit picosig_newSigUnit(
        picoos_MemoryManager mm,
        picoos_Common common,
        picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut,
        picorsrc_Voice voice);

#ifdef __cplusplus
}
#endif

#endif /*PICOSIG_H_*/
