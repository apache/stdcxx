/***************************************************************************
 *
 * access.cpp - test exercising [lib.string.access]
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#include <string>           // for string
#include <cstdlib>          // for free(), size_t
#include <stdexcept>        // for out_of_range

#include <21.strings.h>     // for StringMembers
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()

/**************************************************************************/

// for convenience and brevity
#define NPOS                      _RWSTD_SIZE_MAX
#define At(which)                 StringMembers::at_ ## which
#define OpIndex(which)            StringMembers::op_index_ ## which

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
// operator[] (size_type)
static const TestCase 
op_index_size_test_cases [] = {

#undef TEST
#define TEST(str, off, res) {                    \
        __LINE__, off, -1, -1, -1, -1,           \
        str, sizeof str - 1,                     \
        0, 0, 0, res, 0                          \
    }

    //    +--------------------------------- controlled sequence
    //    |               +----------------- index 
    //    |               |   +------------- expected result 
    //    |               |   |     
    //    V               V   V     
    TEST ("a",            0, 'a' ), 

    TEST ("\0",           0, '\0'),

    TEST ("abc",          0, 'a' ),  
    TEST ("abc",          1, 'b' ),  
    TEST ("abc",          2, 'c' ),  

    TEST ("\0ab\0\0c",    0, '\0'), 

    TEST ("a\0b\0\0c",    0, 'a' ),  
    TEST ("a\0b\0\0c",    1, '\0'),
    TEST ("a\0b\0\0c",    2, 'b' ),  
    TEST ("a\0b\0\0c",    3, '\0'), 
    TEST ("a\0b\0\0c",    4, '\0'),
    TEST ("a\0b\0\0c",    5, 'c' ),   

    TEST ("a\0bc\0\0",    5, '\0'), 

    TEST ("x@4096",       0, 'x' ),  
    TEST ("x@4096",    2048, 'x' ), 
    TEST ("x@4096",    4095, 'x' ), 

    TEST ("last",         3, 't' )  
};

/**************************************************************************/

// used to exercise
// operator[] (size_type) const 
static const TestCase 
op_index_size_const_test_cases [] = {

#undef TEST
#define TEST(str, off, res) {                    \
        __LINE__, off, -1, -1, -1, -1,           \
        str, sizeof str - 1,                     \
        0, 0, 0, res, 0                          \
    }

    //    +--------------------------------- controlled sequence
    //    |               +----------------- index 
    //    |               |   +------------- expected result 
    //    |               |   |     
    //    V               V   V     
    TEST ("a",            0, 'a' ),  
    TEST ("a",            1, NPOS), 

    TEST ("",             0, NPOS), 

    TEST ("\0",           0, '\0'),
    TEST ("\0",           1, NPOS), 

    TEST ("abc",          0, 'a' ),  
    TEST ("abc",          1, 'b' ),  
    TEST ("abc",          2, 'c' ),  
    TEST ("abc",          3, NPOS), 

    TEST ("\0ab\0\0c",    0, '\0'), 

    TEST ("a\0b\0\0c",    0, 'a' ),  
    TEST ("a\0b\0\0c",    1, '\0'),
    TEST ("a\0b\0\0c",    2, 'b' ),  
    TEST ("a\0b\0\0c",    3, '\0'), 
    TEST ("a\0b\0\0c",    4, '\0'),
    TEST ("a\0b\0\0c",    5, 'c' ),  
    TEST ("a\0b\0\0c",    6, NPOS), 

    TEST ("a\0bc\0\0",    5, '\0'), 

    TEST ("x@4096",       0, 'x' ),  
    TEST ("x@4096",    2048, 'x' ), 
    TEST ("x@4096",    4095, 'x' ), 
    TEST ("x@4096",    4096, NPOS),

    TEST ("last",         3, 't' )  
};

/**************************************************************************/

