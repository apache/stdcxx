/***************************************************************************
 *
 * wchar.h
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <rw/_defs.h>

#ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#  ifndef _RWSTD_WCHAR_H_INCLUDED
#    define _RWSTD_WCHAR_H_INCLUDED

#    include <cwchar>

#    ifndef _RWSTD_NO_NAMESPACE

#ifndef _RWSTD_NO_MBSTATE_T
using std::mbstate_t;
#endif
#ifndef _RWSTD_NO_WINT_T
using std::wint_t;
#endif
using std::size_t;
#ifndef _RWSTD_NO_BTOWC
using std::btowc;
#endif
#ifndef _RWSTD_NO_FGETWC
using std::fgetwc;
#endif
#ifndef _RWSTD_NO_FGETWS
using std::fgetws;
#endif
#ifndef _RWSTD_NO_FPUTWC
using std::fputwc;
#endif
#ifndef _RWSTD_NO_FPUTWS
using std::fputws;
#endif
#ifndef _RWSTD_NO_FWPRINTF
using std::fwprintf;
#endif
#ifndef _RWSTD_NO_FWSCANF
using std::fwscanf;
#endif
#ifndef _RWSTD_NO_GETWC
using std::getwc;
#endif
#ifndef _RWSTD_NO_GETWCHAR
using std::getwchar;
#endif
#ifndef _RWSTD_NO_MBRLEN
using std::mbrlen;
#endif
#ifndef _RWSTD_NO_MBRTOWC
using std::mbrtowc;
#endif
#ifndef _RWSTD_NO_MBSINIT
using std::mbsinit;
#endif
#ifndef _RWSTD_NO_MBSRTOWCS
using std::mbsrtowcs;
#endif
#ifndef _RWSTD_NO_PUTWC
using std::putwc;
#endif
#ifndef _RWSTD_NO_PUTWCHAR
using std::putwchar;
#endif
#ifndef _RWSTD_NO_SWPRINTF
using std::swprintf;
#endif
#ifndef _RWSTD_NO_SWSCANF
using std::swscanf;
#endif
#ifndef _RWSTD_NO_UNGETWC
using std::ungetwc;
#endif
#ifndef _RWSTD_NO_VFWPRINTF
using std::vfwprintf;
#endif
#ifndef _RWSTD_NO_VWPRINTF
using std::vwprintf;
#endif
#ifndef _RWSTD_NO_WCRTOMB
using std::wcrtomb;
#endif
#ifndef _RWSTD_NO_WCSCAT
using std::wcscat;
#endif
#ifndef _RWSTD_NO_WCSCHR
using std::wcschr;
#endif
#ifndef _RWSTD_NO_WCSCMP
using std::wcscmp;
#endif
#ifndef _RWSTD_NO_WCSCOLL
using std::wcscoll;
#endif
#ifndef _RWSTD_NO_WCSCPY
using std::wcscpy;
#endif
#ifndef _RWSTD_NO_WCSCSPN
using std::wcscspn;
#endif
#ifndef _RWSTD_NO_WCSLEN
using std::wcslen;
#endif
#ifndef _RWSTD_NO_WCSNCAT
using std::wcsncat;
#endif
#ifndef _RWSTD_NO_WCSNCMP
using std::wcsncmp;
#endif
#ifndef _RWSTD_NO_WCSNCPY
using std::wcsncpy;
#endif
#ifndef _RWSTD_NO_WCSPBRK
using std::wcspbrk;
#endif
#ifndef _RWSTD_NO_WCSRCHR
using std::wcsrchr;
#endif
#ifndef _RWSTD_NO_WCSRTOMBS
using std::wcsrtombs;
#endif
#ifndef _RWSTD_NO_WCSSPN
using std::wcsspn;
#endif
#ifndef _RWSTD_NO_WCSSTR
using std::wcsstr;
#endif
#ifndef _RWSTD_NO_WCSTOD
using std::wcstod;
#endif
#ifndef _RWSTD_NO_WCSTOK
using std::wcstok;
#endif
#ifndef _RWSTD_NO_WCSTOL
using std::wcstol;
#endif
#ifndef _RWSTD_NO_WCSTOUL
using std::wcstoul;
#endif
#ifndef _RWSTD_NO_WCSXFRM
using std::wcsxfrm;
#endif
#ifndef _RWSTD_NO_WCTOB
using std::wctob;
#endif
#ifndef _RWSTD_NO_WMEMCHR
using std::wmemchr;
#endif
#ifndef _RWSTD_NO_WMEMCMP
using std::wmemcmp;
#endif
#ifndef _RWSTD_NO_WMEMCPY
using std::wmemcpy;
#endif
#ifndef _RWSTD_NO_WMEMMOVE
using std::wmemmove;
#endif
#ifndef _RWSTD_NO_WMEMSET
using std::wmemset;
#endif
#ifndef _RWSTD_NO_WPRINTF
using std::wprintf;
#endif
#ifndef _RWSTD_NO_WSCANF
using std::wscanf;
#endif

#    endif   // _RWSTD_NO_NAMESPACE
#  endif   // _RWSTD_WCHAR_H_INCLUDED

#else   // if defined (_RWSTD_NO_DEPRECATED_C_HEADERS)

#  include _RWSTD_ANSI_C_WCHAR_H

#  include <rw/_mbstate.h>

#  ifndef WCHAR_MAX
#    define WCHAR_MAX   _RWSTD_WCHAR_MAX
#  endif

#  ifndef WCHAR_MIN
#    define WCHAR_MIN   _RWSTD_WCHAR_MIN
#  endif

extern "C" {

// add missing declarations for functions known to be defined
// in the system libc

#  if defined (_RWSTD_NO_MBSTATE_T) && defined (_RWSTD_MBSTATE_T)
typedef _RWSTD_MBSTATE_T mbstate_t;
#  endif   // _RWSTD_NO_MBSTATE_T && _RWSTD_MBSTATE_T

#  if defined (_RWSTD_NO_FWPRINTF) && !defined (_RWSTD_NO_FWPRINTF_IN_LIBC)
int fwprintf (FILE*, const wchar_t*, ...);
#    undef _RWSTD_NO_FWPRINTF
#  endif   // _RWSTD_NO_FWPRINTF && !_RWSTD_NO_FWPRINTF_IN_LIBC

#  if defined (_RWSTD_NO_FWSCANF) && !defined (_RWSTD_NO_FWSCANF_IN_LIBC)
int fwscanf (FILE*, const wchar_t*, ...);
#    undef _RWSTD_NO_FWSCANF
#  endif   // _RWSTD_NO_FWSCANF && !_RWSTD_NO_FWSCANF_IN_LIBC

#  if defined (_RWSTD_NO_SWPRINTF) && !defined (_RWSTD_NO_SWPRINTF_IN_LIBC)
#    if !defined (_MSC_VER)

// disabled for MSVC to avoid running into its awful declaration hackery
int swprintf (wchar_t*, _RWSTD_SIZE_T, const wchar_t*, ...);

#    endif   // !MSVC
#    undef _RWSTD_NO_SWPRINTF
#  endif   // _RWSTD_NO_SWPRINTF && !_RWSTD_NO_SWPRINTF_IN_LIBC

#  if defined (_RWSTD_NO_SWSCANF) && !defined (_RWSTD_NO_SWSCANF_IN_LIBC)
int swscanf (const wchar_t*, const wchar_t*, ...);
#    undef _RWSTD_NO_SWSCANF
#  endif   // _RWSTD_NO_SWSCANF && !_RWSTD_NO_SWSCANF_IN_LIBC

#  ifdef _RWSTD_VA_LIST
#    if defined (_RWSTD_NO_VFWPRINTF) && !defined (_RWSTD_NO_VFWPRINTF_IN_LIBC)
int vfwprintf (FILE*, const wchar_t*, _RWSTD_VA_LIST);
#      undef _RWSTD_NO_VFWPRINTF
#    endif   // _RWSTD_NO_VFWPRINTF && !_RWSTD_NO_VFWPRINTF_IN_LIBC

#    if defined (_RWSTD_NO_VFWSCANF) && !defined (_RWSTD_NO_VFWSCANF_IN_LIBC)
int vfwscanf (FILE*, const wchar_t*, _RWSTD_VA_LIST);
#      undef _RWSTD_NO_VFWSCANF
#    endif   // _RWSTD_NO_VFWSCANF && !_RWSTD_NO_VFWSCANF_IN_LIBC

#    if defined (_RWSTD_NO_VWPRINTF) && !defined (_RWSTD_NO_VWPRINTF_IN_LIBC)
int vwprintf (const wchar_t*, _RWSTD_VA_LIST);
#      undef _RWSTD_NO_VWPRINTF
#    endif   // _RWSTD_NO_VWPRINTF && !_RWSTD_NO_VWPRINTF_IN_LIBC

#    if defined (_RWSTD_NO_VWSCANF) && !defined (_RWSTD_NO_VWSCANF_IN_LIBC)
int vwscanf (const wchar_t*, _RWSTD_VA_LIST);
#      undef _RWSTD_NO_VWSCANF
#    endif   // _RWSTD_NO_VWSCANF && !_RWSTD_NO_VWSCANF_IN_LIBC
#  endif   // _RWSTD_VA_LIST

#  if defined (_RWSTD_NO_MBRLEN) && !defined (_RWSTD_NO_MBRLEN_IN_LIBC)
_RWSTD_SIZE_T mbrlen (const char*, _RWSTD_SIZE_T, _RWSTD_MBSTATE_T*);
#    undef _RWSTD_NO_MBRLEN
#  endif   // _RWSTD_NO_MBRLEN && !_RWSTD_NO_MBRLEN_IN_LIBC

#  if defined (_RWSTD_NO_MBRTOWC) && !defined (_RWSTD_NO_MBRTOWC_IN_LIBC)
_RWSTD_SIZE_T mbrtowc (wchar_t*, const char*, _RWSTD_SIZE_T, _RWSTD_MBSTATE_T*);
#    undef _RWSTD_NO_MBRTOWC
#  endif   // _RWSTD_NO_MBRTOWC && !_RWSTD_NO_MBRTOWC_IN_LIBC

#  if defined (_RWSTD_NO_MBSINIT) && !defined (_RWSTD_NO_MBSINIT_IN_LIBC)
int mbsinit (const _RWSTD_MBSTATE_T*);
#    undef _RWSTD_NO_MBSINIT
#  endif   // _RWSTD_NO_MBSINIT && !_RWSTD_NO_MBSINIT_IN_LIBC

#  if defined (_RWSTD_NO_MBSRTOWCS) && !defined (_RWSTD_NO_MBSRTOWCS_IN_LIBC)
_RWSTD_SIZE_T mbsrtowcs (const wchar_t*, const char**, _RWSTD_SIZE_T,
                         _RWSTD_MBSTATE_T*);
#    undef _RWSTD_NO_MBSRTOWCS
#  endif   // _RWSTD_NO_MBSRTOWCS && !_RWSTD_NO_MBSRTOWCS_IN_LIBC

#  if defined (_RWSTD_NO_WCRTOMB) && !defined (_RWSTD_NO_WCRTOMB_IN_LIBC)
_RWSTD_SIZE_T wcrtomb (char*, wchar_t, _RWSTD_MBSTATE_T*);
#    undef _RWSTD_NO_WCRTOMB
#  endif   // _RWSTD_NO_WCRTOMB && !_RWSTD_NO_WCRTOMB_IN_LIBC

#  if defined (_RWSTD_NO_WCSRTOMBS) && !defined (_RWSTD_NO_WCSRTOMBS_IN_LIBC)
_RWSTD_SIZE_T wcsrtombs (char*,  const char**, _RWSTD_SIZE_T,
                         _RWSTD_MBSTATE_T*);
#    undef _RWSTD_NO_WCSRTOMBS
#  endif   // _RWSTD_NO_WCSRTOMBS && !_RWSTD_NO_WCSRTOMBS_IN_LIBC


#  if defined (_RWSTD_NO_WCSCHR) && !defined (_RWSTD_NO_WCSCHR_IN_LIBC)

wchar_t* wcschr (wchar_t*, wchar_t);

}   // extern "C"

inline const wchar_t*
wcschr (const wchar_t *__s, wchar_t __c)
{
    return wcschr (_RWSTD_CONST_CAST (wchar_t*, __s), __c);
}

extern "C" {

#    undef _RWSTD_NO_WCSCHR
#  endif   // _RWSTD_NO_WCSCHR && !_RWSTD_NO_WCSCHR_IN_LIBC


#  if defined (_RWSTD_NO_WCSRCHR) && !defined (_RWSTD_NO_WCSRCHR_IN_LIBC)

wchar_t* wcsrchr (wchar_t*, wchar_t);

}   // extern "C"

inline const wchar_t*
wcsrchr (const wchar_t *__s, wchar_t __c)
{
    return wcsrchr (_RWSTD_CONST_CAST (wchar_t*, __s), __c);
}

extern "C" {

#    undef _RWSTD_NO_WCSCHR
#  endif   // _RWSTD_NO_WCSCHR && !_RWSTD_NO_WCSCHR_IN_LIBC


#  if defined (_RWSTD_NO_WCSSTR) && !defined (_RWSTD_NO_WCSSTR_IN_LIBC)

wchar_t* wcsstr (wchar_t*, const wchar_t*);

}   // extern "C"

inline const wchar_t*
wcsstr (const wchar_t *__s1, const wchar_t *__s2)
{
    return wcsstr (_RWSTD_CONST_CAST (wchar_t*, __s1), __s2);
}

extern "C" {

#    undef _RWSTD_NO_WCSSTR
#  endif   // _RWSTD_NO_WCSSTR && !_RWSTD_NO_WCSSTR_IN_LIBC


#  if defined (_RWSTD_NO_WMEMCHR) && !defined (_RWSTD_NO_WMEMCHR_IN_LIBC)

wchar_t* wmemchr (wchar_t*, wchar_t, _RWSTD_SIZE_T);

}   // extern "C"

inline const wchar_t*
wmemchr (const wchar_t *__s, wchar_t __c, _RWSTD_SIZE_T __n)
{
    return wmemchr (_RWSTD_CONST_CAST (wchar_t*, __s), __c, __n);
}

extern "C" {

#    undef _RWSTD_NO_WMEMCHR
#  endif   // _RWSTD_NO_WMEMCHR && !_RWSTD_NO_WMEMCHR_IN_LIBC


#  if defined (_RWSTD_NO_WMEMCMP) && !defined (_RWSTD_NO_WMEMCMP_IN_LIBC)
int wmemcmp (const wchar_t*, const wchar_t*, _RWSTD_SIZE_T);
#    undef _RWSTD_NO_WMEMCMP
#  endif   // _RWSTD_NO_WMEMCMP && !_RWSTD_NO_WMEMCMP_IN_LIBC

#  if defined (_RWSTD_NO_WMEMCPY) && !defined (_RWSTD_NO_WMEMCPY_IN_LIBC)
wchar_t* wmemcpy (wchar_t*, const wchar_t*, _RWSTD_SIZE_T);
#    undef _RWSTD_NO_WMEMCPY
#  endif   // _RWSTD_NO_WMEMCPY && !_RWSTD_NO_WMEMCPY_IN_LIBC

#  if defined (_RWSTD_NO_WMEMMOVE) && !defined (_RWSTD_NO_WMEMMOVE_IN_LIBC)
wchar_t* wmemmove (wchar_t*, const wchar_t*, _RWSTD_SIZE_T);
#    undef _RWSTD_NO_WMEMMOVE
#  endif   // _RWSTD_NO_WMEMMOVE && !_RWSTD_NO_WMEMMOVE_IN_LIBC

#  if defined (_RWSTD_NO_WMEMSET) && !defined (_RWSTD_NO_WMEMSET_IN_LIBC)
wchar_t* wmemset (wchar_t*, wchar_t, _RWSTD_SIZE_T);
#    undef _RWSTD_NO_WMEMSET
#  endif   // _RWSTD_NO_WMEMSET && !_RWSTD_NO_WMEMSET_IN_LIBC


#  if defined (_RWSTD_NO_WPRINTF) && !defined (_RWSTD_NO_WPRINTF_IN_LIBC)
int wprintf (const wchar_t*, ...);
#    undef _RWSTD_NO_WPRINTF
#  endif   // _RWSTD_NO_WPRINTF && !_RWSTD_NO_WPRINTF_IN_LIBC

#  if defined (_RWSTD_NO_WSCANF) && !defined (_RWSTD_NO_WSCANF_IN_LIBC)
int wscanf (const wchar_t*, ...);
#    undef _RWSTD_NO_WSCANF
#  endif   // _RWSTD_NO_WSCANF && !_RWSTD_NO_WSCANF_IN_LIBC

#  if defined (_RWSTD_NO_WCTOB) && !defined (_RWSTD_NO_WCTOB_IN_LIBC)
int wctob (_RWSTD_WINT_T);
#    undef _RWSTD_NO_WCTOB
#  endif   // _RWSTD_NO_WCTOB && !_RWSTD_NO_WCTOB_IN_LIBC

}   // extern "C"

#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS
