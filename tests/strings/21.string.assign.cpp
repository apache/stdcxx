/***************************************************************************
 *
 * 21.string.assign.cpp - test exercising [lib.string.assign]
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

#include <memory>       // for placement operator new()
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
   // disabled for compilers such as IBM VAC++ or MSVC
   // that can't reliably replace the operators
#  include <rw_new.h>
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE


#define AssignOverload   StringMembers::OverloadId
#define Assign(which)    StringMembers::assign_ ## which

typedef StringMembers::TestCase TestCase;
typedef StringMembers::Test     Test;
typedef StringMembers::Function MemFun;

/**************************************************************************/

// for convenience and brevity
#define LSTR      long_string
#define LLEN      long_string_len

static const std::size_t long_string_len = 4096;
static char long_string [long_string_len];

static const char* const exp_exceptions [] =
    { "unknown exception", "out_of_range", "length_error" };

/**************************************************************************/

// used to exercise:
// assign (const value_type*)
static const TestCase
ptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {                           \
        __LINE__, -1, -1, -1, -1, -1,                           \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |             +--------------------------- sequence to be assigned
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info
    //    |             |             |        |      0 - no exception
    //    |             |             |        |      1 - out_of_range
    //    |             |             |        |      2 - length_error
    //    |             |             |        |     -1 - exc. safety
    //    |             |             |        |
    //    |             |             |        +-------+
    //    V             V             V                V
    TEST ("ab",         "c",          "c",             0),

    TEST ("",           "",           "",              0),
    TEST ("",           "\0",         "",              0),
    TEST ("",           "abc",        "abc",           0),
    TEST ("abc",        "",           "",              0),

    TEST ("\0",         "",           "",              0),
    TEST ("\0",         "a",          "a",             0),
    TEST ("\0",         "\0\0",       "",              0),

    TEST ("\0\0ab",     "cdefghij",   "cdefghij",      0),
    TEST ("a\0\0b",     "cdefghij",   "cdefghij",      0),
    TEST ("ab\0\0",     "cdefghij",   "cdefghij",      0),
    TEST ("a\0b\0\0c",  "e\0",        "e",             0),
    TEST ("\0ab\0\0c",  "e\0",        "e",             0),

    TEST ("",           LSTR,         LSTR,            0),
    TEST (LSTR,         "",           "",              0),

    TEST ("",           0,            "",              0),
    TEST ("abc",        0,            "abc",           0),
    TEST ("a\0\0bc",    0,            "a",             0),
    TEST ("\0\0abc",    0,            "",              0),
    TEST ("abc\0\0",    0,            "abc",           0),

    TEST ("",           LSTR,         LSTR,           -1),

    TEST ("last",       "test",       "test",          0)
};

/**************************************************************************/

// used to exercise:
// assign (const basic_string&)
static const TestCase
str_test_cases [] = {

#undef TEST
#define TEST(s, arg, res, bthrow) {                             \
        __LINE__, -1, -1, -1, -1, -1,                           \
        s, sizeof s - 1,                                        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |             +--------------------------- sequence to be assigned
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info:
    //    |             |             |        |      0 - no exception
    //    |             |             |        |      1 - out_of_range
    //    |             |             |        |      2 - length_error
    //    |             |             |        |     -1 - exc. safety
    //    |             |             |        |
    //    |             |             |        +-----+
    //    |             |             |              |
    //    V             V             V              V
    TEST ("ab",         "c",          "c",           0),

    TEST ("",           "",           "",            0),
    TEST ("",           "\0",         "\0",          0),
    TEST ("",           "abc",        "abc",         0),
    TEST ("abc",        "",           "",            0),

    TEST ("\0",         "",           "",            0),
    TEST ("\0",         "a",          "a",           0),
    TEST ("\0",         "\0\0",       "\0\0",        0),

    TEST ("ab",         "c\0e",       "c\0e",        0),

    TEST ("\0\0ab",     "cdefghij",   "cdefghij",    0),
    TEST ("a\0\0b",     "cdefghij",   "cdefghij",    0),
    TEST ("ab\0\0",     "cdefghij",   "cdefghij",    0),
    TEST ("a\0b\0\0c",  "e\0",        "e\0",         0),
    TEST ("\0ab\0\0c",  "e\0",        "e\0",         0),
    TEST ("ab\0\0c\0",  "\0e",        "\0e",         0),

    TEST ("e\0",        "a\0b\0\0c",  "a\0b\0\0c",   0),
    TEST ("e\0\0",      "\0ab\0\0c",  "\0ab\0\0c",   0),
    TEST ("\0e",        "ab\0\0c\0",  "ab\0\0c\0",   0),

    TEST ("",           LSTR,         LSTR,          0),
    TEST (LSTR,         "",           "",            0),

    TEST ("",           0,            "",            0),
    TEST ("abc",        0,            "abc",         0),
    TEST ("a\0\0bc",    0,            "a\0\0bc",     0),
    TEST ("\0\0abc",    0,            "\0\0abc",     0),
    TEST ("abc\0\0",    0,            "abc\0\0",     0),

    TEST ("last",       "test",       "test",        0)
};

/**************************************************************************/

// used to exercise:
// assign (const value_type*, size_type)
static const TestCase
ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, size, res, bthrow) {                     \
        __LINE__, -1, size, -1, -1, -1,                         \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- sequence to be assigned
    //    |            |            +--------------- assign() n argument
    //    |            |            |  +------------ expected result sequence
    //    |            |            |  |     +------ exception info
    //    |            |            |  |     |        0 - no exception
    //    |            |            |  |     |        1 - out_of_range
    //    |            |            |  |     |        2 - length_error
    //    |            |            |  |     |       -1 - exc. safety
    //    |            |            |  |     |
    //    |            |            |  |     +--------+
    //    V            V            V  V              V
    TEST ("ab",        "c",         1, "c",           0),

    TEST ("",          "",          0,  "",           0),
    TEST ("",          "abc",       1,  "a",          0),
    TEST ("",          "\0",        1,  "\0",         0),

    TEST ("\0",        "",          0,  "",           0),
    TEST ("\0",        "a",         0,  "",           0),
    TEST ("\0",        "a",         1,  "a",          0),
    TEST ("\0",        "\0\0",      1,  "\0",         0),
    TEST ("\0",        "\0\0",      2,  "\0\0",       0),

    TEST ("cde",       "ab",        2,  "ab",         0),
    TEST ("cde",       "ab",        1,  "a",          0),

    TEST ("\0e\0",     "a\0b\0\0c", 0,  "",           0),
    TEST ("\0e\0",     "\0ab\0\0c", 3,  "\0ab",       0),
    TEST ("\0e\0",     "a\0b\0\0c", 6,  "a\0b\0\0c",  0),

    TEST ("a\0b\0\0c", "\0e\0",     3,  "\0e\0",      0),
    TEST ("a\0b\0\0c", "\0\0e\0",   2,  "\0\0",       0),
    TEST ("\0ab\0\0c", "\0e\0",     1,  "\0",         0),
    TEST ("a\0bc\0\0", "\0e",       2,  "\0e",        0),

    TEST ("",          0,           0,  "",           0),
    TEST ("abc",       0,           0,  "",           0),
    TEST ("abc",       0,           1,  "a",          0),
    TEST ("abc",       0,           2,  "ab",         0),
    TEST ("a\0bc",     0,           2,  "a\0",        0),
    TEST ("\0abc\0\0", 0,           1,  "\0",         0),
    TEST ("a\0bc\0\0", 0,           3,  "a\0b",       0),
    TEST ("a\0bc\0\0", 0,           6,  "a\0bc\0\0",  0),

    TEST ("",          LSTR, LLEN - 1,  LSTR,         0),
    TEST ("abcd",      LSTR,        0,  "",           0),
    TEST (LSTR,        LSTR,        0,  "",           0),
    TEST (LSTR,        LSTR,        1,  "x",          0),
    TEST (LSTR,        "",          0,  "",           0),

    TEST ("",          "",         -1,  "",           2),
    TEST ("",          LSTR, LLEN - 1,  LSTR,        -1),

    TEST ("last",      "test",      4, "test",        0)
};

/**************************************************************************/

