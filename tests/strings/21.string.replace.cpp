/***************************************************************************
 *
 * 21.string.replace.cpp - string test exercising lib.string.replace
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

#include <string>       // for string
#include <stdexcept>    // for out_of_range, length_error

#include <cstddef>      // for size_t

#include <cmdopt.h>     // for rw_enabled()
#include <driver.h>     // for rw_test()

#include <alg_test.h>   // for InputIter
#include <rw_printf.h>  // for rw_asnprintf()
#include <rw_char.h>    // for rw_widen()

#include <21.strings.h>

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
   // disabled for compilers such as IBM VAC++ or MSVC
   // that can't reliably replace the operators
#  include <rw_new.h>   // for bad_alloc, replacement operator new
#else
#  include <new>        // for bad_alloc
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

#define Replace(which)    StringMembers::replace_ ## which

typedef StringMembers::OverloadId OverloadId;
typedef StringMembers::TestCase   TestCase;
typedef StringMembers::Test       Test;
typedef StringMembers::Function   MemFun;

/**************************************************************************/

// for convenience and brevity
#define LSTR   StringMembers::long_string
#define LLEN   StringMembers::long_string_len
// one half of the long_string length
#define LPAR   (LLEN / 2)


static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// replace (size_type, size_type, const value_type*)
// replace (iterator, iterator, const value_type*)
static const TestCase
iter_iter_ptr_test_cases [] = {

// iter_iter_ptr_test_cases serves a double duty
#define size_size_ptr_test_cases iter_iter_ptr_test_cases

#undef TEST
#define TEST(str, off, size, arg, res, bthrow) {                \
        __LINE__, off, size, -1, -1, -1,                        \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |            +----------------------------- replace() pos1 argument
    //    |            |  +-------------------------- replace() n1 argument
    //    |            |  |  +----------------------- sequence to be inserted
    //    |            |  |  |           +----------- expected result sequence
    //    |            |  |  |           |        +-- exception info
    //    |            |  |  |           |        |       0   - no exception
    //    |            |  |  |           |        |       1,2 - out_of_range
    //    |            |  |  |           |        |       3   - length_error
    //    |            |  |  |           |        |      -1   - exc. safety
    //    |            |  |  |           |        |
    //    |            |  |  |           |        +-------+
    //    V            V  V  V           V                V
    TEST ("ab",        0, 0, "c",        "cab",           0),

    TEST ("",          0, 0, "",         "",              0),
    TEST ("",          0, 0, "abc",      "abc",           0),

    TEST ("ab",        0, 2, "",         "",              0),
    TEST ("ab",        0, 1, "",         "b",             0),

    TEST ("\0",        0, 1, "",         "",              0),
    TEST ("\0",        0, 1, "a",        "a",             0),
    TEST ("\0",        0, 1, "\0\0",     "",              0),

    TEST ("ah",        0, 1, "bcdefg",   "bcdefgh",       0),
    TEST ("ah",        1, 1, "bcdefg",   "abcdefg",       0),
    TEST ("ah",        0, 2, "bcdefg",   "bcdefg",        0),

    TEST ("abc",       0, 2, "cc",       "ccc",           0),
    TEST ("abc",       1, 2, "cc",       "acc",           0),

    TEST ("abc",       0, 3, "defgh",    "defgh",         0),
    TEST ("abc",       2, 1, "defgh",    "abdefgh",       0),
    TEST ("abc",       2, 1, "de\0gh",   "abde",          0),
    TEST ("abc",       2, 1, "",         "ab",            0),
    TEST ("abc",       1, 1, "defgh",    "adefghc",       0),

    TEST ("abc",       0, 0, "ee",       "eeabc",         0),
    TEST ("abc",       0, 0, "\0\0e\0",  "abc",           0),
    TEST ("abc",       2, 0, "ee",       "abeec",         0),
    TEST ("abc",       1, 0, "ee",       "aeebc",         0),
    TEST ("abc",       1, 0, "e\0\0",    "aebc",          0),

    TEST ("a\0b\0\0c", 0, 3, "",         "\0\0c",         0),
    TEST ("a\0b\0\0c", 0, 3, "\0e",      "\0\0c",         0),

    TEST ("\0ab\0\0c", 0, 0, "e\0",      "e\0ab\0\0c",    0),
    TEST ("a\0b\0c\0", 6, 0, "e\0",      "a\0b\0c\0e",    0),
    TEST ("\0ab\0\0c", 5, 0, "e\0",      "\0ab\0\0ec",    0),

    TEST (LSTR, 0, LLEN - 1, "ab",       "ab",            0),
    TEST (LSTR, 1, LLEN - 2, "ab",       "xab",           0),
    TEST (LSTR, 0, LLEN - 2, "ab",       "abx",           0),
    TEST (LSTR, 1, LLEN - 3, "",         "xx",            0),

    TEST ("",   0,        0, LSTR,       LSTR,            0),
    TEST ("a",  0,        1, LSTR,       LSTR,            0),
    TEST (LSTR, 0, LLEN - 1, LSTR,       LSTR,            0),
    TEST ("\0ab\0\0c", 0, 6, LSTR,       LSTR,            0),

    TEST ("",          0, 0, 0,          "",              0),
    TEST ("abc",       0, 3, 0,          "abc",           0),
    TEST ("abc",       1, 1, 0,          "aabcc",         0),
    TEST ("a\0b\0c\0", 2, 3, 0,          "a\0a\0",        0),
    TEST ("a\0b\0c\0", 0, 0, 0,          "aa\0b\0c\0",    0),
    TEST ("a\0b\0c\0", 6, 0, 0,          "a\0b\0c\0a",    0),
    TEST ("\0ab\0c\0", 3, 3, 0,          "\0ab",          0),
    TEST (LSTR, 0, LLEN - 1, 0,          LSTR,            0),

    TEST ("\0",         2, 0, "",        "\0",            1),
    TEST ("a",         10, 0, "",        "a",             1),
    TEST (LSTR, LLEN + 10, 0, "",        LSTR,            1),

    TEST ("",   0,        0, LSTR,       LSTR,           -1),

    TEST ("last",      4, 0, "test",     "lasttest",      0)
};

