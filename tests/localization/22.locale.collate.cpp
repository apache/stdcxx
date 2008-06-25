/***************************************************************************
 *
 * 22.locale.collate.cpp -- tests for collate-facet member functions
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
 * Copyright 1994-2008 Rogue Wave Software.
 *
 **************************************************************************/

#include <locale>     // for collate, locale
#include <string>     // for string

#include <algorithm>  // for sort and unique
#include <climits>    // for UCHAR_MAX
#include <clocale>    // for LC_COLLATE, setlocale
#include <cstdlib>    // for exit()
#include <cstdio>     // for fprintf()
#include <cstring>    // for strcmp(), strcoll(), ...
#include <cwchar>     // for wcscoll()

#include <rw_driver.h>
#include <rw_environ.h>
#include <rw_file.h>
#include <rw_locale.h>
#include <rw_process.h>


#if _RWSTD_PATH_SEP == '/'
#  define SLASH                 "/"
#else
#  define SLASH                 "\\"
#endif

// strings declared extern to work around a SunPro bug (PR #28124)
// get the source root
#define RELPATH                 "etc" SLASH "nls"
#define TESTS_ETC_PATH          "tests" SLASH "etc"

// the root of the locale directory (RWSTD_LOCALE_ROOT)
#define LOCALE_ROOT             "RWSTD_LOCALE_ROOT"
const char* locale_root;

#define LC_COLLATE_SRC          "LC_COLLATE.src"
#define LC_COLLATE_CM           "LC_COLLATE.cm"
#define TEST_LOCALE_NAME        "test.locale"

/**************************************************************************/

// These overloads are necessary in our template
// functions so that we can make a single function call reguardless
// of the charT we are using

int c_strcoll (const char* s1, const char* s2)
{
    const int ret = std::strcoll(s1, s2);
    return ret ? ret > 0 ? 1 : -1 : 0;
}

std::size_t c_xfrm (char* to, const char* from, std::size_t size)
{
    char safety_buf [8];
    if (0 == to && 0 == size) {
        // prevent buggy implementations (such as MSVC 8) from trying
        // to write to the destination buffer even though it's 0 and
        // its size is zero (see stdcxx-69)
        to = safety_buf;
        *to = '\0';
    }

    std::size_t n = std::strxfrm (to, from, size);

    if (to)
        n = std::strlen (to);

    return n;
}

std::size_t c_strlen (const char* s1)
{
    return std::strlen (s1);
}

const char* narrow (char* dst, const char* src)
{
    if (src == dst || !src || !dst)
        return src;

    std::memcpy (dst, src, std::strlen (src) + 1);
    return dst;
}

const char* widen (char* dst, const char* src)
{
    if (src == dst || !src || !dst)
        return src;

    std::memcpy (dst, src, std::strlen (src) + 1);
    return dst;
}

#ifndef _RWSTD_NO_WCHAR_T

int c_strcoll (const wchar_t* s1, const wchar_t* s2)
{
    const int ret = std::wcscoll(s1, s2);
    return ret ? ret > 0 ? 1 : -1 : 0;
}

std::size_t c_xfrm (wchar_t* to, const wchar_t* from, std::size_t size)
{
#if !defined (_MSC_VER) || _MSC_VER > 1200

    wchar_t safety_buf [8];
    if (0 == to && 0 == size) {
        // prevent buggy implementations (such as MSVC 8) from trying
        // to write to the destination buffer even though it's 0 and
        // its size is zero (see stdcxx-69)
        to = safety_buf;
        *to = L'\0';
    }

    std::size_t n = std::wcsxfrm (to, from, size);

    if (to)
        n = std::wcslen (to);

#else   // MSVC 6 and prior

    // working around an MSVC 6.0 libc bug (PR #26437)

    if (to) {
        std::size_t n = std::wcsxfrm (to, from, size);

        n = std::wcslen (to);

        return n;
    }

    wchar_t tmp [1024];

    std::size_t n = std::wcslen (from);

    _RWSTD_ASSERT (n < sizeof tmp / sizeof *tmp);

    std::wcscpy (tmp, from);

    std::wcsxfrm (tmp, from, sizeof tmp / sizeof *tmp);

    n = std::wcslen (tmp);

#endif   // MSVC 6

    return n;
}

