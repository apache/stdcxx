/***************************************************************************
 *
 * 21.string.copy.cpp - string test exercising [lib.string::copy]
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
#include <cstddef>      // for size_t
#include <stdexcept>    // for out_of_range

#include <cmdopt.h>     // for rw_enabled()
#include <driver.h>     // for rw_test()

#include <rw_char.h>    // for rw_widen()

/**************************************************************************/

struct MemFun
{
    enum charT  { Char, WChar, UChar };
    enum Traits { DefaultTraits, UserTraits };

    MemFun (charT cid, const char *cname,
          Traits tid, const char *tname)
        : cid_ (cid), tid_ (tid), 
          cname_ (cname), tname_ (tname), aname_ ("allocator"),
          fname_ ("copy") { /* empty */ }

    charT       cid_;     // character type id (char or wchar_t)
    Traits      tid_;     // traits type id (default or user-defined)
    const char *cname_;   // character type name
    const char *tname_;   // traits name
    const char *aname_;   // allocator name
    const char *fname_;   // function name
};

/**************************************************************************/

static const int long_string_len = 4096;
static char long_string [long_string_len];

// for convenience and brevity
#define LSTR long_string
#define LLEN long_string_len

static const char* exp_exceptions[] = 
    { "unknown exception", "out_of_range" };

/**************************************************************************/

static const struct TestCase {

    int  line;

    int  cnt;
    int  pos;

    const char* str;
    std::size_t str_len;

    const char* res;
    std::size_t res_len;

    int bthrow;

} test_cases [] = {

#undef TEST
#define TEST(str, cnt, pos, res, bthrow)                                    \
    { __LINE__, cnt, pos, str, sizeof str - 1, res, sizeof res - 1, bthrow }

    //    +------------------------------------------ controlled sequence
    //    |                 +------------------------ copy() n argument
    //    |                 |   +-------------------- copy() pos argument
    //    |                 |   |  +----------------- expected result sequence
    //    |                 |   |  |             +--- exception info:
    //    |                 |   |  |             |      0 - no exception    
    //    |                 |   |  |             |      1 - out_of_range        
    //    V                 V   V  V             V  
    TEST ("ab",             2, -1, "ab",         0),

    TEST ("",               0, -1, "",           0),
    TEST ("",              10, -1, "",           0),

    TEST ("\0",             1, -1, "\0",         0),
    TEST ("\0\0",           1,  1, "\0",         0),
    TEST ("\0\0",           2, -1, "\0\0",       0),

    TEST ("abc",            1, -1, "a",          0),
    TEST ("abc",            1,  1, "b",          0),
    TEST ("abc",            1,  2, "c",          0),

    TEST ("abc",            0, -1, "",           0),
    TEST ("abc",            2, -1, "ab",         0),
    TEST ("abc",            2,  1, "bc",         0),
    TEST ("abc",            3, -1, "abc",        0),
    TEST ("abc",           10,  1, "bc",         0),
    TEST ("abc",            3,  2, "c",          0),

    TEST ("a\0b\0\0c",     10,  1, "\0b\0\0c",   0),
    TEST ("a\0b\0\0c",     10, -1, "a\0b\0\0c",  0),
    TEST ("a\0b\0\0c",      1,  1, "\0",         0),

    TEST ("\0ab\0\0\0c\0", 10,  1, "ab\0\0\0c\0",0),
    TEST ("\0ab\0\0\0c\0",  5, -1, "\0ab\0\0",   0),
    TEST ("\0ab\0\0\0c\0",  3,  3, "\0\0\0",     0),

    TEST ("\0\0ab\0\0c\0",  6, -1, "\0\0ab\0\0", 0),
    TEST ("\0\0ab\0\0c\0",  4,  1, "\0ab\0",     0),

    TEST (LSTR,      LLEN - 1,        -1, LSTR,  0),
    TEST (LSTR,             2,         1, "xx",  0),
    TEST (LSTR,      LLEN - 1, LLEN -  2, "x",   0),

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",     0,          2, "", 1),
    TEST ("a",      0,         10, "", 1),
    TEST (LSTR,     0,  LLEN + 10, "", 1),

#endif   // _RWSTD_NO_EXCEPTIONS

    TEST ("last",           4, -1, "last",       0)
};

/**************************************************************************/

