/***************************************************************************
 *
 * time_put.cpp
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 2001-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>

// disable Compaq C++ pure C headers
#undef __PURE_CNAME

#if (defined (__sun__) || defined (__sun) || defined (sun)) && defined (__EDG__)
    // get tzset() from <time.h>
#   define _XOPEN_SOURCE
#endif

#ifdef __CYGWIN__
   // force CygWin <time.h> to define timezone as an int variable,
   // not as a void function returning char*
#  define timezonevar
#endif   // __CYGWIN__


#include <ctype.h>    // for isspace(), ...
#include <stdio.h>    // for sprintf()
#include <stddef.h>   // for offsetof()
#include <stdlib.h>   // for getenv()
#include <string.h>   // for memcpy(), strlen()
#include <time.h>     // for strftime(), struct tm, tzset()
#include <wchar.h>    // for wcsftime()

#ifndef _MSC_VER

#  ifndef _RWSTD_NO_PURE_C_HEADERS
#    include <locale.h>
#    ifndef LC_MESSAGES
#      define LC_MESSAGES _RWSTD_LC_MESSAGES
#    endif   // LC_MESSAGES
#  endif   // _RWSTD_NO_PURE_C_HEADERS

#  ifndef _RWSTD_NO_NL_LANGINFO
#    include <langinfo.h>             // for nl_langinfo()
#  endif
#else   // if defined (_MSC_VER)
#  if defined (_RWSTD_MSVC) && defined (_WIN64)
     // shut up MSVC/Win64 complaints about possible loss of data
#    pragma warning (disable: 4244)
#  endif
#endif   // _MSC_VER

#include "access.h"

#include <loc/_ctype.h>
#include <loc/_localedef.h>
#include <loc/_punct.h>
#include <loc/_time_put.h>

#include <rw/_traits.h>

#include "setlocale.h"
#include "strtol.h"


#if defined (__EDG__) || !defined (_RWSTD_NO_PURE_C_HEADERS)
#  if defined (__linux__) || defined (__sun)

extern "C" {

// declare these for Linux glibc and SunOS
extern int daylight;
extern long int timezone;
extern void tzset () _LIBC_THROWS ();

}   // extern "C"

#  endif   // __linux__ || __sun
#endif   // __EDG__ || !_RWSTD_NO_PURE_C_HEADERS


#ifdef _RWSTD_NO_DAYLIGHT
   // the XSI POSIX extension, daylight, is not declared in <time.h>
#  define daylight   0
#endif   // _RWSTD_NO_DAYLIGHT


#if defined (_RWSTD_NO_WCSFTIME) && !defined (_RWSTD_NO_WCSFTIME_IN_LIBC)
#  if defined _RWSTD_WCSFTIME_ARG3_T

#    undef _RWSTD_NO_WCSFTIME

extern "C" {

size_t wcsftime (wchar_t*, size_t, _RWSTD_WCSFTIME_ARG3_T, const struct tm*);

}
#  endif   // _RWSTD_WCSFTIME_ARG3_T
#endif   // _RWSTD_NO_WCSFTIME && !_RWSTD_NO_WCSFTIME_IN_LIBC


_RWSTD_NAMESPACE (__rw) {


typedef _STD::ctype_base::mask MaskT;
extern const MaskT __rw_classic_tab [];

#define ISALPHA(c) \
    (_RW::__rw_classic_tab [(unsigned char)c] & _RW::__rw_alpha)

#define ISDIGIT(c) \
    (_RW::__rw_classic_tab [(unsigned char)c] & _RW::__rw_digit)


_RWSTD_SIZE_T
__rw_get_timepunct (const _RW::__rw_facet*, int,
                    const void*[], const _RWSTD_SIZE_T []);

// also declared in _time_put.cc
_RWSTD_EXPORT char*
__rw_put_time (const __rw_facet*, char*, _RWSTD_SIZE_T,
               _STD::ios_base&, char, const tm*,
               char, char, int, int);


#ifndef _RWSTD_NO_WCHAR_T

// also declared in _time_put.cc
_RWSTD_EXPORT wchar_t*
__rw_put_time (const __rw_facet*, wchar_t*, _RWSTD_SIZE_T,
               _STD::ios_base&, wchar_t, const tm*,
               char, char, int, int);

#endif   // _RWSTD_NO_WCHAR_T


/***************************************************************************/


#ifdef _RWSTD_NO_NL_LANGINFO

typedef unsigned char UChar;

// compute the format string corresponding to the "%x" format specifier
// in the current locale (set by setlocale (LC_ALL, ...))
static _RWSTD_SIZE_T
__rw_get_date_fmat (char *fmt)
{
    tm t;

    memset (&t, 0, sizeof t);

    // set a date consisting of unique components
    // such as 2/1/1933
    t.tm_mday = 1; t.tm_mon = 1; t.tm_year = 33;

    char tmp [256];

    tmp [0] = '\0';

    // shut up gcc 2.9x warning: `%x' yields only last 2 digits of year
    const char percent_x[] = "%x";
    strftime (tmp, sizeof tmp, percent_x, &t);

    char *pfmt = fmt;

    char abday [256];   // buffer for abbreviated day name
    char day [256];     // buffer for full day name
    char abmon [256];   // buffer for abbreviated month name
    char mon [256];     // buffer for full month name

    _RWSTD_SIZE_T abday_len = 0;
    _RWSTD_SIZE_T day_len;
    _RWSTD_SIZE_T abmon_len;
    _RWSTD_SIZE_T mon_len;

    for (char *ptmp = tmp; *ptmp; ) {

        // store all whitespace as part of format
        for (; (isspace)(UChar (*ptmp)); ++ptmp)
            *pfmt++ = *ptmp;

        const char *begin = ptmp;

        // skip over all non-digit characters
        for (; *ptmp && !(isdigit)(UChar (*ptmp)); ++ptmp) {
            if ((ispunct)(UChar (*ptmp)) || (isspace)(UChar (*ptmp))) {
                // store all punctuators as part of format
                for ( ; (ispunct)(UChar (*ptmp)) || (isspace)(UChar (*ptmp)); ++ptmp)
                    *pfmt++ = *ptmp;
                break;
            }
            else
                *pfmt++ = *ptmp;
        }

        const _RWSTD_SIZE_T len = ptmp - begin;

        if (len > 1) {

            // if there are 2 or more characters in this date component
            // try to match the sequence against one of the names

            if (!abday_len) {
                // initialize day and month names only when necessary
                abday_len = strftime (abday, sizeof abday, "%a", &t);
                day_len   = strftime (day,   sizeof day,   "%A", &t);
                abmon_len = strftime (abmon, sizeof abmon, "%b", &t);
                mon_len   = strftime (mon,   sizeof mon,   "%B", &t);
            }

            char fmtchar = '\0';
            _RWSTD_SIZE_T start = 0;

            if (day_len <= len && !memcmp (day, begin, day_len)) {
                fmtchar = 'A';
                start   = day_len;
            }
            else if (mon_len <= len && !memcmp (mon, begin, mon_len)) {
                fmtchar = 'B';
                start   = mon_len;
            }
            else if (abday_len <= len && !memcmp (abday, begin, abday_len)) {
                fmtchar = 'a';
                start   = abday_len;
            }
            else if (abmon_len <= len && !memcmp (abmon, begin, abmon_len)) {
                fmtchar = 'b';
                start   = abmon_len;
            }

            if (fmtchar) {
                pfmt   -= len;
                *pfmt++ = '%';
                *pfmt++ = fmtchar;
                for (; start != len; ++start)
                    *pfmt++ = begin [start];
            }
        }

        if ((isdigit)(UChar (*ptmp))) {

            for (begin = ptmp; (isdigit)(UChar (*ptmp)); ++ptmp);

            *pfmt++ = '%';
            if (ptmp - begin == 1) {
                if ('1' == *begin) {
                    if (pfmt != fmt + 1 && ' ' == pfmt [-2]) {
                        pfmt [-2] = '%';
                        --pfmt;
                    }
                    *pfmt++ = 'e';
                }
                else if ('2' == *begin) {
                    *pfmt++ = 'm';
                }
            }
            else if (ptmp - begin == 2) {
                if ('0' == begin [0] && '1' == begin [1])
                    *pfmt++ = 'd';
                else if ('0' == begin [0] && '2' == begin [1])
                    *pfmt++ = 'm';
                else if ('3' == begin [0] && '3' == begin [1])
                    *pfmt++ = 'y';
                else
                    *pfmt++ = '*';
            }
            else if (ptmp - begin == 4 && !memcmp (begin, "1933", 4))
                *pfmt++ = 'Y';
            else
                *pfmt++ = '*';
        }
    }

    *pfmt++ = '\0';

    return pfmt - fmt;
}


// compute the format string corresponding to the "%X" format specifier
// in the current locale (set by setlocale (LC_ALL, ...))
static _RWSTD_SIZE_T
__rw_get_time_fmat (char *fmt)
{
    tm t;

    memset (&t, 0, sizeof t);

    t.tm_sec = 3; t.tm_min = 4; t.tm_hour = 21;   // "9 PM" or "21"

    char tmp [256];

    tmp [0] = '\0';
    strftime (tmp, sizeof tmp, "%X", &t);

    char *pfmt = fmt;

    char pm [256];   // buffer for pm designator

    _RWSTD_SIZE_T pm_len = 0;

    for (char *ptmp = tmp; *ptmp; ) {

        for (; (isspace)(UChar (*ptmp)); ++ptmp)
            *pfmt++ = *ptmp;

        const char *begin = ptmp;

        for (; *ptmp && !(isdigit)(UChar (*ptmp)); ++ptmp) {
            if (   (ispunct)(UChar (*ptmp))
                || (isspace)(UChar (*ptmp))) {
                for (;    (ispunct)(UChar (*ptmp))
                       || (isspace)(UChar (*ptmp)); ++ptmp)
                    *pfmt++ = *ptmp;
                break;
            }
            else
                *pfmt++ = *ptmp;
        }

        const _RWSTD_SIZE_T len = ptmp - begin;

        if (len > 1) {
            if (0 == pm_len) {
                strftime (pm, sizeof pm, "%p", &t);
                pm_len = strlen (pm);
            }

            if (   pm_len <= len
                && !::memcmp (pm, begin, pm_len)) {
                pfmt   -= len;
                *pfmt++ = '%';
                *pfmt++ = 'p';

                // copy whatever follows the pm designator
                for (_RWSTD_SIZE_T i = pm_len; i != len; ++i)
                    *pfmt++ = begin [i];
            }
        }

        if ((isdigit)(UChar (*ptmp))) {

            for (begin = ptmp; (isdigit)(UChar (*ptmp)); ++ptmp);

            *pfmt++ = '%';

            if (pfmt != fmt + 1 && ' ' == pfmt [-2]) {
                pfmt [-2] = '%';
                --pfmt;
            }

            if (ptmp - begin == 1) {

                switch (*begin) {
                case '3': *pfmt++ = 'S'; break;   // tm_sec
                case '4': *pfmt++ = 'M'; break;   // tm_min
                case '9': *pfmt++ = 'I'; break;   // tm_hour
                default: *pfmt++ = '\0';
                }
            }
            else if (ptmp - begin == 2) {

                if ('0' == begin [0]) {
                    switch (begin [1]) {
                    case '3': *pfmt++ = 'S'; break;   // tm_sec
                    case '4': *pfmt++ = 'M'; break;   // tm_min
                    case '9': *pfmt++ = 'I'; break;   // tm_hour
                    default: *pfmt++ = '\0';
                    }
                }
                else if ('2' == begin [0] && '1' == begin [1]) {
                    *pfmt++ = 'H';
                }
                else
                    *pfmt++ = '\0';
            }
            else
                *pfmt++ = '\0';
        }
    }

    *pfmt++ = '\0';

    return pfmt - fmt;
}


