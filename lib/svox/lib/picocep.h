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
 * @file picocep.h
 *
 * Cepstral Smoothing PU - Header file
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#ifndef PICOCEP_H_
#define PICOCEP_H_

#include "picoos.h"
#include "picodata.h"
#include "picorsrc.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* ******************************************************************************
 *   items related to the generic interface
 ********************************************************************************/
picodata_ProcessingUnit picocep_newCepUnit(picoos_MemoryManager mm,
        picoos_Common common, picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut, picorsrc_Voice voice);

#ifdef __cplusplus
}
#endif

#endif /*PICOCEP_H_*/
