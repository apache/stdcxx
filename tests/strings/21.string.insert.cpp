/***************************************************************************
 *
 * 21.string.insert.cpp - string test exercising lib.string.insert
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
#include <rw_char.h>      // for rw_widen()
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity

#define LSTR            StringMembers::long_string
#define LLEN            StringMembers::long_string_len
#define Insert(which)   StringMembers::insert_ ## which

typedef StringMembers::OverloadId OverloadId;
typedef StringMembers::TestCase   TestCase;
typedef StringMembers::Test       Test;
typedef StringMembers::Function   MemFun;

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// insert (size_type pos, const charT* s)
static const TestCase
size_ptr_test_cases [] = {

#undef TEST
#define TEST(str, off, arg, res, bthrow) {                      \
        __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1,     \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- sequence to be inserted
    //    |            |  |           +------------- expected result sequence
    //    |            |  |           |        +---- exception info
    //    |            |  |           |        |         0 - no exception
    //    |            |  |           |        |         1 - out_of_range
    //    |            |  |           |        |         2 - length_error
    //    |            |  |           |        |        -1 - exc. safety
    //    |            |  |           |        |
    //    |            |  |           |        +------------+
    //    V            V  V           V                     V
    TEST ("ab",        0, "c",        "cab",                0),

    TEST ("",          0, "",         "",                   0),
    TEST ("",          0, "abc",      "abc",                0),

    TEST ("\0",        0, "",         "\0",                 0),
    TEST ("",          0, "\0",       "",                   0),

    TEST ("abc",       0, "",         "abc",                0),
    TEST ("abc",       1, "",         "abc",                0),
    TEST ("abc",       3, "",         "abc",                0),

    TEST ("\0",        0, "a",        "a\0",                0),
    TEST ("\0",        1, "a",        "\0a",                0),
    TEST ("\0",        0, "\0",       "\0",                 0),
    TEST ("\0",        1, "\0\0",     "\0",                 0),

    TEST ("cde",       0, "ab",       "abcde",              0),
    TEST ("cde",       1, "ab",       "cabde",              0),
    TEST ("cde",       2, "ab",       "cdabe",              0),
    TEST ("cde",       3, "ab",       "cdeab",              0),

    TEST ("\0ab\0\0c", 0, "\0e\0",    "\0ab\0\0c",          0),
    TEST ("a\0b\0\0c", 2, "e\0e",     "a\0eb\0\0c",         0),
    TEST ("a\0bc\0\0", 6, "e\0e",     "a\0bc\0\0e",         0),

    TEST ("",          0, 0,          "",                   0),
    TEST ("abc",       0, 0,          "abcabc",             0),
    TEST ("abc",       2, 0,          "ababcc",             0),
    TEST ("a\0bc\0\0", 0, 0,          "aa\0bc\0\0",         0),
    TEST ("a\0bc\0\0", 6, 0,          "a\0bc\0\0a",         0),

    TEST ("",          0, LSTR,       LSTR,                 0),
    TEST (LSTR,        0, "",         LSTR,                 0),

    TEST ("\0",        2, "",         "",                   1),
    TEST ("a",         2, "",         "",                   1),
    TEST (LSTR,LLEN + 10, "",         "",                   1),

    TEST ("",          0, LSTR,       LSTR,                -1),

    TEST ("last",      4, "test",     "lasttest",           0)
};

/**************************************************************************/

