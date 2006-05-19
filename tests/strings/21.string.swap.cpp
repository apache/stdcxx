/***************************************************************************
 *
 * 21.string.swap.cpp - string test exercising [lib.string::swap]
 *
 * $Id $ 
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

#include <string>           // for string
#include <cstddef>          // size_t
#include <exception>        // for exception

#include <21.strings.h>     // for StringMembers
#include <driver.h>         // for rw_assert()
#include <rw_allocator.h>   // foir UserAlloc
#include <rw_char.h>        // for rw_expand()
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Swap(which)               StringMembers::swap_ ## which

typedef StringMembers::TestCase   TestCase;
typedef StringMembers::Function   Function;

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// swap (basic_string&)
static const TestCase
str_test_cases [] = {

#undef TEST
#define TEST(str, arg)                                  \
    { __LINE__, -1, -1, -1, -1, -1,                     \
      str, sizeof str - 1, arg, sizeof arg - 1,         \
      0, 0, 0                                           \
    }

    //    +------------------------- controlled "destination" (str) sequence
    //    |            +------------ controlled "source" (arg) sequence
    //    |            |
    //    V            V
    TEST ("",          ""),
    TEST ("",          "a"),
    TEST ("a",         ""),
    TEST ("",          "\0"),
    TEST ("\0",        ""),

    TEST ("a",         "b"),
    TEST ("a",         "bc"),
    TEST ("ab",        "c"),

    TEST ("a\0b\0\0c", "\0b\0\0c"),
    TEST ("\0b\0\0c",  "a\0b\0\0c"),

    TEST ("a\0b\0\0c", "\0\0"),
    TEST ("\0\0",      "a\0b\0\0c"),

    TEST ("x@4096",    ""),
    TEST ("",          "x@4096"),
    TEST ("x@4096",    "\0\0\0"),
    TEST ("\0\0\0",    "x@4096"),
    TEST ("x@4096",    "x@4096"),

    TEST ("",          0),
    TEST ("\0",        0),
    TEST ("abc",       0),
    TEST ("a\0b\0\0c", 0),
    TEST ("x@4096",    0),

    TEST (0,           ""),
    TEST (0,           "\0"),       
    TEST (0,           "abc@1024"),       
    TEST (0,           "a\0b\0\0c"), 
    TEST (0,           "x@4096"),   
 
    TEST ("last",      "test")
};


/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_swap (charT, Traits*, Allocator*,
                const Function&,
                const TestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef UserTraits<UserChar>::MemFun                 UTMemFun;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    static charT warg_buf [BUFSIZE];

    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    std::size_t arg_len = sizeof warg_buf / sizeof *warg_buf;

    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);
    charT* warg = rw_expand (warg_buf, tcase.arg, tcase.arg_len, &arg_len);

    // construct the string object and the argument string
    String s_str (wstr, str_len);
    String s_arg (warg, arg_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    if (warg != warg_buf)
        delete[] warg;

    wstr = 0;
    warg = 0;

    if (0 == tcase.arg)
        arg_len = str_len;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (s_str));

    const charT* const p1 = s_str.data ();
    const charT* const p2 = tcase.arg ? s_arg.data () : s_str.data ();

    const char* const src     = tcase.arg ? tcase.arg : tcase.str;
    const std::size_t src_len = tcase.arg ? tcase.arg_len : tcase.str_len;
    String& arg_str           = tcase.arg ? s_arg : s_str;

    std::size_t n_total_op_assign  = 0;
    std::size_t n_total_op_assign2 = 0;
    std::size_t n_total_op_copy    = 0;
    std::size_t n_total_op_move    = 0;

    std::size_t* const rg_calls = rw_get_call_counters ((Traits*)0, (charT*)0);
    if (rg_calls) {
        n_total_op_assign  = rg_calls[UTMemFun::assign];
        n_total_op_assign2 = rg_calls[UTMemFun::assign2];
        n_total_op_copy    = rg_calls[UTMemFun::copy];
        n_total_op_move    = rg_calls[UTMemFun::move];
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    // no exceptions expected
    const char* expected = 0;
    const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    try {

        // start checking for memory leaks
        rwt_check_leaks (0, 0);

        if (0 == tcase.str)
            String ().swap (arg_str);
        else
            s_str.swap (arg_str);

        if (rg_calls) {

            std::size_t n_op_assign  = 
                rg_calls[UTMemFun::assign]  - n_total_op_assign;
            std::size_t n_op_assign2 = 
                rg_calls[UTMemFun::assign2] - n_total_op_assign2;
            std::size_t n_op_copy    = 
                rg_calls[UTMemFun::copy]    - n_total_op_copy;
            std::size_t n_op_move    = 
                rg_calls[UTMemFun::move]    - n_total_op_move;

            bool success = 
                0 == (n_op_assign | n_op_assign2 | n_op_copy | n_op_move);

            rw_assert (success, 0, tcase.line, 
                       "line %d. %{$FUNCALL}: complexity: %zu assigns, "
                       "%zu assign2s, %zu copies, %zu moves", __LINE__,
                       n_op_assign, n_op_assign2, n_op_copy, n_op_move);
        }

        if (0 == tcase.str) {

            rw_assert (0 == arg_str.capacity (), 0, tcase.line,
                       "line %d. %{$FUNCALL}: expected 0 capacity, got %zu",
                       __LINE__, arg_str.capacity ());
        }
        else {

            const charT* const res_p1 = s_str.data ();
            const charT* const res_p2 = 
                tcase.arg ? s_arg.data () : s_str.data ();

            const std::size_t res1_len = s_str.size ();
            const std::size_t res2_len = 
                tcase.arg ? s_arg.size () : s_str.size ();

            rw_assert (res_p1 == p2 && res_p2 == p1, 0, tcase.line,
                    "line %d. %{$FUNCALL}: got offset %td from expected value,"
                    " arg.data (): got offset %td from expected value", 
                    __LINE__, p2 - res_p1, p1 - res_p2);

            std::size_t match = rw_match (tcase.str, res_p2, tcase.str_len);

            rw_assert (match == str_len, 0, tcase.line,
                    "line %d. %{$FUNCALL}: this == %{#*s}, got this = "
                    "%{/*.*Gs}, differs at pos %zu", 
                    __LINE__, int (src_len), src, int (sizeof (charT)), 
                    int (res1_len), res_p1, match);

            match = rw_match (src, res_p1, src_len);

            rw_assert (match == arg_len, 0, tcase.line,
                    "line %d. %{$FUNCALL}: str == %{#*s}, got str = "
                    "%{/*.*Gs}, differs at pos %zu",  
                    __LINE__, int (tcase.str_len), tcase.str, 
                    int (sizeof (charT)), int (res2_len), res_p2, match);
        }
    }

#ifndef _RWSTD_NO_EXCEPTIONS

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
        str_state.assert_equal (rw_get_string_state (s_str),
                                __LINE__, tcase.line, caught);
    }
    else if (-1 != tcase.bthrow) {
        rw_assert (caught == expected, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                   "%{:}unexpectedly caught %s%{;}",
                   __LINE__, 0 != expected, expected, caught, caught);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_swap);

int main (int argc, char** argv)
{
    static const StringMembers::Test
    tests [] = {

#undef TEST
#define TEST(tag) {                                             \
        StringMembers::swap_ ## tag,                            \
        tag ## _test_cases,                                     \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases  \
    }

        TEST (str)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return StringMembers::run_test (argc, argv, __FILE__,
                                    "lib.string.swap",
                                    test_swap, tests, test_count);
}