/**************************************************************************/

// used to exercise
// replace (size_type, size_type, const basic_string&)
// replace (iterator, iterator, const basic_string&)
static const TestCase
iter_iter_str_test_cases [] = {

// iter_iter_str_test_cases serves a double duty
#define size_size_str_test_cases iter_iter_str_test_cases

#undef TEST
#define TEST(s, off, size, arg, res, bthrow) {                  \
        __LINE__, off, size, -1, -1, -1,                        \
        s, sizeof s - 1,                                        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |            +----------------------------- replace() pos1 argument
    //    |            |  +-------------------------- replace() n1 argument
    //    |            |  |  +----------------------- sequence to be inserted
    //    |            |  |  |           +----------- expected result sequence
    //    |            |  |  |           |        +-- exception info
    //    |            |  |  |           |        |       0   - no exception
    //    |            |  |  |           |        |       1,2 - out_of_range
    //    |            |  |  |           |        |       3   - length_error
    //    |            |  |  |           |        |      -1   - exc. safety
    //    |            |  |  |           |        |
    //    |            |  |  |           |        +-----------+
    //    V            V  V  V           V                    V
    TEST ("ab",        0, 0, "c",        "cab",               0),

    TEST ("",          0, 0, "",         "",                  0),
    TEST ("",          0, 0, "\0",       "\0",                0),
    TEST ("",          0, 0, "abc",      "abc",               0),

    TEST ("ab",        0, 2, "",         "",                  0),
    TEST ("ab",        0, 1, "",         "b",                 0),
    TEST ("ab",        1, 1, "\0",       "a\0",               0),

    TEST ("\0",        0, 1, "",         "",                  0),
    TEST ("\0",        0, 1, "a",        "a",                 0),
    TEST ("\0",        0, 1, "\0\0",     "\0\0",              0),

    TEST ("ah",        0, 1, "bcdefg",   "bcdefgh",           0),
    TEST ("ah",        1, 1, "bcdefg",   "abcdefg",           0),
    TEST ("ah",        0, 2, "bcdefg",   "bcdefg",            0),

    TEST ("abc",       0, 2, "cc",       "ccc",               0),
    TEST ("abc",       1, 2, "cc",       "acc",               0),

    TEST ("abc",       0, 3, "defgh",    "defgh",             0),
    TEST ("abc",       2, 1, "defgh",    "abdefgh",           0),
    TEST ("abc",       2, 1, "de\0gh",   "abde\0gh",          0),
    TEST ("abc",       2, 1, "",         "ab",                0),
    TEST ("abc",       1, 1, "defgh",    "adefghc",           0),

    TEST ("abc",       0, 0, "ee",       "eeabc",             0),
    TEST ("abc",       0, 0, "\0\0e\0",  "\0\0e\0abc",        0),
    TEST ("abc",       2, 0, "ee",       "abeec",             0),
    TEST ("abc",       1, 0, "ee",       "aeebc",             0),
    TEST ("abc",       1, 0, "e\0\0",    "ae\0\0bc",          0),

    TEST ("a\0b\0\0c", 0, 3, "",         "\0\0c",             0),
    TEST ("a\0b\0\0c", 0, 3, "\0e",      "\0e\0\0c",          0),

    TEST ("a\0b\0\0c", 2, 3, "\0e",      "a\0\0ec",           0),
    TEST ("a\0b\0\0c", 0, 3, "\0\0e\0",  "\0\0e\0\0\0c",      0),
    TEST ("\0ab\0\0c", 1, 2, "\0e\0\0",  "\0\0e\0\0\0\0c",    0),

    TEST ("\0ab\0\0c", 0, 0, "\0e",      "\0e\0ab\0\0c",      0),
    TEST ("a\0b\0c\0", 6, 0, "e\0",      "a\0b\0c\0e\0",      0),
    TEST ("\0ab\0\0c", 5, 0, "\0e",      "\0ab\0\0\0ec",      0),

    TEST (LSTR, 0, LLEN - 1, "ab",       "ab",                0),
    TEST (LSTR, 1, LLEN - 2, "ab",       "xab",               0),
    TEST (LSTR, 0, LLEN - 2, "ab",       "abx",               0),
    TEST (LSTR, 1, LLEN - 3, "",         "xx",                0),
    TEST (LSTR, 1, LLEN - 4, "\0\0",     "x\0\0xx",           0),

    TEST ("",   0,        0, LSTR,       LSTR,                0),
    TEST ("a",  0,        1, LSTR,       LSTR,                0),
    TEST (LSTR, 0, LLEN - 1, LSTR,       LSTR,                0),
    TEST ("\0ab\0\0c", 0, 6, LSTR,       LSTR,                0),

    TEST ("abc",       0, 3, 0,          "abc",               0),
    TEST ("abc",       1, 1, 0,          "aabcc",             0),
    TEST ("a\0b\0c\0", 2, 3, 0,          "a\0a\0b\0c\0\0",    0),
    TEST ("a\0b\0c\0", 0, 0, 0,          "a\0b\0c\0a\0b\0c\0",0),
    TEST ("a\0b\0c\0", 6, 0, 0,          "a\0b\0c\0a\0b\0c\0",0),
    TEST ("\0ab\0c\0", 3, 3, 0,          "\0ab\0ab\0c\0",     0),
    TEST (LSTR, 0, LLEN - 1, 0,          LSTR,                0),

    TEST ("\0",         2, 0, "",        "\0",                1),
    TEST ("a",         10, 0, "",        "a",                 1),
    TEST (LSTR, LLEN + 10, 0, "",        LSTR,                1),

    TEST ("",   0,        0, LSTR,       LSTR,               -1),

    TEST ("last",      4, 0, "test",     "lasttest",          0)
};

