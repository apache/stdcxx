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
#include <cstddef>      // for ptrdiff_t, size_t

#include <21.strings.h>
#include <alg_test.h>       // for InputIter
#include <cmdopt.h>         // for rw_enabled()
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Replace(which)    StringMembers::replace_ ## which

typedef StringMembers::Function  Function;
typedef StringMembers::TestCase  TestCase;

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
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
    //    |            |  |  |           |        |   -1   - no exceptions
    //    |            |  |  |           |        |    0   - exception safety
    //    |            |  |  |           |        |    1,2 - out_of_range
    //    |            |  |  |           |        |    3   - length_error
    //    |            |  |  |           |        |
    //    |            |  |  |           |        +-------+
    //    |            |  |  |           |                |
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

    TEST ("x@4096",    0, 4095, "ab",    "abx",           0),
    TEST ("\0@4096",   1, 4094, "abc",   "\0abc\0",       0),
    TEST ("x@4096",    1, 4094, "ab",    "xabx",          0),
    TEST ("x@4096",    0, 4094, "ab",    "abxx",          0),
    TEST ("x@4096",    1, 4093, "",      "xxx",           0),

    TEST ("",          0,    0, "x@4096", "x@4096",       0),
    TEST ("a",         0,    1, "x@4096", "x@4096",       0),
    TEST ("x@4096",    0, 4095, "x@4096", "x@4097",       0),
    TEST ("\0ab\0\0c", 0,    6, "x@4096", "x@4096",       0),

    TEST ("\0",         2,   0, "",       "\0",           1),
    TEST ("a",         10,   0, "",       "a",            1),
    TEST ("x@4096",  4106,   0, "",       "x@4096",       1),

    TEST ("",           0,   0, "x@4096", "x@4096",       0),

    // self-referential replacement
    TEST ("",          0,    0, 0,        "",             0),
    TEST ("abc",       0,    3, 0,        "abc",          0),
    TEST ("abc",       1,    1, 0,        "aabcc",        0),
    TEST ("a\0b\0c\0", 2,    3, 0,        "a\0a\0",       0),
    TEST ("a\0b\0c\0", 0,    0, 0,        "aa\0b\0c\0",   0),
    TEST ("a\0b\0c\0", 6,    0, 0,        "a\0b\0c\0a",   0),
    TEST ("\0ab\0c\0", 3,    3, 0,        "\0ab",         0),
    TEST ("x@4096",    0, 4095, 0,        "x@4097",       0),

    TEST ("last",      4, 0, "test",     "lasttest",      0)
};

/**************************************************************************/

