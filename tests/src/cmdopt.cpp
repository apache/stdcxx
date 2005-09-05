/***************************************************************************
 *
 * $Id$
 *
 ************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#include <cmdopt.h>

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef EINVAL
#  define EINVAL   22   /* e.g., HP-UX, Linux, Solaris */
#endif   // EINVAL

/**************************************************************************/

typedef int (optcallback_t)(int, char*[]);

struct cmdopts_t
{
    char           loptbuf_ [32];   // buffer for long option name
    optcallback_t *callback_;       // function to call to process option
    int           *pcntr_;          // counter to increment for each occurrence

    char          *lopt_;           // long option name
    char           sopt_;           // short option name

    size_t         maxcalls_;       // how many times option can be invoked
    size_t         ncalls_;         // how many times it has been invoked

    bool           arg_ : 1;        // option takes an argument?
    bool           inv_ : 1;        // callback invocation inverted
    bool           envseen_ : 1;    // environment option already processed

};


// total number of registered options
static size_t ncmdopts;

// number of default (always defined) options
static size_t ndefopts;
static cmdopts_t cmdoptbuf [32];
static cmdopts_t *cmdopts = cmdoptbuf;
static size_t optbufsize = sizeof cmdoptbuf / sizeof *cmdoptbuf;

enum {
    rw_bool, rw_char, rw_uchar, rw_schar, rw_ushrt, rw_shrt,
    rw_uint, rw_int, rw_ulong, rw_long, rw_ullong, rw_llong,
    rw_flt, rw_dbl, rw_ldbl, rw_wchar, rw_pvoid
};

static int rw_disabled_types = 0;
static int rw_enabled_types  = 0;

struct linerange_t {
    int first;
    int last;
};

static size_t nlineranges;
static linerange_t *lineranges;
static size_t rangebufsize;

/**************************************************************************/

static int
rw_print_help (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Without an argument, prints this help to stdout and exits with\n"
            "a status of 0 without further processing.\n"
            "With the optional argument prints help on the option with that\n"
            "name, if one exists, and exits with a status of 0. If an option\n"
            "with the specified name does not exist, prints an error message\n"
            "to stderr and exits with a status of 1.\n"
            "The leading underscores in the name of an option are optional.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    // the option name to get help on, if any
    const char* opthelp = 1 < argc ? argv [1] : 0;

    // remove the optional one or two leading underscores
    if (opthelp && '-' == opthelp [0] && opthelp [1])
        opthelp += 1 + ('-' == opthelp [1]);

    if (0 == opthelp)
        printf ("OPTIONS\n");

    // set to a non-zero when the specified option is found
    int option_found = 0;

    for (size_t i = 0; i != ncmdopts; ++i) {

        if (opthelp && *opthelp) {

            if (   cmdopts [i].sopt_ == opthelp [0] && '\0' == opthelp [1]
                || cmdopts [i].lopt_
                && 0 == strcmp (cmdopts [i].lopt_ + 1, opthelp)) {

                // remember that we found the option whose (short
                // or long) name we're to give help on; after printing
                // the help text on the option keep looping in case
                // there is another option and callback with the same
                // name (unlikely but possible)
                option_found = 1;
            }
            else {
                // the option doesn't match, continue searching
                continue;
            }
        }

        printf ("     ");

        if (cmdopts [i].sopt_) {
            printf ("-%c", cmdopts [i].sopt_);

            if (cmdopts [i].lopt_)
                printf (" | ");
        }

        const char *pfx = "";
        const char *sfx = pfx;

        if (cmdopts [i].lopt_) {
            printf ("-%s", cmdopts [i].lopt_);
            if (   cmdopts [i].arg_
                && '=' != cmdopts [i].lopt_ [strlen (cmdopts [i].lopt_) - 1]) {
                pfx = " [ ";
                sfx = " ]";
            }
        }

        printf ("%s%s%s", pfx, cmdopts [i].arg_ ? "<arg>" : "", sfx);

        if (_RWSTD_SIZE_MAX == cmdopts [i].maxcalls_)
            printf (" (each occurrence evaluated)\n");
        else if (1 < cmdopts [i].maxcalls_)
            printf (" (at most %lu occurrences evaluated)\n",
                    cmdopts [i].maxcalls_);
        else
            printf (" (only the first occurrence evaluated)\n");

        // invoke callback with the "--help" option
        if (cmdopts [i].callback_) {

            char* help [2] = { 0, 0 };

            cmdopts [i].callback_ (1, help);

            for (const char *line = help [0]; line; ) {

                const char* const nl = strchr (line, '\n');
                const int len = nl ? int (nl - line) : int (strlen (line));

                printf ("       %.*s\n", len, line);

                line = nl;
                if (nl)
                    ++line;
            }
        }
    }

    if (opthelp && !option_found) {
        fprintf (stderr, "Unknown option \"%s\".\n", opthelp);
        exit (1);
    }

    exit (0);

    return 0;
}


