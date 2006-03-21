/***************************************************************************
 *
 * 21.string.capacity.cpp - test exercising [lib.string.capacity]
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

#include <string>      // for string
#include <cstddef>     // for size_t
#include <stdexcept>   // for length_error

#include <cmdopt.h>    // for rw_enabled()
#include <driver.h>    // for rw_test()
#include <rw_char.h>   // for rw_match(), rw_widen()

/**************************************************************************/

struct MemFun
{
    enum charT { Char, WChar };
    enum Traits { DefaultTraits, UserTraits };
    enum FunTag {
        // which member function to exercise
        size, resize, length, reserve, capacity, max_size, clear, empty
    };

    MemFun (charT cid, const char *cname,
          Traits tid, const char *tname)
        : cid_ (cid), tid_ (tid), mfun_ (),
          cname_ (cname), tname_ (tname), aname_ ("allocator"),
          fname_ (0),
          max_size_ (0) { /* empty */ }

    charT       cid_;     // character type id (char or wchar_t)
    Traits      tid_;     // traits type id (default or user-defined)
    FunTag      mfun_;    // member function id
    const char *cname_;   // character type name
    const char *tname_;   // traits name
    const char *aname_;   // allocator name
    const char *fname_;   // function name

    unsigned int max_size_;
};

/**************************************************************************/

static const size_t long_string_len = 4096U;
static char long_string [long_string_len];

// for convenience and brevity
#define LSTR   long_string
#define LLEN   long_string_len

/**************************************************************************/

template <class charT, class String>
void test_resize (charT, const MemFun *pfid,
                  int         line,         // line number
                  String     &test_str,     // tested string object
                  const char *str,          // source string argument
                  std::size_t str_len,      // the string length
                  std::size_t nparam,       // resize() first argument
                  int         cparam,       // resize() second argument
                  bool        should_throw) // if true the method should throw
{
    typedef unsigned char UChar;

    const charT char_eos   = charT ('\0');
    const charT char_param = -1 == cparam ? char_eos : charT (UChar (cparam));

#ifndef _RWSTD_NO_EXCEPTIONS

    bool ex_thrown = false;
    try {

#endif    // _RWSTD_NO_EXCEPTIONS

    if (-1 == cparam)
        test_str.resize (nparam);
    else
        test_str.resize (nparam, char_param);

#ifndef _RWSTD_NO_EXCEPTIONS

    }
    catch (std::length_error) {
        ex_thrown = true;
    }
    catch (...) {
    }

    rw_assert (should_throw == ex_thrown, 0, line,
               "line %d. basic_string<%s, %s<%2$s>, %s<%2$s>>"
               "(%{#*s}, %zu).resize(%zu%{?}, %{#c}%{;}) "
               "should throw == %b, was thrown == %b",
               __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,
               int (str_len), str, str_len,
               nparam, -1 != cparam, char_param, should_throw, ex_thrown);

    if (ex_thrown)
        return;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    _RWSTD_UNUSED (should_throw);

#endif   // _RWSTD_NO_EXCEPTIONS

    // verify the size of the test string
    rw_assert (test_str.size () == nparam, 0, line,
               "line %d. basic_string<%s, %s<%2$s>, %s<%2$s>>"
               "(%{#*s}, %zu).resize(%zu%{?}, %{#c}%{;}).size() == "
               "%zu, got %zy",
               __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,
               int (str_len), str, str_len, nparam, -1 != cparam, char_param,
               nparam, test_str.size ());

    // create the expected string
    char* const expect_str = new char [nparam + 1];
    rw_widen (expect_str, str, str_len < nparam ? str_len : nparam);

    for (std::size_t i = str_len; i < nparam; ++i)
        expect_str [i] = -1 == cparam ? '\0' : char (cparam);

    // verify that the test_string matches the expected result
    const std::size_t inx = rw_match (expect_str, test_str.data (), nparam);

    rw_assert (inx == nparam, 0, line,
               "line %d. basic_string<%s, %s<%2$s>, %s<%2$s>>"
               "(%{#*s}, %zu).resize(%zu%{?}, %{#c}%{;}) == "
               "%{#*s}, got %{#*.*Ac}",
               __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,
               int (str_len), str, str_len, nparam, -1 != cparam, char_param,
               int (nparam), expect_str,
               int (sizeof (charT)), int (test_str.size ()), test_str.data ());

    delete[] expect_str;
}

