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
#include <cstdlib>      // for free(), size_t
#include <stdexcept>    // for out_of_range, length_error

#include <cmdopt.h>     // for rw_enabled()
#include <driver.h>     // for rw_test()

#include <rw_printf.h>  // for rw_asnprintf()
#include <rw_char.h>    // for rw_widen()
#include <alg_test.h>   // for InputIter<>

#include <21.strings.h>

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
   // disabled for compilers such as IBM VAC++ or MSVC
   // that can't reliably replace the operators
#  include <rw_new.h>
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

#define ReplaceOverload   StringMembers::MemberFunction
#define Replace(which)    StringMembers::replace_ ## which
#define Disabled(which)   StringMembers::opt_memfun_disabled [which]

typedef StringMembers::TestCase TestCase;
typedef StringMembers::Test     Test;

/**************************************************************************/

const char method_name[] = "replace";

/**************************************************************************/

struct MemFun
{
    typedef StringMembers::charT  charT;
    typedef StringMembers::Traits Traits;

    MemFun (charT cid, const char *cname,
          Traits tid, const char *tname)
        : cid_ (cid), tid_ (tid),
          cname_ (cname), tname_ (tname), aname_ ("allocator"),
          fname_ ("replace") { /* empty */ }

    charT       cid_;     // character type id (char or wchar_t)
    Traits      tid_;     // traits type id (default or user-defined)
    const char *cname_;   // character type name
    const char *tname_;   // traits name
    const char *aname_;   // allocator name
    const char *fname_;   // function name
};

/**************************************************************************/

// for convenience and brevity
#define LSTR  long_string
#define LLEN  long_string_len
// one half of the long_string length
#define LPAR  LLEN / 2

static const std::size_t long_string_len = 4096;
static char long_string [long_string_len];

static const char* const exp_exceptions[] =
    { "unknown exception", "out_of_range", "length_error" };

/**************************************************************************/

