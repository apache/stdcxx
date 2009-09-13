/************************************************************************
 *
 * 22.locale.messages.mt.cpp
 *
 * test exercising the thread safety of the messages facet
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

#include <locale>

#include <rw_driver.h>   // for rw_test()
#include <rw_locale.h>   // for rw_create_catalog()
#include <rw_thread.h>   // for rw_thread_pool()
#include <rw_process.h>  // for rw_system()
#include <rw_file.h>     // for SHELL_RM_F
#include <rw_valcmp.h>   // for rw_strncmp ()

#include <cstring>   // for strlen()
#include <cstdio>    // for remove()

// maximum number of threads allowed by the command line interface
#define MAX_CATALOGS     32
#define MAX_THREADS      32
#define MAX_LOOPS    100000

// deault number of catalogs
int opt_ncatalogs = 11;

// default number of threads (will be adjusted to the number
// of processors/cores later)
int opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int opt_nloops = 10000;

#if !defined (_RWSTD_OS_HP_UX) || defined (_ILP32)

// number of locales to use
int opt_nlocales = MAX_THREADS;

#else   // HP-UX in LP64 mode

// work around a small cache size on HP-UX in LP64 mode
// in LP64 mode (see STDCXX-812)
int opt_nlocales = 9;

#endif   // HP-UX 32/64 bit mode

// should all threads share the same set of locale objects instead
// of creating their own?
int opt_shared_locale;

// default timeout used by each threaded section of this test
int opt_timeout = 60;

/**************************************************************************/

// array of locale names to use for testing
static const char*
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

//
struct MyMessageData
{
    // name of the locale the data corresponds to
    const char* locale_name_;

    // optionally set to the named locale for threads to share
    std::locale locale_;

} my_message_data [MAX_THREADS];

char my_catalog_names [64][MAX_CATALOGS];

/**************************************************************************/

#define MAX_SETS 5
#define MAX_MESSAGES  5


int msg_id (int set, int id)
{
#ifdef _WIN32

    return (set - 1) * 5 + id;

#else

    _RWSTD_UNUSED (set);
    return id;

#endif
}

/**************************************************************************/

static const char* const
messages [MAX_SETS][MAX_MESSAGES] = {
    { "First set, first message",
      "First set, second message",
      "First set, third message",
      "First set, fourth message",
      "First set, fifth message"
    },
    { "Second set, first message",
      "Second set, second message",
      "Second set, third message",
      "Second set, fourth message",
      "Second set, fifth message"
    },
    { "Third set, first message",
      "Third set, second message",
      "Third set, third message",
      "Third set, fourth message",
      "Third set, fifth message"
    },
    { "Fourth set, first message",
      "Fourth set, second message",
      "Fourth set, third message",
      "Fourth set, fourth message",
      "Fourth set, fifth message"
    },
    { "Fifth set, first message",
      "Fifth set, second message",
      "Fifth set, third message",
      "Fifth set, fourth message",
      "Fifth set, fifth message"
    }
};

/**************************************************************************/