// exercises:
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

    //    +------------------------------------------- controlled sequence
    //    |             +----------------------------- replace() pos1 argument
    //    |             |  +-------------------------- replace() n1 argument
    //    |             |  |  +----------------------- sequence to be inserted
    //    |             |  |  |           +----------- expected result sequence
    //    |             |  |  |           |        +-- exception info
    //    |             |  |  |           |        |   -1   - no exceptions
    //    |             |  |  |           |        |    0   - exception safety
    //    |             |  |  |           |        |    1,2 - out_of_range
    //    |             |  |  |           |        |    3   - length_error
    //    |             |  |  |           |        |
    //    |             |  |  |           |        +-----------+
    //    |             |  |  |           |                    |
    //    V             V  V  V           V                    V
    TEST ("ab",         0, 0, "c",        "cab",               0),

    TEST ("",           0, 0, "",         "",                  0),
    TEST ("",           0, 0, "\0",       "\0",                0),
    TEST ("",           0, 0, "abc",      "abc",               0),

    TEST ("ab",         0, 2, "",         "",                  0),
    TEST ("ab",         0, 1, "",         "b",                 0),
    TEST ("ab",         1, 1, "\0",       "a\0",               0),

    TEST ("\0",         0, 1, "",         "",                  0),
    TEST ("\0",         0, 1, "a",        "a",                 0),
    TEST ("\0",         0, 1, "\0\0",     "\0\0",              0),

    TEST ("ah",         0, 1, "bcdefg",   "bcdefgh",           0),
    TEST ("ah",         1, 1, "bcdefg",   "abcdefg",           0),
    TEST ("ah",         0, 2, "bcdefg",   "bcdefg",            0),

    TEST ("abc",        0, 2, "cc",       "ccc",               0),
    TEST ("abc",        1, 2, "cc",       "acc",               0),

    TEST ("abc",        0, 3, "defgh",    "defgh",             0),
    TEST ("abc",        2, 1, "defgh",    "abdefgh",           0),
    TEST ("abc",        2, 1, "de\0gh",   "abde\0gh",          0),
    TEST ("abc",        2, 1, "",         "ab",                0),
    TEST ("abc",        1, 1, "defgh",    "adefghc",           0),

    TEST ("abc",        0, 0, "ee",       "eeabc",             0),
    TEST ("abc",        0, 0, "\0\0e\0",  "\0\0e\0abc",        0),
    TEST ("abc",        2, 0, "ee",       "abeec",             0),
    TEST ("abc",        1, 0, "ee",       "aeebc",             0),
    TEST ("abc",        1, 0, "e\0\0",    "ae\0\0bc",          0),

    TEST ("a\0b\0\0c",  0, 3, "",         "\0\0c",             0),
    TEST ("a\0b\0\0c",  0, 3, "\0e",      "\0e\0\0c",          0),

    TEST ("a\0b\0\0c",  2, 3, "\0e",      "a\0\0ec",           0),
    TEST ("a\0b\0\0c",  0, 3, "\0\0e\0",  "\0\0e\0\0\0c",      0),
    TEST ("\0ab\0\0c",  1, 2, "\0e\0\0",  "\0\0e\0\0\0\0c",    0),

    TEST ("\0ab\0\0c",  0, 0, "\0e",      "\0e\0ab\0\0c",      0),
    TEST ("a\0b\0c\0",  6, 0, "e\0",      "a\0b\0c\0e\0",      0),
    TEST ("\0ab\0\0c",  5, 0, "\0e",      "\0ab\0\0\0ec",      0),

    TEST ("x@4096",     0, 4095, "ab",    "abx",               0),
    TEST ("x@4096",     1, 4094, "ab",    "xabx",              0),
    TEST ("x@4096",     0, 4094, "ab",    "abxx",              0),
    TEST ("x@4096",     1, 4093, "",      "xxx",               0),
    TEST ("x@4096",     1, 4092, "\0\0",  "x\0\0xxx",          0),

    TEST ("",           0,    0, "x@4096", "x@4096",           0),
    TEST ("a",          0,    1, "x@4096", "x@4096",           0),
    TEST ("x@4096",     0, 4095, "x@4096", "x@4097",           0),
    TEST ("\0ab\0\0c",  0,    6, "x@4096", "x@4096",           0),

    TEST ("\0",         2,    0, "",        "\0",              1),
    TEST ("a",         10,    0, "",        "a",               1),
    TEST ("x@4096",  4106,    0, "",        "x@4096",          1),
    TEST ("",           0,    0, "x@4096",  "x@4096",          0),

    // self-referential replacement
    TEST ("abc",        0,    3, 0, "abc",                     0),
    TEST ("abc",        1,    1, 0, "aabcc",                   0),
    TEST ("a\0b\0c\0",  2,    3, 0, "a\0a\0b\0c\0\0",          0),
    TEST ("a\0b\0c\0",  0,    0, 0, "a\0b\0c\0a\0b\0c\0",      0),
    TEST ("a\0b\0c\0",  6,    0, 0, "a\0b\0c\0a\0b\0c\0",      0),
    TEST ("\0ab\0c\0",  3,    3, 0, "\0ab\0ab\0c\0",           0),
    TEST ("x@4096",     0, 4095, 0, "x@4097",                  0),

    TEST ("last",       4,    0, "test",     "lasttest",       0)
};

/**************************************************************************/