std::size_t c_strlen (const wchar_t* s1)
{
    return std::wcslen (s1);
}

const wchar_t* widen (wchar_t* dst, const char* src)
{
    static wchar_t buf [4096];

    if (!src)
        return 0;

    if (!dst)
        dst = buf;

    std::size_t len = std::strlen (src);

    _RWSTD_ASSERT (len < sizeof buf /sizeof *buf);

    len = std::mbstowcs (dst, src, sizeof buf / sizeof *buf);

    if (std::size_t (-1) == len)
        *dst = 0;

    return dst;
}

const char* narrow (char* dst, const wchar_t* src)
{
    static char buf [4096];

    if (!src)
        return 0;

    if (!dst)
        dst = buf;

    std::size_t len = std::wcslen (src);

    _RWSTD_ASSERT (len < sizeof buf);

    len = std::wcstombs (dst, src, sizeof buf / sizeof *buf);

    if (std::size_t (-1) == len)
        *dst = 0;

    return dst;
}

#endif   //_RWSTD_NO_WCHAR_T

/**************************************************************************/

template <class charT>
/*static*/ void
gen_str (charT* str, std::size_t size)
{
    // generate a random string with the given size
    if (!size)
        return;

    // use ASCII characters in the printable range
    for (std::size_t i = 0; i != size - 1; ++i)
        str [i] = ' ' + std::rand () % ('~' - ' ');

    str [size - 1] = charT ();
}

/**************************************************************************/

template <class charT>
/*static*/ void
check_libc (const char* charTname)
{
    // the libc implementation of the library should act the same as
    // the c-library.  Go through all the locales, generate some random
    // strings and make sure that the following holds true:
    // transform acts like strxfrm and wcsxfrm,
    // compare acts like strcoll and wcscoll

    int nfail [3] = { 0 };

    rw_info (0, __FILE__, __LINE__,
             "libc std::collate<%s>::transform ()", charTname);

    rw_info (0, __FILE__, __LINE__,
             "libc std::collate<%s>::compare ()", charTname);

    rw_info (0, __FILE__, __LINE__,
             "std::collate<%s>::hash ()", charTname);

    for (const char* locname = rw_locales (LC_COLLATE);
         *locname; locname += std::strlen (locname) + 1) {

        _TRY {
            std::setlocale (LC_COLLATE, locname);
            int max = MB_CUR_MAX;
            if (max > 1)
                continue;

            std::locale loc;

            _TRY {
                loc = std::locale (locname);
            }
            _CATCH (...) {
                rw_assert (false, __FILE__, __LINE__,
                           "std::locale(\"%s\") unexpectedly threw "
                           "an exception", locname);
                continue;
            }

            const std::collate<charT> &co =
                _STD_USE_FACET (std::collate<charT>, loc);
            co._C_opts |= co._C_use_libc;
            co._C_opts &= ~co._C_use_libstd;

            // now the locale is set up so lets test the transform and
            // compare functions

            for (int loop_cntrl = 0; loop_cntrl < 10; loop_cntrl++) {

#define STR_SIZE 16

                charT str1 [STR_SIZE] = { 0 };
                charT str2 [STR_SIZE] = { 0 };

                // generate two random NUL-terminated strings
                gen_str (str1, sizeof str1 / sizeof *str1);
                gen_str (str2, sizeof str2 / sizeof *str2);

                // call transform on the generated string
                // not including the terminating NUL
                const std::basic_string <charT, std::char_traits<charT>,
                    std::allocator<charT> > out =
                    co.transform (str1, str1 + sizeof str1 / sizeof *str1 - 1);

                // get the size of the buffer needed to hold the
                // transformed string (with the terminating NUL)
                std::size_t size = 1U + c_xfrm (0, str1, 0);

                // prevent errors caused by huge return values (e.g., MSVC)
                if (size > STR_SIZE * 64)
                    size = 0;

                std::basic_string <charT, std::char_traits<charT>,
                    std::allocator<charT> > c_out;

                if (size) {
                    c_out.resize (size);

                    // call the C-library transform function
                    size = c_xfrm (&c_out [0], str1, size);

                    if (size > STR_SIZE * 64)
                        size = 0;

                    // shrink to fit (chop off the terminating NUL)
                    c_out.resize (size);
                }

                // make sure the output is the same
                if (out != c_out) {
                    nfail[0]++;
                    rw_assert (false, __FILE__, __LINE__,
                               "%d. collate<%s>::transform(%s, ...) "
                               "== %{S}, got %{S} in locale(\"%s\")",
                               loop_cntrl, charTname, str1,
                               &c_out, &out, locname);
                }

                // now call compare on the two generated strings
                int ret1 = co.compare (str1, str1 + sizeof str1 / sizeof *str1,
                                       str2, str2 + sizeof str2 / sizeof *str2);

                // call the C-library comparison function
                int ret2 = c_strcoll (str1, str2);

                // make sure the results are the same
                if (ret1 != ret2) {
                    nfail [1]++;
                    rw_assert (false, __FILE__, __LINE__,
                               "%d. collate<%s>::compare(%s, ..., %s, ...) "
                               "== %d, got %d in locale(\"%s\")",
                               loop_cntrl, charTname, str1,
                               str2, ret2, ret1, locname);
                }

                // two strings that compare identically must hash
                // identically as well.  Calling hash on the same string is
                // not very conclusive but generating strings that have exactly
                // the same weights is not possible without knowing all the
                // weight orderings
                const long hashNum1 =
                    co.hash (str1, str1 + sizeof str1 / sizeof *str1);

                const long hashNum2 =
                    co.hash (str1, str1 + sizeof str1 / sizeof *str1);

                if (hashNum1 != hashNum2) {
                    nfail[2]++;
                    rw_assert (false, __FILE__, __LINE__,
                               "%d. collate<%s>::hash(%s, ...) == %d, "
                               "got %d in locale(\"%s\")",
                               loop_cntrl, charTname, str1,
                               hashNum1, hashNum2, locname);
                }


            }
        }
        _CATCH (...) {
            rw_assert (false, __FILE__, __LINE__,
                       "locale(\"%s\") threw an exception", locname);
        }
    }

    rw_assert (0 == nfail [0], __FILE__, __LINE__,
               "collate<%s>::transform () failed %d times",
               charTname, nfail [0]);

    rw_assert (0 == nfail [1], __FILE__, __LINE__,
               "collate<%s>::compare () failed %d times",
               charTname, nfail [1]);

    rw_assert (0 == nfail [2], __FILE__, __LINE__,
               "collate<%s>::hash () failed %d times",
               charTname, nfail [2]);
}

