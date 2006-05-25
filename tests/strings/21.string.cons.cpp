/***************************************************************************
 *
 * 21.string.cons.cpp - test exercising string constructors [lib.string.cons]
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
#include <stdexcept>        // for out_of_range, length_error
#include <cstddef>          // for size_t

#include <21.strings.h>
#include <alg_test.h>       // for InputIter
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Ctor(sig)               StringIds::ctor_ ## sig
#define OpSet(sig)              StringIds::op_set_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// basic_string (void)
static const StringTestCase
void_test_cases [] = {

#undef TEST
#define TEST(dummy) {                            \
        __LINE__, -1, -1, -1, -1, -1,            \
        0, 0, dummy, sizeof dummy - 1,           \
        "", 0, 0                                 \
    }

    //    +--------------------- dummy argument
    //    |
    //    V             
    TEST ("ab"),          

    TEST ("last test")
};

/**************************************************************************/

// exercises:
// basic_string (const value_type*)
static const StringTestCase
cptr_test_cases [] = {

#undef TEST
#define TEST(arg, res) {                            \
        __LINE__, -1, -1, -1, -1, -1,               \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, 0                      \
    }

    //    +----------------------------------------- source sequence
    //    |             +--------------------------- expected result sequence
    //    |             |             
    //    |             |            
    //    V             V           
    TEST ("ab",         "ab"),    

    TEST ("",           ""),   
    TEST ("\0",         ""),   

    TEST ("a",          "a"),       
    TEST ("bcd",        "bcd"),       
    TEST ("cdefaihjb",  "cdefaihjb"),      

    TEST ("\0\0ab",     ""),  
    TEST ("a\0\0b",     "a"), 
    TEST ("ab\0\0",     "ab"),  
    TEST ("abefdcc\0a", "abefdcc"),  

    TEST ("x@4096",     "x@4096"),     

    TEST ("last test",  "last test")
};

/**************************************************************************/

// exercises:
// basic_string (const basic_string&)
static const StringTestCase
cstr_test_cases [] = {

#undef TEST
#define TEST(arg, res) {                            \
        __LINE__, -1, -1, -1, -1, -1,               \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, 0                      \
    }

    //    +----------------------------------------- source sequence
    //    |             +--------------------------- expected result sequence
    //    |             |             
    //    |             |            
    //    V             V           
    TEST ("ab",         "ab"),    

    TEST ("",           ""),   
    TEST ("\0",         "\0"),
    TEST ("\0\0",       "\0\0"),

    TEST ("a",          "a"),       
    TEST ("bcd",        "bcd"),       
    TEST ("cdefaihjb",  "cdefaihjb"),      

    TEST ("\0\0ab",     "\0\0ab"),  
    TEST ("a\0\0b",     "a\0\0b"), 
    TEST ("ab\0\0",     "ab\0\0"),  
    TEST ("abefdcc\0a", "abefdcc\0a"),  

    TEST ("x@4096",     "x@4096"),     

    TEST ("last test",  "last test")
};

/**************************************************************************/

// exercises:
// basic_string (const value_type*, size_type)
static const StringTestCase
cptr_size_test_cases [] = {

#undef TEST
#define TEST(arg, size, res) {                      \
        __LINE__, -1, size, -1, -1, -1,             \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, 0                      \
    }

    //    +----------------------------------------- source sequence
    //    |             +--------------------------- ctor n argument
    //    |             |  +------------------------ expected result sequence
    //    |             |  |             
    //    |             |  |            
    //    V             V  V         
    TEST ("ab",         2, "ab"),    

    TEST ("",           0, ""),   
    TEST ("\0",         1, "\0"),   

    TEST ("a",          1, "a"),       
    TEST ("bcd",        3, "bcd"),       
    TEST ("cdefaihjb",  9, "cdefaihjb"),      

    TEST ("\0\0ab",     1, "\0"),
    TEST ("\0\0ab",     4, "\0\0ab"),
    TEST ("a\0\0b",     4, "a\0\0b"), 
    TEST ("ab\0\0",     3, "ab\0"),  
    TEST ("abefdcc\0a", 7, "abefdcc"), 
    TEST ("abefdcc\0a", 8, "abefdcc\0"), 
    TEST ("abefdcc\0a", 9, "abefdcc\0a"), 

    TEST ("x@4096",  4096, "x@4096"),     

    TEST ("last test",  9, "last test")
};