// used to exercise
// insert (size_type pos, const basic_string& str)
static const TestCase
size_str_test_cases [] = {

#undef TEST
#define TEST(str, off, arg, res, bthrow) {                      \
        __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1,     \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- sequence to be inserted
    //    |            |  |           +------------- expected result sequence
    //    |            |  |           |        +---- exception info
    //    |            |  |           |        |         0 - no exception
    //    |            |  |           |        |         1 - out_of_range
    //    |            |  |           |        |         2 - length_error
    //    |            |  |           |        |        -1 - exc. safety
    //    |            |  |           |        |
    //    |            |  |           |        +------------+
    //    V            V  V           V                     V
    TEST ("ab",        0, "c",        "cab",                0),

    TEST ("",          0, "",         "",                   0),
    TEST ("",          0, "abc",      "abc",                0),

    TEST ("\0",        0, "",         "\0",                 0),
    TEST ("",          0, "\0",       "\0",                 0),

    TEST ("abc",       0, "",         "abc",                0),
    TEST ("abc",       1, "",         "abc",                0),
    TEST ("abc",       3, "",         "abc",                0),

    TEST ("\0",        0, "a",        "a\0",                0),
    TEST ("\0",        1, "a",        "\0a",                0),
    TEST ("\0",        0, "\0",       "\0\0",               0),
    TEST ("\0",        1, "\0\0",     "\0\0\0",             0),

    TEST ("cde",       0, "ab",       "abcde",              0),
    TEST ("cde",       1, "ab",       "cabde",              0),
    TEST ("cde",       2, "ab",       "cdabe",              0),
    TEST ("cde",       3, "ab",       "cdeab",              0),

    TEST ("\0ab\0\0c", 0, "\0e\0",    "\0e\0\0ab\0\0c",     0),
    TEST ("a\0b\0\0c", 2, "e\0e",     "a\0e\0eb\0\0c",      0),
    TEST ("a\0bc\0\0", 6, "e\0\0e\0", "a\0bc\0\0e\0\0e\0",  0),

    TEST ("abc",       0, 0,          "abcabc",             0),
    TEST ("abc",       2, 0,          "ababcc",             0),
    TEST ("a\0bc\0\0", 0, 0,          "a\0bc\0\0a\0bc\0\0", 0),
    TEST ("a\0bc\0\0", 6, 0,          "a\0bc\0\0a\0bc\0\0", 0),

    TEST ("",          0, LSTR,       LSTR,                 0),
    TEST (LSTR,        0, "",         LSTR,                 0),

    TEST ("\0",        2, "",         "",                   1),
    TEST ("a",         2, "",         "",                   1),
    TEST (LSTR,LLEN + 10, "",         "",                   1),

    TEST ("",          0, LSTR,       LSTR,                -1),

    TEST ("last",      4, "test",     "lasttest",           0)
};

/**************************************************************************/