#endif   // _RWSTD_NO_NL_LANGINFO


const void*
__rw_get_timepunct (const __rw_facet *pfacet, int flags, _RWSTD_SIZE_T inx)
{
    const int  member = flags & ~__rw_wide;
    const bool wide   = !!(flags & __rw_wide);

    const void *pdata = pfacet->_C_data ();

    if (pdata) {

        const __rw_time_t* const pun =
            _RWSTD_STATIC_CAST (const __rw_time_t*, pdata);

        switch (member) {
        case __rw_abday: return pun->abday (inx, wide);
        case __rw_day: return pun->day (inx, wide);
        case __rw_abmon: return pun->abmon (inx, wide);
        case __rw_month: return pun->mon (inx, wide);
        case __rw_ampm: return pun->am_pm (inx, wide);
        case __rw_d_t_fmt: return pun->d_t_fmt (wide);
        case __rw_d_fmt: return pun->d_fmt (wide);
        case __rw_t_fmt: return pun->t_fmt (wide);
        case __rw_t_fmt_ampm: return pun->t_fmt_ampm (wide);
        case __rw_era_d_t_fmt: return pun->era_d_t_fmt (wide);
        case __rw_era_d_fmt: return pun->era_d_fmt (wide);
        case __rw_era_t_fmt: return pun->era_t_fmt (wide);
        case __rw_era_names: return pun->era_name (inx, wide);
        case __rw_era_fmts: return pun->era_fmt (inx, wide);
        case __rw_alt_digits: return pun->alt_digits (inx, wide);

        default: return pdata;
        }
    }

    const char* const locname = pfacet->_C_get_name ();

    // set all categories (need LC_TIME and LC_CTYPE) and lock
    const __rw_setlocale clocale (locname, _RWSTD_LC_ALL);

    _RWSTD_SIZE_T bufsize = 2048;

    const size_t newsize = bufsize + sizeof (__rw_time_t);
 
    // allocate memory using operator new to avoid mismatch with
    // facet destructor
    char *pbuf = _RWSTD_STATIC_CAST (char*, ::operator new (newsize));

    __rw_time_t *pun = _RWSTD_REINTERPRET_CAST (__rw_time_t*, pbuf);

    // zero out all members
    memset (pun, 0, sizeof *pun);

    // advance buffer pointer past the end of the structure
    pbuf += sizeof *pun;

#ifndef _RWSTD_NO_WCHAR_T

    // reserve offset 0 for the empty string (both narrow and wide)
    *_RWSTD_REINTERPRET_CAST (wchar_t*, pbuf) = L'\0';

    _RWSTD_SIZE_T off = sizeof (wchar_t);

#else   // if defined (_RWSTD_NO_WCHAR_T)

    // reserve offset 0 for the empty string
    *pbuf = '\0';

    _RWSTD_SIZE_T off = sizeof (char);

#endif   // _RWSTD_NO_WCHAR_T

#ifndef _RWSTD_NO_NL_LANGINFO

    static const struct {
        int           nl_item;    // argument to nl_langinfo()
        _RWSTD_SIZE_T moff [2];   // member offset
    } nl_items [] = {


#ifndef _RWSTD_NO_OFFSETOF
#  define OFF(T, m)   offsetof (T, m)
#else
#  define OFF(T, m)   ((char*)&((T*)0)->m - (char*)0)
#endif   // _RWSTD_NO_OFFSETOF


#undef ENTRY
#define ENTRY(item, mem1, mem2)   {                                      \
    item,                                                                \
    { OFF (__rw_time_t, __rw_time_t::mem1) / sizeof (_RWSTD_UINT32_T),   \
      OFF (__rw_time_t, __rw_time_t::mem2) / sizeof (_RWSTD_UINT32_T) }  \
  }

        // each entry contains an `nl_item' value followed by the offset
        // of __rw_time_t member; offsets rather than addresses are used
        // to allow the __rw_time_t struct to be reallocated w/o having
        // to reinitialize the array; this also allows the array to be
        // static const for further speed improvement
        ENTRY (D_T_FMT, d_t_fmt_off [0], d_t_fmt_off [1]),
        ENTRY (D_FMT,   d_fmt_off   [0], d_fmt_off   [1]),
        ENTRY (T_FMT,   t_fmt_off   [0], t_fmt_off   [1]),

#ifdef T_FMT_AMPM
        ENTRY (T_FMT_AMPM, t_fmt_ampm_off [0], t_fmt_ampm_off [1]),
#else
        ENTRY (D_T_FMT,    t_fmt_ampm_off [0], t_fmt_ampm_off [1]),
#endif   // T_FMT_AMPM

        ENTRY (AM_STR, am_pm_off [0][0], am_pm_off [1][0]),
        ENTRY (PM_STR, am_pm_off [0][1], am_pm_off [1][1]),

        ENTRY (ABDAY_1, abday_off [0][0], abday_off [1][0]),
        ENTRY (ABDAY_2, abday_off [0][1], abday_off [1][1]),
        ENTRY (ABDAY_3, abday_off [0][2], abday_off [1][2]),
        ENTRY (ABDAY_4, abday_off [0][3], abday_off [1][3]),
        ENTRY (ABDAY_5, abday_off [0][4], abday_off [1][4]),
        ENTRY (ABDAY_6, abday_off [0][5], abday_off [1][5]),
        ENTRY (ABDAY_7, abday_off [0][6], abday_off [1][6]),

        ENTRY (DAY_1, day_off [0][0], day_off [1][0]),
        ENTRY (DAY_2, day_off [0][1], day_off [1][1]),
        ENTRY (DAY_3, day_off [0][2], day_off [1][2]),
        ENTRY (DAY_4, day_off [0][3], day_off [1][3]),
        ENTRY (DAY_5, day_off [0][4], day_off [1][4]),
        ENTRY (DAY_6, day_off [0][5], day_off [1][5]),
        ENTRY (DAY_7, day_off [0][6], day_off [1][6]),

        ENTRY (ABMON_1,  abmon_off [0][ 0], abmon_off [1][ 0]),
        ENTRY (ABMON_2,  abmon_off [0][ 1], abmon_off [1][ 1]),
        ENTRY (ABMON_3,  abmon_off [0][ 2], abmon_off [1][ 2]),
        ENTRY (ABMON_4,  abmon_off [0][ 3], abmon_off [1][ 3]),
        ENTRY (ABMON_5,  abmon_off [0][ 4], abmon_off [1][ 4]),
        ENTRY (ABMON_6,  abmon_off [0][ 5], abmon_off [1][ 5]),
        ENTRY (ABMON_7,  abmon_off [0][ 6], abmon_off [1][ 6]),
        ENTRY (ABMON_8,  abmon_off [0][ 7], abmon_off [1][ 7]),
        ENTRY (ABMON_9,  abmon_off [0][ 8], abmon_off [1][ 8]),
        ENTRY (ABMON_10, abmon_off [0][ 9], abmon_off [1][ 9]),
        ENTRY (ABMON_11, abmon_off [0][10], abmon_off [1][10]),
        ENTRY (ABMON_12, abmon_off [0][11], abmon_off [1][11]),

        ENTRY (MON_1,  mon_off [0][ 0], mon_off [1][ 0]),
        ENTRY (MON_2,  mon_off [0][ 1], mon_off [1][ 1]),
        ENTRY (MON_3,  mon_off [0][ 2], mon_off [1][ 2]),
        ENTRY (MON_4,  mon_off [0][ 3], mon_off [1][ 3]),
        ENTRY (MON_5,  mon_off [0][ 4], mon_off [1][ 4]),
        ENTRY (MON_6,  mon_off [0][ 5], mon_off [1][ 5]),
        ENTRY (MON_7,  mon_off [0][ 6], mon_off [1][ 6]),
        ENTRY (MON_8,  mon_off [0][ 7], mon_off [1][ 7]),
        ENTRY (MON_9,  mon_off [0][ 8], mon_off [1][ 8]),
        ENTRY (MON_10, mon_off [0][ 9], mon_off [1][ 9]),
        ENTRY (MON_11, mon_off [0][10], mon_off [1][10]),
        ENTRY (MON_12, mon_off [0][11], mon_off [1][11]),

#ifdef ERA_D_T_FMT
        ENTRY (ERA_D_T_FMT, era_d_t_fmt_off [0], era_d_t_fmt_off [1]),
#else
        ENTRY (D_T_FMT,     era_d_t_fmt_off [0], era_d_t_fmt_off [1]),
#endif   // ERA_D_T_FMT

#ifdef ERA_D_FMT
        ENTRY (ERA_D_FMT, era_d_fmt_off [0], era_d_fmt_off [1]),
#else
        ENTRY (D_T_FMT,   era_d_fmt_off [0], era_d_fmt_off [1]),
#endif   // ERA_D_FMT

#ifdef ERA_T_FMT
        ENTRY (ERA_T_FMT, era_t_fmt_off [0], era_t_fmt_off [1])
#else
        ENTRY (D_T_FMT,   era_t_fmt_off [0], era_t_fmt_off [1])
#endif   // ERA_T_FMT

    };

    // points to the first __rw_time_t data member
    _RWSTD_UINT32_T *pmem = _RWSTD_REINTERPRET_CAST (_RWSTD_UINT32_T*, pun);

    const bool c_locale = 'C' == locname [0] && '\0' == locname [1];

    for (_RWSTD_SIZE_T i = 0; i != sizeof nl_items / sizeof *nl_items; ++i) {

        if (D_T_FMT == nl_items [i].nl_item && i)
            continue;

        // enforce POSIX rules for "%c" in the "C" locale
        const char* const str = i || !c_locale ?
            nl_langinfo (nl_items [i].nl_item) : "%a %b %e %H:%M:%S %Y";

        _RWSTD_ASSERT (0 != str);

        _RWSTD_SIZE_T size = 1 + strlen (str);

        // estimate the size required to accommodate both
        // the narrow and the wide char representations
        const _RWSTD_SIZE_T estsize = size * (1 + sizeof (wchar_t));

        if (off + estsize >= bufsize) {
            // reallocate
            const size_t tmpsize = sizeof *pun + bufsize * 2 + estsize;

            // reallocate, again using operator new to avoid mismatch
            // with facet destructor
            char* const tmp = 
                _RWSTD_STATIC_CAST(char*, ::operator new (tmpsize));
            memcpy (tmp, pun, sizeof *pun + off);

            ::operator delete (pun);

            pun      = _RWSTD_REINTERPRET_CAST (__rw_time_t*, tmp);
            pmem     = _RWSTD_REINTERPRET_CAST (_RWSTD_UINT32_T*, pun);
            pbuf     = tmp + sizeof *pun;
            bufsize  = bufsize * 2 + estsize;
        }

        // copy string to the allocated buffer and set its offset
        memcpy (pbuf + off, str, size);
        pmem [nl_items [i].moff [0]] =
            _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off += size;

#ifndef _RWSTD_NO_WCHAR_T

        // make sure wide string is properly aligned
        const _RWSTD_SIZE_T align = off % sizeof (wchar_t);
        if (align)
            off += sizeof (wchar_t) - align;

        // widen the narrow (multibyte) string into the allocated buffer
        // (at an appropriately aligned offset) and set its offset
        wchar_t* const pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
        size = mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

        if (_RWSTD_SIZE_MAX == size) {
            // conversion failure - should not happen
            *pwbuf = L'\0';
            size   = 1;
        }
        else {
            _RWSTD_ASSERT (L'\0' == pwbuf [size]);
            size += 1;
        }

        pmem [nl_items [i].moff [1]] =
            _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off += size * sizeof (wchar_t);

#endif   // _RWSTD_NO_WCHAR_T

    }

