/***************************************************************************
 *
 * 21.string.find.first.not.of.cpp - 
 *      string test exercising lib.string.find.first.not.of
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
#include <stdexcept>    // for length_error

#include <21.strings.h> // for StringMembers
#include <driver.h>     // for rw_test()
#include <rw_char.h>    // for rw_widen()

#define FindFirstNotOf(which)    StringMembers::find_first_not_of_ ## which

typedef StringMembers::OverloadId OverloadId;
typedef StringMembers::TestCase   TestCase;
typedef StringMembers::Test       Test;
typedef StringMembers::Function   MemFun;

/**************************************************************************/

// for convenience and brevity
#define NPOS      _RWSTD_SIZE_MAX
#define LSTR      StringMembers::long_string
#define LLEN      StringMembers::long_string_len

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise 
// find_first_not_of (const value_type*)
static const TestCase ptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res)                                    \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, arg,  \
      sizeof arg - 1, 0, res, 0 }

    //    +----------------------------------- controlled sequence
    //    |             +--------------------- sequence to be found
    //    |             |                +---- expected result 
    //    |             |                |                               
    //    |             |                |                     
    //    V             V                V        
    TEST ("ab",         "c",             0),   

    TEST ("",           "",           NPOS),   
    TEST ("",           "\0",         NPOS),    
    TEST ("",           "a",          NPOS),   

    TEST ("\0",         "",              0),    
    TEST ("\0",         "\0",            0),   
    TEST ("\0",         "a",             0),   

    TEST ("bbcdefghij", "a",             0),    
    TEST ("abcdefghij", "a",             1),  
    TEST ("abcdefghij", "f",             0),   

    TEST ("eaccbbhjig", "cba",           0),     
    TEST ("ceabcbahca", "cba",           1),   
    TEST ("bacbahjicg", "cba",           5),  
    TEST ("abbcbacbca", "cba",        NPOS),
    TEST ("bcbedfbjih", "abce",          4),   
    TEST ("bcaedfajih", "aabced",        5),    
    TEST ("bcedfaacdh", "abcdef",        9),    

    TEST ("e\0cb\0\0g", "b\0\0g",        0),  
    TEST ("e\0cb\0\0g", "cbe",           1),    
    TEST ("\0cb\0\0ge", "\0\0ge",        0),   
    TEST ("bcbc\0\0be", "b\0c",          1),   
    TEST ("gbg\0\0e\0", "bg",            3),      
    TEST ("a\0b",       "e\0gbg\0\0",    0),    
    TEST ("b\0a",       "eb\0gg\0\0",    1), 
    TEST ("ab\0",       "ab\0gg\0\0",    2),   

    TEST ("aaaaaaaaaa", "aaaaaaaaaa", NPOS),     
    TEST ("aaaaabaaaa", "cccccccccb",    0),   
    TEST ("aabaabaaaa", "aaaaaaaaab", NPOS),
    TEST ("bbb",        "aaaaaaaaba", NPOS),        
    TEST ("aab",        "aaaaaaaaaa",    2),    

    TEST (LSTR,         "",              0),     
    TEST (LSTR,         "a",             0),  
    TEST (LSTR,         "x",          NPOS),       
    TEST (LSTR,         "axa",        NPOS),   
    TEST ("abc",        LSTR,            0),    
    TEST ("xabc",       LSTR,            1),  

    TEST ("abcdefghij", 0,            NPOS),      
    TEST ("\0cb\0\0ge", 0,               0),       
    TEST (LSTR,         0,            NPOS),

    TEST ("last test",  "test",          0)       
};

/**************************************************************************/

