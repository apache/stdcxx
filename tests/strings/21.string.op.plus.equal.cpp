/***************************************************************************
 *
 * 21.string.plus.equal.cpp - test exercising [lib.string.op+=]
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

#include <21.strings.h>

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
#  include <rw_new.h>   // for bad_alloc, replacement operator new
#else
#  include <new>        // for bad_alloc
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

#define OpPlusEqOverload   StringMembers::OverloadId
#define OpPlusEq(which)    StringMembers::op_plus_eq_ ## which

typedef StringMembers::TestCase TestCase;
typedef StringMembers::Test     Test;
typedef StringMembers::Function MemFun;

/**************************************************************************/

// for convenience and brevity
#define LSTR      long_string
#define LLEN      long_string_len

static const std::size_t long_string_len = 4096;
static char long_string [long_string_len];

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

// used to exercise
// operator += (const charT* s)
static const TestCase
ptr_test_cases [] = {

#undef TEST
#define TEST(str, src, res, bthrow)                            \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, src,  \
      sizeof src - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |             +--------------------------- sequence to be appended
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info
    //    |             |             |        |         0 - no exception
    //    |             |             |        |         1 - length_error
    //    |             |             |        |        -1 - exc. safety
    //    |             |             |        |
    //    |             |             |        +--------------+
    //    V             V             V                       V
    TEST ("ab",         "c",          "abc",                  0),

    TEST ("",           "",           "",                     0),
    TEST ("",           "\0",         "",                     0),
    TEST ("",           "abc",        "abc",                  0),

    TEST ("\0",         "",           "\0",                   0),
    TEST ("\0",         "a",          "\0a",                  0),
    TEST ("\0",         "\0\0",       "\0",                   0),

    TEST ("ab",         "cd",         "abcd",                 0),
    TEST ("bcd",        "a",          "bcda",                 0),
    TEST ("cde",        "ab",         "cdeab",                0),
    TEST ("abc",        "",           "abc",                  0),
    TEST ("ab",         "c\0e",       "abc",                  0),

    TEST ("\0\0ab",     "cdefghij",   "\0\0abcdefghij",       0),
    TEST ("a\0\0b",     "cdefghij",   "a\0\0bcdefghij",       0),
    TEST ("ab\0\0",     "cdefghij",   "ab\0\0cdefghij",       0),
    TEST ("a\0b\0\0c",  "e\0",        "a\0b\0\0ce",           0),
    TEST ("\0ab\0\0c",  "e\0",        "\0ab\0\0ce",           0),
    TEST ("abcdefghij", "abcdefghij", "abcdefghijabcdefghij", 0),

    TEST ("",           LSTR,         LSTR,                   0),
    TEST (LSTR,         "",           LSTR,                   0),

    TEST ("",           0,            "",                     0),
    TEST ("abc",        0,            "abcabc",               0),
    TEST ("a\0\0bc",    0,            "a\0\0bca",             0),
    TEST ("\0\0abc",    0,            "\0\0abc",              0),
    TEST ("abc\0\0",    0,            "abc\0\0abc",           0),

    TEST ("",           LSTR,         LSTR,                  -1),

    TEST ("last",       "test",       "lasttest",             0)
};

/**************************************************************************/

// used to exercise
// operator += (const basic_string& str)
static const TestCase
str_test_cases [] = {

#undef TEST
#define TEST(str, src, res, bthrow)                            \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, src,  \
      sizeof src - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |             +------------------------- sequence to be appended
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info
    //    |             |             |        |         0 - no exception
    //    |             |             |        |         1 - length_error
    //    |             |             |        |        -1 - exc. safety
    //    |             |             |        |
    //    |             |             |        +--------------+
    //    V             V             V                       V
    TEST ("ab",         "c",          "abc",                  0),

    TEST ("",           "",           "",                     0),
    TEST ("",           "\0",         "\0",                   0),
    TEST ("",           "abc",        "abc",                  0),

    TEST ("\0",         "",           "\0",                   0),
    TEST ("\0",         "a",          "\0a",                  0),
    TEST ("\0",         "\0\0",       "\0\0\0",               0),

    TEST ("ab",         "cd",         "abcd",                 0),
    TEST ("bcd",        "a",          "bcda",                 0),
    TEST ("cde",        "ab",         "cdeab",                0),
    TEST ("abc",        "",           "abc",                  0),
    TEST ("ab",         "c\0e",       "abc\0e",               0),

    TEST ("\0\0ab",     "cdefghij",   "\0\0abcdefghij",       0),
    TEST ("a\0\0b",     "cdefghij",   "a\0\0bcdefghij",       0),
    TEST ("ab\0\0",     "cdefghij",   "ab\0\0cdefghij",       0),
    TEST ("a\0b\0\0c",  "e\0",        "a\0b\0\0ce\0",         0),
    TEST ("\0ab\0\0c",  "e\0",        "\0ab\0\0ce\0",         0),
    TEST ("ab\0\0c\0",  "\0e",        "ab\0\0c\0\0e",         0),
    TEST ("abcdefghij", "abcdefghij", "abcdefghijabcdefghij", 0),

    TEST ("",           LSTR,         LSTR,                   0),
    TEST (LSTR,         "",           LSTR,                   0),

    TEST ("",           0,            "",                     0),
    TEST ("abc",        0,            "abcabc",               0),
    TEST ("a\0\0bc",    0,            "a\0\0bca\0\0bc",       0),
    TEST ("\0\0abc",    0,            "\0\0abc\0\0abc",       0),
    TEST ("abc\0\0",    0,            "abc\0\0abc\0\0",       0),

    TEST ("",           LSTR,         LSTR,                  -1),

    TEST ("last",       "test",       "lasttest",             0)
};