/**************************************************************************/

// used to exercise
// replace (size_type, size_type, const value_type*, size_type)
// replace (iterator, iterator, const value_type*, size_type)
static const TestCase
iter_iter_ptr_size_test_cases [] = {

// iter_iter_range_test_cases serves a double duty
#define size_size_ptr_size_test_cases iter_iter_ptr_size_test_cases

#undef TEST
#define TEST(str, off, size, arg, count, res, bthrow) {         \
        __LINE__, off, size, -1, count, -1,                     \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |            +----------------------------- replace() pos1 argument
    //    |            |  +-------------------------- replace() n1 argument
    //    |            |  |  +----------------------- sequence to be inserted
    //    |            |  |  |            +---------- replace() n2 argument
    //    |            |  |  |            |  +------- expected result sequence
    //    |            |  |  |            |  |     +-- exception info
    //    |            |  |  |            |  |     |       0   - no exception
    //    |            |  |  |            |  |     |       1,2 - out_of_range
    //    |            |  |  |            |  |     |       3   - length_error
    //    |            |  |  |            |  |     |      -1   - exc. safety
    //    |            |  |  |            |  |     |
    //    |            |  |  |            |  |     +------------+
    //    V            V  V  V            V  V                  V
    TEST ("ab",        0, 0, "c",         1, "cab",             0),

    TEST ("",          0, 0, "",          0, "",                0),
    TEST ("",          0, 0, "abc",       3, "abc",             0),

    TEST ("ab",        0, 2, "",          0, "",                0),
    TEST ("ab",        0, 1, "",          0, "b",               0),

    TEST ("\0",        0, 1, "",          0, "",                0),
    TEST ("\0",        0, 1, "a",         1, "a",               0),
    TEST ("\0",        0, 1, "\0\0",      2, "\0\0",            0),

    TEST ("ah",        0, 1, "bcdefg",    3, "bcdh",            0),
    TEST ("ah",        1, 1, "bcdefg",    3, "abcd",            0),
    TEST ("ah",        0, 2, "bcdefg",    5, "bcdef",           0),

    TEST ("abc",       0, 2, "cc",        2, "ccc",             0),
    TEST ("abc",       1, 2, "cc",        2, "acc",             0),
    TEST ("abc",       2, 1, "defgh",     1, "abd",             0),
    TEST ("abc",       2, 1, "de\0gh",    3, "abde\0",          0),
    TEST ("abc",       2, 1, "",          0, "ab",              0),

    TEST ("abc",       0, 0, "ee",        2, "eeabc",           0),
    TEST ("abc",       0, 0, "\0\0e\0",   4, "\0\0e\0abc",      0),
    TEST ("abc",       2, 0, "ee",        2, "abeec",           0),
    TEST ("abc",       1, 0, "ee",        1, "aebc",            0),

    TEST ("a\0b\0\0c", 0, 3, "",          0, "\0\0c",           0),
    TEST ("a\0b\0\0c", 0, 3, "e\0",       2, "e\0\0\0c",        0),
    TEST ("a\0b\0\0c", 2, 3, "e\0",       1, "a\0ec",           0),
    TEST ("a\0b\0\0c", 2, 3, "\0e",       2, "a\0\0ec",         0),
    TEST ("\0ab\0\0c", 2, 3, "\0e",       2, "\0a\0ec",         0),
    TEST ("a\0b\0\0c", 0, 6, "e\0",       2, "e\0",             0),

    TEST (LSTR, 1, LLEN - 1, "\0",        1, "x\0",             0),
    TEST (LSTR, 0, LLEN - 1, "ab",        2, "ab",              0),
    TEST (LSTR, 1, LLEN - 2, "ab",        1, "xa",              0),
    TEST (LSTR, 0, LLEN - 2, "ab",        2, "abx",             0),
    TEST (LSTR, 1, LLEN - 3, "",          0, "xx",              0),
    TEST (LSTR, 1, LLEN - 4, "\0\0",      2, "x\0\0xx",         0),

    TEST ("a",  0,        1, LSTR, LLEN - 1, LSTR,              0),
    TEST (LSTR, 0, LLEN - 1, LSTR, LLEN - 1, LSTR,              0),
    TEST (LSTR, 0, LPAR - 1, LSTR, LPAR - 1, LSTR,              0),

    TEST (LSTR, LPAR - 1, LPAR, LSTR, LPAR,  LSTR,              0),

    TEST ("abc",       0, 3, 0,           2, "ab",              0),
    TEST ("abc",       1, 1, 0,           3, "aabcc",           0),
    TEST ("a\0b\0c\0", 2, 3, 0,           6, "a\0a\0b\0c\0\0",  0),
    TEST ("a\0b\0c\0", 0, 0, 0,           4, "a\0b\0a\0b\0c\0", 0),
    TEST ("\0ab\0c\0", 6, 0, 0,           1, "\0ab\0c\0\0",     0),
    TEST ("\0ab\0c\0", 3, 3, 0,           2, "\0ab\0a",         0),
    TEST (LSTR, 0, LLEN - 1, 0,    LLEN - 1, LSTR,              0),
    TEST (LSTR, 0, LLEN - 1, 0,           1, "x",               0),

    TEST ("\0",         2, 0, "",         0, "\0",              1),
    TEST ("a",         10, 0, "",         0, "a",               1),
    TEST (LSTR, LLEN + 10, 0, "",         0, LSTR,              1),

    TEST ("a",  0,        1, LSTR, LLEN - 1, LSTR,             -1),

    TEST ("last",      4, 0, "test",      4, "lasttest",        0)
};

