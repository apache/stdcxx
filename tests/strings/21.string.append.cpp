/***************************************************************************
 *
 * 21.string.append.cpp - string test exercising lib.string.append
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 2006 Rogue Wave Software.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 **************************************************************************/

#include <string>         // for string
#include <stdexcept>      // for out_of_range, length_error
#include <cstddef>        // for size_t

#include <21.strings.h>   // for StringMembers
#include <alg_test.h>     // for InputIter
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_char.h>      // for rw_expand()
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Append(sig)             StringIds::append_ ## sig
#define PushBack(sig)           StringIds::push_back_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// append (const value_type*)
static const StringTestCase
cptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {                           \
        __LINE__, -1, -1, -1, -1, -1,                           \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |             +--------------------------- sequence to be appended
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info
    //    |             |             |        |         0 - no exception
    //    |             |             |        |         1 - out_of_range
    //    |             |             |        |         2 - length_error
    //    |             |             |        |        -1 - exc. safety
    //    |             |             |        |
    //    |             |             |        +--------------+
    //    V             V             V                       V
    TEST ("ab",         "c",          "abc",                  0),

    TEST ("",           "",           "",                     0),
    TEST ("",           "\0",         "",                     0),
    TEST ("",           "abc",        "abc",                  0),

    TEST ("\0",         "",           "\0",                   0),
    TEST ("\0",         "a",          "\0a",                  0),
    TEST ("\0",         "\0\0",       "\0",                   0),

    TEST ("ab",         "cd",         "abcd",                 0),
    TEST ("bcd",        "a",          "bcda",                 0),
    TEST ("cde",        "ab",         "cdeab",                0),
    TEST ("abc",        "",           "abc",                  0),
    TEST ("ab",         "c\0e",       "abc",                  0),

    TEST ("\0\0ab",     "cdefghij",   "\0\0abcdefghij",       0),
    TEST ("a\0\0b",     "cdefghij",   "a\0\0bcdefghij",       0),
    TEST ("ab\0\0",     "cdefghij",   "ab\0\0cdefghij",       0),
    TEST ("a\0b\0\0c",  "e\0",        "a\0b\0\0ce",           0),
    TEST ("\0ab\0\0c",  "e\0",        "\0ab\0\0ce",           0),
    TEST ("abcdefghij", "abcdefghij", "abcdefghijabcdefghij", 0),

    TEST ("",           "x@4096",     "x@4096",               0),
    TEST ("x@4096",     "",           "x@4096",               0),
    TEST ("x@4096",     "x@4096",     "x@8192",               0),

    TEST ("",           0,            "",                     0),
    TEST ("abc",        0,            "abcabc",               0),
    TEST ("a\0\0bc",    0,            "a\0\0bca",             0),
    TEST ("\0\0abc",    0,            "\0\0abc",              0),
    TEST ("abc\0\0",    0,            "abc\0\0abc",           0),

    TEST ("last",       "test",       "lasttest",             0)
};

/**************************************************************************/

