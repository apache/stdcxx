/************************************************************************
 *
 * 22.locale.numpunct.mt.cpp
 *
 * test exercising the thread safety of the numpunct facet
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

#include <locale>     // for locale, numpunct

#include <clocale>    // for lconv, localeconv()
#include <cstdlib>    // for mbstowcs()
#include <cstring>    // for size_t, strcpy()

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>
#include <valcmp.h>


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

struct NumPunctData
{
    char decimal_point_;
    char thousands_sep_;
    char grouping_ [32];
    char truename_ [32];
    char falsename_ [32];

#ifndef _RWSTD_NO_WCHAR_T

    wchar_t wdecimal_point_;
    wchar_t wthousands_sep_;
    wchar_t wtruename_ [32];
    wchar_t wfalsename_ [32];

#endif   // _RWSTD_NO_WCHAR_T

} punct_data [MAX_THREADS];


extern "C" {

bool test_char;    // exercise num_put<char>
bool test_wchar;   // exercise num_put<wchar_t>


static void*
thread_func (void*)
{
    for (int i = 0; i != rw_opt_nloops; ++i) {

        const std::size_t inx = std::size_t (i) % nlocales;

        // save the name of the locale
        const char* const locale_name = locales [inx];

        const NumPunctData* const data = punct_data + inx;

        // construct a named locale
        const std::locale loc (locale_name);

        if (test_char) {
            // exercise the narrow char specialization of the facet

            const std::numpunct<char> &np =
                std::use_facet<std::numpunct<char> >(loc);

            const char        dp  = np.decimal_point ();
            const char        ts  = np.thousands_sep ();
            const std::string grp = np.grouping ();
            const std::string tn  = np.truename ();
            const std::string fn  = np.falsename ();

            RW_ASSERT (dp == data->decimal_point_);
            RW_ASSERT (ts == data->thousands_sep_);
            RW_ASSERT (0 == rw_strncmp (grp.c_str (), data->grouping_));
            RW_ASSERT (0 == rw_strncmp (tn.c_str (),  data->truename_));
            RW_ASSERT (0 == rw_strncmp (fn.c_str (),  data->falsename_));
        }

        // both specializations may be tested at the same time

        if (test_wchar) {
            // exercise the wide char specialization of the facet

#ifndef _RWSTD_NO_WCHAR_T

            const std::numpunct<wchar_t> &np =
                std::use_facet<std::numpunct<wchar_t> >(loc);

            const wchar_t      dp  = np.decimal_point ();
            const wchar_t      ts  = np.thousands_sep ();
            const std::string  grp = np.grouping ();
            const std::wstring tn  = np.truename ();
            const std::wstring fn  = np.falsename ();

            RW_ASSERT (dp == data->wdecimal_point_);
            RW_ASSERT (ts == data->wthousands_sep_);
            RW_ASSERT (0 == rw_strncmp (grp.c_str (), data->grouping_));
            RW_ASSERT (0 == rw_strncmp (tn.c_str (),  data->wtruename_));
            RW_ASSERT (0 == rw_strncmp (fn.c_str (),  data->wfalsename_));

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

    // iterate over locales, initializing a global punct_data array
    for (const char *name = locale_list; *name; name += std::strlen (name) +1) {

        const std::size_t inx = nlocales;

        locales [inx] = name;

        // set LC_NUMERIC and LC_CTYPE to be able to use mbstowcs()
        if (std::setlocale (LC_ALL, name)) {

            const std::lconv* const pconv = std::localeconv ();
            NumPunctData* const pdata = punct_data + inx;

            // assign just the first character of the (potentially)
            // multibyte decimal_point and thousands_sep (C++ locale
            // can't deal with more)
            pdata->decimal_point_ = *pconv->decimal_point;
            pdata->thousands_sep_ = *pconv->thousands_sep;

            // simply copy the narrow grouping
            std::strcpy (pdata->grouping_, pconv->grouping);

            // FIXME: this will need to change once useful truename
            //        and falsename has been implemented
            std::strcpy (pdata->truename_,  "true");
            std::strcpy (pdata->falsename_, "false");

            wchar_t tmp [2];

            // convert multibyte decimal point and thousands separator
            // to wide characters (assumes they are single character
            // each -- C++ locale can't handle more)
            std::mbstowcs (tmp, pconv->decimal_point, 2);
            pdata->wdecimal_point_ = tmp [0];

            std::mbstowcs (tmp, pconv->thousands_sep, 2);
            pdata->wthousands_sep_ = tmp [0];

            const std::size_t n =
                sizeof pdata->wtruename_ / sizeof (wchar_t);

            std::mbstowcs (pdata->wtruename_, pdata->truename_, n);
            std::mbstowcs (pdata->wfalsename_, pdata->falsename_, n);

            ++nlocales;
        }

        if (nlocales == maxinx)
            break;
    }

    // reset the global locale
    std::setlocale (LC_ALL, "C");

    rw_info (0, 0, 0,
             "testing std::numpunct<charT> with %d thread%{?}s%{;}, "
             "%zu iteration%{?}s%{;} each, in locales { %{ .*A@} }",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops,
             int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::numpunct<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::numpunct<wchar_t>");

    test_char  = false;
    test_wchar = true;

    // start a pool of threads to exercise the thread safety
    // of the wchar_t specialization
    result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    // exercise both the char and the wchar_t specializations
    // at the same time

    rw_info (0, 0, 0,
             "exercising both std::numpunct<char> and std::numpunct<wchar_t>");

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

    // set nthreads to the greater of the number of processors
    // and 2 (for uniprocessor systems) by default
    rw_opt_nthreads = rw_get_cpus ();
    if (rw_opt_nthreads < 2)
        rw_opt_nthreads = 2;

#endif   // _RWSTD_REENTRANT

    return rw_test (argc, argv, __FILE__,
                    "lib.locale.numpunct",
                    "thread safety", run_test,
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-* "    // must be in [0, MAX_THREADS]
                    "|-locales=",        // must be provided
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads,
                    &rw_opt_setlocales);
}
