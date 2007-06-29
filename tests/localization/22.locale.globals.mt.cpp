/************************************************************************
 *
 * 22.locale.globals.mt.cpp
 *
 * test exercising the thread safety of [locale.global.templates]
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

#include <locale>     // for locale

#include <cstring>    // for strlen()

#include <rw_locale.h>
#include <rw_thread.h>   // for rw_get_processors (), rw_thread_pool()
#include <driver.h>


// maximum number of threads allowed by the command line interface
#define MAX_THREADS   16

// default number of threads (will be adjusted to the number
// of processors/cores later)
int opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int opt_nloops = 20000;

/**************************************************************************/

// array of locale names to use for testing
static const char*
locales [MAX_THREADS];

// number of locale names in the array
static std::size_t
nlocales;

/**************************************************************************/

// convenience typedefs for all required specializations of standard facets
typedef std::collate<char>                       Collate;
typedef std::ctype<char>                         Ctype;
typedef std::codecvt<char, char, std::mbstate_t> Codecvt;
typedef std::messages<char>                      Messages;
typedef std::moneypunct<char, false>             Moneypunct0;
typedef std::moneypunct<char, true>              Moneypunct1;
typedef std::numpunct<char>                      Numpunct;
typedef std::money_get<char>                     MoneyGet;
typedef std::money_put<char>                     MoneyPut;
typedef std::num_get<char>                       NumGet;
typedef std::num_put<char>                       NumPut;
typedef std::time_get<char>                      TimeGet;
typedef std::time_put<char>                      TimePut;

typedef std::collate_byname<char>                       CollateByname;
typedef std::ctype_byname<char>                         CtypeByname;
typedef std::codecvt_byname<char, char, std::mbstate_t> CodecvtByname;
typedef std::messages_byname<char>                      MessagesByname;
typedef std::moneypunct_byname<char, false>             Moneypunct0Byname;
typedef std::moneypunct_byname<char, true>              Moneypunct1Byname;
typedef std::numpunct_byname<char>                      NumpunctByname;
typedef std::time_get_byname<char>                      TimeGetByname;
typedef std::time_put_byname<char>                      TimePutByname;


#ifndef _RWSTD_NO_WCHAR_T

typedef std::collate<wchar_t>                       WCollate;
typedef std::ctype<wchar_t>                         WCtype;
typedef std::codecvt<wchar_t, char, std::mbstate_t> WCodecvt;
typedef std::messages<wchar_t>                      WMessages;
typedef std::moneypunct<wchar_t, false>             WMoneypunct0;
typedef std::moneypunct<wchar_t, true>              WMoneypunct1;
typedef std::numpunct<wchar_t>                      WNumpunct;
typedef std::money_get<wchar_t>                     WMoneyGet;
typedef std::money_put<wchar_t>                     WMoneyPut;
typedef std::num_get<wchar_t>                       WNumGet;
typedef std::num_put<wchar_t>                       WNumPut;
typedef std::time_get<wchar_t>                      WTimeGet;
typedef std::time_put<wchar_t>                      WTimePut;

typedef std::collate_byname<wchar_t>                       WCollateByname;
typedef std::ctype_byname<wchar_t>                         WCtypeByname;
typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> WCodecvtByname;
typedef std::messages_byname<wchar_t>                      WMessagesByname;
typedef std::moneypunct_byname<wchar_t, false>             WMoneypunct0Byname;
typedef std::moneypunct_byname<wchar_t, true>              WMoneypunct1Byname;
typedef std::numpunct_byname<wchar_t>                      WNumpunctByname;
typedef std::time_get_byname<wchar_t>                      WTimeGetByname;
typedef std::time_put_byname<wchar_t>                      WTimePutByname;

#endif   // _RWSTD_NO_WCHAR_T