/**************************************************************************/

// used to exercise
// operator+= (value_type c)
static const TestCase
val_test_cases [] = {

#undef TEST
#define TEST(str, val, res, bthrow)                              \
    { __LINE__, -1, -1, -1, -1, val, str, sizeof str - 1, 0, 0,  \
      res, sizeof res - 1, bthrow }

    //    +---------------------------------- controlled sequence
    //    |            +--------------------- character to be appended
    //    |            |   +----------------- expected result sequence
    //    |            |   |           +----- exception info
    //    |            |   |           |        0 - no exception
    //    |            |   |           |        1 - length_error
    //    |            |   |           |       -1 - exc. safety
    //    |            |   |           |
    //    |            |   |           +----+
    //    V            V   V                V
    TEST ("ab",        'c', "abc",          0),

    TEST ("",          'a',  "a",           0),
    TEST ("",          '\0', "\0",          0),

    TEST ("\0",        'a',  "\0a",         0),
    TEST ("\0",        '\0', "\0\0",        0),

    TEST ("cde",       'a',  "cdea",        0),
    TEST ("abc",       '\0', "abc\0",       0),

    TEST ("a\0b\0\0c", '\0', "a\0b\0\0c\0", 0),
    TEST ("\0ab\0\0c", '\0', "\0ab\0\0c\0", 0),
    TEST ("a\0bc\0\0", 'a',  "a\0bc\0\0a",  0),

    TEST ("last",      't', "lastt",        0)
};

/**************************************************************************/

