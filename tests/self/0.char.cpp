/************************************************************************
 *
 * 0.char.cpp - test exercising the UserTraits helper class template
 *              and the rw_widen() set of overloaded functions
 *
 * $Id$
 *
 ************************************************************************
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

#include <rw_char.h>
#include <rw_printf.h>
#include <driver.h>

#include <stdlib.h>
#include <string.h>    // for memset, size_t

/***********************************************************************/

static void
test_user_traits_char ()
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "template class UserTraits<char>");

    rw_warn (0, 0, 0, "UserTraits<char> not exercised");
}

/***********************************************************************/

static void
test_user_traits_wchar_t ()
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "template class UserTraits<wchar_t>");

    rw_warn (0, 0, 0, "UserTraits<wchar_t> not exercised");
}

/***********************************************************************/

static void
test_user_traits_user_char ()
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "template class UserTraits<UserChar>");

    const UserChar src [] = {
        { 1, 2 }, { 0, 0 }, { 3, 4 }, { 5, 6 }, { 7, 8 }, { 0, 0 }
    };

    const size_t nsrc = sizeof src / sizeof *src;

    UserChar dst [nsrc];

    typedef UserTraits<UserChar>         Traits;
    typedef UserTraits<UserChar>::MemFun MemFun;

    //////////////////////////////////////////////////////////////////
    Traits::n_calls_ [MemFun::assign] = 0;

    Traits::assign (dst [0], src [0]);
    rw_assert (dst [0].f == src [0].f && dst [0].c == src [0].c, 0, __LINE__,
               "UserTraits::assign(char_type&, const char_type&)");

    rw_assert (1 == Traits::n_calls_ [MemFun::assign], 0, __LINE__,
               "UserTraits::assign(char_type&, const char_type&)");

    //////////////////////////////////////////////////////////////////
    Traits::n_calls_ [MemFun::eq] = 0;

    bool eq = Traits::eq (dst [0], src [0]);
    rw_assert (eq, 0, __LINE__,
               "UserTraits::eq(const char_type&, const char_type&)");

    rw_assert (1 == Traits::n_calls_ [MemFun::eq], 0, __LINE__,
               "UserTraits::eq(const char_type&, const char_type&)");

    //////////////////////////////////////////////////////////////////
    Traits::n_calls_ [MemFun::lt] = 0;

    bool lt = Traits::lt (dst [0], src [0]);
    rw_assert (!lt, 0, __LINE__,
               "UserTraits::lt(const char_type&, const char_type&)");

    rw_assert (1 == Traits::n_calls_ [MemFun::lt], 0, __LINE__,
               "UserTraits::lt(const char_type&, const char_type&)");

    lt = Traits::lt (src [0], src [1]);
    rw_assert (eq, 0, __LINE__,
               "UserTraits::lt(const char_type&, const char_type&)");

    rw_assert (2 == Traits::n_calls_ [MemFun::lt], 0, __LINE__,
               "UserTraits::lt(const char_type&, const char_type&)");

    //////////////////////////////////////////////////////////////////
    Traits::n_calls_ [MemFun::compare] = 0;

    int compare = Traits::compare (src, src, nsrc);
    rw_assert (0 == compare, 0, __LINE__,
               "UserTraits::compare(const char_type*, const char_type*) "
               "== 0, got %d", compare);

    rw_assert (1 == Traits::n_calls_ [MemFun::compare], 0, __LINE__,
               "UserTraits::compare(const char_type*, const char_type*)");

    compare = Traits::compare (src, src + 1, nsrc - 1);
    rw_assert (+1 == compare, 0, __LINE__,
               "UserTraits::compare(const char_type*, const char_type*) "
               "== +1, got %d", compare);

    rw_assert (2 == Traits::n_calls_ [MemFun::compare], 0, __LINE__,
               "UserTraits::compare(const char_type*, const char_type*)");

    compare = Traits::compare (src + 1, src, nsrc - 1);
    rw_assert (-1 == compare, 0, __LINE__,
               "UserTraits::compare(const char_type*, const char_type*) "
               "== -1, got %d", compare);

    rw_assert (3 == Traits::n_calls_ [MemFun::compare], 0, __LINE__,
               "UserTraits::compare(const char_type*, const char_type*)");

    //////////////////////////////////////////////////////////////////
    Traits::n_calls_ [MemFun::length] = 0;

    size_t length = Traits::length (src);
    rw_assert (1 == length, 0, __LINE__,
               "UserTraits::length(const char_type*)");

    rw_assert (1 == Traits::n_calls_ [MemFun::length], 0, __LINE__,
               "UserTraits::length(const char_type*)");

    length = Traits::length (src + 1);
    rw_assert (0 == length, 0, __LINE__,
               "UserTraits::length(const char_type*)");

    rw_assert (2 == Traits::n_calls_ [MemFun::length], 0, __LINE__,
               "UserTraits::length(const char_type*)");

    length = Traits::length (src + 2);
    rw_assert (nsrc - 3 == length, 0, __LINE__,
               "UserTraits::length(const char_type*) == %zu, got %zu",
               nsrc - 3, length);

    rw_assert (3 == Traits::n_calls_ [MemFun::length], 0, __LINE__,
               "UserTraits::length(const char_type*)");

    //////////////////////////////////////////////////////////////////
    Traits::n_calls_ [MemFun::find] = 0;

    const UserChar *find = Traits::find (src, nsrc, src [0]);
    rw_assert (src == find, 0, __LINE__,
               "UserTraits::find(const char_type*, size_t, const char_type&)");

    rw_assert (1 == Traits::n_calls_ [MemFun::find], 0, __LINE__,
               "UserTraits::find(const char_type*, size_t, const char_type&)");

    find = Traits::find (src, nsrc, src [1]);
    rw_assert (src + 1 == find, 0, __LINE__,
               "UserTraits::find(const char_type*, size_t, const char_type&)");

    rw_assert (2 == Traits::n_calls_ [MemFun::find], 0, __LINE__,
               "UserTraits::find(const char_type*, size_t, const char_type&)");

    find = Traits::find (src, nsrc, src [2]);
    rw_assert (src + 2 == find, 0, __LINE__,
               "UserTraits::find(const char_type*, size_t, const char_type&)");

    rw_assert (3 == Traits::n_calls_ [MemFun::find], 0, __LINE__,
               "UserTraits::find(const char_type*, size_t, const char_type&)");

    find = Traits::find (src + 1, nsrc - 1, src [0]);
    rw_assert (0 == find, 0, __LINE__,
               "UserTraits::find(const char_type*, size_t, const char_type&)");

    rw_assert (4 == Traits::n_calls_ [MemFun::find], 0, __LINE__,
               "UserTraits::find(const char_type*, size_t, const char_type&)");

    //////////////////////////////////////////////////////////////////
    Traits::n_calls_ [MemFun::copy] = 0;

    const UserChar uc = { -1, 1 };

    memset (dst, 0, sizeof dst);
    dst [0] = uc;

    const UserChar *copy = Traits::copy (dst, src, 0);
    rw_assert (dst == copy, 0, __LINE__,
               "UserTraits::copy(char_type*, const char_type*, size_t)");

    rw_assert (Traits::eq (dst [0], uc), 0, __LINE__,
               "UserTraits::copy(char_type*, const char_type*, size_t)");

    rw_assert (1 == Traits::n_calls_ [MemFun::copy], 0, __LINE__,
               "UserTraits::copy(char_type*, const char_type*, size_t)");

    memset (dst, 0, sizeof dst);
    dst [1] = uc;

    copy = Traits::copy (dst, src, 1);

    rw_assert (dst == copy, 0, __LINE__,
               "UserTraits::copy(char_type*, const char_type*, size_t)");

    rw_assert (Traits::eq (dst [0], src [0]) && Traits::eq (dst [1], uc),
               0, __LINE__,
               "UserTraits::copy(char_type*, const char_type*, size_t)");

    rw_assert (2 == Traits::n_calls_ [MemFun::copy], 0, __LINE__,
               "UserTraits::copy(char_type*, const char_type*, size_t)");

    //////////////////////////////////////////////////////////////////
    Traits::n_calls_ [MemFun::move] = 0;

    memset (dst, 0, sizeof dst);
    dst [0] = uc;

    const UserChar *move = Traits::move (dst, src, 0);
    rw_assert (dst == move, 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (Traits::eq (dst [0], uc), 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (1 == Traits::n_calls_ [MemFun::move], 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    memset (dst, 0, sizeof dst);
    dst [1] = uc;

    move = Traits::move (dst, src, 1);

    rw_assert (dst == move, 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (Traits::eq (dst [0], src [0]) && Traits::eq (dst [1], uc),
               0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (2 == Traits::n_calls_ [MemFun::move], 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    
    memcpy (dst, src, sizeof dst);
    move = Traits::move (dst, dst + 1, 1);

    rw_assert (dst == move, 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (   Traits::eq (dst [0], src [1])
               && Traits::eq (dst [1], src [1]),
               0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (3 == Traits::n_calls_ [MemFun::move], 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    memcpy (dst, src, sizeof dst);
    move = Traits::move (dst, dst + 1, 2);

    rw_assert (dst == move, 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (   Traits::eq (dst [0], src [1])
               && Traits::eq (dst [1], src [2])
               && Traits::eq (dst [2], src [2]),
               0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (4 == Traits::n_calls_ [MemFun::move], 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    memcpy (dst, src, sizeof dst);
    move = Traits::move (dst + 1, dst, 1);

    rw_assert (dst + 1 == move, 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (   Traits::eq (dst [0], src [0])
               && Traits::eq (dst [1], src [0])
               && Traits::eq (dst [2], src [2]),
               0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (5 == Traits::n_calls_ [MemFun::move], 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    memcpy (dst, src, sizeof dst);
    move = Traits::move (dst + 1, dst, 2);

    rw_assert (dst + 1 == move, 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (   Traits::eq (dst [0], src [0])
               && Traits::eq (dst [1], src [0])
               && Traits::eq (dst [2], src [1])
               && Traits::eq (dst [3], src [3]),
               0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    rw_assert (6 == Traits::n_calls_ [MemFun::move], 0, __LINE__,
               "UserTraits::move(char_type*, const char_type*, size_t)");

    //////////////////////////////////////////////////////////////////
    Traits::n_calls_ [MemFun::assign] = 0;

    memset (dst, 0, sizeof dst);
    dst [0] = uc;

    const UserChar *assign = Traits::assign (dst, 0, src [0]);

    rw_assert (dst == assign, 0, __LINE__,
               "UserTraits::assign(char_type*, size_t, const char_type&)");

    rw_assert (Traits::eq (dst [0], uc), 0, __LINE__,
               "UserTraits::assign(char_type*, size_t, const char_type&)");

    rw_assert (1 == Traits::n_calls_ [MemFun::assign2], 0, __LINE__,
               "UserTraits::assign(char_type*, size_t, const char_type&)");

    memset (dst, 0, sizeof dst);
    dst [1] = uc;

    assign = Traits::assign (dst, 1, src [0]);
    rw_assert (dst == assign, 0, __LINE__,
               "UserTraits::assign(char_type*, size_t, const char_type&)");

    rw_assert (Traits::eq (dst [0], src [0]) && Traits::eq (dst [1], uc),
               0, __LINE__,
               "UserTraits::assign(char_type*, size_t, const char_type&)");

    rw_assert (2 == Traits::n_calls_ [MemFun::assign2], 0, __LINE__,
               "UserTraits::assign(char_type*, size_t, const char_type&)");

    memset (dst, 0, sizeof dst);
    dst [2] = uc;

    assign = Traits::assign (dst, 2, src [0]);
    rw_assert (dst == assign, 0, __LINE__,
               "UserTraits::assign(char_type*, size_t, const char_type&)");

    rw_assert (   Traits::eq (dst [0], src [0])
               && Traits::eq (dst [1], src [0])
               && Traits::eq (dst [2], uc),
               0, __LINE__,
               "UserTraits::assign(char_type*, size_t, const char_type&)");

    rw_assert (3 == Traits::n_calls_ [MemFun::assign2], 0, __LINE__,
               "UserTraits::assign(char_type*, size_t, const char_type&)");

    //////////////////////////////////////////////////////////////////
    rw_warn (0, 0, 0, "UserTraits<UserChar>::not_eof() not exercised");
    rw_warn (0, 0, 0, "UserTraits<UserChar>::eof() not exercised");
    rw_warn (0, 0, 0, "UserTraits<UserChar>::to_char_type() not exercised");
    rw_warn (0, 0, 0, "UserTraits<UserChar>::to_int_type() not exercised");
    rw_warn (0, 0, 0, "UserTraits<UserChar>::eq_int_type() not exercised");
}

/***********************************************************************/

static void
test_rw_widen ()
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_widen(char*, const char*, size_t)");

    static const char   src []  = "abcdefgh";
    const size_t nsrc = sizeof src - 1;
    static const char   null [nsrc + 1] = "";
    char                cdst [nsrc + 1];

    for (size_t i = 0; i != nsrc + 1; ++i) {

        memset (cdst, 0, sizeof cdst);

        const char* const ret = i < nsrc ?
            rw_widen (cdst, src, i) : rw_widen (cdst, src);

        rw_assert (cdst == ret, 0, __LINE__,
                   "rw_widen(char*, %{#s}%{?}, %zu%{;})",
                   src, i < nsrc);

        rw_assert (0 == memcmp (cdst, src, i) && '\0' == cdst [i],
                   0, __LINE__,
                   "rw_widen(char*, %{#s}%{?}, %zu%{;}) == %{#*s}, got %{#*s}",
                   src, i < nsrc, i, int (i), src, int (i + 1), cdst);
    }

    memset (cdst, '@', sizeof cdst);
    rw_widen (cdst, 0, sizeof cdst);

    rw_assert (0 == memcmp (cdst, null, sizeof cdst), 0, __LINE__,
               "rw_widen(char*, %{#s}, %zu) == %{#*s}, got %{#*s}",
               0, sizeof cdst, int (sizeof cdst), null,
               int (sizeof cdst), cdst);

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_widen(wchar_t*, const char*, size_t)");

#ifndef _RWSTD_NO_WCHAR_T

    static const wchar_t wsrc [] = L"abcdefgh";
    static const wchar_t wnull [nsrc + 1] = L"";
    wchar_t              wdst [nsrc + 1];

    for (size_t i = 0; i != nsrc + 1; ++i) {

        memset (wdst, 0, sizeof wdst);

        const wchar_t* const ret = i < nsrc ?
            rw_widen (wdst, src, i) : rw_widen (wdst, src);

        rw_assert (wdst == ret, 0, __LINE__,
                   "rw_widen(wchar_t*, %{#s}%{?}, %zu%{;})",
                   src, i < nsrc, i);

        rw_assert (   0 == memcmp (wdst, wsrc, i * sizeof *wdst)
                   && '\0' == wdst [i],
                   0, __LINE__,
                   "rw_widen(wchar_t*, %{#s}%{?}, %zu%{;}) == L%{#*ls}, "
                   "got L%{#*ls}",
                   src, i < nsrc, i, int (i), wsrc, int (i + 1), wdst);
    }

    memset (wdst, '@', sizeof wdst);
    rw_widen (wdst, 0, sizeof wdst / sizeof *wdst);

    rw_assert (0 == memcmp (wdst, wnull, sizeof wdst), 0, __LINE__,
               "rw_widen(wchar_t*, %{#s}, %zu) == %{#*s}, got L%{#*ls}",
               0, sizeof wdst, int (sizeof wdst / sizeof *wdst), wnull,
               int (sizeof wdst / sizeof *wdst), wdst);

#else   // if defined (_RWSTD_NO_WCHAR_T)

    rw_note (0, 0, 0, "_RWSTD_NO_WCHAR_T #defined, wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_widen(UserChar*, const char*, size_t)");

    static const UserChar usrc [] = {
        { 0, 'a' }, { 0, 'b' }, { 0, 'c' }, { 0, 'd' },
        { 0, 'e' }, { 0, 'f' }, { 0, 'g' }, { 0, 'h' },
        { 0, '\0' }
    };
    static const UserChar unull [] = {
        { 0, '\0' }, { 0, '\0' }, { 0, '\0' }, { 0, '\0' },
        { 0, '\0' }, { 0, '\0' }, { 0, '\0' }, { 0, '\0' },
        { 0, '\0' }
    };

    UserChar udst [nsrc + 1];

    for (size_t i = 0; i != nsrc + 1; ++i) {

        memset (udst, 0, sizeof udst);

        const UserChar* const ret = i < nsrc ?
            rw_widen (udst, src, i) : rw_widen (udst, src);

        rw_assert (udst == ret, 0, __LINE__,
                   "rw_widen(UserChar*, %{#s}%{?}, %zu%{;})",
                   src, i < nsrc, i);

        rw_assert (   0 == memcmp (udst, usrc, i * sizeof *udst)
                   && 0 == udst [i].f && 0 == udst [i].c,
                   0, __LINE__,
                   "rw_widen(UserChar*, %{#s}%{?}, %zu%{;})",
                   src, i < nsrc, i);
    }

    memset (udst, 1, sizeof udst);
    rw_widen (udst, 0, sizeof udst / sizeof *udst);

    rw_assert (0 == memcmp (udst, unull, sizeof udst), 0, __LINE__,
               "rw_widen(UserChar*, %{#s}, %zu)",
               0, sizeof udst, int (sizeof udst), wnull);
}

/***********************************************************************/

static void
test_rw_expand ()
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_expand(char*, const char*, size_t, size_t*)");

#undef TEST
#define TEST(str, expstr)                                               \
    do {                                                                \
        const size_t str_size = sizeof str - 1;                         \
        const size_t exp_size = sizeof expstr - 1;                      \
        size_t len;                                                     \
        char* const res = rw_expand ((char*)0, str, str_size, &len);    \
        rw_assert (   0 == memcmp (res, expstr, exp_size)               \
                   && exp_size == len,                                  \
                   0, __LINE__,                                         \
                   "rw_expand((char*)0, %{#*s}, %zu, ...) == "          \
                   "%{#*s} (length %zu), got %{#*s} (length %zu)",      \
                   int (str_size), str, str_size,                       \
                   int (exp_size), expstr, exp_size,                    \
                   int (len), res, len);                                \
    } while (0)

    TEST ("",          "");
    TEST ("\0",        "\0");
    TEST ("a",         "a");
    TEST ("a\0",       "a\0");
    TEST ("ab",        "ab");
    TEST ("abc",       "abc");
    TEST ("a\0b",      "a\0b");
    TEST ("a\0b\0c\0", "a\0b\0c\0");

    TEST ("\0@0",      "");
    TEST ("a@0",       "");
    TEST ("\0@1",      "\0");
    TEST ("a@1",       "a");
    TEST ("a@2",       "aa");
    TEST ("a@2\0",     "aa\0");
    TEST ("a@3",       "aaa");
    TEST ("a@1b",      "ab");
    TEST ("a@2b",      "aab");
    TEST ("a@3b",      "aaab");
    TEST ("a@3bc",     "aaabc");
    TEST ("a@3b@1c",   "aaabc");
    TEST ("a@3b@2c",   "aaabbc");
    TEST ("a@3b@2c@1", "aaabbc");
    TEST ("a@3b@3c@2", "aaabbbcc");

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_expand(wchar_t*, const char*, size_t, size_t*)");

#ifndef _RWSTD_NO_WCHAR_T

#  undef TEST
#define TEST(str, expstr)                                                  \
    do {                                                                   \
        const size_t str_size = sizeof str - 1;                            \
        const size_t exp_size = sizeof expstr / sizeof *expstr - 1;        \
        size_t len;                                                        \
        wchar_t* const res = rw_expand ((wchar_t*)0, str, str_size, &len); \
        rw_assert (   0 == memcmp (res, expstr, exp_size)                  \
                   && exp_size == len,                                     \
                   0, __LINE__,                                            \
                   "rw_expand((wchar_t*)0, %{#*s}, %zu, ...) == "          \
                   "%{#*ls} (length %zu), got %{#*ls} (length %zu)",       \
                   int (str_size), str, str_size,                          \
                   int (exp_size), expstr, exp_size,                       \
                   int (len), res, len);                                   \
    } while (0)

    TEST ("",          L"");
    TEST ("\0",        L"\0");
    TEST ("a",         L"a");
    TEST ("a\0",       L"a\0");
    TEST ("ab",        L"ab");
    TEST ("abc",       L"abc");
    TEST ("a\0b",      L"a\0b");
    TEST ("a\0b\0c\0", L"a\0b\0c\0");

    TEST ("\0@0",      L"");
    TEST ("a@0",       L"");
    TEST ("\0@1",      L"\0");
    TEST ("a@1",       L"a");
    TEST ("a@2",       L"aa");
    TEST ("a@2\0",     L"aa\0");
    TEST ("a@3",       L"aaa");
    TEST ("a@1b",      L"ab");
    TEST ("a@2b",      L"aab");
    TEST ("a@3b",      L"aaab");
    TEST ("a@3bc",     L"aaabc");
    TEST ("a@3b@1c",   L"aaabc");
    TEST ("a@3b@2c",   L"aaabbc");
    TEST ("a@3b@2c@1", L"aaabbc");
    TEST ("a@3b@3c@2", L"aaabbbcc");

#else   // if defined (_RWSTD_NO_WCHAR_T)

    rw_note (0, 0, 0, "_RWSTD_NO_WCHAR_T #defined, wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_expand(UserChar*, const char*, size_t, size_t*)");

    rw_warn (0, 0, 0, "rw_expand(UserChar*, const char*, size_t, size_t*) "
             "not exercised");
}


/***********************************************************************/

static void
test_rw_narrow ()
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_narrow(char*, const char*, size_t)");

    static const char   src []  = "abcdefgh";
    const size_t nsrc = sizeof src - 1;
    static const char   null [nsrc + 1] = "";
    char                cdst [nsrc + 1];

    for (size_t i = 0; i != nsrc + 1; ++i) {

        memset (cdst, 0, sizeof cdst);

        const char* const ret = i < nsrc ?
            rw_narrow (cdst, src, i) : rw_narrow (cdst, src);

        rw_assert (cdst == ret, 0, __LINE__,
                   "rw_narrow(char*, %{#s}%{?}, %zu%{;})",
                   src, i < nsrc);

        rw_assert (0 == memcmp (cdst, src, i) && '\0' == cdst [i],
                   0, __LINE__,
                   "rw_narrow(char*, %{#s}%{?}, %zu%{;}) == %{#*s}, got %{#*s}",
                   src, i < nsrc, i, int (i), src, int (i + 1), cdst);
    }

    memset (cdst, '@', sizeof cdst);
    rw_narrow (cdst, (const char*)0, sizeof cdst);

    rw_assert (0 == memcmp (cdst, null, sizeof cdst), 0, __LINE__,
               "rw_narrow(char*, %{#s}, %zu) == %{#*s}, got %{#*s}",
               0, sizeof cdst, int (sizeof cdst), null,
               int (sizeof cdst), cdst);
               
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_narrow(wchar_t*, const wchar_t*, size_t)");

#ifndef _RWSTD_NO_WCHAR_T

    static const wchar_t wsrc [] = L"abcdefgh";

    for (size_t i = 0; i != nsrc + 1; ++i) {

        memset (cdst, 0, sizeof cdst);

        const char* const ret = i < nsrc ?
            rw_narrow (cdst, wsrc, i) : rw_narrow (cdst, wsrc);

        rw_assert (cdst == ret, 0, __LINE__,
                   "rw_narrow(char*, %{#s}%{?}, %zu%{;})",
                   src, i < nsrc, i);

        rw_assert (0 == memcmp (cdst, src, i) && '\0' == cdst [i],
                   0, __LINE__,
                   "rw_narrow(char*, L%{#*ls}%{?}, %zu%{;}) == %{#*s}, "
                   "got %{#*s}",
                   int (i), src, i < nsrc, i, int (i), wsrc,
                   int (i + 1), cdst);
    }

    memset (cdst, '@', sizeof cdst);
    rw_narrow (cdst, (const wchar_t*)0, sizeof cdst);

    rw_assert (0 == memcmp (cdst, null, nsrc + 1), 0, __LINE__,
               "rw_narrow(char*, %{#s}, %zu) == %{#*s}, got %{#*s}",
               0, sizeof cdst, int (sizeof null), null,
               int (sizeof cdst), cdst);

#else   // if defined (_RWSTD_NO_WCHAR_T)

    rw_note (0, 0, 0, "_RWSTD_NO_WCHAR_T #defined, wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_narrow(char*, const UserChar*, size_t)");

    static const UserChar usrc [] = {
        { 0, 'a' }, { 0, 'b' }, { 0, 'c' }, { 0, 'd' },
        { 0, 'e' }, { 0, 'f' }, { 0, 'g' }, { 0, 'h' },
        { 0, '\0' }
    };

    for (size_t i = 0; i != nsrc + 1; ++i) {

        memset (cdst, 0, sizeof cdst);

        const char* const ret = i < nsrc ?
            rw_narrow (cdst, usrc, i) : rw_narrow (cdst, usrc);

        rw_assert (cdst == ret, 0, __LINE__,
                   "rw_narrow(char*, %{#s}%{?}, %zu%{;})",
                   src, i < nsrc, i);

        rw_assert (0 == memcmp (cdst, src, i) && 0 == cdst [i],
                   0, __LINE__,
                   "rw_narrow(char*, %{#s}%{?}, %zu%{;})",
                   src, i < nsrc, i);
    }

    memset (cdst, '@', sizeof cdst);
    rw_narrow (cdst, (const UserChar*)0, sizeof cdst);

    rw_assert (0 == memcmp (cdst, null, nsrc + 1), 0, __LINE__,
               "rw_narrow(char*, %{#s}, %zu)",
               0, sizeof cdst);
}

/***********************************************************************/

static size_t
length (const char *s)
{
    return s ? strlen (s) : 0;
}


static size_t
length (const wchar_t *ws)
{
    size_t len = 0;
    if (ws)
        for (len = 0; ws [len]; ++len);
    return len;
}


static size_t
length (const UserChar *us)
{
    size_t len = 0;
    if (us)
        for (len = 0; us [len].f || us [len].c; ++len);
    return len;
}


static const UserChar*
make_user_string (const char *s, size_t len)
{
    static UserChar usrbuf [32];
    memset (usrbuf, 0, sizeof usrbuf);

    if (s) {
        for (size_t i = 0; i != len; ++i) {

            typedef unsigned char UChar;

            usrbuf [i].f = 0;
            usrbuf [i].c = UChar (s [i]);
        }
        return usrbuf;
    }

    return 0;
}


static void
test_rw_match ()
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_match(char*, const char*, size_t)");

    const size_t size_max = _RWSTD_SIZE_MAX;
    size_t result;

#define LEN(T, s, len) \
  int (size_max == size_t (len) ? length ((const T*)s) : size_t (len))

#undef TEST
#define TEST(s1, s2, len, expect)                                       \
  result = rw_match ((const char*)s1, (const char*)s2, size_t (len));   \
  rw_assert (expect == result,                                          \
             0, __LINE__,                                               \
             "rw_match(%{#*s}, %{#*s}, %zu) == %zu, got %zu",           \
             LEN (char, s1, len), s1, LEN (char, s2, len),              \
             s2, len, expect, result)

    TEST (0,      0,        -1, 0);
    TEST ("",     0,        -1, 0);
    TEST (0,      "",       -1, 0);
    TEST ("",     "",       -1, 0);

    // when invoked with NULL as the first string returns
    // the length of the second string (if non-NULL)
    TEST (0,      "a",      -1, 1);
    TEST (0,      "ab",     -1, 2);
    TEST (0,      "abc",    -1, 3);

    // same as above but with the arguments reversed
    TEST ("a",    0,        -1, 1);
    TEST ("ab",   0,        -1, 2);
    TEST ("abc",  0,        -1, 3);

    TEST ("",     "a",      -1, 0);
    TEST ("a",    "",       -1, 0);
    TEST ("a",    "a",      -1, 1);

    TEST ("a\0bc", "a\0bd", -1, 1);
    TEST ("a\0bc", "a\0bd",  0, 0);
    TEST ("a\0bc", "a\0bd",  1, 1);
    TEST ("a\0bc", "a\0bd",  2, 2);
    TEST ("a\0bc", "a\0bd",  3, 3);
    TEST ("a\0bc", "a\0bd",  4, 3);

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_match(char*, const wchar_t*, size_t)");

#ifndef _RWSTD_NO_WCHAR_T

#undef TEST
#define TEST(s1, s2, len, expect)                                            \
  result = rw_match ((const char*)s1, (const wchar_t*)s2, size_t (len));     \
  rw_assert (expect == result,                                               \
             0, __LINE__,                                                    \
             "rw_match(%{#*s}, L%{#*ls}, %zu) == %zu, got %zu",              \
             LEN (char, s1, len), s1, LEN (wchar_t, s2, len), s2,            \
             len, expect, result)

    TEST (0,       0,        -1, 0);
    TEST ("",      0,        -1, 0);
    TEST (0,       L"",      -1, 0);
    TEST ("",      L"",      -1, 0);

    TEST (0,       L"a",     -1, 1);
    TEST (0,       L"ab",    -1, 2);
    TEST (0,       L"abc",   -1, 3);

    TEST ("a",     0,        -1, 1);
    TEST ("ab",    0,        -1, 2);
    TEST ("abc",   0,        -1, 3);

    TEST ("",      L"a",     -1, 0);
    TEST ("a",     L"",      -1, 0);
    TEST ("a",     L"a",     -1, 1);

    TEST ("a\0bc", L"a\0bd", -1, 1);
    TEST ("a\0bc", L"a\0bd",  0, 0);
    TEST ("a\0bc", L"a\0bd",  1, 1);
    TEST ("a\0bc", L"a\0bd",  2, 2);
    TEST ("a\0bc", L"a\0bd",  3, 3);
    TEST ("a\0bc", L"a\0bd",  4, 3);

#else   // if defined (_RWSTD_NO_WCHAR_T)

    rw_note (0, 0, 0, "_RWSTD_NO_WCHAR_T #defined, wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "rw_match(char*, const UserChar*, size_t)");

#undef TEST
#define TEST(s1, s2, len, expect)                                            \
  result = rw_match ((const char*)s1,                                        \
                     make_user_string (s2, sizeof (s2)), size_t (len));      \
  rw_assert (expect == result,                                               \
             0, __LINE__,                                                    \
             "rw_match(%{#*s}, %{#*s}, %zu) == %zu, got %zu",                \
             LEN (char, s1, len), s1, LEN (char, s2, len), s2,               \
             expect, result)

    TEST (0,       0,       -1, 0);
    TEST ("",      0,       -1, 0);
    TEST (0,       "",      -1, 0);
    TEST ("",      "",      -1, 0);

    TEST (0,       "a",     -1, 1);
    TEST (0,       "ab",    -1, 2);
    TEST (0,       "abc",   -1, 3);

    TEST ("a",     0,       -1, 1);
    TEST ("ab",    0,       -1, 2);
    TEST ("abc",   0,       -1, 3);

    TEST ("",      "a",     -1, 0);
    TEST ("a",     "",      -1, 0);
    TEST ("a",     "a",     -1, 1);

    TEST ("a\0bc", "a\0bd", -1, 1);
    TEST ("a\0bc", "a\0bd",  0, 0);
    TEST ("a\0bc", "a\0bd",  1, 1);
    TEST ("a\0bc", "a\0bd",  2, 2);
    TEST ("a\0bc", "a\0bd",  3, 3);
    TEST ("a\0bc", "a\0bd",  4, 3);
}

/***********************************************************************/

static void
test_formatting ()
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "\"%s\": formatting directive", "%{/Gs}");

    const int wchsize = int (sizeof (wchar_t));
    const int usrsize = int (sizeof (UserChar));

    char *str;

#undef TEST
#define TEST(fmt, arg0, arg1, arg2, expect_str)         \
    str = rw_sprintfa (fmt, arg0, arg1, arg2);          \
    rw_assert (str && 0 == strcmp (str, expect_str),    \
               0, __LINE__,                             \
               "rw_printf(%#s, ...) == %#s; got %#s",   \
               fmt, expect_str, str);                   \
    free (str)

    TEST (">%{/Gs}<", (char*)0, 0, 0, ">(null)<");
    TEST (">%{/Gs}<", "",       0, 0, ">\"\"<");
    TEST (">%{/Gs}<", "a",      0, 0, ">\"a\"<");
    TEST (">%{/Gs}<", "ab",     0, 0, ">\"ab\"<");
    TEST (">%{/Gs}<", "abc",    0, 0, ">\"abc\"<");
    TEST (">%{/Gs}<", "x\0z",   0, 0, ">\"x\"<");

    TEST (">%{/*Gs}<",   1, "abc",  0,     ">\"abc\"<");
    TEST (">%{/*.*Gs}<", 1, 2,      "abc", ">\"ab\"<");
    TEST (">%{/.*Gs}<",  2, "abc",  0,     ">\"ab\"<");
    TEST (">%{/.*Gs}<",  3, "x\0z", 0,     ">\"x\\0z\"<");

    TEST (">%{/*Gs}<",   wchsize, (wchar_t*)0, 0,      ">(null)<");
    TEST (">%{/*Gs}<",   wchsize, L"",         0,      ">\"\"<");
    TEST (">%{/*Gs}<",   wchsize, L"a",        0,      ">\"a\"<");
    TEST (">%{/*Gs}<",   wchsize, L"ab",       0,      ">\"ab\"<");
    TEST (">%{/*Gs}<",   wchsize, L"abc",      0,      ">\"abc\"<");
    TEST (">%{/*Gs}<",   wchsize, L"x\0z",     0,      ">\"x\"<");
    TEST (">%{/*.*Gs}<", wchsize, 2,           L"abc", ">\"ab\"<");
    TEST (">%{/*.*Gs}<", wchsize, 3,           L"x\0z",">\"x\\0z\"<");

#define US(s)   make_user_string (s, sizeof (s))

    TEST (">%{/*Gs}<",   usrsize, (UserChar*)0, 0,           ">(null)<");
    TEST (">%{/*Gs}<",   usrsize, US (""),      0,           ">\"\"<");
    TEST (">%{/*Gs}<",   usrsize, US ("a"),     0,           ">\"a\"<");
    TEST (">%{/*Gs}<",   usrsize, US ("ab"),    0,           ">\"ab\"<");
    TEST (">%{/*Gs}<",   usrsize, US ("abc"),   0,           ">\"abc\"<");
    TEST (">%{/*Gs}<",   usrsize, US ("x\0z"),  0,           ">\"x\"<");
    TEST (">%{/*.*Gs}<", usrsize, 2,            US ("abc"),  ">\"ab\"<");
    TEST (">%{/*.*Gs}<", usrsize, 3,            US ("x\0z"), ">\"x\\0z\"<");
}

/***********************************************************************/

static int no_user_traits;
static int no_user_traits_char;
static int no_user_traits_wchar_t;
static int no_user_traits_user_char;
static int no_rw_widen;
static int no_rw_expand;
static int no_rw_narrow;
static int no_rw_match;
static int no_formatting;


static int
run_test (int, char*[])
{
    if (no_user_traits) {
        rw_note (0, 0, 0, "UserTraits tests disabled");
    }
    else {
        if (no_user_traits_char) {
            rw_note (0, 0, 0, "UserTraits<char> tests disabled");
        }
        else {
            test_user_traits_char ();
        }

        if (no_user_traits_wchar_t) {
            rw_note (0, 0, 0, "UserTraits<wchar_t> tests disabled");
        }
        else {
            test_user_traits_wchar_t ();
        }

        if (no_user_traits_user_char) {
            rw_note (0, 0, 0, "UserTraits<UserChar> tests disabled");
        }
        else {
            test_user_traits_user_char ();
        }
    }

#undef TEST
#define TEST(fun)                                               \
        if (no_ ## fun)                                         \
            rw_note (0, 0, 0, "%s() tests disabled", #fun);     \
        else                                                    \
            test_ ## fun ()

    TEST (rw_widen);
    TEST (rw_expand);
    TEST (rw_narrow);
    TEST (rw_match);
    TEST (formatting);

    return 0;
}

/***********************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "",
                    0,
                    run_test,
                    "|-no-UserTraits# "
                    "|-no-UserTraits<char># "
                    "|-no-UserTraits<wchar_t># "
                    "|-no-UserTraits<UserChar># "
                    "|-no-rw_widen# "
                    "|-no-expand# "
                    "|-no-rw_narrow# "
                    "|-no-rw_macth# "
                    "|-no-formatting#",
                    &no_user_traits,
                    &no_user_traits_char,
                    &no_user_traits_wchar_t,
                    &no_user_traits_user_char,
                    &no_rw_widen,
                    &no_rw_expand, 
                    &no_rw_narrow,
                    &no_rw_match,
                    &no_formatting,
                    0);
}
