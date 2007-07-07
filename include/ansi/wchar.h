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

extern "C" {

// add missing declarations for functions known to be defined
// in the system libc

#if defined (_RWSTD_NO_WCTOB) && !defined (_RWSTD_NO_WCTOB_IN_LIBC)
int wctob (_RWSTD_WINT_T);
#  undef _RWSTD_NO_WCTOB
#endif   // _RWSTD_NO_WCTOB && !_RWSTD_NO_WCTOB_IN_LIBC

}   // extern "C"

#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS
