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
#include <rw_char.h>   // for rw_widen()

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

static const int long_string_len = 4096;
static char long_string [long_string_len];

/**************************************************************************/

template <class charT, class String>
void test_resize (charT, const MemFun *pfid,
                  int         line,         // line number
                  String     *pstr,         // pointer string object
                  const char *str,          // source string argument
                  std::size_t str_len,      // the string length
                  std::size_t nparam,       // method parameter
                  char        cparam,       // method parameter char
                  bool        should_throw) // if true the method should throw
{
    typedef unsigned char UChar;
    const charT char_param = charT (UChar (cparam));
    const charT char_eos   = charT ('\0');

    bool resize2args = charT (UChar (-1)) != char_param;

#ifndef _RWSTD_NO_EXCEPTIONS

    bool ex_thrown = false;
    try {

#endif    // _RWSTD_NO_EXCEPTIONS

    if (resize2args)
        pstr->resize (nparam, char_param);
    else
        pstr->resize (nparam);

#ifndef _RWSTD_NO_EXCEPTIONS

    }
    catch (std::length_error) {
        ex_thrown = true;
    }

    rw_assert (should_throw == ex_thrown, 0, line,
               "line %d. basic_string<%s, %s<%2$s>, %s<%2$s>>"
               "(%{#*s}, %zu).resize(%zu%{?}, %{#c}%{;}) "
               "should throw == %b, was thrown == %b",
               __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,
               int (str_len), str, str_len,
               nparam, resize2args, char_param, should_throw, ex_thrown);

    if (ex_thrown)
        return;

#else   // _RWSTD_NO_EXCEPTIONS

    _RWSTD_UNUSED (should_throw);

#endif

    // check the results
    static charT wstr_tmp [long_string_len];
    rw_widen (wstr_tmp, str, str_len);

    std::size_t ubound = nparam < str_len ? nparam : str_len;
    bool success = true;
    std::size_t i = 0;
    for (; i < ubound; i++) {
        success = wstr_tmp[i] == pstr->c_str()[i];
        if (!success)
            break;
    }

    if (0 < ubound) {
        // to avoid errors in --trace mode
        i = i < ubound ? i : ubound - 1;

        rw_assert (success, 0, line,
                   "line %d. basic_string<%s, %s<%2$s>, %s<%2$s>>"
                   "(%{#*s}, %zu).resize(%zu%{?}, %{#c}%{;}): "
                   "got %{#c} at %zu, expected %{#c}",
                   __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,
                   int (str_len), str, str_len,
                   nparam, resize2args, char_param,
                   pstr->c_str()[i], i + 1, wstr_tmp[i]);
    }

    if (resize2args) {
        i = ubound;
        ubound = str_len < nparam ? nparam : 0;
        for (; i < ubound; i++) {
            success = char_param == pstr->c_str()[i];
            if (!success)
                break;
        }

        if (0 < ubound) {
            // to avoid errors in --trace mode
            i = i < ubound ? i : ubound - 1;

            rw_assert (success, 0, line,
                       "line %d. basic_string<%s, %s<%2$s, %s<%2$s>>"
                       "(%{#*s}, %zu).resize(%zu, %{#c}): "
                       "got %{?}%{#c}%{;}%{?}'%s'%{;} at %zu, expected %{#c}",
                       __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,
                       int (str_len), str, str_len, nparam, cparam,
                       char_eos != pstr->c_str()[i], pstr->c_str()[i],
                       char_eos == pstr->c_str()[i], "eof", i + 1,
                       char_param);
        }
    }
    else {
        const std::string::size_type sz_tmp = pstr->size ();
        rw_assert (nparam == sz_tmp, 0, line,
                   "line %d. basic_string<%s, %s<%2$s>, %s<%2$s>>"
                   "(%{#*s}, %zu).resize(%zu): size() == %5$zu, got %zu",
                   __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,
                   int (str_len), str, str_len, nparam, sz_tmp);
    }
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
    static charT wstr [long_string_len];
    rw_widen (wstr, str, str_len);

    TestString str_ob (wstr, str_len);
    TestString str_def;

    TestString* const pstr = 0 != str ? &str_ob : &str_def;

    if (MemFun::resize == pfid->mfun_)
        return test_resize (charT (), pfid, line, pstr, str, str_len,
                            nparam, cparam, should_throw);

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
        ret = pstr->size ();
        break;

    case MemFun::length:
        ret     = pstr->length ();
        exp_ret = pstr->size ();
        break;

    case MemFun::resize:
        // do nothing, handled above
        break;

    case MemFun::reserve:
        0 == nparam ? pstr->reserve () : pstr->reserve (nparam);
        break;

    case MemFun::capacity:
        ret = pstr->capacity ();
        break;

    case MemFun::max_size:
        ret = pstr->max_size ();
        break;

    case MemFun::empty:
        ret     = pstr->empty () ? 1 : 0;
        exp_ret = 0 == pstr->size() ? 1 : 0;
        break;

    case MemFun::clear:
        pstr->clear ();
        break;
    }

#define CALLFMAT                                                        \
    "line %d. basic_string<%s, %s<%2$s>, %s<%2$s>>(%{?}%{#*S}%{;})"     \
    ".%s(%{?}%zu%{;})"

