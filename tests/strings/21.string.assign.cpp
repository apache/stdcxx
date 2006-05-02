/***************************************************************************
 *
 * 21.string.assign.cpp - test exercising [lib.string.assign]
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
#include <cstddef>        // for size_t
#include <stdexcept>      // for out_of_range, length_error

#include <21.strings.h>   // for StringMembers
#include <alg_test.h>     // for InputIter
#include <driver.h>       // for rw_test()
#include <rw_char.h>      // for rw_widen()

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
   // disabled for compilers such as IBM VAC++ or MSVC
   // that can't reliably replace the operators
#  include <rw_new.h>   // for bad_alloc, replacement operator new
#else
#  include <new>        // for bad_alloc
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE


#define AssignOverload   StringMembers::OverloadId
#define Assign(which)    StringMembers::assign_ ## which

typedef StringMembers::TestCase TestCase;
typedef StringMembers::Test     Test;
typedef StringMembers::Function MemFun;

/**************************************************************************/

// for convenience and brevity
#define LSTR   StringMembers::long_string
#define LLEN   StringMembers::long_string_len

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise:
// assign (const value_type*)
static const TestCase ptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {                           \
        __LINE__, -1, -1, -1, -1, -1,                           \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |             +--------------------------- sequence to be assigned
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info
    //    |             |             |        |      0 - no exception
    //    |             |             |        |      1 - out_of_range
    //    |             |             |        |      2 - length_error
    //    |             |             |        |     -1 - exc. safety
    //    |             |             |        |
    //    |             |             |        +-------+
    //    V             V             V                V
    TEST ("ab",         "c",          "c",             0),

    TEST ("",           "",           "",              0),
    TEST ("",           "\0",         "",              0),
    TEST ("",           "abc",        "abc",           0),
    TEST ("abc",        "",           "",              0),

    TEST ("\0",         "",           "",              0),
    TEST ("\0",         "a",          "a",             0),
    TEST ("\0",         "\0\0",       "",              0),

    TEST ("\0\0ab",     "cdefghij",   "cdefghij",      0),
    TEST ("a\0\0b",     "cdefghij",   "cdefghij",      0),
    TEST ("ab\0\0",     "cdefghij",   "cdefghij",      0),
    TEST ("a\0b\0\0c",  "e\0",        "e",             0),
    TEST ("\0ab\0\0c",  "e\0",        "e",             0),

    TEST ("",           LSTR,         LSTR,            0),
    TEST (LSTR,         "",           "",              0),

    TEST ("",           0,            "",              0),
    TEST ("abc",        0,            "abc",           0),
    TEST ("a\0\0bc",    0,            "a",             0),
    TEST ("\0\0abc",    0,            "",              0),
    TEST ("abc\0\0",    0,            "abc",           0),

    TEST ("",           LSTR,         LSTR,           -1),

    TEST ("last",       "test",       "test",          0)
};

/**************************************************************************/