// exrcises
// insert (size_type off, basic_string& str, size_type off2, size_type n)
// insert (iterator p, InputIterator first, InputIterator last)
static const TestCase
range_test_cases [] = {

// range_test_cases serves a double duty
#define size_str_size_size_test_cases range_test_cases

#undef TEST
#define TEST(str, off, arg, off2, size2, res, bthrow) {                 \
        __LINE__, off, -1, off2, size2, -1, str, sizeof str - 1,        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow                \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- sequence to be inserted
    //    |            |  |            +------------ insert() off2 argument
    //    |            |  |            |  +--------- insert() num argument
    //    |            |  |            |  |  +------ expected result sequence
    //    |            |  |            |  |  |  +--- exception info
    //    |            |  |            |  |  |  |       0 - no exception
    //    |            |  |            |  |  |  |       1 - out_of_range
    //    |            |  |            |  |  |  |       2 - length_error
    //    |            |  |            |  |  |  |      -1 - exc. safety
    //    |            |  |            |  |  |  |
    //    |            |  |            |  |  |  +----------------+
    //    V            V  V            V  V  V                   V
    TEST ("ab",        0, "c",         0, 1, "cab",              0),

    TEST ("",          0, "",          0, 0,  "",                0),
    TEST ("",          0, "abc",       1, 1,  "b",               0),
    TEST ("",          0, "\0",        0, 1,  "\0",              0),

    TEST ("\0",        0, "",          0, 0,  "\0",              0),
    TEST ("\0",        1, "",          0, 0,  "\0",              0),

    TEST ("abc",       0, "",          0, 0,  "abc",             0),
    TEST ("abc",       1, "",          0, 0,  "abc",             0),
    TEST ("abc",       3, "",          0, 0,  "abc",             0),

    TEST ("\0",        0, "a",         0, 1,  "a\0",             0),
    TEST ("\0",        1, "a",         0, 1,  "\0a",             0),
    TEST ("\0",        0, "\0\0",      1, 1,  "\0\0",            0),
    TEST ("\0",        1, "\0\0",      0, 2,  "\0\0\0",          0),
    TEST ("\0",        1, "\0\0",      1, 5,  "\0\0",            0),

    TEST ("cde",       0, "ab",        0, 2,  "abcde",           0),
    TEST ("cde",       1, "ab",        0, 1,  "cade",            0),
    TEST ("cde",       2, "ab",        1, 5,  "cdbe",            0),
    TEST ("cde",       3, "ab",        1, 1,  "cdeb",            0),

    TEST ("ab",        0, "c\0e",      0, 3,  "c\0eab",          0),
    TEST ("ab",        1, "c\0e",      1, 2,  "a\0eb",           0),
    TEST ("ab",        2, "c\0e",      0, 2,  "abc\0",           0),

    TEST ("\0e\0",     1, "\0ab\0\0c", 0, 9,  "\0\0ab\0\0ce\0",  0),
    TEST ("\0e\0",     1, "\0ab\0\0c", 0, 3,  "\0\0abe\0",       0),
    TEST ("a\0b\0\0c", 3, "\0e\0",     0, 3,  "a\0b\0e\0\0\0c",  0),
    TEST ("a\0b\0\0c", 2, "\0\0e\0",   0, 2,  "a\0\0\0b\0\0c",   0),
    TEST ("\0ab\0\0c", 0, "\0e\0",     2, 1,  "\0\0ab\0\0c",     0),
    TEST ("a\0bc\0\0", 6, "\0e",       0, 2,  "a\0bc\0\0\0e",    0),

    TEST ("abc",       0, 0,           1, 1,  "babc",            0),
    TEST ("abc",       2, 0,           0, 2,  "ababc",           0),
    TEST ("a\0bc\0\0", 0, 0,           4, 2,  "\0\0a\0bc\0\0",   0),
    TEST ("a\0bc\0\0", 6, 0,           1, 3,  "a\0bc\0\0\0bc",   0),
    TEST ("abcdef",    0, 0,           1, 2,  "bcabcdef",        0),

    TEST ("",          0, LSTR,        0, LLEN, LSTR,            0),
    TEST (LSTR,        0, "",          0, 0,    LSTR,            0),

    TEST ("\0",        2, "",          0, 0,  "",                1),
    TEST ("",          0, "\0",        2, 0,  "",                2),

    TEST ("a",         2, "",          0, 0,  "",                1),
    TEST ("",          0, "a",         2, 0,  "",                2),

    TEST (LSTR,LLEN + 10, "",          0, 0,  "",                1),
    TEST ("",          0, LSTR,LLEN + 10, 0,  "",                2),

    TEST ("",          0, LSTR, 0, LLEN - 1, LSTR,              -1),

    TEST ("last",      4, "test",      0, 4,  "lasttest",        0)
};

/**************************************************************************/

