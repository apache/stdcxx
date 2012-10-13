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

#define IGNORE    0
#define STR_SIZE  16
#define LOCNAME_SIZE  256

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
    char safety_buf [8] = { 0 };

    if (0 == to && 0 == size) {
        // prevent buggy implementations (such as MSVC 8) from trying
        // to write to the destination buffer even though it's 0 and
        // its size is zero (see stdcxx-69)
        to = safety_buf;
    }

    std::size_t n = std::strxfrm (to, from, size);

    if (to && to != safety_buf)
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
    std::size_t n = 0;

#if !defined (_MSC_VER) || _MSC_VER > 1200

    wchar_t safety_buf [8] = { 0 };

    if (0 == to && 0 == size) {
        // prevent buggy implementations (such as MSVC 8) from trying
        // to write to the destination buffer even though it's 0 and
        // its size is zero (see stdcxx-69)
        to = safety_buf;
    }

    n = std::wcsxfrm (to, from, size);

    if (to && to != safety_buf)
        n = std::wcslen (to);

#else   // MSVC 6 and prior

    // working around an MSVC 6.0 libc bug (PR #26437)
    if (to) {
        std::wcsxfrm (to, from, size);
        n = std::wcslen (to);
    }
    else {
        wchar_t tmp [1024];

        n = std::wcslen (from);
        _RWSTD_ASSERT (n < sizeof tmp / sizeof *tmp);
        
        std::wcscpy (tmp, from);
        std::wcsxfrm (tmp, from, sizeof tmp / sizeof *tmp);
        
        n = std::wcslen (tmp);
    }

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
void
gen_str (charT* str, std::size_t size)
{
    // generate a random string with the given size
    if (!size)
        return;

    // use ASCII characters in the printable range
    for (std::size_t i = 0; i < size - 1; ++i)
        str [i] = ' ' + std::rand () % ('~' - ' ');

    str [size - 1] = charT ();
}

/**************************************************************************/

template <class charT>
void
check_libc_locale (const char* charTname, char const* locname,
                   int (&nfail) [3])
{
    typedef std::char_traits<charT> traits_type;
    typedef std::allocator<charT> allocator_type;
    typedef std::basic_string <charT, traits_type, allocator_type> string_type;

    std::locale loc (locname);

    const std::collate<charT> &co =
        _STD_USE_FACET (std::collate<charT>, loc);

    co._C_opts |= co._C_use_libc;
    co._C_opts &= ~co._C_use_libstd;

    for (int nloops = 0; nloops < 10; ++nloops) {

        charT str1 [STR_SIZE] = { 0 };
        charT str2 [STR_SIZE] = { 0 };

        // generate two random NUL-terminated strings
        gen_str (str1, sizeof str1 / sizeof *str1);
        gen_str (str2, sizeof str2 / sizeof *str2);

        // call transform on the generated string
        // not including the terminating NUL
        string_type out = co.transform (
            str1, str1 + sizeof str1 / sizeof *str1 - 1);

        // get the size of the buffer needed to hold the
        // transformed string (with the terminating NUL)
        std::size_t size = 1U + c_xfrm (0, str1, 0);

        // prevent errors caused by huge return values (e.g., MSVC)
        if (size > STR_SIZE * 64)
            size = 0;

        string_type c_out;

        if (size) {
            c_out.resize (size);

            // call the C-library transform function
            size = c_xfrm (&c_out [0], str1, size);

            if (size > STR_SIZE * 64)
                size = 0;

            // shrink to fit (chop off the terminating NUL)
            c_out.resize (size);
        }

        if (out != c_out)
            ++nfail [0];

        // make sure the output is the same
        rw_assert (out == c_out, __FILE__, __LINE__,
                   "%d. collate<%s>::transform(%{*.*Ac}, ...) "
                   "== %{*.*Ac}, got %{*.*Ac} in locale(\"%s\")",
                   nloops, charTname, 
                   sizeof (charT), sizeof str1 / sizeof *str1, str1,
                   sizeof (charT), c_out.size (), c_out.c_str (),
                   sizeof (charT),   out.size (),   out.c_str (),
                   locname);

        // now call compare on the two generated strings
        int ret1 = co.compare (
            str1, str1 + sizeof str1 / sizeof *str1,
            str2, str2 + sizeof str2 / sizeof *str2);

        // call the C-library comparison function
        int ret2 = c_strcoll (str1, str2);

        if (ret1 != ret2)
            ++nfail [1];

        // make sure the results are the same
        rw_assert (ret1 == ret2, __FILE__, __LINE__,
                   "%d. collate<%s>::compare(%{*.*Ac}, ..., "
                   "%{*.*Ac}, ...) == %d, got %d in locale(\"%s\")",
                   nloops, charTname, 
                   sizeof (charT), sizeof str1 / sizeof *str1, str1,
                   sizeof (charT), sizeof str2 / sizeof *str2, str2,
                   ret2, ret1, locname);

        // two strings that compare identically must hash
        // identically as well.  Calling hash on the same string is
        // not very conclusive but generating strings that have exactly
        // the same weights is not possible without knowing all the
        // weight orderings
        const long hashNum1 =
            co.hash (str1, str1 + sizeof str1 / sizeof *str1);

        const long hashNum2 =
            co.hash (str1, str1 + sizeof str1 / sizeof *str1);

        if (hashNum1 != hashNum2)
            ++nfail [2];

        rw_assert (hashNum1 == hashNum2, __FILE__, __LINE__,
                   "%d. collate<%s>::hash(%{*.*Ac}, ...) == %d, "
                   "got %d in locale(\"%s\")",
                   nloops, charTname, 
                   sizeof (charT), sizeof str1 / sizeof *str1, str1,
                   hashNum1, hashNum2, locname);
    }
}