/**************************************************************************/

static const char*
make_test_locale ()
{
    // create a temporary locale definition file that exercises as
    // many different parts of the collate standard as possible

    char lc_collate_src_path [L_tmpnam + sizeof LC_COLLATE_SRC + 2];
    std::strcpy (lc_collate_src_path, locale_root);
    std::strcat (lc_collate_src_path, SLASH);
    std::strcat (lc_collate_src_path, LC_COLLATE_SRC);

    std::FILE *fout = std::fopen (lc_collate_src_path, "w");

    const char lc_collate_file[] = {
        "LC_COLLATE\n"
        "script <ALL_FORWARD>\n"
        "collating-element <er> from \"<e><r>\"\n"
        "collating-element <ic> from \"ic\"\n"
        "collating-symbol <LETTER>\n"
        "collating-symbol <COLLATING_ELEMENT>\n"
        "collating-symbol <DIGIT>\n"

        "order_start forward;backward;forward,position\n"
        "<LETTER>\n"
        "<COLLATING_ELEMENT>\n"
        "<DIGIT>\n"

        "<a> <a> <LETTER> IGNORE\n"
        "<b> <b> <LETTER> IGNORE\n"

        // "<c>" will have a non-ignored position ordering
        "<c> <c> <LETTER> <c>\n"

        // try giving "<d>" a many-to-one weight
        "<d> \"<d><a>\" <LETTER> IGNORE\n"

        // try giving "<e>" a decimal value weight
        "<e> \\d139 <LETTER> IGNORE\n"

        // try giving "<f>" an octal value weight
        "<f> \\36 <LETTER> IGNORE\n"

        // try giving "<g>" a hex value weight
        "<g> \\x3A <LETTER> IGNORE\n"

        "<zero> <zero> <DIGIT> IGNORE\n"
        "<one> <one> <DIGIT> <zero>\n"
        "<two> <two> <DIGIT> IGNORE\n"
        "<three> <three> <DIGIT> IGNORE\n"
        "<er> <a> <COLLATING_ELEMENT> IGNORE\n"

        // the <ic> collating element will be equivalent to the letter <c>
        "<ic> <c> <LETTER> <c>\n"
        "UNDEFINED IGNORE IGNORE IGNORE\n"

        "order_end\n"

        // define a section in which all of the orders are forward orders
        "order_start <ALL_FORWARD>;forward;forward;forward\n"
        "<h>\n<i>\n<j>\n<k>\n"
        "order_end\n"

        // reorder the elementes in the <ALL_FORWARD> section to appear
        // after the letter "<g>"
        "reorder-after <g>\n"
        "<h>\n<i>\n<j>\n<k>\n"

        // try to reorder "<a>" after "<b>"
        "reorder-after <b>\n"
        "<a> <a> <LETTER> IGNORE\n"
        "reorder-end\n"

        "\nEND LC_COLLATE\n"
    };

    std::fputs (lc_collate_file, fout);

    std::fclose (fout);

    // create a temporary character map file

    char lc_collate_cm_path [L_tmpnam + sizeof LC_COLLATE_CM + 2];
    std::strcpy (lc_collate_cm_path, locale_root);
    std::strcat (lc_collate_cm_path, SLASH);
    std::strcat (lc_collate_cm_path, LC_COLLATE_CM);

    fout = std::fopen (lc_collate_cm_path, "w");
    pcs_write (fout, 0);

    std::fclose (fout);

    return rw_localedef ("-w", lc_collate_src_path,
                         lc_collate_cm_path,
                         TEST_LOCALE_NAME);
}