/**************************************************************************/

// exercises:
// basic_string (const basic_string&, size_type)
static const StringTestCase
cstr_size_test_cases [] = {

#undef TEST
#define TEST(arg, off, res, bthrow) {               \
        __LINE__, off, -1, -1, -1, -1,              \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, bthrow                 \
    }

    //    +----------------------------------------- source sequence
    //    |             +--------------------------- ctor off argument
    //    |             |  +------------------------ expected result sequence
    //    |             |  |              +--------- exception info
    //    |             |  |              |             0 - no exception
    //    |             |  |              |             1 - out_of_range
    //    |             |  |              |
    //    |             |  |              |
    //    V             V  V              V
    TEST ("ab",         0, "ab",          0),    

    TEST ("",           0, "",            0),   
    TEST ("\0",         0, "\0",          0),
    TEST ("\0\0",       0, "\0\0",        0),

    TEST ("a",          0, "a",           0),       
    TEST ("bcd",        0, "bcd",         0),       
    TEST ("cdefaihjb",  0, "cdefaihjb",   0),      

    TEST ("\0\0ab",     0, "\0\0ab",      0),  
    TEST ("a\0\0b",     0, "a\0\0b",      0), 
    TEST ("ab\0\0",     0, "ab\0\0",      0),  
    TEST ("abefdcc\0a", 0, "abefdcc\0a",  0),  

    TEST ("x@4096",     0, "x@4096",      0),

    TEST ("abc",        5, "abc",         1),
    TEST ("x@4096",  4106, "x@4096",      1),
 
    TEST ("last test",  0, "last test",   0)
};

/**************************************************************************/

// exercises:
// basic_string (const basic_string&, size_type, size_type)
// basic_string (InputIterator, InputIterator)
static const StringTestCase
cstr_size_size_test_cases [] = {

#define range_test_cases    cstr_size_size_test_cases

#undef TEST
#define TEST(arg, off, size, res, bthrow) {         \
        __LINE__, off, size, -1, -1, -1,            \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, bthrow                 \
    }

    //    +----------------------------------------- source sequence
    //    |             +--------------------------- ctor off argument
    //    |             |   +----------------------- ctor n argument
    //    |             |   |  +-------------------- expected result sequence
    //    |             |   |  |              +----- exception info
    //    |             |   |  |              |         0 - no exception
    //    |             |   |  |              |         1 - out_of_range
    //    |             |   |  |              |
    //    |             |   |  |              |
    //    V             V   V  V              V
    TEST ("ab",         0,  2, "ab",          0),    

    TEST ("",           0,  0, "",            0),   
    TEST ("\0",         0,  1, "\0",          0),
    TEST ("\0\0",       0,  2, "\0\0",        0),

    TEST ("a",          0,  1, "a",           0),       
    TEST ("bcd",        0,  3, "bcd",         0),       
    TEST ("cdefaihjb",  0,  9, "cdefaihjb",   0),      

    TEST ("\0\0ab",     0,  4, "\0\0ab",      0),  
    TEST ("a\0\0b",     0,  4, "a\0\0b",      0), 
    TEST ("ab\0\0",     0,  4, "ab\0\0",      0),  
    TEST ("abefdcc\0a", 0,  9, "abefdcc\0a",  0),  

    TEST ("x@4096",     0, 4096, "x@4096",    0),     

    TEST ("abc",        5,  3, "abc",         1), 
    TEST ("x@4096",  4106,  3, "xxx",         1), 

    TEST ("last test",  0,  9, "last test",   0)
};

