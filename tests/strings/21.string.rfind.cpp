/***************************************************************************
 *
 * 21.string.rfind.cpp - string test exercising lib.string.rfind
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

#define RFind(which)    StringMembers::rfind_ ## which

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
// rfind (const value_type*)
static const TestCase ptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res)                                            \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, arg,          \
      sizeof arg - 1, 0, res, 0 }

    //    +----------------------------------- controlled sequence
    //    |             +--------------------- sequence to be found
    //    |             |                +---- expected result 
    //    |             |                |                               
    //    |             |                |                     
    //    V             V                V        
    TEST ("ab",         "a",             0),   

    TEST ("",           "",              0),   
    TEST ("",           "\0",            0),    
    TEST ("",           "a",          NPOS),   

    TEST ("\0",         "",              1),    
    TEST ("\0",         "\0",            1),   
    TEST ("\0",         "a",          NPOS),   

    TEST ("bbcdefghij", "a",          NPOS),    
    TEST ("abcdefghij", "a",             0),  
    TEST ("abcdefghij", "f",             5),  
    TEST ("abcdefghij", "j",             9),  

    TEST ("edfcbbhjig", "cba",        NPOS),    
    TEST ("edfcbahjig", "cba",           3),    
    TEST ("edfcbahcba", "cba",           7),   
    TEST ("cbacbahjig", "cba",           3),  

    TEST ("e\0cb\0\0g", "b\0\0g",        3),    
    TEST ("e\0cb\0\0g", "ecb",        NPOS),    
    TEST ("\0cb\0\0ge", "\0\0ge",        7),   
    TEST ("\0cb\0\0ge", "cb\0",          1),   
    TEST ("e\0cbg\0\0", "bg",            3),    
    TEST ("e\0cbg\0\0", "cba",        NPOS),  

    TEST ("bcbedfbjih", "a",          NPOS),   
    TEST ("bcaedfajih", "a",             6),    
    TEST ("bcedfaajih", "a",             6),    
    TEST ("bcaaedfaji", "a",             7),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",    0),     
    TEST ("aaaaabaaaa", "aaaaaaaaaa", NPOS),
    TEST ("aaaaabaaaa", "aaaaa",         0), 
    TEST ("aaaabaaaaa", "aaaaa",         5), 
    TEST ("aaaabaaaaa", "aaaa",          6),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",     1),    
    TEST ("aaaaaaaaa",  "aaaaaaaaaa", NPOS),    

    TEST (LSTR,         "",       LLEN - 1),     
    TEST (LSTR,         "a",          NPOS),  
    TEST (LSTR,         "x",      LLEN - 2),    
    TEST (LSTR,         "xxx",    LLEN - 4),    
    TEST (LSTR,         "xxa",        NPOS),   
    TEST ("abc",        LSTR,         NPOS),    
    TEST ("xxxxxxxxxx", LSTR,         NPOS),  

    TEST ("abcdefghij", 0,               0),      
    TEST ("\0cb\0\0ge", 0,               7),       
    TEST (LSTR,         0,               0),       

    TEST ("last test",  "test",          5)       
};

/**************************************************************************/