/**************************************************************************/


template <class charT>
/*static*/ void
check_libstd_test_locale (const char* charTname)
{
    rw_info (0, __FILE__, __LINE__,
             "libstd std::collate<%s>::transform () "
             "collate test database", charTname);
    rw_info (0, __FILE__, __LINE__,
             "libstd std::collate<%s>::compare () collate test "
             "database", charTname);
    rw_info (0, __FILE__, __LINE__,
             "libstd std::collate<%s>::hash () collate test "
             "database", charTname);

    const char* const locname = make_test_locale ();
    if (locname) {

        std::locale loc;

        _TRY {
            loc = std::locale (locname);
        }
        _CATCH (...) {
            const char* const var = std::getenv (LOCALE_ROOT);

            rw_assert (false, __FILE__, __LINE__,
                       "std::locale(\"%s\") unexpectedly threw "
                       "an exception; " LOCALE_ROOT "=%s",
                       locname, var ? var : "(null)");
            return;
        }

        const std::collate<charT> &co =
            _STD_USE_FACET (std::collate<charT>, loc);
        co._C_opts |=  co._C_use_libstd;
        co._C_opts &= ~co._C_use_libc;

#define IGNORE 0

        // first lets make sure that each character was given the
        // correct weight for each level.

#undef TEST
#define TEST(ch, w0, w1, w2, w3, w3_is_fp)   \
  test_weight_val (charTname, co, charT (ch), w0, w1, w2, w3, w3_is_fp)

        TEST ('a',       6, IGNORE,      2, IGNORE, true);
        TEST ('b',       5, IGNORE,      2, IGNORE, true);
        TEST ('c',       7, IGNORE,      2,      7, true);
        TEST ('d',       8,      6,      2, IGNORE, true);
        TEST ('e',     139, IGNORE,      2, IGNORE, true);
        TEST ('f',      30, IGNORE,      2, IGNORE, true);
        TEST ('g',      58, IGNORE,      2, IGNORE, true);
        TEST ('h',      12, IGNORE,     12,     12, false);
        TEST ('i',      13, IGNORE,     13,     13, false);
        TEST ('j',      14, IGNORE,     14,     14, false);
        TEST ('k',      15, IGNORE,     15,     15, false);
        TEST ('0',      16, IGNORE,      4, IGNORE, true);
        TEST ('1',      17, IGNORE,      4,     16, true);
        TEST ('2',      18, IGNORE,      4, IGNORE, true);
        TEST ('3',      19, IGNORE,      4, IGNORE, true);
        TEST ('l',  IGNORE, IGNORE, IGNORE, IGNORE, true);

        // make sure that strings collate the way we expect them to

        // a should collate greater then b
        test_string (charTname, co, "a", "b", 1) ;

        // the collating element "er" should collate after 'a' and 'b'
        // but before 'c'
        test_string (charTname, co, "er", "a", 1);
        test_string (charTname, co, "er", "b", 1);
        test_string (charTname, co, "er", "c", -1);

        // the collating element "ic" should be equivalent to the letter 'c'
        test_string (charTname, co, "ic", "c", 0);


        // two strings that compare identically must hash
        // identically as well.
        // since ic and c are equivalent elements string they should hash
        // the same
        test_hash (charTname, co, "c", "ic");
    }
    else
        rw_assert (false, __FILE__, __LINE__,
                   "unable to create a locale database");
}