/**************************************************************************/

template <class charT, class Traits>
void test_capacity (charT, Traits*, const MemFun *pfid,
                    int         line,         // line number
                    const char *str,          // string argument
                    std::size_t str_len,      // the string length
                    std::size_t nparam,       // method parameter
                    char        cparam,       // method parameter char
                    std::size_t res,          // method expected result
                    bool        should_throw) // the method should throw
{
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> TestString;

    if (!rw_enabled (line)) {
        rw_note (0, 0, 0, "test on line %d disabled", line);
        return;
    }

    // widen the source sequence into the (possibly wide) character buffer
    static charT wstr [LLEN];
    rw_widen (wstr, str, str_len);

    // construct a test string object either using the specified
    // arguments or using the default ctor
    TestString test_str = str ? TestString (wstr, str_len) : TestString ();

    if (MemFun::resize == pfid->mfun_) {
        test_resize (charT (), pfid, line, test_str, str, str_len,
                     nparam, cparam, should_throw);
        return;
    }

    std::string::size_type ret     = 0;
    std::string::size_type exp_ret = res;

#ifndef _RWSTD_NO_EXCEPTIONS

    const char* const expected = should_throw ? "length_error" : 0;
    const char*       caught   = 0;

    try {

#endif   // _RWSTD_NO_EXCEPTIONS

    // invoke the virtual function with the expected argument (if any)
    switch (pfid->mfun_) {
    case MemFun::size:
        ret = test_str.size ();
        break;

    case MemFun::length:
        ret     = test_str.length ();
        exp_ret = test_str.size ();
        break;

    case MemFun::resize:
        // do nothing, handled above
        break;

    case MemFun::reserve:
        0 == nparam ? test_str.reserve () : test_str.reserve (nparam);
        break;

    case MemFun::capacity:
        ret = test_str.capacity ();
        break;

    case MemFun::max_size:
        ret = test_str.max_size ();
        break;

    case MemFun::empty:
        ret     = test_str.empty ();
        exp_ret = 0 == test_str.size ();
        break;

    case MemFun::clear:
        test_str.clear ();
        break;
    }

#define CALLFMAT                                                        \
    "line %d. basic_string<%s, %s<%2$s>, %s<%2$s>>(%{?}%{#*S}%{;})"     \
    ".%s(%{?}%zu%{;})"

#define CALLARGS                                                \
    __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,         \
    0 != str, int (sizeof (charT)), &test_str, pfid->fname_,    \
    MemFun::reserve == pfid->mfun_, nparam

#ifndef _RWSTD_NO_EXCEPTIONS

    }
    catch (std::length_error) {
        caught = expected;
    }
    catch (...) {
        caught = "unknown exception";
    }

    rw_assert (caught == expected, 0, line,
              CALLFMAT " %{?}expected %s, caught %s"
               "%{:}unexpectedly caught %s%{;}",
               CALLARGS, 0 != expected, expected, caught, caught);

    if (should_throw || caught)
        return;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)
    _RWSTD_UNUSED (should_throw);
