/************************************************************************
 *
 * 22.locale.moneypunct.mt.cpp
 *
 * test exercising the thread safety of the moneypunct facet
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
#include <locale>     // for locale, moneypunct

#include <clocale>    // for lconv, localeconv()
#include <cstdlib>    // for mbstowcs()
#include <cstring>    // for size_t, strcpy()

#include <rw_locale.h>
#include <rw_thread.h>
#include <driver.h>
#include <valcmp.h>


// maximum number of threads allowed by the command line interface
#define MAX_THREADS   32


#ifdef _RWSTD_REENTRANT
int rw_opt_nthreads = 4;
#else   // if !defined (_RWSTD_REENTRANT)
// in non-threaded builds use just one thread
int rw_opt_nthreads = 1;
#endif   // _RWSTD_REENTRANT

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int rw_opt_nloops = 200000;

/**************************************************************************/

// number of locales to test
static std::size_t
nlocales;

/**************************************************************************/

struct MoneypunctData
{
    // the name of the locale the data goes with
    const char* locale_name_;

    char decimal_point_;
    char thousands_sep_;
    char grouping_ [32];
    char curr_symbol_ [32];
    char positive_sign_ [32];
    char negative_sign_ [32];
    int  frac_digits_;
    char pos_format_ [32];
    char neg_format_ [32];

    char int_curr_symbol_ [32];

    int  int_frac_digits_;
    char int_pos_format_ [32];
    char int_neg_format_ [32];

#ifndef _RWSTD_NO_WCHAR_T

    wchar_t wdecimal_point_;
    wchar_t wthousands_sep_;
    wchar_t wcurr_symbol_ [32];
    wchar_t wpositive_sign_ [32];
    wchar_t wnegative_sign_ [32];

    wchar_t wint_curr_symbol_ [32];

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

        const MoneypunctData* const data = punct_data + inx;

        // construct a named locale
        const std::locale loc (data->locale_name_);

        if (test_char) {
            // exercise the narrow char, local specialization of the facet

            typedef std::moneypunct<char, false> Punct;

            const Punct &mp = std::use_facet<Punct>(loc);

            const char           dp  = mp.decimal_point ();
            const char           ts  = mp.thousands_sep ();
            const std::string    grp = mp.grouping ();
            const std::string    cur = mp.curr_symbol ();
            const std::string    pos = mp.positive_sign ();
            const std::string    neg = mp.negative_sign ();
            const int            fd  = mp.frac_digits ();
            const Punct::pattern pfm = mp.pos_format ();
            const Punct::pattern nfm = mp.neg_format ();

            RW_ASSERT (dp == data->decimal_point_);
            RW_ASSERT (ts == data->thousands_sep_);
            RW_ASSERT (fd == data->frac_digits_);
            RW_ASSERT (!rw_strncmp (grp.c_str (), data->grouping_));
            RW_ASSERT (!rw_strncmp (cur.c_str (), data->curr_symbol_));
            RW_ASSERT (!rw_strncmp (pos.c_str (), data->positive_sign_));
            RW_ASSERT (!rw_strncmp (neg.c_str (), data->negative_sign_));

            RW_ASSERT (!std::memcmp (&pfm, data->pos_format_, 4));
            RW_ASSERT (!std::memcmp (&nfm, data->neg_format_, 4));
        }

        if (test_char) {
            // exercise the narrow char, international specialization

            typedef std::moneypunct<char, true> Punct;

            const Punct &mp = std::use_facet<Punct>(loc);

            const char           dp  = mp.decimal_point ();
            const char           ts  = mp.thousands_sep ();
            const std::string    grp = mp.grouping ();
            const std::string    cur = mp.curr_symbol ();
            const std::string    pos = mp.positive_sign ();
            const std::string    neg = mp.negative_sign ();
            const int            fd  = mp.frac_digits ();
            const Punct::pattern pfm = mp.pos_format ();
            const Punct::pattern nfm = mp.neg_format ();

            RW_ASSERT (dp == data->decimal_point_);
            RW_ASSERT (ts == data->thousands_sep_);
            RW_ASSERT (fd == data->frac_digits_);
            RW_ASSERT (!rw_strncmp (grp.c_str (), data->grouping_));
            RW_ASSERT (!rw_strncmp (cur.c_str (), data->int_curr_symbol_));
            RW_ASSERT (!rw_strncmp (pos.c_str (), data->positive_sign_));
            RW_ASSERT (!rw_strncmp (neg.c_str (), data->negative_sign_));

            RW_ASSERT (!std::memcmp (&pfm, data->int_pos_format_, 4));
            RW_ASSERT (!std::memcmp (&nfm, data->int_neg_format_, 4));
        }

