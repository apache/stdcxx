/************************************************************************
 *
 * opt_lines.cpp - definitions of line option handlers
 *
 * $Id:$
 *
 ************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx, Inc. All Rights Reserved.
 * 
 * This computer software  is owned by Quovadx, Inc.  and is protected by
 * U.S.  copyright laws  and other  laws and  by  international treaties.
 * This computer  software is furnished  by Quovadx, Inc., pursuant  to a
 * written license  agreement and may  be used, copied,  transmitted, and
 * stored only in accordance with the terms of such license agreement and
 * with  the inclusion  of  the above  copyright  notice.  This  computer
 * software or any other copies  thereof may not be provided or otherwise
 * made available to any other person.
 * 
 * 
 * U.S. Government Restricted Rights.
 * 
 * This computer software: (a) was developed at private expense and is in
 * all respects the proprietary information of Quovadx, Inc.; (b) was not
 * developed with  government funds;  (c) is a  trade secret  of Quovadx,
 * Inc. for all purposes of the  Freedom of Information Act; and (d) is a
 * commercial item  and thus, pursuant  to Section 12.212 of  the Federal
 * Acquisition  Regulations (FAR) and  DFAR Supplement  Section 227.7202,
 * Government's use,  duplication or disclosure of  the computer software
 * is subject to the restrictions set forth by Quovadx, Inc.
 *
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include "opt_lines.h"

#include <cmdopt.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum {
    l_enabled  = 1,   // line is enabled
    l_disabled = 2,   // line is disabled
    l_expected = 4    // diagnostic on line is expected to be active
};

struct linerange_t {
    int      first;
    int      last;
    unsigned flags: 8;
    char     id [80];
};

static size_t nlineranges;
static linerange_t *lineranges;
static size_t rangebufsize;

/**************************************************************************/

static int
_rw_enable_lines (int first, int last, int flags)
{
    if (nlineranges == rangebufsize) {
        const size_t newbufsize = 2 * nlineranges + 1;
            
        linerange_t* const newranges =
            (linerange_t*)malloc (newbufsize * sizeof (linerange_t));

        if (0 == newranges) {
            abort ();
        }

        memcpy (newranges, lineranges, nlineranges * sizeof (linerange_t));

        free (lineranges);

        lineranges   = newranges;
        rangebufsize = newbufsize;
    }

    lineranges [nlineranges].first = first;
    lineranges [nlineranges].last  = last;

    lineranges [nlineranges].flags = flags;

    ++nlineranges;

    return 0;
}


static int
_rw_enable_line (int argc, char *argv[], int flags)
{
    char *parg = strchr (argv [0], '=');
    assert (0 != parg);

    const char* const argbeg = ++parg;

    // the lower bound of a range of lines to be enabled or disabled
    // negative values are not valid and denote an implicit lower bound
    // of 1 (such as in "-3" which is a shorthand for "1-3")
    long first = -1;

    for ( ; '\0' != *parg ; ) {

        // skip any leading whitespace
        for ( ; ' ' == *parg; ++parg);

        if ('-' == *parg) {
            if (first < 0) {
                first = 0;
                ++parg;
            }
            else {
                fprintf (stderr,
                         "invalid character '%c' at position %d: \"%s\"\n",
                         *parg, int (parg - argbeg), argv [0]);
                return 2;
            }
        }

        // parse a numeric argument
        char *end;
        long line = strtol (parg, &end, 0);

        // skip any trailing whitespace
        for ( ; ' ' == *end; ++end);

        if (end == parg || '-' != *end && ',' != *end && '\0' != *end) {
            fprintf (stderr,
                     "invalid character '%c' at position %d: \"%s\"\n",
                     *end, int (parg - argbeg), argv [0]);
            return 2;
        }

        if (0 <= first) {
            if (line < 0) {
                fprintf (stderr,
                         "invalid value %ld at position %d: \"%s\"\n",
                         line, int (parg - argbeg), argv [0]);
                return 2;
            }

            ++line;

            if ((',' == *end || '-' == *end) && end [1])
                ++end;
        }
        else if (',' == *end) {
            first = line++;
            if ('\0' == end [1]) {
                fprintf (stderr,
                         "invalid character '%c' at position %d: \"%s\"\n",
                         *end, int (parg - argbeg), argv [0]);
                return 2;
            }

            ++end;
        }
        else if ('-' == *end) {
            first = line;
            while (' ' == *++end);
            if ('\0' == *end) {
                line = _RWSTD_INT_MAX;
            }
            else if  (',' == *end) {
                line = _RWSTD_INT_MAX;
                ++end;
            }
            else
                line = -1;
        }
        else if ('\0' == *end) {
            first = line++;
        }
        else {
            fprintf (stderr,
                     "invalid character '%c' at position %d: \"%s\"\n",
                     *end, int (parg - argbeg), argv [0]);
            return 2;
        }

        parg = end;

        if (0 <= first && first < line) {
            _rw_enable_lines (first, line, flags);
            first = -1;
        }
    }

    return 0;
}

