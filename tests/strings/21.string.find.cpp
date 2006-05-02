/***************************************************************************
 *
 * 21.string.find.cpp - string test exercising lib.string.find
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


#define Find(which)    StringMembers::find_ ## which

typedef StringMembers::OverloadId OverloadId;
typedef StringMembers::TestCase   TestCase;
typedef StringMembers::Test       Test;
typedef StringMembers::Function   MemFun;

/**************************************************************************/

// for convenience and brevity
#define LSTR      StringMembers::long_string
#define LLEN      StringMembers::long_string_len

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise 
// find (const value_type*)
static const TestCase ptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res)                                    \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, arg,  \
      sizeof arg - 1, 0, 0, res, 0 }

    //    +----------------------------------- controlled sequence
    //    |             +--------------------- sequence to be found
    //    |             |              +------ expected result 
    //    |             |              |                               
    //    |             |              |                     
    //    V             V              V        
    TEST ("ab",         "a",           0),   

    TEST ("",           "",            0),   
    TEST ("",           "\0",          0),    
    TEST ("",           "a",          -1),   

    TEST ("\0",         "",            0),    
    TEST ("\0",         "\0",          0),   
    TEST ("\0",         "a",          -1),   

    TEST ("bbcdefghij", "a",          -1),    
    TEST ("abcdefghij", "a",           0),  
    TEST ("abcdefghij", "f",           5),  
    TEST ("abcdefghij", "j",           9),  

    TEST ("edfcbbhjig", "cba",        -1),    
    TEST ("edfcbahjig", "cba",         3),    
    TEST ("edfcbahcba", "cba",         3),   
    TEST ("cbacbahjig", "cba",         0),  

    TEST ("e\0cb\0\0g", "b\0\0g",      3),    
    TEST ("e\0cb\0\0g", "ecb",        -1),    
    TEST ("\0cb\0\0ge", "\0\0ge",      0),   
    TEST ("\0cb\0\0ge", "cb\0",        1),   
    TEST ("e\0cbg\0\0", "bg",          3),    
    TEST ("e\0cbg\0\0", "cba",        -1),  

    TEST ("bcbedfbjih", "a",          -1),   
    TEST ("bcaedfajih", "a",           2),    
    TEST ("bcedfaajih", "a",           5),    
    TEST ("bcaaedfaji", "a",           2),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0),     
    TEST ("aaaaabaaaa", "aaaaaaaaaa", -1),   
    TEST ("aaaabaaaaa", "aaaaa",       5),     
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0),    
    TEST ("aaaaaaaaa",  "aaaaaaaaaa", -1),    

    TEST (LSTR,         "",            0),     
    TEST (LSTR,         "a",          -1),  
    TEST (LSTR,         "x",           0),    
    TEST (LSTR,         "xxx",         0),    
    TEST (LSTR,         "xxa",        -1),   
    TEST ("abc",        LSTR,         -1),    
    TEST ("xxxxxxxxxx", LSTR,         -1),  

    TEST ("abcdefghij", 0,             0),      
    TEST ("\0cb\0\0ge", 0,             0),       
    TEST (LSTR,         0,             0),       

    TEST ("last test",  "test",        5)       
};

/**************************************************************************/