#else   // if defined (_RWSTD_NO_NL_LANGINFO)

    tm t;

    memset (&t, 0, sizeof t);

    _RWSTD_SIZE_T len;


#if 0   // FIXME: implement same way as above

    static const struct {
        int tm::      *pmem;
        _RWSTD_SIZE_T  moff;
        const char     fmt [3];
        int            begin;
        int            end;
        int            step;
    } fmats [] = {
        { &tm::tm_wday, "%a", 0,  7,  1 },
        { &tm::tm_wday, "%A", 0,  7,  1 },
        { &tm::tm_mon,  "%b", 0, 12,  1 },
        { &tm::tm_mon,  "%B", 0, 12,  1 },
        { &tm::tm_hour, "%p", 1, 13, 12 }
    };

    for (_RWSTD_SIZE_T i = 0; i != sizeof fmats / sizef *fmats; ++i) {

        for (int j = fmats [i].begin; j != fmats [i].end; j != fmats [i].step) {

            t.*fmats [i].pmem = j;

            len = strftime (pbuf + off, bufsize - off, fmats [i].fmt, &t);

            pun->abday_off [0][t.tm_wday]  = off;
            off                           += len + 1;
        }
    }

#endif   // 0/1


    // copy abbreaviated and full names of days
    for (t.tm_wday = 0; t.tm_wday != 7; ++t.tm_wday) {

        len = strftime (pbuf + off, bufsize - off, "%a", &t);

        pun->abday_off [0][t.tm_wday]  =
            _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off                           += len + 1;

        len = strftime (pbuf + off, bufsize - off, "%A", &t);

        pun->day_off [0][t.tm_wday]  =
            _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off                         += len + 1;

#  ifndef _RWSTD_NO_WCHAR_T

        // make sure wide strings are properly aligned
        const _RWSTD_SIZE_T align = off % sizeof (wchar_t);
        if (align)
            off += sizeof (wchar_t) - align;

#    ifndef _RWSTD_NO_WCSFTIME

        wchar_t *pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
        len = wcsftime (pwbuf, (bufsize - off) / sizeof (*pwbuf), L"%a", &t);

        pun->abday_off [1][t.tm_wday]  =
            _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off                           += (len + 1) * sizeof (wchar_t);

        pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
        len   = wcsftime (pwbuf, (bufsize - off) / sizeof (*pwbuf), L"%A", &t);

        pun->day_off [1][t.tm_wday] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off                         += (len + 1) * sizeof (wchar_t);

#    else   // if defined (_RWSTD_NO_WCSFTIME)

        // widen the narrow (multibyte) string into the allocated buffer
        // (at an appropriately aligned offset) and set its offset
        const char *str =
            _RWSTD_STATIC_CAST (const char*, pun->abday (t.tm_wday, 0));

        wchar_t *pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
        _RWSTD_SIZE_T size =
            mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

        if (_RWSTD_SIZE_MAX == size) {
            // conversion failure - should not happen
            *pwbuf = L'\0';
            size   = 1;
        }
        else {
            _RWSTD_ASSERT (L'\0' == pwbuf [size]);
            size += 1;
        }

        pun->abday_off [1][t.tm_wday] = off;
        off                          += size * sizeof (wchar_t);

        str   = _RWSTD_STATIC_CAST (const char*, pun->day (t.tm_wday, 0));
        pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
        size  = mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

        if (_RWSTD_SIZE_MAX == size) {
            // conversion failure - should not happen
            *pwbuf = L'\0';
            size   = 1;
        }
        else {
            _RWSTD_ASSERT (L'\0' == pwbuf [size]);
            size += 1;
        }

        pun->day_off [1][t.tm_wday] = off;
        off                        += size * sizeof (wchar_t);

#    endif   // _RWSTD_NO_WCSFTIME
#  endif   // _RWSTD_NO_WCHAR_T

    }

    // copy abbreaviated and full names of months
    for (t.tm_mon = 0; t.tm_mon != 12; ++t.tm_mon) {

        len = strftime (pbuf + off, bufsize - off, "%b", &t);

        pun->abmon_off [0][t.tm_mon]  = 
            _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off                          += len + 1;

        len = strftime (pbuf + off, bufsize - off, "%B", &t);

        pun->mon_off [0][t.tm_mon]  = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off                        += len + 1;

#  ifndef _RWSTD_NO_WCHAR_T

        // make sure wide strings are properly aligned
        const _RWSTD_SIZE_T align = off % sizeof (wchar_t);
        if (align)
            off += sizeof (wchar_t) - align;

#    ifndef _RWSTD_NO_WCSFTIME

        wchar_t *pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
        len = wcsftime (pwbuf, (bufsize - off) / sizeof (*pwbuf), L"%b", &t);

        pun->abmon_off [1][t.tm_mon]  =
            _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off                          += (len + 1) * sizeof (wchar_t);

        pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
        len   = wcsftime (pwbuf, (bufsize - off) / sizeof (*pwbuf), L"%B", &t);

        pun->mon_off [1][t.tm_mon]  = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
        off                        += (len + 1) * sizeof (wchar_t);

#    else   // if defined (_RWSTD_NO_WCSFTIME)

        // widen the narrow (multibyte) string into the allocated buffer
        // (at an appropriately aligned offset) and set its offset
        const char *str =
            _RWSTD_STATIC_CAST (const char*, pun->abmon (t.tm_mon, 0));
        wchar_t *pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
        _RWSTD_SIZE_T size =
            mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

        if (_RWSTD_SIZE_MAX == size) {
            // conversion failure - should not happen
            *pwbuf = L'\0';
            size   = 1;
        }
        else {
            _RWSTD_ASSERT (L'\0' == pwbuf [size]);
            size += 1;
        }

        pun->abmon_off [1][t.tm_mon] = off;
        off                         += size * sizeof (wchar_t);

        str   = _RWSTD_STATIC_CAST (const char*, pun->mon (t.tm_mon, 0));
        pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
        size  = mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

        if (_RWSTD_SIZE_MAX == size) {
            // conversion failure - should not happen
            *pwbuf = L'\0';
            size   = 1;
        }
        else {
            _RWSTD_ASSERT (L'\0' == pwbuf [size]);
            size += 1;
        }

        pun->mon_off [1][t.tm_mon] = off;
        off                       += size * sizeof (wchar_t);

#    endif   // _RWSTD_NO_WCSFTIME
#  endif   // _RWSTD_NO_WCHAR_T

    }

    // copy AM/PM designations
    t.tm_hour = 1;
    len = strftime (pbuf + off, bufsize - off, "%p", &t);
    pun->am_pm_off [0][0] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off += len + 1;
    
    t.tm_hour = 13;
    len = strftime (pbuf + off, bufsize - off, "%p", &t);
    pun->am_pm_off [0][1] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off += len + 1;

    // determine the locale's "%x" format (date representation)
    len                 = __rw_get_date_fmat (pbuf + off);
    pun->d_fmt_off [0]  = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off                += len + 1;
    
    // determine the locale's "%X" format (time representation)
    len                 = __rw_get_time_fmat (pbuf + off);
    pun->t_fmt_off [0]  = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off                += len + 1;

    // FIXME: determine "%r" at runtime (just like "%x" and "%X")
    // or have time_put use strftime() instead of hardcoding
    // the default "POSIX" format

    static const char t_fmt_ampm_fmat[] = "%I:%M:%S %p";

    len = sizeof t_fmt_ampm_fmat;
    memcpy (pbuf + off, t_fmt_ampm_fmat, len);
    pun->t_fmt_ampm_off [0]  = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off                     += len;

    // FIXME: determine "%c" at runtime (just like "%x" and "%X"),
    // or have time_put use strftime() instead of hardcoding
    // the default "C" format

    static const char d_t_fmat[] = "%a %b %e %H:%M:%S %Y";
    len                  = sizeof d_t_fmat;
    memcpy (pbuf + off, d_t_fmat, len);
    pun->d_t_fmt_off [0]  = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off                  += len;

    // FIXME: determine "%Ec", "%EX", "%Ex"
    pbuf [off]               = '\0';
    pun->era_d_t_fmt_off [0] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    pun->era_d_fmt_off [0]   = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    pun->era_t_fmt_off [0]   = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off                     += 1;

#  ifndef _RWSTD_NO_WCHAR_T

    const _RWSTD_SIZE_T align = off % sizeof (wchar_t);
    if (align)
        off += sizeof (wchar_t) - align;

    const char    *str;
    wchar_t       *pwbuf;
    _RWSTD_SIZE_T  size;

#    ifndef _RWSTD_NO_WCSFTIME

    t.tm_hour = 1;
    pwbuf     = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
    len       = wcsftime (pwbuf, (bufsize - off) / sizeof (*pwbuf), L"%p", &t);
    pun->am_pm_off [1][0] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off += (len + 1) * sizeof (wchar_t);
    
    t.tm_hour = 13;
    pwbuf     = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
    len       = wcsftime (pwbuf, (bufsize - off) / sizeof (*pwbuf), L"%p", &t);
    pun->am_pm_off [1][1] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off += (len + 1) * sizeof (wchar_t);

#    else   // if defined (_RWSTD_NO_WCSFTIME)

    str   = _RWSTD_STATIC_CAST (const char*, pun->am_pm (0, 0));
    pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
    size  = mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

    if (_RWSTD_SIZE_MAX == size) {
        // conversion failure - should not happen
        *pwbuf = L'\0';
        size   = 1;
    }
    else {
        _RWSTD_ASSERT (L'\0' == pwbuf [size]);
        size += 1;
    }

    pun->am_pm_off [1][0] = off;
    off                  += size * sizeof (wchar_t);

    str   = _RWSTD_STATIC_CAST (const char*, pun->am_pm (1, 0));
    pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
    size  = mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

    if (_RWSTD_SIZE_MAX == size) {
        // conversion failure - should not happen
        *pwbuf = L'\0';
        size   = 1;
    }
    else {
        _RWSTD_ASSERT (L'\0' == pwbuf [size]);
        size += 1;
    }

    pun->am_pm_off [1][1] = off;
    off                  += size * sizeof (wchar_t);