#endif   // _RWSTD_NO_EXCEPTIONS

    // check the results
    if (MemFun::size == pfid->mfun_ || MemFun::length == pfid->mfun_ ||
        MemFun::empty == pfid->mfun_) {

        rw_assert (exp_ret == ret, 0, line,
                   CALLFMAT " == %zu, expected %zu",
                   CALLARGS, ret, exp_ret);

        return;
    }

    if (MemFun::capacity == pfid->mfun_) {
        std::string::size_type cur_sz = test_str.size();
        std::string::size_type max_sz = test_str.max_size();

        rw_assert (cur_sz <= ret && ret <= max_sz, 0, line,
                   CALLFMAT " == %zu, expected %zu < res < %zu",
                   CALLARGS, ret, cur_sz, max_sz);
    }

    if (MemFun::max_size == pfid->mfun_) {
        std::string::size_type cur_sz = test_str.size();

        rw_assert (cur_sz <= ret, 0, line,
                   CALLFMAT " == %zu, expected res > %zu",
                   CALLARGS, ret, cur_sz);
    }

    if (MemFun::reserve == pfid->mfun_ ) {
        ret = test_str.capacity ();
        rw_assert (nparam <= ret, 0, line,
                   CALLFMAT ": capacity() >= %zu, got %zu",
                   CALLARGS, nparam, ret);
        return;
    }

    if (MemFun::clear == pfid->mfun_ ) {
        rw_assert (test_str.empty (), 0, line,
                   CALLFMAT ": string not empty", CALLARGS);
        return;
    }
}

/**************************************************************************/

void test_capacity (MemFun      *pfid,
                    int          line,
                    const char  *str,
                    std::size_t  str_len,
                    int          nparam,
                    int          cparam,
                    std::size_t  res,
                    bool         should_throw)
{
#undef TEST
#define TEST(charT, Traits)                                             \
    test_capacity (charT (), (Traits*)0, pfid, line,                    \
                   str, str_len, nparam, cparam, res, should_throw)

    static const char* const fnames[] = {
        "size", "resize", "length", "reserve", "capacity", "max_size",
        "clear", "empty"
    };

    if (!rw_enabled (line)) {
        rw_note (0, 0, __LINE__, "test on line %d disabled", line);
        return;
    }

    pfid->fname_ = fnames [pfid->mfun_];

    pfid->max_size_ = MemFun::Char == pfid->cid_ ?
        _RWSTD_SIZE_MAX / sizeof (char) - 1
      : _RWSTD_SIZE_MAX / sizeof (wchar_t) - 1;

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
        else
            TEST (wchar_t, UserTraits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T

    }
}

/**************************************************************************/

void test_size (MemFun *pfid)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::size ()",
             pfid->cname_, pfid->tname_, pfid->aname_);

#undef TEST
#define TEST(str, size)                                 \
    test_capacity (pfid, __LINE__, str, sizeof str - 1, \
                   0, 0, size, false)

    //    +--------------------------------------- controlled sequence
    //    |                +---------------------- expected result
    //    |                |                   
    //    |                |                   
    //    V                V                   
    TEST (0,               0);
    TEST ("",              0);

    TEST ("\0",            1);
    TEST ("a",             1);
    TEST (" ",             1);
    TEST ("ab",            2);
    TEST ("bc",            2);

    TEST ("test string",  11);
    TEST ("Test String",  11);

    TEST ("t\0 s",         4);
    TEST ("Test\0string", 11);

    TEST ("\0a\0b",        4);
    TEST ("a\0\0b",        4);
    TEST ("a\0\0\0b",      5);
    TEST ("a\0\0b\0",      5);
    TEST ("a\0b\0\0c",     6);
    TEST ("a\0b\0c\0\0",   7);

    TEST (LSTR,     LLEN - 1);
}

/**************************************************************************/

static int rw_opt_no_size;          // for --no-size
static int rw_opt_no_resize;        // for --no-resize
static int rw_opt_no_length;        // for --no-length
static int rw_opt_no_reserve;       // for --no-reserve
static int rw_opt_no_capacity;      // for --no-capacity
static int rw_opt_no_max_size;      // for --no-max_size
static int rw_opt_no_clear;         // for --no-clear
static int rw_opt_no_empty;         // for --no-empty
static int rw_opt_no_exceptions;    // for --no-exceptions
static int rw_opt_no_char_traits;   // for --no-char_traits
static int rw_opt_no_user_traits;   // for --no-user_traits

/**************************************************************************/