// used to exercise 
// find (const basic_string&)
static const TestCase str_test_cases [] = {

#undef TEST     
#define TEST(str, arg, res)                                    \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, arg,  \
      sizeof arg - 1, 0, 0, res, 0 }

    //    +------------------------------------ controlled sequence
    //    |             +---------------------- sequence to be found
    //    |             |              +------- expected result 
    //    |             |              |                               
    //    |             |              |                     
    //    V             V              V        
    TEST ("ab",         "a",           0),   

    TEST ("",           "",            0),   
    TEST ("",           "\0",         -1),   
    TEST ("",           "a",          -1),  

    TEST ("\0",         "",            0),    
    TEST ("\0",         "\0",          0),   
    TEST ("\0",         "\0\0",       -1),   
    TEST ("\0",         "a",          -1),   
    TEST ("\0a",        "a\0",        -1),   

    TEST ("bbcdefghij", "a",          -1),    
    TEST ("abcdefghij", "a",           0),   
    TEST ("abcdefghij", "f",           5),   
    TEST ("abcdefghij", "j",           9),   

    TEST ("edfcbbhjig", "cba",        -1),   
    TEST ("edfcbahjig", "cba",         3),    
    TEST ("edfcbahcba", "cba",         3),  
    TEST ("cbacbahjig", "cba",         0),   

    TEST ("e\0cb\0\0g", "b\0\0g",      3),    
    TEST ("e\0cb\0\0g", "ecb",        -1),    
    TEST ("\0cb\0\0ge", "\0\0ge",      3),  
    TEST ("\0cb\0\0ge", "cb\0",        1),  
    TEST ("\0cb\0\0ge", "cb\0a",      -1),    
    TEST ("e\0cbg\0\0", "bg",          3),   
    TEST ("e\0cbg\0\0", "\0\0",        5),  
    TEST ("e\0cbg\0\0", "\0\0a",      -1),   
    TEST ("e\0cbg\0\0", "cba",        -1),  

    TEST ("bcbedfbjih", "a",          -1),    
    TEST ("bcaedfajih", "a",           2),     
    TEST ("bcedfaajih", "a",           5),   
    TEST ("bcaaedfaji", "a",           2),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0),   
    TEST ("aaaaabaaaa", "aaaaaaaaaa", -1),    
    TEST ("aaaabaaaaa", "aaaaa",       5),  
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0),    
    TEST ("aaaaaaaaa",  "aaaaaaaaaa", -1),  

    TEST (LSTR,         "",            0),    
    TEST (LSTR,         "a",          -1),    
    TEST (LSTR,         "x",           0),  
    TEST (LSTR,         "xxx",         0),    
    TEST (LSTR,         "xxa",        -1),    
    TEST ("abc",        LSTR,         -1),   
    TEST ("xxxxxxxxxx", LSTR,         -1),    

    TEST ("abcdefghij", 0,             0),    
    TEST ("\0cb\0\0ge", 0,             0),    
    TEST (LSTR,         0,             0),     

    TEST ("last test",  "test",        5)      
};

/**************************************************************************/

// used to exercise 
// find (const value_type*, size_type)
static const TestCase ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)                                    \
    { __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1, arg,      \
      sizeof arg - 1, 0, 0, res, 0 }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- sequence to be found
    //    |            |               +--------- find() off argument 
    //    |            |               |   +----- expected result  
    //    |            |               |   |                             
    //    |            |               |   |             
    //    V            V               V   V                 
    TEST ("ab",        "a",            0,  0),   

    TEST ("",           "",            0,  0),    
    TEST ("",           "\0",          0,  0),    
    TEST ("",           "a",           0, -1),    

    TEST ("\0",         "",            0,  0),    
    TEST ("\0",         "\0",          0,  0),   
    TEST ("\0",         "\0",          1,  1),    
    TEST ("\0",         "a",           0, -1),    

    TEST ("bbcdefghij", "a",           0, -1),    
    TEST ("abcdefghij", "a",           0,  0),   
    TEST ("abcdefghij", "a",           2, -1),    
    TEST ("abcdefghij", "f",           2,  5),   
    TEST ("abcdefghij", "f",           7, -1),    
    TEST ("abcdefghij", "j",           9,  9),  

    TEST ("edfcbbhjig", "cba",         0, -1),    
    TEST ("edfcbahjig", "cba",         1,  3),     
    TEST ("edfcbahjig", "cba",         4, -1),    
    TEST ("edfcbahcba", "cba",         1,  3),    
    TEST ("edfcbahcba", "cba",         5,  7),    
    TEST ("cbacbahjig", "cba",         5, -1),    

    TEST ("e\0cb\0\0g", "b\0\0g",      0,  3),    
    TEST ("e\0cb\0\0g", "b\0\0g",      4, -1),    
    TEST ("e\0cb\0\0g", "ecb",         0, -1),     
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  6),  
    TEST ("\0cb\0\0ge", "cb\0",        1,  1),     
    TEST ("e\0cbg\0\0", "bg",          1,  3),   
    TEST ("e\0cbg\0\0", "cba",         0, -1),    

    TEST ("bcbedfbjih", "a",           0, -1),    
    TEST ("bcaedfajih", "a",           1,  2),     
    TEST ("bcedfaajih", "a",           6,  6),    
    TEST ("bcaaedfaji", "a",           5,  7),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0,  0),    
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1, -1),    
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  0, -1),    
    TEST ("aaaabaaaaa", "aaaaa",       0,  5),    
    TEST ("aaaabaaaaa", "aaaaa",       6, -1),   
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0,  0),     
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   1,  1),   
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   2, -1),    
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  0, -1),    

    TEST (LSTR,         "",            0,  0),    
    TEST (LSTR,         "a",           0, -1),     
    TEST (LSTR,         "x",           0,  0),     
    TEST (LSTR,         "xxx",        10, 10),    
    TEST (LSTR,         "xxa",        10, -1),     
    TEST ("abc",        LSTR,          2, -1),    
    TEST ("xxxxxxxxxx", LSTR,          0, -1),   

    TEST (LSTR,         "xxx",  LLEN - 4, LLEN - 4), 
    TEST (LSTR,         "xxx",  LLEN - 3, -1),     

    TEST ("abcdefghij", 0,             0,  0),    
    TEST ("abcdefghij", 0,             1, -1),   
    TEST ("\0cb\0\0ge", 0,             5,  5),    
    TEST (LSTR,         0,             0,  0),  
    TEST (LSTR,         0,             1, -1),    

    TEST ("",           "",            1, -1),  
    TEST ("abcdefghij", "abc",        10, -1),   
    TEST ("abcdefghij", "cba",        10, -1),    

    TEST ("last test", "test",         0,  5)      
};