template <class charT>
void
check_libc (const char* charTname)
{
    // the libc implementation of the library should act the same as
    // the c-library.  Go through all the locales, generate some random
    // strings and make sure that the following holds true:
    // transform acts like strxfrm and wcsxfrm,
    // compare acts like strcoll and wcscoll

    rw_info (0, __FILE__, __LINE__,
             "libc std::collate<%s>::transform ()", charTname);

    rw_info (0, __FILE__, __LINE__,
             "libc std::collate<%s>::compare ()", charTname);

    rw_info (0, __FILE__, __LINE__,
             "std::collate<%s>::hash ()", charTname);

    int nfail [3] = { 0 };
    char curlocname [256];
    
    for (const char* locname = rw_locales (LC_COLLATE);
         *locname; locname += std::strlen (locname) + 1) {

        std::strcpy (curlocname, std::setlocale (LC_COLLATE, 0));
        
        if (0 == std::setlocale (LC_COLLATE, locname))
            continue;

        int max = MB_CUR_MAX;
        
        if (max == 1) {
            //
            // FIXME test variable length multibyte encodings
            //
            _TRY {
                check_libc_locale<charT> (charTname, locname, nfail);
            }
            _CATCH (...) {
                rw_assert (false, __FILE__, __LINE__,
                           "locale(\"%s\") threw an exception", 
                           locname);
            }
        }

        std::setlocale (LC_COLLATE, curlocname);
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
    // Create a synthetic locale to exercises as many different parts
    // of the collate standard as possible.

    static const char charmap [] = {
        //
        // The portable character set
        //
        "<code_set_name> \"UTF-8\"\n"
        "<mb_cur_max> 1\n<mb_cur_min> 1\n"
        "CHARMAP\n"
        "<NUL> \\x00\n<SOH> \\x01\n<STX> \\x02\n<ETX> \\x03\n<EOT> \\x04\n"
        "<ENQ> \\x05\n<ACK> \\x06\n<BEL> \\x07\n"
        "<backspace> \\x08\n<tab> \\x09\n<newline> \\x0a\n"
        "<vertical-tab> \\x0b\n<form-feed> \\x0c\n"
        "<carriage-return> \\x0d\n"
        "<SO> \\x0e\n<SI> \\x0f\n<DLE> \\x10\n<DC1> \\x11\n<DC2> \\x12\n"
        "<DC3> \\x13\n<DC4> \\x14\n<NAK> \\x15\n<SYN> \\x16\n<ETB> \\x17\n"
        "<CAN> \\x18\n<EM> \\x19\n<SUB> \\x1a\n<ESC> \\x1b\n<IS4> \\x1c\n"
        "<IS3> \\x1d\n<IS2> \\x1e\n<IS1> \\x1f\n"
        "<space> \\x20\n"
        "<exclamation-mark> \\x21\n"
        "<quotation-mark> \\x22\n"
        "<number-sign> \\x23\n"
        "<dollar-sign> \\x24\n"
        "<percent-sign> \\x25\n"
        "<ampersand> \\x26\n"
        "<apostrophe> \\x27\n"
        "<left-parenthesis> \\x28\n"
        "<right-parenthesis> \\x29\n"
        "<asterisk> \\x2a\n"
        "<plus-sign> \\x2b\n"
        "<comma> \\x2c\n"
        "<hyphen> \\x2d\n"
        "<period> \\x2e\n"
        "<slash> \\x2f\n"
        "<zero> \\x30\n<one> \\x31\n<two> \\x32\n<three> \\x33\n"
        "<four> \\x34\n<five> \\x35\n<six> \\x36\n<seven> \\x37\n"
        "<eight> \\x38\n<nine> \\x39\n"
        "<colon> \\x3a\n"
        "<semicolon> \\x3b\n"
        "<less-than-sign> \\x3c\n"
        "<equals-sign> \\x3d\n"
        "<greater-than-sign> \\x3e\n"
        "<question-mark> \\x3f\n"
        "<commercial-at> \\x40\n"
        "<A> \\x41\n<B> \\x42\n<C> \\x43\n<D> \\x44\n<E> \\x45\n<F> \\x46\n"
        "<G> \\x47\n<H> \\x48\n<I> \\x49\n<J> \\x4a\n<K> \\x4b\n<L> \\x4c\n"
        "<M> \\x4d\n<N> \\x4e\n<O> \\x4f\n<P> \\x50\n<Q> \\x51\n<R> \\x52\n"
        "<S> \\x53\n<T> \\x54\n<U> \\x55\n<V> \\x56\n<W> \\x57\n<X> \\x58\n"
        "<Y> \\x59\n<Z> \\x5a\n"
        "<left-square-bracket> \\x5b\n"
        "<backslash> \\x5c\n"
        "<right-square-bracket> \\x5d\n"
        "<circumflex> \\x5e\n"
        "<underscore> \\x5f\n"
        "<grave-accent> \\x60\n"
        "<a> \\x61\n<b> \\x62\n<c> \\x63\n<d> \\x64\n<e> \\x65\n<f> \\x66\n"
        "<g> \\x67\n<h> \\x68\n<i> \\x69\n<j> \\x6a\n<k> \\x6b\n<l> \\x6c\n"
        "<m> \\x6d\n<n> \\x6e\n<o> \\x6f\n<p> \\x70\n<q> \\x71\n<r> \\x72\n"
        "<s> \\x73\n<t> \\x74\n<u> \\x75\n<v> \\x76\n<w> \\x77\n<x> \\x78\n"
        "<y> \\x79\n<z> \\x7a\n"
        "<left-brace> \\x7b\n"
        "<vertical-line> \\x7c\n"
        "<right-brace> \\x7d\n"
        "<tilde> \\x7e\n"
        "<DEL> \\x7f\n"
        "END CHARMAP\n\n"
    };

    // create a temporary locale definition file that exercises as
    // many different parts of the collate standard as possible
    const char lc_collate [] = {
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

    return rw_create_locale (charmap, lc_collate);
}

/**************************************************************************/

template <class charT>
void
test_hash (const char*, const std::collate<charT>&, 
           const char*, const char*);

template <class charT>
void
test_string (const char*, const std::collate<charT>&, 
             const char*, const char*, int);

template <class charT>
void
test_weight_val (const char*, const std::collate<charT>&,
                 charT, int, int, int, int, bool);

template <class charT>
void
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
#define TEST(ch, w0, w1, w2, w3, w3_is_fp)                              \
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

template <class charT>
void
test_hash (const char* charTname, const std::collate<charT>& co,
           const char* str1, const char* str2)
{
    // convert narrow string to a (possibly) wide representation
    charT wstrbuf [256];
    charT wstrbuf2 [256];

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
void
test_string (const char* charTname, const std::collate<charT>& co,
             const char* str1, const char* str2,
             int expected_val)
{
    // convert narrow string to a (possibly) wide representation
    charT wstrbuf [256];
    charT wstrbuf2 [256];

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
void
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
    rw_assert (expected == actual, __FILE__, __LINE__,
               "collate<%s>::transform (\"%c\", ...) == %{*.*Ac}, "
               "got %{*.*Ac}", charTname, ch, sizeof (charT),
               expected.size (), expected.c_str (), sizeof (charT), 
               actual.size (), actual.c_str ());
}

/**************************************************************************/

template <class charT>
void
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

            const char* in_path = std::getenv ("TOPDIR");
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

                char next_line [256];

                if (0 != std::fgets (next_line, 256, f)) {

                    std::size_t line_len = std::strlen (next_line);

                    // get rid of the newline character
                    next_line [--line_len] = '\0';

                    // convert from external to internal encoding
                    // (both of which might be the same type)
                    charT to [256];
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
void
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
void
check_NUL_locale (const char* charTname, const char* locname)
{
    std::locale loc (locname);

    charT s [STR_SIZE];
    gen_str (s, STR_SIZE);

    charT buf [2][STR_SIZE];

    std::memcpy (buf [0], s, sizeof s);
    std::memcpy (buf [1], s, sizeof s);

    //
    // Verify that first buffer compares more:
    // |--------0----| = buf [0]
    // |----0--------| = buf [1]
    // 
    buf [0][4] = charT ();
    buf [1][3] = charT ();

    typedef std::collate<charT> Collate;

    const Collate &col = std::use_facet<Collate> (loc);

    int cmp = col.compare (
        buf [0], buf [0] + sizeof buf [0] / sizeof *buf [0], 
        buf [1], buf [1] + sizeof buf [1] / sizeof *buf [1]);

    rw_assert (cmp > 0, __FILE__, __LINE__,
               "collate<%s>::compare (%{*.*Ac}, %{*.*Ac}) "
               " > 0, failed in locale (\"%s\")", charTname,
               sizeof (charT), sizeof buf [0] / sizeof *buf [0], buf [0],
               sizeof (charT), sizeof buf [1] / sizeof *buf [1], buf [1],
               locname);

    std::memcpy (buf [0], s, sizeof s);
    std::memcpy (buf [1], s, sizeof s);

    //
    // Verify that first compare less:
    // |----0---0----| = buf [0]
    // |----0--------| = buf [1]
    // 
    buf [0][3] = charT ();
    buf [0][5] = charT ();
    buf [1][3] = charT ();

    cmp = col.compare (
        buf [0], buf [0] + sizeof buf [0] / sizeof *buf [0], 
        buf [1], buf [1] + sizeof buf [1] / sizeof *buf [1]);

    rw_assert (cmp < 0, __FILE__, __LINE__,
               "collate<%s>::compare (%{*.*Ac}, ..., %{*.*Ac}, ...) "
               " < 0, failed in locale (\"%s\")", charTname,
               sizeof (charT), sizeof buf [0] / sizeof *buf [0], buf [0],
               sizeof (charT), sizeof buf [1] / sizeof *buf [1], buf [1],
               locname);
}

template <class charT>
void
check_NUL (const char* charTname)
{
    // Verify that the collate facet correctly handles character
    // sequences with embedded NULs.

    rw_info (0, __FILE__, __LINE__,
             "std::collate<%s>::compare () with embedded NUL's", charTname);

    size_t i = 0;

    for (const char* locname = rw_locales (LC_COLLATE); 
         *locname; locname += std::strlen (locname) + 1, ++i) {
        try {
            check_NUL_locale<charT> (charTname, locname);
        }
        catch (...) {
        }
    }
}

/**************************************************************************/

template <class charT>
void
do_test (const char* charTname)
{
    check_libstd_test_locale<charT> (charTname);
    check_libstd<charT> (charTname);
    check_libc<charT> (charTname);
    check_NUL<charT> (charTname);
    check_hash_eff<charT> (charTname);
}

static int
run_test (int /*argc*/, char* /*argv*/ [])
{
    do_test<char> ("char");

#if defined (_RWSTD_NO_WCHAR_T)
    do_test<wchar_t> ("wchar_t");
#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}


int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[lib.category.collate]",
                    "22.2.4 The collate category",
                    run_test, "", 0);
}