#    endif   // _RWSTD_NO_WCSFTIME

    // convert "%x" to its wide equivalent
    str   = _RWSTD_STATIC_CAST (const char*, pun->d_fmt (0));
    pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
    size  = mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

    if (_RWSTD_SIZE_MAX == size) {
        // conversion failure - should not happen
        *pwbuf = L'\0';
        size   = 1;
    }
    else {
        _RWSTD_ASSERT (L'\0' == pwbuf [size]);
        size += 1;
    }

    pun->d_fmt_off [1] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off               += size * sizeof (wchar_t);
    
    // convert "%X" to its wide equivalent
    str   = _RWSTD_STATIC_CAST (const char*, pun->t_fmt (0));
    pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
    size  = mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

    if (_RWSTD_SIZE_MAX == size) {
        // conversion failure - should not happen
        *pwbuf = L'\0';
        size   = 1;
    }
    else {
        _RWSTD_ASSERT (L'\0' == pwbuf [size]);
        size += 1;
    }

    pun->t_fmt_off [1] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off               += size * sizeof (wchar_t);

    // convert "%c" to its wide equivalent
    str   = _RWSTD_STATIC_CAST (const char*, pun->d_t_fmt (0));
    pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
    size  = mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

    if (_RWSTD_SIZE_MAX == size) {
        // conversion failure - should not happen
        *pwbuf = L'\0';
        size   = 1;
    }
    else {
        _RWSTD_ASSERT (L'\0' == pwbuf [size]);
        size += 1;
    }

    pun->d_t_fmt_off [1] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off                 += size * sizeof (wchar_t);

    // convert "%r" to its wide equivalent
    str   = _RWSTD_STATIC_CAST (const char*, pun->t_fmt_ampm (0));
    pwbuf = _RWSTD_REINTERPRET_CAST (wchar_t*, pbuf + off);
    size  = mbstowcs (pwbuf, str, (bufsize - off) / sizeof (*pwbuf));

    if (_RWSTD_SIZE_MAX == size) {
        // conversion failure - should not happen
        *pwbuf = L'\0';
        size   = 1;
    }
    else {
        _RWSTD_ASSERT (L'\0' == pwbuf [size]);
        size += 1;
    }

    pun->t_fmt_ampm_off [1] = _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, off);
    off                    += size * sizeof (wchar_t);

#  endif   // _RWSTD_NO_WCHAR_T
#endif   // _RWSTD_NO_NL_LANGINFO

    // set `impdata' and `impsize' (base dtor will delete)
    __rw_access::_C_get_impdata (*_RWSTD_CONST_CAST (__rw_facet*, pfacet))
        = pun;

    __rw_access::_C_get_impsize (*_RWSTD_CONST_CAST (__rw_facet*, pfacet))
        = _RWSTD_SIZE_MAX;

    // call self recursively on already initialized `impdata'
    return __rw_get_timepunct (pfacet, flags, inx);
}


_RWSTD_SIZE_T
__rw_get_timepunct (const __rw_facet *pfacet, int flags,
                    const void *names[], _RWSTD_SIZE_T sizes[])
{
    const bool wide = !!(flags & __rw_wide);

    const int member = flags & ~__rw_wide;

    _RWSTD_SIZE_T count = 0;

    int flags_2 = 0;

    switch (member) {
    case __rw_abday:
    case __rw_day:
        count =  7;
        break;

    case __rw_abmon:
    case __rw_month:
        count = 12;
        break;

    case __rw_ampm:
        count =  2;
        break;

    case __rw_d_t_fmt:
    case __rw_d_fmt:
    case __rw_t_fmt:
    case __rw_t_fmt_ampm:
    case __rw_era_d_t_fmt:
    case __rw_era_d_fmt:
    case __rw_era_t_fmt:
        count =  1;
        break;

    case __rw_era_fmts:
    case __rw_era_names: {

        const __rw_time_t* const ptime =
            _RWSTD_STATIC_CAST (const __rw_time_t*,
                                __rw_get_timepunct (pfacet, 0, 0));

        count = (_RWSTD_SIZE_T)ptime->era_count ();
        break;
    }

    case __rw_abday | __rw_day:
        count   = 7;
        flags   = __rw_day;
        flags_2 = __rw_abday;
        break;
        
    case __rw_abmon | __rw_month:
        count   = 12;
        flags   = __rw_month;
        flags_2 = __rw_abmon;
        break;

    case __rw_alt_digits: {

        const __rw_time_t* const ptime =
            _RWSTD_STATIC_CAST (const __rw_time_t*,
                                __rw_get_timepunct (pfacet, 0, 0));

        count = (_RWSTD_SIZE_T)ptime->alt_digits_count ();
        break;
    }

    default:
        _RWSTD_ASSERT (!"bad discriminant");
        return 0;
    }

    typedef _STD::char_traits<char> CTraits;

#ifndef _RWSTD_NO_WCHAR_T

    typedef _STD::char_traits<wchar_t> WTraits;

#endif   // _RWSTD_NO_WCHAR_T

    if (wide)
        flags |= __rw_wide;

    for (_RWSTD_SIZE_T i = 0; i != count; ++i) {
        names [i] = __rw_get_timepunct (pfacet, flags, i);

#ifndef _RWSTD_NO_WCHAR_T

        if (wide)
            sizes [i] = WTraits::length ((const wchar_t*)names [i]);
        else

#endif   // _RWSTD_NO_WCHAR_T

            sizes [i] = CTraits::length ((const char*)names [i]);
    }

    if (flags_2) {

        if (wide)
            flags_2 |= __rw_wide;

        for (_RWSTD_SIZE_T j = count; j != 2 * count; ++j) {
            names [j] = __rw_get_timepunct (pfacet, flags_2, j - count);

#ifndef _RWSTD_NO_WCHAR_T

            if (wide)
                sizes [j] = WTraits::length ((const wchar_t*)names [j]);
            else

#endif   // _RWSTD_NO_WCHAR_T

                sizes [j] = CTraits::length ((const char*)names [j]);
        }

        return 2 * count;
    }

    return count;
}


_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_get_timepunct (const __rw_facet *pfacet,
                    int data [4], tm *tmb, int **pmem,
                    const void *names[], _RWSTD_SIZE_T sizes[])
{
    enum _Fmt {   // for convenience
        _AmPm        = __rw_ampm,
        _Date        = __rw_d_fmt,
        _DateTime    = __rw_d_t_fmt,
        _Days        = __rw_day | __rw_abday,
        _Mons        = __rw_month | __rw_abmon,
        _Time        = __rw_t_fmt,
        _TimeAmPm    = __rw_t_fmt_ampm,

        // alternative formats and eras
        _AltDigits   = __rw_alt_digits,
        _EraDate     = __rw_era_d_fmt,
        _EraDateTime = __rw_era_d_t_fmt,
        _EraFmts     = __rw_era_fmts,
        _EraTime     = __rw_era_t_fmt,
        _EraNames    = __rw_era_names
    };

    const char fmt  = data [0];        // on input, get the format...
    const char mod  = data [1];        // ...and modifier characters
    const bool wide = 0 != data [2];   // narrow or wide specialization?

    int &adj = data [0];   // value to increment computed result by
    int &fac = data [1];   // value to multiply computed result by
    int &lob = data [2];   // lower bound on valid input value
    int &hib = data [3];   // higher bound on valid input value

    adj = 0;
    fac = 1;
    lob = 0;
    hib = 0;

    const void** pv =
        _RWSTD_REINTERPRET_CAST (const void**, names);

    _RWSTD_SIZE_T cnt = 0;

    int flags = 0;

    if ('E' == mod) {

        switch (fmt) {

        case 'C':
            // %EC: the name of the period in alternative representation
            *pmem = &tmb->tm_year;
            flags = _EraNames;
            break;

        case 'c':
            // %Ec: alternative date and time representation
            flags = _EraDateTime;
            break;

        case 'X':
            // %EX: alternative time representation
            flags = _EraTime;
            break;

        case 'x':
            // %Ex: alternative date representation
            flags = _EraDate;
            break;

        case 'Y':
            // %EY: full alternative year representation
            flags = _EraFmts;
            break;

        case 'y':
            // %Ey: offset from %EC (year only) in alternative representation
            // FIXME: implement
            break;
        }
    }
    else if ('O' == mod) {

        flags = _AltDigits;

        switch (fmt) {
        case 'd': case 'e':
            // %Od: day of the month using alternative numeric symbols
            // %Oe: equivalent to %Od
            *pmem = &tmb->tm_mday;
            lob   = 1;
            hib   = 31;
            break;

        case 'H':
            // %OH: hour (24-hour clock) using alternative numeric symbols
            *pmem = &tmb->tm_hour;
            hib   = 23;
            break;

        case 'I':
            // %OI: hour (12-hour clock) using the alternative numeric symbols
            *pmem = &tmb->tm_hour;
            hib   = 12;
            break;

        case 'm':
            // %Om: month using alternative numeric symbols
            *pmem = &tmb->tm_mon;
            adj   = -1;
            hib   = 11;
            break;

        case 'M':
            // %OM: minutes using alternative numeric symbols
            *pmem = &tmb->tm_min;
            hib   = 59;
            break;

        case 'S':
            // %OS seconds using alternative numeric symbols
            *pmem = &tmb->tm_sec;
            hib   = 60;
            break;

        case 'U':
            // %OU: Sunday-based week number of the year
            //      using alternative numeric symbols
            // FIXME: implement
            hib = 52;
            break;

        case 'W':
            // %OW: Monday-based week number of the year
            //      using alternative numeric symbols
            // FIXME: implement
            hib = 52;
            break;

        case 'w':
            // %Ow: Sunday-based weekday using alternative numeric symbols
            *pmem = &tmb->tm_wday;
            hib   = 6;
            break;

        case 'y':
            // %Oy: year (offset from %C) using alternative numeric symbols
            *pmem = &tmb->tm_year;
            hib   = 99;
            break;

        default:
            flags = 0;
        }
    }
    else {

        switch (fmt) {

        case 'A': case 'a':   // weekday [00..11]
            *pmem = &tmb->tm_wday;
            hib   = 6;
            flags = _Days;
            break;

        case 'B': case 'b': case 'h':   // month [00..11]
            *pmem = &tmb->tm_mon;
            hib   = 11;
            flags = _Mons;
            break;

        case 'C':   // century [00..99]
            adj   = -1900;
            fac   = 100;
            lob   = -1900;
            hib   = 8000;
            *pmem = &tmb->tm_year;
            break;

        case 'c':   // locale-specific date
            flags = _DateTime;
            break;

        case 'D': {   // equivalent to "%m/%d/%y"
            static const void* const pat[] = { "%m/%d/%y", L"%m/%d/%y" };
            names [0] = pat [wide];
            sizes [0] = 8;
            cnt       = 1;
            break;
        }

        case 'd': case 'e':   // day of month [01..31]
            lob   = 1;
            hib   = 31;
            *pmem = &tmb->tm_mday;
            break;

        case 'k':   // popular extension (AIX, Linux, Solaris)
        case 'H':   // hour [00..23]
            hib   = 23;
            *pmem = &tmb->tm_hour;
            break;

        case 'l':   // popular extension (Linux, Solaris)
        case 'I':   // hour [01..12]
            adj   = -1;
            hib   = 11;
            *pmem = &tmb->tm_hour;
            break;

        case 'j':   // day number of the year [001..366]
            adj   = -1;
            hib   = 365;
            *pmem = &tmb->tm_yday;
            break;

        case 'M':   // minutes [00..59]
            hib   = 59;
            *pmem = &tmb->tm_min;
            break;

        case 'm':   // month [00..11]
            adj   = -1;
            hib   = 11;
            *pmem = &tmb->tm_mon;
            break;

        case 'p':   // the locale's equivalent of AM or PM
            *pmem = &tmb->tm_hour;
            flags = _AmPm;
            break;

        case 'R': {   // equivalent to "%H:%M"
            static const void* const pat[] = { "%H:%M", L"%H:%M" };
            names [0] = pat [wide];
            sizes [0] = 5;
            cnt       = 1;
            break;
        }
        
        case 'r':   // 12-hour clock time using the AM/PM notation
            flags = _TimeAmPm;
            break;

        case 'S':   // seconds [00..60]
            hib   = 60;
            *pmem = &tmb->tm_sec;
            break;

        case 'T': {   // equivalent to "%H:%M:%S"
            static const void* const pat[] = { "%H:%M:%S", L"%H:%M:%S" };

            names [0] = pat [wide];
            sizes [0] = 8;
            cnt       = 1;
            break;
        }
        
        case 'U':   // Sunday-based week number [00..53]
            hib = 53;
            // FIXME: implement
            break;

        case 'W':   // Monday-based week number [00..53]
            hib = 53;
            // FIXME: implement
            break;

        case 'w':   // Sunday-based weekday [0..6]
            hib   = 6;
            *pmem = &tmb->tm_wday;
            break;

        case 'x':   // locale-specific date
            flags = _Date;
            break;

        case 'X':   // locale-specific time
            flags = _Time;
            break;

        case 'y':   // 2-digit year, [00..99]
            *pmem = &tmb->tm_year;
            break;
        
        case 'Y':   // 4-digit year
            adj   = -1900;
            lob   = -1900;
            hib   = _RWSTD_INT_MAX - 1900;
            *pmem = &tmb->tm_year;
            break;

        case '%':
            break;
        }
    }

    if (flags)
        cnt = __rw_get_timepunct (pfacet, flags | (wide ? __rw_wide : 0),
                                  pv, sizes);

    return cnt;
}