#define CALLARGS                                        \
    __LINE__, pfid->cname_, pfid->tname_, pfid->aname_, \
    0 != str, int (sizeof (charT)), pstr, pfid->fname_, \
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
        std::string::size_type cur_sz = pstr->size();
        std::string::size_type max_sz = pstr->max_size();

        rw_assert (cur_sz <= ret && ret <= max_sz, 0, line,
                   CALLFMAT " == %zu, expected %zu < res < %zu",
                   CALLARGS, ret, cur_sz, max_sz);
    }

    if (MemFun::max_size == pfid->mfun_) {
        std::string::size_type cur_sz = pstr->size();

        rw_assert (cur_sz <= ret, 0, line,
                   CALLFMAT " == %zu, expected res > %zu",
                   CALLARGS, ret, cur_sz);
    }

    if (MemFun::reserve == pfid->mfun_ ) {
        ret = pstr->capacity ();
        rw_assert (nparam <= ret, 0, line,
                   CALLFMAT ": capacity() >= %zu, got %zu",
                   CALLARGS, nparam, ret);
        return;
    }

    if (MemFun::clear == pfid->mfun_ ) {
        rw_assert (pstr->empty (), 0, line,
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
                    char         cparam,
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

    TEST (long_string,     long_string_len - 1);
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
#define TEST(str, len, nparam, cparam, ex_throw)                \
    test_capacity (pfid, __LINE__, str, len, nparam,     \
                          cparam, 0, ex_throw)

    //    +--------------------------------------- controlled sequence
    //    |               +----------------------- controlled sequence length
    //    |               |                    +-- resize() integer argument
    //    |               |                    |   +-- resize() char argument
    //    |               |                    |   |    +--exception expected?
    //    |               |                    |   |    |
    //    V               V                    V   V    V
    TEST ("\0",           0,                   0, 'a',  false);
    TEST ("\0",           0,                  10, 'a',  false);

    TEST ("a",            1,                   1, 'a',  false);
    TEST ("a",            1,                   0, 'a',  false);
    TEST ("a",            1,                  10, 'a',  false);

    TEST ("ab",           2,                   2, 'a',  false);
    TEST ("ab",           2,                   1, 'a',  false);
    TEST ("ab",           2,                  10, 'a',  false);

    TEST ("t\0 s",        4,                   6, 'a',  false);
    TEST ("Test\0string", 11,                100, 'a',  false);

    TEST ("a\0\0\0b",     5,                  10, 'a',  false);
    TEST ("a\0\0\0b",     5,                  10, '\0', false);

    TEST ("a\0b\0c\0\0",  7,                  10, 'a',  false);
    TEST ("a\0b\0c\0\0",  7,                  10, '\0', false);

    TEST ("bc",           2, long_string_len - 1, 'a',  false);

    TEST (long_string, long_string_len - 1,                  10, 'a', false);
    TEST (long_string, long_string_len - 1, long_string_len - 1, 'a', false);

#ifndef _RWSTD_NO_EXCEPTIONS

    if (rw_opt_no_exceptions)
        rw_note (0, 0, __LINE__, "exceptions tests disabled");
    else {
        if (_RWSTD_SIZE_MAX > pfid->max_size_) {

            TEST ("\0", 1, pfid->max_size_ + 1, 'a', true);
            TEST ("a" , 1, pfid->max_size_ + 1, 'a', true);
            TEST (long_string, long_string_len - 1,
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

    TEST ("bc",        long_string_len - 1,     false);

    TEST (long_string, 10,                      false);
    TEST (long_string, long_string_len - 1,     false);

#ifndef _RWSTD_NO_EXCEPTIONS

    if (!rw_opt_no_exceptions) {
        if (_RWSTD_SIZE_MAX > pfid->max_size_) {

            TEST ("\0",        pfid->max_size_ + 1,     true);
            TEST ("a",         pfid->max_size_ + 1,     true);
            TEST (long_string, pfid->max_size_ + 1,     true);
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

    TEST (long_string,     long_string_len - 1);
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
    TEST (0,              long_string_len - 1, false);

    TEST ("\0",            0,                  false);
    TEST ("\0",           10,                  false);
    TEST ("\0",           long_string_len - 1, false);

    TEST ("abcd",          0,                  false);
    TEST ("abcd",          2,                  false);
    TEST ("abcd",          4,                  false);
    TEST ("abcd",         10,                  false);
    TEST ("abcd",         long_string_len - 1, false);

    TEST ("t\0 s",         1,                  false);
    TEST ("Test\0string",  4,                  false);
    TEST ("a\0\0b",        2,                  false);
    TEST ("a\0\0b",       10,                  false);
    TEST ("a\0b\0c\0\0",   4,                  false);
    TEST ("a\0b\0c\0\0",  10,                  false);

    TEST (long_string,    10,                  false);
    TEST (long_string,    long_string_len - 1, false);

#ifndef _RWSTD_NO_EXCEPTIONS

    if (_RWSTD_SIZE_MAX > pfid->max_size_) {
        TEST ("\0", pfid->max_size_ + 1, true);
        TEST ("a",  pfid->max_size_ + 1, true);
        TEST (long_string, pfid->max_size_ + 1, true);
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

    TEST (long_string,    long_string_len - 1);
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
    TEST (long_string);
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

    TEST (long_string);
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

    TEST (long_string);
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

    if (pfid->tname_ && rw_opt_no_user_traits) {
        rw_note (1 < rw_opt_no_user_traits++, 0, 0,
                 "user defined traits test disabled");
    }
    else if (!pfid->tname_ && rw_opt_no_char_traits) {
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
    if ('\0' == long_string [0]) {
        // initialize long_string
        for (std::size_t i = 0; i != sizeof long_string - 1; ++i)
            long_string [i] = 'x';
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
                    "|-no-user_traits",
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
