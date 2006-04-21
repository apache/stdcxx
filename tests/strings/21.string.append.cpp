/***************************************************************************
 *
 * 21.string.append.cpp - string test exercising lib.string.append
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


#define AppendOverload   StringMembers::MemberFunction
#define Append(which)    StringMembers::append_ ## which
#define Disabled(which)  StringMembers::opt_memfun_disabled [which]

typedef StringMembers::TestCase TestCase;
typedef StringMembers::Test     Test;

/**************************************************************************/

struct MemFun
{
    typedef StringMembers::charT  charT;
    typedef StringMembers::Traits Traits;

    MemFun (charT cid, const char *cname,
          Traits tid, const char *tname)
        : cid_ (cid), tid_ (tid),
          cname_ (cname), tname_ (tname), aname_ ("allocator"),
          fname_ ("append") { /* empty */ }

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

// used to exercise
// append (const charT* s)
static const TestCase
ptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {                           \
        Append (ptr), __LINE__, -1, -1, -1, -1, -1,             \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |             +--------------------------- sequence to be appended
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info
    //    |             |             |        |         0 - no exception
    //    |             |             |        |         1 - out_of_range
    //    |             |             |        |         2 - length_error
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
// append (const basic_string& str)
static const TestCase
str_test_cases [] = {

#undef TEST
#define TEST(s, arg, res, bthrow) {                             \
        Append (str), __LINE__, -1, -1, -1, -1, -1,             \
        s, sizeof s - 1,                                        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |             +--------------------------- sequence to be appended
    //    |             |             +------------- expected result sequence
    //    |             |             |        +---- exception info
    //    |             |             |        |         0 - no exception
    //    |             |             |        |         1 - out_of_range
    //    |             |             |        |         2 - length_error
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
// append (const charT* s, size_type n)
static const TestCase
ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, size, res, bthrow) {                     \
        Append (ptr_size), __LINE__, -1, size, -1, -1, -1,      \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- sequence to be appended
    //    |            |            +------------ append() n argument
    //    |            |            |  +--------- expected result sequence
    //    |            |            |  |     +--- exception info
    //    |            |            |  |     |      0 - no exception
    //    |            |            |  |     |      1 - out_of_range
    //    |            |            |  |     |      2 - length_error
    //    |            |            |  |     |     -1 - exc. safety
    //    |            |            |  |     |
    //    |            |            |  |     +------------+
    //    V            V            V  V                  V
    TEST ("ab",        "c",         1, "abc",             0),

    TEST ("",          "",          0,  "",               0),
    TEST ("",          "abc",       1,  "a",              0),
    TEST ("",          "\0",        1,  "\0",             0),

    TEST ("\0",        "",          0,  "\0",             0),
    TEST ("\0",        "a",         0,  "\0",             0),
    TEST ("\0",        "a",         1,  "\0a",            0),
    TEST ("\0",        "\0\0",      1,  "\0\0",           0),
    TEST ("\0",        "\0\0",      2,  "\0\0\0",         0),

    TEST ("cde",       "ab",        2,  "cdeab",          0),
    TEST ("cde",       "ab",        1,  "cdea",           0),

    TEST ("\0e\0",     "a\0b\0\0c", 0,  "\0e\0",          0),
    TEST ("\0e\0",     "\0ab\0\0c", 3,  "\0e\0\0ab",      0),

    TEST ("a\0b\0\0c", "\0e\0",     3,  "a\0b\0\0c\0e\0", 0),
    TEST ("a\0b\0\0c", "\0\0e\0",   2,  "a\0b\0\0c\0\0",  0),
    TEST ("\0ab\0\0c", "\0e\0",     1,  "\0ab\0\0c\0",    0),
    TEST ("a\0bc\0\0", "\0e",       2,  "a\0bc\0\0\0e",   0),

    TEST ("",          0,           0,  "",               0),
    TEST ("abc",       0,           0,  "abc",            0),
    TEST ("abc",       0,           1,  "abca",           0),
    TEST ("abc",       0,           2,  "abcab",          0),
    TEST ("a\0bc",     0,           2,  "a\0bca\0",       0),
    TEST ("\0abc\0\0", 0,           1,  "\0abc\0\0\0",    0),
    TEST ("a\0bc\0\0", 0,           3,  "a\0bc\0\0a\0b",  0),

    TEST ("",          LSTR, LLEN - 1,  LSTR,             0),
    TEST (LSTR,        "",          0,  LSTR,             0),

    TEST ("",          LSTR, LLEN - 1,  LSTR,            -1),

    TEST ("last",      "test",      4, "lasttest",        0)
};

/**************************************************************************/

// used to exercise
// append (const basic_string& str, size_type off, size_type n)
// append (InputIterator first, InputIterator last)
static const TestCase
range_test_cases [] = {

// range_test_cases serves a double duty
#define str_off_size_test_cases range_test_cases

#undef TEST
#define TEST(str, arg, off, size, res, bthrow) {                \
        Append (str_off_size), __LINE__, off, size, -1, -1, -1, \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- sequence to be appended
    //    |            |            +------------ append() pos argument
    //    |            |            |  +--------- append() n argument
    //    |            |            |  |  +------ expected result sequence
    //    |            |            |  |  |  +--- exception info
    //    |            |            |  |  |  |       0 - no exception
    //    |            |            |  |  |  |       1 - out_of_range
    //    |            |            |  |  |  |       2 - length_error
    //    |            |            |  |  |  |      -1 - exc. safety
    //    |            |            |  |  |  |
    //    |            |            |  |  |  +----------------+
    //    V            V            V  V  V                   V
    TEST ("ab",        "c",         0, 1, "abc",              0),

    TEST ("",          "",          0, 0,  "",                0),
    TEST ("",          "abc",       1, 1,  "b",               0),
    TEST ("",          "\0",        0, 1,  "\0",              0),

    TEST ("\0",        "",          0, 0,  "\0",              0),

    TEST ("abc",       "",          0, 0,  "abc",             0),

    TEST ("\0",        "a",         0, 1,  "\0a",             0),
    TEST ("\0",        "\0\0",      1, 1,  "\0\0",            0),
    TEST ("\0",        "\0\0",      0, 2,  "\0\0\0",          0),
    TEST ("\0",        "\0\0",      1, 5,  "\0\0",            0),

    TEST ("cde",       "ab",        0, 2,  "cdeab",           0),
    TEST ("cde",       "ab",        0, 1,  "cdea",            0),
    TEST ("cde",       "ab",        1, 5,  "cdeb",            0),

    TEST ("ab",        "c\0e",      0, 3,  "abc\0e",          0),
    TEST ("ab",        "c\0e",      1, 2,  "ab\0e",           0),
    TEST ("ab",        "c\0e",      0, 2,  "abc\0",           0),

    TEST ("\0e\0",     "\0ab\0\0c", 0, 9,  "\0e\0\0ab\0\0c",  0),
    TEST ("\0e\0",     "\0ab\0\0c", 0, 3,  "\0e\0\0ab",       0),
    TEST ("a\0b\0\0c", "\0e\0",     0, 3,  "a\0b\0\0c\0e\0",  0),
    TEST ("a\0b\0\0c", "\0\0e\0",   0, 2,  "a\0b\0\0c\0\0",   0),
    TEST ("\0ab\0\0c", "\0e\0",     2, 1,  "\0ab\0\0c\0",     0),
    TEST ("a\0bc\0\0", "\0e",       0, 2,  "a\0bc\0\0\0e",    0),

    TEST ("",          0,           0, 0,  "",                0),
    TEST ("abc",       0,           1, 0,  "abc",             0),
    TEST ("abc",       0,           1, 1,  "abcb",            0),
    TEST ("abc",       0,           0, 2,  "abcab",           0),
    TEST ("a\0bc\0\0", 0,           4, 2,  "a\0bc\0\0\0\0",   0),
    TEST ("a\0bc\0\0", 0,           1, 3,  "a\0bc\0\0\0bc",   0),
    TEST ("a\0bc\0\0", 0,           3, 9,  "a\0bc\0\0c\0\0",  0),
    TEST ("abcdef",    0,           1, 2,  "abcdefbc",        0),

    TEST ("",          LSTR,        0, LLEN, LSTR,            0),
    TEST (LSTR,        "",          0, 0,    LSTR,            0),

    TEST ("",          "\0",        2, 0,  "",                1),
    TEST ("",          "a",         2, 0,  "",                1),
    TEST ("",          LSTR,LLEN + 10, 0,  "",                1),

    TEST (LSTR,        0,           0, 0, 0,                 -1),

    TEST ("last",      "test",      0, 4, "lasttest",         0)
};

/**************************************************************************/

// used to exercise
// append (charT c, size_type n)
static const TestCase
size_val_test_cases [] = {

#undef TEST
#define TEST(str, size, val, res, bthrow) {                     \
        Append (size_val), __LINE__, -1, size, -1, -1, val,     \
        str, sizeof str - 1,                                    \
        0, 0, res, sizeof res - 1, bthrow                       \
    }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- append() count argument
    //    |            |   +--------------------- character to be appended
    //    |            |   |   +----------------- expected result sequence
    //    |            |   |   |       +--------- exception info
    //    |            |   |   |       |             0 - no exception
    //    |            |   |   |       |             1 - out_of_range
    //    |            |   |   |       |             2 - length_error
    //    |            |   |   |       |            -1 - exc. safety
    //    |            |   |   |       |
    //    |            |   |   |       +-----------+
    //    V            V   V   V                   V
    TEST ("ab",        1, 'c', "abc",              0),

    TEST ("",          0, ' ',  "",                0),
    TEST ("",          1, 'b',  "b",               0),
    TEST ("",          3, 'b',  "bbb",             0),

    TEST ("\0",        0, ' ',  "\0",              0),
    TEST ("",          2, '\0', "\0\0",            0),

    TEST ("\0",        1, 'a',  "\0a",             0),
    TEST ("\0",        1, '\0', "\0\0",            0),
    TEST ("\0",        2, '\0', "\0\0\0",          0),
    TEST ("\0",        0, '\0', "\0",              0),

    TEST ("cde",       1, 'a',  "cdea",            0),
    TEST ("cde",       2, 'a',  "cdeaa",           0),
    TEST ("cde",       3, 'a',  "cdeaaa",          0),

    TEST ("ab",        2, '\0', "ab\0\0",          0),
    TEST ("ab",        1, '\0', "ab\0",            0),
    TEST ("ab",        2, '\0', "ab\0\0",          0),

    TEST ("a\0b\0\0c", 2, '\0', "a\0b\0\0c\0\0",   0),
    TEST ("a\0b\0\0c", 1, '\0', "a\0b\0\0c\0",     0),
    TEST ("\0ab\0\0c", 3, '\0', "\0ab\0\0c\0\0\0", 0),
    TEST ("a\0bc\0\0", 2, 'a',  "a\0bc\0\0aa",     0),

    TEST ("",          LLEN - 1, 'x', LSTR,        0),
    TEST (LSTR,        0,        'x', LSTR,        0),

    TEST ("",          LLEN - 1, 'x', LSTR,       -1),

    TEST ("last",      4, 't', "lasttttt",         0)
};

/**************************************************************************/

static const StringMembers::Test
tests [] = {

#undef TEST
#define TEST(tag, sig) {                                        \
        tag ## _test_cases,                                     \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases, \
        "append " sig                                           \
    }