// used to exercise:
// assign (const basic_string&, size_type, size_type)
// assign (InputIterator, InputIterator)
static const TestCase
range_test_cases [] = {

// range_test_cases serves a double duty
#define str_size_size_test_cases range_test_cases

#undef TEST
#define TEST(str, arg, off, size, res, bthrow) {                \
        __LINE__, off, size, -1, -1, -1,                        \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- sequence to be inserted
    //    |            |            +--------------- assign() pos argument
    //    |            |            |  +------------ assign() n argument
    //    |            |            |  |  +--------- expected result sequence
    //    |            |            |  |  |  +------ exception info
    //    |            |            |  |  |  |        0 - no exception
    //    |            |            |  |  |  |        1 - out_of_range
    //    |            |            |  |  |  |        2 - length_error
    //    |            |            |  |  |  |       -1 - exc. safety
    //    |            |            |  |  |  |
    //    |            |            |  |  |  +-----------+
    //    V            V            V  V  V              V
    TEST ("ab",        "c",         0, 1, "c",           0),

    TEST ("",          "",          0, 0,  "",           0),
    TEST ("",          "abc",       1, 1,  "b",          0),
    TEST ("",          "\0",        0, 1,  "\0",         0),

    TEST ("\0",        "",          0, 0,  "",           0),

    TEST ("abc",       "",          0, 0,  "",           0),

    TEST ("\0",        "a",         0, 1,  "a",          0),
    TEST ("\0",        "\0\0",      1, 1,  "\0",         0),
    TEST ("\0",        "\0\0",      0, 2,  "\0\0",       0),
    TEST ("\0",        "\0\0",      1, 5,  "\0",         0),

    TEST ("cde",       "ab",        0, 2,  "ab",         0),
    TEST ("cde",       "ab",        0, 1,  "a",          0),
    TEST ("cde",       "ab",        1, 5,  "b",          0),

    TEST ("ab",        "c\0e",      0, 3,  "c\0e",       0),
    TEST ("ab",        "c\0e",      1, 2,  "\0e",        0),
    TEST ("ab",        "c\0e",      0, 2,  "c\0",        0),

    TEST ("\0e\0",     "\0ab\0\0c", 0, 9,  "\0ab\0\0c",  0),
    TEST ("\0e\0",     "\0ab\0\0c", 0, 3,  "\0ab",       0),
    TEST ("a\0b\0\0c", "\0e\0",     0, 3,  "\0e\0",      0),
    TEST ("a\0b\0\0c", "\0\0e\0",   0, 2,  "\0\0",       0),
    TEST ("\0ab\0\0c", "\0e\0",     2, 1,  "\0",         0),
    TEST ("\0ab\0\0c", "\0e\0",     2, 9,  "\0",         0),
    TEST ("a\0bc\0\0", "\0e",       0, 2,  "\0e",        0),

    TEST ("",          0,           0, 0,  "",           0),
    TEST ("abc",       0,           1, 0,  "",           0),
    TEST ("abc",       0,           1, 1,  "b",          0),
    TEST ("abc",       0,           0, 2,  "ab",         0),
    TEST ("a\0bc\0\0", 0,           4, 2,  "\0\0",       0),
    TEST ("a\0bc\0\0", 0,           1, 3,  "\0bc",       0),
    TEST ("a\0bc\0\0", 0,           0, 9,  "a\0bc\0\0",  0),
    TEST ("abcdef",    0,           1, 2,  "bc",         0),

    TEST (LSTR,        "",          0, 0,  "",           0),
    TEST ("",          LSTR,        9, 2,  "xx",         0),
    TEST ("",          LSTR,        9, 0,  "",           0),
    TEST ("abc",       LSTR,        2, 1,  "x",          0),
    TEST (LSTR,        LSTR,        2, 3,  "xxx",        0),
    TEST ("",          LSTR,        0, LLEN, LSTR,       0),

    TEST ("",          "\0",        2, 0,  "",           1),
    TEST ("",          "a",         2, 0,  "",           1),
    TEST ("",          LSTR,LLEN + 10, 0,  "",           1),
    TEST ("",          LSTR,        0, LLEN - 1, LSTR,  -1),

    TEST ("last",      "test",      0, 4, "test",        0)
};

/**************************************************************************/