/**************************************************************************/

// used to exercise 
// find (const value_type*, size_type, size_type)
static const TestCase ptr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, size, res, bthrow)                        \
    { __LINE__, off, size, -1, -1, -1, str, sizeof str - 1, arg,      \
      sizeof arg - 1, 0, 0, res, bthrow }

    //    +--------------------------------------- controlled sequence
    //    |            +-------------------------- sequence to be found
    //    |            |               +---------- find() off argument 
    //    |            |               |   +------ find() n argument 
    //    |            |               |   |   +-- expected result  
    //    |            |               |   |   |   exception info 
    //    |            |               |   |   |   |   0 - no exception        
    //    |            |               |   |   |   |   1 - length_error  
    //    |            |               |   |   |   |                           
    //    |            |               |   |   |   +----+             
    //    V            V               V   V   V        V             
    TEST ("ab",        "a",            0,  1,  0,       0),

    TEST ("",           "",            0,  0,  0,       0),
    TEST ("",           "\0",          0,  0,  0,       0),
    TEST ("",           "\0",          0,  1, -1,       0),
    TEST ("",           "a",           0,  0,  0,       0),
    TEST ("",           "a",           0,  1, -1,       0),

    TEST ("\0",         "",            0,  0,  0,       0),
    TEST ("\0",         "\0",          0,  1,  0,       0),
    TEST ("\0",         "\0",          1,  1, -1,       0),
    TEST ("\0\0",       "\0\0",        1,  1,  1,       0),
    TEST ("\0",         "a",           0,  1, -1,       0),

    TEST ("edfcbbhjig", "cba",         0,  3, -1,       0),
    TEST ("edfcbbhjig", "cba",         0,  2,  3,       0),
    TEST ("edfcbahjig", "cba",         1,  3,  3,       0),
    TEST ("edfcbahjig", "cba",         4,  3, -1,       0),
    TEST ("edfcbahjig", "cba",         4,  1, -1,       0),
    TEST ("edfcbahcba", "cba",         1,  3,  3,       0),
    TEST ("edfcbehcba", "cba",         1,  2,  3,       0),
    TEST ("edfcbahcba", "cba",         5,  3,  7,       0),
    TEST ("cbacbahjig", "cba",         5,  3, -1,       0),
    TEST ("cbacbahjcg", "cba",         5,  1,  8,       0),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,  4,  3,       0),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  4, -1,       0),
    TEST ("e\0cb\0\0b", "b\0\0g",      4,  1,  6,       0),
    TEST ("\0b\0\0gb\0","b\0\0g",      2,  2,  5,       0),
    TEST ("\0b\0\0gb\0","b\0\0g",      0,  2,  1,       0),
    TEST ("\0b\0\0gb\0","b\0\0g",      0,  3,  1,       0),
    TEST ("e\0cb\0\0g", "ecb",         0,  2, -1,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  4, -1,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  0,  6,       0),
    TEST ("\0cb\0\0ge", "cb\0",        1,  3,  1,       0),
    TEST ("e\0cbg\0\0", "bg",          1,  2,  3,       0),
    TEST ("e\0cbg\0\0", "cba",         0,  3, -1,       0),
    TEST ("e\0cbg\0\0", "cba",         0,  2,  2,       0),

    TEST ("e\0a\0",     "e\0a\0\0",    0,  4,  0,       0),
    TEST ("e\0a\0",     "e\0a\0\0",    0,  5, -1,       0),
    TEST ("ee\0a\0",    "e\0a\0\0",    1,  4,  1,       0),
    TEST ("ee\0a\0",    "e\0a\0\0",    1,  5, -1,       0),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0, 10,  0,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1, 10, -1,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1,  9,  1,       0),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  0, 10, -1,       0),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  1,  4,  1,       0),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  2,  4,  6,       0),
    TEST ("aaaabaaaaa", "aaaaa",       0,  5,  5,       0),
    TEST ("aaaabaaaaa", "aaaaa",       0,  4,  0,       0),
    TEST ("aaaabaaaaa", "aaaaa",       6,  5, -1,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0,  9,  0,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   1,  9,  1,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   2,  9, -1,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   2,  8,  2,       0),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  0, 10, -1,       0),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  0,  7,  0,       0),

    TEST (LSTR,         "",            0,  0,  0,       0),
    TEST (LSTR,         "a",           0,  1, -1,       0),
    TEST (LSTR,         "x",           0,  1,  0,       0),
    TEST (LSTR,         "xxx",        10,  3, 10,       0),
    TEST (LSTR,         "xxa",        10,  3, -1,       0),
    TEST (LSTR,         "xxa",        10,  2, 10,       0),
    TEST ("abc",        LSTR,          2, 10, -1,       0),
    TEST ("xxxxxxxxxx", LSTR,          0, LLEN - 1, -1, 0),
    TEST ("xxxxxxxxxx", LSTR,          2,  4,  2,       0),

    TEST (LSTR,         "xxx",  LLEN - 4,  3, LLEN - 4, 0),
    TEST (LSTR,         "xxx",  LLEN - 3,  3, -1,       0),
    TEST (LSTR,         "xxx",  LLEN - 3,  2, LLEN - 3, 0),

    TEST ("abcdefghij", 0,             0,  9,  0,       0),
    TEST ("abcdefghij", 0,             1,  9, -1,       0),
    TEST ("\0cb\0\0ge", 0,             5,  7, -1,       0),
    TEST ("\0cb\0ge\0", 0,             6,  1,  6,       0),
    TEST (LSTR,         0,             0, LLEN - 1,  0, 0),
    TEST (LSTR,         0,             1, LLEN - 1, -1, 0),
    TEST (LSTR,         0,             5, LLEN - 6,  5, 0),

    TEST ("",           "",            1,  0, -1,       0),
    TEST ("abcdefghij", "abc",        10,  3, -1,       0),
    TEST ("abcdefghij", "cba",        10,  1, -1,       0),

    TEST ("",           "cba",         0, -1,  0,       1),
    TEST ("abcdefghij", "cba",         0, -1,  0,       1),
    TEST (LSTR,         "xxx",         0, -1,  0,       1),
    TEST ("abcdefghij", LSTR,          0, -1,  0,       1),

    TEST ("last test", "test",         0,  4,  5,       0)
};

