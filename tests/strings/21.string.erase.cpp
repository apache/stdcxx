/***************************************************************************
 *
 * erase.cpp - string test exercising [lib.string::erase]
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
          fname_ ("erase") { /* empty */ }

    charT       cid_;     // character type id (char or wchar_t)
    Traits      tid_;     // traits type id (default or user-defined)
    const char *cname_;   // character type name
    const char *tname_;   // traits name
    const char *aname_;   // allocator name
    const char *fname_;   // function name
};

static const int long_string_len = 4096;
static char long_string [long_string_len];

/**************************************************************************/
// exercises basic_string::erase, 21.3.5.5
// which value determines which erase version to be exercised:
// 1    erase ();
// 2    erase (size_type pos);
// 3    erase (size_type pos, size_type cnt);
// 4    erase (iterator p);
// 5    erase (iterator first, iterator last);
template <class charT, class Traits>
void test_erase (charT, Traits*, MemFun* pfid,
                 int         line,
                 int         which,
                 const char *str,
                 std::size_t str_len,
                 int         pos,
                 int         cnt,
                 const char *res,
                 std::size_t res_len,
                 bool        should_throw)
{
    typedef std::allocator<charT> Allocator;
    typedef std::basic_string<charT, Traits, Allocator> TestString;

    if (!rw_enabled (line)) {
        rw_note (0, 0, 0, "test on line %d disabled", line);
        return;
    }

    // widen the source sequence into the (possibly wide) character buffer
    static charT wstr [long_string_len];
    rw_widen (wstr, str, str_len);

    static charT wres [long_string_len];
    rw_widen (wres, res, res_len);

    charT* pwres = 0 == str ? 0 : wres;

    TestString str_ob (wstr, str_len);
    TestString str_def;

    TestString* pstr = 0 != str ? &str_ob : &str_def;
    TestString& s_res = *pstr;
    typename TestString::iterator it_res = pstr->begin();

    int last = pos + cnt;
    std::string::size_type sz = pstr->size();

    const int f_arg = pos;
    const int s_arg = which >= 4 ? 4 == which ? 0 : pos + cnt : cnt;

#ifndef _RWSTD_NO_EXCEPTIONS

    const char* const expected = should_throw ? "out_of_range" : 0;
    const char*       caught   = 0;

    try {

#endif   // _RWSTD_NO_EXCEPTIONS

    switch (which) {
    case 1: {
            if (pos > 0 || cnt > 0 || cnt == -2)
                return;

            s_res = pstr->erase ();
            break;
        }
    case 2: {
            if (pos < 0 || cnt != -1)
                return;

            s_res = pstr->erase (pos);
            break;
        }
    case 3: {
            if (pos < 0 || cnt < 0)
                return;

            s_res = pstr->erase (pos, cnt);
            break;
        }
    case 4: {
            if (pos < 0 || cnt != -2 || should_throw)
                return;

            typename TestString::iterator it_first (pstr->begin() + pos);
            it_res = pstr->erase (it_first);
            break;
        }
    case 5: {
            if (pos < 0 || cnt < 0 || should_throw)
                return;

            typename TestString::iterator it_first (pstr->begin () + pos);
            typename TestString::iterator it_last (std::size_t (last) == 
                str_len ? pstr->end() : pstr->begin() + last);

            it_res = pstr->erase (it_first, it_last);
            break;
        }
    default:
        RW_ASSERT ("Test logic error");
        return;
    }

#define CALLFMAT                                        \
    "line %d. basic_string<%s, %s<%2$s>, %s<%2$s>>("    \
    "%{?}%{#*s}%{;}"                                    \
    ").erase (%{?}%{?}%d%{;}%{?}, %d%{;}%{;}"           \
    "%{?}begin + %td%{?}, begin + %td%{;}%{;}) "

#define CALLARGS                                                    \
    __LINE__, pfid->cname_, pfid->tname_, pfid->aname_,             \
    0 != str, int (str_len), str,                                   \
    4 > which, 2 == which || 3 == which, f_arg, 3 == which, s_arg,  \
    4 <= which, f_arg, 5 == which, s_arg

#ifndef _RWSTD_NO_EXCEPTIONS

    }
    catch (std::out_of_range) {
        caught = expected;
    }
    catch (...) {
        caught = "unknown exception";
    }

    rw_assert (caught == expected, 0, line,
              CALLFMAT "%{?}expected %s, caught %s"
               "%{:}unexpectedly caught %s%{;}",
               CALLARGS, 0 != expected, expected, caught, caught);

    if (should_throw || caught)
        return;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)
    _RWSTD_UNUSED (should_throw);