extern "C" {

bool test_char;    // exercise messages<char>
bool test_wchar;   // exercise messages<wchar_t>

static void*
thread_func (void*)
{
    const std::string ndflt ("\1\2\3\4");

    struct {
        std::messages_base::catalog cat;
        std::locale loc;
    } ncatalogs [4];

    const unsigned n_ncatalogs = sizeof (ncatalogs) / sizeof (*ncatalogs);

    // 22.2.7.1.2 says values less than 0 returned if catalog can't
    // be opened, so we use -1 as a sentinel.
    for (unsigned c = 0; c < n_ncatalogs; ++c)
        ncatalogs [c].cat = -1;

#ifndef _RWSTD_NO_WCHAR_T

    const std::wstring wdflt (L"\1\2\3\4");

    struct {
        std::messages_base::catalog cat;
        std::locale loc;
    } wcatalogs [5];

    const unsigned n_wcatalogs = sizeof (wcatalogs) / sizeof (*wcatalogs);

    for (unsigned c = 0; c < n_wcatalogs; ++c)
        wcatalogs [c].cat = -1;

#endif   // _RWSTD_NO_WCHAR_T

    ///////////////////////////////////////////////////////////////////////

    for (int i = 0; i < opt_nloops; ++i) {

        if (rw_thread_pool_timeout_expired ())
            break;

        const MyMessageData& data = my_message_data [i % nlocales];

        // construct a named locale, get a reference to the money_get
        // facet from it and use it to format a random money value
        const std::locale loc = 
            opt_shared_locale ? data.locale_
                              : std::locale (data.locale_name_);

        const int set   = 1 + i % MAX_SETS;
        const int msgid = 1 + i % MAX_MESSAGES;

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::messages<char> &nm =
                std::use_facet<std::messages<char> >(loc);

            const unsigned cat_idx = i % n_ncatalogs;
            if (! (ncatalogs [cat_idx].cat < 0)) {
                (nm.close)(ncatalogs [cat_idx].cat);
            }

            const unsigned name_idx = i % opt_ncatalogs;

            const std::messages_base::catalog cat = 
                (nm.open)(my_catalog_names [name_idx], loc);

            ncatalogs [cat_idx].cat = cat;
            ncatalogs [cat_idx].loc = loc;

            if (i & 1) {

                // get a message from the catalog every odd iteration
                const std::messages<char>::string_type res =
                        nm.get (cat, set, msg_id (set, msgid), ndflt);

            }
        }

#ifndef _RWSTD_NO_WCHAR_T

        if (test_wchar) {
            // exercise the wide char specialization of the facet

            const std::messages<wchar_t> &wm =
                std::use_facet<std::messages<wchar_t> >(loc);

            const unsigned cat_idx = i % n_wcatalogs;
            if (! (wcatalogs [cat_idx].cat < 0)) {
                (wm.close)(wcatalogs [cat_idx].cat);
            }

            const unsigned name_idx = i % opt_ncatalogs;

            const std::messages_base::catalog cat = 
                (wm.open)(my_catalog_names [name_idx], loc);

            RW_ASSERT (! (cat < 0));

            wcatalogs [cat_idx].cat = cat;
            wcatalogs [cat_idx].loc = loc;

            if (! (i & 1)) {

                // get a message from the catalog every even iteration
                const std::messages<wchar_t>::string_type res =
                        wm.get (cat, set, msg_id (set, msgid), wdflt);

            }
        }

#endif   // _RWSTD_NO_WCHAR_T

    }

    ///////////////////////////////////////////////////////////////////////

    // close any catalogs that are still open

    for (unsigned c = 0; c < n_ncatalogs; ++c) {
        if (! (ncatalogs [c].cat < 0)) {

            const std::messages<char> &nm =
                std::use_facet<std::messages<char> >(ncatalogs[c].loc);

            (nm.close)(ncatalogs [c].cat);
        }
    }

#ifndef _RWSTD_NO_WCHAR_T

    for (unsigned c = 0; c < n_wcatalogs; ++c) {
        if (! (wcatalogs [c].cat < 0)) {

            const std::messages<wchar_t> &wm =
                std::use_facet<std::messages<wchar_t> >(wcatalogs[c].loc);

            (wm.close)(wcatalogs [c].cat);
        }
    }

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    std::string catalog;

    // initialize the catalog data
    for (int i = 0; i < MAX_SETS; ++i) {

        for (int j = 0; j < MAX_MESSAGES; ++j) {
            catalog.append (messages [i][j],
                            std::strlen (messages [i][j]) + 1);
        }

        catalog.append (1, '\0');
    }

    ///////////////////////////////////////////////////////////////////////

    // create the catalogs and initialize array of catalog names
    for (int i = 0; i < opt_ncatalogs; ++i) {

        char* msg_name = my_catalog_names [i];

#ifndef _WIN32
        std::sprintf (msg_name, "rwstdmessages_%d.msg", i);
#else
        std::sprintf (msg_name, "rwstdmessages_%d.rc", i);
#endif

        const int failed = rw_create_catalog (msg_name, catalog.c_str ());
        rw_fatal (!failed, 0, __LINE__,
                  "failed to create message catalog from %s",
                  msg_name);

#ifndef _WIN32
        std::sprintf (msg_name, "./rwstdmessages_%d.cat", i);
#else
        std::sprintf (msg_name, "rwstdmessages_%d.dll", i);
#endif
    }

    ///////////////////////////////////////////////////////////////////////

    // find all installed locales for which setlocale (LC_ALL) succeeds
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);

    const std::size_t maxinx = RW_COUNT_OF (locales);

    for (const char* name = locale_list;
         *name;
         name += std::strlen (name) + 1) {

        const std::size_t inx = nlocales;
        locales [inx] = name;

        // fill in the money and results for this locale
        MyMessageData& data = my_message_data [inx];
        data.locale_name_ = name;

        try {
            const std::locale loc (data.locale_name_);

            {
                const std::messages<char>& nm =
                    std::use_facet<std::messages<char> >(loc);

                _RWSTD_UNUSED (nm);
            }

#ifndef _RWSTD_NO_WCHAR_T

            {
                const std::messages<wchar_t>& wm =
                    std::use_facet<std::messages<wchar_t> >(loc);

                _RWSTD_UNUSED (wm);
            }

#endif // _RWSTD_NO_WCHAR_T

            if (opt_shared_locale)
                data.locale_ = loc;

            nlocales += 1;

        }
        catch (...) {
            rw_warn (!rw_opt_locales, 0, __LINE__,
                     "unable to use locale(%#s)", name);
        }

        if (nlocales == maxinx || nlocales == std::size_t (opt_nlocales))
            break;
    }

    rw_info (0, 0, 0,
             "testing std::messages<charT> with %d thread%{?}s%{;}, "
             "%d iteration%{?}s%{;} each",
             opt_nthreads, 1 != opt_nthreads,
             opt_nloops, 1 != opt_nloops);

    ///////////////////////////////////////////////////////////////////////

    test_char   = true;
    test_wchar  = false;

    rw_info (0, 0, 0, "exercising std::messages<char>");

    int result = 
        rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                        thread_func, 0, std::size_t (opt_timeout));

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

    ///////////////////////////////////////////////////////////////////////