void test_resize (MemFun *pfid)
{
    rw_info (0, 0, 0,
             "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::resize "
             "(size_type, char_type)",
             pfid->cname_, pfid->tname_, pfid->aname_);

#undef TEST
#define TEST(str, len, nparam, cparam, ex_throw)        \
    test_capacity (pfid, __LINE__, str, len, nparam,    \
                   int (cparam), 0, ex_throw)

    //    +------------------------------------- controlled sequence
    //    |                +-------------------- controlled sequence length
    //    |                |        +----------- resize() integer argument
    //    |                |        |   +------- resize() char argument
    //    |                |        |   |    +-- exception expected?
    //    |                |        |   |    |
    //    V                V        V   V    V
    TEST ("\0",            0,       0,  -1,  false);
    TEST ("\0",            0,       0, 'a',  false);
    TEST ("\0",            0,      10, 'a',  false);

    TEST ("a",             1,       1,  -1,  false);
    TEST ("a",             1,       1, 'a',  false);
    TEST ("a",             1,       0, 'a',  false);
    TEST ("a",             1,      10, 'a',  false);

    TEST ("ab",            2,       2,  -1,  false);
    TEST ("ab",            2,       1, 'a',  false);
    TEST ("ab",            2,      10, 'a',  false);
    TEST ("ab",            2,      10, 'a',  false);

    TEST ("t\0 s",         4,       6,  -1,  false);
    TEST ("t\0 s",         4,       6, 'a',  false);
    TEST ("Test\0string", 11,     100, 'a',  false);

    TEST ("a\0\0\0b",     5,       10,  -1,  false);
    TEST ("a\0\0\0b",     5,       10, 'a',  false);
    TEST ("a\0\0\0b",     5,       10, '\0', false);

    TEST ("a\0b\0c\0\0",  7,       10,  -1,  false);
    TEST ("a\0b\0c\0\0",  7,       10, 'a',  false);
    TEST ("a\0b\0c\0\0",  7,       10, '\0', false);

    TEST ("bc",           2, LLEN - 1,  -1,  false);
    TEST ("bc",           2, LLEN - 1, 'a',  false);

    TEST (LSTR,    LLEN - 1,       10,  -1, false);
    TEST (LSTR,    LLEN - 1,       10, 'a', false);
    TEST (LSTR,    LLEN - 1, LLEN - 1, 'a', false);

#ifndef _RWSTD_NO_EXCEPTIONS

    if (rw_opt_no_exceptions)
        rw_note (0, 0, __LINE__, "exceptions tests disabled");
    else {
        if (_RWSTD_SIZE_MAX > pfid->max_size_) {

            TEST ("\0", 1, pfid->max_size_ + 1, 'a', true);
            TEST ("a" , 1, pfid->max_size_ + 1, 'a', true);
            TEST (LSTR, LLEN - 1,
                  pfid->max_size_ + 1, 'a', true);
        }
    }

#endif   //_RWSTD_NO_EXCEPTIONS

    rw_info (0, 0, 0,
             "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::resize (size_type)",
             pfid->cname_, pfid->tname_, pfid->aname_);

#undef TEST
#define TEST(str, nparam, ex_throw)                     \
    test_capacity (pfid, __LINE__, str, sizeof str - 1, \
                   nparam, -1, 0, ex_throw)

    //    +---------------------------------------- controlled sequence
    //    |             +-------------------------- resize() integer argument
    //    |             |                       +-- exception expected?
    //    |             |                       |
    //    V             V                       V
    TEST ("\0",         0,                      false);
    TEST ("\0",        10,                      false);

    TEST ("a",          1,                      false);
    TEST ("a",          0,                      false);
    TEST ("a",         10,                      false);

    TEST ("ab",         2,                      false);
    TEST ("ab",         1,                      false);
    TEST ("ab",        10,                      false);

    TEST ("bc",        LLEN - 1,     false);

    TEST (LSTR, 10,                      false);
    TEST (LSTR, LLEN - 1,     false);

#ifndef _RWSTD_NO_EXCEPTIONS

    if (!rw_opt_no_exceptions) {
        if (_RWSTD_SIZE_MAX > pfid->max_size_) {

            TEST ("\0",        pfid->max_size_ + 1,     true);
            TEST ("a",         pfid->max_size_ + 1,     true);
            TEST (LSTR, pfid->max_size_ + 1,     true);
        }
    }

#endif   // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

void test_length (MemFun *pfid)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::length ()",
             pfid->cname_, pfid->tname_, pfid->aname_);