// used to exercise 
// find_first_not_of (const basic_string&)
static const TestCase str_test_cases [] = {

#undef TEST     
#define TEST(str, arg, res)                                    \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, arg,  \
      sizeof arg - 1, 0, res, 0 }

    //    +------------------------------------ controlled sequence
    //    |             +---------------------- sequence to be found
    //    |             |                +----- expected result 
    //    |             |                |                               
    //    |             |                |                     
    //    V             V                V        
    TEST ("ab",         "c",             0),   

    TEST ("",           "",           NPOS),   
    TEST ("",           "\0",         NPOS),    
    TEST ("",           "a",          NPOS),   

    TEST ("\0",         "",              0),    
    TEST ("\0",         "\0",         NPOS),  
    TEST ("\0\0",       "\0",         NPOS),
    TEST ("\0",         "a",             0),   

    TEST ("bbcdefghij", "a",             0),    
    TEST ("abcdefghij", "a",             1),  
    TEST ("abcdefghij", "f",             0),   

    TEST ("eaccbbhjig", "cba",           0),     
    TEST ("ceabcbahca", "cba",           1),   
    TEST ("bacbahjicg", "cba",           5),  
    TEST ("abbcbacbca", "cba",        NPOS),
    TEST ("bcbedfbjih", "abce",          4),   
    TEST ("bcaedfajih", "aabced",        5),    
    TEST ("bcedfaacdh", "abcdef",        9),    

    TEST ("e\0cb\0\0g", "b\0\0g",        0),
    TEST ("\0cb\0\0ge", "b\0\0g",        1),
    TEST ("\0gb\0\0ge", "b\0\0g",        6),
    TEST ("e\0cb\0\0g", "cbe",           1),    
    TEST ("\0cb\0\0ge", "\0\0ge",        1),   
    TEST ("bcbc\0\0bc", "b\0c",       NPOS),   
    TEST ("gbg\0\0e\0", "bg",            3),      
    TEST ("a\0b",       "e\0gbg\0\0",    0),    
    TEST ("b\0a",       "eb\0gg\0\0",    2), 
    TEST ("ab\0",       "ab\0gg\0\0", NPOS),   

    TEST ("aaaaaaaaaa", "aaaaaaaaaa", NPOS),     
    TEST ("aaaaabaaaa", "cccccccccb",    0),   
    TEST ("aabaabaaaa", "aaaaaaaaab", NPOS),
    TEST ("bbb",        "aaaaaaaaba", NPOS),        
    TEST ("aab",        "aaaaaaaaaa",    2),    

    TEST (LSTR,         "",              0),     
    TEST (LSTR,         "a",             0),  
    TEST (LSTR,         "x",          NPOS),       
    TEST (LSTR,         "axa",        NPOS),   
    TEST ("abc",        LSTR,            0),    
    TEST ("xabc",       LSTR,            1),  

    TEST ("abcdefghij", 0,            NPOS),      
    TEST ("\0cb\0\0ge", 0,            NPOS),       
    TEST (LSTR,         0,            NPOS),

    TEST ("last test",  "test",          0)      
};

/**************************************************************************/