#endif   // _RWSTD_NO_EXCEPTIONS

    const typename TestString::size_type res_sz = pstr->size();
    const typename TestString::iterator  begin = pstr->begin();
    const typename TestString::iterator  end = pstr->end();

    // verify the returned value
    if (which < 4) {
        rw_assert (s_res == *pstr, 0, line,
                   CALLFMAT " != *this", CALLARGS);
    }
    else {
        bool success = begin <= it_res && it_res <= end;
        rw_assert (success, 0, line,
                   CALLFMAT "returned invalid iterator, "
                   "difference with begin is %td",
                   CALLARGS, it_res - begin);

        std::size_t idx 
            = 4 == which ? std::size_t (pos + 1) : std::size_t (last);

        if (idx == sz) {
            rw_assert (it_res == end, 0, line,
                       CALLFMAT "!= end()", CALLARGS);
        }
        else {
            success = TestString::traits_type::eq (*it_res, wstr[idx]);
            rw_assert (success, 0, line,
                       CALLFMAT "== %#c, expected %#c",
                       CALLARGS, *it_res, str[idx]);
        }   
    }

    // check the results
    std::size_t exp_len = pos >= 0 ? cnt == -1 ? pos : res_len : 0;
    bool success = exp_len == res_sz;
    rw_assert (success, 0, line,
               CALLFMAT ": size() == %zu, expected %zu",
               CALLARGS, res_sz, exp_len);

    if (0 == res_sz)
        return;

    success = !TestString::traits_type::compare (pstr->c_str(), pwres, res_sz);
    rw_assert (success, 0, line,
               CALLFMAT ": got %{#*S}, expected %{#s}",
               CALLARGS, int (sizeof (charT)), pstr, res);
}


/**************************************************************************/

static void
test_erase (MemFun      *pfid,
            int          line,
            int          which,
            const char  *str,
            std::size_t  str_len,
            int          pos,
            int          npos,
            const char  *res,
            std::size_t  res_len,
            bool         should_throw)
{
#undef TEST
#define TEST(charT, Traits)	                                      \
    test_erase (charT (), (Traits*)0, pfid, line, which,              \
                str, str_len, pos, npos, res, res_len, should_throw)     

    if (!rw_enabled (line)) {
        rw_note (0, 0, __LINE__, "test on line %d disabled", line);
        return;
    }

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
}

/**************************************************************************/

static void
test_erase (MemFun *pfid, int which)
{
    rw_info (0, 0, 0, "std::basic_string<%s, %s<%1$s>, %s<%1$s>>::"
             "erase (%{?}%{?}size_type pos%{;}%{?}, size_type n%{;}%{;}"
             "%{?}iterator first%{?}, iterator last%{;}%{;})",
             pfid->cname_, pfid->tname_, pfid->aname_, 4 > which, 
             2 == which || 3 == which, 3 == which, 4 <= which, which == 5);

#undef TEST
#define TEST(str, pos, cnt, res, should_throw)                              \
    test_erase (pfid, __LINE__, which, str, sizeof str - 1,                 \
                pos, cnt, res, sizeof res - 1, should_throw)

    //    +--------------------------------------- controlled sequence
    //    |               +----------------------- erase() pos argument
    //    |               |                    +-- erase() n   argument
    //    |               |                    |  +-- expected result sequence
    //    |               |                    |  |             +-- exception 
    //    |               |                    |  |             |   expected?
    //    V               V                    V  V             V
    TEST (0,              0,                   0, "",           false);
    TEST ("",             0,                   0, "",           false);

    TEST ("\0",          -1,                  -1, "",           false);
    TEST ("\0",           0,                   1, "",           false);
    TEST ("\0",           0,                  -2, "",           false);
    TEST ("s",            0,                  -2, "",           false);

    TEST ("abc",         -1,                  -1, "",           false);
    TEST ("abc",          0,                  -1, "",           false);

    TEST ("abc",          0,                  -2, "bc",         false);
    TEST ("abc",          1,                  -2, "ac",         false);
    TEST ("abc",          2,                  -2, "ab",         false);

    TEST ("abc",          0,                   1, "bc",         false);
    TEST ("abc",          1,                   1, "ac",         false);
    TEST ("abc",          2,                   1, "ab",         false);

    TEST ("abc",          0,                   2, "c",          false);
    TEST ("abc",          1,                   2, "a",          false);
    TEST ("abc",          0,                   3, "",           false);

    TEST ("t\0 s",        0,                   1, "\0 s",       false);
    TEST ("t\0 s",        1,                   1, "t s",        false);
    TEST ("t\0 s",        2,                   2, "t\0",        false);
    TEST ("t\0 s",        1,                   2, "ts",         false);
    TEST ("t\0 s",        0,                   2, " s",         false);
    TEST ("t\0 s",        0,                   4, "",           false);
    TEST ("t\0 s",       -1,                  -1, "",           false);

    TEST ("t\0 s",        0,                  -2, "\0 s",       false);
    TEST ("t\0 s",        1,                  -2, "t s",        false);
    TEST ("t\0 s",        2,                  -2, "t\0s",       false);

    TEST ("\0a\0b",       0,                   3, "b",          false);
    TEST ("\0a\0b",       1,                   1, "\0\0b",      false);
    TEST ("\0a\0b",       3,                   1, "\0a\0",      false);

    TEST ("\0a\0b",       0,                  -2, "a\0b",       false);
    TEST ("\0a\0b",       1,                  -2, "\0\0b",      false);
    TEST ("\0a\0b",       3,                  -2, "\0a\0",      false);

    TEST ("a\0\0\0b",     1,                   3, "ab",         false);
    TEST ("a\0\0\0b",     0,                   1, "\0\0\0b",    false);
    TEST ("a\0\0\0b",     2,                  -1, "a\0",        false);

    TEST ("a\0\0\0b",     1,                  -2, "a\0\0b",     false);
    TEST ("a\0\0\0b",     0,                  -2, "\0\0\0b",    false);

    TEST ("a\0b\0\0c",    0,                  -2, "\0b\0\0c",   false);
    TEST ("a\0b\0\0c",    1,                  -2, "ab\0\0c",    false);

    TEST ("a\0b\0\0c",   -1,                  -1, "",           false);
    TEST ("a\0b\0\0c",    0,                   2, "b\0\0c",     false);
    TEST ("a\0b\0\0c",    1,                   2, "a\0\0c",     false);

    TEST ("test string",  1,                   3, "t string",   false);
    TEST ("Test\0string", 0,                   2, "st\0string", false);
    TEST ("Test\0string", 1,                   4, "Tstring",    false);
    TEST ("Test\0string", 0,                   4, "\0string",   false);
    TEST ("Test\0string", 0,                   5, "string",     false);
    TEST ("Test\0string", 0,                  11, "",           false);
    TEST ("Test\0string", 1,                  -1, "T",          false);

    TEST ("Test\0string", 1,                  -2, "Tst\0string", false);
    TEST ("Test\0string", 4,                  -2, "Teststring", false);

#ifndef _RWSTD_NO_EXCEPTIONS

    TEST ("\0",           2,                   1, "\0",          true);
    TEST ("a",            2,                   1, "a",           true);
    TEST (long_string,    long_string_len + 1, 1, long_string,   true);

#endif   // _RWSTD_NO_EXCEPTIONS

#undef TEST
#define TEST(str, pos, cnt, res, res_len, should_throw)                     \
    test_erase (pfid, __LINE__, which, str, sizeof str - 1,                 \
                pos, cnt, res, res_len, should_throw)

    //    +--------------------------------------- controlled sequence
    //    |             +------------------------- erase() pos argument
    //    |             |    +-------------------- erase() n   argument
    //    |             |    |                    +-- expected result sequence
    //    |             |    |                    |     +-- result length
    //    |             |    |                    |     |  +-- exception 
    //    |             |    |                    |     |  |   expected?
    //    V             V    V                    V     V  V
    TEST (long_string, -1,  -1,                   "",   0, false);
    TEST (long_string,  0,  -1,                   "",   0, false);

    TEST (long_string,  0,   long_string_len - 1, "",   0, false);
    TEST (long_string,  0,   long_string_len - 2, "x",  1, false);
    TEST (long_string,  1,   long_string_len - 2, "x",  1, false);
    TEST (long_string,  1,   long_string_len - 3, "xx", 2, false);

    TEST (long_string,  0,  1, (long_string + 1), long_string_len - 2, false);
    TEST (long_string, long_string_len - 2, 1, (long_string + 1), 
          long_string_len - 2, false);

    TEST (long_string, 0,  -2, (long_string + 1), long_string_len - 2, false);
    TEST (long_string, long_string_len - 2,  -2, 
         (long_string + 1), long_string_len - 2, false);
}

