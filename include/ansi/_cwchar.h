// -*- C++ -*-
/***************************************************************************
 *
 * cwchar - C++ Standard library interface to the ANSI C header wchar.h
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

#ifndef _RWSTD_CWCHAR_H_INCLUDED
#define _RWSTD_CWCHAR_H_INCLUDED


#include <rw/_mbstate.h>   // for _RWSTD_MBSTATE_T
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) {


#ifndef NULL
#  define NULL 0
#endif   // NULL

#ifdef _RWSTD_SIZE_T
typedef _RWSTD_SIZE_T size_t;
#else
typedef unsigned long size_t;
#endif   // _RWSTD_SIZE_T

extern "C" {

// _RWSTD_MBSTATE_T is defined in <rw/_mbstate.h>
typedef _RWSTD_MBSTATE_T mbstate_t;

struct tm;

#ifndef WCHAR_MIN
#  define WCHAR_MIN _RWSTD_WCHAR_MIN
#endif   // WCHAR_MIN

#ifndef WCHAR_MAX
#  define WCHAR_MAX _RWSTD_WCHAR_MAX
#endif   // WCHAR_MAX

#ifndef WEOF
#  define WEOF _RWSTD_WEOF
#endif   // WEOF

#ifndef _RWSTD_NO_WINT_T
typedef _RWSTD_WINT_T wint_t;
#else
typedef int wint_t;
#endif   // _RWSTD_NO_WINT_T

// declared, not defined (FILE need not be a complete type)
struct FILE;

int fwprintf (FILE*, const wchar_t*, ...);
int fwscanf (FILE*, const wchar_t*, ...);
int wprintf (const wchar_t*, ...);
int wscanf (const wchar_t*, ...);
int swprintf (wchar_t*, size_t, const wchar_t*, ...);
int swscanf (const wchar_t*, const wchar_t*, ...);
int vfwprintf (FILE*, const wchar_t *format, _RWSTD_VA_LIST);
int vwprintf (const wchar_t*, _RWSTD_VA_LIST);
int vswprintf (wchar_t*, size_t, const wchar_t*, _RWSTD_VA_LIST);
wint_t fgetwc (FILE*);
wchar_t* fgetws (wchar_t*, int, FILE*);
wint_t fputwc (wchar_t, FILE*);
int fputws (const wchar_t*, FILE*);
wint_t getwc (FILE*);
wint_t getwchar();
wint_t putwc (wchar_t, FILE*);
wint_t putwchar (wchar_t);
wint_t ungetwc (wint_t, FILE*);
int fwide (FILE*, int);
double wcstod (const wchar_t*, wchar_t**);
long int wcstol (const wchar_t*, wchar_t**, int);
unsigned long int wcstoul (const wchar_t*, wchar_t**, int);
wchar_t* wcscpy (wchar_t*, const wchar_t*);
wchar_t* wcsncpy (wchar_t*, const wchar_t*, size_t);
wchar_t* wcscat (wchar_t*, const wchar_t*);
wchar_t* wcsncat (wchar_t*, const wchar_t*, size_t);
int wcscmp (const wchar_t*, const wchar_t*);
int wcscoll (const wchar_t*, const wchar_t*);
int wcsncmp (const wchar_t*, const wchar_t*, size_t);
size_t wcsxfrm (wchar_t*, const wchar_t*, size_t);
wchar_t* wcschr (wchar_t*, wchar_t);
size_t wcscspn (const wchar_t*, const wchar_t*);
wchar_t* wcspbrk (wchar_t*, const wchar_t*);
wchar_t* wcsrchr (wchar_t*, wchar_t);
size_t wcsspn (const wchar_t*, const wchar_t*);
wchar_t* wcsstr (wchar_t*, const wchar_t*);
wchar_t* wcstok (wchar_t*, const wchar_t*, wchar_t**);
size_t wcslen (const wchar_t*);
wchar_t* wmemchr (wchar_t*, wchar_t, size_t);
int wmemcmp (const wchar_t*, const wchar_t*, size_t);
wchar_t* wmemcpy (wchar_t*, const wchar_t*, size_t);
wchar_t* wmemmove (wchar_t*, const wchar_t*, size_t);
wchar_t* wmemset (wchar_t*, wchar_t, size_t);

size_t wcsftime (wchar_t*, size_t, const wchar_t*, const struct tm*);
wint_t btowc (int);
int wctob (wint_t);

int mbsinit (const mbstate_t*);
size_t mbrlen (const char*, size_t, mbstate_t*);
size_t mbrtowc (wchar_t*, const char*, size_t, mbstate_t*);
size_t wcrtomb (char*, wchar_t, mbstate_t*);
size_t mbsrtowcs (wchar_t*, const char**, size_t, mbstate_t*);
size_t wcsrtombs (char*, const wchar_t**, size_t, mbstate_t*);


}   // extern "C"


// 21.4, p14 of ISO/IEC 14882:1998
inline const wchar_t* wcschr (const wchar_t *__s, wchar_t __c)
{
    return wcschr (_RWSTD_CONST_CAST (wchar_t*, __s), __c);
}


// 21.4, p16 of ISO/IEC 14882:1998
inline const wchar_t* wcspbrk (const wchar_t *__s1, const wchar_t *__s2)
{
    return wcspbrk (_RWSTD_CONST_CAST (wchar_t*, __s1), __s2);
}


// 21.4, p18 of ISO/IEC 14882:1998
inline const wchar_t* wcsrchr (const wchar_t *__s, wchar_t __c)
{
    return wcsrchr (_RWSTD_CONST_CAST (wchar_t*, __s), __c);
}


// 21.4, p20 of ISO/IEC 14882:1998
inline const wchar_t* wcsstr (const wchar_t *__s1, const wchar_t *__s2)
{
    return wcsstr (_RWSTD_CONST_CAST (wchar_t*, __s1), __s2);
}


// 21.4, p22 of ISO/IEC 14882:1998
inline const wchar_t* wmemchr (const wchar_t *__p, wchar_t __c, size_t __n)
{
    return wmemchr (_RWSTD_CONST_CAST (wchar_t*, __p), __c, __n);
}


}   // namespace std


#endif   // _RWSTD_CWCHAR_H_INCLUDED
