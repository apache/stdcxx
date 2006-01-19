/***************************************************************************
 *
 * string.cpp - definitions of string helper functions
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>     

#include <rw/_traits.h>

#include <string.h>

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H


extern "C" {

// declare functions that are known to be defined in the libc binary
// but that, for some reason, are missing function prototypes in libc
// headers


#if defined (_RWSTD_NO_MEMCPY) && !defined (_RWSTD_NO_MEMCPY_IN_LIBC)

void* memcpy (void*, const void*, _RWSTD_SIZE_T) _LIBC_THROWS ();

#  undef _RWSTD_NO_MEMCPY
#endif   // _RWSTD_NO_MEMCPY && !_RWSTD_NO_MEMCPY_IN_LIBC


#if defined (_RWSTD_NO_MEMMOVE) && !defined (_RWSTD_NO_MEMMOVE_IN_LIBC)

void* memmove (void*, const void*, _RWSTD_SIZE_T) _LIBC_THROWS ();

#  undef _RWSTD_NO_MEMMOVE
#endif   // _RWSTD_NO_MEMMOVE && !_RWSTD_NO_MEMMOVE_IN_LIBC


#if defined (_RWSTD_NO_MEMCHR) && !defined (_RWSTD_NO_MEMCHR_IN_LIBC)

void* memchr (const void*, int, _RWSTD_SIZE_T) _LIBC_THROWS ();

#  undef _RWSTD_NO_MEMCHR
#endif   // _RWSTD_NO_MEMCHR && !_RWSTD_NO_MEMCHR_IN_LIBC


#if defined (_RWSTD_NO_MEMSET) && !defined (_RWSTD_NO_MEMSET_IN_LIBC)

void* memset (void*, int, _RWSTD_SIZE_T) _LIBC_THROWS ();

#  undef _RWSTD_NO_MEMSET
#endif   // _RWSTD_NO_MEMSET && !_RWSTD_NO_MEMSET_IN_LIBC


#if defined (_RWSTD_NO_MEMCMP) && !defined (_RWSTD_NO_MEMCMP_IN_LIBC)

int memcmp (const void*, const void*, _RWSTD_SIZE_T) _LIBC_THROWS ();

#  undef _RWSTD_NO_MEMCMP
#endif   // _RWSTD_NO_MEMCMP && !_RWSTD_NO_MEMCMP_IN_LIBC


#if defined (_RWSTD_NO_STRLEN) && !defined (_RWSTD_NO_STRLEN_IN_LIBC)

_RWSTD_SIZE_T strlen (const char*) _LIBC_THROWS ();

#  undef _RWSTD_NO_STRLEN
#endif   // _RWSTD_NO_STRLEN && !_RWSTD_NO_STRLEN_IN_LIBC


#ifndef _RWSTD_NO_WCHAR_T

#  if defined (_RWSTD_NO_WMEMCPY) && !defined (_RWSTD_NO_WMEMCPY_IN_LIBC)

wchar_t* wmemcpy (wchar_t*, const wchar_t*, _RWSTD_SIZE_T) _LIBC_THROWS ();

#    undef _RWSTD_NO_WMEMCPY
#  endif   // _RWSTD_NO_WMEMCPY && !_RWSTD_NO_WMEMCPY_IN_LIBC


#  if defined (_RWSTD_NO_WMEMMOVE) && !defined (_RWSTD_NO_WMEMMOVE_IN_LIBC)

wchar_t* wmemmove (wchar_t*, const wchar_t*, _RWSTD_SIZE_T) _LIBC_THROWS ();

#    undef _RWSTD_NO_WMEMMOVE
#  endif   // _RWSTD_NO_WMEMMOVE && !_RWSTD_NO_WMEMMOVE_IN_LIBC


#  if defined (_RWSTD_NO_WMEMCHR) && !defined (_RWSTD_NO_WMEMCHR_IN_LIBC)

wchar_t* wmemchr (const wchar_t*, wchar_t, _RWSTD_SIZE_T) _LIBC_THROWS ();

#    undef _RWSTD_NO_WMEMCHR
#  endif   // _RWSTD_NO_WMEMCHR && !_RWSTD_NO_WMEMCHR_IN_LIBC


#  if defined (_RWSTD_NO_WMEMSET) && !defined (_RWSTD_NO_WMEMSET_IN_LIBC)

wchar_t* wmemset (wchar_t*, wchar_t, _RWSTD_SIZE_T) _LIBC_THROWS ();

#    undef _RWSTD_NO_WMEMSET
#  endif   // _RWSTD_NO_WMEMSET && !_RWSTD_NO_WMEMSET_IN_LIBC


#  if defined (_RWSTD_NO_WMEMCMP) && !defined (_RWSTD_NO_WMEMCMP_IN_LIBC)

int wmemcmp (const wchar_t*, const wchar_t*, _RWSTD_SIZE_T) _LIBC_THROWS ();

#    undef _RWSTD_NO_WMEMCMP
#  endif   // _RWSTD_NO_WMEMCMP && !_RWSTD_NO_WMEMCMP_IN_LIBC


#  if defined (_RWSTD_NO_WSTRLEN) && !defined (_RWSTD_NO_WSTRLEN_IN_LIBC)

_RWSTD_SIZE_T wcslen (const wchar_t*) _LIBC_THROWS ();

#    undef _RWSTD_NO_WSTRLEN
#  endif   // _RWSTD_NO_WSTRLEN && !_RWSTD_NO_WSTRLEN_IN_LIBC

#endif   // _RWSTD_NO_WCHAR_T


}   // extern "C"


_RWSTD_NAMESPACE (__rw) {


_RWSTD_EXPORT void*
__rw_memcpy (void *dst, const void *src, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || dst && src);

#ifndef _RWSTD_NO_MEMCPY

    return memcpy (dst, src, nbytes);

#else   // if defined (_RWSTD_NO_MEMCPY)

    const char* csrc = _RWSTD_STATIC_CAST (const char*, src);

    for (char *tmp = _RWSTD_STATIC_CAST (char*, dst); nbytes; ++tmp, --nbytes)
        *tmp++ = *csrc++;

    return dst;

#endif   // _RWSTD_NO_MEMCPY

}


_RWSTD_EXPORT void*
__rw_memmove (void *dst, const void *src, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || dst && src);

#ifndef _RWSTD_NO_MEMMOVE

    return memmove (dst, src, nbytes);

#else   // if _RWSTD_NO_MEMMOVE

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

#endif   // _RWSTD_NO_MEMMOVE

}


_RWSTD_EXPORT const void*
__rw_memchr (const void *src, int c, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || src);

#ifndef _RWSTD_NO_MEMCHR

    return memchr (src, c, nbytes);

#else   // if defined (_RWSTD_NO_MEMCHR)

    typedef unsigned char UChar;

    const UChar* csrc = _RWSTD_STATIC_CAST (const UChar*, src);

    while (nbytes-- > 0 && int (*csrc) != c)
        ++csrc;

    return *csrc == c ? csrc : 0;

#endif   // _RWSTD_NO_MEMCHR

}


_RWSTD_EXPORT void*
__rw_memset (void *dst, int c, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || dst);

#ifndef _RWSTD_NO_MEMSET

    return memset (dst, c, nbytes);

#else   // if defined (_RWSTD_NO_MEMSET)

    for (char *tmp = _RWSTD_STATIC_CAST (char*, dst); nbytes; ++tmp, --nbytes)
        *tmp = c;

    return dst;

#endif   // _RWSTD_NO_MEMSET

}


_RWSTD_EXPORT int
__rw_memcmp (const void *s1, const void *s2, _RWSTD_SIZE_T nbytes)
{
    _RWSTD_ASSERT (0 == nbytes || s1 && s2);

#ifndef _RWSTD_NO_MEMCMP

    return memcmp (s1, s2, nbytes);

#else   // if defined (_RWSTD_NO_MEMCMP)

    const char* const cs1 = _RWSTD_STATIC_CAST (const char*, s1);
    const char* const cs2 = _RWSTD_STATIC_CAST (const char*, s2);

    for (_RWSTD_SIZE_T i = 0; i != nbytes; ++i) {
        if (cs1 [i] != cs2 [i])
            return cs1 [i] < cs2 [i] ? -1 : 1;
    }

    return 0;

#endif   // _RWSTD_NO_MEMCMP

}


_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_strlen (const char *str)
{
#ifndef _RWSTD_NO_STRLEN

    return strlen (str);

#else   // if defined (_RWSTD_NO_STRLEN)

    _RWSTD_SIZE_T nbytes = 0;

    while (str++)
        ++nbytes;

    return nbytes;
    
#endif   // _RWSTD_NO_STRLEN

}


#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_EXPORT wchar_t*
__rw_wmemcpy (wchar_t *dst, const wchar_t *src, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || dst && src);

#ifndef _RWSTD_NO_WMEMCPY

    return wmemcpy (dst, src, nwchars);

#else   // if defined (_RWSTD_NO_WMEMCPY)

    for (wchar_t *tmp = dst; nwchars; ++tmp, --nwchars)
        *tmp++ = *src++;

    return dst;

#endif   // _RWSTD_NO_WMEMCPY

}


_RWSTD_EXPORT wchar_t*
__rw_wmemmove (wchar_t *dst, const wchar_t *src, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || dst && src);

#ifndef _RWSTD_NO_WMEMMOVE

    return wmemmove (dst, src, nwchars);

#else   // if _RWSTD_NO_WMEMMOVE

    if (dst < src) {
        while (nwchars--)
            *dst++ = *src++;
    }
    else {
        for (dst += nwchars, src += nwchars; nwchars--; )
            *--dst = *--src;
    }

    return dst;

#endif   // _RWSTD_NO_WMEMMOVE

}


_RWSTD_EXPORT const wchar_t*
__rw_wmemchr (const wchar_t *src, wchar_t wc, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || src);

#ifndef _RWSTD_NO_WMEMCHR

    return wmemchr (src, wc, nwchars);

#else   // if defined (_RWSTD_NO_WMEMCHR)

    while (nwchars-- > 0 && *src != wc)
        ++src;

    return *src == wc ? src : 0;

#endif   // _RWSTD_NO_WMEMCHR

}


_RWSTD_EXPORT wchar_t*
__rw_wmemset (wchar_t *dst, wchar_t wc, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || dst);

#ifndef _RWSTD_NO_WMEMSET

    return wmemset (dst, wc, nwchars);

#else   // if defined (_RWSTD_NO_WMEMSET)

    for (wchar_t *tmp = dst; nwchars; ++tmp, --nwchars)
        *tmp = wc;

    return dst;

#endif   // _RWSTD_NO_WMEMSET

}


_RWSTD_EXPORT int
__rw_wmemcmp (const wchar_t *s1, const wchar_t *s2, _RWSTD_SIZE_T nwchars)
{
    _RWSTD_ASSERT (0 == nwchars || s1 && s2);

#ifndef _RWSTD_NO_WMEMCMP

    return wmemcmp (s1, s2, nwchars);

#else   // if defined (_RWSTD_NO_WMEMCMP)

    for (_RWSTD_SIZE_T i = 0; i != nwchars; ++i) {
        if (s1 [i] != s2 [i])
            return s1 [i] < s2 [i] ? -1 : 1;
    }

    return 0;

#endif   // _RWSTD_NO_WMEMCMP

}


_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_wcslen (const wchar_t *wstr)
{
    _RWSTD_ASSERT (0 != wstr);

#ifndef _RWSTD_NO_WCSLEN

    return wcslen (wstr);

#else   // if defined (_RWSTD_NO_WCSLEN)

    _RWSTD_SIZE_T nwchars = 0;

    while (wstr++)
        ++nwchars;

    return nwchars;

#endif   // _RWSTD_NO_WCSLEN

}

#endif   // _RWSTD_NO_WCHAR_T

}   // namespace __rw