// exercises:
// replace (size_type, size_type, const value_type*, size_type)
// replace (iterator, iterator, const value_type*, size_type)
static const TestCase
iter_iter_ptr_size_test_cases [] = {

// iter_iter_ptr_size_test_cases serves a double duty
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
    //    |            |  |  |            |  |     +- exception info
    //    |            |  |  |            |  |     |  -1   - no exceptions
    //    |            |  |  |            |  |     |   0   - exception safety
    //    |            |  |  |            |  |     |   1,2 - out_of_range
    //    |            |  |  |            |  |     |   3   - length_error
    //    |            |  |  |            |  |     |
    //    |            |  |  |            |  |     +------------+
    //    |            |  |  |            |  |                  |
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

    TEST ("x@4096",    1, 4095, "\0",        1, "x\0",          0),
    TEST ("x@4096",    0, 4095, "ab",        2, "abx",          0),
    TEST ("x@4096",    1, 4094, "ab",        1, "xax",          0),
    TEST ("x@4096",    0, 4094, "ab",        2, "abxx",         0),
    TEST ("x@4096",    1, 4093, "",          0, "xxx",          0),
    TEST ("x@4096",    1, 4092, "\0\0",      2, "x\0\0xxx",     0),

    TEST ("a",         0,    1, "x@4096", 4095, "x@4095",       0),
    TEST ("x@4096",    0, 4095, "y@4096", 4095, "y@4095x",      0),
    TEST ("x@4096",    0, 2047, "x@4096", 2047, "x@4096",       0),

    TEST ("x@4096", 2047, 2048, "x@4096", 2048,  "x@4096",      0),

    TEST ("\0",         2,   0, "",         0, "\0",            1),
    TEST ("a",         10,   0, "",         0, "a",             1),
    TEST ("x@4096",  4106,   0, "",         0, "x@4096",        1),

    TEST ("a",          0,   1, "x@4096", 4095, "x@4095",       0),

    // self-referential replacement
    TEST ("a",         0,    0, 0 /* self */,    1, "aa",              0),
    TEST ("a",         0,    1, 0 /* self */,    1, "a",               0),
    TEST ("a",         1,    0, 0 /* self */,    1, "aa",              0),
    TEST ("abc",       0,    3, 0 /* self */,    2, "ab",              0),
    TEST ("abc",       1,    1, 0 /* self */,    3, "aabcc",           0),
    TEST ("a\0b\0c\0", 2,    3, 0 /* self */,    6, "a\0a\0b\0c\0\0",  0),
    TEST ("a\0b\0c\0", 0,    0, 0 /* self */,    4, "a\0b\0a\0b\0c\0", 0),
    TEST ("\0ab\0c\0", 6,    0, 0 /* self */,    1, "\0ab\0c\0\0",     0),
    TEST ("\0ab\0c\0", 3,    3, 0 /* self */,    2, "\0ab\0a",         0),
    TEST ("a@4096",    0,    1, 0 /* self */, 1111, "a@5206",          0),
    TEST ("b@4096",    1,    2, 0 /* self */, 2222, "b@6316",          0),
    TEST ("x@4096",    0, 4095, 0 /* self */, 4095, "x@4096",          0),
    TEST ("x@4096",    0, 4095, 0 /* self */,    1, "xx",              0),

    TEST ("last",      4,    0, "test",          4, "lasttest",        0)
};

/**************************************************************************/

