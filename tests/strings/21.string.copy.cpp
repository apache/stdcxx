/***************************************************************************
 *
 * 21.string.copy.cpp - string test exercising [lib.string::copy]
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
#include <cstddef>      // size_t
#include <stdexcept>    // for out_of_range

#include <21.strings.h> // for StringMembers
#include <driver.h>     // for rw_assert()
#include <rw_char.h>    // for rw_widen()

/**************************************************************************/

// for convenience and brevity

#define NPOS          _RWSTD_SIZE_MAX
#define LSTR          StringMembers::long_string
#define LLEN          StringMembers::long_string_len
#define Copy(which)   StringMembers::copy_ ## which

typedef StringMembers::OverloadId OverloadId;
typedef StringMembers::TestCase   TestCase;
typedef StringMembers::Test       Test;
typedef StringMembers::Function   MemFun;

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// copy (value_type*, size_type)
static const TestCase
ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, size, res) {                \
        __LINE__, -1, size, -1, -1, -1,       \
        str, sizeof str - 1, 0, 0,            \
        res, sizeof res - 1, 0                \
    }

    //    +--------------------------------------- controlled sequence
    //    |                 +--------------------- copy() n argument
    //    |                 |   +----------------- expected result sequence
    //    |                 |   |     
    //    V                 V   V             
    TEST ("ab",             2,  "ab"),       

    TEST ("",               0,  ""),         
    TEST ("",              10,  ""),          

    TEST ("\0",             1,  "\0"),       
    TEST ("\0\0",           2,  "\0\0"),    

    TEST ("abc",            0,  ""), 
    TEST ("abc",            1,  "a"),                 
    TEST ("abc",            2,  "ab"),       
    TEST ("abc",            3,  "abc"), 
    TEST ("abc",            5,  "abc"),

    TEST ("a\0b\0\0c",     10,  "a\0b\0\0c"),  
    TEST ("\0ab\0\0\0c\0",  1,  "\0"),
    TEST ("\0ab\0\0\0c\0",  8,  "\0ab\0\0\0c\0"),
    TEST ("\0ab\0\0\0c\0",  5,  "\0ab\0\0"),   
    TEST ("\0\0ab\0\0c\0",  6,  "\0\0ab\0\0"), 

    TEST ("last",           4, "last")       
};

/**************************************************************************/

// exercises:
// copy (value_type*, size_type, size_type)
static const TestCase
ptr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, size, off, res, bthrow)  {    \
        __LINE__, off, size, -1, -1, -1,        \
        str, sizeof str - 1, 0, 0,              \
        res, sizeof res - 1, bthrow             \
    }

    //    +------------------------------------------ controlled sequence
    //    |                 +------------------------ copy() n argument
    //    |                 |   +-------------------- copy() pos argument
    //    |                 |   |  +----------------- expected result sequence
    //    |                 |   |  |             +--- exception info:
    //    |                 |   |  |             |      0 - no exception    
    //    |                 |   |  |             |      1 - out_of_range        
    //    V                 V   V  V             V  
    TEST ("ab",             2,  0, "ab",         0),

    TEST ("",               0,  0, "",           0),
    TEST ("",              10,  0, "",           0),

    TEST ("\0",             1,  0, "\0",         0),
    TEST ("\0\0",           1,  1, "\0",         0),
    TEST ("\0\0",           2,  0, "\0\0",       0),

    TEST ("abc",            1,  0, "a",          0),
    TEST ("abc",            1,  1, "b",          0),
    TEST ("abc",            1,  2, "c",          0),

    TEST ("abc",            0,  0, "",           0),
    TEST ("abc",            2,  0, "ab",         0),
    TEST ("abc",            2,  1, "bc",         0),
    TEST ("abc",            3,  0, "abc",        0),
    TEST ("abc",           10,  1, "bc",         0),
    TEST ("abc",            3,  2, "c",          0),

    TEST ("a\0b\0\0c",     10,  1, "\0b\0\0c",   0),
    TEST ("a\0b\0\0c",     10,  0, "a\0b\0\0c",  0),
    TEST ("a\0b\0\0c",      1,  1, "\0",         0),

    TEST ("\0ab\0\0\0c\0", 10,  1, "ab\0\0\0c\0",0),
    TEST ("\0ab\0\0\0c\0",  5,  0, "\0ab\0\0",   0),
    TEST ("\0ab\0\0\0c\0",  3,  3, "\0\0\0",     0),

    TEST ("\0\0ab\0\0c\0",  6,  0, "\0\0ab\0\0", 0),
    TEST ("\0\0ab\0\0c\0",  4,  1, "\0ab\0",     0),

    TEST (LSTR,      LLEN - 1,         0, LSTR,  0),
    TEST (LSTR,             2,         1, "xx",  0),
    TEST (LSTR,      LLEN - 1, LLEN -  2, "x",   0),

    TEST ("\0",     0,          2, "",           1),
    TEST ("a",      0,         10, "",           1),
    TEST (LSTR,     0,  LLEN + 10, "",           1),

    TEST ("last",           4,  0, "last",       0)
};

