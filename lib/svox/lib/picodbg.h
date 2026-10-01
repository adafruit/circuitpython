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
 *
 * @file picodbg.h
 *
 * Provides functions and macros to debug the Pico system and to trace
 * the execution of its code.
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 */

/**
 * @addtogroup picodbg
 * ---------------------------------------------------\n
 * <b> Pico Debug Support </b>\n
 * ---------------------------------------------------\n
 * GENERAL REMARKS
 * ---------------
 * This module provides a set of macros to help debug the Pico system.
 * The usage of macros allows for completely removing debug code from
 * the binaries delivered to customers. To enable diagnostic output
 * the preprocessor symbol PICO_DEBUG has to be defined.
 *
 * By using global variables (to store the current log level etc.)
 * this module violates a basic Pico design principle!
 *
 * Justification:\n
 * - going without global data would reduce the functionality
 *   of this module considerably (e.g., log level could not be
 *   changed at runtime etc.)
 * - at the moment, the only known system interdicting global
 *   variables is Symbian; but even there global variables are
 *   possible by using thread-local storage
 * - allocating global data on the heap would require to pass
 *   a handle to this memory block to all routines of this
 *   module which in turn implies that _every_ function in the
 *   Pico system would require a pointer to some global data;
 *   obviously, this would be very awkward
 *
 * Furthermore, this module uses the non-standardized but handy
 * __FUNCTION__ macro. It expands to the name of the enclosing
 * function. For compilers not supporting this macro simply
 * define __FUNCTION__ as an empty string.
 *
 *
 * INITIALIZATION/TERMINATION\n
 * --------------------------\n
 * Before using any debug macros, this module has to be initialized
 * by calling PICODBG_INITIALIZE(). If the routines are not needed
 * anymore, PICODBG_TERMINATE() has to be called to terminate the
 * module (e.g., to close the log file).
 *
 *
 * TRACING\n
 * -------\n
 * Each tracing message is associated with a log level which describes
 * its severity. The following levels are supported:
 * - Trace - Very detailed log messages, potentially of a high
 *            frequency and volume
 * - Debug - Less detailed and/or less frequent debugging messages
 * - Info  - Informational messages
 * - Warn  - Warnings which don't appear to the Pico user
 * - Error - Error messages
 *
 * Tracing messages can use the well-known printf format specification.
 * But because variadic macros (macros with a variable no. arguments)
 * are not commonly supported by compilers a little trick is used
 * which requires the format string and its arguments to be enclosed
 * in double parenthesis:
 *
 * - PICODBG_INFO(("hello, world!"));
 * - PICODBG_TRACE(("argc=%d", argc));
 *    ...
 *
 * Each tracing message is expected to be a single line of text. Some
 * contextual information (e.g., log level, time and date, source file
 * and line number) and a newline are automatically added. The output
 * format can be customized by a call to PICODBG_SET_OUTPUT_FORMAT().
 *
 * Sample output:
 *    - *** info|2008-04-03|14:51:36|dbgdemo.c(15)|hello world
 *    - *** trace|2008-04-03|14:51:36|dbgdemo.c(16)|argc=2
 *    - ...
 *
 * To compose a tracing message line consisting of, e.g. the elements
 * of an array, on the Info level two additional macros shown in the
 * following example are provided:
 *
 *    PICODBG_INFO_CTX();\n
 *    for (i = 0; i < len; i++)\n
 *        ...some calc with arr and i\n
 *        PICODBG_INFO_MSG((" %d", arr[i]));\n
 *    }\n
 *    PICODBG_INFO_MSG(("\n"));\n
 *
 * Colored output of tracing messages helps to capture severe problems
 * quickly. This feature is supported on the Windows platform and on
 * platforms supporting ANSI escape codes. PICODBG_ENABLE_COLORS() lets
 * you turn on and off colored output.
 *
 *
 * FILTERING\n
 * ---------\n
 * By calling PICODBG_SET_LOG_LEVEL() the log level may be changed at
 * any time to increase/decrease the amount of debugging output.
 *
 * By calling PICODBG_SET_LOG_FILTERFN() the log filter may be changed
 * at any time to change the source file name being used as filter for
 * log messages (ie. only tracing info of the specified file will be
 * logged). To disable the file name based filter set the filter file
 * name to an empty string.
 *
 * Future version of this module might provide further filtering
 * possibilities (e.g., filtering based on function names * etc.).
 *
 *
 * LOGGING\n
 * -------\n
 * By default, tracing messages are output to the console (stderr).
 * This allows for separating diagnostic output from other console
 * output to stdout. In addition, tracing messages may be saved to
 * a file by calling PICODBG_SET_LOG_FILE().
 * Currently, file output is the only additional output target; but
 * on embedded systems, more output targets may be required (e.g.,
 * sending output to a serial port or over the network).
 *
 *
 * ASSERTIONS\n
 * ----------\n
 * To support the 'design/programming by contract' paradigm, this
 * module also provides assertions. PICODBG_ASSERT(expr) evualuates
 * an expression and, when the result is false, prints a diagnostic
 * message and aborts the program.
 *
 *
 * FUTURE EXTENSIONS\n
 * -----------------\n
 * - advanced tracing functions to dump complex data
 * - debug memory allocation that can be used to assist in
 *   finding memory problems
 */


#if !defined(__PICODBG_H__)
#define __PICODBG_H__

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* Not all compilers support the __FUNCTION__ macro */
#if !defined(__FUNCTION__) && !defined(__GNUC__)
#define __FUNCTION__ ""
#endif


/* Log levels sorted by severity */
#define PICODBG_LOG_LEVEL_ERROR     1
#define PICODBG_LOG_LEVEL_WARN      2
#define PICODBG_LOG_LEVEL_INFO      3
#define PICODBG_LOG_LEVEL_DEBUG     4
#define PICODBG_LOG_LEVEL_TRACE     5

/* Output format flags */
#define PICODBG_SHOW_LEVEL          0x0001
#define PICODBG_SHOW_DATE           0x0002
#define PICODBG_SHOW_TIME           0x0004
#define PICODBG_SHOW_SRCNAME        0x0008
#define PICODBG_SHOW_SRCLINE        0x0010
#define PICODBG_SHOW_SRCALL         (PICODBG_SHOW_SRCNAME | PICODBG_SHOW_SRCLINE)
#define PICODBG_SHOW_FUNCTION       0x0020
#define PICODBG_SHOW_POS            (PICODBG_SHOW_SRCALL | PICODBG_SHOW_FUNCTION)

/* definition of PICO_DEBUG enables debugging code */
#if defined(PICO_DEBUG)

#define PICODBG_INITIALIZE(level) \
    picodbg_initialize(level)

#define PICODBG_TERMINATE() \
    picodbg_terminate()

#define PICODBG_SET_LOG_LEVEL(level) \
    picodbg_setLogLevel(level)

#define PICODBG_SET_LOG_FILTERFN(name) \
    picodbg_setLogFilterFN(name)

#define PICODBG_SET_LOG_FILE(name) \
    picodbg_setLogFile(name)

#define PICODBG_ENABLE_COLORS(flag) \
    picodbg_enableColors(flag)

#define PICODBG_SET_OUTPUT_FORMAT(format) \
    picodbg_setOutputFormat(format)


#define PICODBG_ASSERT(expr) \
    for (;!(expr);picodbg_assert(__FILE__, __LINE__, __FUNCTION__, #expr))

#define PICODBG_ASSERT_RANGE(val, min, max) \
    PICODBG_ASSERT(((val) >= (min)) && ((val) <= (max)))


#define PICODBG_LOG(level, msg) \
    picodbg_log(level, 1,  __FILE__, __LINE__, __FUNCTION__, picodbg_varargs msg)

#define PICODBG_ERROR(msg) \
    PICODBG_LOG(PICODBG_LOG_LEVEL_ERROR, msg)

#define PICODBG_WARN(msg) \
    PICODBG_LOG(PICODBG_LOG_LEVEL_WARN, msg)

#define PICODBG_INFO(msg) \
    PICODBG_LOG(PICODBG_LOG_LEVEL_INFO, msg)

#define PICODBG_DEBUG(msg) \
    PICODBG_LOG(PICODBG_LOG_LEVEL_DEBUG, msg)

#define PICODBG_TRACE(msg) \
    PICODBG_LOG(PICODBG_LOG_LEVEL_TRACE, msg)


#define PICODBG_INFO_CTX() \
    picodbg_log(PICODBG_LOG_LEVEL_INFO, 0, __FILE__, __LINE__, __FUNCTION__, "")

#define PICODBG_INFO_MSG(msg) \
    picodbg_log_msg(PICODBG_LOG_LEVEL_INFO, __FILE__, picodbg_varargs msg)

#define PICODBG_INFO_MSG_F(filterfn, msg) \
    picodbg_log_msg(PICODBG_LOG_LEVEL_INFO, (const char *)filterfn, picodbg_varargs msg)



/* helper routines; should NOT be used directly! */

void picodbg_initialize(int level);
void picodbg_terminate();

void picodbg_setLogLevel(int level);
void picodbg_setLogFilterFN(const char *name);
void picodbg_setLogFile(const char *name);
void picodbg_enableColors(int flag);
void picodbg_setOutputFormat(unsigned int format);

const char *picodbg_varargs(const char *format, ...);

void picodbg_log(int level, int donewline, const char *file, int line,
                 const char *func, const char *msg);
void picodbg_assert(const char *file, int line, const char *func,
                    const char *expr);

void picodbg_log_msg(int level, const char *file, const char *msg);


#else  /* release version; omit debugging code */

#define PICODBG_INITIALIZE(level)
#define PICODBG_TERMINATE()
#define PICODBG_SET_LOG_LEVEL(level)
#define PICODBG_SET_LOG_FILTERFN(name)
#define PICODBG_SET_LOG_FILE(name)
#define PICODBG_ENABLE_COLORS(flag)
#define PICODBG_SET_OUTPUT_FORMAT(format)

#define PICODBG_ASSERT(expr)
#define PICODBG_ASSERT_RANGE(val, min, max)

#define PICODBG_LOG(level, msg)
#define PICODBG_ERROR(msg)
#define PICODBG_WARN(msg)
#define PICODBG_INFO(msg)
#define PICODBG_DEBUG(msg)
#define PICODBG_TRACE(msg)

#define PICODBG_INFO_CTX()
#define PICODBG_INFO_MSG(msg)
#define PICODBG_INFO_MSG_F(filterfn, msg)


#endif /* defined(PICO_DEBUG) */

#ifdef __cplusplus
}
#endif


#endif /* !defined(__PICODBG_H__) */