// used to exercise 
// rfind (const basic_string&)
static const TestCase str_test_cases [] = {

#undef TEST     
#define TEST(str, arg, res)                                            \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, arg,          \
      sizeof arg - 1, 0, res, 0 }

    //    +------------------------------------ controlled sequence
    //    |             +---------------------- sequence to be found
    //    |             |                +----- expected result 
    //    |             |                |                               
    //    |             |                |                     
    //    V             V                V        
    TEST ("ab",         "a",             0),   

    TEST ("",           "",              0),   
    TEST ("",           "\0",         NPOS),   
    TEST ("",           "a",          NPOS),  

    TEST ("\0",         "",              1),    
    TEST ("\0",         "\0",            0),   
    TEST ("\0",         "\0\0",       NPOS),   
    TEST ("\0",         "a",          NPOS),   
    TEST ("\0a",        "a\0",        NPOS),   

    TEST ("bbcdefghij", "a",          NPOS),    
    TEST ("abcdefghij", "a",             0),   
    TEST ("abcdefghij", "f",             5),   
    TEST ("abcdefghij", "j",             9),   

    TEST ("edfcbbhjig", "cba",        NPOS),   
    TEST ("edfcbahjig", "cba",           3),    
    TEST ("edfcbahcba", "cba",           7),  
    TEST ("cbacbahjig", "cba",           3),   

    TEST ("e\0cb\0\0g", "b\0\0g",        3),    
    TEST ("e\0cb\0\0g", "ecb",        NPOS),    
    TEST ("\0cb\0\0ge", "\0\0ge",        3),  
    TEST ("\0cb\0\0ge", "cb\0",          1),  
    TEST ("\0cb\0\0ge", "cb\0a",      NPOS),    
    TEST ("e\0cbg\0\0", "bg",            3),   
    TEST ("e\0cbg\0\0", "\0\0",          5),
    TEST ("\0\0cg\0\0", "\0\0",          4), 
    TEST ("e\0cbg\0\0", "\0\0a",      NPOS),   
    TEST ("e\0cbg\0\0", "cba",        NPOS),  

    TEST ("bcbedfbjih", "a",          NPOS),    
    TEST ("bcaedfajih", "a",             6),     
    TEST ("bcedfaajih", "a",             6),   
    TEST ("bcaaedfaji", "a",             7),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",    0),   
    TEST ("aaaaabaaaa", "aaaaaaaaaa", NPOS),    
    TEST ("aaaabaaaaa", "aaaaa",         5), 
    TEST ("aaaaabaaaa", "aaaa",          6),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",     1), 
    TEST ("aaaaaaaaa",  "aaaaaaaaaa", NPOS),  

    TEST (LSTR,         "",       LLEN - 1),    
    TEST (LSTR,         "a",          NPOS),    
    TEST (LSTR,         "x",      LLEN - 2),  
    TEST (LSTR,         "xxx",    LLEN - 4),    
    TEST (LSTR,         "xxa",        NPOS),    
    TEST ("abc",        LSTR,         NPOS),   
    TEST ("xxxxxxxxxx", LSTR,         NPOS),    

    TEST ("abcdefghij", 0,               0),    
    TEST ("\0cb\0\0ge", 0,               0),    
    TEST (LSTR,         0,               0),     

    TEST ("last test",  "test",          5)      
};

/**************************************************************************/

// used to exercise 
// rfind (const value_type*, size_type)
static const TestCase ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)                                            \
    { __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1, arg,              \
      sizeof arg - 1, 0, res, 0 }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- sequence to be found
    //    |            |               +--------- rfind() off argument 
    //    |            |               |     +--- expected result  
    //    |            |               |     |                             
    //    |            |               |     |             
    //    V            V               V     V                 
    TEST ("ab",        "a",            0,    0),   

    TEST ("",           "",            0,    0),    
    TEST ("",           "\0",          0,    0),    
    TEST ("",           "a",           0, NPOS),    

    TEST ("\0",         "",            0,    0),  
    TEST ("\0",         "",            1,    1), 
    TEST ("\0",         "\0",          0,    0),   
    TEST ("\0",         "\0",          1,    1),    
    TEST ("\0",         "a",           0, NPOS),    

    TEST ("bbcdefghij", "a",           0, NPOS),    
    TEST ("abcdefghij", "a",           0,    0),   
    TEST ("abcdefghij", "a",           2,    0),    
    TEST ("abcdefghij", "f",           2, NPOS),   
    TEST ("abcdefghij", "f",           7,    5),    
    TEST ("abcdefghij", "j",           9,    9),  

    TEST ("edfcbbhjig", "cba",         0, NPOS),    
    TEST ("edfcbahjig", "cba",         1, NPOS),     
    TEST ("edfcbahjig", "cba",         5,    3),    
    TEST ("edfcbahcba", "cba",         1, NPOS), 
    TEST ("edfcbahcba", "cba",         9,    7), 
    TEST ("edfcbahcba", "cba",         5,    3),    
    TEST ("cbacbahjig", "cba",         5,    3),    

    TEST ("e\0cb\0\0g", "b\0\0g",      5,    3),    
    TEST ("e\0cb\0\0g", "b\0\0g",      4,    3),    
    TEST ("e\0cb\0\0g", "ecb",         7, NPOS),     
    TEST ("\0cb\0\0ge", "\0\0ge",      6,    6), 
    TEST ("\0cb\0\0ge", "cb\0",        0, NPOS),     
    TEST ("\0cb\0\0ge", "cb\0",        1,    1),
    TEST ("\0cb\0\0ge", "cb\0",        2,    1),
    TEST ("e\0cbg\0\0", "bg",          1, NPOS),  
    TEST ("e\0cbg\0\0", "bg",          5,    3), 
    TEST ("e\0cbg\0\0", "cba",         7, NPOS),    

    TEST ("bcbedfbjih", "a",           0, NPOS),    
    TEST ("bcaedfajih", "a",           8,    6),     
    TEST ("bcedfaajih", "a",           6,    6),    
    TEST ("bcaaedfaji", "a",           8,    7),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0,    0),    
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  8,    0),    
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  9, NPOS),    
    TEST ("aaaabaaaaa", "aaaaa",       9,    5),    
    TEST ("aaaabaaaaa", "aaaaa",       6,    5),    
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   9,    1),     
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   8,    1),   
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   7,    1),    
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  8, NPOS),    

    TEST (LSTR,         "",     LLEN - 1,  LLEN - 1),    
    TEST (LSTR,         "a",           0, NPOS),     
    TEST (LSTR,         "x",    LLEN - 1,  LLEN - 2),   
    TEST (LSTR,         "xxx",  LLEN - 2,  LLEN - 4),
    TEST (LSTR,         "xxx",         0,    0),
    TEST (LSTR,         "xxx", LLEN - 10,  LLEN - 10),    
    TEST (LSTR,         "xxa", LLEN - 10, NPOS),     
    TEST ("abc",        LSTR,          2, NPOS),    
    TEST ("xxxxxxxxxx", LSTR,          6, NPOS),   

    TEST (LSTR,         "xxx",         3,    3), 
    TEST (LSTR,         "xxx",         2,    2),     

    TEST ("abcdefghij", 0,             0,    0),    
    TEST ("abcdefghij", 0,             1,    0),   
    TEST ("\0cb\0\0ge", 0,             5,    5),    
    TEST (LSTR,         0,             0,    0),  
    TEST (LSTR,         0,             1,    0),    

    TEST ("",           "",            1,    0),  
    TEST ("abcdefghij", "abc",        10,    0),   
    TEST ("abcdefghij", "cba",        10, NPOS), 

    TEST ("last test", "test",         9,    5)      
};