// used to exercise:
// assign (const basic_string&)
static const TestCase str_test_cases [] = {

#undef TEST
#define TEST(s, arg, res, bthrow) {                             \
        __LINE__, -1, -1, -1, -1, -1,                           \
        s, sizeof s - 1,                                        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |             +--------------------------- sequence to be assigned
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info:
    //    |             |             |        |      0 - no exception
    //    |             |             |        |      1 - out_of_range
    //    |             |             |        |      2 - length_error
    //    |             |             |        |     -1 - exc. safety
    //    |             |             |        |
    //    |             |             |        +-----+
    //    |             |             |              |
    //    V             V             V              V
    TEST ("ab",         "c",          "c",           0),

    TEST ("",           "",           "",            0),
    TEST ("",           "\0",         "\0",          0),
    TEST ("",           "abc",        "abc",         0),
    TEST ("abc",        "",           "",            0),

    TEST ("\0",         "",           "",            0),
    TEST ("\0",         "a",          "a",           0),
    TEST ("\0",         "\0\0",       "\0\0",        0),

    TEST ("ab",         "c\0e",       "c\0e",        0),

    TEST ("\0\0ab",     "cdefghij",   "cdefghij",    0),
    TEST ("a\0\0b",     "cdefghij",   "cdefghij",    0),
    TEST ("ab\0\0",     "cdefghij",   "cdefghij",    0),
    TEST ("a\0b\0\0c",  "e\0",        "e\0",         0),
    TEST ("\0ab\0\0c",  "e\0",        "e\0",         0),
    TEST ("ab\0\0c\0",  "\0e",        "\0e",         0),

    TEST ("e\0",        "a\0b\0\0c",  "a\0b\0\0c",   0),
    TEST ("e\0\0",      "\0ab\0\0c",  "\0ab\0\0c",   0),
    TEST ("\0e",        "ab\0\0c\0",  "ab\0\0c\0",   0),

    TEST ("",           LSTR,         LSTR,          0),
    TEST (LSTR,         "",           "",            0),

    TEST ("",           0,            "",            0),
    TEST ("abc",        0,            "abc",         0),
    TEST ("a\0\0bc",    0,            "a\0\0bc",     0),
    TEST ("\0\0abc",    0,            "\0\0abc",     0),
    TEST ("abc\0\0",    0,            "abc\0\0",     0),

    TEST ("last",       "test",       "test",        0)
};

/**************************************************************************/

// used to exercise:
// assign (const value_type*, size_type)
static const TestCase ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, size, res, bthrow) {                     \
        __LINE__, -1, size, -1, -1, -1,                         \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- sequence to be assigned
    //    |            |            +--------------- assign() n argument
    //    |            |            |  +------------ expected result sequence
    //    |            |            |  |     +------ exception info
    //    |            |            |  |     |        0 - no exception
    //    |            |            |  |     |        1 - out_of_range
    //    |            |            |  |     |        2 - length_error
    //    |            |            |  |     |       -1 - exc. safety
    //    |            |            |  |     |
    //    |            |            |  |     +--------+
    //    V            V            V  V              V
    TEST ("ab",        "c",         1, "c",           0),

    TEST ("",          "",          0,  "",           0),
    TEST ("",          "abc",       1,  "a",          0),
    TEST ("",          "\0",        1,  "\0",         0),

    TEST ("\0",        "",          0,  "",           0),
    TEST ("\0",        "a",         0,  "",           0),
    TEST ("\0",        "a",         1,  "a",          0),
    TEST ("\0",        "\0\0",      1,  "\0",         0),
    TEST ("\0",        "\0\0",      2,  "\0\0",       0),

    TEST ("cde",       "ab",        2,  "ab",         0),
    TEST ("cde",       "ab",        1,  "a",          0),

    TEST ("\0e\0",     "a\0b\0\0c", 0,  "",           0),
    TEST ("\0e\0",     "\0ab\0\0c", 3,  "\0ab",       0),
    TEST ("\0e\0",     "a\0b\0\0c", 6,  "a\0b\0\0c",  0),

    TEST ("a\0b\0\0c", "\0e\0",     3,  "\0e\0",      0),
    TEST ("a\0b\0\0c", "\0\0e\0",   2,  "\0\0",       0),
    TEST ("\0ab\0\0c", "\0e\0",     1,  "\0",         0),
    TEST ("a\0bc\0\0", "\0e",       2,  "\0e",        0),

    TEST ("",          0,           0,  "",           0),
    TEST ("abc",       0,           0,  "",           0),
    TEST ("abc",       0,           1,  "a",          0),
    TEST ("abc",       0,           2,  "ab",         0),
    TEST ("a\0bc",     0,           2,  "a\0",        0),
    TEST ("\0abc\0\0", 0,           1,  "\0",         0),
    TEST ("a\0bc\0\0", 0,           3,  "a\0b",       0),
    TEST ("a\0bc\0\0", 0,           6,  "a\0bc\0\0",  0),

    TEST ("",          LSTR, LLEN - 1,  LSTR,         0),
    TEST ("abcd",      LSTR,        0,  "",           0),
    TEST (LSTR,        LSTR,        0,  "",           0),
    TEST (LSTR,        LSTR,        1,  "x",          0),
    TEST (LSTR,        "",          0,  "",           0),

    TEST ("",          "",         -1,  "",           2),
    TEST ("",          LSTR, LLEN - 1,  LSTR,        -1),

    TEST ("last",      "test",      4, "test",        0)
};