/**************************************************************************/

enum { bufsiz = 256 };

template <class charT>
/*static*/ void
test_hash (const char* charTname, const std::collate<charT>& co,
           const char* str1, const char* str2)
{
    // convert narrow string to a (possibly) wide representation
    charT wstrbuf [bufsiz];
    charT wstrbuf2 [bufsiz];

    const charT* const wstr = widen (wstrbuf, str1);
    const charT* const wstr2 = widen (wstrbuf2, str2);

    long hashNum1 = co.hash (wstr, wstr + c_strlen (wstr));
    long hashNum2 = co.hash (wstr2, wstr2 + c_strlen (wstr2));

    if (hashNum1 != hashNum2) {
        rw_assert (false, __FILE__, __LINE__,
                   "collate<%s>::hash(%s, ...) returned %d and\n "
                   "collate<%s>::hash(%s, ...) returned %d",
                   charTname, str1,
                   hashNum1, charTname, str2, hashNum2);
    }
}

/**************************************************************************/

template <class charT>
/*static*/ void
test_string (const char* charTname, const std::collate<charT>& co,
             const char* str1, const char* str2,
             int expected_val)
{
    // convert narrow string to a (possibly) wide representation
    charT wstrbuf [bufsiz];
    charT wstrbuf2 [bufsiz];

    const charT* const wstr = widen (wstrbuf, str1);
    const charT* const wstr2 = widen (wstrbuf2, str2);

    int ret = co.compare (wstr, wstr + c_strlen (wstr),
                          wstr2, wstr2 +  c_strlen(wstr2));
    if (ret != expected_val)
        rw_assert (false, __FILE__, __LINE__,
                   "libstd std::collate<%s>::compare"
                   "(%s, ..., %s, ...) == %d, got %d",
                   charTname, str1, str2, expected_val, ret);
}

/**************************************************************************/

template <class charT>
/*static*/ void
test_weight_val (const char* charTname, const std::collate<charT>& co,
                 charT ch, int w1a, int w1b, int w2, int w3, bool w3_is_fp)
{
    int w [3][2] = { { w1a, w1b }, { w2, IGNORE }, { w3, IGNORE } };

    typedef std::char_traits<charT>                  Traits;
    typedef std::allocator<charT>                    Alloc;
    typedef std::basic_string <charT, Traits, Alloc> String;

    // construct an expected transformed string out of the weight arguments
    String expected;

    if (sizeof (charT) == sizeof (char)) {
        for (int i = 0; i < 3; ++i) {
            for (int k = 0; k < 2; ++k) {
                if (w [i][k] != IGNORE) {
                    while (w [i][k] > _RWSTD_CHAR_MAX) {
                        expected += charT (_RWSTD_CHAR_MAX);
                        w [i][k] -= _RWSTD_CHAR_MAX;
                    }
                    expected += charT (w [i][k]);
                }
                else if (i == 2 && k == 0 && w3_is_fp)
                    expected += charT (_RWSTD_CHAR_MAX);
            }

            // mark the end of the pass
            expected += charT (1);
        }
    }
    else {
        for (int i = 0; i < 3; ++i) {
            for (int k = 0; k < 2; ++k) {
                if (w [i][k] != IGNORE) {
                    expected += charT (w [i][k]);
                }
                else if (i == 2 && k == 0 && w3_is_fp)
                    expected += charT (_RWSTD_WCHAR_MAX);
            }

            expected += charT (1);
        }
    }

    // get the transformed string
    const String actual = co.transform (&ch, &ch + 1);

    // make sure the strings are equal
    rw_assert (expected != actual, __FILE__, __LINE__,
               "collate<%s>::transform (\"%c\", ...) == %{S}, "
               "got %{S}", charTname, ch, &expected, &actual);
}