// exercises:
// append (const basic_string&)
static const StringTestCase
cstr_test_cases [] = {

#undef TEST
#define TEST(s, arg, res, bthrow) {                             \
        __LINE__, -1, -1, -1, -1, -1,                           \
        s, sizeof s - 1,                                        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |             +--------------------------- sequence to be appended
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info
    //    |             |             |        |         0 - no exception
    //    |             |             |        |         1 - out_of_range
    //    |             |             |        |         2 - length_error
    //    |             |             |        |        -1 - exc. safety
    //    |             |             |        |
    //    |             |             |        +--------------+
    //    V             V             V                       V
    TEST ("ab",         "c",          "abc",                  0),

    TEST ("",           "",           "",                     0),
    TEST ("",           "\0",         "\0",                   0),
    TEST ("",           "abc",        "abc",                  0),

    TEST ("\0",         "",           "\0",                   0),
    TEST ("\0",         "a",          "\0a",                  0),
    TEST ("\0",         "\0\0",       "\0\0\0",               0),

    TEST ("ab",         "cd",         "abcd",                 0),
    TEST ("bcd",        "a",          "bcda",                 0),
    TEST ("cde",        "ab",         "cdeab",                0),
    TEST ("abc",        "",           "abc",                  0),
    TEST ("ab",         "c\0e",       "abc\0e",               0),

    TEST ("\0\0ab",     "cdefghij",   "\0\0abcdefghij",       0),
    TEST ("a\0\0b",     "cdefghij",   "a\0\0bcdefghij",       0),
    TEST ("ab\0\0",     "cdefghij",   "ab\0\0cdefghij",       0),
    TEST ("a\0b\0\0c",  "e\0",        "a\0b\0\0ce\0",         0),
    TEST ("\0ab\0\0c",  "e\0",        "\0ab\0\0ce\0",         0),
    TEST ("ab\0\0c\0",  "\0e",        "ab\0\0c\0\0e",         0),
    TEST ("abcdefghij", "abcdefghij", "abcdefghijabcdefghij", 0),

    TEST ("",           "x@4096",     "x@4096",               0),
    TEST ("x@4096",     "",           "x@4096",               0),
    TEST ("x@4096",     "x@4096",     "x@8192",               0),

    TEST ("",           0,            "",                     0),
    TEST ("abc",        0,            "abcabc",               0),
    TEST ("a\0\0bc",    0,            "a\0\0bca\0\0bc",       0),
    TEST ("\0\0abc",    0,            "\0\0abc\0\0abc",       0),
    TEST ("abc\0\0",    0,            "abc\0\0abc\0\0",       0),

    TEST ("last",       "test",       "lasttest",             0)
};

/**************************************************************************/

// exercises:
// append (const value_type*, size_type)
static const StringTestCase
cptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, size, res, bthrow) {                     \
        __LINE__, -1, size, -1, -1, -1,                         \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- sequence to be appended
    //    |            |            +------------ append() n argument
    //    |            |            |  +--------- expected result sequence
    //    |            |            |  |     +--- exception info
    //    |            |            |  |     |      0 - no exception
    //    |            |            |  |     |      1 - out_of_range
    //    |            |            |  |     |      2 - length_error
    //    |            |            |  |     |     -1 - exc. safety
    //    |            |            |  |     |
    //    |            |            |  |     +------------+
    //    V            V            V  V                  V
    TEST ("ab",        "c",         1, "abc",             0),

    TEST ("",          "",          0,  "",               0),
    TEST ("",          "abc",       1,  "a",              0),
    TEST ("",          "\0",        1,  "\0",             0),

    TEST ("\0",        "",          0,  "\0",             0),
    TEST ("\0",        "a",         0,  "\0",             0),
    TEST ("\0",        "a",         1,  "\0a",            0),
    TEST ("\0",        "\0\0",      1,  "\0\0",           0),
    TEST ("\0",        "\0\0",      2,  "\0\0\0",         0),

    TEST ("cde",       "ab",        2,  "cdeab",          0),
    TEST ("cde",       "ab",        1,  "cdea",           0),

    TEST ("\0e\0",     "a\0b\0\0c", 0,  "\0e\0",          0),
    TEST ("\0e\0",     "\0ab\0\0c", 3,  "\0e\0\0ab",      0),

    TEST ("a\0b\0\0c", "\0e\0",     3,  "a\0b\0\0c\0e\0", 0),
    TEST ("a\0b\0\0c", "\0\0e\0",   2,  "a\0b\0\0c\0\0",  0),
    TEST ("\0ab\0\0c", "\0e\0",     1,  "\0ab\0\0c\0",    0),
    TEST ("a\0bc\0\0", "\0e",       2,  "a\0bc\0\0\0e",   0),

    TEST ("",          0,           0,  "",               0),
    TEST ("abc",       0,           0,  "abc",            0),
    TEST ("abc",       0,           1,  "abca",           0),
    TEST ("abc",       0,           2,  "abcab",          0),
    TEST ("a\0bc",     0,           2,  "a\0bca\0",       0),
    TEST ("\0abc\0\0", 0,           1,  "\0abc\0\0\0",    0),
    TEST ("a\0bc\0\0", 0,           3,  "a\0bc\0\0a\0b",  0),
    TEST ("a@4096",    0,        1111,  "a@5207",         0),
    TEST ("b@4096",    0,        2222,  "b@6318",         0),

    TEST ("",          "x@4096", 4096,  "x@4096",         0),
    TEST ("x@4096",    "",          0,  "x@4096",         0),

    TEST ("last",      "test",      4, "lasttest",        0)
};