// used to exercise 
// find_first_not_of (const value_type*, size_type)
static const TestCase ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)                                    \
    { __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1, arg,      \
      sizeof arg - 1, 0, res, 0 }

    //    +---------------------------------- controlled sequence
    //    |            +--------------------- sequence to be found
    //    |            |               +----- find_first_not_of() off argument
    //    |            |               |     +- expected result  
    //    |            |               |     |                             
    //    |            |               |     |             
    //    V            V               V     V                 
    TEST ("ab",        "c",            0,    0),   

    TEST ("",           "",            0, NPOS),    
    TEST ("",           "\0",          0, NPOS),    
    TEST ("",           "a",           0, NPOS),    

    TEST ("\0",         "",            0,    0),    
    TEST ("\0",         "\0",          0,    0),   
    TEST ("\0",         "\0",          1, NPOS),    
    TEST ("\0",         "a",           0,    0),    

    TEST ("bbcdefghij", "a",           0,    0),    
    TEST ("abcdefghij", "a",           0,    1),   
    TEST ("bcaaaaaaaa", "a",           2, NPOS),    
    TEST ("bcaaafaaaa", "a",           2,    5),      
    TEST ("abcdefghij", "j",           9, NPOS),  

    TEST ("cbbedfhjig", "cba",         0,    3),    
    TEST ("edfcbahjig", "cba",         4,    6),     
    TEST ("edfcbaaabc", "cba",         6, NPOS),    
    TEST ("edfcbahcba", "cba",         9, NPOS),       
    TEST ("cbacbabjig", "cbaig",       0,    7),    
    TEST ("bcaedfajih", "aabced",      0,    5),    
    TEST ("bcedfaacdh", "abcdef",      3,    9),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,    0),    
    TEST ("\0cbe\0\0g", "b\0\0g",      4,    4),    
    TEST ("e\0cb\0\0g", "cbe",         0,    1),     
    TEST ("\0cb\0\0ge", "\0\0ge",      0,    0),  
    TEST ("\0cb\0\0ge", "cb\0",        6,    6),     
    TEST ("e\0gbg\0\0", "bg",          2,    5),     
    TEST ("a\0b",       "a\0gbg\0\0",  0,    1),    
    TEST ("ab\0",       "ab\0gg\0\0",  1,    2), 
    TEST ("a\0b",       "e\0gg\0\0a",  3, NPOS),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0, NPOS),    
    TEST ("aaabaaaaaa", "aaaaaaaaaa",  4, NPOS),    
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  1,    5),   
    TEST ("aabaabaaaa", "aaaaaaaaaa",  0,    2),  
    TEST ("aabaabaaaa", "aaaaaaaaaa",  3,    5), 
    TEST ("aabaabaaaa", "aaaaaaaaab",  0, NPOS),
    TEST ("cbb",        "aaaaaaaaba",  2, NPOS),  
    TEST ("bac",        "aaaaaaaaba",  0,    2),
    TEST ("baa",        "aaaaaaaaaa",  0,    0),     

    TEST (LSTR,         "",            0,    0),    
    TEST (LSTR,         "a",           0,    0),     
    TEST (LSTR,         "x",           0, NPOS),     
    TEST (LSTR,         "xxx",        10, NPOS),    
    TEST (LSTR,         "axa",        10, NPOS),     
    TEST ("abc",        LSTR,          2,    2),    
    TEST ("xxxxxxxxxx", LSTR,          0, NPOS),   

    TEST (LSTR,         "xxx",  LLEN - 4, NPOS), 
    TEST (LSTR,         "xxx",  LLEN - 1, NPOS),     

    TEST ("abcdefghij", 0,             0, NPOS),    
    TEST ("abcdefghij", 0,             1, NPOS),   
    TEST ("\0cb\0\0ge", 0,             5,    5),    
    TEST (LSTR,         0,             0, NPOS),  
    TEST (LSTR,         0,             7, NPOS),    

    TEST ("",           "",            1, NPOS),  
    TEST ("abcdefghij", "abc",        10, NPOS),   
    TEST ("abcdefghij", "cba",        10, NPOS), 

    TEST ("last test", "test",         0,    0)      
};

/**************************************************************************/