/**************************************************************************/

// used to exercise 
// rfind (const value_type*, size_type, size_type)
static const TestCase ptr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, size, res, bthrow)                        \
    { __LINE__, off, size, -1, -1, -1, str, sizeof str - 1, arg,      \
      sizeof arg - 1, 0, res, bthrow }

    //    +--------------------------------------- controlled sequence
    //    |            +-------------------------- sequence to be found
    //    |            |               +---------- rfind() off argument 
    //    |            |               |   +------ rfind() n argument 
    //    |            |               |   |     +-- expected result  
    //    |            |               |   |     |   exception info 
    //    |            |               |   |     |   |   0 - no exception        
    //    |            |               |   |     |   |   1 - length_error  
    //    |            |               |   |     |   |                           
    //    |            |               |   |     |   +----+             
    //    V            V               V   V     V        V             
    TEST ("ab",        "a",            0,  1,    0,       0),

    TEST ("",           "",            0,  0,    0,       0),
    TEST ("",           "\0",          0,  0,    0,       0),
    TEST ("",           "\0",          0,  1, NPOS,       0),
    TEST ("",           "a",           0,  0,    0,       0),
    TEST ("",           "a",           0,  1, NPOS,       0),

    TEST ("\0",         "",            0,  0,    0,       0),
    TEST ("\0",         "\0",          0,  1,    0,       0),
    TEST ("\0",         "\0",          1,  1,    0,       0),
    TEST ("\0\0",       "\0\0",        1,  1,    1,       0),
    TEST ("\0",         "a",           0,  1, NPOS,       0),

    TEST ("edfcbbhjig", "cba",         0,  3, NPOS,       0),
    TEST ("edfcbbhjig", "cba",         0,  2, NPOS,       0),
    TEST ("edfcbbhjig", "cba",         9,  2,    3,       0),
    TEST ("edfcbahjig", "cba",         8,  3,    3,       0),
    TEST ("edfcbahjig", "cba",         2,  3, NPOS,       0),
    TEST ("edfcbahjig", "cba",         2,  1, NPOS,       0),
    TEST ("edfcbahcba", "cba",         8,  3,    7,       0),
    TEST ("edfcbehcba", "cba",         8,  2,    7,       0),
    TEST ("edfcbahcba", "cba",         9,  3,    7,       0),
    TEST ("cbacbahjig", "cba",         5,  3,    3,       0),
    TEST ("cbacbahjig", "cba",         2,  3,    0,       0),
    TEST ("cbacbahjcg", "cba",         2,  1,    0,       0),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,  4, NPOS,       0),
    TEST ("e\0cb\0\0g", "b\0\0g",      7,  4,    3,       0),
    TEST ("e\0cb\0\0b", "b\0\0g",      4,  1,    3,       0),
    TEST ("\0b\0\0gb\0","b\0\0g",      7,  2,    5,       0),
    TEST ("\0b\0\0gb\0","b\0\0g",      2,  2,    1,       0),
    TEST ("\0b\0\0gb\0","b\0\0g",      7,  3,    1,       0),
    TEST ("e\0cb\0\0g", "ecb",         7,  2, NPOS,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  4,    3,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      2,  0,    2,       0),
    TEST ("\0cb\0\0ge", "cb\0",        1,  3,    1,       0),
    TEST ("e\0cbg\0\0", "bg",          1,  2, NPOS,       0),
    TEST ("e\0cbg\0\0", "cba",         7,  3, NPOS,       0),
    TEST ("e\0cbg\0\0", "cba",         7,  2,    2,       0),

    TEST ("e\0a\0",     "e\0a\0\0",    3,  4,    0,       0),
    TEST ("e\0a\0",     "e\0a\0\0",    3,  5, NPOS,       0),
    TEST ("ee\0a\0",    "e\0a\0\0",    4,  4,    1,       0),
    TEST ("ee\0a\0",    "e\0a\0\0",    4,  5, NPOS,       0),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  9, 10,    0,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1, 10,    0,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1,  9,    1,       0),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  0, 10, NPOS,       0),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  1,  4,    1,       0),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  7,  4,    6,       0),
    TEST ("aaaabaaaaa", "aaaaa",       0,  5, NPOS,       0),
    TEST ("aaaabaaaaa", "aaaaa",       0,  4,    0,       0),
    TEST ("aaaabaaaaa", "aaaaa",       6,  5,    5,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0,  9,    0,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   1,  9,    1,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   2,  9,    1,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   7,  8,    2,       0),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  9, 10, NPOS,       0),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  8,  7,    2,       0),

    TEST (LSTR,         "",     LLEN - 1,  0, LLEN - 1,   0),
    TEST (LSTR,         "a",    LLEN - 1,  1, NPOS,       0),
    TEST (LSTR,         "x",    LLEN - 1,  1, LLEN - 2,   0),
    TEST (LSTR,         "xxx", LLEN - 10,  3, LLEN - 10,  0),
    TEST (LSTR,         "xxa", LLEN - 10,  3, NPOS,       0),
    TEST (LSTR,         "xxa",  LLEN - 1,  2, LLEN - 3,   0),
    TEST ("abc",        LSTR,          2, 10, NPOS,       0),
    TEST ("xxxxxxxxxx", LSTR,          0, LLEN - 1, NPOS, 0),
    TEST ("xxxxxxxxxx", LSTR,          2,  4,    2,       0),

    TEST (LSTR,         "xxx",  LLEN - 4,  3, LLEN - 4,   0),
    TEST (LSTR,         "xxx",  LLEN - 3,  3, LLEN - 4,   0),
    TEST (LSTR,         "xxx",  LLEN - 3,  2, LLEN - 3,   0),

    TEST ("abcdefghij", 0,             0,  9,    0,       0),
    TEST ("abcdefghij", 0,             1,  9,    0,       0),
    TEST ("\0cb\0\0ge", 0,             5,  7,    0,       0),
    TEST ("\0cb\0ge\0", 0,             6,  1,    6,       0),
    TEST (LSTR,         0,             0, LLEN - 1,  0,   0),
    TEST (LSTR,         0,             1, LLEN - 1,  0,   0),
    TEST (LSTR,         0,             5, LLEN - 6,  5,   0),

    TEST ("",           "",            1,  0,    0,       0),
    TEST ("abcdefghij", "abc",        10,  3,    0,       0),
    TEST ("abcdefghij", "cba",        10,  1,    2,       0),

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("",           "cba",         0, -1,    0,       1),
    TEST ("abcdefghij", "cba",         0, -1,    0,       1),
    TEST (LSTR,         "xxx",         0, -1,    0,       1),
    TEST ("abcdefghij", LSTR,          0, -1,    0,       1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last test", "test",         9,  4,    5,       0)
};