#undef TEST
#define TEST(str, size)                                 \
    test_capacity (pfid, __LINE__, str, sizeof str - 1, \
                          0, 0, size, false)

    //    +--------------------------------------- controlled sequence
    //    |                +---------------------- expected result
    //    |                |                   
    //    |                |                   
    //    V                V 
    TEST (0,               0);
    TEST ("",              0);

    TEST ("\0",            1);
    TEST ("a",             1);
    TEST (" ",             1);
    TEST ("ab",            2);
    TEST ("bc",            2);

    TEST ("Test String",  11);

    TEST ("t\0 s",         4);
    TEST ("Test\0string", 11);
   
    TEST ("\0a\0b",        4);
    TEST ("a\0\0b",        4);
    TEST ("a\0\0\0b",      5);
    TEST ("a\0\0b\0",      5);
    TEST ("a\0b\0\0c",     6);
    TEST ("a\0b\0c\0\0",   7);

    TEST (LSTR,     LLEN - 1);
}

/**************************************************************************/

void test_reserve (MemFun *pfid)
{
    rw_info (0, 0, 0,
             "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::reserve (size_type)",
             pfid->cname_, pfid->tname_, pfid->aname_);

#undef TEST
#define TEST(str, nparam, ex_throw)                     \
    test_capacity (pfid, __LINE__, str, sizeof str - 1, \
                          nparam, 0, 0, ex_throw)

    //    +--------------------------------------- controlled sequence
    //    |                +---------------------- reserve() argument
    //    |                |                   +-- exception expected?
    //    |                |                   |
    //    V                V                   V
    TEST (0,               0,                  false);
    TEST (0,              10,                  false);
    TEST (0,              LLEN - 1, false);

    TEST ("\0",            0,                  false);
    TEST ("\0",           10,                  false);
    TEST ("\0",           LLEN - 1, false);

    TEST ("abcd",          0,                  false);
    TEST ("abcd",          2,                  false);
    TEST ("abcd",          4,                  false);
    TEST ("abcd",         10,                  false);
    TEST ("abcd",         LLEN - 1, false);

    TEST ("t\0 s",         1,                  false);
    TEST ("Test\0string",  4,                  false);
    TEST ("a\0\0b",        2,                  false);
    TEST ("a\0\0b",       10,                  false);
    TEST ("a\0b\0c\0\0",   4,                  false);
    TEST ("a\0b\0c\0\0",  10,                  false);

    TEST (LSTR,           10,                  false);
    TEST (LSTR,     LLEN - 1,                  false);

#ifndef _RWSTD_NO_EXCEPTIONS

    if (_RWSTD_SIZE_MAX > pfid->max_size_) {
        TEST ("\0", pfid->max_size_ + 1, true);
        TEST ("a",  pfid->max_size_ + 1, true);
        TEST (LSTR, pfid->max_size_ + 1, true);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

void test_capacity (MemFun *pfid)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::capacity ()",
             pfid->cname_, pfid->tname_, pfid->aname_);

#undef TEST
#define TEST(str, size)                                 \
    test_capacity (pfid, __LINE__, str, sizeof str - 1, \
                          0, 0, size, false)

    //    +--------------------------------------- controlled sequence
    //    |                +---------------------- expected result
    //    |                |                   
    //    |                |                   
    //    V                V 
    TEST (0,                0);
    TEST ("\0",             0);

    TEST ("a",            128);
    TEST ("abcd",         128);

    TEST ("t\0 s",        128);
    TEST ("Test\0string", 128);

    TEST ("\0a\0b",       128);
    TEST ("a\0\0\0b",     128);
    TEST ("a\0b\0c\0\0",  128);

    TEST (LSTR,       LLEN - 1);
}