/**************************************************************************/

// used to exercise 
// find (const basic_string&, size_type)
static const TestCase str_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)                                    \
    { __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1, arg,      \
      sizeof arg - 1, 0, 0, res, 0 }

    //    +--------------------------------------- controlled sequence
    //    |             +------------------------- sequence to be found
    //    |             |              +---------- find() off argument
    //    |             |              |   +------ expected result 
    //    |             |              |   |                          
    //    |             |              |   |           
    //    V             V              V   V                
    TEST ("ab",         "a",           0,  0),     

    TEST ("",           "",            0,  0),     
    TEST ("",           "\0",          0, -1),    
    TEST ("",           "a",           0, -1),    

    TEST ("\0",         "",            0,  0),    
    TEST ("\0",         "\0",          0,  0),   
    TEST ("\0",         "\0",          1, -1),    
    TEST ("\0\0",       "\0",          1,  1),   
    TEST ("\0",         "a",           0, -1),    

    TEST ("bbcdefghij", "a",           0, -1),    
    TEST ("abcdefghij", "a",           0,  0),   
    TEST ("abcdefghij", "a",           2, -1),   
    TEST ("abcdefghij", "f",           2,  5),    
    TEST ("abcdefghij", "f",           7, -1),   
    TEST ("abcdefghij", "j",           9,  9),    

    TEST ("edfcbbhjig", "cba",         0, -1),    
    TEST ("edfcbahjig", "cba",         1,  3),    
    TEST ("edfcbahjig", "cba",         4, -1),   
    TEST ("edfcbahcba", "cba",         1,  3),   
    TEST ("edfcbahcba", "cba",         5,  7),   
    TEST ("cbacbahjig", "cba",         5, -1),   

    TEST ("e\0cb\0\0g", "b\0\0g",      0,  3),   
    TEST ("e\0cb\0\0g", "b\0\0g",      4, -1),    
    TEST ("e\0cb\0\0g", "ecb",         0, -1),   
    TEST ("\0cb\0\0ge", "\0\0ge",      6, -1),   
    TEST ("\0cb\0\0ge", "\0\0ge",      1,  3),    
    TEST ("\0cb\0\0ge", "cb\0",        1,  1),    
    TEST ("\0cbg\0\0e", "cb\0",        1, -1),    
    TEST ("e\0cbg\0\0", "bg",          1,  3),   
    TEST ("e\0cbg\0\0", "cba",         0, -1),   

    TEST ("bcbedfbjih", "a",           0, -1),    
    TEST ("bcaedfajih", "a",           1,  2),   
    TEST ("bcedfaajih", "a",           6,  6),    
    TEST ("bcaaedfaji", "a",           5,  7),   

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0,  0),   
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1, -1),    
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  0, -1),    
    TEST ("aaaabaaaaa", "aaaaa",       0,  5),    
    TEST ("aaaabaaaaa", "aaaaa",       6, -1),    
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0,  0),    
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   1,  1),   
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   2, -1),   
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  0, -1),    

    TEST (LSTR,         "",            0,  0),    
    TEST (LSTR,         "a",           0, -1),    
    TEST (LSTR,         "x",           0,  0),   
    TEST (LSTR,         "xxx",        10, 10),     
    TEST (LSTR,         "xxa",        10, -1),    
    TEST ("abc",        LSTR,          2, -1),   
    TEST ("xxxxxxxxxx", LSTR,          0, -1),    

    TEST (LSTR,         "xxx",  LLEN - 4, LLEN - 4), 
    TEST (LSTR,         "xxx",  LLEN - 3, -1),   

    TEST ("abcdefghij", 0,             0,  0),    
    TEST ("abcdefghij", 0,             1, -1),   
    TEST ("\0cb\0\0ge", 0,             5, -1),   
    TEST ("\0cb\0\0ge", 0,             0,  0),    
    TEST (LSTR,         0,             0,  0),    
    TEST (LSTR,         0,             1, -1),   

    TEST ("",           "",            1, -1),   
    TEST ("abcdefghij", "abc",        10, -1),  
    TEST ("abcdefghij", "cba",        10, -1),   

    TEST ("last test",  "test",        0,  5)     
};