// exercises:
// replace (size_type, size_type, basic_string&, size_type, size_type)
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

    //    +------------------------------------------- controlled sequence
    //    |            +------------------------------ replace() pos argument
    //    |            |   +-------------------------- replace() n1 argument
    //    |            |   |  +----------------------- sequence to be inserted
    //    |            |   |  |           +----------- replace() pos2 argument
    //    |            |   |  |           |   +------- replace() n2 argument
    //    |            |   |  |           |   |  +---- expected result sequence
    //    |            |   |  |           |   |  | +-- exception info:
    //    |            |   |  |           |   | |  |   -1    - no exceptions
    //    |            |   |  |           |   |  | |    0    - exception safety
    //    |            |   |  |           |   |  | |    1, 2 - out_of_range
    //    |            |   |  |           |   |  | |    3    - length_error
    //    |            |   |  |           |   |  | |
    //    |            |   |  |           |   |  | +-----------------+
    //    |            |   |  |           |   |  |                   |
    //    V            V   V  V           V   V  V                   V
    TEST ("",          0,  0, "",         0,  0, "",                 0),
    TEST ("",          0,  0, "",         0, -1, "",                 0),
    TEST ("",          0,  0, "",         0,  1, "",                 0),

    TEST ("",          0,  1, "",         0,  0, "",                 0),
    TEST ("",          0,  1, "",         0,  1, "",                 0),
    TEST ("",          0,  1, "",         0, -1, "",                 0),

    TEST ("",          0, -1, "",         0,  0, "",                 0),
    TEST ("",          0, -1, "",         0,  1, "",                 0),
    TEST ("",          0, -1, "",         0, -1, "",                 0),

    TEST ("1",         0,  0, "",         0,  0, "1",                0),
    TEST ("2",         0,  0, "",         0, -1, "2",                0),
    TEST ("3",         0,  0, "",         0,  1, "3",                0),

    TEST ("4",         0,  1, "",         0,  0, "",                 0),
    TEST ("5",         0,  1, "",         0,  1, "",                 0),
    TEST ("6",         0,  1, "",         0, -1, "",                 0),

    TEST ("7",         0, -1, "",         0,  0, "",                 0),
    TEST ("8",         0, -1, "",         0,  1, "",                 0),
    TEST ("9",         0, -1, "",         0, -1, "",                 0),

    TEST ("1",         0,  0, "9",        0,  0, "1",                0),
    TEST ("2",         0,  0, "8",        0, -1, "82",               0),
    TEST ("3",         0,  0, "7",        0,  1, "73",               0),

    TEST ("4",         0,  1, "6",        0,  0, "",                 0),
    TEST ("5",         0,  1, "5",        0,  1, "5",                0),
    TEST ("6",         0,  1, "4",        0, -1, "4",                0),

    TEST ("7",         0, -1, "3",        0,  0, "",                 0),
    TEST ("8",         0, -1, "2",        0,  1, "2",                0),
    TEST ("9",         0, -1, "1",        0, -1, "1",                0),

    TEST ("",          0,  0, "abc",      0,  3, "abc",              0),

    TEST ("ab",        0,  0, "c",        0, 1, "cab",               0),
    TEST ("ab",        0,  2, "",         0, 0, "",                  0),
    TEST ("ab",        0,  1, "",         0, 0, "b",                 0),

    TEST ("\0",        0,  1, "",         0, 0, "",                  0),
    TEST ("\0",        0,  1, "a",        0, 1, "a",                 0),
    TEST ("\0",        0,  1, "\0\0",     1, 1, "\0",                0),
    TEST ("\0",        0,  1, "\0\0",     0, 2, "\0\0",              0),

    TEST ("ah",        0,  1, "bcdefg",   0, 3, "bcdh",              0),
    TEST ("ah",        1,  1, "bcdefg",   0, 3, "abcd",              0),
    TEST ("ah",        0,  1, "bcdefg",   1, 3, "cdeh",              0),
    TEST ("ah",        1,  1, "bcdefg",   1, 3, "acde",              0),
    TEST ("ah",        0,  1, "bcdefg",   0, 6, "bcdefgh",           0),
    TEST ("ah",        1,  1, "bcdefg",   0, 6, "abcdefg",           0),

    TEST ("abc",       0,  2, "cc",       0, 2, "ccc",               0),
    TEST ("abc",       1,  2, "cc",       0, 2, "acc",               0),

    TEST ("abc",       0,  3, "d",        0, 1, "d",                 0),
    TEST ("abc",       0,  3, "def",      0, 3, "def",               0),
    TEST ("abc",       0,  3, "defgh",    0, 5, "defgh",             0),
    TEST ("abc",       2,  1, "defgh",    4, 1, "abh",               0),
    TEST ("abc",       2,  1, "de\0gh",   2, 1, "ab\0",              0),
    TEST ("abc",       2,  1, "",         0, 0, "ab",                0),

    TEST ("abc",       1,  1, "defgh",    1, 2, "aefc",              0),
    TEST ("abc",       0,  0, "ee",       0, 2, "eeabc",             0),
    TEST ("abc",       0,  0, "\0\0e\0",  0, 4, "\0\0e\0abc",        0),
    TEST ("abc",       2,  0, "ee",       0, 2, "abeec",             0),
    TEST ("abc",       2,  1, "\0e\0\0",  0, 4, "ab\0e\0\0",         0),
    TEST ("abc",       1,  0, "ee",       0, 2, "aeebc",             0),
    TEST ("abc",       1,  0, "\0e\0\0",  0, 4, "a\0e\0\0bc",        0),

    TEST ("a\0b\0\0c", 0,  3, "",         0, 0, "\0\0c",             0),
    TEST ("\0ab\0\0c", 0,  3, "",         0, 0, "\0\0c",             0),
    TEST ("a\0b\0\0c", 0,  3, "\0e",      0, 2, "\0e\0\0c",          0),

    TEST ("a\0b\0\0c", 2,  3, "\0e",      0, 2, "a\0\0ec",           0),
    TEST ("a\0b\0\0c", 2,  3, "\0e",      1, 1, "a\0ec",             0),
    TEST ("\0ab\0\0c", 2,  3, "\0e",      0, 2, "\0a\0ec",           0),
    TEST ("\0ab\0\0c", 2,  3, "\0e\0\0",  1, 3, "\0ae\0\0c",         0),

    TEST ("a\0b\0\0c", 0,  6, "\0e",      0, 2, "\0e",               0),
    TEST ("a\0b\0\0c", 0,  6, "\0e",      0, 1, "\0",                0),

    TEST ("\0ab\0\0c", 0,  0, "\0e",      0, 2, "\0e\0ab\0\0c",      0),
    TEST ("a\0b\0c\0", 6,  0, "e\0",      0, 2, "a\0b\0c\0e\0",      0),
    TEST ("\0ab\0\0c", 5,  0, "\0e",      0, 2, "\0ab\0\0\0ec",      0),

    ///////////////////////////////////////////////////////////////////////
    // very long strings
    TEST ("a",         0,     0, "b",      0, -1, "ba",               0),
    TEST ("a@0",       0,     0, "b@0",    0, -1, "",                 0),
    TEST ("a@0",       0,     0, "b@1",    0, -1, "b",                0),
    TEST ("a@1",       0,     0, "b@0",    0, -1, "a",                0),
    TEST ("a@1",       0,     0, "b@1",    0, -1, "ba",               0),
    TEST ("a@2",       0,     0, "b@2",    0, -1, "bbaa",             0),

    TEST ("a@1000",    0,     0, "b@1000", 0, -1, "b@1000a@1000",     0),
    TEST ("a@1000",    0,     1, "b@1001", 0, -1, "b@1001a@999",      0),
    TEST ("a@1000",    0,     2, "b@1002", 0, -1, "b@1002a@998",      0),
    TEST ("a@1000",    1,   998, "b@1003", 0, -1, "ab@1003a",         0),
    TEST ("a@1000",    2,   996, "b@1004", 0, -1, "aab@1004aa",       0),
    TEST ("a@1000",  500,   250, "b@1005", 0, -1, "a@500b@1005a@250", 0),
    TEST ("a@1000",  998,     1, "b@1006", 0, -1, "a@998b@1006a",     0),
    TEST ("a@2000", 1000,    -1, "b",      0, -1, "a@1000b",          0),
    TEST ("a@2000", 1000,   999, "bb",     0, -1, "a@1000bba",        0),

    TEST ("x@4096",    0,  4095, "ab",       0, 2, "abx",             0),
    TEST ("x@4096",    1,  4094, "ab",       0, 2, "xabx",            0),
    TEST ("x@4096",    0,  4094, "ab",       0, 2, "abxx",            0),
    TEST ("x@4096",    1,  4093, "",         0, 0, "xxx",             0),
    TEST ("x@4096",    1,  4092, "\0\0",     0, 2, "x\0\0xxx",        0),

    TEST ("a",      0,     1, "x@4096",   0, 4095, "x@4095",         0),
    TEST ("x@4096", 0,  4095, "x@4096",   0, 4095, "x@4096",         0),
    TEST ("x@4096", 0,  2047, "x@4096",   0, 2047, "x@4096",         0),

    TEST ("x@4096", 2047, 2048, "x@4096", 0, 2048,  "x@4096",        0),

    TEST ("\0",         2, 0, "",           0, 0, "\0",              1),
    TEST ("\0",         0, 0, "\0",         2, 0, "",                2),

    TEST ("a",         10, 0, "",           0, 0, "a",               1),
    TEST ("a",          0, 0, "a",         10, 0, "",                2),

    TEST ("x@4096",  4106, 0, "",           0, 0, "x@4096",          1),
    TEST ("x@4096",     0, 0, "x@4096", 4106, 0, "",                 2),

    TEST ("a",          0, 1, "x@4096", 0, 4095, "x@4095",           0),

    ///////////////////////////////////////////////////////////////////////
    // self-referential replacement
    TEST ("",          0,  0, 0,          0,  0, "",                 0),
    TEST ("",          0,  0, 0,          0,  1, "",                 0),
    TEST ("",          0,  0, 0,          0, -1, "",                 0),

    TEST ("",          0,  1, 0,          0,  0, "",                 0),
    TEST ("",          0,  1, 0,          0,  1, "",                 0),
    TEST ("",          0,  1, 0,          0, -1, "",                 0),

    TEST ("",          0, -1, 0,          0,  0, "",                 0),
    TEST ("",          0, -1, 0,          0,  1, "",                 0),
    TEST ("",          0, -1, 0,          0, -1, "",                 0),

    TEST ("1",         0,  0, 0,          0,  0, "1",                0),
    TEST ("2",         0,  0, 0,          0, -1, "22",               0),
    TEST ("3",         0,  0, 0,          0,  1, "33",               0),

    TEST ("4",         0,  1, 0,          0,  0, "",                 0),
    TEST ("5",         0,  1, 0,          0,  1, "5",                0),
    TEST ("6",         0,  1, 0,          0, -1, "6",                0),

    TEST ("7",         0, -1, 0,          0,  0, "",                 0),
    TEST ("8",         0, -1, 0,          0,  1, "8",                0),
    TEST ("9",         0, -1, 0,          0, -1, "9",                0),

    TEST ("x@4096",    0,  0, 0,          0,  0, "x@4096",           0),
    TEST ("x@4096",    0,  1, 0,          0,  1, "x@4096",           0),
    TEST ("x@4096",    0,  2, 0,          0,  2, "x@4096",           0),
    TEST ("x@4096",    0, -1, 0,          0, -1, "x@4096",           0),

    TEST ("abc",       0, 0, 0,           1, 1,  "babc",             0),
    TEST ("abc",       2, 0, 0,           0, 2,  "ababc",            0),
    TEST ("a\0bc\0\0", 0, 0, 0,           4, 2,  "\0\0a\0bc\0\0",    0),
    TEST ("a\0bc\0\0", 6, 0, 0,           1, 3,  "a\0bc\0\0\0bc",    0),
    TEST ("abcdef",    0, 0, 0,           1, 2,  "bcabcdef",         0),

    TEST ("last",      4, 0, "test",      0, 4, "lasttest",          0)
};