// used to exercise 
// find_first_not_of (const value_type*, size_type, size_type)
static const TestCase ptr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, size, res, bthrow)                        \
    { __LINE__, off, size, -1, -1, -1, str, sizeof str - 1, arg,      \
      sizeof arg - 1, 0, res, bthrow }

    //    +---------------------------------- controlled sequence
    //    |            +--------------------- sequence to be found
    //    |            |               +----- find_first_not_of() off argument
    //    |            |               |   +- find_first_not_of() n argument 
    //    |            |               |   |     +- expected result  
    //    |            |               |   |     |  exception info 
    //    |            |               |   |     |  |   0 - no exception        
    //    |            |               |   |     |  |   1 - length_error  
    //    |            |               |   |     |  |                           
    //    |            |               |   |     |  +-----+             
    //    V            V               V   V     V        V             
    TEST ("ab",        "c",            0,  1,    0,       0),

    TEST ("",           "",            0,  0, NPOS,       0),
    TEST ("",           "\0",          0,  0, NPOS,       0),
    TEST ("",           "\0",          0,  1, NPOS,       0),
    TEST ("",           "a",           0,  0, NPOS,       0),
    TEST ("",           "a",           0,  1, NPOS,       0),

    TEST ("\0",         "",            0,  0,    0,       0),
    TEST ("\0",         "\0",          0,  1, NPOS,       0),
    TEST ("\0",         "\0",          1,  1, NPOS,       0),
    TEST ("\0\0",       "\0\0",        1,  1, NPOS,       0),
    TEST ("\0",         "a",           0,  1,    0,       0),
    TEST ("a",          "\0",          0,  1,    0,       0),

    TEST ("cbbedfhjig", "cba",         0,  3,    3,       0),
    TEST ("edfcbbhjig", "bac",         0,  2,    0,       0),
    TEST ("edfcbahaib", "cba",         7,  3,    8,       0),
    TEST ("edfcbahbcb", "cba",         7,  2, NPOS,       0),
    TEST ("edfcbahbcb", "cba",         3,  2,    5,       0),
    TEST ("edfcbahcba", "bac",         3,  3,    6,       0),
    TEST ("edacbehcba", "abc",         2,  2,    3,       0),
    TEST ("babcbahcba", "cba",         0,  3,    6,       0),
    TEST ("hjigcbacba", "cba",         4,  3, NPOS,       0),
    TEST ("cbacbcccbc", "cba",         5,  1,    8,       0),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,  4,    0,       0),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  4, NPOS,       0),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  1,    4,       0),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  2,    6,       0),
    TEST ("\0b\0\0gb\0","bg\0",        2,  2,    2,       0),
    TEST ("\0b\0\0gb\0","bg\0",        0,  2,    0,       0),
    TEST ("\0b\0\0gb\0","bg\0",        0,  3, NPOS,       0),
    TEST ("e\0cb\0\0g", "a\0e",        0,  3,    2,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      7,  4, NPOS,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  0,    6,       0),
    TEST ("a\0b",       "e\0gbg\0\0",  0,  1,    0,       0),    
    TEST ("a\0b",       "ab\0gg\0\0",  1,  2,    1,       0), 
    TEST ("a\0b",       "\0ba\0\0fe",  1,  2, NPOS,       0),
    TEST ("a\0b",       "e\0gg\0\0a",  3,  6, NPOS,       0),
    TEST ("a\0b",       "e\0gg\0\0a",  0,  7,    2,       0),
    TEST ("\0baa",      "b\0g\0\0ac",  0,  4,    2,       0),

    TEST ("e\0a\0",     "e\0a\0\0",    0,  4, NPOS,       0),
    TEST ("\0\0ea",     "b\0c\0\0",    0,  5,    2,       0),
    TEST ("ee\0b\0",    "e\0a\0b",     1,  4,    3,       0),
    TEST ("be\0a\0",    "\0acefdg",    1,  5, NPOS,       0),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0, 10, NPOS,       0),
    TEST ("baaaaaaaaa", "aaaaaaaaaa",  1, 10, NPOS,       0),
    TEST ("aaaaabaaaa", "cccccccccc",  0, 10,    0,       0),
    TEST ("aaaaabaaaa", "aaaaaccbcc",  1,  4,    5,       0),
    TEST ("aaaabaaaaa", "cccca",       0,  5,    4,       0),
    TEST ("aabaabadca", "acaaacccab",  0,  5,    2,       0), 
    TEST ("abcdefghij", "jihgfedcba",  0, 10, NPOS,       0), 
    TEST ("abcdefghij", "jihgfedcba",  0,  9,    0,       0),
    TEST ("bbb",        "aaaaaaaaba",  2,  2,    2,       0),  
    TEST ("bbb",        "aaaaaaaaba",  0,  9, NPOS,       0),
    TEST ("bab",        "baaaaaaaaa",  0,  0,    0,       0),
    TEST ("bab",        "ccccccccba",  1, 10, NPOS,       0),

    TEST (LSTR,         "",            0,  0,    0,       0),
    TEST (LSTR,         "a",           0,  1,    0,       0),
    TEST (LSTR,         "x",           0,  1, NPOS,       0),
    TEST (LSTR,         "xxx",        10,  3, NPOS,       0),
    TEST (LSTR,         "axx",        10,  1,   10,       0),
    TEST (LSTR,         "xxa",        10,  0,   10,       0),
    TEST (LSTR,         "xxa",        10,  1, NPOS,       0),
    TEST ("abc",        LSTR,          2, 10,    2,       0),
    TEST ("xxxxxxxxxx", LSTR,          0, LLEN - 1, NPOS, 0),
    TEST ("xxxxxxxxxx", LSTR,          2,  4, NPOS,       0),
    TEST ("xxxxxxxxxa", LSTR,          0,  4,    9,       0),

    TEST (LSTR,         "xxx",  LLEN - 4,  3, NPOS,       0),
    TEST (LSTR,         "xxx",  LLEN - 3,  0, LLEN - 3,   0),

    TEST ("abcdefghij", 0,             0, 10, NPOS,       0),
    TEST ("abcdefghij", 0,             1,  9,    9,       0),
    TEST ("abcdefghij", 0,             0,  8,    8,       0),
    TEST ("\0cb\0\0ge", 0,             5,  7, NPOS,       0),
    TEST ("\0cb\0\0ge", 0,             5,  5,    5,       0),
    TEST ("\0cb\0\0cb", 0,             3,  5, NPOS,       0),
    TEST ("\0cb\0ge\0", 0,             6,  1, NPOS,       0),
    TEST (LSTR,         0,             0, LLEN - 1, NPOS, 0),
    TEST (LSTR,         0,      LLEN - 1, LLEN - 1, NPOS, 0),
    TEST (LSTR,         0,      LLEN - 2,  1, NPOS,       0),

    TEST ("",           "",            1,  0, NPOS,       0),
    TEST ("abcdefghij", "abc",        10,  3, NPOS,       0),
    TEST ("abcdefghij", "cba",        10,  1, NPOS,       0),

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("",           "cba",         0, -1,    0,       1),
    TEST ("abcdefghij", "cba",         0, -1,    0,       1),
    TEST (LSTR,         "xxx",         0, -1,    0,       1),
    TEST ("abcdefghij", LSTR,          0, -1,    0,       1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last test", "test",         0,  4,    0,       0)
};

