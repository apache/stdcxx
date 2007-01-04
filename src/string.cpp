/***************************************************************************
 *
 * string.cpp - definitions of string helper functions
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 **************************************************************************/

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>     


_RWSTD_NAMESPACE (__rw) {


_RWSTD_EXPORT void*
__rw_memcpy (void *dst, const void *src, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || dst && src);

    const char* csrc = _RWSTD_STATIC_CAST (const char*, src);

    for (char *tmp = _RWSTD_STATIC_CAST (char*, dst); nbytes; --nbytes)
        *tmp++ = *csrc++;

    return dst;
}


_RWSTD_EXPORT void*
__rw_memmove (void *dst, const void *src, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || dst && src);

    char* cdst = _RWSTD_STATIC_CAST (char*, dst);
    const char* csrc = _RWSTD_STATIC_CAST (const char*, src);

    if (cdst < csrc) {
        while (nbytes--)
            *cdst++ = *csrc++;
    }
    else {
        for (cdst += nbytes, csrc += nbytes; nbytes--; )
            *--cdst = *--csrc;
    }

    return dst;
}


_RWSTD_EXPORT const void*
__rw_memchr (const void *src, int c, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || src);

    typedef unsigned char UChar;

    const UChar* csrc = _RWSTD_STATIC_CAST (const UChar*, src);

    while (nbytes > 0 && int (*csrc) != c) {
        ++csrc;
        --nbytes;
    }

    return nbytes ? csrc : 0;
}


_RWSTD_EXPORT void*
__rw_memset (void *dst, int c, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || dst);

    for (char *tmp = _RWSTD_STATIC_CAST (char*, dst); nbytes; ++tmp, --nbytes)
        *tmp = c;

    return dst;
}


_RWSTD_EXPORT int
__rw_memcmp (const void *s1, const void *s2, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || s1 && s2);

    typedef unsigned char UChar;

    const UChar* cs1 = _RWSTD_STATIC_CAST (const UChar*, s1);
    const UChar* cs2 = _RWSTD_STATIC_CAST (const UChar*, s2);

    int result = 0;

    for ( ; nbytes && !(result = *cs1 - *cs2); ++cs1, ++cs2, --nbytes);

    return result;
}


_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_strlen (const char *str)
{
    const char* const begin = str;

    for (; *str; ++str);

    return _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, str - begin);
}


#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_EXPORT wchar_t*
__rw_wmemcpy (wchar_t *dst, const wchar_t *src, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || dst && src);

    for (wchar_t *tmp = dst; nwchars; --nwchars)
        *tmp++ = *src++;

    return dst;
}


_RWSTD_EXPORT wchar_t*
__rw_wmemmove (wchar_t *dst, const wchar_t *src, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || dst && src);

    if (dst < src) {
        while (nwchars--)
            *dst++ = *src++;
    }
    else {
        for (dst += nwchars, src += nwchars; nwchars--; )
            *--dst = *--src;
    }

    return dst;
}


_RWSTD_EXPORT const wchar_t*
__rw_wmemchr (const wchar_t *src, wchar_t wc, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || src);

    while (nwchars > 0 && *src != wc) {
        ++src;
        --nwchars;
    }

    return nwchars ? src : 0;
}


_RWSTD_EXPORT wchar_t*
__rw_wmemset (wchar_t *dst, wchar_t wc, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || dst);

    for (wchar_t *tmp = dst; nwchars; ++tmp, --nwchars)
        *tmp = wc;

    return dst;
}


_RWSTD_EXPORT int
__rw_wmemcmp (const wchar_t *s1, const wchar_t *s2, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || s1 && s2);

    int result = 0;

    for ( ; nwchars && !(result = *s1 - *s2); ++s1, ++s2, --nwchars);

    return result;
}


_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_wcslen (const wchar_t *wstr)
{
    _RWSTD_ASSERT (0 != wstr);

    const wchar_t* const begin = wstr;

    for (; *wstr; ++wstr);

    return _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, wstr - begin);
}

#endif   // _RWSTD_NO_WCHAR_T

}   // namespace __rw