/**************************************************************************/

// used to exercise:
// assign (const basic_string&, size_type, size_type)
// assign (InputIterator, InputIterator)
static const TestCase range_test_cases [] = {

// range_test_cases serves a double duty
#define str_size_size_test_cases range_test_cases

#undef TEST
#define TEST(str, arg, off, size, res, bthrow) {                \
        __LINE__, off, size, -1, -1, -1,                        \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- sequence to be inserted
    //    |            |            +--------------- assign() pos argument
    //    |            |            |  +------------ assign() n argument
    //    |            |            |  |  +--------- expected result sequence
    //    |            |            |  |  |  +------ exception info
    //    |            |            |  |  |  |        0 - no exception
    //    |            |            |  |  |  |        1 - out_of_range
    //    |            |            |  |  |  |        2 - length_error
    //    |            |            |  |  |  |       -1 - exc. safety
    //    |            |            |  |  |  |
    //    |            |            |  |  |  +-----------+
    //    V            V            V  V  V              V
    TEST ("ab",        "c",         0, 1, "c",           0),

    TEST ("",          "",          0, 0,  "",           0),
    TEST ("",          "abc",       1, 1,  "b",          0),
    TEST ("",          "\0",        0, 1,  "\0",         0),

    TEST ("\0",        "",          0, 0,  "",           0),

    TEST ("abc",       "",          0, 0,  "",           0),

    TEST ("\0",        "a",         0, 1,  "a",          0),
    TEST ("\0",        "\0\0",      1, 1,  "\0",         0),
    TEST ("\0",        "\0\0",      0, 2,  "\0\0",       0),
    TEST ("\0",        "\0\0",      1, 5,  "\0",         0),

    TEST ("cde",       "ab",        0, 2,  "ab",         0),
    TEST ("cde",       "ab",        0, 1,  "a",          0),
    TEST ("cde",       "ab",        1, 5,  "b",          0),

    TEST ("ab",        "c\0e",      0, 3,  "c\0e",       0),
    TEST ("ab",        "c\0e",      1, 2,  "\0e",        0),
    TEST ("ab",        "c\0e",      0, 2,  "c\0",        0),

    TEST ("\0e\0",     "\0ab\0\0c", 0, 9,  "\0ab\0\0c",  0),
    TEST ("\0e\0",     "\0ab\0\0c", 0, 3,  "\0ab",       0),
    TEST ("a\0b\0\0c", "\0e\0",     0, 3,  "\0e\0",      0),
    TEST ("a\0b\0\0c", "\0\0e\0",   0, 2,  "\0\0",       0),
    TEST ("\0ab\0\0c", "\0e\0",     2, 1,  "\0",         0),
    TEST ("\0ab\0\0c", "\0e\0",     2, 9,  "\0",         0),
    TEST ("a\0bc\0\0", "\0e",       0, 2,  "\0e",        0),

    TEST ("",          0,           0, 0,  "",           0),
    TEST ("abc",       0,           1, 0,  "",           0),
    TEST ("abc",       0,           1, 1,  "b",          0),
    TEST ("abc",       0,           0, 2,  "ab",         0),
    TEST ("a\0bc\0\0", 0,           4, 2,  "\0\0",       0),
    TEST ("a\0bc\0\0", 0,           1, 3,  "\0bc",       0),
    TEST ("a\0bc\0\0", 0,           0, 9,  "a\0bc\0\0",  0),
    TEST ("abcdef",    0,           1, 2,  "bc",         0),

    TEST (LSTR,        "",          0, 0,  "",           0),
    TEST ("",          LSTR,        9, 2,  "xx",         0),
    TEST ("",          LSTR,        9, 0,  "",           0),
    TEST ("abc",       LSTR,        2, 1,  "x",          0),
    TEST (LSTR,        LSTR,        2, 3,  "xxx",        0),
    TEST ("",          LSTR,        0, LLEN, LSTR,       0),

    TEST ("",          "\0",        2, 0,  "",           1),
    TEST ("",          "a",         2, 0,  "",           1),
    TEST ("",          LSTR,LLEN + 10, 0,  "",           1),
    TEST ("",          LSTR,        0, LLEN - 1, LSTR,  -1),

    TEST ("last",      "test",      0, 4, "test",        0)
};