/**************************************************************************/

static int
_rw_opt_enable_line (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {

        static const char helpstr[] = {
            "Enables the line or lines specified by <arg>.\n"
            "The syntax of <arg> is as follows: \n"
            "<arg>   ::= <range> [ , <range> ]\n"
            "<range> ::= [ - ] <number> | <number> - [ <number> ]\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    return _rw_enable_line (argc, argv, l_enabled);
}

/**************************************************************************/

static int
_rw_opt_no_line (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {

        static const char helpstr[] = {
            "Disables the line or lines specified by <arg>.\n"
            "The syntax of <arg> is as follows: \n"
            "<arg>   ::= <range> [ , <range> ]\n"
            "<range> ::= [ - ] <number> | <number> - [ <number> ]\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    return _rw_enable_line (argc, argv, l_disabled);
}

/**************************************************************************/

static int
_rw_opt_expect_line (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {

        static const char helpstr[] = {
            "Marks the line or lines specified by <arg> as \"expected\".\n"
            "Inactive diagnostics on such lines will be issued as unexpected.\n"
            "The syntax of <arg> is as follows: \n"
            "<arg>   ::= <range> [ , <range> ]\n"
            "<range> ::= [ - ] <number> | <number> - [ <number> ]\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    return _rw_enable_line (argc, argv, l_expected);
}

/************************************************************************/

/* extern */ int
_rw_setopts_lines ()
{
    const int result =
        rw_setopts ("|-enable-line="   // argument required
                    "|-expect= "       // argument required
                    "|-no-line= ",     // argument required
                    _rw_opt_enable_line,
                    _rw_opt_expect_line,
                    _rw_opt_no_line);

    return result;
}

/************************************************************************/

/* extern */ int
_rw_expected (int line)
{
    int line_expected = 0;

    for (size_t i = 0; i != nlineranges; ++i) {

        const int first = lineranges [i].first;
        const int last  = lineranges [i].last;

        if (lineranges [i].flags & (l_disabled | l_enabled)) {
            continue;
        }

        if (first <= line && line < last)
            line_expected = 0 != (lineranges [i].flags & l_expected);
    }

    return line_expected;
}

/************************************************************************/

_TEST_EXPORT int
rw_enabled (int line)
{
    int nenabled = 0;
    int ndisabled = 0;

    int line_enabled = -1;

    for (size_t i = 0; i != nlineranges; ++i) {

        const int first = lineranges [i].first;
        const int last  = lineranges [i].last;

        if (lineranges [i].flags & l_disabled) {
            ++ndisabled;
        }
        else if (lineranges [i].flags & l_enabled) {
            ++nenabled;
        }
        else {
            continue;
        }

        if (first <= line && line < last)
            line_enabled = 0 != (lineranges [i].flags & l_enabled);
    }

    if (nenabled && -1 == line_enabled)
        line_enabled = 0;

    return line_enabled;
}
