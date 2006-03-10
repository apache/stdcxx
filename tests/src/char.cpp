/**************************************************************************
 *
 * char.cpp - helpers for user-defined character type and its traits
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

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC
#include <rw_char.h>

#include <string.h>   // for memcpy()


size_t
UserTraits<UserChar>::
n_calls_ [UserTraits<UserChar>::MemFun::n_funs];


void
UserTraits<UserChar>::
assign (char_type &c1, const char_type &c2)
{
    ++n_calls_ [MemFun::assign];

    c1.f = c2.f;
    c1.c = c2.c;
}


bool
UserTraits<UserChar>::
eq (const char_type &c1, const char_type &c2)
{
    ++n_calls_ [MemFun::eq];

    return c1.f == c2.f && c1.c == c2.c;
}


bool
UserTraits<UserChar>::
lt (const char_type &c1, const char_type &c2)
{
    ++n_calls_ [MemFun::lt];

    return c1.f < c2.f || c1.f == c2.f && c1.c < c2.c;
}


int UserTraits<UserChar>::
compare (const char_type *s1, const char_type *s2, size_t n)
{
    RW_ASSERT (0 == n || s1 && s2);

    ++n_calls_ [MemFun::compare];

    for (size_t i = 0; i != n; ++i) {
        if (s1 [i].f != s2 [i].f || s1 [i].c != s2 [i].c) {
            if (   s1 [i].f < s2 [i].f
                || s1 [i].f == s2 [i].f && s1 [i].c < s2 [i].c)
                return -1;

            return 1;
        }
    }

    return 0;
}
        

size_t UserTraits<UserChar>::
length (const char_type *s)
{
    RW_ASSERT (0 != s);

    ++n_calls_ [MemFun::length];

    size_t len = 0;

    for (; s [len].f || s [len].c; ++len);

    return len;
}
 

const UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
find (const char_type *s, size_t n, const char_type &c)
{
    RW_ASSERT (0 == n || s);

    ++n_calls_ [MemFun::find];

    for (; n--; ++s) {
        if (s->f == c.f && s->c == c.c)
            return s;
    }

    return 0;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
copy (char_type *dst, const char_type *src, size_t n)
{
    RW_ASSERT (0 == n || dst && src);

    ++n_calls_ [MemFun::copy];

    for (size_t i = 0; i != n; dst [i] = src [i]);

    return dst;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
move (char_type *dst, const char_type *src, size_t n)
{
    RW_ASSERT (0 == n || dst && src);

    ++n_calls_ [MemFun::move];

    for (; n--; dst [n] = src [n]);

    return dst;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
assign (char_type *s, size_t n, char_type c)
{
    RW_ASSERT (0 == n || s);

    ++n_calls_ [MemFun::assign2];

    for (; n--; s [n] = c);

    return s;
}


UserTraits<UserChar>::int_type
UserTraits<UserChar>::
not_eof (const int_type &i)
{
    ++n_calls_ [MemFun::not_eof];

    if (i.equal (int_type::eof ())) {
        const char_type c = { 0, 0 };

        return int_type::from_char (c);
    }

    return i;
}


UserTraits<UserChar>::char_type
UserTraits<UserChar>::
to_char_type (const int_type &i)
{
    ++n_calls_ [MemFun::to_char_type];

    return i.to_char ();
}
      

UserTraits<UserChar>::int_type
UserTraits<UserChar>::
to_int_type (const char_type &c)
{
    ++n_calls_ [MemFun::to_int_type];

    return int_type::from_char (c);
}


bool
UserTraits<UserChar>::
eq_int_type (const int_type &i1, const int_type &i2)
{
    ++n_calls_ [MemFun::eq_int_type];

    return i1.equal (i2);
}


UserTraits<UserChar>::int_type
UserTraits<UserChar>::
eof ()
{
    ++n_calls_ [MemFun::eof];

    return int_type::eof ();
}


_TEST_EXPORT
char*
rw_widen (char *dst, const char *src, size_t len)
{
    // dst must point to an array of at least 1 element
    RW_ASSERT (0 != dst);

    // allow src to be null
    if (0 == src)
        src = "";

    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len)
        len = src ? strlen (src) : 0;

    // copy src into dst
    if (len)
        memcpy (dst, src, len);
    else
        *dst = '\0';

    return dst;
}


#ifndef _RWSTD_WCHAR_T

_TEST_EXPORT
wchar_t*
rw_widen (wchar_t *dst, const char *src, size_t len)
{
    // dst must point to an array of at least 1 element
    RW_ASSERT (0 != dst);

    // allow src to be null
    if (0 == src)
        src = "";

    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len)
        len = src ? strlen (src) : 0;

    // widen src into dst one element at a time
    for (size_t i = 0; ; ++i) {
        typedef unsigned char UChar;

        // always NUL-terminate dst
        dst [i] = wchar_t (UChar (src [i]));

        if (i == len)
            break;
    }

    return dst;
}

#endif   // _RWSTD_WCHAR_T


_TEST_EXPORT
UserChar*
rw_widen (UserChar *dst, const char *src, size_t len)
{
    // dst must point to an array of at least 1 element
    RW_ASSERT (0 != dst);

    // allow src to be null
    if (0 == src)
        src = "";

    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len)
        len = src ? strlen (src) : 0;

    // widen src into dst one element at a time
    for (size_t i = 0; ; ++i) {
        typedef unsigned char UChar;

        // always NUL-terminate dst
        dst [i].f = 0;
        dst [i].c = UChar (src [i]);

        if (i == len)
            break;
    }

    return dst;
}
