// -*- C++ -*-
/***************************************************************************
 *
 * cstring - C++ Standard library interface to the ANSI C header string.h
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

#ifndef _RWSTD_CSTRING_H_INCLUDED
#define _RWSTD_CSTRING_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


#ifndef NULL
#  define NULL 0
#endif   // NULL

extern "C" {

#ifdef _RWSTD_SIZE_T
typedef _RWSTD_SIZE_T size_t;
#else
typedef unsigned long size_t;
#endif   // _RWSTD_SIZE_T


void* memcpy (void*, const void*, size_t);
void* memmove (void*, const void*, size_t);
char* strcpy (char*, const char*);
char* strncpy (char*, const char*, size_t);
char* strcat (char*, const char*);
char* strncat (char*, const char*, size_t);
int memcmp (const void*, const void*, size_t);
int strcmp (const char*, const char*);
int strcoll (const char*, const char*);
int strncmp (const char*, const char*, size_t);
size_t strxfrm (char*, const char*, size_t);
void* memchr (void*, int, size_t);   // 21.4, p12 of ISO/IEC 14882:1998
char* strchr (char*, int);   // 21.4, p4 of ISO/IEC 14882:1998
size_t strcspn (const char*, const char*);
char* strpbrk (char*, const char*);   // 21.4, p6 of ISO/IEC 14882:1998
char* strrchr (char*, int);   // 21.4, p8 of ISO/IEC 14882:1998
size_t strspn (const char*, const char*);
char* strstr (char*, const char*);   // 21.4, p10 of ISO/IEC 14882:1998
char* strtok (char*, const char*);
void* memset (void*, int, size_t);
char* strerror (int);
size_t strlen (const char*);


}   // extern "C"


// 21.4, p12 of ISO/IEC 14882:1998
inline const void* memchr (const void *__p, int __c, size_t __n)
{
    return memchr (_RWSTD_CONST_CAST (void*, __p), __c, __n);
}

// 21.4, p4 of ISO/IEC 14882:1998
inline const char* strchr (const char *__s, int __c)
{
    return strchr (_RWSTD_CONST_CAST (char*, __s), __c);
}

// 21.4, p6 of ISO/IEC 14882:1998
inline const char* strpbrk (const char *__s1, const char *__s2)
{
    return strpbrk (_RWSTD_CONST_CAST (char*, __s1), __s2);
}

// 21.4, p8 of ISO/IEC 14882:1998
inline const char* strrchr (const char *__s, int __c)
{
    return strrchr (_RWSTD_CONST_CAST (char*, __s), __c);
}

// 21.4, p10 of ISO/IEC 14882:1998
inline const char* strstr (const char *__s1, const char *__s2)
{
    return strstr (_RWSTD_CONST_CAST (char*, __s1), __s2);
}


}   // namespace std


#endif   // _RWSTD_CSTRING_H_INCLUDED
