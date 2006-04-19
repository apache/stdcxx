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
          fname_ ("assign") { /* empty */ }

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

static const char* const exp_exceptions [] =
    { "unknown exception", "out_of_range", "length_error" };

/**************************************************************************/

enum AssignOverload {
    // assign (const value_type*)
    assign_ptr,
    // assign (const basic_string&)
    assign_str,
    // assign (const value_type*, size_type)
    assign_ptr_size,
    // assign (const basic_string&, size_type, size_type)
    assign_str_off_size,
    // assign (size_type, value_type)
    assign_size_val,
    // assign (InputIterator, InputIterator)
    assign_range,

    // must be last: number of exercised overloads
    n_assign_overloads
};

/**************************************************************************/

struct TestCase
{
    int         line;      // test case line number

    int         off;       // offset (position argument)
    int         size;      // size (count argument)
    int         val;       // value (single character to insert)

    const char* str;       // controlled sequence
    std::size_t str_len;   // length of sequence

    const char* arg;       // sequence to insert
    std::size_t arg_len;   // length of sequence

    const char* res;       // resulting sequence
    std::size_t res_len;   // length of sequence

    int         bthrow;    // exception expected
};

/**************************************************************************/

static int rw_opt_no_char_traits;              // for --no-char_traits
static int rw_opt_no_user_traits;              // for --no-user_traits

static int rw_opt_no_user_chars;               // for --no-user_chars
static int rw_opt_no_exceptions;               // for --no-exceptions
static int rw_opt_no_exception_safety;         // for --no-exception-safety

// array of options to disable each overload of assign
static int rw_opt_no_assign [n_assign_overloads];

/**************************************************************************/

// used to exercise:
// assign (const value_type*)
static const TestCase
assign_ptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow)                            \
    { __LINE__, -1, -1, -1, str, sizeof str - 1, arg,          \
      sizeof arg - 1, res, sizeof res - 1, bthrow }

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
assign_str_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow)                            \
    { __LINE__, -1, -1, -1, str, sizeof str - 1, arg,          \
      sizeof arg - 1, res, sizeof res - 1, bthrow }

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
assign_ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, size, res, bthrow)                            \
    { __LINE__, -1, size, -1, str, sizeof str - 1, arg,              \
      sizeof arg - 1, res, sizeof res - 1, bthrow }

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
assign_range_test_cases [] = {

// assign_range_test_cases serve a double duty
#define assign_str_off_size_test_cases assign_range_test_cases

#undef TEST
#define TEST(str, arg, off, size, res, bthrow)                            \
    { __LINE__, off, size, -1, str, sizeof str - 1, arg,                  \
      sizeof arg - 1, res, sizeof res - 1, bthrow }

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
assign_size_val_test_cases [] = {

#undef TEST
#define TEST(str, size, val, res, bthrow)                            \
    { __LINE__, -1, size, val, str, sizeof str - 1, 0, 0,            \
      res, sizeof res - 1, bthrow }

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

static const struct FunctionTag
{
    AssignOverload  which;     // which overload of insert()
    const TestCase *t_cases;   // test cases to exercise
    std::size_t     n_cases;   // number of test cases
    const char     *funsig;    // function signature
} function_tags [] = {

#undef TEST
#define TEST(tag, sig) {                                        \
        tag, tag ## _test_cases,                                \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases, \
        "assign " sig                                           \
    }

    TEST (assign_ptr,          "(const value_type*)"),
    TEST (assign_str,          "(const basic_string&)"),
    TEST (assign_ptr_size,     "(const value_type*, size_type)"),
    TEST (assign_str_off_size, "(const basic_string&, size_type, size_type)"),
    TEST (assign_size_val,     "(size_type, value_type)"),
    TEST (assign_range,        "(InputIterator, InputIterator)")
};

/**************************************************************************/

template <class charT, class Traits>
void test_assign_exceptions (charT, Traits*,
                             const AssignOverload  which,
                             const TestCase       &cs,
                             const char           *funcall)
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
            case assign_ptr:
                s_str.assign (arg_ptr);
                break;

            case assign_str:
                s_str.assign (arg_str);
                break;

            case assign_ptr_size:
                s_str.assign (arg_ptr, cs.size);
                break;

            case assign_str_off_size:
                s_str.assign (arg_str, cs.off, cs.size);
                break;

            case assign_size_val:
                s_str.assign (cs.size, arg_val);
                break;

            case assign_range: {
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
                       "line %d: %s: size unexpectedly changed "
                       "from %zu to %zu after an exception",
                       __LINE__, funcall, size, s_str.size ());

            rw_assert (s_str.capacity () == capacity, 0, cs.line,
                       "line %d: %s: capacity unexpectedly "
                       "changed from %zu to %zu after an exception",
                       __LINE__, funcall, capacity, s_str.capacity ());


            rw_assert (s_str.begin () == begin, 0, cs.line,
                       "line %d: %s: begin() unexpectedly "
                       "changed from after an exception by %d",
                       __LINE__, funcall, s_str.begin () - begin);


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
               "line %d: %s: failed to throw an expected exception",
               __LINE__, funcall);

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
                        const TestCase &cs,
                        const char     *funcall)
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
               "line %d. %s expected %{#*s}, got %{/*.*Gs}, "
               "difference at off %zu for %s",
               __LINE__, funcall, int (cs.res_len), cs.res,
               int (sizeof (charT)), int (s_str.size ()), s_str.c_str (),
               match, itname);
}

