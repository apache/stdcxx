/************************************************************************
 *
 * 22.locale.num.put.mt.cpp
 *
 * test exercising the thread safety of the num_put facet
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
 **************************************************************************/

#include <ios>        // for ios
#include <iterator>   // for ostreambuf_iterator
#include <locale>     // for locale, num_put

#include <cstring>    // for strlen()

#include <rw_locale.h>
#include <rw_thread.h>   // for rw_get_processors (), rw_thread_pool()
#include <driver.h>


// maximum number of threads allowed by the command line interface
#define MAX_THREADS   32

// default number of threads (will be adjusted to the number
// of processors/cores later)
int rw_opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int rw_opt_nloops = 200000;

/**************************************************************************/

// array of locale names to use for testing
static const char*
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

extern "C" {

bool test_char;    // exercise num_put<char>
bool test_wchar;   // exercise num_put<wchar_t>


static void*
thread_func (void*)
{
    // dummy streambuf-derived object the doesn't do anything
    // but allows ostreambuf_iterator to "think" it can write
    // to it
    struct NarrowBuf: std::streambuf {
        int_type overflow (int_type c) { return c; }
    } sb;

#ifndef _RWSTD_NO_WCHAR_T

    struct WideBuf: std::wstreambuf {
        int_type overflow (int_type c) { return c; }
    } wb;

#endif   // _RWSTD_NO_WCHAR_T

    struct Ios: std::ios {
        Ios () { this->init (0); }
    } io;

    const std::ios::fmtflags baseflags[] = {
        std::ios::oct,
        std::ios::dec,
        std::ios::hex
    };

    const std::ios::fmtflags fmtflags[] = {
        std::ios::showpos,
        std::ios::showpoint,
        std::ios::fixed,
        std::ios::scientific
    };

    const std::ios::fmtflags adjustflags[] = {
        std::ios::internal,
        std::ios::left,
        std::ios::right
    };

    for (int i = 0; i != rw_opt_nloops; ++i) {

        // save the name of the locale
        const char* const locale_name = locales [i % nlocales];

        // construct a named locale and imbue it in the ios object
        // so that the locale is used not only by the num_put facet
        // but also by the numpunct facet
        const std::locale loc (locale_name);
        io.imbue (loc);

        enum PutId {
            put_bool,
            put_long,
            put_ulong,

#ifndef _RWSTD_NO_LONG_LONG

            put_llong,
            put_ullong,

#endif   // _RWSTD_NO_LONG_LONG

            put_dbl,
            put_ldbl,
            put_ptr,
            put_max
        };

        const std::ios::fmtflags base =
            baseflags [i % (sizeof baseflags / sizeof *baseflags)];

        const std::ios::fmtflags fmt =
            fmtflags [i % (sizeof baseflags / sizeof *baseflags)];

        const std::ios::fmtflags adjust =
            adjustflags [i % (sizeof baseflags / sizeof *baseflags)];

        io.flags (base | fmt | adjust);

        io.width (i % 16);

        // exercise postive and negative values
        const int ival = i & 1 ? -i : i;

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::num_put<char> &np =
                std::use_facet<std::num_put<char> >(loc);

            const std::ostreambuf_iterator<char> iter (&sb);

            switch (i % put_max) {
            case put_bool:
                if (i & 2)
                    io.setf (std::ios::boolalpha);
                else
                    io.unsetf (std::ios::boolalpha);
                        
                np.put (iter, io, ' ', bool (ival));
                break;

            case put_long:
                np.put (iter, io, ' ', long (ival));
                break;

            case put_ulong:
                np.put (iter, io, ' ', (unsigned long)ival);
                break;

#ifndef _RWSTD_NO_LONG_LONG

            case put_llong:
                np.put (iter, io, ' ', (_RWSTD_LONG_LONG)ival);
                break;

#endif   // _RWSTD_NO_LONG_LONG

            case put_ullong:
                np.put (iter, io, ' ', (unsigned _RWSTD_LONG_LONG)ival);
                break;

            case put_dbl:
                np.put (iter, io, ' ', double (ival));
                break;

            case put_ldbl:
                np.put (iter, io, ' ', (long double)ival);
                break;

            case put_ptr:
                np.put (iter, io, ' ', (void*)ival);
                break;
            }
        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::num_put<wchar_t> &np =
                std::use_facet<std::num_put<wchar_t> >(loc);

            const std::ostreambuf_iterator<wchar_t> iter (&wb);

            switch (i % put_max) {
            case put_bool:
                np.put (iter, io, L' ', bool (ival));
                break;

            case put_long:
                np.put (iter, io, L' ', long (ival));
                break;

            case put_ulong:
                np.put (iter, io, L' ', (unsigned long)ival);
                break;

#ifndef _RWSTD_NO_LONG_LONG

            case put_llong:
                np.put (iter, io, L' ', (_RWSTD_LONG_LONG)ival);
                break;

            case put_ullong:
                np.put (iter, io, L' ', (unsigned _RWSTD_LONG_LONG)ival);
                break;

#endif   // _RWSTD_NO_LONG_LONG

            case put_dbl:
                np.put (iter, io, L' ', double (ival));
                break;

            case put_ldbl:
                np.put (iter, io, L' ', (long double)ival);
                break;

            case put_ptr:
                np.put (iter, io, L' ', (void*)ival);
                break;
            }

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
             "testing std::num_put<charT> with %d thread%{?}s%{;}, "
             "%zu iteration%{?}s%{;} each, in locales { %{ .*A@} }",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops,
             int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::num_put<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::num_put<wchar_t>");

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
             "exercising both std::num_put<char> and std::num_put<wchar_t>");

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
                    "lib.locale.num.put",
                    "thread safety", run_test,
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-* "    // must be in [0, MAX_THREADS]
                    "|-locales=",        // must be provided
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads,
                    &rw_opt_setlocales);
}