/**************************************************************************/

// exercises:
// append (const basic_string&, size_type, size_type)
// append (InputIterator, InputIterator)
static const StringTestCase
range_test_cases [] = {

// range_test_cases serves a double duty
#define cstr_size_size_test_cases range_test_cases

#undef TEST
#define TEST(str, arg, off, size, res, bthrow) {                \
        __LINE__, off, size, -1, -1, -1,                        \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- sequence to be appended
    //    |            |            +------------ append() pos argument
    //    |            |            |  +--------- append() n argument
    //    |            |            |  |  +------ expected result sequence
    //    |            |            |  |  |  +--- exception info
    //    |            |            |  |  |  |       0 - no exception
    //    |            |            |  |  |  |       1 - out_of_range
    //    |            |            |  |  |  |       2 - length_error
    //    |            |            |  |  |  |      -1 - exc. safety
    //    |            |            |  |  |  |
    //    |            |            |  |  |  +----------------+
    //    V            V            V  V  V                   V
    TEST ("ab",        "c",         0, 1, "abc",              0),

    TEST ("",          "",          0, 0,  "",                0),
    TEST ("",          "abc",       1, 1,  "b",               0),
    TEST ("",          "\0",        0, 1,  "\0",              0),

    TEST ("\0",        "",          0, 0,  "\0",              0),

    TEST ("abc",       "",          0, 0,  "abc",             0),

    TEST ("\0",        "a",         0, 1,  "\0a",             0),
    TEST ("\0",        "\0\0",      1, 1,  "\0\0",            0),
    TEST ("\0",        "\0\0",      0, 2,  "\0\0\0",          0),
    TEST ("\0",        "\0\0",      1, 5,  "\0\0",            0),

    TEST ("cde",       "ab",        0, 2,  "cdeab",           0),
    TEST ("cde",       "ab",        0, 1,  "cdea",            0),
    TEST ("cde",       "ab",        1, 5,  "cdeb",            0),

    TEST ("ab",        "c\0e",      0, 3,  "abc\0e",          0),
    TEST ("ab",        "c\0e",      1, 2,  "ab\0e",           0),
    TEST ("ab",        "c\0e",      0, 2,  "abc\0",           0),

    TEST ("\0e\0",     "\0ab\0\0c", 0, 9,  "\0e\0\0ab\0\0c",  0),
    TEST ("\0e\0",     "\0ab\0\0c", 0, 3,  "\0e\0\0ab",       0),
    TEST ("a\0b\0\0c", "\0e\0",     0, 3,  "a\0b\0\0c\0e\0",  0),
    TEST ("a\0b\0\0c", "\0\0e\0",   0, 2,  "a\0b\0\0c\0\0",   0),
    TEST ("\0ab\0\0c", "\0e\0",     2, 1,  "\0ab\0\0c\0",     0),
    TEST ("a\0bc\0\0", "\0e",       0, 2,  "a\0bc\0\0\0e",    0),

    TEST ("",          0,           0, 0,  "",                0),
    TEST ("abc",       0,           1, 0,  "abc",             0),
    TEST ("abc",       0,           1, 1,  "abcb",            0),
    TEST ("abc",       0,           0, 2,  "abcab",           0),
    TEST ("a\0bc\0\0", 0,           4, 2,  "a\0bc\0\0\0\0",   0),
    TEST ("a\0bc\0\0", 0,           1, 3,  "a\0bc\0\0\0bc",   0),
    TEST ("a\0bc\0\0", 0,           3, 9,  "a\0bc\0\0c\0\0",  0),
    TEST ("abcdef",    0,           1, 2,  "abcdefbc",        0),

    TEST ("a@1000",    "b@1000",    0,  999, "a@1000b@999",   0),
    TEST ("a@1000",    "b@1001",    0, 1000, "a@1000b@1000",  0),
    TEST ("a@1000",    "b@1002",    0,  102, "a@1000b@102",   0),
    TEST ("",          "x@4096",    0, 4096, "x@4096",        0),
    TEST ("x@4096",    "",          0,    0, "x@4096",        0),
    TEST ("x@4096",    "a@4096",  100,   10, "x@4096a@10",    0),

    TEST ("",          "\0",        2, 0,  "",                1),
    TEST ("",          "a",         2, 0,  "",                1),
    TEST ("",          "x@4096", 4106, 0,  "",                1),

    TEST ("last",      "test",      0, 4, "lasttest",         0)
};