// used to exercise:
// assign (size_type, value_type)
static const TestCase
size_val_test_cases [] = {

#undef TEST
#define TEST(str, size, val, res, bthrow) {     \
        __LINE__, -1, size, -1, -1, val,        \
        str, sizeof str - 1,                    \
        0, 0, res, sizeof res - 1, bthrow       \
    }

    //    +----------------------------------------- controlled sequence
    //    |            +---------------------------- assign() count argument
    //    |            |   +------------------------ character to be assigned
    //    |            |   |   +-------------------- expected result sequence
    //    |            |   |   |       +------------ exception info
    //    |            |   |   |       |              0 - no exception
    //    |            |   |   |       |              1 - out_of_range
    //    |            |   |   |       |              2 - length_error
    //    |            |   |   |       |             -1 - exc. safety
    //    |            |   |   |       |
    //    |            |   |   |       +--------+
    //    V            V   V   V                V
    TEST ("ab",        1, 'c', "c",             0),

    TEST ("",          0, ' ',  "",             0),
    TEST ("",          1, 'b',  "b",            0),
    TEST ("",          3, 'b',  "bbb",          0),

    TEST ("\0",        0, ' ',  "",             0),
    TEST ("",          2, '\0', "\0\0",         0),

    TEST ("\0",        1, 'a',  "a",            0),
    TEST ("\0",        1, '\0', "\0",           0),
    TEST ("\0",        2, '\0', "\0\0",         0),
    TEST ("\0",        0, '\0', "",             0),

    TEST ("cde",       3, 'a',  "aaa",          0),
    TEST ("ab",        2, '\0', "\0\0",         0),
    TEST ("ab",        1, '\0', "\0",           0),

    TEST ("a\0b\0\0c", 2, '\0', "\0\0",         0),
    TEST ("a\0b\0\0c", 1, '\0', "\0",           0),
    TEST ("\0ab\0\0c", 3, '\0', "\0\0\0",       0),
    TEST ("a\0bc\0\0", 2, 'a',  "aa",           0),

    TEST ("",   LLEN - 1, 'x',  LSTR,           0),
    TEST (LSTR,        0, 'x',  "",             0),

    TEST ("",         -1, 'x',  "",             2),
    TEST ("",   LLEN - 1, 'x',  LSTR,          -1),

    TEST ("last",      4, 't',  "tttt",         0)
};

/**************************************************************************/