extern "C" {

static void*
test_has_facet (void*)
{
    const std::locale classic (std::locale::classic ());

    for (int i = 0; i != opt_nloops; ++i) {

        // save the name of the locale
        const char* const locale_name = locales [i % nlocales];

        // construct a named locale and imbue it in the ios object
        // so that the locale is used not only by the num_put facet
        // but also by the numpunct facet
        const std::locale loc (locale_name);

        const bool byname = loc != classic;

        RW_ASSERT (std::has_facet<Collate>(loc));
        RW_ASSERT (std::has_facet<Ctype>(loc));
        RW_ASSERT (std::has_facet<Codecvt>(loc));
        RW_ASSERT (std::has_facet<Messages>(loc));
        RW_ASSERT (std::has_facet<Moneypunct0>(loc));
        RW_ASSERT (std::has_facet<Moneypunct1>(loc));
        RW_ASSERT (std::has_facet<MoneyGet>(loc));
        RW_ASSERT (std::has_facet<MoneyPut>(loc));
        RW_ASSERT (std::has_facet<NumGet>(loc));
        RW_ASSERT (std::has_facet<NumPut>(loc));
        RW_ASSERT (std::has_facet<TimeGet>(loc));
        RW_ASSERT (std::has_facet<TimePut>(loc));

        RW_ASSERT (byname == std::has_facet<CollateByname>(loc));
        RW_ASSERT (byname == std::has_facet<CtypeByname>(loc));
        RW_ASSERT (byname == std::has_facet<CodecvtByname>(loc));
        RW_ASSERT (byname == std::has_facet<MessagesByname>(loc));
        RW_ASSERT (byname == std::has_facet<Moneypunct0Byname>(loc));
        RW_ASSERT (byname == std::has_facet<Moneypunct1Byname>(loc));
        RW_ASSERT (byname == std::has_facet<TimeGetByname>(loc));
        RW_ASSERT (byname == std::has_facet<TimePutByname>(loc));

#ifndef _RWSTD_NO_WCHAR_T

        RW_ASSERT (std::has_facet<WCollate>(loc));
        RW_ASSERT (std::has_facet<WCtype>(loc));
        RW_ASSERT (std::has_facet<WCodecvt>(loc));
        RW_ASSERT (std::has_facet<WMessages>(loc));
        RW_ASSERT (std::has_facet<WMoneypunct0>(loc));
        RW_ASSERT (std::has_facet<WMoneypunct1>(loc));
        RW_ASSERT (std::has_facet<WMoneyGet>(loc));
        RW_ASSERT (std::has_facet<WMoneyPut>(loc));
        RW_ASSERT (std::has_facet<WNumGet>(loc));
        RW_ASSERT (std::has_facet<WNumPut>(loc));
        RW_ASSERT (std::has_facet<WTimeGet>(loc));
        RW_ASSERT (std::has_facet<WTimePut>(loc));

        RW_ASSERT (byname == std::has_facet<WCollateByname>(loc));
        RW_ASSERT (byname == std::has_facet<WCtypeByname>(loc));
        RW_ASSERT (byname == std::has_facet<WCodecvtByname>(loc));
        RW_ASSERT (byname == std::has_facet<WMessagesByname>(loc));
        RW_ASSERT (byname == std::has_facet<WMoneypunct0Byname>(loc));
        RW_ASSERT (byname == std::has_facet<WMoneypunct1Byname>(loc));
        RW_ASSERT (byname == std::has_facet<WTimeGetByname>(loc));
        RW_ASSERT (byname == std::has_facet<WTimePutByname>(loc));

#endif   // _RWSTD_NO_WCHAR_T

    }

    return 0;
}


static void*
test_use_facet (void*)
{
    const std::locale classic (std::locale::classic ());

    for (int i = 0; i != opt_nloops; ++i) {

        // save the name of the locale
        const char* const locale_name = locales [i % nlocales];

        // construct a named locale and imbue it in the ios object
        // so that the locale is used not only by the num_put facet
        // but also by the numpunct facet
        const std::locale loc (locale_name);

        const bool byname = loc != classic;

        {
            // get references to the char specializations of facets
            const Collate     &f0  = std::use_facet<Collate>(loc);
            const Ctype       &f1  = std::use_facet<Ctype>(loc);
            const Codecvt     &f2  = std::use_facet<Codecvt>(loc);
            const Messages    &f3  = std::use_facet<Messages>(loc);
            const Moneypunct0 &f4  = std::use_facet<Moneypunct0>(loc);
            const Moneypunct1 &f5  = std::use_facet<Moneypunct1>(loc);
            const MoneyGet    &f6  = std::use_facet<MoneyGet>(loc);
            const MoneyPut    &f7  = std::use_facet<MoneyPut>(loc);
            const Numpunct    &f8  = std::use_facet<Numpunct>(loc);
            const NumGet      &f9  = std::use_facet<NumGet>(loc);
            const NumPut      &f10 = std::use_facet<NumPut>(loc);
            const TimeGet     &f11 = std::use_facet<TimeGet>(loc);
            const TimePut     &f12 = std::use_facet<TimePut>(loc);

            if (byname) {
                // get references to the char specializations
                // of byname facets
                const CollateByname &f0n =
                    std::use_facet<CollateByname>(loc);
                const CtypeByname &f1n =
                    std::use_facet<CtypeByname>(loc);
                const CodecvtByname &f2n =
                    std::use_facet<CodecvtByname>(loc);
                const MessagesByname &f3n =
                    std::use_facet<MessagesByname>(loc);
                const Moneypunct0Byname &f4n =
                    std::use_facet<Moneypunct0Byname>(loc);
                const Moneypunct1Byname &f5n =
                    std::use_facet<Moneypunct1Byname>(loc);
                const NumpunctByname &f8n =
                    std::use_facet<NumpunctByname>(loc);
                const TimeGetByname &f11n =
                    std::use_facet<TimeGetByname>(loc);
                const TimePutByname &f12n =
                    std::use_facet<TimePutByname>(loc);

                RW_ASSERT (byname == (&f0  == &f0n));
                RW_ASSERT (byname == (&f1  == &f1n));
                RW_ASSERT (byname == (&f2  == &f2n));
                RW_ASSERT (byname == (&f3  == &f3n));
                RW_ASSERT (byname == (&f4  == &f4n));
                RW_ASSERT (byname == (&f5  == &f5n));
                RW_ASSERT (byname == (&f8  == &f8n));
                RW_ASSERT (byname == (&f11 == &f11n));
                RW_ASSERT (byname == (&f12 == &f12n));
            }
            else {

#ifndef _RWSTD_NO_THREAD_SAFE_EXCEPTIONS

                bool threw;

#define TEST_USE_FACET(Facet, loc)                       \
    try { threw = false; std::use_facet<Facet>(loc); }   \
    catch (...) { threw = true; }                        \
    RW_ASSERT (threw)

                TEST_USE_FACET (CollateByname, loc);
                TEST_USE_FACET (CtypeByname, loc);
                TEST_USE_FACET (CodecvtByname, loc);
                TEST_USE_FACET (MessagesByname, loc);
                TEST_USE_FACET (Moneypunct0Byname, loc);
                TEST_USE_FACET (Moneypunct1Byname, loc);
                TEST_USE_FACET (NumpunctByname, loc);
                TEST_USE_FACET (TimeGetByname, loc);
                TEST_USE_FACET (TimePutByname, loc);

#endif   // _RWSTD_NO_THREAD_SAFE_EXCEPTIONS

            }
        }

#ifndef _RWSTD_NO_WCHAR_T

        {
            const WCollate     &f0  = std::use_facet<WCollate>(loc);
            const WCtype       &f1  = std::use_facet<WCtype>(loc);
            const WCodecvt     &f2  = std::use_facet<WCodecvt>(loc);
            const WMessages    &f3  = std::use_facet<WMessages>(loc);
            const WMoneypunct0 &f4  = std::use_facet<WMoneypunct0>(loc);
            const WMoneypunct1 &f5  = std::use_facet<WMoneypunct1>(loc);
            const WMoneyGet    &f6  = std::use_facet<WMoneyGet>(loc);
            const WMoneyPut    &f7  = std::use_facet<WMoneyPut>(loc);
            const WNumpunct    &f8  = std::use_facet<WNumpunct>(loc);
            const WNumGet      &f9  = std::use_facet<WNumGet>(loc);
            const WNumPut      &f10 = std::use_facet<WNumPut>(loc);
            const WTimeGet     &f11 = std::use_facet<WTimeGet>(loc);
            const WTimePut     &f12 = std::use_facet<WTimePut>(loc);

            if (byname) {
                const WCollateByname &f0n =
                    std::use_facet<WCollateByname>(loc);
                const WCtypeByname &f1n =
                    std::use_facet<WCtypeByname>(loc);
                const WCodecvtByname &f2n =
                    std::use_facet<WCodecvtByname>(loc);
                const WMessagesByname &f3n =
                    std::use_facet<WMessagesByname>(loc);
                const WMoneypunct0Byname &f4n =
                    std::use_facet<WMoneypunct0Byname>(loc);
                const WMoneypunct1Byname &f5n =
                    std::use_facet<WMoneypunct1Byname>(loc);
                const WTimeGetByname &f11n =
                    std::use_facet<WTimeGetByname>(loc);
                const WTimePutByname &f12n =
                    std::use_facet<WTimePutByname>(loc);

                RW_ASSERT (byname == (&f0  == &f0n));
                RW_ASSERT (byname == (&f1  == &f1n));
                RW_ASSERT (byname == (&f2  == &f2n));
                RW_ASSERT (byname == (&f3  == &f3n));
                RW_ASSERT (byname == (&f4  == &f4n));
                RW_ASSERT (byname == (&f5  == &f5n));
                RW_ASSERT (byname == (&f11 == &f11n));
                RW_ASSERT (byname == (&f12 == &f12n));
            }
            else {

#ifndef _RWSTD_NO_THREAD_SAFE_EXCEPTIONS

                bool threw;

#define TEST_USE_FACET(Facet, loc)                       \
    try { threw = false; std::use_facet<Facet>(loc); }   \
    catch (...) { threw = true; }                        \
    RW_ASSERT (threw)

                TEST_USE_FACET (WCollateByname, loc);
                TEST_USE_FACET (WCtypeByname, loc);
                TEST_USE_FACET (WCodecvtByname, loc);
                TEST_USE_FACET (WMessagesByname, loc);
                TEST_USE_FACET (WMoneypunct0Byname, loc);
                TEST_USE_FACET (WMoneypunct1Byname, loc);
                TEST_USE_FACET (WNumpunctByname, loc);
                TEST_USE_FACET (WTimeGetByname, loc);
                TEST_USE_FACET (WTimePutByname, loc);

#endif   // _RWSTD_NO_THREAD_SAFE_EXCEPTIONS

            }
        }

#endif   // _RWSTD_NO_WCHAR_T

    }

    return 0;
}


}   // extern "C"

