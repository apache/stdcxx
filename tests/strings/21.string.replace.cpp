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

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
   // disabled for compilers such as IBM VAC++ or MSVC
   // that can't reliably replace the operators
#  include <rw_new.h>
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

/**************************************************************************/

struct MemFun
{
    enum charT  { Char, WChar, UChar };
    enum Traits { DefaultTraits, UserTraits };

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

typedef enum ReplaceTags {

    // replace (size_type pos1, size_type n1, const charT* p)
    replace_off_size_ptr           =  1,   
    // replace (size_type pos1, size_type n1, basic_string& s)
    replace_off_size_str           =  2,
    // replace (size_type pos1, size_type n1, charT* p, size_type n2)
    replace_off_size_ptr_size      =  3,
    // replace (pos1, n1, basic_string& s, size_type pos2, size_type n2)
    replace_off_size_str_off_size  =  4,
    // replace (size_type pos1, size_type n1, size_type n, charT c)
    replace_off_size_size_val      =  5,
    // replace (iterator i1, iterator i2, const charT* p)
    replace_ptr                    =  6,
    // replace (iterator i1, iterator i2, basic_string& s)
    replace_str                    =  7,
    // replace (iterator i1, iterator i2, charT* p, size_type n2)
    replace_ptr_size               =  8,
    // replace (iterator i1, iterator i2, size_type n, charT c)
    replace_size_val               =  9,
    // replace (iterator i1, iterator i2, InputIterator j1, InputIterator j2)
    replace_range                  = 10

} RTags;

/**************************************************************************/

struct TestCase
{
    int  line;

    int  pos1;
    int  num;
    int  pos2;
    int  count;
    int  ch;

    const char* str;
    std::size_t str_len;

    const char* src;
    std::size_t src_len;

    const char* res;
    std::size_t res_len;