// used to exercise
// at (size_type)
// at (size_type) const 
static const TestCase 
at_size_test_cases [] = {

#define at_size_const_test_cases    at_size_test_cases

#undef TEST
#define TEST(str, off, res, bthrow) {           \
        __LINE__, off, -1, -1, -1, -1,          \
        str, sizeof str - 1,                    \
        0, 0, 0, res, bthrow                    \
    }

    //    +--------------------------------- controlled sequence
    //    |               +----------------- index 
    //    |               |  +-------------- expected result 
    //    |               |  |     +-------- exception info 
    //    |               |  |     |              0 - no exception
    //    |               |  |     |              1 - out_of_range
    //    |               |  |     |
    //    V               V  V     V
    TEST ("a",            0, 'a',  0),
    TEST ("a",            1, NPOS, 1),

    TEST ("",             0, NPOS, 1),

    TEST ("\0",           0, '\0', 0),
    TEST ("\0",           1, NPOS, 1),

    TEST ("abc",          0, 'a',  0),
    TEST ("abc",          1, 'b',  0),
    TEST ("abc",          2, 'c',  0),
    TEST ("abc",          3, NPOS, 1),

    TEST ("\0ab\0\0c",    0, '\0', 0),

    TEST ("a\0b\0\0c",    0, 'a',  0),
    TEST ("a\0b\0\0c",    1, '\0', 0),
    TEST ("a\0b\0\0c",    2, 'b',  0),
    TEST ("a\0b\0\0c",    3, '\0', 0),
    TEST ("a\0b\0\0c",    4, '\0', 0),
    TEST ("a\0b\0\0c",    5, 'c',  0),
    TEST ("a\0b\0\0c",    6, NPOS, 1),

    TEST ("a\0bc\0\0",    5, '\0', 0),

    TEST ("x@4096",       0, 'x',  0),
    TEST ("x@4096",    2048, 'x',  0),
    TEST ("x@4096",    4095, 'x',  0),
    TEST ("x@4096",    4096, NPOS, 1),

    TEST ("last",         3, 't',  0)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_access (charT, Traits*, Allocator*, 
                  OverloadId      which,
                  const TestCase &cs)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::reference                   StrRef;
    typedef typename String::const_reference             StrConstRef;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    charT* wstr = rw_expand (wstr_buf, cs.str, cs.str_len, &str_len);

    // construct the string object 
    String  s_str (wstr, str_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    wstr = 0;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (s_str));

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 1 == cs.bthrow ? exceptions[1] : 0;
    const char* caught = 0;

#endif   // _RWSTD_NO_EXCEPTIONS

    try {

        bool success = false;
        charT cres;
        char exp_res = NPOS != cs.nres 
            ? (char) cs.nres 
          : typename std::string::value_type();

        switch (which) {
        case OpIndex (size): {
            StrRef res = s_str [cs.off];

            success = 1 == rw_match (&exp_res, &res, 1);
            cres = res;
            break;
        }
        case OpIndex (size_const): {
            StrConstRef res = const_cast<const String&>(s_str) [cs.off];

            success = 1 == rw_match (&exp_res, &res, 1);
            cres = res;
            break;
        }
        case At (size): {
            StrRef res = s_str.at (cs.off);

            success = 1 == rw_match (&exp_res, &res, 1);
            cres = res;
            break;
        }
        case At (size_const): {
            StrConstRef res = const_cast<const String&>(s_str).at (cs.off);

            success = 1 == rw_match (&exp_res, &res, 1);
            cres = res;
            break;
        }
        default:
            RW_ASSERT ("test logic error: unknown access overload");
            return;
        }

        rw_assert (success, 0, cs.line,
                   "line %d. %{$FUNCALL} == %{#c}, got %{#c}",
                   __LINE__, cs.nres, cres);
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    catch (const std::out_of_range &ex) {
        caught = exceptions [1];
        rw_assert (caught == expected, 0, cs.line,
                   "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                   "unexpectedly%{;} caught std::%s(%#s)",
                   __LINE__, 0 != expected, expected, caught, ex.what ());
    }
    catch (const std::exception &ex) {
        caught = exceptions [4];
        rw_assert (0, 0, cs.line,
                   "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                   "unexpectedly%{;} caught std::%s(%#s)",
                   __LINE__, 0 != expected, expected, caught, ex.what ());
    }
    catch (...) {
        caught = exceptions [0];
        rw_assert (0, 0, cs.line,
                   "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                   "unexpectedly%{;} caught %s",
                   __LINE__, 0 != expected, expected, caught);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    if (caught) {
        // verify that an exception thrown during allocation
        // didn't cause a change in the state of the object
        str_state.assert_equal (rw_get_string_state (s_str),
                                __LINE__, cs.line, caught);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_access);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::tag, tag ## _test_cases,                 \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (op_index_size),
        TEST (op_index_size_const),
        TEST (at_size),
        TEST (at_size_const)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return StringMembers::run_test (argc, argv, __FILE__,
                                    "lib.string.access",
                                    test_access, tests, test_count);
}