/**************************************************************************/

// used to exercise 
// rfind (const basic_string&, size_type)
static const TestCase str_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)                                    \
    { __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1, arg,      \
      sizeof arg - 1, 0, res, 0 }

    //    +--------------------------------------- controlled sequence
    //    |             +------------------------- sequence to be found
    //    |             |              +---------- rfind() off argument
    //    |             |              |     +---- expected result 
    //    |             |              |     |                          
    //    |             |              |     |           
    //    V             V              V     V                
    TEST ("ab",         "a",           0,    0),     

    TEST ("",           "",            0,    0),     
    TEST ("",           "\0",          0, NPOS),    
    TEST ("",           "a",           0, NPOS),    

    TEST ("\0",         "",            0,    0),    
    TEST ("\0",         "\0",          0,    0),   
    TEST ("\0",         "\0",          1,    0),    
    TEST ("\0\0",       "\0",          2,    1),   
    TEST ("\0",         "a",           0, NPOS),    

    TEST ("bbcdefghij", "a",           0, NPOS),    
    TEST ("abcdefghij", "a",           0,    0),   
    TEST ("abcdefghij", "a",           9,    0),   
    TEST ("abcdefghij", "f",           2, NPOS),    
    TEST ("abcdefghij", "f",           7,    5),   
    TEST ("abcdefghij", "j",           9,    9),    

    TEST ("edfcbbhjig", "cba",         9, NPOS),    
    TEST ("edfcbahjig", "cba",         1, NPOS),    
    TEST ("edfcbahjig", "cba",         4,    3),   
    TEST ("edfcbahcba", "cba",         9,    7),   
    TEST ("edfcbahcba", "cba",         6,    3),   
    TEST ("cbacbahjig", "cba",         5,    3),   

    TEST ("e\0cb\0\0g", "b\0\0g",      7,    3),   
    TEST ("e\0cb\0\0g", "b\0\0g",      2, NPOS),    
    TEST ("e\0cb\0\0g", "ecb",         7, NPOS),   
    TEST ("\0cb\0\0ge", "\0\0ge",      6,    3),   
    TEST ("\0cb\0\0ge", "\0\0ge",      1, NPOS),    
    TEST ("\0cb\0\0ge", "cb\0",        2,    1),    
    TEST ("\0cbg\0\0e", "cb\0",        0, NPOS),    
    TEST ("e\0cbg\0\0", "bg",          6,    3),   
    TEST ("e\0cbg\0\0", "cba",         7, NPOS),   

    TEST ("bcbedfbjih", "a",           9, NPOS),    
    TEST ("bcaedfajih", "a",           9,    6),   
    TEST ("bcedfaajih", "a",           6,    6),    
    TEST ("bcaaedfaji", "a",           5,    3),   

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0,    0),   
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  9,    0),    
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  9, NPOS),    
    TEST ("aaaabaaaaa", "aaaaa",       9,    5),    
    TEST ("aaaabaaaaa", "aaaaa",       3, NPOS),    
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   9,    1),    
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0,    0),    
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  8, NPOS),    

    TEST (LSTR,         "",     LLEN - 1,  LLEN - 1),    
    TEST (LSTR,         "a",    LLEN - 1, NPOS),    
    TEST (LSTR,         "x",    LLEN - 1,  LLEN - 2),   
    TEST (LSTR,         "xxx", LLEN - 10,  LLEN - 10),     
    TEST (LSTR,         "xxa", LLEN - 10, NPOS),    
    TEST ("abc",        LSTR,          2, NPOS),   
    TEST ("xxxxxxxxxx", LSTR,   LLEN - 1, NPOS),    

    TEST (LSTR,         "xxx",  LLEN - 1, LLEN - 4), 
    TEST (LSTR,         "xxx",  LLEN - 6, LLEN - 6),   

    TEST ("abcdefghij", 0,             9,    0),    
    TEST ("abcdefghij", 0,             1,    0),   
    TEST ("\0cb\0\0ge", 0,             5,    0),      
    TEST (LSTR,         0,             0,    0),    
    TEST (LSTR,         0,             1,    0),   

    TEST ("",           "",            1,    0),   
    TEST ("abcdefghij", "abc",        10,    0),  
    TEST ("abcdefghij", "cba",        10, NPOS),

    TEST ("last test",  "test",        9,    5)     
};