/**************************************************************************/

// exercises:
// basic_string (size_type, value_type)
static const StringTestCase
size_val_test_cases [] = {

#undef TEST
#define TEST(size, val, res) {                      \
        __LINE__, -1, size, -1, -1, val,            \
        0, 0, 0, 0,                                 \
        res, sizeof res - 1, 0                      \
    }

    //    +---------------------------------------- ctor n argument 
    //    |          +----------------------------- source value
    //    |          |    +------------------------ expected result sequence
    //    |          |    |             
    //    |          |    |            
    //    V          V    V         
    TEST (1,        'a',  "a"), 
    TEST (1,        '\0', "\0"),

    TEST (2,        'a',  "aa"), 
    TEST (2,        '\0', "\0\0"),

    TEST (5,        '\0', "\0\0\0\0\0"),
    TEST (10,       'a',  "aaaaaaaaaa"), 

    TEST (4096,     'x',  "x@4096"),

    TEST (4,        't',  "tttt") 
};

/**************************************************************************/
// exercises:
// operator= (const value_type*)
static const StringTestCase
cptr_op_set_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {               \
        __LINE__, -1, -1, -1, -1, -1,               \
        str, sizeof str - 1, arg, sizeof arg - 1,   \
        res, sizeof res - 1, bthrow                 \
    }

    //    +----------------------------------------- source sequence
    //    |          +------------------------------ argument sequence
    //    |          |            +----------------- expected result sequence
    //    |          |            |              +-- exception info
    //    |          |            |              |      0 - no exception
    //    |          |            |              |     -1 - excpetion safety 
    //    |          |            |              |  
    //    |          |            |              |            
    //    V          V            V              V          
    TEST ("",       "ab",         "ab",          0),    

    TEST ("",       "",           "",            0),   
    TEST ("abc",    "",           "",            0),   
    TEST ("",       "\0",         "",            0), 
    TEST ("abc",    "\0",         "",            0),

    TEST ("",       "a",          "a",           0),  
    TEST ("\0\0",   "a",          "a",           0), 
    TEST ("a",      "bcd",        "bcd",         0),       
    TEST ("x@4096", "bcd",        "bcd",         0), 
    TEST ("",       "cdefaihjb",  "cdefaihjb",   0),      
    TEST ("a\0b",   "cdefaihjb",  "cdefaihjb",   0),

    TEST ("",       "\0\0ab",     "",            0),  
    TEST ("c\0d",   "\0\0ab",     "",            0),
    TEST ("",       "a\0\0b",     "a",           0), 
    TEST ("bcd",    "a\0\0b",     "a",           0), 
    TEST ("\0",     "ab\0\0",     "ab",          0), 
    TEST ("x@4096", "ab\0\0",     "ab",          0), 
    TEST ("\0a",    "abefdcc\0a", "abefdcc",     0),  
    TEST ("x@4096", "abefdcc\0a", "abefdcc",     0),

    TEST ("abc",    "x@4096",     "x@4096",      0),  

    TEST ("",       0,            "",            0),
    TEST ("a\0b\0", 0,            "a",           0), 
    TEST ("x@4096", 0,            "x@4096",      0), 

    TEST ("abcd",   "x@4096",     "x@4096",     -1), 

    TEST ("",       "last test",  "last test",   0)
};