/**************************************************************************/

template <class charT, class Traits>
void test_assign_range (charT          *wstr,
                        charT          *wsrc,
                        Traits*,
                        const TestCase &cs,
                        const char     *funcall)
{
    if (cs.bthrow)  // this method doesn't throw
        return;

    test_assign_range (wstr, wsrc, (Traits*)0,
                       InputIter<charT>(0, 0, 0), cs, funcall);

    // there is no need to call test_assign_range
    // for other iterators in this case
    if (0 == cs.arg)
        return;

    test_assign_range (wstr, wsrc, (Traits*)0,
                       ConstFwdIter<charT>(0, 0, 0), cs, funcall);

    test_assign_range (wstr, wsrc, (Traits*)0,
                       ConstBidirIter<charT>(0, 0, 0), cs, funcall);

    test_assign_range (wstr, wsrc, (Traits*)0,
                       ConstRandomAccessIter<charT>(0, 0, 0), cs, funcall);
}

/**************************************************************************/

template <class charT, class Traits>
void test_assign (charT, Traits*,
                  const AssignOverload  which,
                  const TestCase       &cs,
                  const char           *funcall)
{
    typedef std::allocator<charT>                        Allocator;
    typedef std::basic_string <charT, Traits, Allocator> TestString;
    typedef typename TestString::iterator                StringIter;

    static charT wstr [LLEN];
    static charT wsrc [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (wsrc, cs.arg, cs.arg_len);

    // special processing for assign_range to exercise all iterators
    if (assign_range == which) {
        test_assign_range (wstr, wsrc, (Traits*)0, cs, funcall);
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
    if (1 == cs.bthrow && assign_str_off_size == which)
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
    case assign_ptr:
        res_ptr = &s_str.assign (arg_ptr);
        break;

    case assign_str:
        res_ptr = &s_str.assign (arg_str);
        break;

    case assign_ptr_size:
        res_ptr = &s_str.assign (arg_ptr, size);
        break;

    case assign_str_off_size:
        res_ptr = &s_str.assign (arg_str, cs.off, size);
        break;

    case assign_size_val: {
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
               "line %d. %s returned invalid reference, offset is %zu",
               __LINE__, funcall, res_off);

    // verfiy that strings length are equal
    rw_assert (cs.res_len == s_str.size (), 0, cs.line,
               "line %d. %s expected %{#*s} with length %zu, got %{/*.*Gs} "
               "with length %zu", __LINE__, funcall, int (cs.res_len),
               cs.res, cs.res_len, int (sizeof (charT)), int (s_str.size ()),
               s_str.c_str (), s_str.size ());

    // verfiy that assign results match expected result
    const std::size_t match = rw_match (cs.res, s_str.c_str(), cs.res_len);

    rw_assert (match == cs.res_len, 0, cs.line,
               "line %d. %s expected %{#*s}, got %{/*.*Gs}, "
               "difference at off %zu",
               __LINE__, funcall, int (cs.res_len), cs.res,
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
               "line %d. %s %{?}expected %s, caught %s"
               "%{:}unexpectedly caught %s%{;}",
               __LINE__, funcall, 0 != expected, expected, caught, caught);
}

/**************************************************************************/

static char*
get_assign_format (const MemFun         *pfid,
                   const AssignOverload  which,
                   const TestCase       &cs)
{
    char*       buf     = 0;
    std::size_t bufsize = 0;

    if (   MemFun::DefaultTraits == pfid->tid_
        && (MemFun::Char == pfid->cid_ || MemFun::WChar == pfid->cid_))
        rw_asnprintf (&buf, &bufsize,
                      "std::%{?}w%{;}string (%{#*s}).assign",
                      MemFun::WChar == pfid->cid_,
                      int (cs.str_len), cs.str);
    else
        rw_asnprintf (&buf, &bufsize,
                      "std::basic_string<%s, %s<%1$s>, %s<%1$s>>(%{#*s})"
                      ".assign", pfid->cname_, pfid->tname_, pfid->aname_,
                      int (cs.str_len), cs.str);

    // assignment from self to self?
    const bool self = 0 == cs.arg;

    switch (which) {
    case assign_ptr:
        rw_asnprintf (&buf, &bufsize,
                      "%{+} (%{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      !self, int (cs.arg_len), cs.arg, self);
        break;

    case assign_str:
        rw_asnprintf (&buf, &bufsize,
                      "%{+} (%{?}string (%{#*s})%{;}%{?}*this%{;})",
                      !self, int (cs.arg_len), cs.arg, self);
        break;

    case assign_ptr_size:
        rw_asnprintf (&buf, &bufsize, "%{+} ("
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)",
                      !self, int (cs.arg_len), cs.arg, self, cs.size);
        break;

    case assign_str_off_size:
        rw_asnprintf (&buf, &bufsize, "%{+} ("
                      "%{?}string (%{#*s})%{;}%{?}*this%{;}, %zu, %zu)",
                      !self, int (cs.arg_len), cs.arg,
                      self, cs.off, cs.size);
        break;

    case assign_size_val:
        rw_asnprintf (&buf, &bufsize,
                      "%{+} (%zu, %#c)", cs.size, cs.val);
        break;

    case assign_range:
        rw_asnprintf (&buf, &bufsize, "%{+} ("
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu, "
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu)",
                      !self, int (cs.arg_len), cs.arg,
                      self, cs.off, !self, int (cs.arg_len), cs.arg,
                      self, cs.off + cs.size);
        break;

    default:
        RW_ASSERT (!"test logic error: unknown assign overload");
    }

    return buf;
}

/**************************************************************************/

static void
test_assign (const MemFun *pfid, const AssignOverload which,
             const TestCase& cs, bool exc_safety_test)
{
    // format the description of the function call including
    // the values of arguments for use in diagnostics
    char* const funcall = get_assign_format (pfid, which, cs);

#undef TEST
#define TEST(charT, Traits)                                                 \
    exc_safety_test ?                                                       \
        test_assign_exceptions (charT (), (Traits*)0, which, cs, funcall)   \
      : test_assign (charT(), (Traits*)0, which, cs, funcall)

    if (MemFun::DefaultTraits == pfid->tid_) {
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

    std::free (funcall);
}

/**************************************************************************/

static void
test_assign (const MemFun *pfid, const FunctionTag& ftag)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::%s",
             pfid->cname_, pfid->tname_, pfid->aname_, ftag.funsig);

    if (rw_opt_no_exception_safety)
        rw_note (0, 0, 0,
                 "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
                 "%s exception safety test disabled",
                 pfid->cname_, pfid->tname_, pfid->aname_, ftag.funsig);

#ifdef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    else
        rw_warn (0, 0, __LINE__,
                 "%s exception safety test: no replacable new and delete",
                 ftag.funsig);

#endif  //_RWSTD_NO_REPLACEABLE_NEW_DELETE

    for (std::size_t i = 0; i != ftag.n_cases; ++i) {

        if (!rw_enabled (ftag.t_cases [i].line)) {
            rw_note (0, 0, __LINE__,
                     "test on line %d disabled", ftag.t_cases [i].line);
            continue;
        }

        // do not exercise exceptions if they were disabled
        if (0 != rw_opt_no_exceptions && 0 != ftag.t_cases [i].bthrow)
            continue;

        // do not exercise exception safety if they were disabled
        if (0 != rw_opt_no_exception_safety && -1 == ftag.t_cases [i].bthrow)
            continue;

        test_assign (pfid, ftag.which, ftag.t_cases [i],
                     -1 == ftag.t_cases [i].bthrow);
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
                     "string::assign exceptions tests disabled");

        static const std::size_t ftags =
            sizeof function_tags / sizeof *function_tags;

        for (std::size_t i = 0; i < ftags; i++) {

            if (rw_opt_no_assign [function_tags [i].which])
                rw_note (0, 0, 0,
                         "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
                         "%s test disabled", pfid->cname_, pfid->tname_,
                         pfid->aname_, function_tags [i].funsig);
            else
                test_assign (pfid, function_tags [i]);
        }
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

    if (rw_enabled ("char")) {

        MemFun fid (MemFun::Char, "char", MemFun::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = MemFun::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::assign char tests disabled");

    if (rw_enabled ("wchar_t")) {

        MemFun fid (MemFun::WChar, "wchar_t", MemFun::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = MemFun::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::assign wchar tests disabled");

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
                    "lib.string.assign",
                    0 /* no comment */,
                    run_test,
                    "|-no-char_traits# "
                    "|-no-user_traits# "
                    "|-no-user_chars# "
                    "|-no-exceptions# "
                    "|-no-exception-safety# "

                    "|-no-assign-ptr# "
                    "|-no-assign-str# "
                    "|-no-assign-ptr-size# "
                    "|-no-assign-str-off-size# "
                    "|-no-assign-size-val# "
                    "|-no-assign-range#",

                    &rw_opt_no_char_traits,
                    &rw_opt_no_user_traits,
                    &rw_opt_no_user_chars,
                    &rw_opt_no_exceptions,
                    &rw_opt_no_exception_safety,

                    rw_opt_no_assign + assign_ptr,
                    rw_opt_no_assign + assign_str,
                    rw_opt_no_assign + assign_ptr_size,
                    rw_opt_no_assign + assign_str_off_size,
                    rw_opt_no_assign + assign_size_val,
                    rw_opt_no_assign + assign_range);
}