/**************************************************************************/

// exercises:
// append (value_type, size_type)
static const StringTestCase
size_val_test_cases [] = {

#undef TEST
#define TEST(str, size, val, res, bthrow) {     \
        __LINE__, -1, size, -1, -1, val,        \
        str, sizeof str - 1,                    \
        0, 0, res, sizeof res - 1, bthrow       \
    }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- append() count argument
    //    |            |   +--------------------- character to be appended
    //    |            |   |   +----------------- expected result sequence
    //    |            |   |   |       +--------- exception info
    //    |            |   |   |       |             0 - no exception
    //    |            |   |   |       |             1 - out_of_range
    //    |            |   |   |       |             2 - length_error
    //    |            |   |   |       |            -1 - exc. safety
    //    |            |   |   |       |
    //    |            |   |   |       +-----------+
    //    V            V   V   V                   V
    TEST ("ab",        1, 'c', "abc",              0),

    TEST ("",          0, ' ',  "",                0),
    TEST ("",          1, 'b',  "b",               0),
    TEST ("",          3, 'b',  "bbb",             0),

    TEST ("\0",        0, ' ',  "\0",              0),
    TEST ("",          2, '\0', "\0\0",            0),

    TEST ("\0",        1, 'a',  "\0a",             0),
    TEST ("\0",        1, '\0', "\0\0",            0),
    TEST ("\0",        2, '\0', "\0\0\0",          0),
    TEST ("\0",        0, '\0', "\0",              0),

    TEST ("cde",       1, 'a',  "cdea",            0),
    TEST ("cde",       2, 'a',  "cdeaa",           0),
    TEST ("cde",       3, 'a',  "cdeaaa",          0),

    TEST ("ab",        2, '\0', "ab\0\0",          0),
    TEST ("ab",        1, '\0', "ab\0",            0),
    TEST ("ab",        2, '\0', "ab\0\0",          0),

    TEST ("a\0b\0\0c", 2, '\0', "a\0b\0\0c\0\0",   0),
    TEST ("a\0b\0\0c", 1, '\0', "a\0b\0\0c\0",     0),
    TEST ("\0ab\0\0c", 3, '\0', "\0ab\0\0c\0\0\0", 0),
    TEST ("a\0bc\0\0", 2, 'a',  "a\0bc\0\0aa",     0),

    TEST ("",       4096, 'x', "x@4096",           0),
    TEST ("x@4096",    0, 'x', "x@4096",           0),

    TEST ("last",      4, 't', "lasttttt",         0)
};

/**************************************************************************/