/**************************************************************************/

// used to exercise 
// find_first_not_of (const basic_string&, size_type)
static const TestCase str_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)                                    \
    { __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1, arg,      \
      sizeof arg - 1, 0, res, 0 }

    //    +---------------------------------- controlled sequence
    //    |             +-------------------- sequence to be found
    //    |             |              +----- find_first_not_of() off argument
    //    |             |              |     +- expected result 
    //    |             |              |     |                          
    //    |             |              |     |           
    //    V             V              V     V                
    TEST ("ab",         "c",           0,    0),     

    TEST ("",           "",            0, NPOS),    
    TEST ("",           "\0",          0, NPOS),    
    TEST ("",           "a",           0, NPOS),    

    TEST ("\0",         "",            0,    0),    
    TEST ("\0",         "\0",          0, NPOS),   
    TEST ("\0",         "\0",          1, NPOS), 
    TEST ("\0\0",       "\0",          0, NPOS),  
    TEST ("\0",         "a",           0,    0),    

    TEST ("bbcdefghij", "a",           0,    0),    
    TEST ("abcdefghij", "a",           0,    1),   
    TEST ("bcaaaaaaaa", "a",           2, NPOS),    
    TEST ("bcaaafaaaa", "a",           2,    5),      
    TEST ("abcdefghij", "j",           9, NPOS),  

    TEST ("cbbedfhjig", "cba",         0,    3),    
    TEST ("edfcbahjig", "cba",         4,    6),     
    TEST ("edfcbaaabc", "cba",         6, NPOS),    
    TEST ("edfcbahcba", "cba",         9, NPOS),       
    TEST ("cbacbabjig", "cbaig",       0,    7),    
    TEST ("bcaedfajih", "aabced",      0,    5),    
    TEST ("bcedfaacdh", "abcdef",      3,    9),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,    0),    
    TEST ("\0cbe\0\0g", "b\0\0g",      4, NPOS),    
    TEST ("e\0cb\0\0g", "cbe",         0,    1),     
    TEST ("\0ge\0\0cb", "\0\0ge",      0,    5),  
    TEST ("\0cb\0\0ge", "cb\0",        6,    6), 
    TEST ("\0cb\0\0ce", "cb\0",        0,    6), 
    TEST ("e\0gbg\0\0", "bg",          2,    5),
    TEST ("a\0c",       "a\0gbg\0\0",  0,    2),
    TEST ("a\0b",       "a\0gbg\0\0",  0, NPOS),    
    TEST ("ab\0",       "ab\0gg\0\0",  1, NPOS), 
    TEST ("a\0b",       "e\0gg\0\0a",  3, NPOS),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0, NPOS),    
    TEST ("aaabaaaaaa", "aaaaaaaaaa",  4, NPOS),    
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  1,    5),   
    TEST ("aabaabaaaa", "aaaaaaaaaa",  0,    2),  
    TEST ("aabaabaaaa", "aaaaaaaaaa",  3,    5), 
    TEST ("aabaabaaaa", "aaaaaaaaab",  0, NPOS),
    TEST ("cbb",        "aaaaaaaaba",  2, NPOS),  
    TEST ("bac",        "aaaaaaaaba",  0,    2),
    TEST ("baa",        "aaaaaaaaaa",  0,    0),     

    TEST (LSTR,         "",            0,    0),    
    TEST (LSTR,         "a",           0,    0),     
    TEST (LSTR,         "x",           0, NPOS),     
    TEST (LSTR,         "xxx",        10, NPOS),    
    TEST (LSTR,         "axa",        10, NPOS),     
    TEST ("abc",        LSTR,          2,    2),    
    TEST ("xxxxxxxxxx", LSTR,          0, NPOS),   

    TEST (LSTR,         "xxx",  LLEN - 4, NPOS), 
    TEST (LSTR,         "xxx",  LLEN - 1, NPOS),     

    TEST ("abcdefghij", 0,             0, NPOS),    
    TEST ("abcdefghij", 0,             1, NPOS),   
    TEST ("\0cb\0\0ge", 0,             5, NPOS),    
    TEST (LSTR,         0,             0, NPOS),  
    TEST (LSTR,         0,             7, NPOS),    

    TEST ("",           "",            1, NPOS),  
    TEST ("abcdefghij", "abc",        10, NPOS),   
    TEST ("abcdefghij", "cba",        10, NPOS), 

    TEST ("last test",  "test",        0,    0)     
};