static char*
__rw_fmt_time (const __rw_facet *pfacet, char *buf, _RWSTD_SIZE_T bufsize,
               _STD::ios_base &flags, char fill, const tm *tmb,
               const char *pat)
{
    _RWSTD_ASSERT (0 != tmb);
    _RWSTD_ASSERT (0 != pat);

    for (; *pat; ++pat) {

        if ('%' == *pat && pat [1]) {

            int width = -1;   // width of curent specifier (-1 = unspecified)
            int prec  = -1;   // precision of current specifier

            ++pat;

            // Extension: width and precision
            // Reference: strftime() on HP-UX 11.00
            // Format: %[[-|0]w][.p]

            // [-|0] right justification (left by default),
            //       padded with '0' characters (spaces by default)
            // w     minimum field width
            // .p    minimum number of digits (expanded on the left with '0')
            //       for the following specifiers:
            //           d, H, I, j, m, M, o, S, U, w, W, y and Y
            //       maximum number of characters (truncated if necessary)
            //       for the following specifiers:
            //           a, A, b, B, c, D, E, F, h, n, N, p, r, t,
            //           T, x, X, z, Z, and %

            if ('-' == *pat) {
                // right justify
                // FIXME: handle right justification
                ++pat;
            }
            else if ('0' == *pat) {
                // pad justified output with '0'
                fill = '0';
                ++pat;
            }

            if (*pat >= '0' && *pat <= '9') {

                // if width is given, set precision to 0 to prevent
                // it from being overridden later by __rw_put_time()
                width = 0;
                prec  = 0;

                for (; *pat >= '0' && *pat <= '9'; ++pat) {
                    // set the width of the current specifier
                    width = width * 10 + *pat - '0';
                }
            }

            if ('.' == *pat) {
                ++pat;

                if (*pat >= '0' && *pat <= '9') {

                    // treat missing width as 0 (e.g., "%.3d") to prevent
                    // it from being overridden later by __rw_put_time()
                    if (-1 == width)
                        width = 0;

                    prec = 0;

                    for (; *pat >= '0' && *pat <= '9'; ++pat) {
                        // set the precision of the current specifier
                        prec = prec * 10 + *pat - '0';
                    }
                }
            }

            char fmtmod = *pat;   // format modifier
            char fmt;             // format specifier

            if ('E' == fmtmod && *pat) {

                fmt = *++pat;

                switch (fmt) {
                    // check valid format modifiers
                case 'c': case 'C': case 'x': case 'X': case 'y': case 'Y':
                    break;

                case '\0':
                    break;

                default:
                    fmt = 0;   // `pat' doesn't point to a valid format
                }
            }
            else if ('O' == fmtmod) {

                fmt = *++pat;

                switch (fmt) {
                    // check valid format modifiers
                case 'd': case 'e': case 'H': case 'I': case 'm': case 'M':
                case 'S': case 'u': case 'U': case 'V': case 'w': case 'W':
                case 'y':
                    break;

                case '\0':
                    break;

                default:
                    fmt = 0;   // `pat' doesn't point to a valid format
                }
            }
            else {
                fmt    = fmtmod;
                fmtmod = 0;
            }

            if (char (-1) != fmt) {
                char *end = __rw_put_time (pfacet, buf, bufsize, flags, fill,
                                           tmb, fmt, fmtmod, width, prec);

                if (!end)
                    return 0;

                buf = end;
                continue;
            }
        }

        *buf++ = *pat;
    }

    return buf;
}


#ifndef _RWSTD_NO_WCHAR_T

static wchar_t*
__rw_fmt_time (const __rw_facet *pfacet, wchar_t *wbuf, _RWSTD_SIZE_T bufsize,
               _STD::ios_base &flags, wchar_t fill, const tm *tmb,
               const wchar_t *wpat)
{
    _RWSTD_ASSERT (0 != tmb);
    _RWSTD_ASSERT (0 != wpat);

    for (; *wpat; ++wpat) {

        if (L'%' == *wpat && wpat [1]) {

            int width = -1;   // width of the curent specifier (-1 = unspecified)
            int prec  = -1;   // precision of the current specifier

            ++wpat;

            // Extension: width and precision
            // Reference: strftime() on HP-UX 11.00
            // Format: %[[-|0]w][.p]

            // [-|0] right justification (left by default),
            //       padded with '0' characters (spaces by default)
            // w     minimum field width
            // .p    minimum number of digits (expanded on the left with '0')
            //       for the following specifiers:
            //           d, H, I, j, m, M, o, S, U, w, W, y and Y
            //       maximum number of characters (truncated if necessary)
            //       for the following specifiers:
            //           a, A, b, B, c, D, E, F, h, n, N, p, r, t,
            //           T, x, X, z, Z, and %

            if (L'-' == *wpat) {
                // right justify
                // FIXME: handle right justification
                ++wpat;
            }
            else if (L'0' == *wpat) {
                // pad justified output with '0'
                fill = L'0';
                ++wpat;
            }

            if (*wpat >= L'0' && *wpat <= L'9') {

                // if width is given, set precision to 0 to prevent
                // it from being overridden later by __rw_put_time()
                width = 0;
                prec  = 0;

                for (; *wpat >= L'0' && *wpat <= L'9'; ++wpat) {
                    // set the width of the current specifier
                    width = width * 10U + *wpat - L'0';
                }
            }

            if (L'.' == *wpat) {
                ++wpat;

                if (*wpat >= L'0' && *wpat <= L'9') {

                    // treat missing width as 0 (e.g., "%.3d") to prevent
                    // it from being overridden later by __rw_put_time()
                    if (-1 == width)
                        width = 0;

                    prec = 0;

                    for (; *wpat >= L'0' && *wpat <= L'9'; ++wpat) {
                        // set the precision of the current specifier
                        prec = prec * 10 + *wpat - L'0';
                    }
                }
            }

            char fmtmod = char (*wpat);   // format modifier
            char fmt;                     // format specifier

            if ('E' == fmtmod && *wpat) {

                fmt = char (*++wpat);

                switch (fmt) {
                    // check valid format modifiers
                case 'c': case 'C': case 'x': case 'X': case 'y': case 'Y':
                    break;

                case '\0':
                    break;

                default:
                    fmt = 0;   // `wpat' doesn't point to a valid format
                }
            }
            else if ('O' == fmtmod) {

                fmt = char (*++wpat);

                switch (fmt) {
                    // check valid format modifiers
                case 'd': case 'e': case 'H': case 'I': case 'm': case 'M':
                case 'S': case 'u': case 'U': case 'V': case 'w': case 'W':
                case 'y':
                    break;

                case '\0':
                    break;

                default:
                    fmt = 0;   // `wpat' doesn't point to a valid format
                }
            }
            else {
                fmt    = fmtmod;
                fmtmod = 0;
            }

            if (char (-1) != fmt) {
                wchar_t* const end =
                    __rw_put_time (pfacet, wbuf, bufsize, flags, fill,
                                   tmb, fmt, fmtmod, width, prec);

                if (!end)
                    return 0;

                wbuf = end;
                continue;
            }
        }

        *wbuf++ = *wpat;
    }

    return wbuf;
}

#endif   // _RWSTD_NO_WCHAR_T


// returns true iff `y' is a leap year
static inline bool
__rw_isleap (int y)
{
    return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
}


// returns the ISO 8601 week number from the given date
// sets `year' to the ISO 8601 year number for that date
static int
__rw_iso_week (int& year, int yday, int wday)
{
    // adjust the week day - tm struct uses a 0-based weekday number
    // starting with Sunday, while ISO 8601 week starts with Monday
    const int w = (wday + 7 - 1) % 7;

    // number of days in the current year
    const int y = 365 + __rw_isleap (year);

    // number of days between first of January and the first week-end
    const int fdoy = (yday + 7 - w) % 7;

    int isow;

    if (fdoy >= 4) {
        // first of Jan belongs to first week
        isow = yday + (7 - fdoy);
    }
    else {
        // first of Jan belongs to last week of last year
        isow = yday - fdoy;
    }

    if (isow < 0) { 
        // the day belongs to last year
        year--;
        return __rw_iso_week (year, yday + 365 + __rw_isleap (year), wday);
    }

    // the day could belong to next year; check that
    const int r = y - yday;
    if (r <= 3 && 3 - r >= w) {
        year++;
        return w;
    }
        
    return isow;
}


