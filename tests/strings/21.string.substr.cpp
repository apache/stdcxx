/***************************************************************************
 *
 * 21.string.substr.cpp - string test exercising [lib.string::substr]
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
#include <cstddef>      // for size_t
#include <stdexcept>    // for out_of_range

#include <21.strings.h> // for StringMembers
#include <driver.h>     // for rw_test()
#include <rw_char.h>    // for rw_widen()

/**************************************************************************/

// for convenience and brevity
#define LSTR             StringMembers::long_string
#define LLEN             StringMembers::long_string_len
#define Substr(which)    StringMembers::substr_ ## which

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
// substr ()
static const TestCase 
void_test_cases [] = {

#undef TEST
#define TEST(str, res)                                                   \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, 0, 0,           \
      res, sizeof res - 1, 0 }

    //    +------------------------------------- controlled sequence
    //    |                   +----------------- expected result sequence
    //    |                   |                    
    //    V                   V             
    TEST ("abc",              "abc"),  

    TEST ("",                 ""),  
    TEST ("\0",               "\0"),  
    TEST ("a\0",              "a\0"), 
    TEST ("\0a",              "\0a"), 

    TEST ("a\0\0bcd\0",       "a\0\0bcd\0"),
    TEST ("\0\0abcd\0",       "\0\0abcd\0"),
    TEST ("\0\0ab\0\0",       "\0\0ab\0\0"),

    TEST (LSTR,               LSTR),

    TEST ("last",             "last")
};


/**************************************************************************/

// used to exercise 
// substr (size_type)
static const TestCase 
size_test_cases [] = {

#undef TEST
#define TEST(str, off, res, bthrow)                                         \
    { __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1, 0, 0,             \
      res, sizeof res - 1, bthrow }

    //    +--------------------------------------- controlled sequence
    //    |                 +--------------------- substr() off argument
    //    |                 |   +----------------- expected result sequence
    //    |                 |   |             +--- exception info:
    //    |                 |   |             |      0 - no exception    
    //    |                 |   |             |      1 - out_of_range        
    //    V                 V   V             V  
    TEST ("abc",            0,  "abc",        0),

    TEST ("",               0,  "",           0),  
    TEST ("\0",             0,  "\0",         0), 
    TEST ("\0",             1,  "",           0), 
    TEST ("a\0",            0,  "a\0",        0), 
    TEST ("a\0",            1,  "\0",         0), 
    TEST ("\0a",            0,  "\0a",        0), 
    TEST ("\0a",            1,  "a",          0), 

    TEST ("a\0\0bcd\0",     0,  "a\0\0bcd\0", 0),
    TEST ("a\0\0bcd\0",     1,  "\0\0bcd\0",  0),
    TEST ("a\0\0bcd\0",     6,  "\0",         0),
    TEST ("a\0\0bcd\0",     7,  "",           0),
    TEST ("\0\0abcd\0",     0,  "\0\0abcd\0", 0),
    TEST ("\0\0abcd\0",     1,  "\0abcd\0",   0),
    TEST ("\0\0abcd\0",     2,  "abcd\0",     0),
    TEST ("\0\0ab\0\0",     0,  "\0\0ab\0\0", 0),
    TEST ("\0\0ab\0\0",     1,  "\0ab\0\0",   0),
    TEST ("\0\0ab\0\0",     4,  "\0\0",       0),
    TEST ("\0\0ab\0\0",     6,  "",           0),

    TEST (LSTR,             0,  LSTR,         0),
    TEST (LSTR,      LLEN - 3,  "xx",         0),
    TEST (LSTR,      LLEN - 9,  "xxxxxxxx",   0),
    TEST (LSTR,      LLEN - 1,  "",           0),

    TEST ("\0",             2,  "",           1),
    TEST ("a",             10,  "",           1),
    TEST (LSTR,     LLEN + 10,  "",           1),

    TEST ("last",           0,  "last",       0)
};

/**************************************************************************/