/**************************************************************************/

// used to exercise:
// assign (size_type, value_type)
static const TestCase size_val_test_cases [] = {

#undef TEST
#define TEST(str, size, val, res, bthrow) {     \
        __LINE__, -1, size, -1, -1, val,        \
        str, sizeof str - 1,                    \
        0, 0, res, sizeof res - 1, bthrow       \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- assign() count argument
    //    |            |   +------------------------ character to be assigned
    //    |            |   |   +-------------------- expected result sequence
    //    |            |   |   |       +------------ exception info
    //    |            |   |   |       |              0 - no exception
    //    |            |   |   |       |              1 - out_of_range
    //    |            |   |   |       |              2 - length_error
    //    |            |   |   |       |             -1 - exc. safety
    //    |            |   |   |       |
    //    |            |   |   |       +--------+
    //    V            V   V   V                V
    TEST ("ab",        1, 'c', "c",             0),

    TEST ("",          0, ' ',  "",             0),
    TEST ("",          1, 'b',  "b",            0),
    TEST ("",          3, 'b',  "bbb",          0),

    TEST ("\0",        0, ' ',  "",             0),
    TEST ("",          2, '\0', "\0\0",         0),

    TEST ("\0",        1, 'a',  "a",            0),
    TEST ("\0",        1, '\0', "\0",           0),
    TEST ("\0",        2, '\0', "\0\0",         0),
    TEST ("\0",        0, '\0', "",             0),

    TEST ("cde",       3, 'a',  "aaa",          0),
    TEST ("ab",        2, '\0', "\0\0",         0),
    TEST ("ab",        1, '\0', "\0",           0),

    TEST ("a\0b\0\0c", 2, '\0', "\0\0",         0),
    TEST ("a\0b\0\0c", 1, '\0', "\0",           0),
    TEST ("\0ab\0\0c", 3, '\0', "\0\0\0",       0),
    TEST ("a\0bc\0\0", 2, 'a',  "aa",           0),

    TEST ("",   LLEN - 1, 'x',  LSTR,           0),
    TEST (LSTR,        0, 'x',  "",             0),

    TEST ("",         -1, 'x',  "",             2),
    TEST ("",   LLEN - 1, 'x',  LSTR,          -1),

    TEST ("last",      4, 't',  "tttt",         0)
};

/**************************************************************************/