/**************************************************************************/

// used to exercise
// replace (size_type , size_type , basic_string& s, size_type , size_type )
// replace (iterator, Iterator, InputIterator, InputIterator)
static const TestCase
iter_iter_range_test_cases [] = {

// iter_iter_range_test_cases serves a double duty
#define size_size_str_size_size_test_cases iter_iter_range_test_cases

#undef TEST
#define TEST(str, off, size, arg, off2, size2, res, bthrow) {   \
        __LINE__, off, size, off2, size2, -1,                   \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |            +----------------------------- replace() pos argument
    //    |            |  +-------------------------- replace() n1 argument
    //    |            |  |  +----------------------- sequence to be inserted
    //    |            |  |  |            +---------- replace() pos2 argument
    //    |            |  |  |            |  +------- replace() n2 argument
    //    |            |  |  |            |  |  +---- expected result sequence
    //    |            |  |  |            |  |  |  +- exception info
    //    |            |  |  |            |  |  |  |     0   - no exception
    //    |            |  |  |            |  |  |  |     1,2 - out_of_range
    //    |            |  |  |            |  |  |  |     3   - length_error
    //    |            |  |  |            |  |  |  |    -1   - exc. safety
    //    |            |  |  |            |  |  |  |
    //    |            |  |  |            |  |  |  +----------------+
    //    V            V  V  V            V  V  V                   V

    TEST ("ab",        0, 0, "c",         0, 1, "cab",              0),

    TEST ("",          0, 0, "",          0, 0, "",                 0),
    TEST ("",          0, 0, "abc",       0, 3, "abc",              0),

    TEST ("ab",        0, 2, "",          0, 0, "",                 0),
    TEST ("ab",        0, 1, "",          0, 0, "b",                0),

    TEST ("\0",        0, 1, "",          0, 0, "",                 0),
    TEST ("\0",        0, 1, "a",         0, 1, "a",                0),
    TEST ("\0",        0, 1, "\0\0",      1, 1, "\0",               0),
    TEST ("\0",        0, 1, "\0\0",      0, 2, "\0\0",             0),

    TEST ("ah",        0, 1, "bcdefg",    0, 3, "bcdh",             0),
    TEST ("ah",        1, 1, "bcdefg",    0, 3, "abcd",             0),
    TEST ("ah",        0, 1, "bcdefg",    1, 3, "cdeh",             0),
    TEST ("ah",        1, 1, "bcdefg",    1, 3, "acde",             0),
    TEST ("ah",        0, 1, "bcdefg",    0, 6, "bcdefgh",          0),
    TEST ("ah",        1, 1, "bcdefg",    0, 6, "abcdefg",          0),

    TEST ("abc",       0, 2, "cc",        0, 2, "ccc",              0),
    TEST ("abc",       1, 2, "cc",        0, 2, "acc",              0),

    TEST ("abc",       0, 3, "d",         0, 1, "d",                0),
    TEST ("abc",       0, 3, "def",       0, 3, "def",              0),
    TEST ("abc",       0, 3, "defgh",     0, 5, "defgh",            0),
    TEST ("abc",       2, 1, "defgh",     4, 1, "abh",              0),
    TEST ("abc",       2, 1, "de\0gh",    2, 1, "ab\0",             0),
    TEST ("abc",       2, 1, "",          0, 0, "ab",               0),

    TEST ("abc",       1, 1, "defgh",     1, 2, "aefc",             0),
    TEST ("abc",       0, 0, "ee",        0, 2, "eeabc",            0),
    TEST ("abc",       0, 0, "\0\0e\0",   0, 4, "\0\0e\0abc",       0),
    TEST ("abc",       2, 0, "ee",        0, 2, "abeec",            0),
    TEST ("abc",       2, 1, "\0e\0\0",   0, 4, "ab\0e\0\0",        0),
    TEST ("abc",       1, 0, "ee",        0, 2, "aeebc",            0),
    TEST ("abc",       1, 0, "\0e\0\0",   0, 4, "a\0e\0\0bc",       0),

    TEST ("a\0b\0\0c", 0, 3, "",          0, 0, "\0\0c",            0),
    TEST ("\0ab\0\0c", 0, 3, "",          0, 0, "\0\0c",            0),
    TEST ("a\0b\0\0c", 0, 3, "\0e",       0, 2, "\0e\0\0c",         0),

    TEST ("a\0b\0\0c", 2, 3, "\0e",       0, 2, "a\0\0ec",          0),
    TEST ("a\0b\0\0c", 2, 3, "\0e",       1, 1, "a\0ec",            0),
    TEST ("\0ab\0\0c", 2, 3, "\0e",       0, 2, "\0a\0ec",          0),
    TEST ("\0ab\0\0c", 2, 3, "\0e\0\0",   1, 3, "\0ae\0\0c",        0),

    TEST ("a\0b\0\0c", 0, 6, "\0e",       0, 2, "\0e",              0),
    TEST ("a\0b\0\0c", 0, 6, "\0e",       0, 1, "\0",               0),

    TEST ("\0ab\0\0c", 0, 0, "\0e",       0, 2, "\0e\0ab\0\0c",     0),
    TEST ("a\0b\0c\0", 6, 0, "e\0",       0, 2, "a\0b\0c\0e\0",     0),
    TEST ("\0ab\0\0c", 5, 0, "\0e",       0, 2, "\0ab\0\0\0ec",     0),

    TEST (LSTR, 0, LLEN - 1, "ab",        0, 2, "ab",               0),
    TEST (LSTR, 1, LLEN - 2, "ab",        0, 2, "xab",              0),
    TEST (LSTR, 0, LLEN - 2, "ab",        0, 2, "abx",              0),
    TEST (LSTR, 1, LLEN - 3, "",          0, 0, "xx",               0),
    TEST (LSTR, 1, LLEN - 4, "\0\0",      0, 2, "x\0\0xx",          0),

    TEST ("a",  0, 1,        LSTR, 0, LLEN - 1, LSTR,               0),
    TEST (LSTR, 0, LLEN - 1, LSTR, 0, LLEN - 1, LSTR,               0),
    TEST (LSTR, 0, LPAR - 1, LSTR, 0, LPAR - 1, LSTR,               0),

    TEST (LSTR, LPAR - 1, LPAR, LSTR, 0, LPAR,  LSTR,               0),

    TEST ("abc",       0, 0, 0,           1, 1,  "babc",            0),
    TEST ("abc",       2, 0, 0,           0, 2,  "ababc",           0),
    TEST ("a\0bc\0\0", 0, 0, 0,           4, 2,  "\0\0a\0bc\0\0",   0),
    TEST ("a\0bc\0\0", 6, 0, 0,           1, 3,  "a\0bc\0\0\0bc",   0),
    TEST ("abcdef",    0, 0, 0,           1, 2,  "bcabcdef",        0),

    TEST ("\0",         2, 0, "",           0, 0, "\0",             1),
    TEST ("\0",         0, 0, "\0",         2, 0, "",               2),

    TEST ("a",         10, 0, "",           0, 0, "a",              1),
    TEST ("a",          0, 0, "a",         10, 0, "",               2),

    TEST (LSTR, LLEN + 10, 0, "",           0, 0, LSTR,             1),
    TEST (LSTR, 0,         0, LSTR, LLEN + 10, 0, "",               2),

    TEST ("a",          0, 1, LSTR, 0, LLEN - 1, LSTR,             -1),

    TEST ("last",      4, 0, "test",      0, 4, "lasttest",         0)
};