/**************************************************************************/

template <class charT, class Traits>
void test_copy (charT, Traits*,                 
                OverloadId      which,
                const TestCase &tcase)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;

    static charT wstr [LLEN];

    // construct strings
    rw_widen (wstr, tcase.str, tcase.str_len);
    const TestString str (wstr, tcase.str_len);

    const std::size_t min_len =
        tcase.str_len < std::size_t (tcase.size) ? tcase.str_len : tcase.size;

    std::size_t res = 0;

    // create destination array and initialize it with garbage
    charT* const s_res = new charT [min_len + 1];

    char cgb = '@';
    charT wcgb = make_char (cgb, (charT*)0);
    Traits::assign (s_res, min_len + 1, wcgb);

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected?
    const char* const expected = tcase.bthrow ? exceptions [1] : 0;
    const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    try {
        switch (which) {
        case Copy (ptr_size): {
            res = str.copy (s_res, tcase.size);
            break;
        }

        case Copy (ptr_size_size): {
            res = str.copy (s_res, tcase.size, tcase.off);
            break;
        }
        default:
            RW_ASSERT (!"logic error: unknown copy overload");
            return;
        }

        // verify the returned value
        rw_assert (res == tcase.nres, 0, tcase.line,
                   "line %d. %{$FUNCALL} == %zu, got %zu", 
                   __LINE__, tcase.nres, res);

        const std::size_t match = rw_match (tcase.res, s_res, tcase.nres);
        bool success = match == tcase.nres;

        rw_assert (success, 0, tcase.line,
                   "line %d. %{$FUNCALL} expected %{#*s}, "
                   "got %{/*.*Gs}, differ at pos %zu",
                   __LINE__, int (tcase.nres), tcase.res, 
                   int (sizeof (charT)), int (res), s_res, match);

        success = 1 == rw_match (&cgb, &s_res [min_len], 1);
        rw_assert (success, 0, tcase.line,
                   "line %d. %{$FUNCALL} detected writing past the end of "
                   "the provided buffer", __LINE__);
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    catch (const std::out_of_range &ex) {
        caught = exceptions [1];
        rw_assert (caught == expected, 0, tcase.line,
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

    if (caught) {
        // verify that an exception thrown during allocation
        // didn't cause a change in the state of the object
        str_state.assert_equal (rw_get_string_state (str),
                                __LINE__, tcase.line, caught);
    }
    else if (-1 != tcase.bthrow) {
        rw_assert (caught == expected, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                   "%{:}unexpectedly caught %s%{;}",
                   __LINE__, 0 != expected, expected, caught, caught);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    delete[] s_res;
}

/**************************************************************************/

DEFINE_TEST_DISPATCH (test_copy);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::copy_ ## tag,                            \
        tag ## _test_cases,                                     \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (ptr_size),
        TEST (ptr_size_size)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return StringMembers::run_test (argc, argv, __FILE__,
                                    "lib.string.copy",
                                    test_copy, tests, test_count);
}
