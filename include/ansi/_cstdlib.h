// -*- C++ -*-
/***************************************************************************
 *
 * cstdlib - C++ Standard library interface to the ANSI C header stdlib.h
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_CSTDLIB_H_INCLUDED
#define _RWSTD_CSTDLIB_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


#ifndef EXIT_FAILURE
#  define EXIT_FAILURE   1
#endif

#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS   0
#endif

#ifndef MB_CUR_MAX
#   define MB_CUR_MAX _RWSTD_MB_CUR_MAX
#endif

#ifndef NULL
#  define NULL 0
#endif

#ifndef RAND_MAX
#  define RAND_MAX   _RWSTD_RAND_MAX
#endif


extern "C" {


struct div_t
{
    // traditional layout (e.g., HP-UX, Linux, Solaris)
    int quot;
    int rem;
};

struct ldiv_t
{
    // traditional layout (e.g., HP-UX, Linux, Solaris)
    long quot;
    long rem;
};


#ifdef _RWSTD_SIZE_T
typedef _RWSTD_SIZE_T size_t;
#else
typedef unsigned long size_t;
#endif   // _RWSTD_SIZE_T


double atof (const char*);
int atoi (const char*);
long int atol (const char*);
double strtod (const char*, char**);
long int strtol (const char*, char**, int);
unsigned long int strtoul (const char*, char**, int);
int rand (void);
void srand (unsigned int);
void* calloc (size_t, size_t);
void free (void*);
void* malloc (size_t) ;
void* realloc (void*, size_t);
void abort (void);
int atexit (void (*) (void));
void exit (int);
char* getenv (const char*);
int system (const char*);
void* bsearch (const void*, const void*, size_t, size_t,
               int (*) (const void*, const void*));
void qsort (void*, size_t, size_t, int (*) (const void *, const void *));
int abs (int);
div_t div (int, int);
long int labs (long int);
ldiv_t ldiv (long int, long int);
int mblen (const char*, size_t);
int mbtowc (wchar_t*, const char*, size_t);
int wctomb (char*, wchar_t);
size_t mbstowcs (wchar_t*, const char*, size_t);
size_t wcstombs (char*, const wchar_t*, size_t);


}   // extern "C"

#ifndef _RWSTD_NO_EXTERN_C_COMPATIBILITY
#  ifndef _RWSTD_NO_EXTERN_C_OVERLOAD

    extern "C" {
        typedef int (*__rw_compar_t)(const void*, const void*);
    }

    extern "C++" {

    // 25.4, p3 of ISO/IEC 14882:1998
    inline void*
    bsearch (const void *__key, const void *__base, size_t __n, size_t __size,
             int (*__cmp)(const void*, const void*))
    {
        return bsearch (__key, __base, __n, __size,
                        _RWSTD_REINTERPRET_CAST (__rw_compar_t, __cmp));
    }

    // 25.4, p4 of ISO/IEC 14882:1998
    inline void
    qsort (void *__base, size_t __n, size_t __size,
           int (*__cmp)(const void*, const void*))
    {
        return _RWSTD_C::qsort (__base, __n, __size,
                               _RWSTD_REINTERPRET_CAST (__rw_compar_t, __cmp));
    }

    inline long
    abs (long __x)
    {
        return labs (__x);
    }

    inline ldiv_t
    div (long __x, long __y)
    {
        return ldiv (__x, __y);
    }

    }   // extern "C++"

#  endif   // _RWSTD_NO_EXTERN_C_OVERLOAD
#endif   // _RWSTD_NO_EXTERN_C_COMPATIBILITY


}   // namespace std


#endif   // _RWSTD_CSTDLIB_H_INCLUDED