/**************************************************************************/
// exercises:
// operator= (const basic_string&)
static const StringTestCase
cstr_op_set_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {               \
        __LINE__, -1, -1, -1, -1, -1,               \
        str, sizeof str - 1, arg, sizeof arg - 1,   \
        res, sizeof res - 1, bthrow                 \
    }

    //    +----------------------------------------- source sequence
    //    |          +------------------------------ argument sequence
    //    |          |            +----------------- expected result sequence
    //    |          |            |              +-- exception info
    //    |          |            |              |      0 - no exception
    //    |          |            |              |     -1 - excpetion safety 
    //    |          |            |              |  
    //    |          |            |              |            
    //    V          V            V              V
    TEST ("",       "ab",         "ab",          0),    

    TEST ("",       "",           "",            0),   
    TEST ("abc",    "",           "",            0),   
    TEST ("",       "\0",         "\0",          0), 
    TEST ("abc",    "\0",         "\0",          0),

    TEST ("",       "a",          "a",           0),  
    TEST ("\0\0",   "a",          "a",           0), 
    TEST ("a",      "bcd",        "bcd",         0),       
    TEST ("x@4096", "bcd",        "bcd",         0), 
    TEST ("",       "cdefaihjb",  "cdefaihjb",   0),      
    TEST ("a\0b",   "cdefaihjb",  "cdefaihjb",   0),

    TEST ("",       "\0\0ab",     "\0\0ab",      0),  
    TEST ("c\0d",   "\0\0ab",     "\0\0ab",      0),
    TEST ("",       "a\0\0b",     "a\0\0b",      0), 
    TEST ("bcd",    "a\0\0b",     "a\0\0b",      0), 
    TEST ("\0",     "ab\0\0",     "ab\0\0",      0), 
    TEST ("x@4096", "ab\0\0",     "ab\0\0",      0), 
    TEST ("\0a",    "abefdcc\0a", "abefdcc\0a",  0),  
    TEST ("x@4096", "abefdcc\0a", "abefdcc\0a",  0),

    TEST ("abc",    "x@4096",     "x@4096",      0),   

    TEST ("",       0,            "",            0),
    TEST ("a\0b\0", 0,            "a\0b\0",      0), 
    TEST ("x@4096", 0,            "x@4096",      0), 

    TEST ("",       "last test",  "last test",   0)
};


/**************************************************************************/

// exercises:
// operator= (value_type)
static const StringTestCase
val_op_set_test_cases [] = {

#undef TEST
#define TEST(str, val, res) {                 \
        __LINE__, -1, -1, -1, -1, val,        \
        str, sizeof str - 1, 0, 0,            \
        res, sizeof res - 1, 0                \
    }

    //    +----------------------------------- initial sequence
    //    |          +------------------------ source value
    //    |          |    +------------------- expected result sequence
    //    |          |    |             
    //    |          |    |            
    //    V          V    V         
    TEST ("",       'a',  "a"), 
    TEST ("abcdef", 'a',  "a"),
    TEST ("",       '\0', "\0"),
    TEST ("a",      '\0', "\0"),
    TEST ("\0\0",   'x',  "x"),
    TEST ("x@4096", 'x',  "x"),
    TEST ("",       't',  "t") 
};


/**************************************************************************/