// returns a pointer to the era_t element describing the era
// corresponding to the date pointed to by `tmb', or 0 if no
// such era exists
static const __rw_time_t::era_t*
__rw_get_era (const __rw_time_t *ptime, const tm *tmb)
{
    _RWSTD_ASSERT (0 != tmb);

    const int year = 1900 + tmb->tm_year;

    // get the number of eras in this locale
    const _RWSTD_SIZE_T neras = (_RWSTD_SIZE_T)ptime->era_count ();

    for (_RWSTD_SIZE_T i = 0; i != neras; ++i) {
        const __rw_time_t::era_t* const pera = ptime->era (i);

        _RWSTD_ASSERT (0 != pera);

        // offset <  0 implies direction of '-'
        // offset >= 0 implies direction of '+'
        const int b = pera->offset < 0;   // start index
        const int e = !b;                 // end index

        // check to see if the specified date belongs to the era
        if (   year > pera->year [b] && year < pera->year [e]
            || (   (year == pera->year [b] || year == pera->year [e])
                && (   tmb->tm_mon > pera->month [b]
                    && tmb->tm_mon < pera->month [e]
                    || (    (   tmb->tm_mon == pera->month [b]
                             || tmb->tm_mon == pera->month [e])
                        && tmb->tm_mday >= pera->day [b]
                        && tmb->tm_mday <= pera->day [e])))) {
            return pera;
        }
    }

    return 0;
}


static int
__rw_get_zone_off (const char *var, const char **var_end)
{
    _RWSTD_ASSERT (0 != var);
    _RWSTD_ASSERT (0 != var_end);

    bool neg = false;

    if ('-' == *var) {
        neg = true;
        ++var;
    }
    else if ('+' == *var) {
        ++var;
    }

    int offset = 0;

    // compute the four-digit offset in the format hhmm from
    // the string `var' in the format hh[:mm[:ss]], where
    // hh is in the range "0" through "24", and ss and mm
    // both in the range "00" through "59"

    if (ISDIGIT (*var)) {

        offset = *var++ - '0';

        if (ISDIGIT (*var)) {
            if (offset < 2 || *var >= '0' && *var <= '4') {
                // add offset in hours
                offset = offset * 10 + *var++ - '0';
            }
            else
                offset = _RWSTD_INT_MIN;
        }

        if (0 <= offset)
            offset *= 100;

        if (':' == *var) {

            ++var;

            if (var [0] >= '0' && var [0] <= '5' && ISDIGIT (var [1])) {
                // add offset in minutes
                offset += 10 * (*var++ - '0');
                offset += (*var++ - '0');

                if (':' == *var) {

                    ++var;

                    if (   var [0] >= '0' && var [0] <= '5'
                        && ISDIGIT (var [1])) {

                        // ignore offset in seconds
                        var += 2;
                    }
                    else {
                        offset = _RWSTD_INT_MIN;
                    }
                }
            }
            else {
                offset = _RWSTD_INT_MIN;
            }
        }
    }
    else {
        offset = _RWSTD_INT_MIN;
    }

    *var_end = var;

    return neg ? -offset : offset;
}


// Also used as a parameter to rw_get_static_mutex to ensure
// the uniqueness of the mutex object used in synchronizing
// the threads using __rw_get_time_put_data()
// (see RWSTD_MT_STATIC_GUARD below)
struct __rw_time_put_data
{
    const void *fmt;      // format string to use to format val with
    const void *str;      // the final formatted string
    int         val;      // numeric value to format
    int         altval;   // value to format using alternative symbols
    int         width;    // the width of numeric output (as in "%*.0d")
    int         prec;     // the precision of numeric output (as in "%0.*d")
};


static int
__rw_get_zone (__rw_time_put_data &tpd, const char *var, int isdst)
{
    _RWSTD_ASSERT (0 != var);

    // TZ format:
    //     :characters
    // or
    //     std offset[dst[offset][,start[/time],end[/time]]]

    int std_off;

    if (':' == *var)
        goto use_tzset;   // can't parse implementation-defined formats

    if ('<' == *var) {
        // skip past the alphanumeric std designator enclosed in <>
        while (*var && '>' != *var++);
    }
    else {
        const char* const stdbeg = var;

        // skip past the alphabetic std designator to the beginning
        // of the required dst offset; if no dst offset is found,
        // the variable is not in the required POSIX format and must
        // be handled in an implementation-defined way (i.e., using
        // tzset())

        for (; ISALPHA (*var); ++var);

        if (   var == stdbeg
            || *var && '+' != *var && '-' != *var && !ISDIGIT (*var))
            goto use_tzset;
    }

    // get the std offset from TZ in case dst must be computed
    std_off = __rw_get_zone_off (var, &var);

    if (*var && _RWSTD_INT_MIN != std_off && isdst) {

        const char* const dstbeg = var;

        if ('<' == *var)
            // skip past the quoted alphanumeric dst designator
            while (*var && '>' != *var++);
        else {
            // skip past the alphabetic dst designator
            for (; ISALPHA (*var); ++var);
        }

        if (   var == dstbeg
            || *var && '+' != *var && '-' != *var && !ISDIGIT (*var))
            goto use_tzset;

        tpd.val = __rw_get_zone_off (var, &var);

        if (_RWSTD_INT_MIN == tpd.val) {

            // failed to extract dst offset from TZ
            // check if that's because there was none

            if ('\0' == *var) {
                // according to SUSv3 (POSIC), if no offset follows dst,
                // the alternative time is assumed to be one hour ahead
                // of standard time

                tpd.val = std_off + 100;

                if (2359 < tpd.val)
                    tpd.val %= 2400;
            }
            else
                goto use_tzset;
        }
    }
    else
        goto use_tzset;

    return 0;   // success

use_tzset:

    // the lock cannot prevent another thread from calling tzset()
    // directly (or one of the other functions in the #else block)
    // for "stronger" thread-safety it might be better to take the
    // #else branch below and use gmtime_r() there (that would
    // only be safe if mktime() were thread-safe)
    _RWSTD_MT_STATIC_GUARD (__rw_time_put_data);

#ifndef _RWSTD_NO_TIMEZONE

    // set the POSIX `timezone' and `daylight' extern variables
    tzset ();

    // tzet() sets timezone to the difference, in seconds, between
    // Coordinated Universal Time (UTC) and local standard time
    tpd.val = _RWSTD_STATIC_CAST (int, timezone / 60);

#else   // if defined (_RWSTD_NO_TIMEZONE)

    tpd.val = 0;

    // calculate the difference the hard way
    static const time_t tgm = 0;

    const tm* const tmp = gmtime (&tgm);

    if (tmp) {
        tm tmgm = *tmp;

        const time_t tlocal = mktime (&tmgm);

        if (time_t (-1) != tlocal) {

            const double diff = difftime (tlocal, tgm);

            tpd.val = int (diff / 60);
        }
        else {
            // FIXME: indicate the nature of the error to the caller somehow
            return -1;
        }
    }
    else {
        // FIXME: indicate the nature of the error to the caller somehow
        return -1;
    }

#endif   // _RWSTD_NO_TIMEZONE

    // set the value to the difference in the HH:MM "format"
    tpd.val = 100 * (tpd.val / 60) + tpd.val % 60;

    if (daylight && isdst)
        tpd.val += (tpd.val < 0 ? -100 : 100) * isdst;

    return 0;   // success
}


static void
__rw_get_zone_name (__rw_time_put_data&, const char*, int)
{
    // FIXME: retrieve tiem zone name from TZ, store it in a suitable
    // buffer, and have time_put::put() output the contents of that
    // buffer
}