// used to exercise
// insert (size_type pos, const charT* s, size_type n)
static const TestCase
size_ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, off, arg, size2, res, bthrow) {               \
        __LINE__, off, -1, -1, size2, -1, str, sizeof str - 1,  \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- sequence to be inserted
    //    |            |  |            +------------ insert() num argument
    //    |            |  |            |  +--------- expected result sequence
    //    |            |  |            |  |     +--- exception info
    //    |            |  |            |  |     |      0 - no exception
    //    |            |  |            |  |     |      1 - out_of_range
    //    |            |  |            |  |     |      2 - length_error
    //    |            |  |            |  |     |     -1 - exc. safety
    //    |            |  |            |  |     |
    //    |            |  |            |  |     +------------+
    //    V            V  V            V  V                  V
    TEST ("ab",        0, "c",         1, "cab",             0),

    TEST ("",          0, "",          0,  "",               0),
    TEST ("",          0, "abc",       1,  "a",              0),

    TEST ("\0",        0, "",          0,  "\0",             0),
    TEST ("\0",        1, "",          0,  "\0",             0),
    TEST ("",          0, "\0",        1,  "\0",             0),

    TEST ("\0",        0, "a",         0,  "\0",             0),
    TEST ("\0",        1, "a",         1,  "\0a",            0),
    TEST ("\0",        0, "\0\0",      1,  "\0\0",           0),
    TEST ("\0",        1, "\0\0",      2,  "\0\0\0",         0),

    TEST ("cde",       0, "ab",        2,  "abcde",          0),
    TEST ("cde",       1, "ab",        1,  "cade",           0),
    TEST ("cde",       3, "ab",        1,  "cdea",           0),
    TEST ("cde",       2, "ab",        2,  "cdabe",          0),

    TEST ("\0e\0",     3, "a\0b\0\0c", 0,  "\0e\0",          0),
    TEST ("\0e\0",     3, "\0ab\0\0c", 3,  "\0e\0\0ab",      0),

    TEST ("a\0b\0\0c", 3, "\0e\0",     3,  "a\0b\0e\0\0\0c", 0),
    TEST ("a\0b\0\0c", 2, "\0\0e\0",   2,  "a\0\0\0b\0\0c",  0),
    TEST ("\0ab\0\0c", 0, "\0e\0",     1,  "\0\0ab\0\0c",    0),
    TEST ("a\0bc\0\0", 6, "\0e",       2,  "a\0bc\0\0\0e",   0),

    TEST ("abc",       0, 0,           1,  "aabc",           0),
    TEST ("abc",       2, 0,           2,  "ababc",          0),
    TEST ("\0abc\0\0", 0, 0,           1,  "\0\0abc\0\0",    0),
    TEST ("a\0bc\0\0", 6, 0,           3,  "a\0bc\0\0a\0b",  0),

    TEST ("",          0, LSTR, LLEN - 1,  LSTR,             0),
    TEST (LSTR,        0, "",          0,  LSTR,             0),

    TEST ("\0",        2, "",          0,  "",               1),
    TEST ("a",         2, "",          0,  "",               1),
    TEST (LSTR,LLEN + 10, "",          0,  "",               1),

    TEST ("",          0, LSTR, LLEN - 1,  LSTR,            -1),

    TEST ("last",      4, "test",      4,  "lasttest",       0)
};

/**************************************************************************/

