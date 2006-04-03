/***************************************************************************
 *
 * 21.string.insert.cpp - string test exercising lib.string.insert
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

/**************************************************************************/

struct MemFun
{
    enum charT  { Char, WChar, UChar };
    enum Traits { DefaultTraits, UserTraits };

    MemFun (charT cid, const char *cname,
          Traits tid, const char *tname)
        : cid_ (cid), tid_ (tid), 
          cname_ (cname), tname_ (tname), aname_ ("allocator"),
          fname_ ("insert") { /* empty */ }

    charT       cid_;     // character type id (char or wchar_t)
    Traits      tid_;     // traits type id (default or user-defined)
    const char *cname_;   // character type name
    const char *tname_;   // traits name
    const char *aname_;   // allocator name
    const char *fname_;   // function name
};

/**************************************************************************/

// for convenience and brevity
#define LSTR      long_string
#define LLEN      long_string_len

static const std::size_t long_string_len = 4096;
static char long_string [long_string_len];

static const char* const exp_exceptions[] = 
    { "unknown exception", "out_of_range", "length_error" };

/**************************************************************************/

typedef enum InsertTags {

    // insert (size_type pos, const charT* s)
    insert_off_ptr          =  1,   
    // insert (size_type pos, const basic_string& str)
    insert_off_str          =  2,
    // insert (size_type pos, const charT* s, size_type n)
    insert_off_ptr_size     =  3,
    // insert (size_type pos1, basic_string& str, size_type pos2, size_type n)
    insert_off_str_off_size =  4,
    // insert (size_type pos, size_type n, charT c)
    insert_off_size_val     =  5,
    // insert (iterator p, charT c)
    insert_val              =  6,
    // insert (iterator p, size_type n, charT c)
    insert_size_val         =  7,
    // insert (iterator p, InputIterator first, InputIterator last)
    insert_range            =  8

} ITags;

/**************************************************************************/

struct TestCase
{
    int  line;