static void
__rw_get_time_put_data (__rw_time_put_data &tpd,
                        const __rw_facet   *facet,
                        const tm *tmb,
                        char fmt, char mod, bool wide)
{
    _RWSTD_ASSERT (0 != facet);

    // extension: if `tmb' is 0, output the locale-specific format string
    _RWSTD_ASSERT (   0 != tmb
                   || 'c' == fmt || 'r' == fmt || 'x' == fmt || 'X' == fmt);

    static const union {
#ifndef _RWSTD_NO_WCHAR_T
        wchar_t wide_data [1];
#endif   // _RWSTD_NO_WCHAR_T
        char data [4];
    } null_fmt = { { 0 } };

    const __rw_time_t *ptime =
        _RWSTD_STATIC_CAST (const __rw_time_t*, facet->_C_data ());

    if (!ptime)
        ptime = _RWSTD_STATIC_CAST (const __rw_time_t*,
                                    __rw_get_timepunct (facet, 0, 0));

    _RWSTD_ASSERT (0 != ptime);

    // invalidate data
    tpd.fmt = tpd.str = 0;
    tpd.val = tpd.altval = _RWSTD_INT_MIN;
    tpd.width = tpd.prec = 1;

    switch (fmt) {

    case 'a':
        // %a: the locale's abbreviated weekday name. [tm_wday]
        tpd.str = ptime->abday (tmb->tm_wday % 7U, wide);
        break;

    case 'A':
        // %A: the locale's full weekday name. [tm_wday]
        tpd.str = ptime->day (tmb->tm_wday % 7U, wide);
        break;

    case 'b':
        // %b: the locale's abbreviated month name. [tm_mon]
        tpd.str = ptime->abmon (tmb->tm_mon % 12U, wide);
        break;

    case 'B':
        // %B: the locale's full month name. [tm_mon]
        tpd.str = ptime->mon (tmb->tm_mon % 12U, wide);
        break;

    case 'c':
        // %c:  the locale's appropriate date and time representation.
        // %Ec: the locale's alternative date and time representation.

        if ('E' == mod)
            tpd.fmt = ptime->era_d_t_fmt (wide);

        if (   !tpd.fmt
#ifndef _RWSTD_NO_WCHAR_T
            ||  wide && !*_RWSTD_STATIC_CAST (const wchar_t*, tpd.fmt)
#endif   // _RWSTD_NO_WCHAR_T
            || !wide && !*_RWSTD_STATIC_CAST (const char*, tpd.fmt))
            tpd.fmt = ptime->d_t_fmt (wide);

        _RWSTD_ASSERT (0 != tpd.fmt);
        break;

    case 'C': {
        // %C:  the year divided by 100 and truncated to an integer,
        //      as a decimal number (00-99). [tm_year]
        // %EC: the name of the base year (period) in the locale's
        //      alternative representation.

        if ('E' == mod) {
            const __rw_time_t::era_t *pera = __rw_get_era (ptime, tmb);

            if (pera) {
                const _RWSTD_PTRDIFF_T i = pera - ptime->era (0);
                tpd.str = ptime->era_name (i, wide);
                break;
            }
        }

        // take care to handle negative tm_year
        tpd.val  = ((1900 + tmb->tm_year) / 100) % 100;
        tpd.prec = 2;
        break;
    }

    case 'd':
        // %d:  the day of the month as a decimal number (01-31). [tm_mday]
        // %Od: the day of the month, using the locale's alternative numeric
        //      symbols (filled as needed with leading zeros, or with leading
        //      spaces if there is no alternative symbol for zero).

        if ('O' == mod)
            tpd.altval = tmb->tm_mday;
        else
            tpd.val = tmb->tm_mday;

        tpd.prec = 2;

        break;

    case 'D': {
        // %D: equivalent to "%m/%d/%y". [tm_mon, tm_mday, tm_year]
        static const void* const fmats[] = { "%m/%d/%y", L"%m/%d/%y" };

        tpd.fmt = fmats [wide];
        break;
    }

    case 'e':
        // %e:  the day of the month as a decimal number (1-31);
        //      a single digit is preceded by a space. [tm_mday]
        // %Oe: is replaced by the day of the month, using the locale's
        //      alternative numeric symbols (filled as needed with leading
        //      spaces).

        if ('O' == mod)
            tpd.altval = tmb->tm_mday;
        else
            tpd.val = tmb->tm_mday;

        tpd.width = 2;
        break;

    case 'F': {
        // %F: equivalent to "%Y-%m-%d" (the ISO 8601 date format).
        //     [tm_year, tm_mon, tm_mday]

        static const void* const fmats[] = { "%Y-%m-%d", L"%Y-%m-%d" };

        tpd.fmt = fmats [wide];
        break;
    }

    case 'g':
        // %g: the last 2 digits of the week-based year as a decimal number
        //     (00-99). [tm_year, tm_wday, tm_yday]
        
        // get the ISO 8601 year
        tpd.val = tmb->tm_year + 1900;
        __rw_iso_week (tpd.val, tmb->tm_yday, tmb->tm_wday);

        tpd.val %= 100;
        tpd.prec = 2;
        break;

    case 'G':
        // %G: the week-based year as a decimal number (e.g., 1997).
        //     [tm_year, tm_wday, tm_yday]
        
        // get the ISO 8601 year
        tpd.val = tmb->tm_year + 1900;
        __rw_iso_week (tpd.val, tmb->tm_yday, tmb->tm_wday);

        break;

    case 'h':
        // %h: equivalent to "%b". [tm_mon]
        tpd.fmt = ptime->abmon (tmb->tm_mon % 12U, wide);
        break;

    case 'k':
        // %k:  the hour (24-hour clock) as a decimal number (0-23)
        //      with single digits preceded by a blank. [tm_hour]
        //      this is a popular extension implemented for example
        //      on AIX, Linux and Solaris
        tpd.width = 2;
        // fall through

    case 'H':
        // %H:  the hour (24-hour clock) as a decimal number (00-23). [tm_hour]
        // %OH: the hour (24-hour clock), using the locale's alternative
        //      numeric symbols.

        if ('O' == mod)
            tpd.altval = tmb->tm_hour;
        else
            tpd.val = tmb->tm_hour;

        // set precision only for "%H" and not for "%k"
        // (i.e., when width hasn't been overridden above)
        if (1 == tpd.width)
            tpd.prec = 2;
        break;

    case 'l':
        // %l: the hour (12-hour clock) as a decimal number (1,12)
        //     with single digits preceded by a blank. [tm_hour]
        //     this is a popular extension implemented for example
        //     on Linux and Solaris
        tpd.width = 2;
        // fall through

    case 'I': {
        // %I:  the hour (12-hour clock) as a decimal number (01-12). [tm_hour]
        // %OI: the hour (12-hour clock), using the locale's alternative
        //      numeric symbols.

        unsigned hour = tmb->tm_hour % 12;
        if (!hour)
            hour = 12;

        if ('O' == mod)
            tpd.altval = hour;
        else
            tpd.val = hour;

        // set precision only for "%I" and not for "%l"
        // (i.e., when width hasn't been overridden above)
        if (1 == tpd.width)
            tpd.prec = 2;

        break;
    }

    case 'j':
        // %j: the day of the year as a decimal number (001-366). [tm_yday]

        tpd.val  = tmb->tm_yday + 1;
        tpd.prec = 3;
        break;

    case 'm':
        // %m:  the month as a decimal number (01-12). [tm_mon]
        // %Om: the month, using the locale's alternative numeric symbols.

        if ('O' == mod)
            tpd.altval = tmb->tm_mon + 1;
        else
            tpd.val = tmb->tm_mon + 1;

        tpd.prec = 2;
        break;

    case 'M':
        // %M:  the minute as a decimal number (00-59). [tm_min]
        // %OM: the minutes, using the locale's alternative numeric symbols.

        if ('O' == mod)
            tpd.altval = tmb->tm_min;
        else
            tpd.val = tmb->tm_min;

        tpd.prec = 2;
        break;

    case 'n': {
        // %n: a new-line character.
        static const void* const fmats [] = { "\n", L"\n" };
        tpd.str = fmats [wide];
        break;
    }

    case 'p':
        // %p: the locale's equivalent of the AM/PM designations
        //     associated with a 12-hour clock. [tm_hour]
        tpd.fmt = ptime->am_pm ((tmb->tm_hour / 12) % 2U, wide);
        break;

    case 'r':
        // %r: the locale's 12-hour clock time. [tm_hour, tm_min, tm_sec]
        tpd.fmt = ptime->t_fmt_ampm (wide);
        break;

    case 'R': {
        // %R: equivalent to "%H:%M". [tm_hour, tm_min]
        static const void* const fmats[] = { "%H:%M", L"%H:%M" };

        tpd.fmt = fmats [wide];
        break;
    }

    case 'S':
        // %S:  the second as a decimal number (00-60). [tm_sec]
        // %OS: the seconds, using the locale's alternative numeric symbols.

        if ('O' == mod)
            tpd.altval = tmb->tm_sec;
        else
            tpd.val = tmb->tm_sec;

        tpd.prec = 2;
        break;

    case 't': {
        // %t: a horizontal-tab character.
        static const void* const fmats[] = { "\t", L"\t" };

        tpd.str = fmats [wide];
        break;
    }

    case 'T': {
        // %T: equivalent to "%H:%M:%S" (the ISO 8601 time format).
        //     [tm_hour, tm_min, tm_sec]
        static const void* const fmats[] = { "%H:%M:%S", L"%H:%M:%S" };

        tpd.fmt = fmats [wide];
        break;
    }

    case 'u':
        // %u:  the ISO 8601 weekday as a decimal number (1-7),
        //      where Monday is 1. [tm_wday]
        // %Ou: the ISO 8601 weekday as a number in the locale's alternative
        //      representation, where Monday is 1.

        if ('O' == mod)
            tpd.altval = 1 + (tmb->tm_wday ? tmb->tm_wday - 1 : 6);
        else
            tpd.val = 1 + (tmb->tm_wday ? tmb->tm_wday - 1 : 6);

        break;

    case 'U': {
        // %U:  the week number of the year (the first Sunday as the first
        //      day of week 1) as a decimal number (00-53).
        //      [tm_year, tm_wday, tm_yday]
        // %OU: the week number, using the locale's alternative numeric
        //      symbols.

        const int week = (tmb->tm_yday - tmb->tm_wday + 7) / 7;

        if ('O' == mod)
            tpd.altval = week;
        else
            tpd.val = week;

        tpd.prec = 2;
        break;
    }

    case 'V': {
        // %V:  the ISO 8601 week number (see below) as a decimal number
        //      (01-53). [tm_year, tm_wday, tm_yday]
        // %OV: the ISO 8601 week number, using the locale's alternative
        //      numeric symbols.

        int year = tmb->tm_year + 1900;
        const int week =
            __rw_iso_week (year, tmb->tm_yday, tmb->tm_wday) / 7 + 1;

        if ('O' == mod)
            tpd.altval = week;
        else
            tpd.val = week;

        tpd.prec = 2;
        break;
    }

    case 'w':
        // %w:  the weekday as a decimal number (0-6), where Sunday is 0.
        //      [tm_wday]
        // %Ow: the weekday as a number, using the locale's alternative
        //      numeric symbols.

        if ('O' == mod)
            tpd.altval = tmb->tm_wday;
        else
            tpd.val = tmb->tm_wday;

        break;

    case 'W': {
        // %W:  the week number of the year (the first Monday as the first
        //      day of week 1) as a decimal number (00-53).
        //      [tm_year, tm_wday, tm_yday]
        // %OW: the week number of the year, using the locale's alternative
        //      numeric symbols.

        const int week = (tmb->tm_yday - (tmb->tm_wday - 1 + 7) % 7 + 7) / 7;

        if ('O' == mod)
            tpd.altval = week;
        else
            tpd.val = week;

        tpd.prec = 2;
        break;
    }

    case 'x':
        // %x:  the locale's appropriate date representation.
        // %Ex: the locale's alternative date representation.

        if ('E' == mod)
            tpd.fmt = ptime->era_d_fmt (wide);

        if (   !tpd.fmt
#ifndef _RWSTD_NO_WCHAR_T
            ||  wide && !*_RWSTD_STATIC_CAST (const wchar_t*, tpd.fmt)
#endif   // _RWSTD_NO_WCHAR_T
            || !wide && !*_RWSTD_STATIC_CAST (const char*, tpd.fmt))
            tpd.fmt = ptime->d_fmt (wide);

        break;

    case 'X':
        // %X:  the locale's appropriate time representation.
        // %EX: the locale's alternative time representation.

        if ('E' == mod)
            tpd.fmt = ptime->era_t_fmt (wide);

        if (   !tpd.fmt
#ifndef _RWSTD_NO_WCHAR_T
            ||  wide && !*_RWSTD_STATIC_CAST (const wchar_t*, tpd.fmt)
#endif   // _RWSTD_NO_WCHAR_T
            || !wide && !*_RWSTD_STATIC_CAST (const char*, tpd.fmt))
            tpd.fmt = ptime->t_fmt (wide);

        break;

    case 'y':
        // %y:  the last 2 digits of the year as a decimal number (00-99).
        //      [tm_year]
        // %Ey: the offset from %EC (year only) in the locale's alternative
        ///     representation.
        // %Oy: the last 2 digits of the year, using the locale's alternative
        //      numeric symbols.

        if ('E' == mod) {

            const __rw_time_t::era_t* const pera = __rw_get_era (ptime, tmb);

            if (pera) {
                tpd.altval = pera->offset < 0
                    ? pera->year [0] - 1900 - tmb->tm_year - pera->offset
                    : 1900 + tmb->tm_year - pera->year [0] + pera->offset;

                break;
            }
        }

        if ('O' == mod)
            tpd.altval = (1900 + tmb->tm_year) % 100;
        else
            tpd.val = (1900 + tmb->tm_year) % 100;

        tpd.prec = 2;
        break;

    case 'Y':
        // %Y:  the year as a decimal number (e.g., 1997). [tm_year]
        // %EY: the locale's full alternative year representation.

        if (mod) {
            const __rw_time_t::era_t* const pera = __rw_get_era (ptime, tmb);

            if (   pera
                && !!(tpd.fmt = ptime->era_fmt (pera - ptime->era (0), wide)))
                break;
        }

        tpd.val = 1900 + tmb->tm_year;
        break;

    case 'z':
        // %z: the offset from UTC in the ISO 8601:2000 standard format
        //     (+hhmm or -hhmm), or by no characters if no timezone is
        //     determinable.
        //     For example, "-0430" means 4 hours 30 minutes behind UTC
        //     (west of Greenwich).
        //     If tm_isdst is zero, the standard time offset is used.
        //     If tm_isdst is greater than zero, the daylight savings
        //     time offset is used.
        //     If tm_isdst is negative, no characters are returned.
        //     [tm_isdst]

        if (tmb->tm_isdst < 0) {
            // force no output
            tpd.val = _RWSTD_INT_MIN;
            tpd.fmt = null_fmt.data;
            break;
        }

        // fall through

    case 'Z': {
        // %Z: the locale's time zone name or abbreviation, or no
        //     characters if no time zone is determinable. [tm_isdst]

        if ('z' == fmt) {

            tpd.prec = 4;

#if defined (__GLIBC__) && defined (_RWSTD_NO_PURE_C_HEADERS)

            // GNU glibc uses gmtoff and zone instead of timezone and
            // tzname when computing/formatting time zone information

#  if defined (__STRICT_ANSI__) && !defined (__USE_BSD)

            tpd.val = tmb->__tm_gmtoff;

#  else   // if !defined (__STRICT_ANSI__) || defined (__USE_BSD)

            tpd.val = tmb->tm_gmtoff;

#  endif   // __STRICT_ANSI__, __USE_BSD

            if (tpd.val) {

                // use the offset from GMT only if it's non-zero
                // and ignore the tm_isdst flag (glibc behavior)
                tpd.val /= 60;
                tpd.val  = 100 * (tpd.val / 60) + tpd.val % 60;

                break;
            }

            // otherwise (0 GMT offset), use the TZ environment variable
            // to determine the global offset and consider the tm_isdst
            // flag (POSIX behavior)

#endif   // __GLIBC__ && _RWSTD_NO_PURE_C_HEADERS

            const char* const var = getenv ("TZ");

            if (!var || !*var || __rw_get_zone (tpd, var, tmb->tm_isdst)) {
                // force no output on error
                tpd.val = _RWSTD_INT_MIN;
                tpd.fmt = null_fmt.data;
                break;
            }
        }
        else {
            const char* const var = getenv ("TZ");

            if (!var || !*var) {
                // force no output
                tpd.val = _RWSTD_INT_MIN;
                tpd.fmt = null_fmt.data;
                break;
            }

            __rw_get_zone_name (tpd, var, tmb->tm_isdst);
        }
        
        break;
    }

    case '%': {
        static const void* const fmats[] = { "%", L"%" };

        tpd.str = fmats [wide];
        break;
    }

    default: break;
    }
}