template <class charT, class Traits>
void test_copy (charT, Traits*,                 
                const MemFun   *pfid, 
                const TestCase &cs)
{
    RW_ASSERT (0 != pfid);

    typedef std::basic_string <charT, Traits, 
                               std::allocator<charT> > TestString;

    static charT wstr [LLEN];
    static const charT eos = charT ();

    const bool use_pos = -1 != cs.pos;

    // construct strings
    rw_widen (wstr, cs.str, cs.str_len);
    const TestString str (wstr, cs.str_len);

    const std::size_t min_len =
        cs.str_len < std::size_t (cs.cnt) ? cs.str_len : cs.cnt;

    // create destination array and initialize it with garbage
    charT* const s_res = new charT [min_len + 1];

    char cgb = '@';
    charT wcgb = make_char (cgb, (charT*)0);
    Traits::assign (s_res, min_len + 1, wcgb);

    // (name of) expected and caught exception
    const char* expected = 0;
    const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

    expected = cs.bthrow && use_pos ? exp_exceptions [1] : 0;

    try {

#endif   // _RWSTD_NO_EXCEPTIONS

    const typename TestString::size_type res = use_pos ? 
        str.copy (s_res, cs.cnt, cs.pos)
      : str.copy (s_res, cs.cnt);

#define CALLFMAT                                                \
    "line %d. std::basic_string<%s, %s<%2$s>, %s<%2$s>>"        \
    "(%{#*s}).copy (%{#*s}, %zu%{?}, %zu%{;})"

#define CALLARGS                                                \
    __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,         \
    int (cs.str_len), cs.str, 1, &eos, cs.cnt,                  \
    use_pos, cs.pos

    // verify the returned value
    rw_assert (res == cs.res_len, 0, cs.line,
               CALLFMAT " == %zu, got %zu", 
               CALLARGS, cs.res_len, res);

    std::size_t match = rw_match (cs.res, s_res, cs.res_len);
    bool success = match == cs.res_len;

    rw_assert (success, 0, cs.line,
               CALLFMAT " expected %{#*s}, got %{/*.*Gs}, differ at pos %zu",
               CALLARGS, int (cs.res_len), cs.res, 
               int (sizeof (charT)), int (res), s_res, match);

    success = 1 == rw_match (&cgb, &s_res [min_len], 1);
    rw_assert (success, 0, cs.line,
               CALLFMAT " detected writing past the end of "
               "the provided buffer", CALLARGS);

#ifndef _RWSTD_NO_EXCEPTIONS

    }
    catch (std::out_of_range) {
        caught = exp_exceptions [1];
    }
    catch (...) {
        caught = exp_exceptions [0];
    }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

#endif   // _RWSTD_NO_EXCEPTIONS

    rw_assert (caught == expected, 0, cs.line,
               CALLFMAT " %{?}expected %s, caught %s"
               "%{:}unexpectedly caught %s%{;}",
               CALLARGS, 0 != expected, expected, caught, caught);

    delete[] s_res;
}

/**************************************************************************/

static int rw_opt_no_char_traits;              // for --no-char_traits
static int rw_opt_no_user_traits;              // for --no-user_traits

static int rw_opt_no_user_chars;               // for --no-user_chars
static int rw_opt_no_exceptions;               // for --no-exceptions

static int rw_opt_no_copy;                     // for --no-copy
static int rw_opt_no_copy_pos;                 // for --no-copy-pos

/**************************************************************************/

static void 
note_test_disabled (const MemFun *pfid, bool use_pos)
{
    RW_ASSERT (0 != pfid);

    rw_note (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
             "copy (charT* s, size_type n%{?}, size_type pos%{;}) "
             "test disbled",
             pfid->cname_, pfid->tname_, pfid->aname_, use_pos);
}

static void 
test_copy (const MemFun *pfid) 
{
    RW_ASSERT (0 != pfid);

    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
             "copy (charT* s, size_type n, size_type pos = 0)",
             pfid->cname_, pfid->tname_, pfid->aname_);

    if (rw_opt_no_copy)
        note_test_disabled (pfid, false);

    if (rw_opt_no_copy_pos) 
        note_test_disabled (pfid, true);

#undef TEST
#define TEST(charT, Traits, cs)	                            \
        test_copy (charT(), (Traits*)0, pfid, cs)

    static const std::size_t ncases = sizeof test_cases / sizeof *test_cases;

    for (std::size_t i = 0; i != ncases; ++i) {

        if (!rw_enabled (test_cases[i].line)) {
            rw_note (0, 0, __LINE__, 
                     "test on line %d disabled", test_cases[i].line);
            continue;
        }

        // do not exercise copy (charT* s, size_type n)
        if (rw_opt_no_copy && -1 == test_cases[i].pos)
            continue;

        // do not exercise copy (charT* s, size_type n, size_type pos)
        if (rw_opt_no_copy_pos && -1 != test_cases[i].pos)
            continue;

        // do not exercise exceptions if they were disabled
        if (0 != rw_opt_no_exceptions && 0 != test_cases[i].bthrow)
            continue;

        if (MemFun:: DefaultTraits == pfid->tid_) {
            if (MemFun::Char == pfid->cid_)
                TEST (char, std::char_traits<char>, test_cases[i]);

#ifndef _RWSTD_NO_WCHAR_T
            else
                TEST (wchar_t, std::char_traits<wchar_t>, test_cases[i]);
#endif   // _RWSTD_NO_WCHAR_T

        }
        else {
            if (MemFun::Char == pfid->cid_)
                TEST (char, UserTraits<char>, test_cases[i]);

#ifndef _RWSTD_NO_WCHAR_T
            else if (MemFun::WChar == pfid->cid_)
                TEST (wchar_t, UserTraits<wchar_t>, test_cases[i]);
#endif   // _RWSTD_NO_WCHAR_T

            else
                TEST (UserChar, UserTraits<UserChar>, test_cases[i]);
        }
    }
}

/**************************************************************************/

static void
run_test (const MemFun *pfid)
{
    RW_ASSERT (0 != pfid);

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
                     "string::copy exceptions tests disabled");

        test_copy (pfid);
    }
}

/**************************************************************************/

int run_test (int, char*[])
{
    if ('\0' == LSTR [0]) {
        // initialize long_string
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
        rw_note (0, 0, 0, "string::copy char tests disabled");

    if (rw_enabled ("wchar_t")) {

        MemFun fid (MemFun::WChar, "wchar_t", MemFun::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = MemFun::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::copy wchar tests disabled");

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
                    "lib.string.copy",
                    0 /* no comment */, run_test,
                    "|-no-char_traits# "
                    "|-no-user_traits# "
                    "|-no-user_chars# "
                    "|-no-exceptions# "
                    "|-no-copy# "
                    "|-no-copy-pos#",
                    &rw_opt_no_char_traits,
                    &rw_opt_no_user_traits,
                    &rw_opt_no_user_chars,
                    &rw_opt_no_exceptions,
                    &rw_opt_no_copy,
                    &rw_opt_no_copy_pos);
}