    TEST (ptr,          "(const value_type*)"),
    TEST (str,          "(const basic_string&)"),
    TEST (ptr_size,     "(const value_type*, size_type)"),
    TEST (str_off_size, "(const basic_string&, size_type, size_type)"),
    TEST (size_val,     "(size_type, value_type)"),
    TEST (range,        "(InputIterator, InputIterator)")
};

/**************************************************************************/

template <class charT, class Traits>
void test_exceptions (charT, Traits*,
                      const TestCase &cs,
                      const char     *funcall)
{
    typedef std::basic_string <charT, Traits,
                               std::allocator<charT> > TestString;
    typedef typename TestString::iterator StringIter;
    typedef typename TestString::const_iterator ConstStringIter;

    static charT wstr [LLEN];
    static charT wsrc [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (wsrc, cs.arg, cs.arg_len);

    TestString s_str (wstr, cs.str_len);
    TestString s_arg (wsrc, cs.arg_len);

    std::size_t throw_after = 0;

    const std::size_t     size     = s_str.size ();
    const std::size_t     capacity = s_str.capacity ();
    const ConstStringIter begin    = s_str.begin ();

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rwt_free_store* const pst = rwt_get_free_store (0);

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // iterate for`n=throw_after' starting at the next call to operator
    // new, forcing each call to throw an exception, until the appendion
    // finally succeeds (i.e, no exception is thrown)
    for ( ; ; ) {

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_after + 1;

#  endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE
#endif   // _RWSTD_NO_EXCEPTIONS

        _TRY {
            if (Append (ptr) == cs.which)
                s_str.append (cs.arg ? wsrc : s_str.c_str ());

            else if (Append (str) == cs.which)
                s_str.append (cs.arg ? s_arg : s_str);

            else if (Append (ptr_size) == cs.which)
                s_str.append (cs.arg ? wsrc : s_str.c_str (), cs.size);

            else if (Append (str_off_size) == cs.which)
                s_str.append (cs.arg ? s_arg : s_str, cs.off, cs.size);

            else if (Append (size_val) == cs.which)
                s_str.append (cs.size, make_char (char (cs.val), (charT*)0));

            else if (Append (range) == cs.which)
                s_str.append (s_arg.begin (), s_arg.end ());

            break;
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
            // and force the next one to fail, and try to append again
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
void test_append_range (charT* wstr,
                        charT* wsrc,
                        Traits*,
                        const Iterator &it,
                        const TestCase &cs,
                        const char     *funcall)
{
    typedef std::basic_string <charT, Traits,
                               std::allocator<charT> > String;
    typedef typename String::iterator StringIter;

    const char* const itname =
        cs.arg ? type_name (it, (charT*)0) : "basic_string::iterator";

    String s_str (wstr, cs.str_len);
    String s_arg (wsrc, cs.arg_len);

    std::size_t off_last = cs.off + cs.size;

    if (cs.arg) {
        off_last = off_last > s_arg.size () ? s_arg.size () : off_last;

        const Iterator first = make_iter (wsrc + cs.off,
            wsrc + cs.off, wsrc + off_last, Iterator (0, 0, 0));
        const Iterator last  = make_iter (wsrc + off_last,
            wsrc + cs.off, wsrc + off_last, Iterator (0, 0, 0));

        s_str.append (first, last);
    }
    else {
        StringIter first (s_str.begin () + cs.off);
        StringIter last  (off_last > s_str.size () ?
            s_str.end ()
          : s_str.begin () + off_last);

        s_str.append (first, last);
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
void test_append_range (charT* wstr,
                        charT* wsrc,
                        Traits*,
                        const TestCase &cs,
                        const char     *funcall)
{
    if (cs.bthrow)  // this method doesn't throw
        return;

    test_append_range (wstr, wsrc, (Traits*)0,
                       InputIter<charT>(0, 0, 0), cs, funcall);

    // there is no need to call test_append_range
    // for other iterators in this case
    if (0 == cs.arg)
        return;

    test_append_range (wstr, wsrc, (Traits*)0,
                       ConstFwdIter<charT>(0, 0, 0), cs, funcall);

    test_append_range (wstr, wsrc, (Traits*)0,
                       ConstBidirIter<charT>(0, 0, 0), cs, funcall);

    test_append_range (wstr, wsrc, (Traits*)0,
                       ConstRandomAccessIter<charT>(0, 0, 0), cs, funcall);
}

/**************************************************************************/

template <class charT, class Traits>
void test_append (charT, Traits*,
                  const TestCase &cs,
                  const char     *funcall)
{
    typedef std::basic_string <charT, Traits,
                               std::allocator<charT> > TestString;
    typedef typename TestString::iterator StringIter;

    static charT wstr [LLEN];
    static charT wsrc [LLEN];

    rw_widen (wstr, cs.str, cs.str_len);
    rw_widen (wsrc, cs.arg, cs.arg_len);

    // special processing for append_range to exercise all iterators
    if (Append (range) == cs.which) {
        test_append_range (wstr, wsrc, (Traits*)0, cs, funcall);
        return;
    }

    TestString s_str (wstr, cs.str_len);
    TestString s_arg (wsrc, cs.arg_len);

    std::size_t res_off = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    if (1 == cs.bthrow && Append (str_off_size) == cs.which)
        expected = exp_exceptions [1];
    if (2 == cs.bthrow)
        expected = exp_exceptions [2];

    const char* caught = 0;

    try {

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (cs.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    switch (cs.which) {
    case Append (ptr): {
        TestString& s_res = s_str.append (cs.arg ? wsrc : s_str.c_str ());
        res_off = &s_res - &s_str;
        break;
    }

    case Append (str): {
        TestString& s_res = s_str.append (cs.arg ? s_arg : s_str);
        res_off = &s_res - &s_str;
        break;
    }

    case Append (ptr_size): {
        TestString& s_res =
            s_str.append (cs.arg ? wsrc : s_str.c_str (), cs.size);
        res_off = &s_res - &s_str;
        break;
    }

    case Append (str_off_size): {
        TestString& s_res =
            s_str.append (cs.arg ? s_arg : s_str, cs.off, cs.size);
        res_off = &s_res - &s_str;
        break;
    }

    case Append (size_val): {
        TestString& s_res =
            s_str.append (cs.size, make_char (char (cs.val), (charT*)0));
        res_off = &s_res - &s_str;
        break;
    }

    default:
        RW_ASSERT (!"test logic error: unknown append overload");
    }

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

    // verfiy that append results match expected result
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
               __LINE__, funcall, 0 != expected, expected, caught, caught);
}

/**************************************************************************/

static void
test_append (const MemFun *pfid,
             const TestCase& cs, bool exc_safety_test)
{
    // format the description of the function call including
    // the values of arguments for use in diagnostics
    char* const funcall =
        StringMembers::format (pfid->cid_, pfid->tid_,
                               StringMembers::DefaultAllocator,
                               cs);

#undef TEST
#define TEST(charT, Traits)	                                          \
    exc_safety_test ?                                                     \
        test_exceptions (charT (), (Traits*)0, cs, funcall) \
      : test_append (charT (), (Traits*)0, cs, funcall)

    if (StringMembers::DefaultTraits == pfid->tid_) {
        if (StringMembers::Char == pfid->cid_)
            TEST (char, std::char_traits<char>);

#ifndef _RWSTD_NO_WCHAR_T
    else
        TEST (wchar_t, std::char_traits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T

    }
    else {
       if (StringMembers::Char == pfid->cid_)
           TEST (char, UserTraits<char>);

#ifndef _RWSTD_NO_WCHAR_T
       else if (StringMembers::WChar == pfid->cid_)
           TEST (wchar_t, UserTraits<wchar_t>);
#endif   // _RWSTD_NO_WCHAR_T

       else
           TEST (UserChar, UserTraits<UserChar>);
    }

    std::free (funcall);
}

/**************************************************************************/

static void
test_append (const MemFun *pfid, const Test& test)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::%s",
             pfid->cname_, pfid->tname_, pfid->aname_, test.funsig);

    if (StringMembers::opt_no_exception_safety)
        rw_note (0, 0, 0,
                 "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
                 "%s exception safety test disabled",
                 pfid->cname_, pfid->tname_, pfid->aname_, test.funsig);

#ifdef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    else
        rw_warn (0, 0, __LINE__,
                 "%s exception safety test: no replacable new and delete",
                 test.funsig);

#endif  //_RWSTD_NO_REPLACEABLE_NEW_DELETE

    for (std::size_t i = 0; i != test.case_count; ++i) {

        if (!rw_enabled (test.cases [i].line)) {
            rw_note (0, 0, __LINE__,
                     "test on line %d disabled", test.cases [i].line);
            continue;
        }

        // do not exercise exceptions if they were disabled
        if (   0 != StringMembers::opt_no_exceptions
            && 0 != test.cases [i].bthrow)
            continue;

        // do not exercise exception safety if they were disabled
        if (   0 != StringMembers::opt_no_exception_safety
            && -1 == test.cases [i].bthrow)
            continue;

        test_append (pfid, test.cases [i], -1 == test.cases [i].bthrow);
    }
}


/**************************************************************************/

static void
run_test (const MemFun *pfid)
{
    if (   StringMembers::UserTraits == pfid->tid_
        && StringMembers::opt_no_user_traits) {
        rw_note (1 < StringMembers::opt_no_user_traits++, 0, 0,
                 "user defined traits test disabled");
    }
    else if (   StringMembers::DefaultTraits == pfid->tid_
             && StringMembers::opt_no_char_traits) {
        rw_note (1 < StringMembers::opt_no_char_traits++, 0, 0,
                 "char_traits test disabled");
    }
    else {

        if (StringMembers::opt_no_exceptions)
            rw_note (1 < StringMembers::opt_no_exceptions++, 0, 0,
                     "string::append exceptions tests disabled");

        static const std::size_t ntests = sizeof tests / sizeof *tests;

        for (std::size_t i = 0; i < ntests; i++) {

            if (Disabled (tests [i].cases [0].which))
                rw_note (0, 0, 0,
                         "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
                         "%s test disabled", pfid->cname_, pfid->tname_,
                         pfid->aname_, tests [i].funsig);
            else
                test_append (pfid, tests [i]);
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

        MemFun fid (StringMembers::Char, "char",
                    StringMembers::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = StringMembers::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::append char tests disabled");

    if (rw_enabled ("wchar_t")) {

        MemFun fid (StringMembers::WChar, "wchar_t",
                    StringMembers::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = StringMembers::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::append wchar tests disabled");

    if (StringMembers::opt_no_user_char) {
        rw_note (0, 0, 0, "user defined chars test disabled");
    }
    else {
        MemFun fid (StringMembers::UChar, "UserChar",
                    StringMembers::UserTraits, 0);
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
                    "lib.string.append",
                    0 /* no comment */,
                    run_test,
                    "|-no-char_traits# "
                    "|-no-user_traits# "
                    "|-no-user_char# "
                    "|-no-exceptions# "
                    "|-no-exception-safety# "

                    "|-no-append-ptr# "
                    "|-no-append-str# "
                    "|-no-append-ptr-size# "
                    "|-no-append-str-off-size# "
                    "|-no-append-size-val# "
                    "|-no-append-range#",

                    &StringMembers::opt_no_char_traits,
                    &StringMembers::opt_no_user_traits,
                    &StringMembers::opt_no_user_char,
                    &StringMembers::opt_no_exceptions,
                    &StringMembers::opt_no_exception_safety,

                    &Disabled (Append (ptr)),
                    &Disabled (Append (str)),
                    &Disabled (Append (ptr_size)),
                    &Disabled (Append (str_off_size)),
                    &Disabled (Append (size_val)),
                    &Disabled (Append (range)),
                    // sentinel
                    (void*)0);
}