static int
rw_set_ignenv (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Prevents options specified in the RWSTD_TESTOPTS environment\n"
            "variable from taking effect.\n"
            "Unless this option is specified, the RWSTD_TESTOPTS environment\n"
            "variable is processed as if its value were specified on the \n"
            "command line.\n"
            "For example, setting the value of the variable to the string\n"
            "\"--verbose --no-wchar\" and invoking this program with no\n"
            "command line arguments will have the same effect as invoking\n"
            "it with the two arguments on the command line.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    return 0;
}

static int rw_enable_type (int bit, bool enable)
{
    const int set_mask = 1 << bit;

    if (enable)
        rw_enabled_types |= set_mask;
    else
        rw_disabled_types |= set_mask;

    return 0;
}

#define DEFINE_HANDLERS(T)                               \
        static int rw_no_ ## T (int, char *[]) {         \
            return rw_enable_type (rw_ ## T, false);     \
        }                                                \
        static int rw_en_ ## T (int, char *[]) {         \
            return rw_enable_type (rw_ ## T, true);      \
        }                                                \
        typedef void unused_type /* allow a semicolon */


DEFINE_HANDLERS (bool);
DEFINE_HANDLERS (char);
DEFINE_HANDLERS (schar);
DEFINE_HANDLERS (uchar);
DEFINE_HANDLERS (shrt);
DEFINE_HANDLERS (ushrt);
DEFINE_HANDLERS (int);
DEFINE_HANDLERS (uint);
DEFINE_HANDLERS (long);
DEFINE_HANDLERS (ulong);
DEFINE_HANDLERS (llong);
DEFINE_HANDLERS (ullong);
DEFINE_HANDLERS (flt);
DEFINE_HANDLERS (dbl);
DEFINE_HANDLERS (ldbl);
DEFINE_HANDLERS (wchar);
DEFINE_HANDLERS (pvoid);

static int
rw_enable_lines (int first, int last, bool enable)
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

    if (enable) {
        lineranges [nlineranges].first = first;
        lineranges [nlineranges].last  = last;
    }
    else {
        lineranges [nlineranges].first = -first;
        lineranges [nlineranges].last  = -last;
    }

    ++nlineranges;

    return 0;
}