/**************************************************************************/

template <class charT>
/*static*/ void
check_libstd (const char* charTname)
{
    rw_info (0, __FILE__, __LINE__,
             "libstd std::collate<%s>::transform () sorting "
             "file test", charTname);

    rw_info (0, __FILE__, __LINE__,
             "libstd std::collate<%s>::compare () sorting "
             "file test", charTname);


    // This test works by using a series of sorted input files
    // we randomize the words in the input files and sort them using
    // the proper locale's collate facet.  This test will automatically
    // generate the required locales.

    static const char* const locales[][3] = {
        //
        // +-- locale name
        // |        +-- character set
        // |        |             +-- input file name
        // |        |             |
        // V        V             V
        { "cs_CZ", "ISO-8859-2", "collate.cs_CZ.in" },   // Czech, Czech Rep.
        { "da_DK", "ISO-8859-1", "collate.da_DK.in" },   // Danish, Denmark
        { "en_US", "ISO-8859-1", "collate.en_US.in" },   // English, US
        { "hr_HR", "ISO-8859-2", "collate.hr_HR.in" },   // Hungarian, Hungary
        { "sv_SE", "ISO-8859-1", "collate.sv_SE.in" },   // Swedish, Sweden
        { "th_TH", "TIS-620",    "collate.th_TH.in" }    // Thai, Thailand
    };

    const std::size_t nlocales = sizeof locales / sizeof *locales;

    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

    for (std::size_t i = 0; i < nlocales; ++i) {

        const char* const locname =
            rw_localedef ("-w --no_position",
                          locales [i][0], locales [i][1], 0);

        if (locname) {

            std::locale loc;

            _TRY {
                loc = std::locale (locname);
            }
            _CATCH (...) {
                const char* const var = std::getenv (LOCALE_ROOT);

                rw_assert (false, __FILE__, __LINE__,
                           "std::locale(\"%s\") unexpectedly threw "
                           "an exception; " LOCALE_ROOT "=%s",
                           locname, var ? var : "(null)");
                continue;
            }

            const std::collate<charT> &co =
                _STD_USE_FACET (std::collate<charT>, loc);

            co._C_opts |= co._C_use_libstd;
            co._C_opts &= ~co._C_use_libc;

            typedef std::codecvt<charT, char, std::mbstate_t> CodeCvt;

            const CodeCvt &cvt = _STD_USE_FACET (CodeCvt, loc);

            cvt._C_opts |= cvt._C_use_libstd;
            cvt._C_opts &= ~cvt._C_use_libc;

            // 'in' holds the strings from the input file and is there
            // sorting will take place.
            String in [1000];

            // out holds the strings located in the output file
            String out [1000];

#define TOPDIR   "TOPDIR"   /* the TOPDIR environment variable */

            const char* in_path = std::getenv (TOPDIR);
            if (!in_path || !*in_path) {
                std::fprintf (stderr, "TOPDIR not defined or empty");

                std::exit (1);
            }

            std::string path (in_path);
            path += SLASH TESTS_ETC_PATH SLASH;
            path += locales [i][2];

            std::FILE* const f = std::fopen (path.c_str (), "r");
            if (!f) {
                rw_assert (false, __FILE__, __LINE__,
                           "file \"%s\" could not be opened", path.c_str ());
                break;
            }

            std::size_t j = 0;
            while (1) {
                char next_line [bufsiz];

                if (0 != std::fgets (next_line, bufsiz, f)) {

                    std::size_t line_len = std::strlen (next_line);

                    // get rid of the newline character
                    next_line [--line_len] = '\0';

                    // convert from external to internal encoding
                    // (both of which might be the same type)
                    charT to [bufsiz];
                    const char* from_next;
                    charT*      to_next;

                    static std::mbstate_t initial;
                    std::mbstate_t mbs = initial;

                    const std::codecvt_base::result res =
                        cvt.in (mbs,
                                next_line, next_line + line_len + 1,
                                from_next,
                                to, to + sizeof to / sizeof *to,
                                to_next);

                    if (cvt.ok == res) {
                        in [j]  = to;
                        out [j] = to;
                    }
                    else if (cvt.noconv == res) {
                        in [j]  = (charT*)next_line;
                        out [j] = (charT*)next_line;
                    }

                    j++;
                }
                else
                    break;
            }
            // close the file
            std::fclose (f);

            // now bubble sort the items in the array
            std::size_t idx;
            std::size_t idx2;
            String tmp;
            String tmp2;

            bool flipped;

            if (j > 1) {
                idx = 1;
                do {
                    flipped = false;
                    for (idx2 = j - 1; idx2 >= idx; --idx2) {

                        const std::size_t idx1 = idx2 - 1;

                        if (co.compare (in [idx1].c_str (),
                                        in [idx1].c_str () + in [idx1].size (),
                                        in [idx2].c_str (),
                                        in [idx2].c_str () + in [idx2].size ())
                            > 0) {
                            in [idx1].swap (in [idx2]);
                            flipped = true;
                        }
                    }
                } while (++idx < j && flipped);
            }

            // the items are sorted now lets make sure that they are sorted
            // the same way they are sorted in the output file.
            std::size_t nfail = 0;

            for (std::size_t k = 0; k < j; ++k) {

                if (in [k] != out [k]) {

                    nfail++;

                    rw_assert (false, __FILE__, __LINE__,
                               "%{S} != %{S} at line %u of %s",
                               &out [k], &in [k],
                               k + 1, locales [i][2]);

                }
            }

            rw_assert (!nfail, __FILE__, __LINE__,
                       "collate<%s>::compare() failed %d times",
                       charTname, nfail);
        }
    }
}