/**************************************************************************/

// exercises:
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
    //    |            |  |  |   |    |       +------ exception info
    //    |            |  |  |   |    |       |       -1   - no exceptions
    //    |            |  |  |   |    |       |        0   - exception safety
    //    |            |  |  |   |    |       |        1,2 - out_of_range
    //    |            |  |  |   |    |       |        3   - length_error
    //    |            |  |  |   |    |       |
    //    |            |  |  |   |    |       +--------+
    //    |            |  |  |   |    |                |
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

    TEST ("x@4096",    0, 4095, 2, 'a',  "aax",        0),
    TEST ("x@4096",    1, 4094, 2, 'a',  "xaax",       0),
    TEST ("x@4096",    0, 4094, 2, 'a',  "aaxx",       0),
    TEST ("x@4096",    1, 4093, 0, 'a',  "xxx",        0),
    TEST ("x@4096",    1, 4092, 1, '\0', "x\0xxx",     0),

    TEST ("a",         0,    1, 4095, 'x', "x@4095",       0),
    TEST ("x@4096",    0, 4095, 4095, 'a', "a@4095x",      0),
    TEST ("x@4096",    0, 2047, 2047, 'b', "b@2047x@2049", 0),

    TEST ("x@4096", 2047, 2048, 2048,   'x', "x@4096",     0),

    TEST ("\0",         2,   0,    0, ' ',  "\0",          1),
    TEST ("a",         10,   0,    0, ' ',  "a",           1),
    TEST ("x@4096",  4106,   0,    0, ' ',  "x@4096",      1),

    TEST ("a",          0,   1, 4095, 'x', "x@4095",       0),

    TEST ("last",      4, 0, 4, 't', "lasttttt",           0)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator, class Iterator>