/**************************************************************************/

// used to exercise 
// find_first_not_of (value_type)
static const TestCase val_test_cases [] = {

#undef TEST
#define TEST(str, val, res)                                     \
    { __LINE__, -1, -1, -1, -1, val, str, sizeof str - 1,       \
      0, 0, 0, res, 0 }

    //    +----------------------------- controlled sequence
    //    |              +-------------- character to be found
    //    |              |       +------ expected result  
    //    |              |       |                           
    //    |              |       |                
    //    V              V       V                 
    TEST ("ab",          'c',    0),   

    TEST ("",            'a', NPOS),  
    TEST ("",           '\0', NPOS),   

    TEST ("\0",         '\0', NPOS),   
    TEST ("\0\0",       '\0', NPOS),  
    TEST ("\0",          'a',    0),   
    TEST ("a\0",         'a',    1),   

    TEST ("e\0cb\0\0g", '\0',    0), 
    TEST ("\0ecb\0\0g", '\0',    1),
    TEST ("bbb\0cb\0e",  'b',    3),    
    TEST ("\0ecb\0\0g",  'a',    0),   
    TEST ("\0cbge\0\0", '\0',    1),    

    TEST (LSTR,          'x', NPOS),  
    TEST (LSTR,         '\0',    0),   
    TEST (LSTR,          'a',    0),

    TEST ("last test",   't',    0)    
};

/**************************************************************************/

// used to exercise 
// find_first_not_of (value_type, size_type)
static const TestCase val_size_test_cases [] = {

#undef TEST
#define TEST(str, val, off, res)                                     \
    { __LINE__, off, -1, -1, -1, val, str, sizeof str - 1,           \
      0, 0, 0, res, 0 }

    //    +------------------------------ controlled sequence
    //    |              +--------------- character to be found
    //    |              |     +--------- find_first_not_of() off argument
    //    |              |     |     +--- expected result  
    //    |              |     |     |                              
    //    |              |     |     |               
    //    V              V     V     V                   
    TEST ("ab",          'c',  0,    0),    

    TEST ("",            'a',  0, NPOS),   
    TEST ("",           '\0',  0, NPOS),    

    TEST ("\0",         '\0',  1, NPOS),    
    TEST ("\0",          'a',  0,    0),   
    TEST ("\0\0",       '\0',  1, NPOS),    
    TEST ("\0\0",        'a',  3, NPOS),  
    TEST ("\0\0",        'a',  1,    1),
    TEST ("\0\0",       '\0',  3, NPOS),    

    TEST ("e\0cb\0\0g", '\0',  1,    2),    
    TEST ("ecb\0\0\0g", '\0',  3,    6),    
    TEST ("e\0cbg\0\0", '\0',  5, NPOS),    
    TEST ("eb\0\0\0cg",  'b',  1,    2),    
    TEST ("e\0\0\0bbb",  'b',  4, NPOS),    
    TEST ("e\0cb\0\0g",  'a',  0,    0),   
    TEST ("\0cbge\0\0", '\0',  0,    1),   
    TEST ("\0\0\0cbge", '\0',  0,    3),  
    TEST ("\0cbge\0\0", '\0',  9, NPOS),        

    TEST (LSTR,          'x',  0, NPOS),   
    TEST (LSTR,          'x',  5, NPOS),   
    TEST (LSTR,         '\0',  0,    0),    
    TEST (LSTR,          'a',  3,    3),     
    TEST (LSTR,          'x', LLEN - 1, NPOS),
    TEST (LSTR,          'x', LLEN - 2, NPOS),

    TEST ("last test",   't',  0,    0)      
};