#ifndef _RWSTD_NO_WCHAR_T

    test_char   = false;
    test_wchar  = true;

    rw_info (0, 0, 0, "exercising std::messages<wchar_t>");

    result = 
        rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                        thread_func, 0, std::size_t (opt_timeout));

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

    ///////////////////////////////////////////////////////////////////////

    test_char   = true;
    test_wchar  = true;

    rw_info (0, 0, 0, "exercising std::messages<char> and "
                      "std::messages<wchar_t>");

    result = 
        rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                        thread_func, 0, std::size_t (opt_timeout));

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              opt_nthreads, thread_func);

#endif // _RWSTD_NO_WCHAR_T

    ///////////////////////////////////////////////////////////////////////

    rw_system (SHELL_RM_F "rwstdmessages_*");

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
#ifdef _RWSTD_REENTRANT

    // set nthreads to the greater of the number of processors
    // and 2 (for uniprocessor systems) by default
    opt_nthreads = rw_get_cpus ();
    if (opt_nthreads < 2)
        opt_nthreads = 2;

#endif   // _RWSTD_REENTRANT

    return rw_test (argc, argv, __FILE__,
                    "lib.locale.messages",
                    "thread safety", run_test,
                    "|-soft-timeout#0 "  // must be non-negative
                    "|-nloops#0 "        // must be non-negative
                    "|-ncatalogs#0-* "   // must be non-negative
                    "|-nthreads#0-* ",    // must be in [0, MAX_THREADS]
                    &opt_timeout,
                    &opt_nloops,
                    int (MAX_CATALOGS),
                    &opt_ncatalogs,
                    int (MAX_THREADS),
                    &opt_nthreads);
}