// used to exercise
// replace (size_type pos1, size_type n1, const charT* s)
static const TestCase
off_size_ptr_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, res, bthrow)                               \
    { Replace (off_size_ptr), __LINE__, off, size, -1, -1, -1,               \
      str, sizeof str - 1, arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

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
// replace (Iterator i1, Iterator i2, const charT* s)
static const TestCase
ptr_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, res, bthrow)                               \
    { Replace (ptr), __LINE__, off, size, -1, -1, -1,                        \
      str, sizeof str - 1, arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

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
// replace (size_type pos1, size_type n1, basic_string& s)
static const TestCase
off_size_str_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, res, bthrow)                               \
    { Replace (off_size_str), __LINE__, off, size, -1, -1, -1,               \
      str, sizeof str - 1, arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

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
// replace (Iterator i1, Iterator i2, basic_string& s)
static const TestCase
str_test_cases [] = {

#undef TEST
#define TEST(s, off, size, arg, res, bthrow)                                 \
    { Replace (str), __LINE__, off, size, -1, -1, -1,                        \
      s, sizeof s - 1, arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

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
// replace (size_type pos1, size_type n1, const charT* s, size_type n2)
static const TestCase
off_size_ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, count, res, bthrow)                        \
    { Replace (off_size_ptr_size), __LINE__, off, size, -1, count, -1,       \
      str, sizeof str - 1, arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

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
// replace (Iterator i1, Iterator i2, const charT* s, size_type n2)
static const TestCase
ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, count, res, bthrow)                        \
    { Replace (ptr_size), __LINE__, off, size, -1, count, -1,                \
      str, sizeof str - 1, arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

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
// replace (size_type pos1, size_type n1, basic_string& s,
//          size_type pos2, size_type n2)
static const TestCase
off_size_str_off_size_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, off2, size2, res, bthrow)                  \
    { Replace (off_size_str_off_size), __LINE__, off, size, off2, size2, -1, \
      str, sizeof str - 1, arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

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
    TEST (LSTR,         0, 0, LSTR, 0, LPAR - 1, 0,                -1),

    TEST ("last",      4, 0, "test",      0, 4, "lasttest",         0)
};


/**************************************************************************/

// used to exercise
// replace (iterator i1, Iterator i2, InputIterator j1, InputIterator j2)
static const TestCase
range_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, off2, size2, res, bthrow)                  \
    { Replace (range), __LINE__, off, size, off2, size2, -1,                 \
      str, sizeof str - 1, arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

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
    TEST (LSTR,         0, 0, LSTR, 0, LPAR - 1, 0,                -1),

    TEST ("last",      4, 0, "test",      0, 4, "lasttest",         0)
};

/**************************************************************************/

// used to exercise
// replace (size_type pos1, size_type n1, charT c, size_type n2)
static const TestCase
off_size_size_val_test_cases [] = {

#undef TEST
#define TEST(str, off, size, count, val, res, bthrow)                     \
    { Replace (off_size_size_val), __LINE__, off, size, -1, count, val,   \
      str, sizeof str - 1, 0, 0, res, sizeof res - 1, bthrow }

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

// used to exercise
// replace (iterator i1, Iterator i2, charT c, size_type n2)
static const TestCase
size_val_test_cases [] = {

#undef TEST
#define TEST(str, off, size, count, val, res, bthrow)                     \
    { Replace (size_val), __LINE__, off, size, -1, count, val,            \
      str, sizeof str - 1, 0, 0, res, sizeof res - 1, bthrow }

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

static const StringMembers::Test
tests [] = {

#undef TEST
#define TEST(tag, sig) {                                         \
        tag ## _test_cases,                                      \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases,  \
        "replace " sig                                           \
    }

    TEST (off_size_ptr, "(size_type, size_type, const value_type*)"),
    TEST (off_size_str, "(size_type, size_type, basic_string&)"),
    TEST (off_size_ptr_size,
          "(size_type, size_type, const value_type*, size_type)"),
    TEST (off_size_str_off_size,
          "size_type, size_type, basic_string&, size_type, size_type)"),
    TEST (off_size_size_val, "(size_type, size_type, size_type, value_type)"),
    TEST (ptr, "(iterator, iterator, const value_type*)"),
    TEST (str, "(iterator, iterator, basic_string&)"),
    TEST (ptr_size, "(iterator, iterator, const value_type*, size_type)"),
    TEST (size_val, "(iterator, iterator, size_type, value_type)"),
    TEST (range, "(iterator, iterator, InputIterator, InputIterator)")
};

/**************************************************************************/

template <class charT, class Traits>
void test_exceptions (charT, Traits*,
                      const TestCase &cs,
                      const char     *funcall)
{
    typedef std::basic_string <charT, Traits,
                               std::allocator<charT> > TestString;
    typedef typename TestString::iterator StringIter;
    typedef typename TestString::const_iterator ConstStringIter;

    const bool use_iters = (Replace (ptr) <= cs.which);

    static charT wstr [LLEN];
    static charT warg [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (warg, cs.arg, cs.arg_len);

    /* const */ TestString s_str (wstr, cs.str_len);
    const       TestString s_arg (warg, cs.arg_len);

    const int first = use_iters ? cs.off : cs.str_len + 1;
    const int last  = use_iters ? cs.off + cs.size : cs.str_len + 1;

    const StringIter it_first (std::size_t (first) >= s_str.size () ?
                               s_str.end () : s_str.begin () + first);
    const StringIter it_last  (std::size_t (last) >= s_str.size () ?
                               s_str.end () : s_str.begin () + last);

    const charT* const arg_ptr = cs.arg ? warg : s_str.c_str ();
    const TestString&  arg_str = cs.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (cs.val), (charT*)0);

    std::size_t throw_after = 0;

    const std::size_t     size     = s_str.size ();
    const std::size_t     capacity = s_str.capacity ();
    const ConstStringIter begin    = s_str.begin ();

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rwt_free_store* const pst = rwt_get_free_store (0);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // iterate for`n=throw_after' starting at the next call to operator
    // new, forcing each call to throw an exception, until the insertion
    // finally succeeds (i.e, no exception is thrown)
    for ( ; ; ) {

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#endif   // _RWSTD_NO_EXCEPTIONS

        _TRY {
            if (Replace (off_size_ptr) == cs.which)
                s_str.replace (cs.off, cs.size, arg_ptr);

            else if (Replace (off_size_str) == cs.which)
                s_str.replace (cs.off, cs.size, arg_str);

            else if (Replace (off_size_ptr_size) == cs.which)
                s_str.replace (cs.off, cs.size, arg_ptr, cs.size2);

            else if (Replace (off_size_str_off_size) == cs.which)
                s_str.replace (cs.off, cs.size, arg_str, cs.off2, cs.size2);

            else if (Replace (off_size_size_val) == cs.which)
                s_str.replace (cs.off, cs.size, cs.size2, arg_val);

            else if (Replace (ptr) == cs.which)
                s_str.replace (it_first, it_last, arg_ptr);

            else if (Replace (str) == cs.which)
                s_str.replace (it_first, it_last, arg_str);

            else if (Replace (ptr_size) == cs.which)
                s_str.replace (it_first, it_last, arg_ptr, cs.size2);

            else if (Replace (size_val) == cs.which)
                s_str.replace (it_first, it_last, cs.size2, arg_val);

            else if (Replace (range) == cs.which)
                s_str.replace (it_first, it_last, s_arg.begin(), s_arg.end());

            break;
        }
        _CATCH (...) {

#ifndef _RWSTD_NO_EXCEPTIONS

            // verify that an exception thrown during allocation
            // doesn't cause a change in the state of the vector

            rw_assert (s_str.size () == size, 0, cs.line,
                       "line %d: %s: size unexpectedly changed "
                       "from %zu to %zu after an exception",
                       __LINE__, funcall, size, s_str.size ());

            rw_assert (s_str.capacity () == capacity, 0, cs.line,
                       "line %d: %s: capacity unexpectedly "
                       "changed from %zu to %zu after an exception",
                       __LINE__, funcall, capacity, s_str.capacity ());


            rw_assert (s_str.begin () == begin, 0, cs.line,
                       "line %d: %s: begin() unexpectedly "
                       "changed from after an exception by %d",
                       __LINE__, funcall, s_str.begin () - begin);


            // increment to allow this call to operator new to succeed
            // and force the next one to fail, and try to insert again
            ++throw_after;

#endif   // _RWSTD_NO_EXCEPTIONS

        }   // catch
    }   // for

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify that if exceptions are enabled and when capacity changes
    // at least one exception is thrown
    rw_assert (   *pst->throw_at_calls_ [0] == std::size_t (-1)
               || throw_after,
               0, cs.line,
               "line %d: %s: failed to throw an expected exception",
               __LINE__, funcall);

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    _RWSTD_UNUSED (size);
    _RWSTD_UNUSED (capacity);
    _RWSTD_UNUSED (throw_after);

#endif   // _RWSTD_NO_EXCEPTIONS

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    *pst->throw_at_calls_ [0] = std::size_t (-1);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
}

/**************************************************************************/

template <class charT, class Traits, class Iterator>
void test_replace_range (const charT* wstr,
                         const charT* warg,
                         Traits*,
                         const Iterator &it,
                         const TestCase &cs,
                         const char     *funcall)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;

    const char* const itname =
        cs.arg ? type_name (it, (charT*)0) : "basic_string::iterator";

    /* const */ String s_str (wstr, cs.str_len);
    const       String s_arg (warg, cs.arg_len);

    std::size_t off_last   = cs.off + cs.size;
    std::size_t off_first2 = cs.off2;
    std::size_t off_last2  = cs.off2 + cs.size2;

    const StringIter it_first (std::size_t (cs.off) >= s_str.size () ?
                               s_str.end () : s_str.begin () + cs.off);
    const StringIter it_last  (std::size_t (off_last) >= s_str.size () ?
                               s_str.end () : s_str.begin () + off_last);

    if (cs.arg) {
        off_first2 = off_first2 > s_arg.size () ? s_arg.size () : off_first2;
        off_last2  = off_last2  > s_arg.size () ? s_arg.size () : off_last2;

        const charT* const warg_beg = warg + off_first2;
        const charT* const warg_end = warg + off_last2;

        const Iterator first (warg_beg, warg_beg,        warg_end);
        const Iterator last  (warg_end, warg + cs.off2,  warg_end);

        s_str.replace (it_first, it_last, first, last);
    }
    else {
        const StringIter first (std::size_t (cs.off2) >= s_str.size () ?
                                s_str.end () : s_str.begin () + cs.size2);
        const StringIter last (off_last2 > s_str.size () ?
                               s_str.end () : s_str.begin () + off_last2);

        s_str.replace (it_first, it_last, first, last);
    }

    const std::size_t match = rw_match (cs.res, s_str.c_str(), cs.res_len);

    rw_assert (match == cs.res_len, 0, cs.line,
               "line %d. %s expected %{#*s}, got %{/*.*Gs}, "
               "difference at offset %zu for %s",
               __LINE__, funcall, int (cs.res_len), cs.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits>
void test_replace_range (const charT* wstr,
                         const charT* warg,
                         Traits*,
                         const TestCase &cs,
                         const char     *funcall)
{
    if (cs.bthrow)  // this method doesn't throw
        return;

    test_replace_range (wstr, warg, (Traits*)0,
                       InputIter<charT>(0, 0, 0), cs, funcall);

    // there is no need to call test_replace_range
    // for other iterators in this case
    if (0 == cs.arg)
        return;

    test_replace_range (wstr, warg, (Traits*)0,
                       ConstFwdIter<charT>(0, 0, 0), cs, funcall);

    test_replace_range (wstr, warg, (Traits*)0,
                       ConstBidirIter<charT>(0, 0, 0), cs, funcall);

    test_replace_range (wstr, warg, (Traits*)0,
                       ConstRandomAccessIter<charT>(0, 0, 0), cs, funcall);
}

/**************************************************************************/

template <class charT, class Traits>
void test_replace (charT, Traits*,
                   const TestCase &cs,
                   const char     *funcall)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::iterator                StringIter;

    const bool use_iters = (Replace (ptr) <= cs.which);

    static charT wstr [LLEN];
    static charT warg [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (warg, cs.arg, cs.arg_len);

    // special processing for replace_range to exercise all iterators
    if (Replace (range) == cs.which) {
        test_replace_range (wstr, warg, (Traits*)0, cs, funcall);
        return;
    }

    /* const */ TestString s_str (wstr, cs.str_len);
    const       TestString s_arg (warg, cs.arg_len);

    std::size_t res_off = 0;
    int first = use_iters ? cs.off : cs.str_len + 1;
    int last  = use_iters ? cs.off + cs.size : cs.str_len + 1;
    std::size_t size = cs.size2 >= 0 ? cs.size2 : s_str.max_size () + 1;

    StringIter it_first (std::size_t (first) >= s_str.size () ?
                         s_str.end () : s_str.begin () + first);
    StringIter it_last  (std::size_t (last) >= s_str.size () ?
                         s_str.end () : s_str.begin () + last);

    // string function argument
    const charT* const arg_ptr = cs.arg ? warg : s_str.c_str ();
    const TestString&  arg_str = cs.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (cs.val), (charT*)0);

    // address of returned reference
    const TestString* res_ptr = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    if (1 == cs.bthrow && !use_iters)
        expected = exp_exceptions [1];
    if (2 == cs.bthrow && Replace (off_size_str_off_size) == cs.which)
        expected = exp_exceptions [1];
    if (3 == cs.bthrow && !use_iters)
        expected = exp_exceptions [2];

    const char* caught = 0;

    try {

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (cs.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    switch (cs.which)
    {
    case Replace (off_size_ptr): {
        res_ptr = &s_str.replace (cs.off, cs.size, arg_ptr);
        break;
    }

    case Replace (off_size_str): {
        res_ptr = &s_str.replace (cs.off, cs.size, arg_str);
        break;
    }

    case Replace (off_size_ptr_size): {
        res_ptr = &s_str.replace (cs.off, cs.size, arg_ptr, size);
        break;
    }

    case Replace (off_size_str_off_size): {
        res_ptr = &s_str.replace (cs.off, cs.size, arg_str, cs.off2, size);
        break;
    }

    case Replace (off_size_size_val): {
        res_ptr = &s_str.replace (cs.off, cs.size, size, arg_val);
        break;
    }

    case Replace (ptr): {
        res_ptr = &s_str.replace (it_first, it_last, arg_ptr);
        break;
    }

    case Replace (str): {
        res_ptr = &s_str.replace (it_first, it_last, arg_str);
        break;
    }

    case Replace (ptr_size): {
        res_ptr = &s_str.replace (it_first, it_last, arg_ptr, size);
        break;
    }

    case Replace (size_val): {
        res_ptr = &s_str.replace (it_first, it_last, size, arg_val);
        break;
    }

    default:
        RW_ASSERT ("test logic error: unknown replace overload");
        return;
    }

    res_off = res_ptr - &s_str;

    // verify the returned value
    rw_assert (0 == res_off, 0, cs.line,
               "line %d. %s returned invalid reference, offset is %zu%",
               __LINE__, funcall, res_off);

    // verfiy that strings length are equal
    rw_assert (cs.res_len == s_str.size (), 0, cs.line,
               "line %d. %s expected %{#*s} with length %zu, got %{/*.*Gs} "
               "with length %zu", __LINE__, funcall, int (cs.res_len),
               cs.res, cs.res_len, int (sizeof (charT)), int (s_str.size ()),
               s_str.c_str (), s_str.size ());

    // verfiy that replace results match expected result
    const std::size_t match = rw_match (cs.res, s_str.c_str(), cs.res_len);

    rw_assert (match == cs.res_len, 0, cs.line,
               "line %d. %s expected %{#*s}, got %{/*.*Gs}, "
               "difference at offset %zu",
               __LINE__, funcall, int (cs.res_len), cs.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match);

#ifndef _RWSTD_NO_EXCEPTIONS

    }
    catch (std::out_of_range) {
        caught = exp_exceptions[1];
    }
    catch (std::length_error) {
        caught = exp_exceptions[2];
    }
    catch (...) {
        caught = exp_exceptions[0];
    }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)
    _RWSTD_UNUSED (should_throw);
#endif   // _RWSTD_NO_EXCEPTIONS

    rw_assert (caught == expected, 0, cs.line,
               "line %d. %s %{?}expected %s, caught %s"
               "%{:}unexpectedly caught %s%{;}", __LINE__,
               funcall, 0 != expected, expected, caught, caught);
}

/**************************************************************************/

static void
test_replace (const MemFun *pfid, const TestCase& cs, bool exc_safety_test)
{
    // format the description of the function call including
    // the values of arguments for use in diagnostics
    char* const funcall =
        StringMembers::format (pfid->cid_, pfid->tid_,
                               StringMembers::DefaultAllocator,
                               cs);

#undef TEST
#define TEST(charT, Traits)                                             \
    exc_safety_test ?                                                   \
        test_exceptions (charT (), (Traits*)0, cs, funcall)             \
      : test_replace (charT (), (Traits*)0, cs, funcall)

    if (StringMembers::DefaultTraits == pfid->tid_) {
        if (StringMembers::Char == pfid->cid_)
            TEST (char, std::char_traits<char>);

#ifndef _RWSTD_NO_WCHAR_T
    else
        TEST (wchar_t, std::char_traits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T

    }
    else {
       if (StringMembers::Char == pfid->cid_)
           TEST (char, UserTraits<char>);

#ifndef _RWSTD_NO_WCHAR_T
       else if (StringMembers::WChar == pfid->cid_)
           TEST (wchar_t, UserTraits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T

       else
           TEST (UserChar, UserTraits<UserChar>);
    }

    std::free (funcall);
}

/**************************************************************************/

static void
test_replace (const MemFun *pfid, const Test& test)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::%s",
             pfid->cname_, pfid->tname_, pfid->aname_, test.funsig);

    if (StringMembers::opt_no_exception_safety)
        rw_note (0, 0, 0,
                 "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
                 "%s exception safety test disabled",
                 pfid->cname_, pfid->tname_, pfid->aname_, test.funsig);

#ifdef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    else
        rw_warn (0, 0, __LINE__,
                 "%s exception safety test: no replacable new and delete",
                 test.funsig);

#endif  //_RWSTD_NO_REPLACEABLE_NEW_DELETE

    for (std::size_t i = 0; i != test.case_count; ++i) {

        if (!rw_enabled (test.cases [i].line)) {
            rw_note (0, 0, __LINE__,
                     "test on line %d disabled", test.cases [i].line);
            continue;
        }

        // do not exercise exceptions if they were disabled
        if (   0 != StringMembers::opt_no_exceptions
            && 0 != test.cases [i].bthrow)
            continue;

        // do not exercise exception safety if they were disabled
        if (    0 != StringMembers::opt_no_exception_safety
            && -1 == test.cases [i].bthrow)
            continue;

        test_replace (pfid, test.cases [i], -1 == test.cases [i].bthrow);
    }
}


/**************************************************************************/

static void
run_test (const MemFun *pfid)
{
    if (   StringMembers::UserTraits == pfid->tid_
        && StringMembers::opt_no_user_traits) {
        rw_note (1 < StringMembers::opt_no_user_traits++, 0, 0,
                 "user defined traits test disabled");
    }
    else if (   StringMembers::DefaultTraits == pfid->tid_
             && StringMembers::opt_no_char_traits) {
        rw_note (1 < StringMembers::opt_no_char_traits++, 0, 0,
                 "char_traits test disabled");
    }
    else {

        if (StringMembers::opt_no_exceptions)
            rw_note (1 < StringMembers::opt_no_exceptions++, 0, 0,
                     "string::replace exceptions tests disabled");

        static const std::size_t ntests = sizeof tests / sizeof *tests;

        for (std::size_t i = 0; i < ntests; i++) {

            int tmp = tests [i].cases [0].which - StringMembers::replace_first;

            if (Disabled (tmp))
                rw_note (0, 0, 0,
                         "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
                         "%s test disabled", pfid->cname_, pfid->tname_,
                         pfid->aname_, tests [i].funsig);
            else
                test_replace (pfid, tests [i]);
        }
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    if ('\0' == LSTR [0]) {
        // initialize LSTR
        for (std::size_t i = 0; i != sizeof LSTR - 1; ++i)
            LSTR [i] = 'x';
    }

    if (rw_enabled ("char")) {

        MemFun fid (StringMembers::Char, "char",
                    StringMembers::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = StringMembers::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::%s char tests disabled", method_name);

    if (rw_enabled ("wchar_t")) {

        MemFun fid (StringMembers::WChar, "wchar_t",
                    StringMembers::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = StringMembers::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::%s wchar tests disabled", method_name);

    if (StringMembers::opt_no_user_char) {
        rw_note (0, 0, 0, "user defined chars test disabled");
    }
    else {
        MemFun fid (StringMembers::UChar, "UserChar",
                    StringMembers::UserTraits, 0);
        fid.tname_ = "UserTraits";
        run_test (&fid);
    }

    // silence a bogus EDG eccp remark #550-D:
    // variable "exp_exceptions" was set but never used
    _RWSTD_UNUSED (exp_exceptions);

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.replace",
                    0 /* no comment */, run_test,
                    "|-no-char_traits# "
                    "|-no-user_traits# "
                    "|-no-user_char# "
                    "|-no-exceptions# "
                    "|-no-exception-safety# "

                    "|-no-replace-off-size-ptr# "
                    "|-no-replace-off-size-str# "
                    "|-no-replace-off_size-ptr-size# "
                    "|-no-replace-off-size-str-off-size# "
                    "|-no-replace-off-size-size-val# "
                    "|-no-replace-ptr# "
                    "|-no-replace-str# "
                    "|-no-replace-ptr-size# "
                    "|-no-replace-size-val# "
                    "|-no-replace-range#",

                    &StringMembers::opt_no_char_traits,
                    &StringMembers::opt_no_user_traits,
                    &StringMembers::opt_no_user_char,
                    &StringMembers::opt_no_exceptions,
                    &StringMembers::opt_no_exception_safety,

                    &Disabled (Replace (off_size_ptr)),
                    &Disabled (Replace (off_size_str)),
                    &Disabled (Replace (off_size_ptr_size)),
                    &Disabled (Replace (off_size_str_off_size)),
                    &Disabled (Replace (off_size_size_val)),
                    &Disabled (Replace (ptr)),
                    &Disabled (Replace (str)),
                    &Disabled (Replace (ptr_size)),
                    &Disabled (Replace (size_val)),
                    &Disabled (Replace (range)),

                    // sentinel
                    (void*)0);
}