/**************************************************************************/

// used to exercise
// replace (size_type, size_type, value_type, size_type)
// replace (iterator, iterator, size_type, value_type)
static const TestCase
iter_iter_size_val_test_cases [] = {

// iter_iter_size_val_test_cases serves a double duty
#define size_size_size_val_test_cases iter_iter_size_val_test_cases

#undef TEST
#define TEST(str, off, size, count, val, res, bthrow) { \
        __LINE__, off, size, -1, count, val,            \
        str, sizeof str - 1,                            \
        0, 0, res, sizeof res - 1, bthrow               \
    }

    //    +------------------------------------------ controlled sequence
    //    |            +----------------------------- replace() pos1 argument
    //    |            |  +-------------------------- replace() n1 argument
    //    |            |  |  +----------------------- replace() count argument
    //    |            |  |  |   +------------------- character to be inserted
    //    |            |  |  |   |    +-------------- expected result sequence
    //    |            |  |  |   |    |       +------- exception info
    //    |            |  |  |   |    |       |          0   - no exception
    //    |            |  |  |   |    |       |          1,2 - out_of_range
    //    |            |  |  |   |    |       |          3   - length_error
    //    |            |  |  |   |    |       |         -1   - exc. safety
    //    |            |  |  |   |    |       |
    //    |            |  |  |   |    |       +--------+
    //    V            V  V  V   V    V                V
    TEST ("ab",        0, 0, 1, 'c',  "cab",           0),

    TEST ("",          0, 0, 0, 'c',  "",              0),
    TEST ("",          0, 0, 3, 'c',  "ccc",           0),

    TEST ("ab",        0, 2, 0, 'c',  "",              0),
    TEST ("ab",        0, 1, 0, 'c',  "b",             0),

    TEST ("\0",        0, 1, 0, ' ',  "",              0),
    TEST ("\0",        0, 1, 1, 'a',  "a",             0),
    TEST ("\0",        0, 1, 1, '\0', "\0",            0),
    TEST ("\0",        0, 1, 2, '\0', "\0\0",          0),

    TEST ("ah",        0, 1, 1, 'c',  "ch",            0),
    TEST ("ah",        1, 1, 1, 'c',  "ac",            0),
    TEST ("ah",        0, 1, 4, 'c',  "cccch",         0),
    TEST ("ah",        1, 1, 4, 'c',  "acccc",         0),

    TEST ("abc",       0, 2, 2, 'c',  "ccc",           0),
    TEST ("abc",       1, 2, 2, 'c',  "acc",           0),

    TEST ("abc",       0, 3, 1, 'c',  "c",             0),
    TEST ("abc",       0, 3, 5, 'c',  "ccccc",         0),
    TEST ("abc",       2, 1, 1, 'c',  "abc",           0),
    TEST ("abc",       2, 1, 0, 'c',  "ab",            0),

    TEST ("abc",       1, 1, 5, 'c',  "acccccc",       0),
    TEST ("abc",       0, 0, 2, 'c',  "ccabc",         0),
    TEST ("abc",       0, 0, 3, '\0', "\0\0\0abc",     0),
    TEST ("abc",       2, 0, 2, 'e',  "abeec",         0),
    TEST ("abc",       2, 0, 3, '\0', "ab\0\0\0c",     0),
    TEST ("abc",       1, 0, 1, '\0', "a\0bc",         0),

    TEST ("a\0b\0\0c", 0, 3, 1, '\0', "\0\0\0c",       0),
    TEST ("a\0b\0\0c", 2, 3, 2, '\0', "a\0\0\0c",      0),
    TEST ("a\0b\0\0c", 2, 2, 1, '\0', "a\0\0\0c",      0),
    TEST ("\0ab\0\0c", 2, 3, 0, '\0', "\0ac",          0),
    TEST ("\0ab\0\0c", 2, 1, 2, '\0', "\0a\0\0\0\0c",  0),

    TEST ("a\0b\0\0c", 0, 6, 2, '\0', "\0\0",          0),

    TEST ("\0ab\0\0c", 0, 0, 2, '\0', "\0\0\0ab\0\0c", 0),
    TEST ("a\0b\0c\0", 6, 0, 2, '\0', "a\0b\0c\0\0\0", 0),
    TEST ("\0ab\0\0c", 5, 0, 1, '\0', "\0ab\0\0\0c",   0),

    TEST (LSTR, 0, LLEN - 1, 2, 'a',  "aa",            0),
    TEST (LSTR, 1, LLEN - 2, 2, 'a',  "xaa",           0),
    TEST (LSTR, 0, LLEN - 2, 2, 'a',  "aax",           0),
    TEST (LSTR, 1, LLEN - 3, 0, 'a',  "xx",            0),
    TEST (LSTR, 1, LLEN - 4, 1, '\0', "x\0xx",         0),

    TEST ("a",  0,        1,  LLEN - 1, 'x', LSTR,     0),
    TEST (LSTR, 0, LLEN - 1,  LLEN - 1, 'x', LSTR,     0),
    TEST (LSTR, 0, LPAR - 1,  LPAR - 1, 'x', LSTR,     0),

    TEST (LSTR, LPAR - 1, LPAR, LPAR,   'x', LSTR,     0),

    TEST ("\0",         2, 0, 0, ' ',  "\0",           1),
    TEST ("a",         10, 0, 0, ' ',  "a",            1),
    TEST (LSTR, LLEN + 10, 0, 0, ' ',  LSTR,           1),

    TEST ("a",  0,        1,  LLEN - 1, 'x', LSTR,    -1),

    TEST ("last",      4, 0, 4, 't', "lasttttt",       0)
};