// exercises:
// push_back (value_type)
static const StringTestCase
push_back_val_test_cases [] = {

#undef TEST
#define TEST(str, val, res, bthrow) {           \
        __LINE__, -1, -1, -1, -1, val,          \
        str, sizeof str - 1,                    \
        0, 0, res, sizeof res - 1, bthrow       \
    }

    //    +-------------------------------------- controlled sequence
    //    |            +--------------------- character to be appended
    //    |            |   +----------------- expected result sequence
    //    |            |   |       +--------- exception info
    //    |            |   |       |             0 - no exception
    //    |            |   |       |            -1 - exc. safety
    //    |            |   |       |
    //    |            |   |       +------------+
    //    V            V   V                    V
    TEST ("ab",        'c', "abc",              0),

    TEST ("",          'a', "a",                0),
    TEST ("",          '\0', "\0",              0),
    TEST ("\0",        'a', "\0a",              0),
    TEST ("\0",        '\0', "\0\0",            0),

    TEST ("a\0b\0\0c", '\0', "a\0b\0\0c\0",     0),
    TEST ("a\0bc\0\0", 'a', "a\0bc\0\0a",       0),
    TEST ("\0abc\0\0", 'a', "\0abc\0\0a",       0),

    TEST ("x@4095",    'x', "x@4096",           0),

    TEST ("last",      't', "lastt",            0)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator, class Iterator>