    int bthrow;

};

/**************************************************************************/

static int rw_opt_no_char_traits;              // for --no-char_traits
static int rw_opt_no_user_traits;              // for --no-user_traits

static int rw_opt_no_user_chars;               // for --no-user_chars
static int rw_opt_no_exceptions;               // for --no-exceptions
static int rw_opt_no_exception_safety;         // for --no-exception-safety

// for --no-replace-off-size-ptr
static int rw_opt_no_replace_off_size_ptr;     
// for --no-replace-off-size-str
static int rw_opt_no_replace_off_size_str;      
// for --no-replace-off-size-ptr-size
static int rw_opt_no_replace_off_size_ptr_size;      
// for --no-replace-off-size-str-off-size
static int rw_opt_no_replace_off_size_str_off_size;  
// for --no-replace-off-size-size-val
static int rw_opt_no_replace_off_size_size_val;      
// for --no-replace-ptr
static int rw_opt_no_replace_ptr;          
// for --no-replace-str
static int rw_opt_no_replace_str; 
// for --no-replace-ptr-size
static int rw_opt_no_replace_ptr_size;  
// for --no-replace-size-val
static int rw_opt_no_replace_size_val;          
// for --no-replace-range
static int rw_opt_no_replace_range;             

/**************************************************************************/

// used to exercise 
// replace (size_type pos1, size_type n1, const charT* s)
// replace (Iterator i1, Iterator i2, const charT* s)
static const TestCase off_size_test_cases [] = {

#undef TEST
#define TEST(str, pos1, num, src, res, bthrow)                       \
    { __LINE__, pos1, num, -1, -1, -1, str, sizeof str - 1, src,     \
      sizeof src - 1, res, sizeof res - 1, bthrow }

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

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",         2, 0, "",        "\0",            1),
    TEST ("a",         10, 0, "",        "a",             1),
    TEST (LSTR, LLEN + 10, 0, "",        LSTR,            1),

    TEST ("",   0,        0, LSTR,       LSTR,           -1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",      4, 0, "test",     "lasttest",      0)
};

/**************************************************************************/

// used to exercise 
// replace (size_type pos1, size_type n1, basic_string& s)
// replace (Iterator i1, Iterator i2, basic_string& s)
static const TestCase off_size_str_test_cases [] = {

#undef TEST
#define TEST(str, pos1, num, src, res, bthrow)                       \
    { __LINE__, pos1, num, -1, -1, -1, str, sizeof str - 1, src,     \
      sizeof src - 1, res, sizeof res - 1, bthrow }

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

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",         2, 0, "",        "\0",                1),
    TEST ("a",         10, 0, "",        "a",                 1),
    TEST (LSTR, LLEN + 10, 0, "",        LSTR,                1),

    TEST ("",   0,        0, LSTR,       LSTR,               -1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",      4, 0, "test",     "lasttest",          0)
};

/**************************************************************************/

// used to exercise
// replace (size_type pos1, size_type n1, const charT* s, size_type n2)
// replace (Iterator i1, Iterator i2, const charT* s, size_type n2)
static const TestCase off_size_size_test_cases [] = {

#undef TEST
#define TEST(str, pos1, num, src, count, res, bthrow)                     \
    { __LINE__, pos1, num, -1, count, -1, str, sizeof str - 1, src,       \
      sizeof src - 1, res, sizeof res - 1, bthrow }

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

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",         2, 0, "",         0, "\0",              1),
    TEST ("a",         10, 0, "",         0, "a",               1),
    TEST (LSTR, LLEN + 10, 0, "",         0, LSTR,              1),

    TEST ("a",  0,        1, LSTR, LLEN - 1, LSTR,             -1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",      4, 0, "test",      4, "lasttest",        0)
};

/**************************************************************************/

// used to exercise 
// replace (size_type pos1, size_type n1, basic_string& s, 
//          size_type pos2, size_type n2)
// replcae (iterator i1, Iterator i2, InputIterator j1, InputIterator j2)
static const TestCase range_test_cases [] = {

#undef TEST
#define TEST(str, pos1, num, src, pos2, count, res, bthrow)               \
    { __LINE__, pos1, num, pos2, count, -1, str, sizeof str - 1, src,     \
      sizeof src - 1, res, sizeof res - 1, bthrow }

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

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",         2, 0, "",           0, 0, "\0",             1),
    TEST ("\0",         0, 0, "\0",         2, 0, "",               2),

    TEST ("a",         10, 0, "",           0, 0, "a",              1),
    TEST ("a",          0, 0, "a",         10, 0, "",               2),

    TEST (LSTR, LLEN + 10, 0, "",           0, 0, LSTR,             1),
    TEST (LSTR, 0,         0, LSTR, LLEN + 10, 0, "",               2),

    TEST ("a",          0, 1, LSTR, 0, LLEN - 1, LSTR,             -1),
    TEST (LSTR,         0, 0, LSTR, 0, LPAR - 1, 0,                -1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",      4, 0, "test",      0, 4, "lasttest",         0)
};

/**************************************************************************/

// used to exercise
// replace (size_type pos1, size_type n1, charT c, size_type n2)
// replace (iterator i1, Iterator i2, charT c, size_type n2)
static const TestCase size_val_test_cases [] = {

#undef TEST
#define TEST(str, pos1, num, count, ch, res, bthrow)                     \
    { __LINE__, pos1, num, -1, count, ch, str, sizeof str - 1, 0,        \
      0, res, sizeof res - 1, bthrow }

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

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",         2, 0, 0, ' ',  "\0",           1),
    TEST ("a",         10, 0, 0, ' ',  "a",            1),
    TEST (LSTR, LLEN + 10, 0, 0, ' ',  LSTR,           1),

    TEST ("a",  0,        1,  LLEN - 1, 'x', LSTR,    -1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",      4, 0, 4, 't', "lasttttt",       0)
};

/**************************************************************************/

static const struct FunctionTag
{
    RTags           r_tag;
    const int      *p_opt;
    const TestCase *t_cases;
    std::size_t     n_cases;
    const char     *str_hdr;

} function_tags [] = {

#undef TEST
#define TEST(tag, opt, cases, hdr)                              \
    { tag, &opt, cases, sizeof cases / sizeof *cases, hdr }     

    TEST (replace_off_size_ptr, rw_opt_no_replace_off_size_ptr, 
          off_size_test_cases,
          "replace (size_type pos1, size_type n1, const charT* p)"),

    TEST (replace_off_size_str, rw_opt_no_replace_off_size_str, 
          off_size_str_test_cases,            
          "replace (size_type pos1, size_type n1, basic_string& s)"),

    TEST (replace_off_size_ptr_size, rw_opt_no_replace_off_size_ptr_size, 
          off_size_size_test_cases,                   
          "replace (size_type pos1, size_type n1, charT* p, size_type n2)"),

    TEST (replace_off_size_str_off_size, 
          rw_opt_no_replace_off_size_str_off_size, range_test_cases, 
          "replace (size_type pos1, size_type n1, basic_string& s, "
          "size_type pos2, size_type n2)"),

    TEST (replace_off_size_size_val, rw_opt_no_replace_off_size_size_val, 
          size_val_test_cases, 
          "replace (size_type pos1, size_type n1, size_type n, charT c)"),

    TEST (replace_ptr, rw_opt_no_replace_ptr, off_size_test_cases, 
          "replace (iterator i1, iterator i2, const charT* p)"),

    TEST (replace_str, rw_opt_no_replace_str, off_size_str_test_cases, 
          "replace (iterator i1, iterator i2, basic_string& s)"),

    TEST (replace_ptr_size, rw_opt_no_replace_ptr_size, 
          off_size_size_test_cases, 
          "replace (iterator i1, iterator i2, charT* p, size_type n2)"),

    TEST (replace_size_val, rw_opt_no_replace_size_val, 
          size_val_test_cases, 
          "replace (iterator i1, iterator i2, size_type n, charT c)"),

    TEST (replace_range, rw_opt_no_replace_range, range_test_cases, 
          "replace (iterator i1, iterator i2, InputIterator j1, "
          "InputIterator j2)")
};

/**************************************************************************/

template <class charT, class Traits>
void test_replace_exceptions (charT, Traits*,  
                              const RTags     which,
                              const TestCase &cs,
                              const char     *replace_fmt)
{
    typedef std::basic_string <charT, Traits, 
                               std::allocator<charT> > TestString;
    typedef typename TestString::iterator StringIter;
    typedef typename TestString::const_iterator ConstStringIter;

    const bool use_iters = (replace_ptr <= which);

    static charT wstr [LLEN];
    static charT wsrc [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (wsrc, cs.src, cs.src_len);

    TestString s_str (wstr, cs.str_len);
    TestString s_src (wsrc, cs.src_len);

    int first = use_iters ? cs.pos1 : cs.str_len + 1;
    int last  = use_iters ? cs.pos1 + cs.num : cs.str_len + 1;

    StringIter it_first (std::size_t (first) >= s_str.size () ? 
                         s_str.end () : s_str.begin () + first);
    StringIter it_last  (std::size_t (last) >= s_str.size () ?
                         s_str.end () : s_str.begin () + last);

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
            if (replace_off_size_ptr == which)
                s_str.replace (cs.pos1, cs.num, 
                               cs.src ? wsrc : s_str.c_str ());

            else if (replace_off_size_str == which)
                s_str.replace (cs.pos1, cs.num, cs.src ? s_src : s_str);

            else if (replace_off_size_ptr_size == which)
                s_str.replace (cs.pos1, cs.num, 
                               cs.src ? wsrc : s_str.c_str (), cs.count);

            else if (replace_off_size_str_off_size == which)
                s_str.replace (cs.pos1, cs.num, 
                               cs.src ? s_src : s_str, cs.pos2, cs.count);

            else if (replace_off_size_size_val == which)
                s_str.replace (cs.pos1, cs.num, cs.count, 
                               make_char ((char) cs.ch, (charT*)0));

            else if (replace_ptr == which)
                s_str.replace (it_first, it_last, 
                               cs.src ? wsrc : s_str.c_str ());

            else if (replace_str == which)
                s_str.replace (it_first, it_last, cs.src ? s_src : s_str);

            else if (replace_ptr_size == which)
                s_str.replace (it_first, it_last, 
                               cs.src ? wsrc : s_str.c_str (), cs.count);

            else if (replace_size_val == which)
                s_str.replace (it_first, it_last, cs.count, 
                               make_char ((char) cs.ch, (charT*)0));

            else if (replace_range == which)
                s_str.replace (it_first, it_last, 
                               s_src.begin (), s_src.end ());

            break;
        }
        _CATCH (...) {

#ifndef _RWSTD_NO_EXCEPTIONS

            // verify that an exception thrown during allocation
            // doesn't cause a change in the state of the vector

            rw_assert (s_str.size () == size, 0, cs.line,
                       "line %d: %s: size unexpectedly changed "
                       "from %zu to %zu after an exception",
                       __LINE__, replace_fmt, size, s_str.size ());

            rw_assert (s_str.capacity () == capacity, 0, cs.line,
                       "line %d: %s: capacity unexpectedly "
                       "changed from %zu to %zu after an exception",
                       __LINE__, replace_fmt, capacity, s_str.capacity ());

            
            rw_assert (s_str.begin () == begin, 0, cs.line,
                       "line %d: %s: begin() unexpectedly "
                       "changed from after an exception by %d",
                       __LINE__, replace_fmt, s_str.begin () - begin);


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
               __LINE__, replace_fmt);

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
void test_replace_range (charT* wstr,
                         charT* wsrc, 
                         Traits*,
                         const Iterator &it,
                         const TestCase &cs,
                         const char     *replace_fmt)
{
    typedef std::basic_string <charT, Traits, 
                               std::allocator<charT> > String;
    typedef typename String::iterator StringIter;

    const char* const itname = 
        cs.src ? type_name (it, (charT*)0) : "basic_string::iterator";

    String s_str (wstr, cs.str_len);
    String s_src (wsrc, cs.src_len);

    std::size_t off_last   = cs.pos1 + cs.num;
    std::size_t off_first2 = cs.pos2;
    std::size_t off_last2  = cs.pos2 + cs.count;

    StringIter it_first (std::size_t (cs.pos1) >= s_str.size () ? 
                         s_str.end () : s_str.begin () + cs.pos1);
    StringIter it_last  (std::size_t (off_last) >= s_str.size () ? 
                         s_str.end () : s_str.begin () + off_last);

    if (cs.src) {
        off_first2 = off_first2 > s_src.size () ? s_src.size () : off_first2;
        off_last2  = off_last2  > s_src.size () ? s_src.size () : off_last2;

        const Iterator first = make_iter (wsrc + off_first2, 
            wsrc + off_first2, wsrc + off_last2, Iterator (0, 0, 0));
        const Iterator last  = make_iter (wsrc + off_last2, 
            wsrc + cs.pos2, wsrc + off_last2, Iterator (0, 0, 0));

        s_str.replace (it_first, it_last, first, last);
    }
    else {
        StringIter first (std::size_t (cs.pos2) >= s_str.size () ? 
                          s_str.end () : s_str.begin () + cs.pos2); 
        StringIter last  (off_last2 > s_str.size () ? 
                          s_str.end () : s_str.begin () + off_last2);

        s_str.replace (it_first, it_last, first, last);
    }

    const std::size_t match = rw_match (cs.res, s_str.c_str(), cs.res_len);

    rw_assert (match == cs.res_len, 0, cs.line,
               "line %d. %s expected %{#*s}, got %{/*.*Gs}, "
               "difference at pos %zu for %s", 
               __LINE__, replace_fmt, int (cs.res_len), cs.res, 
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (), 
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits>
void test_replace_range (charT* wstr, 
                         charT* wsrc, 
                         Traits*,
                         const TestCase &cs,
                         const char     *replace_fmt)
{
    if (cs.bthrow)  // this method doesn't throw
        return;

    test_replace_range (wstr, wsrc, (Traits*)0, 
                       InputIter<charT>(0, 0, 0), cs, replace_fmt);

    // there is no need to call test_replace_range 
    // for other iterators in this case
    if (0 == cs.src)
        return;

    test_replace_range (wstr, wsrc, (Traits*)0, 
                       ConstFwdIter<charT>(0, 0, 0), cs, replace_fmt);

    test_replace_range (wstr, wsrc, (Traits*)0, 
                       ConstBidirIter<charT>(0, 0, 0), cs, replace_fmt);

    test_replace_range (wstr, wsrc, (Traits*)0, 
                       ConstRandomAccessIter<charT>(0, 0, 0), cs, replace_fmt);
}

/**************************************************************************/

template <class charT, class Traits>
void test_replace (charT, Traits*,  
                   const RTags     which,
                   const TestCase &cs,
                   const char     *replace_fmt)
{
    typedef std::basic_string <charT, Traits, 
                               std::allocator<charT> > TestString;
    typedef typename TestString::iterator StringIter;

    const bool use_iters = (replace_ptr <= which);

    static charT wstr [LLEN];
    static charT wsrc [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (wsrc, cs.src, cs.src_len);

    // special processing for replace_range to exercise all iterators
    if (replace_range == which) {
        test_replace_range (wstr, wsrc, (Traits*)0, cs, replace_fmt);
        return;
    }

    TestString s_str (wstr, cs.str_len);
    TestString s_src (wsrc, cs.src_len);

    std::size_t res_off = 0;
    int first = use_iters ? cs.pos1 : cs.str_len + 1;
    int last  = use_iters ? cs.pos1 + cs.num : cs.str_len + 1;

    StringIter it_first (std::size_t (first) >= s_str.size () ? 
                         s_str.end () : s_str.begin () + first);
    StringIter it_last  (std::size_t (last) >= s_str.size () ?
                         s_str.end () : s_str.begin () + last);

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    if (1 == cs.bthrow && !use_iters)
        expected = exp_exceptions [1];
    if (2 == cs.bthrow && replace_off_size_str_off_size == which)
        expected = exp_exceptions [1];
    if (3 == cs.bthrow && !use_iters)
        expected = exp_exceptions [2];

    const char* caught = 0;

    try {

#endif   // _RWSTD_NO_EXCEPTIONS

    switch (which)
    {
    case replace_off_size_ptr: {
        TestString& s_res = 
            s_str.replace (cs.pos1, cs.num, cs.src ? wsrc : s_str.c_str ());
        res_off = &s_res - &s_str;
        break;
    }

    case replace_off_size_str: {
        TestString& s_res = 
            s_str.replace (cs.pos1, cs.num, cs.src ? s_src : s_str);
        res_off = &s_res - &s_str;
        break;
    }

    case replace_off_size_ptr_size: {
        TestString& s_res = 
            s_str.replace (cs.pos1, cs.num, 
                           cs.src ? wsrc : s_str.c_str (), cs.count);
        res_off = &s_res - &s_str;
        break;
    }

    case replace_off_size_str_off_size: {
        TestString& s_res = 
            s_str.replace (cs.pos1, cs.num, 
                           cs.src ? s_src : s_str, cs.pos2, cs.count);
        res_off = &s_res - &s_str;
        break;
    }

    case replace_off_size_size_val: {
        TestString& s_res = 
            s_str.replace (cs.pos1, cs.num, cs.count, 
                           make_char ((char) cs.ch, (charT*)0));
        res_off = &s_res - &s_str;
        break;
    }

    case replace_ptr: {
        TestString& s_res = 
            s_str.replace (it_first, it_last, cs.src ? wsrc : s_str.c_str ());
        res_off = &s_res - &s_str;
        break;
    }

    case replace_str: {
        TestString& s_res = 
            s_str.replace (it_first, it_last, cs.src ? s_src : s_str);
        res_off = &s_res - &s_str;
        break;
    }

    case replace_ptr_size: {
        TestString& s_res = 
            s_str.replace (it_first, it_last, 
                           cs.src ? wsrc : s_str.c_str (), cs.count);
        res_off = &s_res - &s_str;
        break;
    }

    case replace_size_val: {
        TestString& s_res = 
            s_str.replace (it_first, it_last, cs.count, 
                           make_char ((char) cs.ch, (charT*)0));
        res_off = &s_res - &s_str;
        break;
    }

    default:
        RW_ASSERT ("test logic error: unknown replace overload");
        return;
    }

    // verify the returned value
    rw_assert (0 == res_off, 0, cs.line,
               "line %d. %s returned invalid reference, offset is %zu%", 
               __LINE__, replace_fmt, res_off);

    // verfiy that strings length are equal
    rw_assert (cs.res_len == s_str.size (), 0, cs.line,
               "line %d. %s expected %{#*s} with length %zu, got %{/*.*Gs} "
               "with length %zu", __LINE__, replace_fmt, int (cs.res_len), 
               cs.res, cs.res_len, int (sizeof (charT)), int (s_str.size ()),
               s_str.c_str (), s_str.size ());

    // verfiy that replace results match expected result
    const std::size_t match = rw_match (cs.res, s_str.c_str(), cs.res_len);

    rw_assert (match == cs.res_len, 0, cs.line,
               "line %d. %s expected %{#*s}, got %{/*.*Gs}, "
               "difference at pos %zu", 
               __LINE__, replace_fmt, int (cs.res_len), cs.res, 
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
               replace_fmt, 0 != expected, expected, caught, caught);
}

/**************************************************************************/

void get_replace_format (char** pbuf, std::size_t* pbufsize, 
                        const MemFun *pfid, 
                        const RTags which, 
                        const TestCase& cs)
{
    if (   MemFun::DefaultTraits == pfid->tid_
        && (MemFun::Char == pfid->cid_ || MemFun::WChar == pfid->cid_))
        rw_asnprintf (pbuf, pbufsize,
                      "std::%{?}w%{;}string (%{#*s}).replace",
                      MemFun::WChar == pfid->cid_,
                      int (cs.str_len), cs.str);
    else
        rw_asnprintf (pbuf, pbufsize,
                      "std::basic_string<%s, %s<%1$s>, %s<%1$s>>(%{#*s})"
                      ".replace", pfid->cname_, pfid->tname_, pfid->aname_, 
                      int (cs.str_len), cs.str);

    const bool self = 0 == cs.src;

    switch (which)
    {
    case replace_off_size_ptr:
        rw_asnprintf (pbuf, pbufsize, 
                      "%{+} (%zu, %zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      cs.pos1, cs.num, !self, int (cs.src_len), cs.src, self);
        break;

    case replace_off_size_str:
        rw_asnprintf (pbuf, pbufsize, 
                      "%{+} (%zu, %zu, %{?}string (%{#*s})%{;}%{?}*this%{;})",
                      cs.pos1, cs.num, !self, int (cs.src_len), cs.src, self);
        break;

    case replace_off_size_ptr_size:
        rw_asnprintf (pbuf, pbufsize, "%{+} ("
                      "%zu, %zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)", 
                      cs.pos1, cs.num, !self, int (cs.src_len),
                      cs.src, self, cs.count);
        break;

    case replace_off_size_str_off_size:
        rw_asnprintf (pbuf, pbufsize, "%{+} (%zu, %zu, "
                      "%{?}string (%{#*s})%{;}%{?}*this%{;}, %zu, %zu)",
                      cs.pos1, cs.num, !self, int (cs.src_len), cs.src,
                      self, cs.pos2, cs.count);
        break;

    case replace_off_size_size_val:
        rw_asnprintf (pbuf, pbufsize, 
                      "%{+} (%zu, %zu, %zu, %#c)",
                      cs.pos1, cs.num, cs.count, cs.ch);
        break;

    case replace_ptr:
        rw_asnprintf (pbuf, pbufsize, "%{+} (begin + %zu, begin + %zu, "
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      cs.pos1, cs.pos1 + cs.num, 
                      !self, int (cs.src_len), cs.src, self);
        break;

    case replace_str:
        rw_asnprintf (pbuf, pbufsize, "%{+} (begin + %zu, begin + %zu, " 
                      "%{?}string (%{#*s})%{;}%{?}*this%{;})",
                      cs.pos1, cs.pos1 + cs.num, 
                      !self, int (cs.src_len), cs.src, self);
        break;

    case replace_ptr_size:
        rw_asnprintf (pbuf, pbufsize, "%{+} (begin + %zu, begin + %zu, " 
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)", 
                      cs.pos1, cs.pos1 + cs.num, !self, int (cs.src_len),
                      cs.src, self, cs.count);
        break;

    case replace_size_val:
        rw_asnprintf (pbuf, pbufsize, 
                      "%{+} (begin + %zu, begin + %zu, %zu, %#c)",
                      cs.pos1, cs.pos1 + cs.num, cs.count, cs.ch);
        break;

    case replace_range:
        rw_asnprintf (pbuf, pbufsize, "%{+} (begin + %zu, begin + %zu, "
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu, "
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu)", 
                      cs.pos1, cs.pos1 + cs.num, !self, int (cs.src_len), 
                      cs.src, self, cs.pos2, !self, int (cs.src_len), cs.src,
                      self, cs.pos2 + cs.count);
        break;
    }
}

/**************************************************************************/

void test_replace (const MemFun *pfid, const RTags which, 
                   const TestCase& cs,  bool exc_safety_test)
{
    char* buf = 0;
    std::size_t buf_sz = 0;
    get_replace_format (&buf, &buf_sz, pfid, which, cs); 

#undef TEST
#define TEST(charT, Traits)	                                             \
    !exc_safety_test ?                                                   \
        test_replace (charT(), (Traits*)0, which, cs, buf)               \
      : test_replace_exceptions (charT(), (Traits*)0, which, cs, buf)

    if (MemFun:: DefaultTraits == pfid->tid_) {
        if (MemFun::Char == pfid->cid_)
            TEST (char, std::char_traits<char>);

#ifndef _RWSTD_NO_WCHAR_T
    else
        TEST (wchar_t, std::char_traits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T

    }
    else {
       if (MemFun::Char == pfid->cid_)
           TEST (char, UserTraits<char>);

#ifndef _RWSTD_NO_WCHAR_T
       else if (MemFun::WChar == pfid->cid_)
           TEST (wchar_t, UserTraits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T

       else
           TEST (UserChar, UserTraits<UserChar>);
    }

    free (buf);
}

/**************************************************************************/

static void
test_replace (const MemFun *pfid, const FunctionTag& ftag)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::%s",
             pfid->cname_, pfid->tname_, pfid->aname_, ftag.str_hdr);

    if (rw_opt_no_exception_safety)
        rw_note (0, 0, 0,
                 "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
                 "%s exception safety test disabled", 
                 pfid->cname_, pfid->tname_, pfid->aname_, ftag.str_hdr);

#ifdef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    else
        rw_warn (0, 0, __LINE__,
                 "%s exception safety test: no replacable new and delete",
                 ftag.str_hdr);

#endif  //_RWSTD_NO_REPLACEABLE_NEW_DELETE

    for (std::size_t i = 0; i != ftag.n_cases; ++i) {

        if (!rw_enabled (ftag.t_cases[i].line)) {
            rw_note (0, 0, __LINE__, 
                     "test on line %d disabled", ftag.t_cases[i].line);
            continue;
        }

        // do not exercise exceptions if they were disabled
        if (0 != rw_opt_no_exceptions && 0 != ftag.t_cases[i].bthrow)
            continue;

        // do not exercise exception safety if they were disabled
        if (0 != rw_opt_no_exception_safety && -1 == ftag.t_cases[i].bthrow)
            continue;

        test_replace (pfid, ftag.r_tag, ftag.t_cases[i], 
                      -1 == ftag.t_cases[i].bthrow);
    }
}


/**************************************************************************/

static void
run_test (const MemFun *pfid)
{
    if (MemFun::UserTraits == pfid->tid_ && rw_opt_no_user_traits) {
        rw_note (1 < rw_opt_no_user_traits++, 0, 0,
                 "user defined traits test disabled");
    }
    else if (MemFun::DefaultTraits == pfid->tid_  && rw_opt_no_char_traits) {
        rw_note (1 < rw_opt_no_char_traits++, 0, 0,
                 "char_traits test disabled");
    }
    else {

        if (rw_opt_no_exceptions)
            rw_note (1 < rw_opt_no_exceptions++, 0, 0,
                     "string::replace exceptions tests disabled"); 

        static const std::size_t ftags = 
            sizeof function_tags / sizeof *function_tags;

        for (std::size_t i = 0; i < ftags; i++) {

            if (*function_tags[i].p_opt) 
                rw_note (0, 0, 0, 
                         "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
                         "%s test disabled", pfid->cname_, pfid->tname_, 
                         pfid->aname_, function_tags[i].str_hdr);
            else
                test_replace (pfid, function_tags[i]);
        }
    }
}

/**************************************************************************/

int run_test (int, char*[])
{
    if ('\0' == LSTR [0]) {
        // initialize LSTR
        for (std::size_t i = 0; i != sizeof LSTR - 1; ++i)
            LSTR [i] = 'x';
    }

    if (rw_enabled ("char")) {

        MemFun fid (MemFun::Char, "char", MemFun::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = MemFun::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::replace char tests disabled");

    if (rw_enabled ("wchar_t")) {

        MemFun fid (MemFun::WChar, "wchar_t", MemFun::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = MemFun::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::replace wchar tests disabled");

    if (rw_opt_no_user_chars) {
        rw_note (0, 0, 0, "user defined chars test disabled");
    }
    else {
        MemFun fid (MemFun::UChar, "UserChar", MemFun::UserTraits, 0);
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
                    "|-no-user_chars# "
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

                    &rw_opt_no_char_traits,
                    &rw_opt_no_user_traits,
                    &rw_opt_no_user_chars,
                    &rw_opt_no_exceptions,
                    &rw_opt_no_exception_safety,

                    &rw_opt_no_replace_off_size_ptr,
                    &rw_opt_no_replace_off_size_str,
                    &rw_opt_no_replace_off_size_ptr_size,
                    &rw_opt_no_replace_off_size_str_off_size,
                    &rw_opt_no_replace_off_size_size_val,
                    &rw_opt_no_replace_ptr,
                    &rw_opt_no_replace_str,
                    &rw_opt_no_replace_ptr_size,
                    &rw_opt_no_replace_size_val,
                    &rw_opt_no_replace_range);
}
