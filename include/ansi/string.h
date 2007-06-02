/***************************************************************************
 *
 * string.h
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
 * Copyright 1994-2005 Rogue Wave Software.
 * 
 **************************************************************************/

#include <rw/_defs.h>

#ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#  ifndef _RWSTD_STRING_H_INCLUDED
#    define _RWSTD_STRING_H_INCLUDED
#    include <cstring>

#    ifndef _RWSTD_NO_NAMESPACE

using std::size_t;

using std::memchr;

#if !defined (__GNUG__) || __GNUG__ > 2

// predeclared intrinsic functions cannot be redeclared
using std::memcmp;
using std::memcpy;
using std::strcmp;
using std::strcpy;
using std::strlen;

#endif   // < gcc 3.0

using std::memmove;
using std::memset;
using std::strcat;
using std::strchr;
using std::strcoll;
using std::strcspn;
using std::strerror;
using std::strncat;
using std::strncmp;
using std::strncpy;
using std::strpbrk;
using std::strrchr;
using std::strspn;
using std::strstr;
using std::strtok;
using std::strxfrm;

#    endif   // _RWSTD_NO_NAMESPACE
#  endif   // _RWSTD_STRING_H_INCLUDED

#else

#ifndef _RWSTD_STRING_H_INCLUDED
#define _RWSTD_STRING_H_INCLUDED

#  include _RWSTD_ANSI_C_STRING_H

#    undef index
#    undef rindex

#    undef strcpy
#    undef strcmp

#    undef strlen
#    undef strchr
#    undef strrchr
#    undef strcat
#    undef memchr
#    undef memcpy
#    undef memmove
#    undef memcmp
#    undef memset

#    undef memccpy
#    undef strncat
#    undef strncmp
#    undef strncpy


#if defined (_RWSTD_NO_MEMCHR) && !defined (_RWSTD_NO_MEMCHR_IN_LIBC)

extern "C" void* memchr (void*, int, _RWSTD_SIZE_T);

inline const void* memchr (const void *__s, int __c, _RWSTD_SIZE_T __n)
{
    return memchr (_RWSTD_CONST_CAST (void*, __s), __c, __n);
}

#  undef _RWSTD_NO_MEMCHR

#endif   // _RWSTD_NO_MEMCHR && !_RWSTD_NO_MEMCHR_IN_LIBC


#if defined (_RWSTD_NO_STRPBRK) && !defined (_RWSTD_NO_STRPBRK_IN_LIBC)

extern "C" char* strpbrk (char*, const char*);

inline const char* strpbrk (const char *__s, const char *__pat)
{
    return strpbrk (_RWSTD_CONST_CAST (char*, __s), __pat);
}

#  undef _RWSTD_NO_STRPBRK

#endif   // _RWSTD_NO_STRPBRK && !_RWSTD_NO_STRPBRK_IN_LIBC


#if defined (_RWSTD_NO_STRCHR) && !defined (_RWSTD_NO_STRCHR_IN_LIBC)

extern "C" char* strchr (char*, int);

inline const char* strchr (const char *__s, int __c)
{
    return strchr (_RWSTD_CONST_CAST (char*, __s), __c);
}

#  undef _RWSTD_NO_STRCHR

#endif   // _RWSTD_NO_STRCHR && !_RWSTD_NO_STRCHR_IN_LIBC


#if defined (_RWSTD_NO_STRRCHR) && !defined (_RWSTD_NO_STRRCHR_IN_LIBC)

extern "C" char* strrchr (char*, int);

inline const char* strrchr (const char *__s, int __c)
{
    return strrchr (_RWSTD_CONST_CAST (char*, __s), __c);
}

#  undef _RWSTD_NO_STRRCHR

#endif   // _RWSTD_NO_STRRCHR && !_RWSTD_NO_STRRCHR_IN_LIBC


#if defined (_RWSTD_NO_STRSTR) && !defined (_RWSTD_NO_STRSTR_IN_LIBC)

extern "C" char* strstr (char*, const char*);

inline const char* strstr (const char *__s1, const char *__s2)
{
    return strstr (_RWSTD_CONST_CAST (char*, __s1), __s2);
}

#  undef _RWSTD_NO_STRSTR

#endif   // _RWSTD_NO_STRSTR && !_RWSTD_NO_STRSTR_IN_LIBC

#  endif   // _RWSTD_STRING_H_INCLUDED
#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS
