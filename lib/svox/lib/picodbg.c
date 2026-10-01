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
 * @file picodbg.c
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

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


#if defined(PICO_DEBUG)

/* Two variants of colored console output are implemented:
   COLOR_MODE_WINDOWS
      uses the Windows API function SetConsoleTextAttribute
   COLOR_MODE_ANSI
      uses ANSI escape codes */
#if defined(_WIN32)
#define COLOR_MODE_WINDOWS
#else
#define COLOR_MODE_ANSI
#endif


#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>
#include <string.h>

#include "picodbg.h"


/* Maximum length of a formatted tracing message */
#define MAX_MESSAGE_LEN         999

/* Maximum length of contextual information */
#define MAX_CONTEXT_LEN         499

/* Maximum length of filename filter */
#define MAX_FILTERFN_LEN         16

/* Delimiter used in debug messages */
#define MSG_DELIM               "|"

/* Standard output file for debug messages */
#define STDDBG                  stdout /* or stderr */

/* Default setup */
#define PICODBG_DEFAULT_LEVEL   PICODBG_LOG_LEVEL_WARN
#define PICODBG_DEFAULT_FILTERFN   ""
#define PICODBG_DEFAULT_FORMAT  \
    (PICODBG_SHOW_LEVEL | PICODBG_SHOW_SRCNAME | PICODBG_SHOW_FUNCTION)
#define PICODBG_DEFAULT_COLOR   1


/* Current log level */
static int logLevel = PICODBG_DEFAULT_LEVEL;

/* Current log filter (filename) */
static char logFilterFN[MAX_FILTERFN_LEN + 1];

/* Current log file or NULL if no log file is set */
static FILE *logFile = NULL;

/* Current output format */
static int logFormat = PICODBG_DEFAULT_FORMAT;

/* Color mode for console output (0 : disable colors, != 0 : enable colors */
static int optColor = 0;

/* Buffer for context information */
static char ctxbuf[MAX_CONTEXT_LEN + 1];

/* Buffer to format tracing messages */
static char msgbuf[MAX_MESSAGE_LEN + 1];


/* *** Support for colored text output to console *****/


/* Console text colors */
enum color_t {
    /* order matches Windows color codes */
    ColorBlack,
    ColorBlue,
    ColorGreen,
    ColorCyan,
    ColorRed,
    ColorPurple,
    ColorBrown,
    ColorLightGray,
    ColorDarkGray,
    ColorLightBlue,
    ColorLightGreen,
    ColorLightCyan,
    ColorLightRed,
    ColorLightPurple,
    ColorYellow,
    ColorWhite
};


static enum color_t picodbg_getLevelColor(int level)
{
    switch (level) {
        case PICODBG_LOG_LEVEL_ERROR: return ColorLightRed;
        case PICODBG_LOG_LEVEL_WARN : return ColorYellow;
        case PICODBG_LOG_LEVEL_INFO : return ColorGreen;
        case PICODBG_LOG_LEVEL_DEBUG: return ColorLightGray;
        case PICODBG_LOG_LEVEL_TRACE: return ColorDarkGray;
    }
    return ColorWhite;
}


#if defined(COLOR_MODE_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static int picodbg_setTextAttr(FILE *stream, int attr)
{
    HANDLE hConsole;

    if (stream == stdout) {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    } else if (stream == stderr) {
        hConsole = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        hConsole = INVALID_HANDLE_VALUE;
    }

    if (hConsole != INVALID_HANDLE_VALUE) {
        /* do nothing if console output is redirected to a file */
        if (GetFileType(hConsole) == FILE_TYPE_CHAR) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            SetConsoleTextAttribute(hConsole, (WORD) attr);
            return (int) csbi.wAttributes;
        }
    }

    return 0;
}

#elif defined(COLOR_MODE_ANSI)

static int picodbg_setTextAttr(FILE *stream, int attr)
{
    const char *c = "";

    if (attr == -1) {
        c = "0";
    } else switch (attr) {
        case ColorBlack:       c = "0;30"; break;
        case ColorRed:         c = "0;31"; break;
        case ColorGreen:       c = "0;32"; break;
        case ColorBrown:       c = "0;33"; break;
        case ColorBlue:        c = "0;34"; break;
        case ColorPurple:      c = "0;35"; break;
        case ColorCyan:        c = "0;36"; break;
        case ColorLightGray:   c = "0;37"; break;
        case ColorDarkGray:    c = "1;30"; break;
        case ColorLightRed:    c = "1;31"; break;
        case ColorLightGreen:  c = "1;32"; break;
        case ColorYellow:      c = "1;33"; break;
        case ColorLightBlue:   c = "1;34"; break;
        case ColorLightPurple: c = "1;35"; break;
        case ColorLightCyan:   c = "1;36"; break;
        case ColorWhite:       c = "1;37"; break;
    }

    fprintf(stream, "\x1b[%sm", c);
    return -1;
}

#else

static int picodbg_setTextAttr(FILE *stream, int attr)
{
    /* avoid 'unreferenced formal parameter' */
    (void) stream;
    (void) attr;
    return 0;
}

#endif


/* *** Auxiliary routines *****/


static const char *picodbg_fileTitle(const char *file)
{
    const char *name = file, *str = file;

    /* try to extract file name without path in a platform independent
       way, i.e., skip all chars preceding path separator chars like
       '/' (Unix, MacOSX), '\' (Windows, DOS), and ':' (MacOS9) */
    while (*str) {
        if ((*str == '\\') || (*str == '/') || (*str == ':')) {
            name = str + 1;
        }
        str++;
    }

    return name;
}