/**************************************************************************/

template <class charT, class Traits>
void test_find_first_not_of (charT, Traits*,  
                             OverloadId      which,
                             const TestCase &cs)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::const_iterator          ConstStringIter;

    static charT wstr [LLEN];
    static charT warg [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (warg, cs.arg, cs.arg_len);

    const TestString s_str (wstr, cs.str_len);
    const TestString s_arg (warg, cs.arg_len);

    std::size_t res = 0;
    std::size_t exp_res = NPOS != cs.nres ? cs.nres : TestString::npos;

    const std::size_t     ssize    = s_str.size ();
    const std::size_t     capacity = s_str.capacity ();
    const ConstStringIter begin    = s_str.begin ();

    const charT* const ptr_arg = cs.arg ? warg : s_str.c_str ();
    const TestString&  str_arg = cs.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (cs.val), (charT*)0);

    std::size_t size = cs.size >= 0 ? cs.size : s_arg.max_size () + 1;

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    if (1 == cs.bthrow)
        expected = exceptions [2];

    const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (cs.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    try {
    switch (which) {
        case FindFirstNotOf (ptr): {
            res = s_str.find_first_not_of (ptr_arg);
            break;
        }

        case FindFirstNotOf (str): {
            res = s_str.find_first_not_of (str_arg);
            break;
        }

        case FindFirstNotOf (ptr_size): {
            res = s_str.find_first_not_of (ptr_arg, cs.off);
            break;
        }

        case FindFirstNotOf (ptr_size_size): {
            res = s_str.find_first_not_of (ptr_arg, cs.off, size);
            break;
        }

        case FindFirstNotOf (str_size): {
            res = s_str.find_first_not_of (str_arg, cs.off);
            break;
        }

        case FindFirstNotOf (val): {
            res = s_str.find_first_not_of (arg_val);
            break;
        }

        case FindFirstNotOf (val_size): {
            res = s_str.find_first_not_of (arg_val, cs.off);
            break;
        }

        default:
            RW_ASSERT ("test logic error: unknown find_first_not_of overload");
            return;
        }

        // verify the returned value
        rw_assert (exp_res == res, 0, cs.line,
                   "line %d. %{$FUNCALL} == %{?}%zu%{;}%{?}npos%{;}, "
                   "got %{?}%zu%{;}%{?}npos%{;}", 
                   __LINE__, NPOS != cs.nres, exp_res, NPOS == cs.nres, 
                   TestString::npos != res, res, TestString::npos == res);
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    catch (const std::length_error &ex) {
        caught = exceptions [2];
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

        rw_assert (s_str.size () == ssize, 0, cs.line,
                   "line %d: %{$FUNCALL}: size unexpectedly changed "
                   "from %zu to %zu after an exception",
                   __LINE__, ssize, s_str.size ());

        rw_assert (s_str.capacity () == capacity, 0, cs.line,
                   "line %d: %{$FUNCALL}: capacity unexpectedly "
                   "changed from %zu to %zu after an exception",
                   __LINE__, capacity, s_str.capacity ());

        rw_assert (s_str.begin () == begin, 0, cs.line,
                   "line %d: %{$FUNCALL}: begin() unexpectedly "
                   "changed from after an exception by %d",
                   __LINE__, s_str.begin () - begin);
    }
    else if (-1 != cs.bthrow) {
        rw_assert (caught == expected, 0, cs.line,
                   "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                   "%{:}unexpectedly caught %s%{;}",
                   __LINE__, 0 != expected, expected, caught, caught);
    }

#else // if defined (_RWSTD_NO_EXCEPTIONS)

    _RWSTD_UNUSED (ssize);
    _RWSTD_UNUSED (capacity);

#endif   // _RWSTD_NO_EXCEPTIONS
}

/**************************************************************************/

DEFINE_TEST_DISPATCH (test_find_first_not_of);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::find_first_not_of_ ## tag,               \
        tag ## _test_cases,                                     \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (ptr),
        TEST (str),
        TEST (ptr_size),
        TEST (ptr_size_size),
        TEST (str_size),
        TEST (val),
        TEST (val_size)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return StringMembers::run_test (argc, argv, __FILE__,
                                    "lib.string.find.first.not.of",
                                    test_find_first_not_of, tests, test_count);
}