    int  pos1;
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

static int rw_opt_no_insert_off_ptr;           // for --no-insert-off-ptr
static int rw_opt_no_insert_off_str;           // for --no-insert-off-str
static int rw_opt_no_insert_off_ptr_size;      // for --no-insert-off-ptr-size
static int rw_opt_no_insert_off_str_off_size;  // --no-insert-off-str-off-size
static int rw_opt_no_insert_off_size_val;      // for --no-insert-off-size-val
static int rw_opt_no_insert_size_val;          // for --no-insert-size-val
static int rw_opt_no_insert_val;               // for --no-insert-val
static int rw_opt_no_insert_range;             // for --no-insert-range

/**************************************************************************/

// used to exercise 
// insert (size_type pos, const charT* s)
static const TestCase off_test_cases [] = {

#undef TEST
#define TEST(str, pos1, src, res, bthrow)                            \
    { __LINE__, pos1, -1, -1, -1, str, sizeof str - 1, src,          \
      sizeof src - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- sequence to be inserted
    //    |            |  |           +------------- expected result sequence
    //    |            |  |           |        +---- exception info 
    //    |            |  |           |        |         0 - no exception        
    //    |            |  |           |        |         1 - out_of_range        
    //    |            |  |           |        |         2 - length_error        
    //    |            |  |           |        |                       
    //    |            |  |           |        +------------+             
    //    V            V  V           V                     V
    TEST ("ab",        0, "c",        "cab",                0),

    TEST ("",          0, "",         "",                   0),
    TEST ("",          0, "abc",      "abc",                0),

    TEST ("\0",        0, "",         "\0",                 0),
    TEST ("",          0, "\0",       "",                   0),

    TEST ("abc",       0, "",         "abc",                0),
    TEST ("abc",       1, "",         "abc",                0),
    TEST ("abc",       3, "",         "abc",                0),

    TEST ("\0",        0, "a",        "a\0",                0),
    TEST ("\0",        1, "a",        "\0a",                0),
    TEST ("\0",        0, "\0",       "\0",                 0),
    TEST ("\0",        1, "\0\0",     "\0",                 0),

    TEST ("cde",       0, "ab",       "abcde",              0),
    TEST ("cde",       1, "ab",       "cabde",              0),
    TEST ("cde",       2, "ab",       "cdabe",              0),
    TEST ("cde",       3, "ab",       "cdeab",              0),

    TEST ("\0ab\0\0c", 0, "\0e\0",    "\0ab\0\0c",          0),
    TEST ("a\0b\0\0c", 2, "e\0e",     "a\0eb\0\0c",         0),
    TEST ("a\0bc\0\0", 6, "e\0e",     "a\0bc\0\0e",         0),

    TEST ("abc",       0, 0,          "abcabc",             0),
    TEST ("abc",       2, 0,          "ababcc",             0),
    TEST ("a\0bc\0\0", 0, 0,          "aa\0bc\0\0",         0),
    TEST ("a\0bc\0\0", 6, 0,          "a\0bc\0\0a",         0),

    TEST ("",          0, LSTR,       LSTR,                 0),
    TEST (LSTR,        0, "",         LSTR,                 0),

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",        2, "",         "",                   1),
    TEST ("a",         2, "",         "",                   1),
    TEST (LSTR,LLEN + 10, "",         "",                   1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",      4, "test",     "lasttest",           0)
};

/**************************************************************************/

// used to exercise 
// insert (size_type pos, const basic_string& str)
static const TestCase off_str_test_cases [] = {

#undef TEST
#define TEST(str, pos1, src, res, bthrow)                            \
    { __LINE__, pos1, -1, -1, -1, str, sizeof str - 1, src,          \
      sizeof src - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- sequence to be inserted
    //    |            |  |           +------------- expected result sequence
    //    |            |  |           |        +---- exception info 
    //    |            |  |           |        |         0 - no exception        
    //    |            |  |           |        |         1 - out_of_range        
    //    |            |  |           |        |         2 - length_error        
    //    |            |  |           |        |                       
    //    |            |  |           |        +------------+             
    //    V            V  V           V                     V
    TEST ("ab",        0, "c",        "cab",                0),

    TEST ("",          0, "",         "",                   0),
    TEST ("",          0, "abc",      "abc",                0),

    TEST ("\0",        0, "",         "\0",                 0),
    TEST ("",          0, "\0",       "\0",                 0),

    TEST ("abc",       0, "",         "abc",                0),
    TEST ("abc",       1, "",         "abc",                0),
    TEST ("abc",       3, "",         "abc",                0),

    TEST ("\0",        0, "a",        "a\0",                0),
    TEST ("\0",        1, "a",        "\0a",                0),
    TEST ("\0",        0, "\0",       "\0\0",               0),
    TEST ("\0",        1, "\0\0",     "\0\0\0",             0),

    TEST ("cde",       0, "ab",       "abcde",              0),
    TEST ("cde",       1, "ab",       "cabde",              0),
    TEST ("cde",       2, "ab",       "cdabe",              0),
    TEST ("cde",       3, "ab",       "cdeab",              0),

    TEST ("\0ab\0\0c", 0, "\0e\0",    "\0e\0\0ab\0\0c",     0),
    TEST ("a\0b\0\0c", 2, "e\0e",     "a\0e\0eb\0\0c",      0),
    TEST ("a\0bc\0\0", 6, "e\0\0e\0", "a\0bc\0\0e\0\0e\0",  0),

    TEST ("abc",       0, 0,          "abcabc",             0),
    TEST ("abc",       2, 0,          "ababcc",             0),
    TEST ("a\0bc\0\0", 0, 0,          "a\0bc\0\0a\0bc\0\0", 0),
    TEST ("a\0bc\0\0", 6, 0,          "a\0bc\0\0a\0bc\0\0", 0),

    TEST ("",          0, LSTR,       LSTR,                 0),
    TEST (LSTR,        0, "",         LSTR,                 0),

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",        2, "",         "",                   1),
    TEST ("a",         2, "",         "",                   1),
    TEST (LSTR,LLEN + 10, "",         "",                   1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",      4, "test",     "lasttest",           0)
};

/**************************************************************************/

// used to exercise 
// insert (size_type pos1, basic_string& str, size_type pos2, size_type n)
// insert (iterator p, InputIterator first, InputIterator last)
static const TestCase range_test_cases [] = {

#undef TEST
#define TEST(str, pos1, src, pos2, count, res, bthrow)                \
    { __LINE__, pos1, pos2, count, -1, str, sizeof str - 1, src,      \
      sizeof src - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- sequence to be inserted
    //    |            |  |            +------------ insert() pos2 argument
    //    |            |  |            |  +--------- insert() num argument 
    //    |            |  |            |  |  +------ expected result sequence
    //    |            |  |            |  |  |  +--- exception info  
    //    |            |  |            |  |  |  |       0 - no exception        
    //    |            |  |            |  |  |  |       1 - out_of_range        
    //    |            |  |            |  |  |  |       2 - length_error        
    //    |            |  |            |  |  |  |                         
    //    |            |  |            |  |  |  +----------------+             
    //    V            V  V            V  V  V                   V             
    TEST ("ab",        0, "c",         0, 1, "cab",              0),

    TEST ("",          0, "",          0, 0,  "",                0),
    TEST ("",          0, "abc",       1, 1,  "b",               0),
    TEST ("",          0, "\0",        0, 1,  "\0",              0),

    TEST ("\0",        0, "",          0, 0,  "\0",              0),
    TEST ("\0",        1, "",          0, 0,  "\0",              0),

    TEST ("abc",       0, "",          0, 0,  "abc",             0),
    TEST ("abc",       1, "",          0, 0,  "abc",             0),
    TEST ("abc",       3, "",          0, 0,  "abc",             0),

    TEST ("\0",        0, "a",         0, 1,  "a\0",             0),
    TEST ("\0",        1, "a",         0, 1,  "\0a",             0),
    TEST ("\0",        0, "\0\0",      1, 1,  "\0\0",            0),
    TEST ("\0",        1, "\0\0",      0, 2,  "\0\0\0",          0),
    TEST ("\0",        1, "\0\0",      1, 5,  "\0\0",            0),

    TEST ("cde",       0, "ab",        0, 2,  "abcde",           0),
    TEST ("cde",       1, "ab",        0, 1,  "cade",            0),
    TEST ("cde",       2, "ab",        1, 5,  "cdbe",            0),
    TEST ("cde",       3, "ab",        1, 1,  "cdeb",            0),

    TEST ("ab",        0, "c\0e",      0, 3,  "c\0eab",          0),
    TEST ("ab",        1, "c\0e",      1, 2,  "a\0eb",           0),
    TEST ("ab",        2, "c\0e",      0, 2,  "abc\0",           0),

    TEST ("\0e\0",     1, "\0ab\0\0c", 0, 9,  "\0\0ab\0\0ce\0",  0),
    TEST ("\0e\0",     1, "\0ab\0\0c", 0, 3,  "\0\0abe\0",       0),
    TEST ("a\0b\0\0c", 3, "\0e\0",     0, 3,  "a\0b\0e\0\0\0c",  0),
    TEST ("a\0b\0\0c", 2, "\0\0e\0",   0, 2,  "a\0\0\0b\0\0c",   0),
    TEST ("\0ab\0\0c", 0, "\0e\0",     2, 1,  "\0\0ab\0\0c",     0),
    TEST ("a\0bc\0\0", 6, "\0e",       0, 2,  "a\0bc\0\0\0e",    0),

    TEST ("abc",       0, 0,           1, 1,  "babc",            0),
    TEST ("abc",       2, 0,           0, 2,  "ababc",           0),
    TEST ("a\0bc\0\0", 0, 0,           4, 2,  "\0\0a\0bc\0\0",   0),
    TEST ("a\0bc\0\0", 6, 0,           1, 3,  "a\0bc\0\0\0bc",   0),
    TEST ("abcdef",    0, 0,           1, 2,  "bcabcdef",        0),

    TEST ("",          0, LSTR,        0, LLEN, LSTR,            0),
    TEST (LSTR,        0, "",          0, 0,    LSTR,            0),

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",        2, "",          0, 0,  "",                1),
    TEST ("",          0, "\0",        2, 0,  "",                2),

    TEST ("a",         2, "",          0, 0,  "",                1),
    TEST ("",          0, "a",         2, 0,  "",                2),

    TEST (LSTR,LLEN + 10, "",          0, 0,  "",                1),
    TEST ("",          0, LSTR,LLEN + 10, 0,  "",                2),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",      4, "test",      0, 4,  "lasttest",        0)
};

/**************************************************************************/

// used to exercise
// insert (size_type pos, const charT* s, size_type n)
static const TestCase off_size_test_cases [] = {

#undef TEST
#define TEST(str, pos1, src, count, res, bthrow)                     \
    { __LINE__, pos1, -1, count, -1, str, sizeof str - 1, src,       \
      sizeof src - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- sequence to be inserted
    //    |            |  |            +------------ insert() num argument 
    //    |            |  |            |  +--------- expected result sequence 
    //    |            |  |            |  |     +--- exception info 
    //    |            |  |            |  |     |      0 - no exception        
    //    |            |  |            |  |     |      1 - out_of_range        
    //    |            |  |            |  |     |      2 - length_error        
    //    |            |  |            |  |     |                           
    //    |            |  |            |  |     +------------+             
    //    V            V  V            V  V                  V             
    TEST ("ab",        0, "c",         1, "cab",             0),

    TEST ("",          0, "",          0,  "",               0),
    TEST ("",          0, "abc",       1,  "a",              0),

    TEST ("\0",        0, "",          0,  "\0",             0),
    TEST ("\0",        1, "",          0,  "\0",             0),
    TEST ("",          0, "\0",        1,  "\0",             0),

    TEST ("\0",        0, "a",         0,  "\0",             0),
    TEST ("\0",        1, "a",         1,  "\0a",            0),
    TEST ("\0",        0, "\0\0",      1,  "\0\0",           0),
    TEST ("\0",        1, "\0\0",      2,  "\0\0\0",         0),

    TEST ("cde",       0, "ab",        2,  "abcde",          0),
    TEST ("cde",       1, "ab",        1,  "cade",           0),
    TEST ("cde",       3, "ab",        1,  "cdea",           0),
    TEST ("cde",       2, "ab",        2,  "cdabe",          0),

    TEST ("\0e\0",     3, "a\0b\0\0c", 0,  "\0e\0",          0),
    TEST ("\0e\0",     3, "\0ab\0\0c", 3,  "\0e\0\0ab",      0),

    TEST ("a\0b\0\0c", 3, "\0e\0",     3,  "a\0b\0e\0\0\0c", 0),
    TEST ("a\0b\0\0c", 2, "\0\0e\0",   2,  "a\0\0\0b\0\0c",  0),
    TEST ("\0ab\0\0c", 0, "\0e\0",     1,  "\0\0ab\0\0c",    0),
    TEST ("a\0bc\0\0", 6, "\0e",       2,  "a\0bc\0\0\0e",   0),

    TEST ("abc",       0, 0,           1,  "aabc",           0),
    TEST ("abc",       2, 0,           2,  "ababc",          0),
    TEST ("\0abc\0\0", 0, 0,           1,  "\0\0abc\0\0",    0),
    TEST ("a\0bc\0\0", 6, 0,           3,  "a\0bc\0\0a\0b",  0),

    TEST ("",          0, LSTR, LLEN - 1,  LSTR,             0),
    TEST (LSTR,        0, "",          0,  LSTR,             0),

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",        2, "",          0,  "",               1),
    TEST ("a",         2, "",          0,  "",               1),
    TEST (LSTR,LLEN + 10, "",          0,  "",               1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",      4, "test",      4,  "lasttest",       0)
};

/**************************************************************************/

// used to exercise
// insert (size_type pos, size_type n, charT c)
// insert (iterator  p,   size_type n, charT c)
static const TestCase size_val_test_cases [] = {

#undef TEST
#define TEST(str, pos1, count, ch, res, bthrow)                     \
    { __LINE__, pos1, -1, count, ch, str, sizeof str - 1, 0,        \
      0, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |  +------------------------- insert() count argument
    //    |            |  |   +--------------------- character to be inserted
    //    |            |  |   |   +----------------- expected result sequence 
    //    |            |  |   |   |       +--------- exception info 
    //    |            |  |   |   |       |             0 - no exception        
    //    |            |  |   |   |       |             1 - out_of_range        
    //    |            |  |   |   |       |             2 - length_error     
    //    |            |  |   |   |       |                         
    //    |            |  |   |   |       +-----------+             
    //    V            V  V   V   V                   V             
    TEST ("ab",        0, 1, 'c', "cab",              0),

    TEST ("",          0, 0, ' ',  "",                0),
    TEST ("",          0, 1, 'b',  "b",               0),

    TEST ("\0",        0, 0, ' ',  "\0",              0),
    TEST ("\0",        1, 0, ' ',  "\0",              0),
    TEST ("",          0, 2, '\0', "\0\0",            0),

    TEST ("\0",        0, 1, 'a',  "a\0",             0),
    TEST ("\0",        1, 1, 'a',  "\0a",             0),
    TEST ("\0",        0, 1, '\0', "\0\0",            0),
    TEST ("\0",        1, 2, '\0', "\0\0\0",          0),
    TEST ("\0",        1, 0, '\0', "\0",              0),

    TEST ("cde",       0, 2, 'a',  "aacde",           0),
    TEST ("cde",       1, 1, 'a',  "cade",            0),
    TEST ("cde",       3, 3, 'a',  "cdeaaa",          0),
    TEST ("cde",       2, 3, 'a',  "cdaaae",          0),

    TEST ("ab",        0, 2, '\0', "\0\0ab",          0),
    TEST ("ab",        1, 1, '\0', "a\0b",            0),
    TEST ("ab",        2, 2, '\0', "ab\0\0",          0),

    TEST ("a\0b\0\0c", 3, 2, '\0', "a\0b\0\0\0\0c",   0),
    TEST ("a\0b\0\0c", 2, 1, '\0', "a\0\0b\0\0c",     0),
    TEST ("\0ab\0\0c", 0, 3, '\0', "\0\0\0\0ab\0\0c", 0),
    TEST ("a\0bc\0\0", 6, 2, 'a',  "a\0bc\0\0aa",     0),

    TEST ("",          0, LLEN - 1, 'x', LSTR,        0),
    TEST (LSTR,        0, 0,        'x', LSTR,        0),

    TEST ("last",      4, 4, 't',  "lasttttt",        0)
};

/**************************************************************************/

// used to exercise
// insert (iterator p, charT c)
static const TestCase val_test_cases [] = {

#undef TEST
#define TEST(str, pos1, ch, res, bthrow)                            \
    { __LINE__, pos1, -1, -1, ch, str, sizeof str - 1, 0,           \
      0, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- insert() pos argument
    //    |            |    +----------------------- character to be inserted
    //    |            |    |   +------------------- expected result sequence 
    //    |            |    |   |               +--- exception info 
    //    |            |    |   |               |       0 - no exception        
    //    |            |    |   |               |       1 - out_of_range        
    //    |            |    |   |               |       2 - length_error     
    //    |            |    |   |               |                 
    //    |            |    |   |               |
    //    V            V    V   V               V  
    TEST ("ab",        0,  'c', "cab",          0),

    TEST ("",          0,  'b', "b",            0),
    TEST ("",          0, '\0', "\0",           0),

    TEST ("\0",        0,  'a', "a\0",          0),
    TEST ("\0",        1,  'a', "\0a",          0),
    TEST ("\0",        0, '\0', "\0\0",         0),
    TEST ("\0",        1, '\0', "\0\0",         0),

    TEST ("cde",       0,  'a', "acde",         0),
    TEST ("cde",       1,  'a', "cade",         0),
    TEST ("cde",       2,  'a', "cdae",         0),
    TEST ("cde",       3,  'a', "cdea",         0),

    TEST ("a\0b\0\0c", 3, '\0', "a\0b\0\0\0c",  0),
    TEST ("\0ab\0\0c", 0, '\0', "\0\0ab\0\0c",  0),
    TEST ("a\0bc\0\0", 6,  'a', "a\0bc\0\0a",   0),

    TEST ("last",      4,  't', "lastt",        0)
};

/**************************************************************************/

static const struct FunctionTag
{
    ITags           i_tag;
    const int      *p_opt;
    const TestCase *t_cases;
    std::size_t     n_cases;
    const char     *str_hdr;

} function_tags [] = {

#undef TEST
#define TEST(tag, opt, cases, hdr)                              \
    { tag, &opt, cases, sizeof cases / sizeof *cases, hdr }     

    TEST (insert_off_ptr, rw_opt_no_insert_off_ptr, off_test_cases,                  
          "insert (size_type pos, const charT* s)"),

    TEST (insert_off_str, rw_opt_no_insert_off_str, off_str_test_cases,            
          "insert (size_type pos, const basic_string& str)"),

    TEST (insert_off_ptr_size, rw_opt_no_insert_off_ptr_size, 
          off_size_test_cases,                   
          "insert (size_type pos, const charT* s, size_type n)"),

    TEST (insert_off_str_off_size, rw_opt_no_insert_off_str_off_size, 
          range_test_cases, "insert (size_type pos1, const basic_string& str,"
          " size_type pos2, size_type n)"),

    TEST (insert_off_size_val, rw_opt_no_insert_off_size_val, 
          size_val_test_cases, "insert (size_type pos, size_type n, charT c)"),

    TEST (insert_size_val, rw_opt_no_insert_size_val, 
          size_val_test_cases, "insert (iterator p, size_type n, charT c)"),

    TEST (insert_val, rw_opt_no_insert_val, val_test_cases, 
          "insert (iterator p, charT c)"),

    TEST (insert_range, rw_opt_no_insert_range, range_test_cases, 
          "insert (iterator p, InputIterator first, InputIterator last)")
};

/**************************************************************************/

template <class charT, class Traits, class Iterator>
void test_insert_range (charT* wstr,
                        charT* wsrc, 
                        Traits*,
                        const Iterator &it,
                        const TestCase &cs,
                        const char     *insert_fmt)
{
    typedef std::basic_string <charT, Traits, 
                               std::allocator<charT> > String;
    typedef typename String::iterator StringIter;

    const char* const itname = 
        cs.src ? type_name (it, (charT*)0) : "basic_string::iterator";

    String s_str (wstr, cs.str_len);
    String s_src (wsrc, cs.src_len);

    std::size_t off_last = cs.pos2 + cs.count;

    StringIter iter (s_str.begin () + cs.pos1);

    if (cs.src) {
        off_last = off_last > s_src.size () ? s_src.size () : off_last;

        const Iterator first = make_iter (wsrc + cs.pos2, 
            wsrc + cs.pos2, wsrc + off_last, Iterator (0, 0, 0));
        const Iterator last  = make_iter (wsrc + off_last, 
            wsrc + cs.pos2, wsrc + off_last, Iterator (0, 0, 0));

        s_str.insert (iter, first, last);
    }
    else {
        StringIter first (s_str.begin () + cs.pos2);
        StringIter last  (off_last > s_str.size () ? 
            s_str.end () 
          : s_str.begin () + off_last);

        s_str.insert (iter, first, last);
    }

    const std::size_t match = rw_match (cs.res, s_str.c_str(), cs.res_len);

    rw_assert (match == cs.res_len, 0, cs.line,
               "line %d. %s expected %{#*s}, got %{/*.*Gs}, "
               "difference at pos %zu for %s", 
               __LINE__, insert_fmt, int (cs.res_len), cs.res, 
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (), 
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits>
void test_insert_range (charT* wstr, 
                        charT* wsrc, 
                        Traits*,
                        const TestCase &cs,
                        const char     *insert_fmt)
{
    if (cs.bthrow)  // this method doesn't throw
        return;

    test_insert_range (wstr, wsrc, (Traits*)0, 
                       InputIter<charT>(0, 0, 0), cs, insert_fmt);

    // there is no need to call test_insert_range 
    // for other iterators in this case
    if (0 == cs.src)
        return;

    test_insert_range (wstr, wsrc, (Traits*)0, 
                       ConstFwdIter<charT>(0, 0, 0), cs, insert_fmt);

    test_insert_range (wstr, wsrc, (Traits*)0, 
                       ConstBidirIter<charT>(0, 0, 0), cs, insert_fmt);

    test_insert_range (wstr, wsrc, (Traits*)0, 
                       ConstRandomAccessIter<charT>(0, 0, 0), cs, insert_fmt);
}

/**************************************************************************/

template <class charT, class Traits>
void test_insert (charT, Traits*,  
                  const ITags     which,
                  const TestCase &cs,
                  const char     *insert_fmt)
{
    typedef std::basic_string <charT, Traits, 
                               std::allocator<charT> > TestString;
    typedef typename TestString::iterator StringIter;

    const bool use_iters = (insert_size_val <= which);

    static charT wstr [LLEN];
    static charT wsrc [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (wsrc, cs.src, cs.src_len);

    // special processing for insert_range to exercise all iterators
    if (insert_range == which) {
        test_insert_range (wstr, wsrc, (Traits*)0, cs, insert_fmt);
        return;
    }

    TestString s_str (wstr, cs.str_len);
    TestString s_src (wsrc, cs.src_len);

    std::size_t res_off = 0;
    std::size_t exp_off = insert_val == which ? cs.pos1 : 0;

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    if (1 == cs.bthrow && !use_iters)
        expected = exp_exceptions [1];
    if (2 == cs.bthrow && insert_off_str_off_size == which)
        expected = exp_exceptions [1];
    if (3 == cs.bthrow && !use_iters)
        expected = exp_exceptions [2];

    const char* caught = 0;

    try {

#endif   // _RWSTD_NO_EXCEPTIONS

    switch (which)
    {
    case insert_off_ptr: {
        TestString& s_res = 
            s_str.insert (cs.pos1, cs.src ? wsrc : s_str.c_str ());
        res_off = &s_res - &s_str;
        break;
    }

    case insert_off_str: {
        TestString& s_res = s_str.insert (cs.pos1, cs.src ? s_src : s_str);
        res_off = &s_res - &s_str;
        break;
    }

    case insert_off_ptr_size: {
        TestString& s_res = 
            s_str.insert (cs.pos1, cs.src ? wsrc : s_str.c_str (), cs.count);
        res_off = &s_res - &s_str;
        break;
    }

    case insert_off_str_off_size: {
        TestString& s_res = 
            s_str.insert (cs.pos1, cs.src ? s_src : s_str, cs.pos2, cs.count);
        res_off = &s_res - &s_str;
        break;
    }

    case insert_off_size_val: {
        TestString& s_res = s_str.insert (cs.pos1, cs.count, 
            make_char ((char) cs.ch, (charT*)0));
        res_off = &s_res - &s_str;
        break;
    }

    case insert_size_val: {
        StringIter it (s_str.begin () + cs.pos1);
        s_str.insert (it, cs.count, make_char ((char) cs.ch, (charT*)0));
        break;
    }

    case insert_val: {
        StringIter it (s_str.begin () + cs.pos1);
        StringIter res_it = 
            s_str.insert (it, make_char ((char) cs.ch, (charT*)0));
        res_off = res_it - s_str.begin ();
        break;
    }

    default:
        RW_ASSERT ("test logic error: unknown insert overload");
        return;
    }

    // verify the returned value
    rw_assert (exp_off == res_off, 0, cs.line,
               "line %d. %s %{?}== %zu, got %zu%{;}"
               "%{?}returned invalid reference, offset is %zu%{;}", 
               __LINE__, insert_fmt, insert_val == which, exp_off, res_off,
               insert_val != which, res_off);

    // verfiy that strings length are equal
    rw_assert (cs.res_len == s_str.size (), 0, cs.line,
               "line %d. %s expected %{#*s} with length %zu, got %{/*.*Gs} "
               "with length %zu", __LINE__, insert_fmt, int (cs.res_len), 
               cs.res, cs.res_len, int (sizeof (charT)), int (s_str.size ()),
               s_str.c_str (), s_str.size ());

    // verfiy that insert results match expected result
    const std::size_t match = rw_match (cs.res, s_str.c_str(), cs.res_len);

    rw_assert (match == cs.res_len, 0, cs.line,
               "line %d. %s expected %{#*s}, got %{/*.*Gs}, "
               "difference at pos %zu", 
               __LINE__, insert_fmt, int (cs.res_len), cs.res, 
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
               "%{:}unexpectedly caught %s%{;}",
               __LINE__, insert_fmt, 0 != expected, expected, caught, caught);
}

/**************************************************************************/

void get_insert_format (char** pbuf, std::size_t* pbufsize, 
                        const MemFun *pfid, 
                        const ITags which, 
                        const TestCase& cs)
{
    if (   MemFun::DefaultTraits == pfid->tid_
        && (MemFun::Char == pfid->cid_ || MemFun::WChar == pfid->cid_))
        rw_asnprintf (pbuf, pbufsize,
                      "std::%{?}w%{;}string (%{#*s}).insert",
                      MemFun::WChar == pfid->cid_,
                      int (cs.str_len), cs.str);
    else
        rw_asnprintf (pbuf, pbufsize,
                      "std::basic_string<%s, %s<%1$s>, %s<%1$s>>(%{#*s})",
                      pfid->cname_, pfid->tname_, pfid->aname_, 
                      int (cs.str_len), cs.str);

    const bool self = 0 == cs.src;

    switch (which)
    {
    case insert_off_ptr:
        rw_asnprintf (pbuf, pbufsize, 
                      "%{+} (%zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      cs.pos1, !self, int (cs.src_len), cs.src, self);
        break;

    case insert_off_str:
        rw_asnprintf (pbuf, pbufsize, 
                      "%{+} (%zu, %{?}string (%{#*s})%{;}%{?}*this%{;})",
                      cs.pos1, !self, int (cs.src_len), cs.src, self);
        break;

    case insert_off_ptr_size:
        rw_asnprintf (pbuf, pbufsize, "%{+} ("
                      "%zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)", 
                      cs.pos1, !self, int (cs.src_len),
                      cs.src, self, cs.count);
        break;

    case insert_off_str_off_size:
        rw_asnprintf (pbuf, pbufsize, "%{+} ("
                      "%zu, %{?}string (%{#*s})%{;}%{?}*this%{;}, %zu, %zu)",
                      cs.pos1, !self, int (cs.src_len), cs.src,
                      self, cs.pos2, cs.count);
        break;

    case insert_off_size_val:
        rw_asnprintf (pbuf, pbufsize, 
                      "%{+} (%zu, %zu, %#c)",
                      cs.pos1, cs.count, cs.ch);
        break;

    case insert_size_val:
        rw_asnprintf (pbuf, pbufsize, 
                      "%{+} (begin + %zu, %zu, %#c)",
                      cs.pos1, cs.count, cs.ch);
        break;

    case insert_val:
        rw_asnprintf (pbuf, pbufsize, 
                      "%{+} (begin + %zu, %#c)",
                      cs.pos1, cs.ch);
        break;

    case insert_range:
        rw_asnprintf (pbuf, pbufsize, "%{+} (begin + %zu, "
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu, "
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu)", 
                      cs.pos1, !self, int (cs.src_len), cs.src,
                      self, cs.pos2, !self, int (cs.src_len), cs.src, 
                      self, cs.pos2 + cs.count);
        break;
    }
}

/**************************************************************************/

void test_insert (const MemFun *pfid, const ITags which, const TestCase& cs)
{
    char* buf = 0;
    std::size_t buf_sz = 0;
    get_insert_format (&buf, &buf_sz, pfid, which, cs); 

#undef TEST
#define TEST(charT, Traits)	                                    \
    test_insert (charT(), (Traits*)0, which, cs, buf)

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
test_insert (const MemFun *pfid, const FunctionTag& ftag)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::%s",
             pfid->cname_, pfid->tname_, pfid->aname_, ftag.str_hdr);

    for (std::size_t i = 0; i != ftag.n_cases; ++i) {

        if (!rw_enabled (ftag.t_cases[i].line)) {
            rw_note (0, 0, __LINE__, 
                     "test on line %d disabled", ftag.t_cases[i].line);
            continue;
        }

        // do not exercise exceptions if they were disabled
        if (0 != rw_opt_no_exceptions && 0 != ftag.t_cases[i].bthrow)
            continue;

        test_insert (pfid, ftag.i_tag, ftag.t_cases[i]);
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
                     "string::insert exceptions tests disabled"); 

        static const std::size_t ftags = 
            sizeof function_tags / sizeof *function_tags;

        for (std::size_t i = 0; i < ftags; i++) {

            if (*function_tags[i].p_opt) 
                rw_note (0, 0, 0, 
                         "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
                         "%s test disabled", pfid->cname_, pfid->tname_, 
                         pfid->aname_, function_tags[i].str_hdr);
            else
                test_insert (pfid, function_tags[i]);
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
        rw_note (0, 0, 0, "string::insert char tests disabled");

    if (rw_enabled ("wchar_t")) {

        MemFun fid (MemFun::WChar, "wchar_t", MemFun::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = MemFun::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::insert wchar tests disabled");

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
                    "lib.string.insert",
                    0 /* no comment */, run_test,
                    "|-no-char_traits# "
                    "|-no-user_traits# "
                    "|-no-user_chars# "
                    "|-no-exceptions# "
                    "|-no-insert-off-ptr# "
                    "|-no-insert-off-str# "
                    "|-no-insert-off-ptr-size# "
                    "|-no-insert-off-str-off-size# "
                    "|-no-insert-off-size-val# "
                    "|-no-insert-size-val# "
                    "|-no-insert-val# "
                    "|-no-insert-range#",
                    &rw_opt_no_char_traits,
                    &rw_opt_no_user_traits,
                    &rw_opt_no_user_chars,
                    &rw_opt_no_exceptions,
                    &rw_opt_no_insert_off_ptr,
                    &rw_opt_no_insert_off_str,
                    &rw_opt_no_insert_off_ptr_size,
                    &rw_opt_no_insert_off_str_off_size,
                    &rw_opt_no_insert_off_size_val,
                    &rw_opt_no_insert_size_val,
                    &rw_opt_no_insert_val,
                    &rw_opt_no_insert_range);
}