        // both specializations may be tested at the same time

#ifndef _RWSTD_NO_WCHAR_T

        if (test_wchar) {
            // exercise the wide char, local specialization of the facet

            typedef std::moneypunct<wchar_t, false> Punct;

            const Punct &mp = std::use_facet<Punct>(loc);

            const char           dp  = mp.decimal_point ();
            const char           ts  = mp.thousands_sep ();
            const std::string    grp = mp.grouping ();
            const std::wstring   cur = mp.curr_symbol ();
            const std::wstring   pos = mp.positive_sign ();
            const std::wstring   neg = mp.negative_sign ();
            const int            fd  = mp.frac_digits ();
            const Punct::pattern pfm = mp.pos_format ();
            const Punct::pattern nfm = mp.neg_format ();

            RW_ASSERT (dp == data->wdecimal_point_);
            RW_ASSERT (ts == data->wthousands_sep_);
            RW_ASSERT (fd == data->frac_digits_);
            RW_ASSERT (!rw_strncmp (grp.c_str (), data->grouping_));
            RW_ASSERT (!rw_strncmp (cur.c_str (), data->wcurr_symbol_));
            RW_ASSERT (!rw_strncmp (pos.c_str (), data->wpositive_sign_));
            RW_ASSERT (!rw_strncmp (neg.c_str (), data->wnegative_sign_));

            RW_ASSERT (!std::memcmp (&pfm, data->pos_format_, 4));
            RW_ASSERT (!std::memcmp (&nfm, data->neg_format_, 4));
        }