/**************************************************************************/

void test_max_size (MemFun *pfid)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::max_size ()",
             pfid->cname_, pfid->tname_, pfid->aname_);

#undef TEST
#define TEST(str)                                       \
    test_capacity (pfid, __LINE__, str, sizeof str - 1, \
                          0, 0, 0, false)

    TEST (0);
    TEST ("\0");
    TEST ("abcd");
    TEST (LSTR);
}

/**************************************************************************/

void test_clear (MemFun *pfid)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::clear ()",
             pfid->cname_, pfid->tname_, pfid->aname_);

#undef TEST
#define TEST(str)                                       \
    test_capacity (pfid, __LINE__, str, sizeof str - 1, \
                          0, 0, 0, false)

    TEST (0);
    TEST ("\0");
    TEST ("a");
    TEST ("ab");
    TEST ("abcde");

    TEST ("t\0 s");
    TEST ("Test\0string");

    TEST ("\0a\0b");
    TEST ("a\0\0\0b");
    TEST ("a\0b\0c\0\0");

    TEST (LSTR);
}

/**************************************************************************/

void test_empty (MemFun *pfid)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::empty ()",
             pfid->cname_, pfid->tname_, pfid->aname_);

    pfid->mfun_ = MemFun::empty;

#undef TEST
#define TEST(str)                                       \
    test_capacity (pfid, __LINE__, str, sizeof str - 1, \
                          0, 0, 0, false)

    TEST (0);
    TEST ("\0");
    TEST ("a");
    TEST ("ab");
    TEST ("abcde");

    TEST ("\0 s");
    TEST ("t\0 s");
    TEST ("Test\0string");

    TEST ("\0a\0b");
    TEST ("a\0\0\0b");
    TEST ("a\0b\0c\0\0");

    TEST (LSTR);
}

/**************************************************************************/

static void
run_test (MemFun *pfid)
{

#undef TEST
#define TEST(function)                                  \
    if (rw_opt_no_ ## function)                         \
        rw_note (1 < rw_opt_no_ ## function++, 0, 0,    \
                 "%s test disabled", #function);        \
    else  {                                             \
        pfid->mfun_ = MemFun::function;                 \
        test_ ## function (pfid);                       \
    } (void)0

    if (MemFun::UserTraits == pfid->tid_ && rw_opt_no_user_traits) {
        rw_note (1 < rw_opt_no_user_traits++, 0, 0,
                 "user defined traits test disabled");
    }
    else if (MemFun::DefaultTraits == pfid->tid_ && rw_opt_no_char_traits) {
        rw_note (1 < rw_opt_no_char_traits++, 0, 0,
                 "char_traits test disabled");
    }
    else {
        TEST (size);
        TEST (resize);
        TEST (length);
        TEST (reserve);
        TEST (capacity);
        TEST (max_size);
        TEST (clear);
        TEST (empty);        
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
        rw_note (0, 0, 0, "char tests disabled");

    if (rw_enabled ("wchar_t")) {

        MemFun fid (MemFun::WChar, "wchar_t", MemFun::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = MemFun::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "wchar_t tests disabled");

    return 0;

}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.capacity",
                    0 /* no comment */,
                    run_test,
                    "|-no-size# "
                    "|-no-resize# "
                    "|-no-length# "
                    "|-no-reserve# "
                    "|-no-capacity# "
                    "|-no-max_size# "
                    "|-no-clear# "
                    "|-no-empty# "
                    "|-no-exceptions# "
                    "|-no-char_traits# "
                    "|-no-user_traits#",
                    &rw_opt_no_size,
                    &rw_opt_no_resize,
                    &rw_opt_no_length,
                    &rw_opt_no_reserve,
                    &rw_opt_no_capacity,
                    &rw_opt_no_max_size,
                    &rw_opt_no_clear,
                    &rw_opt_no_empty,
                    &rw_opt_no_exceptions,
                    &rw_opt_no_char_traits,
                    &rw_opt_no_user_traits);
}
