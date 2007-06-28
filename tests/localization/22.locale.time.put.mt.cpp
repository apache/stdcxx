/************************************************************************
 *
 * 22.locale.time.put.mt.cpp
 *
 * test exercising the thread safety of the time_put facet
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
 * Copyright 2002-2007 Rogue Wave Software.
 * 
 **************************************************************************/

#include <ios>        // for ios
#include <iterator>   // for ostreambuf_iterator
#include <locale>     // for locale, time_put

#include <cstring>    // for strlen()
#include <ctime>      // for tm

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>


#define MAX_THREADS      32
#define MAX_LOOPS    100000

// default number of threads (will be adjusted to the number
// of processors/cores later)
int rw_opt_nthreads = 1;

// the number of times each thread should iterate
int rw_opt_nloops = MAX_LOOPS;

/**************************************************************************/

// array of locale names to use for testing
static const char*
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

extern "C" {

bool test_char;    // exercise time_put<char>
bool test_wchar;   // exercise time_put<wchar_t>


static void*
thread_func (void*)
{
    std::tm tmb = std::tm ();

    const char cvtspecs[] = "aAbBcCdDeFgGhHIjmMnprRStTuUVwWxXyYzZ%";

    // dummy streambuf-derived object the doesn't do anything
    // but allows ostreambuf_iterator to "think" it can write
    // to it
    struct NarrowBuf: std::streambuf {
        int_type overflow (int_type c) { return c; }
    } sb;

#ifndef _RWSTD_NO_WCHAR_T

    struct WideBuf: std::wstreambuf {
        int_type overflow (int_type c) { return c; }
    } wsb;

#endif   // _RWSTD_NO_WCHAR_T

    struct Ios: std::ios {
        Ios () { this->init (0); }
    } io;

    int j = 0;

    for (int i = 0; i != rw_opt_nloops; ++i, ++j) {

        // initialize tm with random but valid values
        tmb.tm_sec  = ++j % 61;
        tmb.tm_min  = ++j % 60;
        tmb.tm_min  = ++j % 60;
        tmb.tm_wday = ++j % 7;
        tmb.tm_mon  = ++j % 12;
        tmb.tm_year = ++j;

        // generate a "random" conversion specifier from the set
        // of valid specifiers recognized by the facet to exercise
        // all (or most) code paths
        const char cvt = cvtspecs [i % (sizeof cvtspecs - 1)];

        // save the name of the locale
        const char* const locale_name = locales [i % nlocales];

        // construct a named locale, get a reference to the time_put
        // facet from it and use it to format a random time value
        // using a random conversion specifier
        const std::locale loc (locale_name);

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::time_put<char> &tp =
                std::use_facet<std::time_put<char> >(loc);

            // format a "random" but valid tm value using the random
            // format specifier
            tp.put (std::ostreambuf_iterator<char>(&sb),
                    io, ' ', &tmb, cvt);

        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::time_put<wchar_t> &wtp =
                std::use_facet<std::time_put<wchar_t> >(loc);

            wtp.put (std::ostreambuf_iterator<wchar_t>(&wsb),
                     io, L' ', &tmb, cvt);

#endif   // _RWSTD_NO_WCHAR_T

        }
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    // find all installed locales for which setlocale(LC_ALL) succeeds
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);

    const std::size_t maxinx = sizeof locales / sizeof *locales;

    for (const char *name = locale_list; *name; name += std::strlen (name) +1) {
        locales [nlocales++] = name;

        if (nlocales == maxinx)
            break;
    }

    rw_info (0, 0, 0,
             "testing std::time_put<charT> with %d thread%{?}s%{;}, "
             "%zu iteration%{?}s%{;} each, in locales { %{ .*A@} }",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops,
             int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::time_put<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::time_put<wchar_t>");

    test_char  = false;
    test_wchar = true;

    // start a pool of threads to exercise wstring thread safety
    result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    // exercise bothe the char and the wchar_t specializations
    // at the same time

    rw_info (0, 0, 0,
             "exercising both std::time_put<char> and std::time_put<wchar_t>");

    test_char  = true;
    test_wchar = true;

    // start a pool of threads to exercise wstring thread safety
    result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#endif   // _RWSTD_NO_WCHAR_T

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
#ifdef _RWSTD_REENTRANT

    // set nthreads to the number of processors by default
    rw_opt_nthreads = rw_get_cpus ();

#endif   // _RWSTD_REENTRANT

    return rw_test (argc, argv, __FILE__,
                    "lib.locale.time.put",
                    "thread safety", run_test,
                    "|-nloops#0 "       // must be non-negative
                    "|-nthreads#0-* "   // must be in [0, MAX_THREADS]
                    "|-locales=",       // must be provided
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads,
                    &rw_opt_setlocales);
}