template <class charT, class Traits>
void test_exceptions (charT, Traits*,
                      AssignOverload  which,
                      const TestCase &cs)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::iterator                StringIter;
    typedef typename TestString::const_iterator          ConstStringIter;

    static charT wstr [LLEN];
    static charT wsrc [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (wsrc, cs.arg, cs.arg_len);

    /* const */ TestString s_str (wstr, cs.str_len);
    const       TestString s_arg (wsrc, cs.arg_len);

    std::size_t throw_after = 0;

    const std::size_t     size     = s_str.size ();
    const std::size_t     capacity = s_str.capacity ();
    const ConstStringIter begin    = s_str.begin ();

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rwt_free_store* const pst = rwt_get_free_store (0);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    const charT* const arg_ptr = cs.arg ? wsrc : s_str.c_str ();
    const TestString&  arg_str = cs.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (cs.val), (charT*)0);

    // iterate for`n=throw_after' starting at the next call to operator
    // new, forcing each call to throw an exception, until the assignion
    // finally succeeds (i.e, no exception is thrown)
    for ( ; ; ) {

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#endif   // _RWSTD_NO_EXCEPTIONS

        _TRY {
            switch (which) {
            case Assign (ptr):
                s_str.assign (arg_ptr);
                break;

            case Assign (str):
                s_str.assign (arg_str);
                break;

            case Assign (ptr_size):
                s_str.assign (arg_ptr, cs.size);
                break;

            case Assign (str_size_size):
                s_str.assign (arg_str, cs.off, cs.size);
                break;

            case Assign (size_val):
                s_str.assign (cs.size, arg_val);
                break;

            case Assign (range): {
                const ConstStringIter first = s_arg.begin ();
                const ConstStringIter last  = s_arg.end ();
                s_str.assign (first, last);
                break;
            }

            default:
                RW_ASSERT (!"test logic error: unknown assign overload");
            }

            break;   // out of for loop
        }
        _CATCH (...) {

#ifndef _RWSTD_NO_EXCEPTIONS

            // verify that an exception thrown during allocation
            // doesn't cause a change in the state of the vector

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


            // increment to allow this call to operator new to succeed
            // and force the next one to fail, and try to assign again
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
               "line %d: %{$FUNCALL}: failed to throw an expected exception",
               __LINE__);

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
void test_assign_range (charT          *wstr,
                        charT          *wsrc,
                        Traits*,
                        const Iterator &it,
                        const TestCase &cs)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> String;

    const char* const itname =
        cs.arg ? type_name (it, (charT*)0) : "basic_string::iterator";

    /* const */ String s_str (wstr, cs.str_len);
    const       String s_arg (wsrc, cs.arg_len);

    std::size_t off_last = cs.off + cs.size;

    if (cs.arg) {
        off_last = off_last > s_arg.size () ? s_arg.size () : off_last;

        const Iterator first (wsrc + cs.off,   wsrc + cs.off, wsrc + off_last);
        const Iterator last  (wsrc + off_last, wsrc + cs.off, wsrc + off_last);

        s_str.assign (first, last);
    }
    else {
        typedef typename String::iterator StringIter;

        const StringIter first (s_str.begin () + cs.off);
        const StringIter last  (off_last > s_str.size () ?
            s_str.end ()
          : s_str.begin () + off_last);

        s_str.assign (first, last);
    }

    const std::size_t match = rw_match (cs.res, s_str.c_str(), cs.res_len);

    rw_assert (match == cs.res_len, 0, cs.line,
               "line %d. %{$FUNCALL} expected %{#*s}, got %{/*.*Gs}, "
               "difference at off %zu for %s",
               __LINE__, int (cs.res_len), cs.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits>
void test_assign_range (charT          *wstr,
                        charT          *wsrc,
                        Traits*,
                        const TestCase &cs)
{
    if (cs.bthrow)  // this method doesn't throw
        return;

    test_assign_range (wstr, wsrc, (Traits*)0,
                       InputIter<charT>(0, 0, 0), cs);

    // there is no need to call test_assign_range
    // for other iterators in this case
    if (0 == cs.arg)
        return;

    test_assign_range (wstr, wsrc, (Traits*)0,
                       ConstFwdIter<charT>(0, 0, 0), cs);

    test_assign_range (wstr, wsrc, (Traits*)0,
                       ConstBidirIter<charT>(0, 0, 0), cs);

    test_assign_range (wstr, wsrc, (Traits*)0,
                       ConstRandomAccessIter<charT>(0, 0, 0), cs);
}

/**************************************************************************/

template <class charT, class Traits>
void test_assign (charT, Traits*,
                  AssignOverload  which,
                  const TestCase &cs)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::iterator                StringIter;

    static charT wstr [LLEN];
    static charT wsrc [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (wsrc, cs.arg, cs.arg_len);

    // special processing for assign_range to exercise all iterators
    if (Assign (range) == which) {
        test_assign_range (wstr, wsrc, (Traits*)0, cs);
        return;
    }

    /* const */ TestString s_str (wstr, cs.str_len);
    const       TestString s_arg (wsrc, cs.arg_len);

    std::size_t res_off = 0;
    std::size_t size = cs.size >= 0 ? cs.size : s_str.max_size () + 1;

    // first function argument
    const charT* const arg_ptr = cs.arg ? wsrc : s_str.c_str ();
    const TestString&  arg_str = cs.arg ? s_arg : s_str;

    // address of returned reference
    const TestString* res_ptr = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    if (1 == cs.bthrow && Assign (str_size_size) == which)
        expected = exp_exceptions [1];
    if (2 == cs.bthrow)
        expected = exp_exceptions [2];

    const char* caught = 0;

    try {

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (cs.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    switch (which) {
    case Assign (ptr):
        res_ptr = &s_str.assign (arg_ptr);
        break;

    case Assign (str):
        res_ptr = &s_str.assign (arg_str);
        break;

    case Assign (ptr_size):
        res_ptr = &s_str.assign (arg_ptr, size);
        break;

    case Assign (str_size_size):
        res_ptr = &s_str.assign (arg_str, cs.off, size);
        break;

    case Assign (size_val): {
        const charT val = make_char (char (cs.val), (charT*)0);
        res_ptr = &s_str.assign (size, val);
        break;
    }

    default:
        RW_ASSERT (!"test logic error: unknown assign overload");
    }

    res_off = res_ptr - &s_str;

    // verify the returned value
    rw_assert (0 == res_off, 0, cs.line,
               "line %d. %{$FUNCALL} returned invalid reference, offset is %zu",
               __LINE__, res_off);

    // verfiy that strings length are equal
    rw_assert (cs.res_len == s_str.size (), 0, cs.line,
               "line %d. %{$FUNCALL}: expected %{#*s} with length %zu, "
               "got %{/*.*Gs} with length %zu", __LINE__, int (cs.res_len),
               cs.res, cs.res_len, int (sizeof (charT)), int (s_str.size ()),
               s_str.c_str (), s_str.size ());

    // verfiy that assign results match expected result
    const std::size_t match = rw_match (cs.res, s_str.c_str(), cs.res_len);

    rw_assert (match == cs.res_len, 0, cs.line,
               "line %d. %{$FUNCALL}: expected %{#*s}, got %{/*.*Gs}, "
               "difference at off %zu",
               __LINE__, int (cs.res_len), cs.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match);

#ifndef _RWSTD_NO_EXCEPTIONS

    }
    catch (std::out_of_range) {
        caught = exp_exceptions [1];
    }
    catch (std::length_error) {
        caught = exp_exceptions [2];
    }
    catch (...) {
        caught = exp_exceptions [0];
    }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)
    _RWSTD_UNUSED (should_throw);
#endif   // _RWSTD_NO_EXCEPTIONS

    rw_assert (caught == expected, 0, cs.line,
               "line %d. %{$FUNCALL}: %{?}expected %s, caught %s"
               "%{:}unexpectedly caught %s%{;}",
               __LINE__, 0 != expected, expected, caught, caught);
}

/**************************************************************************/

static void
test_assign (const MemFun &memfun, const TestCase& tcase)
{
    // exercise exception safety?
    const bool exception_safety = -1 == tcase.bthrow;

#undef TEST
#define TEST(charT, Traits)                                             \
    exception_safety ?                                                  \
        test_exceptions (charT (), (Traits*)0, memfun.which_, tcase)    \
      : test_assign (charT (), (Traits*)0, memfun.which_, tcase)

    if (StringMembers::DefaultTraits == memfun.traits_id_) {
        if (StringMembers::Char == memfun.char_id_)
            TEST (char, std::char_traits<char>);

#ifndef _RWSTD_NO_WCHAR_T
    else
        TEST (wchar_t, std::char_traits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T

    }
    else {
       if (StringMembers::Char == memfun.char_id_)
           TEST (char, UserTraits<char>);

#ifndef _RWSTD_NO_WCHAR_T
       else if (StringMembers::WChar == memfun.char_id_)
           TEST (wchar_t, UserTraits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T

       else
           TEST (UserChar, UserTraits<UserChar>);
    }
}

/**************************************************************************/

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
        StringMembers::assign_ ## tag,                          \
        tag ## _test_cases,                                     \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (ptr),
        TEST (str),
        TEST (ptr_size),
        TEST (str_size_size),
        TEST (size_val),
        TEST (range)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    StringMembers::run_test (test_assign, tests, test_count);

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.assign",
                    0 /* no comment */,
                    run_test,
                    "|-no-char_traits# "
                    "|-no-user_traits# "
                    "|-no-user_char# "
                    "|-no-exceptions# "
                    "|-no-exception-safety# "

                    "|-no-assign-ptr# "
                    "|-no-assign-str# "
                    "|-no-assign-ptr-size# "
                    "|-no-assign-str-size-size# "
                    "|-no-assign-size-val# "
                    "|-no-assign-range#",

                    &StringMembers::opt_no_char_traits,
                    &StringMembers::opt_no_user_traits,
                    &StringMembers::opt_no_user_char,
                    &StringMembers::opt_no_exceptions,
                    &StringMembers::opt_no_exception_safety,

                    &Disabled (Assign (ptr)),
                    &Disabled (Assign (str)),
                    &Disabled (Assign (ptr_size)),
                    &Disabled (Assign (str_size_size)),
                    &Disabled (Assign (size_val)),
                    &Disabled (Assign (range)),
                    // sentinel
                    (void*)0);
}