template <class charT, class Traits>
void test_op_plus_eq (charT, Traits*,
                      OpPlusEqOverload which,
                      const TestCase  &cs)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::iterator                StringIter;
    typedef typename TestString::const_iterator          ConstStringIter;

    static charT wstr [LLEN];
    static charT warg [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (warg, cs.arg, cs.arg_len);

    TestString s_str (wstr, cs.str_len);
    TestString s_arg (warg, cs.arg_len);

    std::size_t res_off = 0;
    std::size_t throw_after = 0;

    const std::size_t     size     = s_str.size ();
    const std::size_t     capacity = s_str.capacity ();
    const ConstStringIter begin    = s_str.begin ();

    const charT* const arg_ptr = cs.arg ? warg : s_str.c_str ();
    const TestString&  arg_str = cs.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (cs.val), (charT*)0);

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rwt_free_store* const pst = rwt_get_free_store (0);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // iterate for`n=throw_after' starting at the next call to operator
    // new, forcing each call to throw an exception, until the appendion
    // finally succeeds (i.e, no exception is thrown)
    for ( ; ; ) {

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        if (-1 == cs.bthrow)
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#endif   // _RWSTD_NO_EXCEPTIONS

#ifndef _RWSTD_NO_EXCEPTIONS

        // is some exception expected ?
        const char* expected = 0;
        if (1 == cs.bthrow)
            expected = exceptions [2];      // length_error
        if (-1 == cs.bthrow)
            expected = exceptions [3];      // bad_alloc

        const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (cs.bthrow)
            return;

#endif   // _RWSTD_NO_EXCEPTIONS

        try {
            switch (which) {
            case OpPlusEq (ptr): {
                const TestString& s_res = s_str += arg_ptr;
                res_off = &s_res - &s_str;
                break;
            }

            case OpPlusEq (str): {
                const TestString& s_res = s_str += arg_str;
                res_off = &s_res - &s_str;
                break;
            }

            case OpPlusEq (val): {
                const TestString& s_res = s_str += arg_val;
                res_off = &s_res - &s_str;
                break;
            }

            default:
                RW_ASSERT ("test logic error: unknown operator += overload");
                return;
            }

            // verify the returned value
            rw_assert (0 == res_off, 0, cs.line,
                       "line %d. %{$FUNCALL} returned invalid reference, "
                       "offset is %zu", __LINE__, res_off);

            // verfiy that strings length are equal
            rw_assert (cs.res_len == s_str.size (), 0, cs.line,
                       "line %d. %{$FUNCALL} expected %{#*s} "
                       "with length %zu, got %{/*.*Gs} with length %zu",
                       __LINE__, int (cs.res_len), cs.res, cs.res_len,
                       int (sizeof (charT)), int (s_str.size ()),
                       s_str.c_str (), s_str.size ());

            if (cs.res_len == s_str.size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (cs.res, s_str.c_str(), cs.res_len);

                rw_assert (match == cs.res_len, 0, cs.line,
                           "line %d. %{$FUNCALL} expected %{#*s}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, int (cs.res_len), cs.res,
                           int (sizeof (charT)), int (s_str.size ()),
                           s_str.c_str (), match);
            }
        }

#ifndef _RWSTD_NO_EXCEPTIONS

        catch (const std::length_error &ex) {
            caught = exceptions [2];
            rw_assert (caught == expected, 0, cs.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught, ex.what ());
        }
        catch (const std::bad_alloc &ex) {
            caught = exceptions [3];
            rw_assert (-1 == cs.bthrow, 0, cs.line,
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

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        _RWSTD_UNUSED (should_throw);

#endif   // _RWSTD_NO_EXCEPTIONS

        if (caught) {
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object

            rw_assert (s_str.size () == size, 0, cs.line,
                       "line %d: %{$FUNCALL}: size unexpectedly changed "
                       "from %zu to %zu after an exception",
                       __LINE__, size, s_str.size ());

            rw_assert (s_str.capacity () == capacity, 0, cs.line,
                       "line %d: %{$FUNCALL}: capacity unexpectedly "
                       "changed from %zu to %zu after an exception",
                       __LINE__, capacity, s_str.capacity ());

            rw_assert (s_str.begin () == begin, 0, cs.line,
                       "line %d: %{$FUNCALL}: begin() unexpectedly "
                       "changed from after an exception by %d",
                       __LINE__, s_str.begin () - begin);

            if (-1 == cs.bthrow) {
                // increment to allow this call to operator new to succeed
                // and force the next one to fail, and try calling the same
                // function again
                ++throw_after;
                continue;
            }
        }
        else if (-1 != cs.bthrow) {
            rw_assert (caught == expected, 0, cs.line,
                       "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                       "%{:}unexpectedly caught %s%{;}",
                       __LINE__, 0 != expected, expected, caught, caught);
        }

        break;
    }

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify that if exceptions are enabled and when capacity changes
    // at least one exception is thrown
    rw_assert (   *pst->throw_at_calls_ [0] == std::size_t (-1)
               || throw_after,
               0, cs.line,
               "line %d: %{$FUNCALL}: failed to throw an expected exception",
               __LINE__);

    *pst->throw_at_calls_ [0] = std::size_t (-1);

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    _RWSTD_UNUSED (size);
    _RWSTD_UNUSED (capacity);
    _RWSTD_UNUSED (throw_after);

#endif   // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

DEFINE_TEST_DISPATCH (test_op_plus_eq);

static int
run_test (int, char*[])
{
    if ('\0' == LSTR [0]) {
        // initialize LSTR
        for (std::size_t i = 0; i != sizeof LSTR - 1; ++i)
            LSTR [i] = 'x';
    }

    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::op_plus_eq_ ## tag, tag ## _test_cases,  \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (ptr),
        TEST (str),
        TEST (val)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    StringMembers::run_test (test_op_plus_eq, tests, test_count);

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.op+=",
                    0 /* no comment */, run_test,
                    "|-no-char_traits# "
                    "|-no-user_traits# "
                    "|-no-user_char# "
                    "|-no-exceptions# "
                    "|-no-exception-safety# "

                    "|-no-op-append-ptr# "
                    "|-no-op-append-str# "
                    "|-no-op-append-val#",

                    &StringMembers::opt_no_char_traits,
                    &StringMembers::opt_no_user_traits,
                    &StringMembers::opt_no_user_char,
                    &StringMembers::opt_no_exceptions,
                    &StringMembers::opt_no_exception_safety,

                    &Disabled (OpPlusEq (ptr)),
                    &Disabled (OpPlusEq (str)),
                    &Disabled (OpPlusEq (val)),

                    // sentinel
                    (void*)0);
}