/**************************************************************************/

static int rw_opt_no_char_traits;   // for --no-char_traits
static int rw_opt_no_user_traits;   // for --no-user_traits
static int rw_opt_no_user_chars;    // for --no-user_chars

/**************************************************************************/

static void
run_test (MemFun *pfid)
{
    if (pfid->tname_ && rw_opt_no_user_traits) {
        rw_note (1 < rw_opt_no_user_traits++, 0, 0,
                 "user defined traits test disabled");
    }
    else if (!pfid->tname_ && rw_opt_no_char_traits) {
        rw_note (1 < rw_opt_no_char_traits++, 0, 0,
                 "char_traits test disabled");
    }
    else {
        // exercise all erase overloads
        for (int i = 1; i <= 5; i++)
            test_erase (pfid, i);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
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
        rw_note (0, 0, 0, "string::erase char tests disabled");

    if (rw_enabled ("wchar_t")) {

        MemFun fid (MemFun::WChar, "wchar_t", MemFun::DefaultTraits, 0);

        fid.tname_ = "char_traits";

        run_test (&fid);

        fid.tid_   = MemFun::UserTraits;
        fid.tname_ = "UserTraits";

        run_test (&fid);
    }
    else
        rw_note (0, 0, 0, "string::erase wchar tests disabled");

    if (rw_opt_no_user_chars) {
        rw_note (0, 0, 0, "user defined chars test disabled");
    }
    else {
        MemFun fid (MemFun::UChar, "UserChar", MemFun::UserTraits, 0);
        fid.tname_ = "UserTraits";
        run_test (&fid);
    }

    return 0;

}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.erase",
                    0 /* no comment */,
                    run_test,
                    "|-no-char_traits# "
                    "|-no-user_traits# ",
                    "|-no-user_chars",
                    &rw_opt_no_char_traits,
                    &rw_opt_no_user_traits,
                    &rw_opt_no_user_chars);
}