// used to exercise 
// substr (size_type, size_type)
static const TestCase 
size_size_test_cases [] = {

#undef TEST
#define TEST(str, off, size, res, bthrow)                                    \
    { __LINE__, off, size, -1, -1, -1, str, sizeof str - 1, 0, 0,            \
      res, sizeof res - 1, bthrow }

    //    +------------------------------------------ controlled sequence
    //    |                 +------------------------ substr() off argument
    //    |                 |   +-------------------- substr() n argument
    //    |                 |   |  +----------------- expected result sequence
    //    |                 |   |  |             +--- exception info:
    //    |                 |   |  |             |      0 - no exception    
    //    |                 |   |  |             |      1 - out_of_range        
    //    V                 V   V  V             V  
    TEST ("abc",            0,  3, "abc",        0),

    TEST ("",               0,  0, "",           0),  
    TEST ("\0",             0,  1, "\0",         0), 
    TEST ("\0",             0,  0, "",           0),
    TEST ("\0",             1,  1, "",           0), 
    TEST ("a\0",            0,  2, "a\0",        0), 
    TEST ("a\0",            1,  2, "\0",         0), 
    TEST ("a\0",            0,  1, "a",          0), 
    TEST ("a\0",            1,  1, "\0",         0), 
    TEST ("a\0",            1,  0, "",           0), 
    TEST ("\0a",            0,  2, "\0a",        0), 
    TEST ("\0a",            0,  1, "\0",         0),
    TEST ("\0a",            1,  2, "a",          0), 

    TEST ("a\0\0bcd\0",     0,  7, "a\0\0bcd\0", 0),
    TEST ("a\0\0bcd\0",     1,  7, "\0\0bcd\0",  0),
    TEST ("a\0\0bcd\0",     0,  3, "a\0\0",      0),
    TEST ("a\0\0bcd\0",     1,  2, "\0\0",       0),
    TEST ("a\0\0bcd\0",     6,  7, "\0",         0),
    TEST ("a\0\0bcd\0",     7,  7, "",           0),
    TEST ("\0\0abcd\0",     0,  7, "\0\0abcd\0", 0),
    TEST ("\0\0abcd\0",     1,  7, "\0abcd\0",   0),
    TEST ("\0\0abcd\0",     0,  3, "\0\0a",      0),
    TEST ("\0\0abcd\0",     1,  1, "\0",         0),
    TEST ("\0\0abcd\0",     6,  3, "\0",         0),
    TEST ("\0\0abcd\0",     1,  6, "\0abcd\0",   0),
    TEST ("\0\0abcd\0",     0,  0, "",           0),
    TEST ("\0\0abcd\0",     2,  7, "abcd\0",     0),
    TEST ("\0\0ab\0\0",     0,  6, "\0\0ab\0\0", 0),
    TEST ("\0\0ab\0\0",     1,  6, "\0ab\0\0",   0),
    TEST ("\0\0ab\0\0",     0,  2, "\0\0",       0),
    TEST ("\0\0ab\0\0",     4,  6, "\0\0",       0),
    TEST ("\0\0ab\0\0",     2,  3, "ab\0",       0),
    TEST ("\0\0ab\0\0",     6,  6, "",           0),

    TEST (LSTR,             0, LLEN - 1, LSTR,   0),
    TEST (LSTR,      LLEN - 2, LLEN - 1, "x",    0),
    TEST (LSTR,      LLEN - 9,  2, "xx",         0),
    TEST (LSTR,      LLEN - 4,  5, "xxx",        0),
    TEST (LSTR,      LLEN - 9,  8, "xxxxxxxx",   0),
    TEST (LSTR,      LLEN - 1,  1, "",           0),

    TEST ("\0",             2,  0, "",           1),
    TEST ("a",             10,  0, "",           1),
    TEST (LSTR,     LLEN + 10,  0, "",           1),

    TEST ("last",           0,  4, "last",       0)
};

/**************************************************************************/

template <class charT, class Traits>
void test_substr (charT, Traits*,
                  OverloadId      which,
                  const TestCase &cs)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::const_iterator          ConstStringIter;

    static charT wstr [LLEN];

    // construct string
    rw_widen (wstr, cs.str, cs.str_len);
    const TestString str (wstr, cs.str_len);

    TestString s_res;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected?
    const char* const expected = cs.bthrow ? exceptions [1] : 0;
    const char* caught = 0;

#endif   // _RWSTD_NO_EXCEPTIONS

    try {
        switch (which) {
            case Substr (void):
                s_res = str.substr ();
                break;

            case Substr (size):
                s_res = str.substr (cs.off);
                break;

            case Substr (size_size):
                s_res = str.substr (cs.off, cs.size);
                break;

            default:
                RW_ASSERT (!"logic error: unknown substr overload");
                return;
        }

        // verfiy that strings length are equal
        rw_assert (cs.nres == s_res.size (), 0, cs.line,
                   "line %d. %{$FUNCALL} expected %{#*s} with length "
                   "%zu, got %{/*.*Gs} with length %zu",
                   __LINE__, int (cs.nres), cs.res, 
                   cs.nres, int (sizeof (charT)), 
                   int (s_res.size ()), s_res.c_str (), s_res.size ());

        if (cs.nres == s_res.size ()) {
            // if the result length matches the expected length
            // (and only then), also verify that the resulted
            // string matches the expected result
            const std::size_t match =
                rw_match (cs.res, s_res.c_str(), cs.nres);

            rw_assert (match == cs.nres, 0, cs.line,
                       "line %d. %{$FUNCALL} expected %{#*s}, "
                       "got %{/*.*Gs}, difference at offset %zu",
                       __LINE__, int (cs.nres), cs.res,
                       int (sizeof (charT)), int (s_res.size ()),
                       s_res.c_str (), match);
        }
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

    if (caught) {
        // verify that an exception thrown during allocation
        // didn't cause a change in the state of the object
        str_state.assert_equal (rw_get_string_state (str),
                                __LINE__, cs.line, caught);
    }

#endif   // _RWSTD_NO_EXCEPTIONS
}

/**************************************************************************/

DEFINE_TEST_DISPATCH (test_substr);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::substr_ ## tag, tag ## _test_cases,      \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (void),
        TEST (size),
        TEST (size_size)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return StringMembers::run_test (argc, argv, __FILE__,
                                    "lib.string.substr",
                                    test_substr, tests, test_count);
}