/**************************************************************************/

// used to exercise 
// find (value_type)
static const TestCase val_test_cases [] = {

#undef TEST
#define TEST(str, val, res)                                     \
    { __LINE__, -1, -1, -1, -1, val, str, sizeof str - 1,       \
      0, 0, 0, 0, res, 0 }

    //    +----------------------------- controlled sequence
    //    |              +-------------- character to be found
    //    |              |     +-------- expected result  
    //    |              |     |                           
    //    |              |     |                
    //    V              V     V                 
    TEST ("ab",          'a',  0),   

    TEST ("",            'a', -1),  
    TEST ("",           '\0', -1),   

    TEST ("\0",         '\0',  0),   
    TEST ("\0\0",       '\0',  0),  
    TEST ("\0",          'a', -1),   

    TEST ("e\0cb\0\0g", '\0',  1),    
    TEST ("e\0cb\0\0g",  'b',  3),    
    TEST ("e\0cb\0\0g",  'a', -1),   
    TEST ("\0cbge\0\0", '\0',  0),   
    TEST ("\0cbge\0\0",  'b',  2),   
    TEST ("\0cbge\0\0",  'a', -1),   

    TEST (LSTR,          'x',  0),  
    TEST (LSTR,         '\0', -1),   
    TEST (LSTR,          'a', -1),  

    TEST ("last test",   't',  3)    
};