        if (test_wchar) {
            // exercise the wide char, international specialization

            typedef std::moneypunct<wchar_t, true> Punct;

            const Punct &mp = std::use_facet<Punct>(loc);

            const char           dp  = mp.decimal_point ();
            const char           ts  = mp.thousands_sep ();
            const std::string    grp = mp.grouping ();
            const std::wstring   cur = mp.curr_symbol ();
            const std::wstring   pos = mp.positive_sign ();
            const std::wstring   neg = mp.negative_sign ();
            const int            fd  = mp.frac_digits ();
            const Punct::pattern pfm = mp.pos_format ();
            const Punct::pattern nfm = mp.neg_format ();

            RW_ASSERT (dp == data->wdecimal_point_);
            RW_ASSERT (ts == data->wthousands_sep_);
            RW_ASSERT (fd == data->frac_digits_);
            RW_ASSERT (!rw_strncmp (grp.c_str (), data->grouping_));
            RW_ASSERT (!rw_strncmp (cur.c_str (), data->wint_curr_symbol_));
            RW_ASSERT (!rw_strncmp (pos.c_str (), data->wpositive_sign_));
            RW_ASSERT (!rw_strncmp (neg.c_str (), data->wnegative_sign_));

            RW_ASSERT (!std::memcmp (&pfm, data->int_pos_format_, 4));
            RW_ASSERT (!std::memcmp (&nfm, data->int_neg_format_, 4));
        }

#endif   // _RWSTD_NO_WCHAR_T

    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static void
get_format (MoneypunctData *pdata, const std::lconv *pconv)
{
    // code copied from src/punct.cpp

    enum {
        // for syntactic convenience
        none   = std::money_base::none,
        space  = std::money_base::space,
        symbol = std::money_base::symbol,
        sign   = std::money_base::sign,
        value  = std::money_base::value
    };

    static const std::money_base::pattern pat[] = {

        // cs_precedes [0..1]:
        //
        // An integer set to 1 if the currency_symbol precedes the value
        // for a monetary value, and set to 0 if the symbol succeeds
        // the value.

        // sep_by_space [0..2]:
        //
        // 0  No space separates the currency_symbol from the value for
        //    a monetary value.
        // 1  If the currency symbol and sign string are adjacent, a space
        //    separates them from the value; otherwise, a space separates
        //    the currency symbol from the value.
        // 2  If the currency symbol and sign string are adjacent, a space
        //    separates them; otherwise, a space separates the sign string
        //    from the value.

        // sign_posn [0..4]:
        //
        // An integer set to a value indicating the positioning of the
        // positive_sign for a monetary value. The following integer
        // values shall be recognized:
        //
        // 0  Parentheses enclose the value and the currency_symbol.
        // 1  The sign string precedes the value and the currency_symbol.
        // 2  The sign string succeeds the value and the currency_symbol.
        // 3  The sign string immediately precedes the currency_symbol.
        // 4  The sign string immediately succeeds the currency_symbol.

        // +-------- cs_precedes
        // |+----- sep_by_space
        // ||+-- sign_posn
        // |||
        // VVV  ....        -     1      $       .           // pattern
        /* 000: -1$. */ { { sign, value, symbol, none } },   // "\3\4\2\0"
        /* 001: -1$. */ { { sign, value, symbol, none } },   // "\3\4\2\0"
        /* 002: 1$-. */ { { value, symbol, sign, none } },   // "\4\2\3\0"
        /* 003: 1-$. */ { { value, sign, symbol, none } },   // "\4\3\2\0"
        /* 004: 1$-. */ { { value, symbol, sign, none } },   // "\4\2\3\0"

        /* 010: -1 $ */ { { sign, value, space, symbol } },  // "\3\4\1\2"
        /* 011: -1 $ */ { { sign, value, space, symbol } },  // "\3\4\1\2"
        /* 012: 1 $- */ { { value, space, symbol, sign } },  // "\4\1\2\3"
        /* 013: 1 -$ */ { { value, space, sign, symbol } },  // "\4\3\3\2"
        /* 014: 1 $- */ { { value, space, symbol, sign } },  // "\4\1\2\3"

        /* 020: - 1$ */ { { sign, space, value, symbol } },  // "\3\1\4\2"
        /* 021: - 1$ */ { { sign, space, value, symbol } },  // "\3\1\4\2"
        /* 022: 1$ - */ { { value, symbol, space, sign } },  // "\4\2\1\3"
        /* 023: 1- $ */ { { value, sign, space, symbol } },  // "\4\3\1\2"
        /* 024: 1$ - */ { { value, symbol, space, sign } },  // "\4\2\1\3"

        /* 100: -$1. */ { { sign, symbol, value, none } },   // "\3\2\4\0"
        /* 101: -$1. */ { { sign, symbol, value, none } },   // "\3\2\4\0"
        /* 102: $1-. */ { { symbol, value, sign, none } },   // "\2\4\3\0"
        /* 103: -$1. */ { { sign, symbol, value, none } },   // "\3\2\4\0"
        /* 104: $-1. */ { { symbol, sign, value, none } },   // "\2\3\4\0"

        /* 110: -$ 1 */ { { sign, symbol, space, value } },  // "\3\2\1\4"
        /* 111: -$ 1 */ { { sign, symbol, space, value } },  // "\3\2\1\4"
        /* 112: $ 1- */ { { symbol, space, value, sign } },  // "\2\1\4\3"
        /* 113: -$ 1 */ { { sign, symbol, space, value } },  // "\3\2\1\4"
        /* 114: $- 1 */ { { symbol, sign, space, value } },  // "\2\3\1\4"

        /* 120: - $1 */ { { sign, space, symbol, value } },  // "\3\1\2\4"
        /* 121: - $1 */ { { sign, space, symbol, value } },  // "\3\1\2\4"
        /* 122: $1 - */ { { symbol, value, space, sign } },  // "\2\4\1\3"
        /* 123: - $1 */ { { sign, space, symbol, value } },  // "\3\1\2\4"
        /* 124: $ -1 */ { { symbol, space, sign, value } }   // "\2\1\3\4"
    };

    std::size_t inx;

    inx =   std::size_t (pconv->p_cs_precedes) * (3U * 5U)
          + std::size_t (pconv->p_sep_by_space) * 5U
          + std::size_t (pconv->p_sign_posn);

    if (inx < sizeof pat / sizeof *pat)
        std::memcpy (pdata->pos_format_, pat + inx, sizeof *pat);
    else
        std::memset (pdata->pos_format_, none, sizeof *pat);

    inx =   std::size_t (pconv->n_cs_precedes) * (3U * 5U)
          + std::size_t (pconv->n_sep_by_space) * 5U
          + std::size_t (pconv->n_sign_posn);

    if (inx < sizeof pat / sizeof *pat)
        std::memcpy (pdata->neg_format_, pat + inx, sizeof *pat);
    else
        std::memset (pdata->neg_format_, none, sizeof *pat);


#ifndef _RWSTD_NO_LCONV_INT_FMAT

    inx =   std::size_t (pconv->int_p_cs_precedes) * (3U * 5U)
          + std::size_t (pconv->int_p_sep_by_space) * 5U
          + std::size_t (pconv->int_p_sign_posn);

    if (inx < sizeof pat / sizeof *pat)
        std::memcpy (pdata->int_pos_format_, pat + inx, sizeof *pat);
    else
        std::memset (pdata->int_pos_format_, none, sizeof *pat);

    inx =   std::size_t (pconv->int_n_cs_precedes) * (3U * 5U)
          + std::size_t (pconv->int_n_sep_by_space) * 5U
          + std::size_t (pconv->int_n_sign_posn);

    if (inx < sizeof pat / sizeof *pat)
        memcpy (pdata->int_neg_format_, pat + inx, sizeof *pat);
    else
        memset (pdata->int_neg_format_, none, sizeof *pat);

    std::strcpy (pdata->int_curr_symbol_, pconv->int_curr_symbol);

    pdata->int_frac_digits_ = pconv->int_frac_digits;

#else   // if defined (_RWSTD_NO_LCONV_INT_FMAT)

    std::strcpy (pdata->int_curr_symbol_, pconv->curr_symbol);

    pdata->int_frac_digits_ = pconv->frac_digits;

#endif   // _RWSTD_NO_LCONV_INT_FMAT

}


static int
run_test (int, char**)
{
    // get a NUL-separated list of names of installed locales
    char* const locale_list = rw_locales ();

    // array of locale names to use for testing
    const char* locales [sizeof punct_data / sizeof *punct_data];

    const std::size_t maxinx = sizeof locales / sizeof *locales;

    // iterate over locales, initializing a global punct_data array
    for (char *name = locale_list; *name; name += std::strlen (name) + 1) {

        const std::size_t inx = nlocales;

        // set LC_NUMERIC and LC_CTYPE to be able to use mbstowcs()
        if (std::setlocale (LC_ALL, name)) {

            const std::lconv* const pconv = std::localeconv ();
            MoneypunctData* const pdata = punct_data + inx;

            locales [inx] = pdata->locale_name_ = name;

            // assign just the first character of the (potentially)
            // multibyte decimal_point and thousands_sep (C++ locale
            // can't deal with more)
            pdata->decimal_point_ = *pconv->mon_decimal_point;
            pdata->thousands_sep_ = *pconv->mon_thousands_sep;

            pdata->frac_digits_   = pconv->frac_digits;

            // simply copy the narrow grouping, currency symbols,
            // and signs
            std::strcpy (pdata->grouping_,      pconv->mon_grouping);
            std::strcpy (pdata->curr_symbol_,   pconv->currency_symbol);
            std::strcpy (pdata->negative_sign_, pconv->negative_sign);
            std::strcpy (pdata->positive_sign_, pconv->positive_sign);
            std::strcpy (pdata->grouping_,      pconv->mon_grouping);

            get_format (pdata, pconv);

#ifndef _RWSTD_WCHAR_T

            wchar_t tmp [2];

            // convert multibyte decimal point and thousands separator
            // to wide characters (assumes they are single character
            // each -- C++ locale can't handle more)
            std::mbstowcs (tmp, pconv->mon_decimal_point, 2);
            pdata->wdecimal_point_ = tmp [0];

            std::mbstowcs (tmp, pconv->mon_thousands_sep, 2);
            pdata->wthousands_sep_ = tmp [0];

            const std::size_t n =
                sizeof pdata->wcurr_symbol_ / sizeof (wchar_t);

            std::mbstowcs (pdata->wcurr_symbol_,   pdata->curr_symbol_,   n);
            std::mbstowcs (pdata->wnegative_sign_, pdata->negative_sign_, n);
            std::mbstowcs (pdata->wpositive_sign_, pdata->positive_sign_, n);

            std::mbstowcs (pdata->wint_curr_symbol_,
                           pdata->int_curr_symbol_,
                           n);

#endif   // _RWSTD_WCHAR_T

            ++nlocales;
        }

        if (nlocales == maxinx)
            break;
    }

    // reset the global locale
    std::setlocale (LC_ALL, "C");

    rw_info (0, 0, 0,
             "testing std::moneypunct<charT> with %d thread%{?}s%{;}, "
             "%zu iteration%{?}s%{;} each, in locales { %{ .*A@} }",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops,
             int (nlocales), "%#s", locales);

    rw_info (0, 0, 0, "exercising std::moneypunct<char>");

    test_char  = true;
    test_wchar = false;

    // create and start a pool of threads and wait for them to finish
    int result =
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0, thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#ifndef _RWSTD_NO_WCHAR_T

    rw_info (0, 0, 0, "exercising std::moneypunct<wchar_t>");

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
             "exercising both std::moneypunct<char> "
             "and std::moneypunct<wchar_t>");

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
    return rw_test (argc, argv, __FILE__,
                    "lib.locale.moneypunct",
                    "thread safety", run_test,
                    "|-nloops#0 "       // must be non-negative
                    "|-nthreads#0-*",   // must be in [0, MAX_THREADS]
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads);
}