template <class charT, class Traits, class Allocator, class Iterator>
void test_ctor_range (const charT*    warg,
                      std::size_t     warg_len,
                      std::size_t     res_len,
                      Traits*, Allocator*,
                      const Iterator &it,
                      const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;

    const char* const itname =
        tcase.arg ? type_name (it, (charT*)0) : "basic_string::iterator";

    /*const*/ String s_arg (warg, warg_len);

    std::size_t off_last = tcase.off + tcase.size;

    const StringIter it_first (std::size_t (tcase.off) >= s_arg.size () ?
                               s_arg.end () : s_arg.begin () + tcase.off);
    const StringIter it_last  (std::size_t (off_last) >= s_arg.size () ?
                               s_arg.end () : s_arg.begin () + off_last);

    const String s_str (it_first, it_last);

    const std::size_t match =
        rw_match (tcase.res, s_str.c_str(), tcase.nres);

    rw_assert (match == res_len, 0, tcase.line,
               "line %d. %{$FUNCALL} expected %{#*s}, got %{/*.*Gs}, "
               "difference at offset %zu for %s",
               __LINE__, int (tcase.nres), tcase.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_ctor_range (const charT* warg,
                      std::size_t  warg_len,
                      std::size_t  res_len,
                      Traits*, Allocator*, 
                      const StringTestCase &tcase)
{
    if (tcase.bthrow)  // this method doesn't throw
        return;

    test_ctor_range (warg, warg_len, res_len, (Traits*)0, (Allocator*)0, 
                     InputIter<charT>(0, 0, 0), tcase);

    test_ctor_range (warg, warg_len, res_len, (Traits*)0, (Allocator*)0,
                     ConstFwdIter<charT>(0, 0, 0), tcase);

    test_ctor_range (warg, warg_len, res_len, (Traits*)0, (Allocator*)0,
                     ConstBidirIter<charT>(0, 0, 0), tcase);

    test_ctor_range (warg, warg_len, res_len, (Traits*)0, (Allocator*)0,
                     ConstRandomAccessIter<charT>(0, 0, 0), tcase);
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_ctor (charT, Traits*, Allocator*,
                const StringFunc     &func,
                const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;

    static const std::size_t BUFSIZE = 256;

    static charT warg_buf [BUFSIZE];
    std::size_t arg_len = sizeof warg_buf / sizeof *warg_buf;
    charT* warg = rw_expand (warg_buf, tcase.arg, tcase.arg_len, &arg_len);

    static charT wres_buf [BUFSIZE];
    std::size_t res_len = sizeof wres_buf / sizeof *wres_buf;
    charT* wres = rw_expand (wres_buf, tcase.res, tcase.nres, &res_len);

    if (Ctor (range) == func.which_) {
        test_ctor_range (warg, arg_len, res_len, 
                        (Traits*)0, (Allocator*)0, tcase);

        if (warg != warg_buf)
            delete[] warg;

        if (wres != wres_buf)
            delete[] wres;

        return;
    }

    // construct the string object to be modified
    // and the (possibly unused) argument string
    const String  arg (warg, arg_len, Allocator ());
    if (warg != warg_buf)
        delete[] warg;

    warg = 0;

    // offset and extent function arguments
    const std::size_t arg_off  = std::size_t (tcase.off);
    const std::size_t arg_size = std::size_t (tcase.size);

    // string function argument
    const charT* const arg_ptr = arg.c_str ();
    const String&      arg_str = arg;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    // (name of) expected and caught exception
    const char* expected = 0;
    const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

    if (1 == tcase.bthrow)
        expected = exceptions [1];      // out_of_range
    else if (2 == tcase.bthrow)
        expected = exceptions [2];      // length_error

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow) {
        if (wres != wres_buf)
            delete[] wres;

        return;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // pointer to the returned reference
    String* ret_ptr = 0;

    // start checking for memory leaks
    rwt_check_leaks (0, 0);

    try {
        switch (func.which_) {

        case Ctor (void):
            ret_ptr = new String ();
            break;

        case Ctor (alloc):
            ret_ptr = new String (Allocator ());
            break;

        case Ctor (cptr):
            ret_ptr = new String (arg_ptr);
            break;

        case Ctor (cptr_alloc):
            ret_ptr = new String (arg_ptr, Allocator ());
            break;

        case Ctor (cstr):
            ret_ptr = new String (arg_str);
            break;

        case Ctor (cptr_size):
            ret_ptr = new String (arg_ptr, arg_size);
            break;

        case Ctor (cptr_size_alloc):
            ret_ptr = new String (arg_ptr, arg_size, Allocator ());
            break;

        case Ctor (cstr_size):
            ret_ptr = new String (arg_str, arg_off);
            break;

        case Ctor (cstr_size_size):
            ret_ptr = new String (arg_str, arg_off, arg_size);
            break;

        case Ctor (cstr_size_size_alloc):
            ret_ptr = new String (arg_str, arg_off, arg_size, Allocator ());
            break;

        case Ctor (size_val):
            ret_ptr = new String (tcase.size, arg_val);
            break;

        case Ctor (size_val_alloc):
            ret_ptr = new String (tcase.size, arg_val, Allocator ());
            break;

        default:
            RW_ASSERT (!"logic error: unknown constructor overload");
        }
            
        // verify that returned pointer is valid
        rw_assert (0 != ret_ptr, 0, tcase.line,
                   "line %d. %{$FUNCALL} expected %{#*s}, got null",
                   __LINE__, int (res_len), tcase.res);

        if (0 != ret_ptr) {
            // verify the length of the resulting string
            rw_assert (res_len == ret_ptr->size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{#*s} with length "
                       "%zu, got %{/*.*Gs} with length %zu",
                       __LINE__, int (res_len), tcase.res, 
                       res_len, int (sizeof (charT)), 
                       int (ret_ptr->size ()), ret_ptr->c_str (), 
                       ret_ptr->size ());

            if (Ctor (void) != func.which_) {
                // verify the capacity of the resulting string
                rw_assert (ret_ptr->size () <= ret_ptr->capacity (), 0, 
                           tcase.line, "line %d. %{$FUNCALL} expected "
                           "capacity () >= size (), got capacity () == "
                           "%zu, size () == %zu", __LINE__,   
                           ret_ptr->capacity (), ret_ptr->size ());
            }

            if (res_len == ret_ptr->size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, ret_ptr->c_str (), tcase.nres);

                rw_assert (match == res_len, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %{/*.*Gs}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, int (sizeof (charT)), int (res_len), 
                           wres, int (sizeof (charT)), 
                           int (ret_ptr->size ()), ret_ptr->c_str (), 
                           match);
            }

            delete ret_ptr;
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
        rw_assert (-1 == tcase.bthrow, 0, tcase.line,
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

    /* const */ std::size_t nbytes;
    const       std::size_t nblocks = rwt_check_leaks (&nbytes, 0);

    // FIXME: verify the number of blocks the function call
    // is expected to allocate and detect any memory leaks
    const std::size_t expect_blocks = nblocks;

    rw_assert (nblocks == expect_blocks, 0, tcase.line,
               "line %d. %{$FUNCALL} allocated %td bytes in %td blocks",
               __LINE__, nbytes, expect_blocks);

    if (wres != wres_buf)
        delete[] wres;
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_op_set (charT, Traits*, Allocator*,
                  const StringFunc     &func,
                  const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename UserTraits<charT>::MemFun           UTMemFun;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    static charT warg_buf [BUFSIZE];

    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    std::size_t arg_len = sizeof warg_buf / sizeof *warg_buf;

    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);
    charT* warg = rw_expand (warg_buf, tcase.arg, tcase.arg_len, &arg_len);

    // construct the string object to be modified
    // and the (possibly unused) argument string
    /* const */ String  str (wstr, str_len, Allocator ());
    const       String  arg (warg, arg_len, Allocator ());

    if (wstr != wstr_buf)
        delete[] wstr;

    if (warg != warg_buf)
        delete[] warg;

    wstr = 0;
    warg = 0;

    static charT wres_buf [BUFSIZE];
    std::size_t res_len = sizeof wres_buf / sizeof *wres_buf;
    charT* wres = rw_expand (wres_buf, tcase.res, tcase.nres, &res_len);

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

    // string function argument
    const charT* const arg_ptr = tcase.arg ? arg.c_str () : str.c_str ();
    const String&      arg_str = tcase.arg ? arg : str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    std::size_t total_length_calls = 0;
    std::size_t n_length_calls = 0;
    std::size_t* const rg_calls = OpSet (cptr) == func.which_ ?
        rw_get_call_counters ((Traits*)0, (charT*)0) : 0;

    if (rg_calls)
        total_length_calls = rg_calls [UTMemFun::length];

    rwt_free_store* const pst = rwt_get_free_store (0);

    // iterate for`throw_after' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_after;
    for (throw_after = 0; ; ++throw_after) {

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        if (-1 == tcase.bthrow) {
            expected = exceptions [3];      // bad_alloc
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow) {
            if (wres != wres_buf)
                delete[] wres;

            return;
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        // start checking for memory leaks
        rwt_check_leaks (0, 0);

        try {
            switch (func.which_) {

            case OpSet (cptr):
                str = arg_ptr;
                break;

            case OpSet (cstr):
                str = arg_str;
                break;

            case OpSet (val):
                str = arg_val;
                break;

            default:
                RW_ASSERT (!"logic error: unknown operator= overload");
            }
            
            // verify the length of the resulting string
            rw_assert (res_len == str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{#*s} with length "
                       "%zu, got %{/*.*Gs} with length %zu",
                       __LINE__, int (res_len), tcase.res, 
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
                            __LINE__, int (sizeof (charT)), int (res_len), 
                            wres, int (sizeof (charT)), 
                            int (str.size ()), str.c_str (), match);
            }

            // verify that Traits::length was used
            if (rg_calls) {
                rw_assert (n_length_calls - total_length_calls > 0, 
                           0, tcase.line, "line %d. %{$FUNCALL} doesn't "
                           "use traits::length()", __LINE__);
            }
        }

#ifndef _RWSTD_NO_EXCEPTIONS

        catch (const std::bad_alloc &ex) {
            caught = exceptions [3];
            rw_assert (-1 == tcase.bthrow, 0, tcase.line,
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

        /* const */ std::size_t nbytes;
        const       std::size_t nblocks = rwt_check_leaks (&nbytes, 0);

        // FIXME: verify the number of blocks the function call
        // is expected to allocate and detect any memory leaks
        const std::size_t expect_blocks = nblocks;

        rw_assert (nblocks == expect_blocks, 0, tcase.line,
                   "line %d. %{$FUNCALL} allocated %td bytes in %td blocks",
                   __LINE__, nbytes, expect_blocks);

        if (caught) {
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object
            str_state.assert_equal (rw_get_string_state (str),
                                    __LINE__, tcase.line, caught);
            if (-1 == tcase.bthrow) {
                // allow this call to operator new to succeed and try
                // to make the next one to fail during the next call
                // to the same function again
                continue;
            }
        }
        else if (-1 != tcase.bthrow) {
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
    rw_assert (   *pst->throw_at_calls_ [0] == std::size_t (-1)
               || throw_after,
               0, tcase.line,
               "line %d: %{$FUNCALL}: failed to throw an expected exception",
               __LINE__);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    *pst->throw_at_calls_ [0] = std::size_t (-1);

    if (wres != wres_buf)
        delete[] wres;
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_cons (charT, Traits*, Allocator*,
                const StringFunc     &func,
                const StringTestCase &tcase)
{
    if (StringIds::fid_op_set == (func.which_ & StringIds::fid_mask))
        test_op_set (charT (), (Traits*)0, (Allocator*)0, func, tcase);
    else
        test_ctor (charT (), (Traits*)0, (Allocator*)0, func, tcase);
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_cons);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Ctor (sig), sig ## _test_cases,                         \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (void),
        TEST (cptr),
        TEST (cstr),
        TEST (cptr_size),
        TEST (cstr_size),
        TEST (cstr_size_size),
        TEST (size_val),
        TEST (range),

#undef TEST
#define TEST(sig) {                             \
        OpSet (sig), sig ## _op_set_test_cases, \
          sizeof sig ## _op_set_test_cases      \
        / sizeof *sig ## _op_set_test_cases     \
    }

        TEST (cptr),
        TEST (cstr),
        TEST (val)

    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.cons",
                               test_cons, tests, test_count);
}