void test_replace_range (const charT*    wstr,
                         std::size_t     wstr_len,
                         const charT*    warg,
                         std::size_t     warg_len,
                         std::size_t     res_len,
                         Traits*,
                         Allocator*,
                         const Iterator &it,
                         const TestCase &tcase)
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
    String str (wstr, size1);

    // compute the offset and the extent (the number of elements)
    // of the first range into the string object being modified
    const std::size_t off1 =
        std::size_t (tcase.off) < size1 ? std::size_t (tcase.off) : size1;
    const std::size_t ext1 =
        off1 + tcase.size < size1 ? std::size_t (tcase.size) : size1 - off1;

    // compute the offset and the extent (the number of elements)
    // of the second range into the argument of the function call
    const std::size_t off2 =
        std::size_t (tcase.off2) < size2 ? std::size_t (tcase.off2) : size2;
    const std::size_t ext2 =
        off2 + tcase.size2 < size2 ? std::size_t (tcase.size2) : size2 - off2;

    // create a pair of iterators into the string object being modified
    const StringIter first1 (str.begin () + off1);
    const StringIter last1 (first1 + ext1);

    if (tcase.arg) {
        const charT* const beg = warg + off2;
        const charT* const end = beg + ext2;

        const Iterator first2 (beg, beg, end);
        const Iterator last2  (end, beg, end);

        str.replace (first1, last1, first2, last2);
    }
    else {
        // self-referential modification (replacing a range
        // of elements with a subrange of its own elements)
        const StringIter first2 (str.begin () + off2);
        const StringIter last2 (first2 + ext2);

        str.replace (first1, last1, first2, last2);
    }

    // detrmine whether the produced sequence matches the exepceted result
    const std::size_t match = rw_match (tcase.res, str.data (), tcase.nres);

    rw_assert (match == res_len, 0, tcase.line,
               "line %d. %{$FUNCALL} expected %{#*s}, got %{/*.*Gs}, "
               "difference at offset %zu for %s",
               __LINE__, int (tcase.nres), tcase.res,
               int (sizeof (charT)), int (str.size ()), str.c_str (),
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_replace_range (const charT    *wstr,
                         std::size_t     wstr_len,
                         const charT    *warg,
                         std::size_t     warg_len,
                         std::size_t     res_len,
                         Traits*,
                         Allocator*,
                         const TestCase &tcase)
{
    if (tcase.bthrow) {
        // FIXME: exercise exceptions
        return;
    }

    // exercise InputIterator *or* string::iterator (i.e., self
    // referential modification), depending on the value of tcase.arg
    test_replace_range (wstr, wstr_len, warg, warg_len, res_len,
                        (Traits*)0, (Allocator*)0,
                        InputIter<charT>(0, 0, 0), tcase);

    if (0 == tcase.arg) {
        // avoid exercising the same function multiple times
        return;
    }

    test_replace_range (wstr, wstr_len, warg, warg_len, res_len,
                        (Traits*)0, (Allocator*)0,
                        ConstFwdIter<charT>(0, 0, 0), tcase);

    test_replace_range (wstr, wstr_len, warg, warg_len, res_len,
                        (Traits*)0, (Allocator*)0,
                        ConstBidirIter<charT>(0, 0, 0), tcase);

    test_replace_range (wstr, wstr_len, warg, warg_len, res_len,
                        (Traits*)0, (Allocator*)0,
                        ConstRandomAccessIter<charT>(0, 0, 0), tcase);
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_replace (charT, Traits*, Allocator*,
                   const Function &func,
                   const TestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;
    typedef typename UserTraits<charT>::MemFun           UTMemFun;

    static const std::size_t BUFSIZE = 256;

    // small local buffers to avoid expensive dynamic memory
    // allocation in most cases
    static charT wstr_buf [BUFSIZE];
    static charT warg_buf [BUFSIZE];
    static charT wres_buf [BUFSIZE];

    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    std::size_t arg_len = sizeof warg_buf / sizeof *warg_buf;
    std::size_t res_len = sizeof wres_buf / sizeof *wres_buf;

    // expand "compressed" string literals in the form "a@<count>"
    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);
    charT* warg = rw_expand (warg_buf, tcase.arg, tcase.arg_len, &arg_len);
    charT* wres = rw_expand (wres_buf, tcase.res, tcase.nres,    &res_len);

    if (Replace (iter_iter_range) == func.which_) {
        // special processing for the replace() template member
        // function to exercise all iterator categories
        test_replace_range (wstr, str_len, warg, arg_len, 
                            res_len, (Traits*)0, (Allocator*)0, tcase);

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

    // compute the offset and the extent (the number of elements)
    // of the first range into the string object being modified
    const std::size_t size1 = str_len;
    const std::size_t off1 =
        std::size_t (tcase.off) < size1 ? std::size_t (tcase.off) : size1;
    const std::size_t ext1 =
        off1 + tcase.size < size1 ? tcase.size : size1 - off1;

    // create a pair of iterators into the string object being
    // modified (used only by the iterator overloads)
    const StringIter first (str.begin () + off1);
    const StringIter last (first + ext1);

    // offset and extent function arguments
    const std::size_t arg_off  = std::size_t (tcase.off);
    const std::size_t arg_size = std::size_t (tcase.size);
    const std::size_t arg_off2 = std::size_t (tcase.off2);
    const std::size_t arg_size2 =
        tcase.size2 >= 0 ? tcase.size2 : str.max_size () + 1;

    // string function argument
    const charT* const arg_ptr = tcase.arg ? arg.c_str () : str.c_str ();
    const String&      arg_str = tcase.arg ? arg : str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    // get a pointer to the Traits::length() function call counter
    // or 0 when no such counter exists (i.e., when Traits is not
    // UserTraits)
    std::size_t* length_calls =
           Replace (size_size_ptr) == func.which_
        || Replace (iter_iter_ptr) == func.which_ ?
        rw_get_call_counters ((Traits*)0, (charT*)0) : 0;

    if (length_calls) {
        length_calls += UTMemFun::length;
        *length_calls = 0;
    }

    rwt_free_store* const pst = rwt_get_free_store (0);
    SharedAlloc*    const pal = SharedAlloc::instance ();

    // out_of_range is only generated from size_type overloads
    // of replace() and not from the iterator equivalents of
    // the same functions
    const bool use_iters = Replace (iter_iter_ptr) <= func.which_;

    // pointer to the returned reference
    const String* ret_ptr = 0;

    // iterate for`throw_count' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_count;
    for (throw_count = 0; ; ++throw_count) {

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        if (1 == tcase.bthrow && !use_iters)
            expected = exceptions [1];      // out_of_range
        else if (2 == tcase.bthrow)
            expected = exceptions [1];      // out_of_range
        else if (3 == tcase.bthrow && !use_iters)
            expected = exceptions [2];      // length_error
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

        // start checking for memory leaks
        rw_check_leaks (str.get_allocator ());

        try {
            switch (func.which_) {
            case Replace (size_size_ptr):
                ret_ptr = &str.replace (arg_off, arg_size, arg_ptr);
                break;

            case Replace (size_size_str):
                ret_ptr = &str.replace (arg_off, arg_size, arg_str);
                break;

            case Replace (size_size_ptr_size):
                ret_ptr = &str.replace (arg_off, arg_size, arg_ptr, arg_size2);
                break;

            case Replace (size_size_str_size_size):
                ret_ptr = &str.replace (arg_off, arg_size, arg_str,
                                        arg_off2, arg_size2);
                break;

            case Replace (size_size_size_val):
                ret_ptr = &str.replace (arg_off, arg_size, arg_size2, arg_val);
                break;

            case Replace (iter_iter_ptr):
                ret_ptr = &str.replace (first, last, arg_ptr);
                break;

            case Replace (iter_iter_str):
                ret_ptr = &str.replace (first, last, arg_str);
                break;

            case Replace (iter_iter_ptr_size):
                ret_ptr = &str.replace (first, last, arg_ptr, arg_size2);
                break;

            case Replace (iter_iter_size_val):
                ret_ptr = &str.replace (first, last, arg_size2, arg_val);
                break;

            default:
                RW_ASSERT (!"logic error: unknown replace overload");
            }

            // verify that the reference returned from the function
            // refers to the modified string object (i.e., *this
            // within the function)
            const std::ptrdiff_t res_off = ret_ptr - &str;

            // verify the returned value
            rw_assert (0 == res_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} returned invalid reference, "
                       "offset is %td", __LINE__, res_off);

            

            // verfiy that the length of the resulting string
            rw_assert (res_len == str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{/*.*Gs} with length "
                       "%zu, got %{/*.*Gs} with length %zu",
                       __LINE__, int (sizeof (charT)), int (res_len), wres, 
                       res_len, int (sizeof (charT)), 
                       int (str.size ()), str.c_str (), str.size ());

            if (res_len == str.size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, str.c_str (), tcase.nres);

                rw_assert (match == res_len, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %{/*.*Gs}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, int (sizeof (charT)), int (res_len), wres,
                           int (sizeof (charT)), int (str.size ()),
                           str.c_str (), match);
            }

            // verify that Traits::length() was used
            if (length_calls) {
                rw_assert (0 < *length_calls,
                           0, tcase.line, "line %d. %{$FUNCALL} doesn't "
                           "use traits_type::length()", __LINE__);
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
            // verify that the exception didn't cause a change
            // in the state of the object
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

    const std::size_t expect_throws = 0;

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

DEFINE_STRING_TEST_DISPATCH (test_replace);

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

    const int status =
        StringMembers::run_test (argc, argv, __FILE__,
                                 "lib.string.replace",
                                 test_replace, tests, test_count);

    return status;
}