/**************************************************************************/


template <class charT>
/*static*/ void
check_hash_eff (const char* charTname)
{
    // test effectiveness of hash function
    rw_info (0, __FILE__, __LINE__,
             "std::collate<%s>::hash () -- effectiveness", charTname);

    // since the same hash algorithm is used for both byname and non-byname
    // facets, simply set up a std::locale that uses the "C" locale
    std::locale loc ("C");
    const std::collate<charT> &co =
        _STD_USE_FACET (std::collate<charT>, loc);


    int nfail = 0;

    charT s[100];
    bool next = true;

    // generate `N' unique strings and hash them, storing each value
    static const std::size_t N = 100;
    long hashed [N] = { 0 };

    std::size_t k;
    for (k = 1; k != N && next; ++k) {
        // generate a unique string
        gen_str (s, k);

        // compute hash value
        hashed [k] = co.hash (s, s + std::char_traits<charT>::length(s));
    }

    // sort hashed values, then remove all duplicates
    std::sort (hashed, hashed + k);
    k = std::unique (hashed, hashed + k) - hashed;

    // assert that the probability of a collision is less than 1%
    // according to 22.2.4.1, p3, the likelihood should be very small,
    // approaching 1.0 / numeric_limits<unsigned long>::max()
    if (N - k > N /100) {
        nfail++;
        rw_assert (false, __FILE__, __LINE__,
                   "collate<%s>::do_hash (const char_type*, "
                   "const char_type*); "
                   "probability of collision %f",
                   charTname, double (N - k) / N);
    }

    rw_assert (!nfail, __FILE__, __LINE__,
               "collate<%s>::do_hash () failed %d times", charTname,
               nfail);

}

/**************************************************************************/