/**************************************************************************/

// used to exercise 
// find (value_type, size_type)
static const TestCase val_size_test_cases [] = {

#undef TEST
#define TEST(str, val, off, res)                                     \
    { __LINE__, off, -1, -1, -1, val, str, sizeof str - 1,           \
      0, 0, 0, 0, res, 0 }

    //    +------------------------------ controlled sequence
    //    |              +--------------- character to be found
    //    |              |     +--------- find() off argument
    //    |              |     |   +----- expected result  
    //    |              |     |   |                              
    //    |              |     |   |               
    //    V              V     V   V                   
    TEST ("ab",          'a',  0,  0),    

    TEST ("",            'a',  0, -1),   
    TEST ("",           '\0',  0, -1),    

    TEST ("\0",         '\0',  1, -1),    
    TEST ("\0",          'a',  0, -1),   
    TEST ("\0\0",       '\0',  1,  1),    
    TEST ("\0\0",        'a',  3, -1),   
    TEST ("\0\0",       '\0',  3, -1),    

    TEST ("e\0cb\0\0g", '\0',  1,  1),    
    TEST ("e\0cb\0\0g", '\0',  2,  4),    
    TEST ("e\0cb\0\0g", '\0',  6, -1),    
    TEST ("e\0cb\0\0g",  'b',  1,  3),    
    TEST ("e\0cb\0\0g",  'b',  4, -1),    
    TEST ("e\0cb\0\0g",  'a',  0, -1),   
    TEST ("\0cbge\0\0", '\0',  0,  0),   
    TEST ("\0cbge\0\0", '\0',  1,  5),  
    TEST ("\0cbge\0\0", '\0',  9, -1),    
    TEST ("\0cbge\0\0",  'b',  0,  2),  
    TEST ("\0bgeb\0\0",  'b',  2,  4),   
    TEST ("\0cbge\0\0",  'a',  1, -1),    

    TEST (LSTR,          'x',  0,  0),   
    TEST (LSTR,          'x',  5,  5),   
    TEST (LSTR,         '\0',  0, -1),    
    TEST (LSTR,          'a',  3, -1),     
    TEST (LSTR,          'x', LLEN - 1, -1),
    TEST (LSTR,          'x', LLEN - 2, LLEN - 2),

    TEST ("last test",   't',  0,  3)      
};

/**************************************************************************/

template <class charT, class Traits>
void test_find (charT, Traits*,  
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
    std::size_t exp_res = -1 != cs.nres ? cs.nres : TestString::npos;

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
        case Find (ptr): {
            res = s_str.find (ptr_arg);
            break;
        }

        case Find (str): {
            res = s_str.find (str_arg);
            break;
        }

        case Find (ptr_size): {
            res = s_str.find (ptr_arg, cs.off);
            break;
        }

        case Find (ptr_size_size): {
            res = s_str.find (ptr_arg, cs.off, size);
            break;
        }

        case Find (str_size): {
            res = s_str.find (str_arg, cs.off);
            break;
        }

        case Find (val): {
            res = s_str.find (arg_val);
            break;
        }

        case Find (val_size): {
            res = s_str.find (arg_val, cs.off);
            break;
        }

        default:
            RW_ASSERT ("test logic error: unknown find overload");
            return;
        }

        // verify the returned value
        rw_assert (exp_res == res, 0, cs.line,
                   "line %d. %{$FUNCALL} == %{?}%zu%{;}%{?}npos%{;}, "
                   "got %{?}%zu%{;}%{?}npos%{;}", 
                   __LINE__, -1 != cs.nres, exp_res, -1 == cs.nres, 
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

DEFINE_TEST_DISPATCH (test_find);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::find_ ## tag,                            \
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
                                    "lib.string.find",
                                    test_find, tests, test_count);
}