_RWSTD_EXPORT char*
__rw_put_time (const __rw_facet *facet, char *buf, _RWSTD_SIZE_T bufsize,
               _STD::ios_base &flags, char fill, const tm *tmb,
               char fmt, char mod, int width, int prec)
{
    _RWSTD_ASSERT (0 != facet);

    _RWSTD_SIZE_T res = 0;   // size of formatted output, -1 on error

    // compute the values and get the format
    __rw_time_put_data tpd;
    __rw_get_time_put_data (tpd, facet, tmb, fmt, mod, false /* narrow */);

    if (_RWSTD_INT_MIN != tpd.altval) {

        // retrieve the alternative numeric symbols (if any)

        const __rw_time_t *ptime =
            _RWSTD_STATIC_CAST (const __rw_time_t*, facet->_C_data ());

        _RWSTD_ASSERT (0 != ptime);

        const _RWSTD_UINT32_T ndigits = ptime->alt_digits_count ();

        const _RWSTD_UINT32_T altval =
            _RWSTD_STATIC_CAST (_RWSTD_UINT32_T, tpd.altval);

        if (altval < ndigits) {

            // format using alternative numeric symbols
            const char* digit =
               _RWSTD_STATIC_CAST (const char*, ptime->alt_digits (altval, 0));

            _RWSTD_ASSERT (digit);

            char *pbuf = buf;

            while (*digit)
                *pbuf++ = *digit++;

            return pbuf;
        }

        // if the value exceeds the range representable by the available
        // alternative numeric symbols, format using ordinary digits
        tpd.val = tpd.altval;
    }

    if (_RWSTD_INT_MIN != tpd.val) {

        const char* const fmtstr = 'z' == fmt ? "%+*.*d" : "%*.*d";

        res = (_RWSTD_SIZE_T)sprintf (buf, fmtstr,
                                      width < 0 ? tpd.width : width,
                                      prec < 0 ? tpd.prec : prec, tpd.val);
    }
    else {
        if (!tmb && tpd.fmt) {
            tpd.str = tpd.fmt;
            tpd.fmt = 0;
        }

        if (tpd.fmt) {

            const char* const fmtstr =
                _RWSTD_STATIC_CAST (const char*, tpd.fmt);

            return __rw_fmt_time (facet, buf, bufsize, flags,
                                  fill, tmb, fmtstr);
        }

        if (tpd.str) {
            // copy data if available

            const char *src = _RWSTD_STATIC_CAST (const char*, tpd.str);

            for (char *dst = buf; *src && res < bufsize; ++src, ++dst, ++res)
                *dst = *src;

            _RWSTD_ASSERT (bufsize >= res);
        }
        else {
            char fmtstr [4] = { '%', fmt, '\0', '\0' };

            if (mod) {
                fmtstr [1] = mod;
                fmtstr [2] = fmt;
            }

            // use strftime() for locale-independent formatting
            res = strftime (buf, bufsize, fmtstr, tmb);

            _RWSTD_ASSERT (bufsize > res);
        }
    }

    return buf + res;
}


#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_EXPORT wchar_t*
__rw_put_time (const __rw_facet *facet, wchar_t *wbuf, _RWSTD_SIZE_T bufsize,
               _STD::ios_base &flags, wchar_t fill, const tm *tmb,
               char fmt, char mod, int width, int prec)
{
    _RWSTD_ASSERT (0 != facet);

    _RWSTD_SIZE_T res = 0;   // size of formatted output, -1 on error

    __rw_time_put_data tpd;

    __rw_get_time_put_data (tpd, facet, tmb, fmt, mod, true /* wide */);

    if (_RWSTD_INT_MIN != tpd.altval) {

        const __rw_time_t *ptime =
            _RWSTD_STATIC_CAST (const __rw_time_t*, facet->_C_data ());

        _RWSTD_ASSERT (0 != ptime);

        const _RWSTD_UINT32_T n = ptime->alt_digits_count ();

        if (tpd.altval >= 0 && tpd.altval < int (n)) {

            const _RWSTD_SIZE_T altval =
                _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, tpd.altval);

            // format using alternative numeric symbols
            const wchar_t* digit =
            _RWSTD_STATIC_CAST (const wchar_t*, ptime->alt_digits (altval, 1));

            _RWSTD_ASSERT (digit);

            wchar_t *pbuf = wbuf;

            while (*digit)
                *pbuf++ = *digit++;

            return pbuf;
        }

        // format using ordinary digits
        tpd.val = tpd.altval;
    }

    if (_RWSTD_INT_MIN != tpd.val) {

#ifndef _RWSTD_NO_SWPRINTF

        const wchar_t *fmtstr = 'z' == fmt ? L"%+*.*d" : L"%*.*d";

        res = swprintf (wbuf, 
#if !defined (__MINGW32__) && (!defined (_MSC_VER) || 1400 <= _MSC_VER)
                        // MSVC 8.0 changed swprintf() to conform
                        // to the C standard signature
                        bufsize,
#endif   // not MSVC || MSVC >= 8.0
                        fmtstr,
                        width < 0 ? tpd.width : width,
                        prec < 0 ? tpd.prec : prec, tpd.val);

#else   // if defined (_RWSTD_NO_SWPRINTF)

        const char *fmtstr = 'z' == fmt ? "%+*.*d" : "%*.*d";

        char buf [64];

        res = (_RWSTD_SIZE_T)::sprintf (buf, fmtstr,
                                        width < 0 ? tpd.width : width,
                                        prec < 0 ? tpd.prec : prec, tpd.val);

        _RWSTD_ASSERT (res < sizeof buf);

        wchar_t *dst = wbuf;

        for (const char *s = buf; *s; ++s, ++dst)
            *dst = _RWSTD_STATIC_CAST (unsigned char, *s);

        res = dst - wbuf;

#endif   // _RWSTD_NO_SWPRINTF

    }
    else {
        if (!tmb && tpd.fmt) {
            tpd.str = tpd.fmt;
            tpd.fmt = 0;
        }

        if (tpd.fmt) {

            const wchar_t* const wfmt =
                _RWSTD_STATIC_CAST (const wchar_t*, tpd.fmt);

            return __rw_fmt_time (facet, wbuf, bufsize, flags, fill, tmb, wfmt);
        }

        if (tpd.str) {
            // copy data if available

            const wchar_t *src = _RWSTD_STATIC_CAST (const wchar_t*, tpd.str);

            for (wchar_t *dst = wbuf; *src && res < bufsize; ++src, ++dst, ++res)
                *dst = *src;

            _RWSTD_ASSERT (bufsize >= res);
        }
        else {

#if !defined (_RWSTD_NO_WCSFTIME_WCHAR_T_FMAT) && !defined (_RWSTD_NO_WCSFTIME)

            wchar_t fmtstr [4] = { L'%', fmt, L'\0', L'\0' };

#else   // if _RWSTD_NO_WCSFTIME_WCHAR_T_FMAT || _RWSTD_NO_WCSFTIME

            // work around incorrect wcsftime() declarations some
            // platforms
            char fmtstr [4] = { '%', fmt, '\0', '\0' };

#endif   // !_RWSTD_NO_WCSFTIME_WCHAR_T_FMAT && !_RWSTD_NO_WCSFTIME

            if (mod) {
                fmtstr [1] = mod;
                fmtstr [2] = fmt;
            }

#ifndef _RWSTD_NO_WCSFTIME

            // use wcsftime() for locale-independent formatting
            res = wcsftime (wbuf, bufsize, fmtstr, tmb);

            _RWSTD_ASSERT (res < bufsize);

#else   // if defined (_RWSTD_NO_WCSFTIME)

            char buf [256];

            // use strftime() for locale-independent formatting
            res = strftime (buf, sizeof buf, fmtstr, tmb);

            if (res <= sizeof buf) {
                // widen narrow (not multibyte) result into wide buffer
                for (_RWSTD_SIZE_T i = 0; i != res; ++i)
                    wbuf [i] = _RWSTD_STATIC_CAST (unsigned char, buf [i]);
            }

#endif   // _RWSTD_NO_WCSFTIME
        }
    }

    return wbuf + res;
}

#endif   // _RWSTD_NO_WCHAR_T


}   // namespace __rw