static void picodbg_logToStream(int level, int donewline,
                                const char *context, const char *msg)
{
    int oldAttr = 0;

    if (optColor) {
        oldAttr = picodbg_setTextAttr(STDDBG, picodbg_getLevelColor(level));
    }

    fprintf(STDDBG, "%s%s", context, msg);
    if (donewline) fprintf(STDDBG, "\n");
    if (logFile != NULL) {
        fprintf(logFile, "%s%s", context, msg);
        if (donewline) fprintf(logFile, "\n");
    }

    if (optColor) {
        picodbg_setTextAttr(STDDBG, oldAttr);
    }
}


/* *** Exported routines *****/


void picodbg_initialize(int level)
{
    logLevel  = level;
    strcpy(logFilterFN, PICODBG_DEFAULT_FILTERFN);
    logFile   = NULL;
    logFormat = PICODBG_DEFAULT_FORMAT;
    optColor  = PICODBG_DEFAULT_COLOR;
    PICODBG_ASSERT_RANGE(level, 0, PICODBG_LOG_LEVEL_TRACE);
}


void picodbg_terminate()
{
    if (logFile != NULL) {
        fclose(logFile);
    }

    logLevel = 0;
    logFile  = NULL;
}


void picodbg_setLogLevel(int level)
{
    PICODBG_ASSERT_RANGE(level, 0, PICODBG_LOG_LEVEL_TRACE);
    logLevel = level;
}


void picodbg_setLogFilterFN(const char *name)
{
    strcpy(logFilterFN, name);
}


void picodbg_setLogFile(const char *name)
{
    if (logFile != NULL) {
        fclose(logFile);
    }

    if ((name != NULL) && (strlen(name) > 0)) {
        logFile = fopen(name, "wt");
    } else {
        logFile = NULL;
    }
}


void picodbg_enableColors(int flag)
{
    optColor = (flag != 0);
}


void picodbg_setOutputFormat(unsigned int format)
{
    logFormat = format;
}


const char *picodbg_varargs(const char *format, ...)
{
    int len;

    va_list argptr;
    va_start(argptr, format);

    len = vsprintf(msgbuf, format, argptr);
    PICODBG_ASSERT_RANGE(len, 0, MAX_MESSAGE_LEN);

    return msgbuf;
}


void picodbg_log(int level, int donewline, const char *file, int line,
                 const char *func, const char *msg)
{
    char cb[MAX_CONTEXT_LEN + 1];

    PICODBG_ASSERT_RANGE(level, 0, PICODBG_LOG_LEVEL_TRACE);

    if ((level <= logLevel) &&
        ((strlen(logFilterFN) == 0) || !strcmp(logFilterFN, picodbg_fileTitle(file)))) {
        /* compose output format string */
        strcpy(ctxbuf, "*** ");
        if (logFormat & PICODBG_SHOW_LEVEL) {
            switch (level) {
                case PICODBG_LOG_LEVEL_ERROR:
                    strcat(ctxbuf, "error" MSG_DELIM);
                    break;
                case PICODBG_LOG_LEVEL_WARN:
                    strcat(ctxbuf, "warn " MSG_DELIM);
                    break;
                case PICODBG_LOG_LEVEL_INFO:
                    strcat(ctxbuf, "info " MSG_DELIM);
                    break;
                case PICODBG_LOG_LEVEL_DEBUG:
                    strcat(ctxbuf, "debug" MSG_DELIM);
                    break;
                case PICODBG_LOG_LEVEL_TRACE:
                    strcat(ctxbuf, "trace" MSG_DELIM);
                    break;
                default:
                    break;
            }
        }
        if (logFormat & PICODBG_SHOW_DATE) {
            /* nyi */
        }
        if (logFormat & PICODBG_SHOW_TIME) {
            /* nyi */
        }
        if (logFormat & PICODBG_SHOW_SRCNAME) {
            sprintf(cb, "%-10s", picodbg_fileTitle(file));
            strcat(ctxbuf, cb);
            if (logFormat & PICODBG_SHOW_SRCLINE) {
                sprintf(cb, "(%d)", line);
                strcat(ctxbuf, cb);
            }
            strcat(ctxbuf, MSG_DELIM);
        }
        if (logFormat & PICODBG_SHOW_FUNCTION) {
            if (strlen(func) > 0) {
                sprintf(cb, "%-18s", func);
                strcat(ctxbuf, cb);
                strcat(ctxbuf, MSG_DELIM);
            }
        }

        picodbg_logToStream(level, donewline, ctxbuf, msg);
    }
}


void picodbg_log_msg(int level, const char *file, const char *msg)
{
    PICODBG_ASSERT_RANGE(level, 0, PICODBG_LOG_LEVEL_TRACE);

    if ((level <= logLevel) &&
        ((strlen(logFilterFN) == 0) || !strcmp(logFilterFN, picodbg_fileTitle(file)))) {
        picodbg_logToStream(level, 0, "", msg);
    }
}


void picodbg_assert(const char *file, int line, const char *func, const char *expr)
{
    if (strlen(func) > 0) {
        fprintf(STDDBG, "assertion failed: %s, file %s, function %s, line %d",
            expr, picodbg_fileTitle(file), func, line);
    } else {
        fprintf(STDDBG, "assertion failed: %s, file %s, line %d",
            expr, picodbg_fileTitle(file), line);
    }
    picodbg_terminate();
    abort();
}


#else

/* To prevent warning about "translation unit is empty" when
   diagnostic output is disabled. */
static void picodbg_dummy(void) {
    picodbg_dummy();
}

#endif /* defined(PICO_DEBUG) */

#ifdef __cplusplus
}
#endif


/* end */