// exrecises
// insert (size_type pos, size_type n, charT c)
// insert (iterator p, size_type n, charT c)
static const TestCase
size_val_test_cases [] = {

// size_val_test_cases serves a double duty
#define size_size_val_test_cases size_val_test_cases

#undef TEST
#define TEST(str, off, size2, val, res, bthrow) {               \
        __LINE__, off, -1, -1, size2, val, str, sizeof str - 1, \
        0, 0, res, sizeof res - 1, bthrow                       \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- insert() size2 argument
    //    |            |  |   +--------------------- character to be inserted
    //    |            |  |   |   +----------------- expected result sequence
    //    |            |  |   |   |       +--------- exception info
    //    |            |  |   |   |       |             0 - no exception
    //    |            |  |   |   |       |             1 - out_of_range
    //    |            |  |   |   |       |             2 - length_error
    //    |            |  |   |   |       |            -1 - exc. safety
    //    |            |  |   |   |       |
    //    |            |  |   |   |       +-----------+
    //    V            V  V   V   V                   V
    TEST ("ab",        0, 1, 'c', "cab",              0),

    TEST ("",          0, 0, ' ',  "",                0),
    TEST ("",          0, 1, 'b',  "b",               0),

    TEST ("\0",        0, 0, ' ',  "\0",              0),
    TEST ("\0",        1, 0, ' ',  "\0",              0),
    TEST ("",          0, 2, '\0', "\0\0",            0),

    TEST ("\0",        0, 1, 'a',  "a\0",             0),
    TEST ("\0",        1, 1, 'a',  "\0a",             0),
    TEST ("\0",        0, 1, '\0', "\0\0",            0),
    TEST ("\0",        1, 2, '\0', "\0\0\0",          0),
    TEST ("\0",        1, 0, '\0', "\0",              0),

    TEST ("cde",       0, 2, 'a',  "aacde",           0),
    TEST ("cde",       1, 1, 'a',  "cade",            0),
    TEST ("cde",       3, 3, 'a',  "cdeaaa",          0),
    TEST ("cde",       2, 3, 'a',  "cdaaae",          0),

    TEST ("ab",        0, 2, '\0', "\0\0ab",          0),
    TEST ("ab",        1, 1, '\0', "a\0b",            0),
    TEST ("ab",        2, 2, '\0', "ab\0\0",          0),

    TEST ("a\0b\0\0c", 3, 2, '\0', "a\0b\0\0\0\0c",   0),
    TEST ("a\0b\0\0c", 2, 1, '\0', "a\0\0b\0\0c",     0),
    TEST ("\0ab\0\0c", 0, 3, '\0', "\0\0\0\0ab\0\0c", 0),
    TEST ("a\0bc\0\0", 6, 2, 'a',  "a\0bc\0\0aa",     0),

    TEST ("",          0, LLEN - 1, 'x', LSTR,        0),
    TEST (LSTR,        0, 0,        'x', LSTR,        0),

    TEST ("",          0, LLEN - 1, 'x', LSTR,       -1),

    TEST ("last",      4, 4, 't',  "lasttttt",        0)
};

/**************************************************************************/