template <class charT>
/*static*/ void
check_NUL (const char* charTname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::collate<%s>::compare() with embedded NULs", charTname);

    // verify that the collate facet correctly handles
    // character sequences with embedded NULs

    charT buf_1 [STR_SIZE];
    charT buf_2 [STR_SIZE];

    bool fail = false;

    unsigned i = 0;

    for (const char* locname = rw_locales (LC_COLLATE);
         *locname && !fail; locname += std::strlen (locname) + 1, ++i) {

        std::locale loc;

        _TRY {
            loc = std::locale (locname);
        }
        _CATCH (...) {
            continue;
        }

        const std::size_t buflen = sizeof buf_1 / sizeof *buf_1 - 1;

        gen_str (buf_1, sizeof buf_1 / sizeof *buf_1);
        std::memcpy (buf_2, buf_1, sizeof buf_2);

        // compute a random index into the character buffers
        // at which to set the element to NUL; the indices
        // are such that (inx_1 > inx_2) always holds
        const std::size_t inx_2 = std::rand () % (buflen - 1);
        const std::size_t inx_1 =
            inx_2 + 1 + std::rand () % (buflen - inx_2 - 1);

        buf_2 [inx_2] = charT ();

        typedef std::collate<charT> CollateT;

        const CollateT &col = std::use_facet<CollateT>(loc);

        int cmp = col.compare (buf_1, buf_1 + buflen, buf_2, buf_2 + buflen);

        if (!cmp) {
            typedef typename CollateT::string_type StringT;

            const StringT str_1 (buf_1, buflen);
            const StringT str_2 (buf_2, buflen);

            fail = true;

            rw_assert (false, __FILE__, __LINE__,
                       "collate<%s>::compare(%{S}, ..., %{S}, ...) "
                       "!= 0, got 0 in locale(\"%s\")", charTname,
                       &str_1, &str_2, locname);
        }

        // set the character at the smaller index in both buffers to
        // NUL, then set a character at the larger index in the first
        // buffer to NUL, compare the two, and verify that the buffers
        // compare unequal (buf_1 probably less)
        buf_1 [inx_1] = charT ();
        buf_1 [inx_2] = charT ();

        cmp = col.compare (buf_1, buf_1 + buflen, buf_2, buf_2 + buflen);

        if (!cmp) {
            typedef typename CollateT::string_type StringT;

            const StringT str_1 (buf_1, buflen);
            const StringT str_2 (buf_2, buflen);

            fail = true;

            rw_assert (false, __FILE__, __LINE__,
                       "collate<%s>::compare(%{S}, ..., %{S}, ...) "
                       "!= 0, got 0 in locale(\"%s\")", charTname,
                       &str_1, &str_2, locname);
        }
    }
}

/**************************************************************************/

template <class charT>
/*static*/ void
do_test (const char* charTname)
{
    check_libstd_test_locale<charT> (charTname);
    check_libstd<charT> (charTname);
    check_libc<charT> (charTname);
    check_NUL<charT> (charTname);
    check_hash_eff<charT> (charTname);
}


#if _RWSTD_PATH_SEP == '/'
#  define RM_RF    "rm -rf "
#else
#  define RM_RF    "rmdir /Q /S "
#endif   // _RWSTD_PATH_SEP == '/'


static int
run_test (int /*argc*/, char* /*argv*/ [])
{
    // set any additional environment variables defined in
    // the RW_PUTENV environment variable (if it exists)
    rw_putenv (0);

    // create a temporary directory for files created by the test
    char namebuf [L_tmpnam];
    locale_root = std::tmpnam (namebuf);

    char envvar [sizeof LOCALE_ROOT + L_tmpnam] = LOCALE_ROOT "=";
    std::strcat (envvar, locale_root);

    rw_system ("mkdir %s", locale_root);

    // set the LOCALE_ROOT variable where std::locale looks
    // for locale database files
    rw_putenv (envvar);

    do_test<char> ("char");

#ifndef _RWSTD_NO_WCHAR_T

    do_test<wchar_t> ("wchar_t");

#endif   // _RWSTD_NO_WCHAR_T

    // remove temporary locale databases created by the test
    rw_system (RM_RF "%s", locale_root);

    return 0;
}


/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[lib.category.collate]",
                    "22.2.4 The collate category",
                    run_test, "", 0);
}

