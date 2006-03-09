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


int UserTraits<UserChar>::
compare (const char_type *s1, const char_type *s2, size_t n)
{
    RW_ASSERT (0 == n || s1 && s2);

    for (size_t i = 0; i != n; ++i) {
        if (!eq (s1[i], s2[i])) {
            return lt (s1[i], s2[i]) ? -1 : 1;
        }
    }

    return 0;
}
        

size_t UserTraits<UserChar>::
length (const char_type *s)
{
    RW_ASSERT (0 != s);

    size_t len = 0;
    for (; !eq (*s++, char_type::eos ()); ++len);
    return len;
}
 

const UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
find (const char_type *s, size_t n, const char_type &c)
{
    RW_ASSERT (0 == n || s);

    for (; n-- && !eq (*s, c); ++s);

    return eq (*s, c) ? s : 0;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
copy (char_type *dst, const char_type *src, size_t n)
{
    for (; n--; *dst++ = *src++);

    return dst;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
move (char_type *s1, const char_type *s2, size_t n)
{
    RW_ASSERT (0 == n || s1 && s2);

    if (s1 < s2)
        copy (s1, s2, n);
    else if (s2 < s1) {
        s1 += n;
        s2 += n;
        for (size_t i = 0; i != n; ++i) 
            assign (*--s1, *--s2);
    }
    return s1;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
assign (char_type *s, size_t n, char_type c)
{
    RW_ASSERT (0 == n || s);

    for (; n--; assign (s [n], c));

    return s;
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