void test_append_range (const charT    *wstr,
                        std::size_t     wstr_len,
                        const charT    *warg,
                        std::size_t     warg_len,
                        std::size_t     res_len,
                        Traits*, 
                        Allocator*,
                        const Iterator &it,
                        const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;

    const char* const itname =
        tcase.arg ? type_name (it, (charT*)0) : "basic_string::iterator";

    // compute the size of the controlled sequence and the size
    // of the sequence denoted by the argument keeping in mind
    // that the latter may refer to the former
    const std::size_t size1 = wstr_len;
    const std::size_t size2 = tcase.arg ? warg_len : size1;

    // construct the string object to be modified
    String s_str (wstr, size1);

    // compute the offset and the extent (the number of elements)
    // of the second range into the argument of the function call
    const std::size_t off =
        std::size_t (tcase.off) < size2 ? std::size_t (tcase.off) : size2;
    const std::size_t ext =
        off + tcase.size < size2 ? std::size_t (tcase.size) : size2 - off;

    if (tcase.arg) {
        const charT* const beg = warg + off;
        const charT* const end = beg + ext;

        const Iterator first (beg, beg, end);
        const Iterator last  (end, beg, end);

        s_str.append (first, last);
    }
    else {
        // self-referential modification (appending a range
        // of elements with a subrange of its own elements)
        const StringIter first (s_str.begin () + off);
        const StringIter last (first + ext);

        s_str.append (first, last);
    }

    // detrmine whether the produced sequence matches the exepceted result
    const std::size_t match = rw_match (tcase.res, s_str.data (), tcase.nres);

    rw_assert (match == res_len, 0, tcase.line,
               "line %d. %{$FUNCALL} expected %{#*s}, got %{/*.*Gs}, "
               "difference at offset %zu for %s",
               __LINE__, int (tcase.nres), tcase.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_append_range (const charT    *wstr,
                        std::size_t     wstr_len,
                        const charT    *warg,
                        std::size_t     warg_len,
                        std::size_t     res_len,
                        Traits*,
                        Allocator*,
                        const StringTestCase &tcase)
{
    if (tcase.bthrow)  // this method doesn't throw
        return;

    test_append_range (wstr, wstr_len, warg, warg_len, res_len, (Traits*)0,
                       (Allocator*)0, InputIter<charT>(0, 0, 0), tcase);

    // there is no need to call test_append_range
    // for other iterators in this case
    if (0 == tcase.arg)
        return;

    test_append_range (wstr, wstr_len, warg, warg_len, res_len, (Traits*)0,
                       (Allocator*)0, ConstFwdIter<charT>(0, 0, 0), tcase);

    test_append_range (wstr, wstr_len, warg, warg_len, res_len, (Traits*)0,
                       (Allocator*)0, ConstBidirIter<charT>(0, 0, 0), tcase);

    test_append_range (wstr, wstr_len, warg, warg_len, res_len, 
                      (Traits*)0, (Allocator*)0, 
                       ConstRandomAccessIter<charT>(0, 0, 0), tcase);
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_append (charT, Traits*, Allocator*,
                  const StringFunc     &func,
                  const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;
    typedef typename UserTraits<charT>::MemFun           UTMemFun;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    static charT warg_buf [BUFSIZE];

    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    std::size_t arg_len = sizeof warg_buf / sizeof *warg_buf;

    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);
    charT* warg = rw_expand (warg_buf, tcase.arg, tcase.arg_len, &arg_len);

    static charT wres_buf [BUFSIZE];
    std::size_t res_len = sizeof wres_buf / sizeof *wres_buf;
    charT* wres = rw_expand (wres_buf, tcase.res, tcase.nres, &res_len);

    // special processing for append_range to exercise all iterators
    if (Append (range) == func.which_) {
        test_append_range (wstr, str_len, warg, arg_len, res_len, 
                          (Traits*)0, (Allocator*)0, tcase);

        if (wstr != wstr_buf)
            delete[] wstr;

        if (warg != warg_buf)
            delete[] warg;

         if (wres != wres_buf)
            delete[] wres;

        return;
    }

    // construct the string object to be modified
    // and the (possibly unused) argument string
    /* const */ String str (wstr, str_len);
    const       String arg (warg, arg_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    if (warg != warg_buf)
        delete[] warg;

    wstr = 0;
    warg = 0;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

    const charT* const ptr_arg = tcase.arg ? arg.c_str () : str.c_str ();
    const String&      str_arg = tcase.arg ? arg : str;
    const charT        val_arg = (make_char (char (tcase.val), (charT*)0));

    std::size_t total_length_calls = 0;
    std::size_t n_length_calls = 0;
    std::size_t* const rg_calls = rw_get_call_counters ((Traits*)0, (charT*)0);

    if (rg_calls)
        total_length_calls = rg_calls [UTMemFun::length];

    rwt_free_store* const pst = rwt_get_free_store (0);
    SharedAlloc*    const pal = SharedAlloc::instance ();

    // iterate for`throw_after' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_count;
    for (throw_count = 0; ; ++throw_count) {

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        if (1 == tcase.bthrow && Append (cstr_size_size) == func.which_)
            expected = exceptions [1];   // out_of_range
        else if (2 == tcase.bthrow)
            expected = exceptions [2];   // length_error
        else if (0 == tcase.bthrow) {
            // by default excercise the exception safety of the function
            // by iteratively inducing an exception at each call to operator
            // new or Allocator::allocate() until the call succeeds
            expected = exceptions [3];      // bad_alloc
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_count + 1;
            pal->throw_at_calls_ [pal->m_allocate] =
                pal->throw_at_calls_ [pal->m_allocate] + throw_count + 1;
        }
        else {
            // exceptions disabled for this test case
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow) {
            if (wres != wres_buf)
                delete[] wres;

            return;
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        // pointer to the returned reference
        const String* ret_ptr = 0;

        // start checking for memory leaks
        rw_check_leaks (str.get_allocator ());

        try {

            switch (func.which_) {

            case Append (cptr):
                ret_ptr = &str.append (ptr_arg);
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;

            case Append (cstr):
                ret_ptr = &str.append (str_arg);
                break;

            case Append (cptr_size):
                ret_ptr = &str.append (ptr_arg, tcase.size);
                break;

            case Append (cstr_size_size):
                ret_ptr = &str.append (str_arg, tcase.off, tcase.size);
                break;

            case Append (size_val):
                ret_ptr = &str.append (tcase.size, val_arg);
                break;

            case PushBack (val):
                str.push_back (val_arg);
                ret_ptr = &str;
                break;

            default:
                RW_ASSERT (!"logic error: unknown append overload");
            }

            // verify that the reference returned from the function
            // refers to the modified string object (i.e., *this
            // within the function)
            const std::ptrdiff_t ret_off = ret_ptr - &str;

            // verify the returned value
            rw_assert (0 == ret_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} returned invalid reference, "
                       "offset is %td", __LINE__, ret_off);

            // verify that strings are of equal length
            rw_assert (res_len == str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{#*s}, "
                       "length %zu, got %{/*.*Gs}, length %zu",
                       __LINE__, int (tcase.nres), tcase.res,
                       res_len, int (sizeof (charT)),
                       int (str.size ()), str.c_str (), str.size ());

            if (res_len == str.size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, str.c_str (), tcase.nres);

                rw_assert (match == res_len, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %{#*s}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, int (tcase.nres), tcase.res,
                           int (sizeof (charT)), int (str.size ()),
                           str.c_str (), match);
            }

            // verify that Traits::length was used
            if (Append (cptr) == func.which_ && rg_calls) {
                rw_assert (n_length_calls - total_length_calls > 0, 
                           0, tcase.line, "line %d. %{$FUNCALL} doesn't "
                           "use traits::length()", __LINE__);
            }
        }

#ifndef _RWSTD_NO_EXCEPTIONS

        catch (const std::out_of_range &ex) {
            caught = exceptions [1];
            rw_assert (caught == expected, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught, ex.what ());
        }
        catch (const std::length_error &ex) {
            caught = exceptions [2];
            rw_assert (caught == expected, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught, ex.what ());
        }
        catch (const std::bad_alloc &ex) {
            caught = exceptions [3];
            rw_assert (0 == tcase.bthrow, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught, ex.what ());
        }
        catch (const std::exception &ex) {
            caught = exceptions [4];
            rw_assert (0, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught, ex.what ());
        }
        catch (...) {
            caught = exceptions [0];
            rw_assert (0, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught %s",
                       __LINE__, 0 != expected, expected, caught);
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        // FIXME: verify the number of blocks the function call
        // is expected to allocate and detect any memory leaks
        rw_check_leaks (str.get_allocator (), tcase.line,
                        std::size_t (-1), std::size_t (-1));

        if (caught) {
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object
            str_state.assert_equal (rw_get_string_state (str),
                                    __LINE__, tcase.line, caught);

            if (0 == tcase.bthrow) {
                // allow this call to operator new to succeed and try
                // to make the next one to fail during the next call
                // to the same function again
                continue;
            }
        }
        else if (0 < tcase.bthrow) {
            rw_assert (caught == expected, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                       "%{:}unexpectedly caught %s%{;}",
                       __LINE__, 0 != expected, expected, caught, caught);
        }

        break;
    }

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify that if exceptions are enabled and when capacity changes
    // at least one exception is thrown
    const std::size_t expect_throws = str_state.capacity_ < str.capacity ();

#else   // if defined (_RWSTD_NO_REPLACEABLE_NEW_DELETE)

    const std::size_t expect_throws = 
        (StringIds::UserAlloc == func.alloc_id_) 
      ? str_state.capacity_ < str.capacity (): 0;

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rw_assert (expect_throws == throw_count, 0, tcase.line,
               "line %d: %{$FUNCALL}: expected exactly 1 %s exception "
               "while changing capacity from %zu to %zu, got %zu",
               __LINE__, exceptions [3],
               str_state.capacity_, str.capacity (), throw_count);

    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;

    if (wres != wres_buf)
        delete[] wres;
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_append);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Append (sig), sig ## _test_cases,                       \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (cptr),
        TEST (cstr),
        TEST (cptr_size),
        TEST (cstr_size_size),
        TEST (size_val),
        TEST (range),

        { 
            StringIds::push_back_val, push_back_val_test_cases,
            sizeof push_back_val_test_cases / sizeof *push_back_val_test_cases
        }
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.append",
                               test_append, tests, test_count);
}
