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
 * @file picobase.h
 *
 * base functionality
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#ifndef PICOBASE_H_
#define PICOBASE_H_

#include "picoos.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* maximum number of bytes of an UTF8 character */
#define PICOBASE_UTF8_MAXLEN    4

typedef picoos_uint8  picobase_utf8char[PICOBASE_UTF8_MAXLEN+1];  /* always zero terminated */
typedef picoos_uint8  picobase_utf8;
typedef picoos_uint16 picobase_utf16;
typedef picoos_uint32 picobase_utf32;

/* ***************************************************************/
/* Unicode UTF8 functions */
/* ***************************************************************/

/**
 * Determines the number of UTF8 characters contained in
 *            the UTF8 string 'utf8str' of maximum length maxlen (in bytes)
 * @param    utf8str : a string encoded in UTF8
 * @param    maxlen  : max length (in bytes) accessible in utf8str
 * @return   >=0 : length of the UTF8 string in number of UTF8 characters
 *                     up to the first '\0' or maxlen
 * @return   <0 : not starting with a valid UTF8 character
 * @remarks  strict implementation, not allowing invalid utf8
*/
picoos_int32 picobase_utf8_length(const picoos_uint8 *utf8str,
                                  const picoos_uint16 maxlen);


/**
 * Determines the number of bytes an UTF8 character used based
 *            on the first byte of the UTF8 character
 * @param    firstchar: the first (and maybe only) byte of an UTF8 character
 * @return   positive value in {1,4} : number of bytes of the UTF8 character
 * @return   0 :if not a valid UTF8 character start
 * @remarks strict implementation, not allowing invalid utf8
*/
/* picoos_uint8 picobase_det_utf8_length(const picoos_uint8 firstchar); */

#define picobase_det_utf8_length(x)  (  ((x)<(picoos_uint8)'\200')?1:(((x)>=(picoos_uint8)'\370')?0:(((x)>=(picoos_uint8)'\360')?4:(((x)>=(picoos_uint8)'\340')?3:(((x)>=(picoos_uint8)'\300')?2:0)))) )

/**
 * Converts the content of 'utf8str' to lowercase and stores it on 'lowercase'
 *            on the first byte of the UTF8 character
 * @param    utf8str : utf8 string
 * @param    lowercaseMaxLen : maximal number of bytes available in 'lowercase'
 * @param    lowercase : string converted to lowercase (output)
 * @param    done : flag to report success/failure of the operation (output)
 * @return  TRUE if successful, FALSE otherwise
*/
picoos_int32 picobase_lowercase_utf8_str (picoos_uchar utf8str[], picoos_char lowercase[], picoos_int32 lowercaseMaxLen, picoos_uint8 * done);

/**
 * Converts the content of 'utf8str' to upperrcase and stores it on 'uppercase'
 * @param    utf8str : utf8 string
 * @param    uppercase : string converted to uppercase (output)
 * @param    uppercaseMaxLen : maximal number of bytes available in 'uppercase'
 * @param    done : flag to report success/failure of the operation (output)
 * @return  TRUE if successful, FALSE otherwise
*/
picoos_int32 picobase_uppercase_utf8_str (picoos_uchar utf8str[], picoos_char uppercase[], int uppercaseMaxLen, picoos_uint8 * done);

/**
 * Gets next UTF8 character 'utf8char' from the UTF8 string
 *            'utf8s' starting at position 'pos'
 * @param    utf8s : UTF8 string
 * @param    utf8slenmax : max length accessible in utf8s
 * @param    pos : position from where the UTF8 character is checked and copied
 *            (set also as output to the position directly following the UTF8 char)
 * @param    utf8char : zero terminated UTF8 character containing 1 to 4 bytes (output)
 * @return  TRUE if okay
 * @return  FALSE if there is no valid UTF8 char or no more UTF8 char available within utf8len
*/
picoos_uint8 picobase_get_next_utf8char(const picoos_uint8 *utf8s,
                                        const picoos_uint32 utf8slenmax,
                                        picoos_uint32 *pos,
                                        picobase_utf8char utf8char);

/**
 * Same as picobase_get_next_utf8char
 *            without copying the char to utf8char
*/
picoos_uint8 picobase_get_next_utf8charpos(const picoos_uint8 *utf8s,
                                           const picoos_uint32 utf8slenmax,
                                           picoos_uint32 *pos);

/**
 * Gets previous UTF8 character 'utf8char' from the UTF8 string
 *             'utf8s' starting the backward search at position 'pos-1'
 * @param    utf8s : UTF8 string
 * @param    utf8slenmin : min length accessible in utf8s
 * @param    pos : the search for the prev UTF8 char starts at [pos-1]
 *            (set also as output to the start position of the prev UTF8 character)
 * @param    utf8char : zero terminated UTF8 character containing 1 to 4 bytes (output)
 * @return  TRUE if okay
 * @return  FALSE if there is no valid UTF8 char preceeding pos or no more UTF8 char available within utf8len
*/
picoos_uint8 picobase_get_prev_utf8char(const picoos_uint8 *utf8s,
                                        const picoos_uint32 utf8slenmin,
                                        picoos_uint32 *pos,
                                        picobase_utf8char utf8char);

/**
 * Same as picobase_get_prev_utf8char
 *            without copying the char to utf8char
*/
picoos_uint8 picobase_get_prev_utf8charpos(const picoos_uint8 *utf8s,
                                           const picoos_uint32 utf8slenmin,
                                           picoos_uint32 *pos);


/**
 * returns TRUE if the input string is UTF8 and uppercase
 * @param    str : UTF8 string
 * @param    strmaxlen : max length for the input string
 * @return  TRUE if string is UTF8 and uppercase
 * @return  FALSE otherwise
*/
extern picoos_bool picobase_is_utf8_uppercase (picoos_uchar str[], picoos_int32 strmaxlen);

/**
 * returns TRUE if the input string is UTF8 and lowercase
 * @param    str : UTF8 string
 * @param    strmaxlen : max length for the input string
 * @return  TRUE if string is UTF8 and lowercase
 * @return  FALSE otherwise
*/
extern picoos_bool picobase_is_utf8_lowercase (picoos_uchar str[], picoos_int32 strmaxlen);

#ifdef __cplusplus
}
#endif

#endif /*PICOBASE_H_*/