static int
rw_enable_line (int argc, char *argv[], bool enable)
{
    if (1 == argc && argv && 0 == argv [0]) {

        static const char helpstr[] = {
            "Enables or disables the lines specified by <arg>.\n"
            "The syntax of <arg> is as follows: \n"
            "<arg>   ::= <range> [ , <range> ]\n"
            "<range> ::= [ - ] <number> | <number> - [ <number> ]\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    char *parg = strchr (argv [0], '=');
    assert (0 != parg);

    const char* const argbeg = ++parg;

    // the lower bound of a range of lines to be enabled or disabled
    // negative values are not valid and denote an implicit lower bound
    // of 1 (such as in "-3" which is a shorthand for "1-3")
    long first = -1;

    for ( ; '\0' != *parg ; ) {

        printf ("%s (%ld)\n", parg, first);

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
            printf ("  [%ld, %ld)\n", first, line);
            rw_enable_lines (first, line, enable);
            first = -1;
        }
    }

    for (size_t i = 0; i != nlineranges; ++i) {
        printf ("[%d, %d)\n", lineranges [i].first, lineranges [i].last);
    }

    return 0;
}

static int rw_en_line (int argc, char *argv[])
{
    return rw_enable_line (argc, argv, true);
}

static int rw_no_line (int argc, char *argv[])
{
    return rw_enable_line (argc, argv, false);
}

static void rw_set_myopts ()
{
    if (0 != ncmdopts)
        return;

    static bool recursive;

    if (recursive)
        return;

    recursive = true;

    rw_setopts ("|-help: "   // argument optional
                "|-ignenv "
                // options to disable fundamental types
                "|-no-bool |-no-char |-no-schar |-no-uchar "
                "|-no-shrt |-no-ushrt "
                "|-no-int |-no-uint "
                "|-no-long |-no-ulong "
                "|-no-llong |-no-ullong "
                "|-no-flt |-no-dbl |-no-ldbl "
                "|-no-wchar |-no-pvoid "
                // option to disable ranges of lines
                "|-no-line= "
                // options to enable fundamental types
                "|-enable-bool |-enable-char |-enable-schar |-enable-uchar "
                "|-enable-shrt |-enable-ushrt "
                "|-enable-int |-enable-uint "
                "|-enable-long |-enable-ulong "
                "|-enable-llong |-enable-ullong "
                "|-enable-flt |-enable-dbl |-enable-ldbl "
                "|-enable-wchar |-enable-pvoid "
                // option to enable ranges of lines
                "|-enable-line= ",
                //
                rw_print_help,
                rw_set_ignenv,
                // handlers to disable fundamental types
                rw_no_bool, rw_no_char, rw_no_schar, rw_no_uchar,
                rw_no_shrt, rw_no_ushrt,
                rw_no_int, rw_no_uint,
                rw_no_long, rw_no_ulong,
                rw_no_llong, rw_no_ullong,
                rw_no_flt, rw_no_dbl, rw_no_ldbl,
                rw_no_wchar, rw_no_pvoid,
                rw_no_line,
                // handlers to enable fundamental types
                rw_en_bool, rw_en_char, rw_en_schar, rw_en_uchar,
                rw_en_shrt, rw_en_ushrt,
                rw_en_int, rw_en_uint,
                rw_en_long, rw_en_ulong,
                rw_en_llong, rw_en_ullong,
                rw_en_flt, rw_en_dbl, rw_en_ldbl,
                rw_en_wchar, rw_en_pvoid,
                rw_en_line,
                (optcallback_t*)0);

    ndefopts = ncmdopts;

    recursive = false;
}


//////////////////////////////////////////////////////////////////////
// syntax of the option description string:
//
// opts ::= opt [ ':' | '=' | '#' ] [ @N | @* | '!' ] [ opts ]
// opt  ::= <sopt> [ '|' <lopt>]
//      ::= '|' <lopt>
// sopt ::= char
// lopt ::= char char*
// char ::= any character other than a space (' '), bar ('|'),
//          colon (':') and the equals sign ('=')

_TEST_EXPORT int
rw_vsetopts (const char *opts, va_list va)
{
    if (0 == opts) {

        // reset to 0

        for (size_t i = 0; i != ncmdopts; ++i) {
            if (cmdopts [i].lopt_ != cmdopts [i].loptbuf_)
                free (cmdopts [i].lopt_);
        }

        if (cmdopts != cmdoptbuf)
            free (cmdopts);

        cmdopts  = cmdoptbuf;
        ncmdopts = 0;

        return 0;
    }

    rw_set_myopts ();

    const char *next = opts;

    for ( ; ; ++ncmdopts) {

        while (' ' == *next)
            ++next;

        if ('\0' == *next) {
            break;
        }

        if (ncmdopts == optbufsize) {

            const size_t newbufsize = 2 * ncmdopts + 1;
            
            cmdopts_t* const newopts =
                (cmdopts_t*)malloc (newbufsize * sizeof (cmdopts_t));

            if (0 == newopts) {
                abort ();
            }

            memcpy (newopts, cmdopts, ncmdopts * sizeof (cmdopts_t));

            if (cmdopts != cmdoptbuf)
                free (cmdopts);

            cmdopts    = newopts;
            optbufsize = newbufsize;
        }

        memset (cmdopts + ncmdopts, 0, sizeof *cmdopts);

        if ('|' != *next)
            cmdopts [ncmdopts].sopt_ = *next++;

        if ('|' == *next) {
            const char* end = strpbrk (++next, "|@:=*!# ");
            if (0 == end)
                end = next + strlen (next);

            // copy the option name up to but not including the delimiter
            // (except when the delimiter is the equals sign ('='), which
            // becomes the last character of the option name
            const size_t optlen = size_t (end - next) + ('=' == *end);

            if (optlen < sizeof cmdopts [ncmdopts].loptbuf_)
                cmdopts [ncmdopts].lopt_ = cmdopts [ncmdopts].loptbuf_;
            else
                cmdopts [ncmdopts].lopt_ = (char*)malloc (optlen + 1);

            memcpy (cmdopts [ncmdopts].lopt_, next, optlen);
            cmdopts [ncmdopts].lopt_ [optlen] = '\0';

            next = end;
        }

        // only the first occurrence of each command line option
        // causes an invocation of the callback, all subsequent
        // ones will be ignored by default
        cmdopts [ncmdopts].maxcalls_ = 1;

        bool arg_is_callback = true;

        if (':' == *next || '=' == *next) {
            // ':' : argument optional
            // '=' : argument required
            cmdopts [ncmdopts].arg_ = true;
            ++next;
        }
        else if ('#' == *next) {
            // insead of a pointer to a callback, the argument
            // is a pointer to an int counter that is to be
            // incremented for each occurrence of the option
            // during processing
            arg_is_callback = false;
            ++next;
            
            // an unlimited number of occurrences of the option
            // are allowed and will be counted
            cmdopts [ncmdopts].maxcalls_ = _RWSTD_SIZE_MAX;
        }

        if ('@' == *next) {

            ++next;

            // at most how many occurrences of an option can be processed?
            if ('*' == *next) {
                // unlimited
                cmdopts [ncmdopts].maxcalls_ = _RWSTD_SIZE_MAX;
                ++next;
            }
            else {
                // at most this many
                char *end;
                cmdopts [ncmdopts].maxcalls_ = strtoul (next, &end, 10);
                next = end;
            }
        }
        else if ('!' == *next) {
            cmdopts [ncmdopts].inv_ = true;
            ++next;
        }

        if (arg_is_callback) {
            // retrieve the callback and verify it's not null
            // (null callback is permitted in the special case when
            // the short option is '-', i.e., when setting up or
            // resetting an "unknown option" handler)
            cmdopts [ncmdopts].callback_ = va_arg (va, optcallback_t*);
        }
        else {
            // retrieve the address of the int counter where to keep
            // track of the number of occurrences of the option
            cmdopts [ncmdopts].pcntr_ = va_arg (va, int*);
        }

        if (   '-' != cmdopts [ncmdopts].sopt_
            && 0 == cmdopts [ncmdopts].callback_
            && 0 == cmdopts [ncmdopts].pcntr_) {

            if (cmdopts [ncmdopts].lopt_)
                fprintf (stderr, "null handler for option -%s\n",
                         cmdopts [ncmdopts].lopt_);
            else
                fprintf (stderr, "null handler for option -%c\n",
                         cmdopts [ncmdopts].sopt_);
                
            abort ();
        }
    }

    return int (ncmdopts - ndefopts);
}


_TEST_EXPORT int
rw_setopts (const char *opts, ...)
{
    va_list va;
    va_start (va, opts);
    const int result = rw_vsetopts (opts, va);
    va_end (va);
    return result;
}


_TEST_EXPORT int
rw_runopts (int argc, char *argv[])
{
    rw_set_myopts ();

    static bool recursive = false;

    // ignore options set in the environment?
    bool ignenv = recursive;

    // return status
    int status = 0;

    // number of options processed
    int nopts = 0;

    // index of registered option whose callback should be invoked
    // for command line options that do not match any other
    size_t not_found_inx = _RWSTD_SIZE_MAX;

    // iterate over the command line arguments until a callback
    // returns a non-zero value or until all options have been
    // successfully processed
    for (int i = 0; i < argc && argv [i] && 0 == status; ++i) {

        if (0 == strcmp ("--ignore-environment", argv [i])) {
            // ignore options set in the environment
            ignenv = true;
            continue;
        }

        if (0 == strcmp ("--", argv [i])) {
            // "--" terminates options, everything
            // after it is treated as an argument
            break;
        }

        bool found = false;

        // look up each command line option (i.e., a string that starts
        // with a dash ('-')) and invoke the callback associated with it
        for (size_t j = 0; j != ncmdopts; ++j) {

            if ('-' == cmdopts [j].sopt_)
                not_found_inx = j;

            if ('-' == argv [i][0]) {

                // the name of the option without the leading dash
                const char* const optname = argv [i] + 1;

                // look for the first equals sign
                const char* const eq = strchr (optname, '=');

                // compute the length of the ooption including the equals
                // sign (if any)
                const size_t optlen =
                    eq ? size_t (eq - optname + 1) : strlen (optname);

                // try to match the long option first, and only if it
                // doesn't match try the short single-character option
                if (   cmdopts [j].lopt_
                    && optlen == strlen (cmdopts [j].lopt_)
                    && 0 == memcmp (optname, cmdopts [j].lopt_, optlen)
                    || cmdopts [j].sopt_
                    && optname [0] == cmdopts [j].sopt_
                    && (1 == optlen || cmdopts [j].arg_)) {

                    // matching option has been found
                    found = true;

                    // ignore the option if invoked recursively (by processing
                    // options set in the environment) and the option has
                    // already been seen (this prevents duplicate processing
                    // of options that are set both on the command line and
                    // in the environment and allows option with an argument
                    // set on the command line to override those set in the
                    // environment)

                    if (cmdopts [j].ncalls_ && recursive)
                        continue;

                    // if the option has been evaluated the maximum number
                    // of times, avoid evaluating it and continue processing
                    if (cmdopts [j].maxcalls_ <= cmdopts [j].ncalls_)
                        continue;

                    if (cmdopts [j].callback_) {
                        if (!cmdopts [j].inv_) {
                            // when the command line argument matched
                            // the option,  invoke the callback function
                            status = cmdopts [j].callback_ (argc - i, argv + i);
                        }
                    }
                    else {
                        assert (0 != cmdopts [j].pcntr_);
                        ++*cmdopts [j].pcntr_;
                    }

                    ++cmdopts [j].ncalls_;

                    if (recursive)
                        cmdopts [j].envseen_ = true;

                    ++nopts;

                    if (status) {
                        // when the status returned from the last callback
                        // is non-0 stop further processing (including
                        // any options set in the environment) and return
                        // status to the caller
                        ignenv = true;
                        break;
                    }
                }
            }
        }

        if (!found && '-' == argv [i][0]) {

            // invoke the appropriate error handler for an option
            // that was not found
            if (_RWSTD_SIZE_MAX != not_found_inx) {

                // invoke the error handler set up through rw_setopts()
                // and let the handler decide whether to go on processing
                // other options or whether to abort
                status = cmdopts [not_found_inx].callback_ (argc - i, argv + i);
                if (status) {
                    // no further processing done
                    ignenv = true;
                    break;
                }
            }
            else {
                // print an error message to stderr when no error
                // handler has been set up
                fprintf (stderr, "unknown option: %s\n", argv [i]);
                ignenv = true;
                errno  = EINVAL;
                status = 1;
                break;
            }
        }
    }

    if (!ignenv) {
        // process options from the environment
        const char* const envar = getenv ("RWSTD_TESTOPTS");
        if (envar) {
            recursive = true;
            rw_runopts (envar);
            recursive = false;
        }
    }

    // invoke any inverted callbacks or bump their user-specified counters,
    // and reset internal counters indicating if/how many times each option
    // has been processed
    for (size_t j = 0; j != ncmdopts; ++j) {

        if (cmdopts [j].inv_ && 0 == cmdopts [j].ncalls_ && 0 == status) {

            if (cmdopts [j].callback_)
                status = cmdopts [j].callback_ (0, 0);
            else {
                assert (0 != cmdopts [j].pcntr_);
                ++*cmdopts [j].pcntr_;
            }
        }

        cmdopts [j].ncalls_  = 0;
        cmdopts [j].envseen_ = false;
    }

    return status;
}


_TEST_EXPORT int
rw_runopts (const char *str)
{
    assert (0 != str);

    rw_set_myopts ();

    char buf [80];      // fixed size buffer to copy `str' into
    char *pbuf = buf;   // a modifiable copy of `str'

    size_t len = strlen (str);
    if (len >= sizeof buf) {
        // allocate if necessary
        pbuf = (char*)malloc (len + 1);
        if (!pbuf)
            return -1;
    }

    // copy `str' to modifiable buffer
    memcpy (pbuf, str, len + 1);

    char *tmp_argv_buf [32] = { 0 };   // fixed size argv buffer
    char **argv = tmp_argv_buf;        // array of arguments

    // initial size of argument array (will grow as necessary)
    size_t argv_size = sizeof tmp_argv_buf / sizeof *tmp_argv_buf;
    size_t argc = 0;   // number of arguments in array

    int in_quotes = 0;   // quoted argument being processed

    for (char *s = pbuf; *s; ++s) {
        if ('"' == *s) {
            in_quotes = !in_quotes;
            continue;
        }

        if (in_quotes)
            continue;

        // split up unquoted space-separated arguments
        if (argc == 0 || ' ' == *s) {
            if (argc > 0) 
                *s = 0;

            // skip over leading spaces
            if (argc > 0 || ' ' == *s)
                while (' ' == *++s);

            if (*s) {
                if (argc == argv_size) {
                    // grow `argv' as necessary
                    char **tmp = (char**)malloc (sizeof *tmp * argv_size * 2);
                    if (!tmp) {
                        if (argv != tmp_argv_buf)
                            free (argv);
                        return -1;
                    }

                    // copy existing elementes and zero out any new entries
                    memcpy (tmp, argv, sizeof *tmp * argv_size);
                    memset (tmp + argv_size, 0, sizeof *tmp * argv_size);

                    // free existing buffer if necessary
                    if (argv != tmp_argv_buf)
                        free (argv);

                    // reassign buffer and increase size
                    argv       = tmp;
                    argv_size *= 2;
                }

                // add argument to array
                argv [argc++] = s;
            }
        }
    }

    // process `argc' options pointed to by `argv'
    const int status = rw_runopts (int (argc), argv);

    // free buffers if necessary
    if (argv != tmp_argv_buf)
        free (argv);

    if (pbuf != buf)
        free (pbuf);

    return status;

}


_TEST_EXPORT int
rw_enabled (const char *name)
{
    int enabled = -1;

    static const struct {
        int bit;
        const char *name;
    } types[] = {
        { rw_bool, "bool" },

        { rw_char, "char" },

        { rw_schar, "schar" },
        { rw_schar, "signed char" },
        { rw_uchar, "uchar" },
        { rw_uchar, "unsigned char" },

        { rw_shrt, "shrt" },
        { rw_shrt, "short" },
        { rw_shrt, "signed short" },
        { rw_ushrt, "ushrt" },
        { rw_ushrt, "unsigned short" },

        { rw_int, "int" },
        { rw_int, "signed int" },
        { rw_uint, "uint" },
        { rw_uint, "unsigned int" },

        { rw_long, "long" },
        { rw_long, "signed long" },
        { rw_ulong, "ulong" },
        { rw_ulong, "unsigned long" },

        { rw_llong, "llong" },
        { rw_llong, "long long" },
        { rw_llong, "signed long long" },
        { rw_ullong, "ullong" },
        { rw_ullong, "unsigned long long" },

        { rw_flt, "flt" },
        { rw_flt, "float" },

        { rw_dbl, "dbl" },
        { rw_dbl, "double" },

        { rw_ldbl, "ldbl" },
        { rw_ldbl, "long double" },

        { rw_wchar, "wchar" },
        { rw_wchar, "wchar_t" },

        { rw_pvoid, "pvoid" },
        { rw_pvoid, "void*" }
    };

    for (size_t i = 0; i != sizeof types / sizeof *types; ++i) {
        if (0 == strcmp (types [i].name, name)) {
            const int mask = 1 << types [i].bit;

            enabled = !(rw_disabled_types & mask);

            if (rw_enabled_types)
                enabled = enabled && (rw_enabled_types & mask);

            break;
        }
    }

    return enabled;
}


_TEST_EXPORT int
rw_enabled (int line)
{
    int nenabled = 0;
    int ndisabled = 0;

    int line_enabled = -1;

    for (size_t i = 0; i != nlineranges; ++i) {

        const int first = lineranges [i].first;
        const int last  = lineranges [i].last;

        if (first < 0) {
            line = -line;
            ++ndisabled;
        }
        else {
            ++nenabled;
        }

        if (lineranges [i].first <= line && line < lineranges [i].last)
            line_enabled = 0 < line;
    }

    if (nenabled && -1 == line_enabled)
        line_enabled = 0;

    return line_enabled;
}