template <class charT, class Traits, class Iterator>
void test_assign_range (charT          *wstr,
                        charT          *wsrc,
                        Traits*,
                        const Iterator &it,
                        const TestCase &tcase)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;

    const char* const itname =
        tcase.arg ? type_name (it, (charT*)0) : "basic_string::iterator";

    /* const */ String s_str (wstr, tcase.str_len);
    const       String s_arg (wsrc, tcase.arg_len);

    std::size_t off_last = tcase.off + tcase.size;

    if (tcase.arg) {
        if (off_last > s_arg.size ())
            off_last = s_arg.size ();

        const Iterator
            first (wsrc + tcase.off, wsrc + tcase.off, wsrc + off_last);
        const Iterator
            last  (wsrc + off_last, wsrc + tcase.off, wsrc + off_last);

        s_str.assign (first, last);
    }
    else {

        const StringIter first (s_str.begin () + tcase.off);
        const StringIter last  (off_last > s_str.size () ?
            s_str.end ()
          : s_str.begin () + off_last);

        s_str.assign (first, last);
    }

    const std::size_t match =
        rw_match (tcase.res, s_str.c_str(), tcase.nres);

    rw_assert (match == tcase.nres, 0, tcase.line,
               "line %d. %{$FUNCALL} expected %{#*s}, got %{/*.*Gs}, "
               "difference at off %zu for %s",
               __LINE__, int (tcase.nres), tcase.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits>
void test_assign_range (charT          *wstr,
                        charT          *wsrc,
                        Traits*,
                        const TestCase &tcase)
{
    if (tcase.bthrow)  // this method doesn't throw
        return;

    test_assign_range (wstr, wsrc, (Traits*)0,
                       InputIter<charT>(0, 0, 0), tcase);

    // there is no need to call test_assign_range
    // for other iterators in this case
    if (0 == tcase.arg)
        return;

    test_assign_range (wstr, wsrc, (Traits*)0,
                       ConstFwdIter<charT>(0, 0, 0), tcase);

    test_assign_range (wstr, wsrc, (Traits*)0,
                       ConstBidirIter<charT>(0, 0, 0), tcase);

    test_assign_range (wstr, wsrc, (Traits*)0,
                       ConstRandomAccessIter<charT>(0, 0, 0), tcase);
}

/**************************************************************************/

template <class charT, class Traits>
void test_assign (charT, Traits*,
                  AssignOverload  which,
                  const TestCase &tcase)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::iterator                StringIter;
    typedef typename TestString::const_iterator          ConstStringIter;

    static charT wstr [LLEN];
    static charT wsrc [LLEN];

    rw_widen (wstr, tcase.str, tcase.str_len);
    rw_widen (wsrc, tcase.arg, tcase.arg_len);

    // special processing for assign_range to exercise all iterators
    if (Assign (range) == which) {
        test_assign_range (wstr, wsrc, (Traits*)0, tcase);
        return;
    }

    /* const */ TestString s_str (wstr, tcase.str_len);
    const       TestString s_arg (wsrc, tcase.arg_len);

    std::size_t res_off = 0;
    std::size_t throw_after = 0;
    std::size_t size = tcase.size >= 0 ? tcase.size : s_str.max_size () + 1;

    const std::size_t     ssize    = s_str.size ();
    const std::size_t     capacity = s_str.capacity ();
    const ConstStringIter begin    = s_str.begin ();

    // first function argument
    const charT* const arg_ptr = tcase.arg ? wsrc : s_str.c_str ();
    const TestString&  arg_str = tcase.arg ? s_arg : s_str;

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rwt_free_store* const pst = rwt_get_free_store (0);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // iterate for`n=throw_after' starting at the next call to operator
    // new, forcing each call to throw an exception, until the appendion
    // finally succeeds (i.e, no exception is thrown)
    for ( ; ; ) {

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        if (-1 == tcase.bthrow)
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#endif   // _RWSTD_NO_EXCEPTIONS

#ifndef _RWSTD_NO_EXCEPTIONS

        // is some exception expected ?
        const char* expected = 0;
        if (1 == tcase.bthrow && Assign (str_size_size) == which)
            expected = exceptions [1];   // out_of_range
        if (2 == tcase.bthrow)
            expected = exceptions [2];   // length_error
        if (-1 == tcase.bthrow)
            expected = exceptions [3];   // bad_alloc

        const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow)
            return;

#endif   // _RWSTD_NO_EXCEPTIONS

        try {
            switch (which) {
            case Assign (ptr): {
                const TestString& s_res = s_str.assign (arg_ptr);
                res_off = &s_res - &s_str;
                break;
            }

            case Assign (str): {
                const TestString& s_res = s_str.assign (arg_str);
                res_off = &s_res - &s_str;
                break;
            }

            case Assign (ptr_size): {
                const TestString& s_res = s_str.assign (arg_ptr, size);
                res_off = &s_res - &s_str;
                break;
            }

            case Assign (str_size_size): {
                const TestString& s_res = 
                    s_str.assign (arg_str, tcase.off, size);
                res_off = &s_res - &s_str;
                break;
            }

            case Assign (size_val): {
                const charT val = make_char (char (tcase.val), (charT*)0);
                const TestString& s_res = s_str.assign (size, val);
                res_off = &s_res - &s_str;
                break;
            }

            default:
                RW_ASSERT (!"test logic error: unknown assign overload");
            }


            // verify the returned value
            rw_assert (0 == res_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} returned invalid reference, "
                       "offset is %zu", __LINE__, res_off);

            // verfiy that strings length are equal
            rw_assert (tcase.nres == s_str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL}: expected %{#*s} with length "
                       "%zu, got %{/*.*Gs} with length %zu", __LINE__, 
                       int (tcase.nres), tcase.res, tcase.nres, 
                       int (sizeof (charT)), int (s_str.size ()), 
                       s_str.c_str (), s_str.size ());

            if (tcase.nres == s_str.size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, s_str.c_str(), tcase.nres);

                rw_assert (match == tcase.nres, 0, tcase.line,
                           "line %d. %{$FUNCALL}: expected %{#*s}, "
                           "got %{/*.*Gs}, difference at off %zu",
                           __LINE__, int (tcase.nres), tcase.res,
                           int (sizeof (charT)), int (s_str.size ()), 
                           s_str.c_str (), match);
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
            rw_assert (-1 == tcase.bthrow, 0, tcase.line,
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

        if (caught) {
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object

            rw_assert (s_str.size () == ssize, 0, tcase.line,
                       "line %d: %{$FUNCALL}: size unexpectedly changed "
                       "from %zu to %zu after an exception",
                       __LINE__, ssize, s_str.size ());

            rw_assert (s_str.capacity () == capacity, 0, tcase.line,
                       "line %d: %{$FUNCALL}: capacity unexpectedly "
                       "changed from %zu to %zu after an exception",
                       __LINE__, capacity, s_str.capacity ());

            rw_assert (s_str.begin () == begin, 0, tcase.line,
                       "line %d: %{$FUNCALL}: begin() unexpectedly "
                       "changed from after an exception by %d",
                       __LINE__, s_str.begin () - begin);

            if (-1 == tcase.bthrow) {
                // increment to allow this call to operator new to succeed
                // and force the next one to fail, and try calling the same
                // function again
                ++throw_after;
                continue;
            }
        }
        else if (-1 != tcase.bthrow) {
            rw_assert (caught == expected, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                       "%{:}unexpectedly caught %s%{;}",
                       __LINE__, 0 != expected, expected, caught, caught);
        }

        break;
    }

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify that if exceptions are enabled and when capacity changes
    // at least one exception is thrown
    rw_assert (   *pst->throw_at_calls_ [0] == std::size_t (-1)
               || throw_after,
               0, tcase.line,
               "line %d: %{$FUNCALL}: failed to throw an expected exception",
               __LINE__);

    *pst->throw_at_calls_ [0] = std::size_t (-1);

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    _RWSTD_UNUSED (ssize);
    _RWSTD_UNUSED (capacity);
    _RWSTD_UNUSED (throw_after);

#endif   // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

DEFINE_TEST_DISPATCH (test_assign);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::assign_ ## tag,                          \
        tag ## _test_cases,                                     \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (ptr),
        TEST (str),
        TEST (ptr_size),
        TEST (str_size_size),
        TEST (size_val),
        TEST (range)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return StringMembers::run_test (argc, argv, __FILE__,
                                    "lib.string.assign",
                                    test_assign, tests, test_count);
}