/**************************************************************************/

template <class charT, class Traits, class Iterator>
void test_replace_range (const charT*    wstr,
                         const charT*    warg,
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
    const       String s_arg (warg, tcase.arg_len);

    std::size_t off_last   = tcase.off + tcase.size;
    std::size_t off_first2 = tcase.off2;
    std::size_t off_last2  = tcase.off2 + tcase.size2;

    const StringIter it_first (std::size_t (tcase.off) >= s_str.size () ?
                               s_str.end () : s_str.begin () + tcase.off);
    const StringIter it_last  (std::size_t (off_last) >= s_str.size () ?
                               s_str.end () : s_str.begin () + off_last);

    if (tcase.arg) {
        off_first2 = off_first2 > s_arg.size () ? s_arg.size () : off_first2;
        off_last2  = off_last2  > s_arg.size () ? s_arg.size () : off_last2;

        const charT* const warg_beg = warg + off_first2;
        const charT* const warg_end = warg + off_last2;

        const Iterator first (warg_beg, warg_beg,           warg_end);
        const Iterator last  (warg_end, warg + tcase.off2,  warg_end);

        s_str.replace (it_first, it_last, first, last);
    }
    else {
        const StringIter first (std::size_t (tcase.off2) >= s_str.size () ?
                                s_str.end () : s_str.begin () + tcase.size2);
        const StringIter last (off_last2 > s_str.size () ?
                               s_str.end () : s_str.begin () + off_last2);

        s_str.replace (it_first, it_last, first, last);
    }

    const std::size_t match =
        rw_match (tcase.res, s_str.c_str(), tcase.nres);

    rw_assert (match == tcase.nres, 0, tcase.line,
               "line %d. %{$FUNCALL} expected %{#*s}, got %{/*.*Gs}, "
               "difference at offset %zu for %s",
               __LINE__, int (tcase.nres), tcase.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits>
void test_replace_range (const charT* wstr,
                         const charT* warg,
                         Traits*,
                         const TestCase &tcase)
{
    if (tcase.bthrow)  // this method doesn't throw
        return;

    test_replace_range (wstr, warg, (Traits*)0,
                       InputIter<charT>(0, 0, 0), tcase);

    // there is no need to call test_replace_range
    // for other iterators in this case
    if (0 == tcase.arg)
        return;

    test_replace_range (wstr, warg, (Traits*)0,
                       ConstFwdIter<charT>(0, 0, 0), tcase);

    test_replace_range (wstr, warg, (Traits*)0,
                       ConstBidirIter<charT>(0, 0, 0), tcase);

    test_replace_range (wstr, warg, (Traits*)0,
                       ConstRandomAccessIter<charT>(0, 0, 0), tcase);
}

/**************************************************************************/

template <class charT, class Traits>
void test_replace (charT, Traits*,
                   OverloadId      which,
                   const TestCase &tcase)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::iterator                StringIter;
    typedef typename TestString::const_iterator          ConstStringIter;

    typedef typename UserTraits<charT>::MemFun UTMemFun;

    const bool use_iters = Replace (iter_iter_ptr) <= which;

    static charT wstr [LLEN];
    static charT warg [LLEN];

    rw_widen (wstr, tcase.str, tcase.str_len);
    rw_widen (warg, tcase.arg, tcase.arg_len);

    // special processing for replace_range to exercise all iterators
    if (Replace (iter_iter_range) == which) {
        test_replace_range (wstr, warg, (Traits*)0, tcase);
        return;
    }

    /* const */ TestString s_str (wstr, tcase.str_len);
    const       TestString s_arg (warg, tcase.arg_len);

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (s_str));

    std::size_t res_off = 0;

    int first = use_iters ? tcase.off : tcase.str_len + 1;
    int last  = use_iters ? tcase.off + tcase.size : tcase.str_len + 1;
    std::size_t size = tcase.size2 >= 0 ? tcase.size2 : s_str.max_size () + 1;

    StringIter it_first (std::size_t (first) >= s_str.size () ?
                         s_str.end () : s_str.begin () + first);
    StringIter it_last  (std::size_t (last) >= s_str.size () ?
                         s_str.end () : s_str.begin () + last);

    // string function argument
    const charT* const arg_ptr = tcase.arg ? warg : s_str.c_str ();
    const TestString&  arg_str = tcase.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    std::size_t total_length_calls = 0;
    std::size_t n_length_calls = 0;
    std::size_t* const rg_calls = rw_get_call_counters ((Traits*)0, (charT*)0);

    if (rg_calls)
        total_length_calls = rg_calls [UTMemFun::length];

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rwt_free_store* const pst = rwt_get_free_store (0);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // iterate for`throw_after' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_after;
    for (throw_after = 0; ; ++throw_after) {

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        if (-1 == tcase.bthrow)
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#endif   // _RWSTD_NO_EXCEPTIONS

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        if (1 == tcase.bthrow && !use_iters)
            expected = exceptions [1];      // out_of_range
        if (2 == tcase.bthrow && Replace (size_size_str_size_size) == which)
            expected = exceptions [1];      // out_of_range
        if (3 == tcase.bthrow && !use_iters)
            expected = exceptions [2];      // length_error
        if (-1 == tcase.bthrow)
            expected = exceptions [3];      // bad_alloc

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow)
            return;

#endif   // _RWSTD_NO_EXCEPTIONS

        try {
            switch (which) {
            case Replace (size_size_ptr): {
                const TestString& s_res = 
                    s_str.replace (tcase.off, tcase.size, arg_ptr);
                res_off = &s_res - &s_str;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            }

            case Replace (size_size_str): {
                const TestString& s_res = 
                    s_str.replace (tcase.off, tcase.size, arg_str);
                res_off = &s_res - &s_str;
                break;
            }

            case Replace (size_size_ptr_size): {
                const TestString& s_res = 
                    s_str.replace (tcase.off, tcase.size, arg_ptr, size);
                res_off = &s_res - &s_str;
                break;
            }

            case Replace (size_size_str_size_size): {
                const TestString& s_res = 
                    s_str.replace (tcase.off, tcase.size, arg_str,
                                   tcase.off2, size);
                res_off = &s_res - &s_str;
                break;
            }

            case Replace (size_size_size_val): {
                const TestString& s_res = 
                    s_str.replace (tcase.off, tcase.size, size, arg_val);
                res_off = &s_res - &s_str;
                break;
            }

            case Replace (iter_iter_ptr): {
                const TestString& s_res = 
                    s_str.replace (it_first, it_last, arg_ptr);
                res_off = &s_res - &s_str;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            }

            case Replace (iter_iter_str): {
                const TestString& s_res = 
                    s_str.replace (it_first, it_last, arg_str);
                res_off = &s_res - &s_str;
                break;
            }

            case Replace (iter_iter_ptr_size): {
                const TestString& s_res = 
                    s_str.replace (it_first, it_last, arg_ptr, size);
                res_off = &s_res - &s_str;
                break;
            }

            case Replace (iter_iter_size_val): {
                const TestString& s_res = 
                    s_str.replace (it_first, it_last, size, arg_val);
                res_off = &s_res - &s_str;
                break;
            }

            default:
                RW_ASSERT (!"logic error: unknown replace overload");
                return;
            }

            // verify the returned value
            rw_assert (0 == res_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} returned invalid reference, "
                       "offset is %zu", __LINE__, res_off);

            // verfiy that strings length are equal
            rw_assert (tcase.nres == s_str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{#*s} with length "
                       "%zu, got %{/*.*Gs} with length %zu",
                       __LINE__, int (tcase.nres), tcase.res, 
                       tcase.nres, int (sizeof (charT)), 
                       int (s_str.size ()), s_str.c_str (), s_str.size ());

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
            const bool verify_length_using = Replace (size_size_ptr) == which 
                                          || Replace (iter_iter_ptr) == which;

            if (verify_length_using && rg_calls) {
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

    _RWSTD_UNUSED (pst);
    _RWSTD_UNUSED (throw_after);

#endif   // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

DEFINE_TEST_DISPATCH (test_replace);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::replace_ ## tag, tag ## _test_cases,     \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (size_size_ptr),
        TEST (size_size_str),
        TEST (size_size_ptr_size),
        TEST (size_size_str_size_size),
        TEST (size_size_size_val),
        TEST (iter_iter_ptr),
        TEST (iter_iter_str),
        TEST (iter_iter_ptr_size),
        TEST (iter_iter_size_val),
        TEST (iter_iter_range)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return StringMembers::run_test (argc, argv, __FILE__,
                                    "lib.string.replace",
                                    test_replace, tests, test_count);
}