/**************************************************************************/

int opt_has_facet;   // exercise std::has_facet?
int opt_use_facet;   // exercise std::use_facet?


static int
run_test (int, char**)
{
    // find all installed locales for which setlocale(LC_ALL) succeeds
    const char* const locale_list =
        rw_opt_locales ? rw_opt_locales : rw_locales (_RWSTD_LC_ALL);

    const std::size_t maxinx = sizeof locales / sizeof *locales;

    // set to true if the classic "C" locale is on the lost
    bool has_classic = false;

    for (const char *name = locale_list; *name; name += std::strlen (name) +1) {

        locales [nlocales++] = name;

        if (!has_classic && 0 == std::strcmp ("C", name))
            has_classic = true;

        if (nlocales == maxinx)
            break;
    }

    // when the classic "C" locale isn't on the list put it there
    // unless the list was explicitly specified on the command line
    if (1 < nlocales && !has_classic && 0 == rw_opt_locales)
        locales [0] = "C";

    int result;

    rw_info (0, 0, 0,
             "testing std::locale globals with %d thread%{?}s%{;}, "
             "%zu iteration%{?}s%{;} each, in %zu locales { %{ .*A@} }",
             opt_nthreads, 1 != opt_nthreads,
             opt_nloops, 1 != opt_nloops,
             nlocales, int (nlocales), "%#s", locales);

    if (opt_has_facet >= 0) {
        rw_info (0, 0, 0,
                 "template <class T> bool std::has_facet (const locale&)");

        // create and start a pool of threads and wait for them to finish
        result = rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                                 test_has_facet, 0);

        rw_error (result == 0, 0, __LINE__,
                  "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
                  opt_nthreads, test_has_facet);
    }
    else {
        rw_note (0, 0, 0, "std::has_facet test disabled");
    }

    if (opt_use_facet >= 0) {
        rw_info (0, 0, 0,
                 "template <class T> const T& std::use_facet (const locale&)");

#ifdef _RWSTD_NO_THREAD_SAFE_EXCEPTIONS

        rw_warning (0, 0, 0,
                    "exceptions not thread safe, skipping that part of test")

#endif   // _RWSTD_NO_THREAD_SAFE_EXCEPTIONS


        // create and start a pool of threads and wait for them to finish
        result = rw_thread_pool (0, std::size_t (opt_nthreads), 0,
                                 test_use_facet, 0);

        rw_error (result == 0, 0, __LINE__,
                  "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
                  opt_nthreads, test_use_facet);
    }
    else {
        rw_note (0, 0, 0, "std::use_facet test disabled");
    }

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
                    "lib.locale.global.templates",
                    "thread safety", run_test,
                    "|-has_facet~ "
                    "|-use_facet~ "
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-* "    // must be in [0, MAX_THREADS]
                    "|-locales=",        // must be provided
                    &opt_has_facet,
                    &opt_use_facet,
                    &opt_nloops,
                    int (MAX_THREADS),
                    &opt_nthreads,
                    &rw_opt_setlocales);
}