// used to exercise
// insert (iterator p, charT c)
static const TestCase
val_test_cases [] = {

#undef TEST
#define TEST(str, off, val, res, bthrow)                                \
    { __LINE__, off, -1, -1, -1, val, str, sizeof str - 1, 0,           \
      0, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |    +----------------------- character to be inserted
    //    |            |    |   +------------------- expected result sequence
    //    |            |    |   |               +--- exception info
    //    |            |    |   |               |       0 - no exception
    //    |            |    |   |               |       1 - out_of_range
    //    |            |    |   |               |       2 - length_error
    //    |            |    |   |               |      -1 - exc. safety
    //    |            |    |   |               |
    //    |            |    |   |               |
    //    V            V    V   V               V
    TEST ("ab",        0,  'c', "cab",          0),

    TEST ("",          0,  'b', "b",            0),
    TEST ("",          0, '\0', "\0",           0),

    TEST ("\0",        0,  'a', "a\0",          0),
    TEST ("\0",        1,  'a', "\0a",          0),
    TEST ("\0",        0, '\0', "\0\0",         0),
    TEST ("\0",        1, '\0', "\0\0",         0),

    TEST ("cde",       0,  'a', "acde",         0),
    TEST ("cde",       1,  'a', "cade",         0),
    TEST ("cde",       2,  'a', "cdae",         0),
    TEST ("cde",       3,  'a', "cdea",         0),

    TEST ("a\0b\0\0c", 3, '\0', "a\0b\0\0\0c",  0),
    TEST ("\0ab\0\0c", 0, '\0', "\0\0ab\0\0c",  0),
    TEST ("a\0bc\0\0", 6,  'a', "a\0bc\0\0a",   0),

#undef TEST
#define TEST(str, len, off, val, res, bthrow)                           \
    { __LINE__, off, -1, -1, -1, val, str, len, 0,                      \
      0, res, sizeof res - 1, bthrow }

    TEST (LSTR + 1, LLEN - 2, 0, 'x', LSTR,    -1),

    TEST ("last",  4,  4,  't', "lastt",        0)
};

/**************************************************************************/

template <class charT, class Traits, class Iterator>
void test_insert_range (charT* wstr,
                        charT* warg,
                        Traits*,
                        const Iterator &it,
                        const TestCase &tcase)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;

    const char* const itname =
        tcase.arg ? type_name (it, (charT*)0) : "basic_string::iterator";

    String s_str (wstr, tcase.str_len);
    String s_arg (warg, tcase.arg_len);

    std::size_t off_last = tcase.off2 + tcase.size2;

    StringIter iter (s_str.begin () + tcase.off);

    if (tcase.arg) {
        off_last = off_last > s_arg.size () ? s_arg.size () : off_last;

        const Iterator first = make_iter (warg + tcase.off2,
            warg + tcase.off2, warg + off_last, Iterator (0, 0, 0));
        const Iterator last  = make_iter (warg + off_last,
            warg + tcase.off2, warg + off_last, Iterator (0, 0, 0));

        s_str.insert (iter, first, last);
    }
    else {
        const StringIter first (s_str.begin () + tcase.off2);
        const StringIter last  (off_last > s_str.size () ?
                                s_str.end ()
                              : s_str.begin () + off_last);

        s_str.insert (iter, first, last);
    }

    const std::size_t match =
        rw_match (tcase.res, s_str.c_str(), tcase.nres);

    rw_assert (match == tcase.nres, 0, tcase.line,
               "line %d. %{$FUNCALL} expected %{#*s}, got %{/*.*Gs}, "
               "difference at pos %zu for %s",
               __LINE__, int (tcase.nres), tcase.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits>
void test_insert_range (charT* wstr,
                        charT* warg,
                        Traits*,
                        const TestCase &tcase)
{
    if (tcase.bthrow)  // this method doesn't throw
        return;

    test_insert_range (wstr, warg, (Traits*)0,
                       InputIter<charT>(0, 0, 0), tcase);

    // there is no need to call test_insert_range
    // for other iterators in this case
    if (0 == tcase.arg)
        return;

    test_insert_range (wstr, warg, (Traits*)0,
                       ConstFwdIter<charT>(0, 0, 0), tcase);

    test_insert_range (wstr, warg, (Traits*)0,
                       ConstBidirIter<charT>(0, 0, 0), tcase);

    test_insert_range (wstr, warg, (Traits*)0,
                       ConstRandomAccessIter<charT>(0, 0, 0), tcase);
}

/**************************************************************************/

template <class charT, class Traits>
void test_insert (charT, Traits*,
                  OverloadId      which,
                  const TestCase &tcase)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::iterator                StringIter;
    typedef typename TestString::const_iterator          ConstStringIter;

    typedef typename UserTraits<charT>::MemFun UTMemFun;

    static charT wstr [LLEN];
    static charT warg [LLEN];

    rw_widen (wstr, tcase.str, tcase.str_len);
    rw_widen (warg, tcase.arg, tcase.arg_len);

    // special processing for insert_range to exercise all iterators
    if (Insert (range) == which) {
        test_insert_range (wstr, warg, (Traits*)0, tcase);
        return;
    }

    /* const */ TestString s_str (wstr, tcase.str_len);
    const       TestString s_arg (warg, tcase.arg_len);

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (s_str));

    std::size_t res_off = 0;
    std::size_t exp_off = Insert (val) == which ? tcase.off : 0;

    const charT* const arg_ptr = tcase.arg ? warg : s_str.c_str ();
    const TestString&  arg_str = tcase.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    std::size_t total_length_calls = 0;
    std::size_t n_length_calls = 0;
    std::size_t* const rg_calls = rw_get_call_counters ((Traits*)0, (charT*)0);

    if (rg_calls)
        total_length_calls = rg_calls [UTMemFun::length];

    rwt_free_store* const pst = rwt_get_free_store (0);

    // iterate for`throw_after' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_after;
    for (throw_after = 0; ; ++throw_after) {

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        const bool use_iters = (Insert (val) <= which);

        if (1 == tcase.bthrow && !use_iters)
            expected = exceptions [1];      // out_of_range
        else if (2 == tcase.bthrow && Insert (size_str_size_size) == which)
            expected = exceptions [1];      // out_of_range
        else if (3 == tcase.bthrow && !use_iters)
            expected = exceptions [2];      // length_error
        else if (-1 == tcase.bthrow) {
            expected = exceptions [3];      // bad_alloc
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow)
            return;

#endif   // _RWSTD_NO_EXCEPTIONS

        try {
            switch (which) {
            case Insert (size_ptr): {
                const TestString& s_res = s_str.insert (tcase.off, arg_ptr);
                res_off = &s_res - &s_str;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            }

            case Insert (size_str): {
                const TestString& s_res = s_str.insert (tcase.off, arg_str);
                res_off = &s_res - &s_str;
                break;
            }

            case Insert (size_ptr_size): {
                const TestString& s_res = 
                    s_str.insert (tcase.off, arg_ptr, tcase.size2);
                res_off = &s_res - &s_str;
                break;
            }

            case Insert (size_str_size_size): {
                const TestString& s_res = 
                    s_str.insert (tcase.off, arg_str, tcase.off2, tcase.size2);
                res_off = &s_res - &s_str;
                break;
            }

            case Insert (size_size_val): {
                const TestString& s_res = 
                    s_str.insert (tcase.off, tcase.size2, arg_val);
                res_off = &s_res - &s_str;
                break;
            }

            case Insert (size_val): {
                StringIter it (s_str.begin () + tcase.off);
                s_str.insert (it, tcase.size2, arg_val);
                break;
            }

            case Insert (val): {
                StringIter it (s_str.begin () + tcase.off);
                StringIter res_it = s_str.insert (it, arg_val);
                res_off = res_it - s_str.begin ();
                break;
            }

            default:
                RW_ASSERT (!"logic error: unknown insert overload");
                return;
            }

            // verify the returned value
            rw_assert (exp_off == res_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}== %zu, got %zu%{;}"
                       "%{?}returned invalid reference, offset is %zu%{;}",
                       __LINE__, Insert (val) == which, exp_off, res_off,
                       Insert (val) != which, res_off);

            // verfiy that strings length are equal
            rw_assert (tcase.nres == s_str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{#*s} with length "
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
                           "line %d. %{$FUNCALL} expected %{#*s}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, int (tcase.nres), tcase.res,
                           int (sizeof (charT)), int (s_str.size ()),
                           s_str.c_str (), match);
            }

            // verify that Traits::length was used
            if (Insert (size_ptr) == which && rg_calls) {
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
            str_state.assert_equal (rw_get_string_state (s_str),
                                    __LINE__, tcase.line, caught);

            if (-1 == tcase.bthrow) {
                // allow this call to operator new to succeed and try
                // to make the next one to fail during the next call
                // to the same function again
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

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify that if exceptions are enabled and when capacity changes
    // at least one exception is thrown
    rw_assert (   *pst->throw_at_calls_ [0] == std::size_t (-1)
               || throw_after,
               0, tcase.line,
               "line %d: %{$FUNCALL}: failed to throw an expected exception",
               __LINE__);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    *pst->throw_at_calls_ [0] = std::size_t (-1);
}

/**************************************************************************/

DEFINE_TEST_DISPATCH (test_insert);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::insert_ ## tag, tag ## _test_cases,      \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (size_ptr),
        TEST (size_str),
        TEST (size_ptr_size),
        TEST (size_str_size_size),
        TEST (size_size_val),
        TEST (val),
        TEST (size_val),
        TEST (range)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return StringMembers::run_test (argc, argv, __FILE__,
                                    "lib.string.insert",
                                    test_insert, tests, test_count);
}