/**************************************************************************/

// used to exercise 
// rfind (value_type)
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
    TEST ("ab",          'a',    0),   

    TEST ("",            'a', NPOS),  
    TEST ("",           '\0', NPOS),   

    TEST ("\0",         '\0',    0),   
    TEST ("\0\0",       '\0',    1),  
    TEST ("\0",          'a', NPOS),   

    TEST ("e\0cb\0\0g", '\0',    5),    
    TEST ("e\0cb\0\0g",  'b',    3),    
    TEST ("e\0cb\0\0g",  'a', NPOS),   
    TEST ("\0cbge\0\0", '\0',    6),   
    TEST ("\0cbge\0\0",  'b',    2),   
    TEST ("\0cbge\0\0",  'a', NPOS),   

    TEST (LSTR,          'x', LLEN - 2),  
    TEST (LSTR,         '\0', NPOS),   
    TEST (LSTR,          'a', NPOS),  

    TEST ("last test",   't',    8)    
};

/**************************************************************************/

// used to exercise 
// rfind (value_type, size_type)
static const TestCase val_size_test_cases [] = {

#undef TEST
#define TEST(str, val, off, res)                                        \
    { __LINE__, off, -1, -1, -1, val, str, sizeof str - 1,              \
      0, 0, 0, res, 0 }

    //    +------------------------------ controlled sequence
    //    |              +--------------- character to be found
    //    |              |     +--------- rfind() off argument
    //    |              |     |     +--- expected result  
    //    |              |     |     |                              
    //    |              |     |     |               
    //    V              V     V     V                   
    TEST ("ab",          'a',  0,    0),    

    TEST ("",            'a',  0, NPOS),   
    TEST ("",           '\0',  0, NPOS),    

    TEST ("\0",         '\0',  1,    0),    
    TEST ("\0",          'a',  0, NPOS),   
    TEST ("\0\0",       '\0',  2,    1),    
    TEST ("\0\0",        'a',  3, NPOS),   
    TEST ("\0\0",       '\0',  3,    1),    

    TEST ("e\0cb\0\0g", '\0',  1,    1),    
    TEST ("e\0cb\0\0g", '\0',  5,    5),    
    TEST ("e\0cb\0\0g", '\0',  0, NPOS),    
    TEST ("e\0cb\0\0g",  'b',  1, NPOS),    
    TEST ("e\0cb\0\0g",  'b',  4,    3),    
    TEST ("e\0cb\0\0g",  'a',  6, NPOS),   
    TEST ("\0cbge\0\0", '\0',  0,    0),   
    TEST ("\0cbge\0\0", '\0',  5,    5),  
    TEST ("\0cbge\0\0", '\0',  9,    6),    
    TEST ("\0cbge\0\0",  'b',  5,    2),  
    TEST ("\0bgeb\0\0",  'b',  5,    4),   
    TEST ("\0cbge\0\0",  'a',  6, NPOS),    

    TEST (LSTR,          'x',  0,    0),
    TEST (LSTR,          'x',  5,    5),
    TEST (LSTR,          'x', LLEN - 1, LLEN - 2),      
    TEST (LSTR,         '\0', LLEN - 1, NPOS),    
    TEST (LSTR,          'a', LLEN - 3, NPOS),     
    TEST (LSTR,          'x', LLEN - 2, LLEN - 2),
    TEST (LSTR,          'x', LLEN + 9, LLEN - 2),

    TEST ("last test",   't',  9,    8)      
};

/**************************************************************************/

template <class charT, class Traits>
void test_rfind (charT, Traits*,  
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
        case RFind (ptr): {
            res = s_str.rfind (ptr_arg);
            break;
        }

        case RFind (str): {
            res = s_str.rfind (str_arg);
            break;
        }

        case RFind (ptr_size): {
            res = s_str.rfind (ptr_arg, cs.off);
            break;
        }

        case RFind (ptr_size_size): {
            res = s_str.rfind (ptr_arg, cs.off, size);
            break;
        }

        case RFind (str_size): {
            res = s_str.rfind (str_arg, cs.off);
            break;
        }

        case RFind (val): {
            res = s_str.rfind (arg_val);
            break;
        }

        case RFind (val_size): {
            res = s_str.rfind (arg_val, cs.off);
            break;
        }

        default:
            RW_ASSERT ("test logic error: unknown rfind overload");
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

DEFINE_TEST_DISPATCH (test_rfind);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::rfind_ ## tag,                           \
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
                                    "lib.string.rfind",
                                    test_rfind, tests, test_count);
}

